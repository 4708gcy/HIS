#include "Modules/DrugSafety.h"
#include <fstream>
#include <sstream>
#include <algorithm>

namespace {

/// 将药品名按分隔符拆分为成分列表
/// "阿莫西林克拉维酸钾" → ["阿莫西林克拉维酸钾"]（无分隔符，不拆分）
/// "阿莫西林/克拉维酸钾" → ["阿莫西林", "克拉维酸钾"]
std::vector<std::string> tokenizeDrugName(const std::string &drugName)
{
    std::vector<std::string> tokens;
    std::string token;
    for (char c : drugName)
    {
        if (c == ',' || c == '，' || c == '/' || c == ' ' || c == '、' || c == '(' || c == '（')
        {
            if (!token.empty())
            {
                tokens.push_back(token);
                token.clear();
            }
        }
        else if (c == ')' || c == '）')
        {
            // 跳过右括号
        }
        else
        {
            token += c;
        }
    }
    if (!token.empty())
        tokens.push_back(token);

    // 如果没有任何分隔符，整个名称作为一个 token
    if (tokens.empty())
        tokens.push_back(drugName);

    return tokens;
}

/// 检查 target 是否是 candidates 中某个词的精确子成分
/// 只有当 target 作为 candidate 的独立成分出现时才返回 true
/// 例如：target="阿莫西林" 在 candidate 的成分列表 ["阿莫西林", "克拉维酸"] 中 → true
///       target="阿莫西林" 在 candidate 的成分列表 ["阿莫西林克拉维酸钾"] 中 → false（不是独立成分）
bool isConstituentOf(const std::string &target, const std::string &candidate)
{
    auto tokens = tokenizeDrugName(candidate);
    for (const auto &t : tokens)
    {
        if (t == target) return true;
    }
    return false;
}

} // anonymous namespace

std::vector<std::pair<std::string, std::string>> DrugSafety::loadInteractions(const std::string &filePath)
{
    std::vector<std::pair<std::string, std::string>> result;
    std::ifstream file(filePath);
    if (!file.is_open())
        return result;

    std::string line;
    while (std::getline(file, line))
    {
        if (line.empty())
            continue;
        std::istringstream iss(line);
        std::string drug1, drug2;
        if (std::getline(iss, drug1, ',') && std::getline(iss, drug2, ','))
        {
            result.emplace_back(drug1, drug2);
        }
    }
    return result;
}

bool DrugSafety::hasInteraction(const std::string &drug1, const std::string &drug2,
                                const std::vector<std::pair<std::string, std::string>> &interactions)
{
    for (const auto &p : interactions)
    {
        // 1. 精确匹配：药品名完全相同
        if ((p.first == drug1 && p.second == drug2) ||
            (p.first == drug2 && p.second == drug1))
            return true;

        // 2. 成分匹配：药品名通过分隔符拆分为成分后，检查成分列表中是否存在交互对
        auto tokens1 = tokenizeDrugName(drug1);
        auto tokens2 = tokenizeDrugName(drug2);

        // 检查两个药品中是否分别含有交互对的两个成分
        bool d1HasA = false, d1HasB = false;
        for (const auto &t : tokens1)
        {
            if (t == p.first) d1HasA = true;
            if (t == p.second) d1HasB = true;
        }
        bool d2HasA = false, d2HasB = false;
        for (const auto &t : tokens2)
        {
            if (t == p.first) d2HasA = true;
            if (t == p.second) d2HasB = true;
        }

        if ((d1HasA && d2HasB) || (d1HasB && d2HasA))
            return true;

        // 3. 特殊处理：如果两个药品中有一个是复方制剂（>1 个成分），且该复方本身就包含了交互对的两个成分，则不应视为相互作用
        // （复方制剂已经考虑了成分间的相互作用）
        bool singleDrugHasBoth = (d1HasA && d1HasB) || (d2HasA && d2HasB);
        if (singleDrugHasBoth && (tokens1.size() > 1 || tokens2.size() > 1))
            continue;  // 不是真正的相互作用——是单个复方制剂内部成分
    }
    return false;
}

std::vector<std::string> DrugSafety::checkAllergyConflict(
    const std::string &allergyHistory,
    const std::vector<std::string> &prescribedDrugNames)
{
    std::vector<std::string> conflicts;
    if (allergyHistory.empty() || allergyHistory == "#")
        return conflicts;

    // 按常见分隔符切分过敏原
    std::vector<std::string> allergens;
    std::string token;
    for (char c : allergyHistory)
    {
        if (c == ',' || c == '，' || c == ' ' || c == '/' || c == '、')
        {
            if (!token.empty())
            {
                allergens.push_back(token);
                token.clear();
            }
        }
        else
        {
            token += c;
        }
    }
    if (!token.empty())
        allergens.push_back(token);

    for (const auto &allergen : allergens)
    {
        if (allergen.empty()) continue;

        for (const auto &drugName : prescribedDrugNames)
        {
            // 检查药品的成分列表中是否含有该过敏原
            auto drugTokens = tokenizeDrugName(drugName);
            bool found = false;
            for (const auto &dt : drugTokens)
            {
                // 使用成分精确匹配（而非子字符串模糊匹配）
                if (dt == allergen)
                {
                    found = true;
                    break;
                }
            }

            // 回退：如果药物名没有分隔符，用原药物名做精确匹配
            if (!found && drugTokens.size() == 1 && drugTokens[0] == drugName)
            {
                if (drugName == allergen)
                    found = true;
            }

            if (found)
            {
                if (std::find(conflicts.begin(), conflicts.end(), drugName) == conflicts.end())
                    conflicts.push_back(drugName);
            }
        }
    }
    return conflicts;
}

std::vector<std::string> DrugSafety::checkDDI(
    const std::string &newDrug,
    const std::vector<std::string> &existingDrugs,
    const std::vector<std::pair<std::string, std::string>> &interactions)
{
    std::vector<std::string> conflictingDrugs;
    for (const auto &existing : existingDrugs)
    {
        if (hasInteraction(newDrug, existing, interactions))
        {
            conflictingDrugs.push_back(existing);
        }
    }
    return conflictingDrugs;
}
