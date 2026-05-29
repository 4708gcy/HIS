#include "Modules/DataAnalysis.h"
#include "Entities/Examination.h"  // VitalSigns
#include "Entities/Hospitalization.h"
#include "Entities/Registration.h"
#include "Entities/Medicine.h"
#include "Core/UI.h"
#include "Core/GetTime.h"
#include <algorithm>
#include <iomanip>
#include <cmath>
#include <set>

// ===================== 时间工具 =====================

std::string extractYearMonth(const std::string &timestamp)
{
    if (timestamp.size() < 7 || timestamp == "#") return "#";
    return timestamp.substr(0, 7);
}

int daysBetween(const std::string &from, const std::string &to)
{
    if (from == "#" || to == "#") return 0;
    int y1, m1, d1, y2, m2, d2;
    char sep;
    std::istringstream f1(from), f2(to);
    if (!(f1 >> y1 >> sep >> m1 >> sep >> d1)) return 0;
    if (!(f2 >> y2 >> sep >> m2 >> sep >> d2)) return 0;
    auto toJulian = [](int y, int m, int d) {
        if (m <= 2) { y--; m += 12; }
        return 365 * y + y / 4 - y / 100 + y / 400 + 306 * (m + 1) / 10 + d;
    };
    return std::abs(toJulian(y2, m2, d2) - toJulian(y1, m1, d1));
}

// ===================== 月度统计 =====================

std::vector<MonthlyDeptStats> computeMonthlyStats(Hospitalization *hosHead, Registration *regHead)
{
    std::map<std::string, std::map<std::string, MonthlyDeptStats>> data;
    // 科室从实际数据中动态聚合，不再硬编码

    Hospitalization *h = hosHead;
    while (h)
    {
        if (!h->isDeleted)
        {
            std::string admitMonth = extractYearMonth(h->admitTime);
            std::string dischMonth = extractYearMonth(h->dischargeTime);
            if (admitMonth != "#")
            {
                auto &s = data[h->department][admitMonth];
                s.department = h->department;
                s.month = admitMonth;
                s.newAdmissions++;
                s.totalHospitalizationCost += h->totalCost;
            }
            if (dischMonth != "#")
            {
                auto &s = data[h->department][dischMonth];
                s.department = h->department;
                s.month = dischMonth;
                s.discharges++;
                int days = daysBetween(h->admitTime, h->dischargeTime);
                s.totalBedDays += (days > 0 ? days : 1);
            }
        }
        h = h->next;
    }

    Registration *r = regHead;
    while (r)
    {
        if (!r->isDeleted)
        {
            std::string month = extractYearMonth(r->registerTime);
            if (month != "#")
            {
                auto &s = data[r->department][month];
                s.department = r->department;
                s.month = month;
                s.totalRegistrationCount++;
            }
        }
        r = r->next;
    }

    std::vector<MonthlyDeptStats> result;
    for (auto &deptPair : data)
        for (auto &monthPair : deptPair.second)
            result.push_back(monthPair.second);
    std::sort(result.begin(), result.end(), [](const MonthlyDeptStats &a, const MonthlyDeptStats &b) {
        if (a.department != b.department) return a.department < b.department;
        return a.month < b.month;
    });
    return result;
}

// ===================== 需求预测 =====================

