#include "Modules/DrugSafety.h"
#include <fstream>
#include <sstream>
#include <algorithm>

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
        if ((p.first == drug1 && p.second == drug2) ||
            (p.first == drug2 && p.second == drug1))
            return true;
        // 子串匹配：药品名可能包含相互作用药品名作为成分
        if ((drug1.find(p.first) != std::string::npos && drug2.find(p.second) != std::string::npos) ||
            (drug1.find(p.second) != std::string::npos && drug2.find(p.first) != std::string::npos))
            return true;
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
        { // 逗号、中文逗号、空格、斜杠、顿号
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
        for (const auto &drugName : prescribedDrugNames)
        {
            // 子串匹配：过敏原可能是药品名的子串（如 "青霉素" 匹配 "阿莫西林胶囊" 不太准，但匹配 "青霉素V钾片" 可以）
            if (drugName.find(allergen) != std::string::npos ||
                allergen.find(drugName) != std::string::npos)
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
