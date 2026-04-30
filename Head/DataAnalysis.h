#ifndef DATAANALYSIS_H
#define DATAANALYSIS_H

#include <string>
#include <vector>
#include <map>

// 前向声明
struct Hospitalization;
struct bedInfo;
struct Registration;
struct Medicine;
struct MedicineFlow;

// ===================== 分析结果结构体 =====================

struct MonthlyDeptStats
{
    std::string department;
    std::string month;          // "YYYY-MM"
    int newAdmissions = 0;      // 入院人数
    int discharges = 0;         // 出院人数
    int totalRegistrationCount = 0;
    double totalHospitalizationCost = 0.0;
    int totalBedDays = 0;
};

struct DemandPrediction
{
    std::string department;
    double movingAvg3Month = 0.0;
    double linearTrend = 0.0;
    double growthRate = 0.0;
    int predictedNextMonth = 0;
};

struct DeptBedAnalysis
{
    std::string department;
    int totalBeds = 0;
    int occupiedBeds = 0;
    int availableBeds = 0;
    double utilizationRate = 0.0;
    int totalUseTimes = 0;
    int totalDaysOccupied = 0;
    double avgStayDays = 0.0;
    int predictedDemand = 0;
    int recommendedBeds = 0;
};

struct BedReallocationSuggestion
{
    std::string action;         // "调入" / "调出"
    std::string fromDept;
    std::string toDept;
    int bedCount = 0;
    std::string rationale;
};

struct DeptMedicineStats
{
    std::string department;
    int totalMedicineTypes = 0;
    int totalStock = 0;
    double totalPurchaseValue = 0.0;
    double totalSaleValue = 0.0;
    double profitMargin = 0.0;
};

struct DataAnalysisReport
{
    std::string generatedAt;
    std::vector<MonthlyDeptStats> monthlyStats;
    std::vector<DemandPrediction> predictions;
    std::vector<DeptBedAnalysis> bedAnalyses;
    std::vector<BedReallocationSuggestion> reallocationSuggestions;
    std::vector<DeptMedicineStats> medicineStats;
    std::vector<std::string> keyFindings;
};

// ===================== 计算函数 =====================

std::string extractYearMonth(const std::string &timestamp);
int daysBetween(const std::string &from, const std::string &to);

std::vector<MonthlyDeptStats> computeMonthlyStats(Hospitalization *hosHead, Registration *regHead);
std::vector<DemandPrediction> predictDemand(const std::vector<MonthlyDeptStats> &monthlyStats);
std::vector<DeptBedAnalysis> analyzeBedAllocation(bedInfo *bedHead, const std::vector<DemandPrediction> &predictions);
std::vector<BedReallocationSuggestion> suggestBedReallocation(const std::vector<DeptBedAnalysis> &analyses);
std::vector<DeptMedicineStats> analyzeMedicineStats(Medicine *medHead);
std::vector<std::string> generateKeyFindings(
    const std::vector<DemandPrediction> &predictions,
    const std::vector<DeptBedAnalysis> &bedAnalyses,
    const std::vector<BedReallocationSuggestion> &suggestions);

DataAnalysisReport runFullAnalysis(
    Hospitalization *hosHead, bedInfo *bedHead,
    Registration *regHead, Medicine *medHead);

// ===================== 展示函数 =====================

void displayTabularReport(const DataAnalysisReport &report);
void displayAsciiChart(const DataAnalysisReport &report);
void displaySummaryReport(const DataAnalysisReport &report);

#endif // DATAANALYSIS_H