std::vector<DemandPrediction> predictDemand(const std::vector<MonthlyDeptStats> &monthlyStats)
{
    std::vector<DemandPrediction> result;

    // 从已有统计数据中提取唯一科室列表
    std::set<std::string> deptSet;
    for (auto &s : monthlyStats)
        if (!s.department.empty() && s.department != "#")
            deptSet.insert(s.department);

    for (const auto &dept : deptSet)
    {
        std::vector<std::pair<std::string, int>> months;
        for (auto &s : monthlyStats)
            if (s.department == dept)
                months.push_back({s.month, s.newAdmissions});
        std::sort(months.begin(), months.end());

        std::vector<int> admissions;
        for (auto &p : months) admissions.push_back(p.second);

        DemandPrediction pred;
        pred.department = dept;

        if (admissions.empty())
        {
            result.push_back(pred);
            continue;
        }

        int n = (int)admissions.size();
        int maCount = std::min(n, 3);
        double sum = 0;
        for (int i = n - maCount; i < n; i++) sum += admissions[i];
        pred.movingAvg3Month = sum / maCount;

        if (n >= 2)
        {
            double meanX = (n - 1) / 2.0;
            double meanY = 0;
            for (int v : admissions) meanY += v;
            meanY /= n;

            double num = 0, den = 0;
            for (int i = 0; i < n; i++)
            {
                num += (i - meanX) * (admissions[i] - meanY);
                den += (i - meanX) * (i - meanX);
            }
            pred.linearTrend = (den != 0) ? num / den : 0;
        }

        if (n >= 2 && admissions[n - 2] > 0)
            pred.growthRate = (admissions[n - 1] - admissions[n - 2]) * 100.0 / admissions[n - 2];

        pred.predictedNextMonth = std::max(0, (int)std::round(pred.movingAvg3Month + pred.linearTrend));

        // Holt-Winters 双指数平滑（无季节性）
        if (n >= 3)
        {
            double alpha = 0.3, beta = 0.1;
            double level = admissions[0];
            double trend = admissions[1] - admissions[0];
            for (int i = 1; i < n; i++)
            {
                double newLevel = alpha * admissions[i] + (1 - alpha) * (level + trend);
                double newTrend = beta * (newLevel - level) + (1 - beta) * trend;
                level = newLevel;
                trend = newTrend;
            }
            pred.holtWintersForecast = std::max(0.0, level + trend);
            // 如果 Holt-Winters 更可靠（数据量够），用它覆盖简单预测
            if (n >= 6)
                pred.predictedNextMonth = std::max(0, (int)std::round(pred.holtWintersForecast));
        }

        result.push_back(pred);
    }
    return result;
}

// ===================== 床位分析 =====================

std::vector<DeptBedAnalysis> analyzeBedAllocation(bedInfo *bedHead, const std::vector<DemandPrediction> &predictions)
{
    std::vector<DeptBedAnalysis> result;

    // 从床位数据中提取唯一科室列表
    std::set<std::string> deptSet;
    {
        bedInfo *tmp = bedHead;
        while (tmp)
        {
            if (!tmp->isDeleted && !tmp->department.empty() && tmp->department != "#")
                deptSet.insert(tmp->department);
            tmp = tmp->next;
        }
    }
    if (deptSet.empty())
        deptSet = {"内科", "外科", "妇产科", "急诊科", "儿科"};

    for (const auto &dept : deptSet)
    {
        DeptBedAnalysis a;
        a.department = dept;
        bedInfo *b = bedHead;
        while (b)
        {
            if (!b->isDeleted && b->department == dept)
            {
                a.totalBeds++;
                a.totalUseTimes += b->useTimes;
                a.totalDaysOccupied += b->daysOccupied;
                if (b->status == bedStatus::OCCUPIED) a.occupiedBeds++;
                else if (b->status == bedStatus::AVAILABLE) a.availableBeds++;
            }
            b = b->next;
        }

        if (a.totalBeds > 0)
            a.utilizationRate = a.occupiedBeds * 100.0 / a.totalBeds;
        if (a.totalUseTimes > 0)
            a.avgStayDays = (double)a.totalDaysOccupied / a.totalUseTimes;

        for (auto &p : predictions)
            if (p.department == dept) a.predictedDemand = p.predictedNextMonth;

        if (a.predictedDemand > 0 && a.avgStayDays > 0)
            a.recommendedBeds = std::max(1, (int)std::ceil(a.predictedDemand * a.avgStayDays / 30.0 * 1.2));
        else
            a.recommendedBeds = a.totalBeds;

        result.push_back(a);
    }
    return result;
}

// ===================== 床位调拨建议 =====================

std::vector<BedReallocationSuggestion> suggestBedReallocation(const std::vector<DeptBedAnalysis> &analyses)
{
    std::vector<BedReallocationSuggestion> result;
    std::vector<const DeptBedAnalysis *> highDemand, lowDemand;

    for (auto &a : analyses)
    {
        if (a.utilizationRate > 85 && a.recommendedBeds > a.totalBeds)
            highDemand.push_back(&a);
        else if (a.utilizationRate < 50 && a.totalBeds > a.recommendedBeds)
            lowDemand.push_back(&a);
    }

    for (auto *hd : highDemand)
    {
        int needed = hd->recommendedBeds - hd->totalBeds;
        for (auto *ld : lowDemand)
        {
            if (needed <= 0) break;
            int available = ld->totalBeds - ld->recommendedBeds;
            int moveCount = std::min(needed, available);
            if (moveCount > 0)
            {
                BedReallocationSuggestion s;
                s.action = "调入";
                s.fromDept = ld->department;
                s.toDept = hd->department;
                s.bedCount = moveCount;
                char buf[200];
                snprintf(buf, sizeof(buf), "%s使用率%.1f%%需求大，%s使用率%.1f%%可调剂",
                         hd->department.c_str(), hd->utilizationRate, ld->department.c_str(), ld->utilizationRate);
                s.rationale = buf;
                result.push_back(s);
                needed -= moveCount;
            }
        }
    }
    return result;
}

// ===================== 药品统计 =====================

std::vector<DeptMedicineStats> analyzeMedicineStats(Medicine *medHead)
{
    std::map<std::string, DeptMedicineStats> byDept;
    DeptMedicineStats allStats;
    allStats.department = "全院";

    Medicine *m = medHead;
    while (m)
    {
        if (!m->isDeleted)
        {
            int purchaseVal = m->stock * m->purchasePrice;
            int saleVal = m->stock * m->salePrice;
            auto &s = byDept[m->department];
            s.department = m->department;
            s.totalMedicineTypes++;
            s.totalStock += m->stock;
            s.totalPurchaseValue += purchaseVal;
            s.totalSaleValue += saleVal;
            allStats.totalMedicineTypes++;
            allStats.totalStock += m->stock;
            allStats.totalPurchaseValue += purchaseVal;
            allStats.totalSaleValue += saleVal;
        }
        m = m->next;
    }

    for (auto &pair : byDept)
        if (pair.second.totalPurchaseValue > 0)
            pair.second.profitMargin = (pair.second.totalSaleValue - pair.second.totalPurchaseValue) / pair.second.totalPurchaseValue * 100;
    if (allStats.totalPurchaseValue > 0)
        allStats.profitMargin = (allStats.totalSaleValue - allStats.totalPurchaseValue) / allStats.totalPurchaseValue * 100;

    std::vector<DeptMedicineStats> result;
    for (auto &pair : byDept)
        result.push_back(pair.second);
    std::sort(result.begin(), result.end(), [](const DeptMedicineStats &a, const DeptMedicineStats &b)
              { return a.department < b.department; });
    result.push_back(allStats);
    return result;
}

// ===================== 关键发现 =====================

std::vector<std::string> generateKeyFindings(
    const std::vector<DemandPrediction> &predictions,
    const std::vector<DeptBedAnalysis> &bedAnalyses,
    const std::vector<BedReallocationSuggestion> &suggestions)
{
    std::vector<std::string> findings;

    for (auto &p : predictions)
    {
        if (p.predictedNextMonth > 0)
        {
            char buf[200];
            snprintf(buf, sizeof(buf), "%s预测下月入院%d人次（近3月均值%.1f，趋势%.1f/月）",
                     p.department.c_str(), p.predictedNextMonth, p.movingAvg3Month, p.linearTrend);
            findings.push_back(buf);
        }
    }

    for (auto &a : bedAnalyses)
    {
        if (a.utilizationRate > 85)
        {
            char buf[200];
            snprintf(buf, sizeof(buf), "%s床位使用率高达%.1f%%，建议增加%d张床位",
                     a.department.c_str(), a.utilizationRate, std::max(0, a.recommendedBeds - a.totalBeds));
            findings.push_back(buf);
        }
        else if (a.utilizationRate < 50 && a.totalBeds > 0)
        {
            char buf[200];
            snprintf(buf, sizeof(buf), "%s床位使用率仅%.1f%%，存在资源闲置",
                     a.department.c_str(), a.utilizationRate);
            findings.push_back(buf);
        }
    }

    for (auto &s : suggestions)
    {
        char buf[200];
        snprintf(buf, sizeof(buf), "建议从%s调出%d张床位至%s（%s）",
                 s.fromDept.c_str(), s.bedCount, s.toDept.c_str(), s.rationale.c_str());
        findings.push_back(buf);
    }

    if (findings.empty())
        findings.push_back("当前数据不足以生成分析建议，请录入更多历史数据。");

    return findings;
}

// ===================== Z-score 异常检测 =====================

std::vector<std::string> detectAnomalies(const std::vector<MonthlyDeptStats> &monthlyStats, double zThreshold)
{
    std::vector<std::string> anomalies;

    // 按科室分组收集入院人数
    std::map<std::string, std::vector<std::pair<std::string, int>>> deptData;
    for (auto &s : monthlyStats)
        deptData[s.department].push_back({s.month, s.newAdmissions});

    for (auto &[dept, data] : deptData)
    {
        if (data.size() < 3) continue;

        std::sort(data.begin(), data.end());
        std::vector<int> values;
        for (auto &d : data) values.push_back(d.second);

        // 计算均值和标准差
        double mean = 0;
        for (int v : values) mean += v;
        mean /= values.size();

        double variance = 0;
        for (int v : values) variance += (v - mean) * (v - mean);
        variance /= values.size();
        double stddev = std::sqrt(variance);

        if (stddev < 0.001) continue; // 无波动则跳过

        // 检查每个月份的 Z-score
        for (size_t i = 0; i < data.size(); i++)
        {
            double z = (values[i] - mean) / stddev;
            if (std::abs(z) > zThreshold)
            {
                char buf[200];
                snprintf(buf, sizeof(buf), "[%s] %s 入院%d人次异常（Z=%.2f，均值%.1f±%.1f）%s",
                         dept.c_str(), data[i].first.c_str(), values[i], z, mean, stddev,
                         z > 0 ? "——显著偏高" : "——显著偏低");
                anomalies.push_back(buf);
            }
        }
    }

    if (anomalies.empty())
        anomalies.push_back("所有科室入院数据正常，未检测到异常波动。");

    return anomalies;
}

// ===================== 总分析管线 =====================

DataAnalysisReport runFullAnalysis(
    Hospitalization *hosHead, bedInfo *bedHead,
    Registration *regHead, Medicine *medHead)
{
    DataAnalysisReport report;
    report.generatedAt = MyTime::getInstance().getTime();

    report.monthlyStats = computeMonthlyStats(hosHead, regHead);
    report.predictions = predictDemand(report.monthlyStats);
    report.bedAnalyses = analyzeBedAllocation(bedHead, report.predictions);
    report.reallocationSuggestions = suggestBedReallocation(report.bedAnalyses);
    report.medicineStats = analyzeMedicineStats(medHead);
    report.anomalies = detectAnomalies(report.monthlyStats);
    report.keyFindings = generateKeyFindings(report.predictions, report.bedAnalyses, report.reallocationSuggestions);

    return report;
}

// ===================== ASCII 条形图辅助 =====================

static void printAsciiBar(const std::string &label, double value, double maxValue, int barWidth = 25)
{
    int filled = (maxValue > 0) ? (int)(value / maxValue * barWidth) : 0;
    int empty = barWidth - filled;
    std::cout << "  " << std::left << std::setw(8) << label << " |";
    for (int i = 0; i < filled; i++) std::cout << "#";
    for (int i = 0; i < empty; i++) std::cout << ".";
    std::cout << "| " << std::fixed << std::setprecision(1) << value;
    std::cout << std::endl;
}

// ===================== 展示：详细报表 =====================

void displayTabularReport(const DataAnalysisReport &report)
{
    printTitle("数据分析详细报表");
    std::cout << "生成时间: " << report.generatedAt << std::endl << std::endl;

    std::cout << "【月度科室统计】" << std::endl;
    std::cout << std::left << std::setw(8) << "科室" << std::setw(8) << "月份"
              << std::setw(8) << "入院" << std::setw(8) << "出院"
              << std::setw(8) << "挂号" << std::setw(12) << "住院费用"
              << std::setw(8) << "床位天数" << std::endl;
    std::cout << std::string(70, '-') << std::endl;
    for (auto &s : report.monthlyStats)
    {
        std::cout << std::left << std::setw(8) << s.department << std::setw(8) << s.month
                  << std::setw(8) << s.newAdmissions << std::setw(8) << s.discharges
                  << std::setw(8) << s.totalRegistrationCount
                  << std::setw(12) << std::fixed << std::setprecision(2) << (s.totalHospitalizationCost / 100.0)
                  << std::setw(8) << s.totalBedDays << std::endl;
    }

    std::cout << "\n【下月需求预测】" << std::endl;
    std::cout << std::left << std::setw(8) << "科室" << std::setw(12) << "近3月均值"
              << std::setw(10) << "线性趋势" << std::setw(10) << "增长率"
              << std::setw(12) << "预测入院" << std::endl;
    std::cout << std::string(55, '-') << std::endl;
    for (auto &p : report.predictions)
    {
        std::cout << std::left << std::setw(8) << p.department
                  << std::setw(12) << std::fixed << std::setprecision(1) << p.movingAvg3Month
                  << std::setw(10) << std::setprecision(1) << p.linearTrend
                  << std::setw(10) << std::setprecision(1) << p.growthRate << "%"
                  << std::setw(12) << p.predictedNextMonth << std::endl;
    }

    std::cout << "\n【床位分配分析】" << std::endl;
    std::cout << std::left << std::setw(8) << "科室" << std::setw(8) << "总床位"
              << std::setw(8) << "已占用" << std::setw(8) << "使用率"
              << std::setw(10) << "平均住院" << std::setw(10) << "预测需求"
              << std::setw(10) << "建议床位" << std::endl;
    std::cout << std::string(65, '-') << std::endl;
    for (auto &a : report.bedAnalyses)
    {
        std::cout << std::left << std::setw(8) << a.department
                  << std::setw(8) << a.totalBeds
                  << std::setw(8) << a.occupiedBeds
                  << std::setw(8) << std::fixed << std::setprecision(1) << a.utilizationRate << "%"
                  << std::setw(10) << std::setprecision(1) << a.avgStayDays << "天"
                  << std::setw(10) << a.predictedDemand
                  << std::setw(10) << a.recommendedBeds << std::endl;
    }

    std::cout << "\n【床位调拨建议】" << std::endl;
    if (report.reallocationSuggestions.empty())
        std::cout << "  当前床位分配合理，无需调整。" << std::endl;
    else
    {
        for (auto &s : report.reallocationSuggestions)
            std::cout << "  " << s.action << ": 从 " << s.fromDept << " 调出 " << s.bedCount
                      << " 张 -> " << s.toDept << " （" << s.rationale << "）" << std::endl;
    }

    std::cout << "\n【药品库存分析】" << std::endl;
    std::cout << std::left << std::setw(8) << "科室" << std::setw(8) << "种类"
              << std::setw(8) << "库存" << std::setw(14) << "采购价值"
              << std::setw(14) << "销售价值" << std::setw(10) << "利润率" << std::endl;
    std::cout << std::string(65, '-') << std::endl;
    for (auto &s : report.medicineStats)
    {
        std::cout << std::left << std::setw(8) << s.department
                  << std::setw(8) << s.totalMedicineTypes
                  << std::setw(8) << s.totalStock
                  << std::setw(14) << std::fixed << std::setprecision(2) << (s.totalPurchaseValue / 100.0)
                  << std::setw(14) << std::setprecision(2) << (s.totalSaleValue / 100.0)
                  << std::setw(10) << std::setprecision(1) << s.profitMargin << "%" << std::endl;
    }
}

// ===================== 展示：ASCII 图表 =====================

void displayAsciiChart(const DataAnalysisReport &report)
{
    printTitle("数据分析可视化");

    std::cout << "【床位使用率对比】" << std::endl;
    double maxRate = 0;
    for (auto &a : report.bedAnalyses)
        if (a.utilizationRate > maxRate) maxRate = a.utilizationRate;
    for (auto &a : report.bedAnalyses)
        printAsciiBar(a.department, a.utilizationRate, maxRate > 0 ? maxRate : 100, 20);

    std::cout << "\n【预测需求 vs 建议床位】" << std::endl;
    double maxBeds = 0;
    for (auto &a : report.bedAnalyses)
        if (a.recommendedBeds > maxBeds) maxBeds = a.recommendedBeds;
    if (maxBeds == 0) maxBeds = 1;
    for (auto &a : report.bedAnalyses)
    {
        std::cout << "  " << std::left << std::setw(8) << a.department << " 当前:";
        printAsciiBar("", (double)a.totalBeds, maxBeds, 10);
        std::cout << "  " << std::left << std::setw(8) << "" << " 建议:";
        printAsciiBar("", (double)a.recommendedBeds, maxBeds, 10);
        std::cout << std::endl;
    }

    std::cout << "\n【各科室入院趋势（最近数月）】" << std::endl;
    double maxPred = 0;
    for (auto &pp : report.predictions)
        if (pp.predictedNextMonth > maxPred) maxPred = pp.predictedNextMonth;
    if (maxPred == 0) maxPred = 1;
    for (auto &p : report.predictions)
    {
        std::cout << "  " << std::left << std::setw(8) << p.department << " 预测:";
        printAsciiBar("", (double)p.predictedNextMonth, maxPred, 20);
    }

    std::cout << "\n【各科室药品利润率】" << std::endl;
    double maxMargin = 0;
    for (auto &s : report.medicineStats)
        if (s.profitMargin > maxMargin) maxMargin = s.profitMargin;
    if (maxMargin == 0) maxMargin = 1;
    for (auto &s : report.medicineStats)
        printAsciiBar(s.department, s.profitMargin, maxMargin, 20);
}

// ===================== 展示：摘要报告 =====================

void displaySummaryReport(const DataAnalysisReport &report)
{
    printTitle("数据分析摘要与建议");
    std::cout << "生成时间: " << report.generatedAt << std::endl << std::endl;

    std::cout << "【关键发现】" << std::endl;
    for (size_t i = 0; i < report.keyFindings.size(); i++)
        std::cout << "  " << (i + 1) << ". " << report.keyFindings[i] << std::endl;

    int totalBeds = 0, totalOccupied = 0;
    for (auto &a : report.bedAnalyses)
    {
        totalBeds += a.totalBeds;
        totalOccupied += a.occupiedBeds;
    }
    std::cout << "\n【全院汇总】" << std::endl;
    std::cout << "  总床位: " << totalBeds << " | 总使用率: "
              << std::fixed << std::setprecision(1)
              << (totalBeds > 0 ? totalOccupied * 100.0 / totalBeds : 0) << "%" << std::endl;

    int totalPredicted = 0;
    for (auto &p : report.predictions) totalPredicted += p.predictedNextMonth;
    std::cout << "  预测下月总入院: " << totalPredicted << " 人次" << std::endl;

    if (!report.reallocationSuggestions.empty())
    {
        std::cout << "\n【床位调整方案】" << std::endl;
        for (auto &s : report.reallocationSuggestions)
            std::cout << "  从 " << s.fromDept << " 调出 " << s.bedCount
                      << " 张 -> " << s.toDept << std::endl;
    }

    if (!report.anomalies.empty())
    {
        std::cout << "\n【异常检测（Z-score）】" << std::endl;
        for (auto &a : report.anomalies)
            std::cout << "  " << a << std::endl;
    }
}
