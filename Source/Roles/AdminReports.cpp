/**
 * @file AdminReports.cpp
 * @brief 管理员统计报表功能实现
 * @details 包含科室报表、医生工作量、患者统计、床位利用率、药品库存、数据分析等6个报表
 */

#include "Roles/Admin.h"
#include "Core/UI.h"
#include "Core/Database.h"
#include "Core/User.h"
#include "Entities/Registration.h"
#include "Entities/Consultation.h"
#include "Entities/Examination.h"
#include "Entities/Hospitalization.h"
#include "Entities/MedicationRecord.h"
#include "Entities/Medicine.h"
#include "Modules/DataAnalysis.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <set>
#include <map>
#include <vector>
// ======================================== 统计报表功能 ========================================

void Admin::showDepartmentReport(Doctor *&docHead, Registration *&regHead, Consultation *&conHead)
{
    // 时间范围选择
    int timeChoice = timeRangeMenu();
    if (timeChoice == 0) return;
    std::string startTime, endTime;
    if (timeChoice == 1) { startTime = getCurrentMonthStart(); }
    else if (timeChoice == 2) { startTime = getMonthsAgoStart(3); }
    else if (timeChoice == 3) { startTime = getMonthsAgoStart(6); }
    else if (timeChoice == 4)
    {
        startTime = inputDateCheck("请输入起始日期 (YYYY-MM-DD): ") + " 00:00:00";
        endTime = inputDateCheck("请输入结束日期 (YYYY-MM-DD): ") + " 23:59:59";
    }

    printTitle("科室统计总览");
    if (timeChoice != 5)
        std::cout << "时间范围: " << startTime << " ~ " << (endTime.empty() ? "至今" : endTime) << std::endl << std::endl;

    Database &db = GetDB();
    if (db.isConnected())
    {
        // SQL path: aggregate via database queries
        std::string endBound = endTime.empty() ? "9999-12-31 23:59:59" : endTime;

        // Get per-department stats via a single JOIN query
        std::string sql =
            "SELECT d.department, "
            "  (SELECT COUNT(*) FROM doctors WHERE department = d.department AND is_deleted = 0) as doctor_count, "
            "  COUNT(r.registration_id) as reg_count, "
            "  COALESCE(SUM(r.fee_cents), 0) as total_fee "
            "FROM registrations r "
            "JOIN doctors d ON d.department = r.department AND d.is_deleted = 0 "
            "WHERE r.is_deleted = 0 AND r.register_time BETWEEN " + Database::quote(startTime) +
            " AND " + Database::quote(endBound) +
            " GROUP BY d.department";
        MYSQL_RES *res = db.query(sql);
        if (res)
        {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)))
            {
                std::string dept = Database::readString(row, 0);
                int doctorCount = Database::readInt(row, 1);
                int regCount = Database::readInt(row, 2);
                long long totalFee = Database::readInt(row, 3);

                // Get consultation count for this department
                std::string conSql =
                    "SELECT COUNT(*) FROM consultations WHERE is_deleted = 0 AND department = " +
                    Database::quote(dept) + " AND consultation_time BETWEEN " + Database::quote(startTime) +
                    " AND " + Database::quote(endBound);
                MYSQL_RES *conRes = db.query(conSql);
                int conCount = 0;
                if (conRes)
                {
                    MYSQL_ROW conRow = mysql_fetch_row(conRes);
                    if (conRow) conCount = Database::readInt(conRow, 0);
                    mysql_free_result(conRes);
                }

                std::cout << "【" << dept << "】 医生: " << doctorCount << "人 | 挂号: " << regCount
                          << "次 | 看诊: " << conCount << "次 | 挂号收入: " << std::fixed << std::setprecision(2) << (totalFee / 100.0) << "元" << std::endl;
            }
            mysql_free_result(res);
        }

        // Also find departments that exist in doctors but had no registrations in the period
        std::string extraSql =
            "SELECT DISTINCT d.department FROM doctors d "
            "WHERE d.is_deleted = 0 AND d.department IS NOT NULL AND d.department != '#' "
            "AND d.department NOT IN ("
            "  SELECT DISTINCT r.department FROM registrations r "
            "  WHERE r.is_deleted = 0 AND r.department IS NOT NULL AND r.department != '#'"
            ")";
        MYSQL_RES *extraRes = db.query(extraSql);
        if (extraRes)
        {
            MYSQL_ROW eRow;
            while ((eRow = mysql_fetch_row(extraRes)))
            {
                std::string dept = Database::readString(eRow, 0);
                // Get doctor count
                std::string docSql = "SELECT COUNT(*) FROM doctors WHERE is_deleted = 0 AND department = " + Database::quote(dept);
                MYSQL_RES *docRes = db.query(docSql);
                int docCount = 0;
                if (docRes) { MYSQL_ROW dr = mysql_fetch_row(docRes); if (dr) docCount = Database::readInt(dr, 0); mysql_free_result(docRes); }
                std::cout << "【" << dept << "】 医生: " << docCount << "人 | 挂号: 0次 | 看诊: 0次 | 挂号收入: 0.00元" << std::endl;
            }
            mysql_free_result(extraRes);
        }
    }
    else
    {
        // Fallback: linked-list traversal
        std::set<std::string> deptSet;
        {
            Doctor *d = docHead;
            while (d) { if (!d->isDeleted && !d->department.empty() && d->department != "#") deptSet.insert(d->department); d = d->next; }
            Registration *r = regHead;
            while (r) { if (!r->isDeleted && !r->department.empty() && r->department != "#") deptSet.insert(r->department); r = r->next; }
            Consultation *c = conHead;
            while (c) { if (!c->isDeleted && !c->department.empty() && c->department != "#") deptSet.insert(c->department); c = c->next; }
        }
        if (deptSet.empty())
            deptSet = {"内科", "外科", "妇产科", "急诊科", "儿科"};

        for (const auto &dept : deptSet)
        {
            int doctorCount = 0, regCount = 0, conCount = 0;
            long long totalFee = 0;

            Doctor *d = docHead;
            while (d) { if (!d->isDeleted && d->department == dept) doctorCount++; d = d->next; }

            Registration *r = regHead;
            while (r)
            {
                if (!r->isDeleted && r->department == dept && isTimestampInRange(r->registerTime, startTime, endTime))
                {
                    regCount++;
                    totalFee += r->fee;
                }
                r = r->next;
            }

            Consultation *c = conHead;
            while (c)
            {
                if (!c->isDeleted && c->department == dept && isTimestampInRange(c->consultationTime, startTime, endTime))
                    conCount++;
                c = c->next;
            }

            std::cout << "【" << dept << "】 医生: " << doctorCount << "人 | 挂号: " << regCount
                      << "次 | 看诊: " << conCount << "次 | 挂号收入: " << std::fixed << std::setprecision(2) << (totalFee / 100.0) << "元" << std::endl;
        }
    }
    pause("统计报表 > 科室统计");
}

void Admin::showDoctorWorkloadReport(Doctor *&docHead, Consultation *&conHead, Examination *&examHead, Hospitalization *&hosHead)
{
    // 时间范围选择
    int timeChoice = timeRangeMenu();
    if (timeChoice == 0) return;
    std::string startTime, endTime;
    if (timeChoice == 1) { startTime = getCurrentMonthStart(); }
    else if (timeChoice == 2) { startTime = getMonthsAgoStart(3); }
    else if (timeChoice == 3) { startTime = getMonthsAgoStart(6); }
    else if (timeChoice == 4)
    {
        startTime = inputDateCheck("请输入起始日期 (YYYY-MM-DD): ") + " 00:00:00";
        endTime = inputDateCheck("请输入结束日期 (YYYY-MM-DD): ") + " 23:59:59";
    }

    // 按医生ID聚合统计
    struct DocStat
    {
        std::string name, dept, title;
        int consultations = 0, examinations = 0, hospitalizations = 0;
    };
    std::map<std::string, DocStat> stats;

    Database &db = GetDB();
    if (db.isConnected())
    {
        // SQL path: use JOIN queries
        std::string endBound = endTime.empty() ? "9999-12-31 23:59:59" : endTime;

        std::string sql =
            "SELECT d.doctor_id, d.username, d.department, d.title, "
            "  COALESCE(con.consultations, 0), "
            "  COALESCE(exa.examinations, 0), "
            "  COALESCE(hos.hospitalizations, 0) "
            "FROM doctors d "
            "LEFT JOIN (SELECT doctor_id, COUNT(*) as consultations FROM consultations WHERE is_deleted=0 AND consultation_time BETWEEN " +
            Database::quote(startTime) + " AND " + Database::quote(endBound) +
            " GROUP BY doctor_id) con ON d.doctor_id = con.doctor_id "
            "LEFT JOIN (SELECT doctor_id, COUNT(*) as examinations FROM examinations WHERE is_deleted=0 AND order_time BETWEEN " +
            Database::quote(startTime) + " AND " + Database::quote(endBound) +
            " GROUP BY doctor_id) exa ON d.doctor_id = exa.doctor_id "
            "LEFT JOIN (SELECT doctor_id, COUNT(*) as hospitalizations FROM hospitalizations WHERE is_deleted=0 AND apply_time BETWEEN " +
            Database::quote(startTime) + " AND " + Database::quote(endBound) +
            " GROUP BY doctor_id) hos ON d.doctor_id = hos.doctor_id "
            "WHERE d.is_deleted = 0 "
            "ORDER BY d.doctor_id";

        MYSQL_RES *res = db.query(sql);
        if (res)
        {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)))
            {
                std::string id = Database::readString(row, 0);
                DocStat &s = stats[id];
                s.name = Database::readString(row, 1);
                s.dept = Database::readString(row, 2);
                int titleInt = Database::readInt(row, 3);
                s.title = doctorTitleToString(static_cast<DoctorTitle>(titleInt));
                s.consultations = Database::readInt(row, 4);
                s.examinations = Database::readInt(row, 5);
                s.hospitalizations = Database::readInt(row, 6);
            }
            mysql_free_result(res);
        }
    }
    else
    {
        // Fallback: linked-list traversal
        Doctor *d = docHead;
        while (d)
        {
            if (!d->isDeleted)
            {
                DocStat &s = stats[d->getUserID()];
                s.name = d->getUsername();
                s.dept = d->department;
                s.title = doctorTitleToString(d->title);
            }
            d = d->next;
        }

        Consultation *con = conHead;
        while (con)
        {
            if (!con->isDeleted && con->doctorID != "#" && isTimestampInRange(con->consultationTime, startTime, endTime))
            {
                auto it = stats.find(con->doctorID);
                if (it != stats.end()) it->second.consultations++;
            }
            con = con->next;
        }

        Examination *exa = examHead;
        while (exa)
        {
            if (!exa->isDeleted && exa->doctorID != "#" && isTimestampInRange(exa->orderTime, startTime, endTime))
            {
                auto it = stats.find(exa->doctorID);
                if (it != stats.end()) it->second.examinations++;
            }
            exa = exa->next;
        }

        Hospitalization *hos = hosHead;
        while (hos)
        {
            if (!hos->isDeleted && hos->doctorID != "#" && isTimestampInRange(hos->applyTime, startTime, endTime))
            {
                auto it = stats.find(hos->doctorID);
                if (it != stats.end()) it->second.hospitalizations++;
            }
            hos = hos->next;
        }
    }

    // 输出表格（使用 getDisplayWidth 处理 CJK 对齐）
    auto pad = [](const std::string &s, int width) -> std::string
    {
        int dw = getDisplayWidth(s);
        int spaces = width - dw;
        return s + std::string(spaces > 0 ? spaces : 0, ' ');
    };

    const int W_ID = 8, W_NAME = 10, W_DEPT = 8, W_TITLE = 14, W_NUM = 6, W_HOS = 6;
    const std::string SEP = "-+-";
    auto dash = [](int w) -> std::string { return std::string(w, '-'); };

    printTitle("医生工作量统计");
    if (timeChoice != 5)
        std::cout << "时间范围: " << startTime << " ~ " << (endTime.empty() ? "至今" : endTime) << std::endl
                  << std::endl;

    std::cout << pad("ID", W_ID) << " | "
              << pad("姓名", W_NAME) << " | "
              << pad("科室", W_DEPT) << " | "
              << pad("职称", W_TITLE) << " | "
              << pad("看诊数", W_NUM) << " | "
              << pad("检查数", W_NUM) << " | "
              << "住院证" << std::endl;
    std::cout << dash(W_ID) << SEP << dash(W_NAME) << SEP << dash(W_DEPT) << SEP
              << dash(W_TITLE) << SEP << dash(W_NUM) << SEP << dash(W_NUM) << SEP
              << dash(W_HOS) << std::endl;

    int totalCon = 0, totalExa = 0, totalHos = 0;
    for (auto &kv : stats)
    {
        DocStat &s = kv.second;
        std::cout << pad(kv.first, W_ID) << " | "
                  << pad(s.name, W_NAME) << " | "
                  << pad(s.dept, W_DEPT) << " | "
                  << pad(s.title, W_TITLE) << " | "
                  << pad(std::to_string(s.consultations), W_NUM) << " | "
                  << pad(std::to_string(s.examinations), W_NUM) << " | "
                  << s.hospitalizations << std::endl;
        totalCon += s.consultations;
        totalExa += s.examinations;
        totalHos += s.hospitalizations;
    }

    std::cout << dash(W_ID) << SEP << dash(W_NAME) << SEP << dash(W_DEPT) << SEP
              << dash(W_TITLE) << SEP << dash(W_NUM) << SEP << dash(W_NUM) << SEP
              << dash(W_HOS) << std::endl;
    std::cout << pad("合计", W_ID) << " | "
              << pad("", W_NAME) << " | "
              << pad("", W_DEPT) << " | "
              << pad("", W_TITLE) << " | "
              << pad(std::to_string(totalCon), W_NUM) << " | "
              << pad(std::to_string(totalExa), W_NUM) << " | "
              << totalHos << std::endl;

    pause("统计报表 > 医生工作量");
}

void Admin::showPatientReport(Patient *&patientHead, Registration *&regHead, Consultation *&conHead)
{
    // 时间范围选择
    int timeChoice = timeRangeMenu();
    if (timeChoice == 0) return;
    std::string startTime, endTime;
    if (timeChoice == 1) { startTime = getCurrentMonthStart(); }
    else if (timeChoice == 2) { startTime = getMonthsAgoStart(3); }
    else if (timeChoice == 3) { startTime = getMonthsAgoStart(6); }
    else if (timeChoice == 4)
    {
        startTime = inputDateCheck("请输入起始日期 (YYYY-MM-DD): ") + " 00:00:00";
        endTime = inputDateCheck("请输入结束日期 (YYYY-MM-DD): ") + " 23:59:59";
    }

    printTitle("患者就诊统计");
    if (timeChoice != 5)
        std::cout << "时间范围: " << startTime << " ~ " << (endTime.empty() ? "至今" : endTime) << std::endl << std::endl;

    int totalPatients = 0, hospitalized = 0;
    long long totalBalance = 0;
    int newRegistrations = 0, newConsultations = 0;
    long long regRevenue = 0;

    Database &db = GetDB();
    if (db.isConnected())
    {
        // SQL path: aggregate via database queries
        std::string endBound = endTime.empty() ? "9999-12-31 23:59:59" : endTime;

        // Patient overview
        std::string pSql =
            "SELECT COUNT(*), "
            "  SUM(CASE WHEN is_hospitalized = 1 THEN 1 ELSE 0 END), "
            "  COALESCE(SUM(balance_cents), 0) "
            "FROM patients WHERE is_deleted = 0";
        MYSQL_RES *pRes = db.query(pSql);
        if (pRes)
        {
            MYSQL_ROW row = mysql_fetch_row(pRes);
            if (row)
            {
                totalPatients = Database::readInt(row, 0);
                hospitalized = Database::readInt(row, 1);
                totalBalance = Database::readInt(row, 2);
            }
            mysql_free_result(pRes);
        }

        // Registration stats in time range
        std::string rSql =
            "SELECT COUNT(*), COALESCE(SUM(fee_cents), 0) "
            "FROM registrations WHERE is_deleted = 0 AND register_time BETWEEN " +
            Database::quote(startTime) + " AND " + Database::quote(endBound);
        MYSQL_RES *rRes = db.query(rSql);
        if (rRes)
        {
            MYSQL_ROW row = mysql_fetch_row(rRes);
            if (row)
            {
                newRegistrations = Database::readInt(row, 0);
                regRevenue = Database::readInt(row, 1);
            }
            mysql_free_result(rRes);
        }

        // Consultation stats in time range
        std::string cSql =
            "SELECT COUNT(*) "
            "FROM consultations WHERE is_deleted = 0 AND consultation_time BETWEEN " +
            Database::quote(startTime) + " AND " + Database::quote(endBound);
        MYSQL_RES *cRes = db.query(cSql);
        if (cRes)
        {
            MYSQL_ROW row = mysql_fetch_row(cRes);
            if (row) newConsultations = Database::readInt(row, 0);
            mysql_free_result(cRes);
        }
    }
    else
    {
        // Fallback: linked-list traversal
        Patient *current = patientHead;
        while (current)
        {
            if (!current->isDeleted)
            {
                totalPatients++;
                totalBalance += current->balance;
                if (current->isHospitalized) hospitalized++;
            }
            current = current->next;
        }

        Registration *r = regHead;
        while (r)
        {
            if (!r->isDeleted && isTimestampInRange(r->registerTime, startTime, endTime))
            {
                newRegistrations++;
                regRevenue += r->fee;
            }
            r = r->next;
        }

        Consultation *c = conHead;
        while (c)
        {
            if (!c->isDeleted && isTimestampInRange(c->consultationTime, startTime, endTime))
                newConsultations++;
            c = c->next;
        }
    }

    std::cout << "总患者数: " << totalPatients << std::endl;
    std::cout << "当前住院中: " << hospitalized << std::endl;
    std::cout << "患者账户总余额: " << std::fixed << std::setprecision(2) << (totalBalance / 100.0) << " 元" << std::endl;
    std::cout << std::string(50, '-') << std::endl;
    std::cout << "【时间段统计】" << std::endl;
    std::cout << "  新增挂号: " << newRegistrations << " 次 | 挂号收入: " << std::fixed << std::setprecision(2) << (regRevenue / 100.0) << " 元" << std::endl;
    std::cout << "  新增看诊: " << newConsultations << " 次" << std::endl;
    std::cout << "  总收入(挂号): " << std::fixed << std::setprecision(2) << (regRevenue / 100.0) << " 元" << std::endl;
    pause("统计报表 > 患者统计");
}

void Admin::showBedUtilizationReport(bedInfo *&bedHead, Hospitalization *&hosHead)
{
    // 时间范围选择
    int timeChoice = timeRangeMenu();
    if (timeChoice == 0) return;
    std::string startTime, endTime;
    if (timeChoice == 1) { startTime = getCurrentMonthStart(); }
    else if (timeChoice == 2) { startTime = getMonthsAgoStart(3); }
    else if (timeChoice == 3) { startTime = getMonthsAgoStart(6); }
    else if (timeChoice == 4)
    {
        startTime = inputDateCheck("请输入起始日期 (YYYY-MM-DD): ") + " 00:00:00";
        endTime = inputDateCheck("请输入结束日期 (YYYY-MM-DD): ") + " 23:59:59";
    }

    printTitle("床位使用率统计");
    if (timeChoice != 5)
        std::cout << "时间范围: " << startTime << " ~ " << (endTime.empty() ? "至今" : endTime) << std::endl << std::endl;

    int total = 0, occupied = 0, available = 0, maintenance = 0;
    std::map<std::string, int> deptTotal, deptOccupied;

    // 时间段内入院/出院统计
    int admittedInPeriod = 0, dischargedInPeriod = 0;
    long long totalHosRevenue = 0;

    Database &db = GetDB();
    if (db.isConnected())
    {
        // SQL path: aggregate via database queries
        std::string endBound = endTime.empty() ? "9999-12-31 23:59:59" : endTime;

        // Bed status overview: status 1=OCCUPIED, 2=CLEANING, 3=AVAILABLE, 4=UNAVAILABLE
        // In the linked-list code: OCCUPIED(1), CLEANING(2), AVAILABLE(0/3), UNAVAILABLE(4)
        // But enum values are: OCCUPIED=1, CLEANING=2, AVAILABLE=3, UNAVAILABLE=4
        // The linked list code counts: OCCUPIED -> occupied, AVAILABLE -> available, else -> maintenance
        // So: occupied = status=1, available = status=3, maintenance = status IN (2,4)
        std::string bedSql =
            "SELECT COUNT(*) as total, "
            "  SUM(CASE WHEN status = 1 THEN 1 ELSE 0 END) as occupied, "
            "  SUM(CASE WHEN status = 3 THEN 1 ELSE 0 END) as available, "
            "  SUM(CASE WHEN status IN (2, 4) THEN 1 ELSE 0 END) as maintenance "
            "FROM bed_info WHERE is_deleted = 0";
        MYSQL_RES *bedRes = db.query(bedSql);
        if (bedRes)
        {
            MYSQL_ROW row = mysql_fetch_row(bedRes);
            if (row)
            {
                total = Database::readInt(row, 0);
                occupied = Database::readInt(row, 1);
                available = Database::readInt(row, 2);
                maintenance = Database::readInt(row, 3);
            }
            mysql_free_result(bedRes);
        }

        // Per-department bed stats
        std::string deptSql =
            "SELECT department, COUNT(*) as total, "
            "  SUM(CASE WHEN status = 1 THEN 1 ELSE 0 END) as occupied "
            "FROM bed_info WHERE is_deleted = 0 GROUP BY department";
        MYSQL_RES *deptRes = db.query(deptSql);
        if (deptRes)
        {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(deptRes)))
            {
                std::string dept = Database::readString(row, 0);
                int deptTot = Database::readInt(row, 1);
                int deptOcc = Database::readInt(row, 2);
                deptTotal[dept] = deptTot;
                deptOccupied[dept] = deptOcc;
            }
            mysql_free_result(deptRes);
        }

        // Hospitalization period stats
        // HospitalizationStatus: APPLIED=1, PAID=2, ADMITTED=3, DISCHARGED=4, VOIDED=5
        // Revenue from DISCHARGED(4) or ADMITTED(3)
        std::string hosSql =
            "SELECT "
            "  SUM(CASE WHEN admit_time BETWEEN " + Database::quote(startTime) + " AND " + Database::quote(endBound) + " THEN 1 ELSE 0 END) as admitted, "
            "  SUM(CASE WHEN discharge_time BETWEEN " + Database::quote(startTime) + " AND " + Database::quote(endBound) + " THEN 1 ELSE 0 END) as discharged, "
            "  COALESCE(SUM(CASE WHEN status IN (3, 4) THEN total_cost_cents ELSE 0 END), 0) as total_revenue "
            "FROM hospitalizations WHERE is_deleted = 0";
        MYSQL_RES *hosRes = db.query(hosSql);
        if (hosRes)
        {
            MYSQL_ROW row = mysql_fetch_row(hosRes);
            if (row)
            {
                admittedInPeriod = Database::readInt(row, 0);
                dischargedInPeriod = Database::readInt(row, 1);
                totalHosRevenue = Database::readInt(row, 2);
            }
            mysql_free_result(hosRes);
        }
    }
    else
    {
        // Fallback: linked-list traversal
        bedInfo *current = bedHead;
        while (current)
        {
            if (!current->isDeleted)
            {
                total++;
                deptTotal[current->department]++;
                if (current->status == bedStatus::OCCUPIED) { occupied++; deptOccupied[current->department]++; }
                else if (current->status == bedStatus::AVAILABLE) available++;
                else maintenance++;
            }
            current = current->next;
        }

        Hospitalization *h = hosHead;
        while (h)
        {
            if (!h->isDeleted)
            {
                if (isTimestampInRange(h->admitTime, startTime, endTime)) admittedInPeriod++;
                if (isTimestampInRange(h->dischargeTime, startTime, endTime)) dischargedInPeriod++;
                if (h->status == HospitalizationStatus::DISCHARGED || h->status == HospitalizationStatus::ADMITTED)
                    totalHosRevenue += h->totalCost;
            }
            h = h->next;
        }
    }

    std::cout << "总床位: " << total << " | 已占用: " << occupied << " | 空闲: " << available
              << " | 维护中: " << maintenance << std::endl;
    if (total > 0)
        std::cout << "总使用率: " << std::fixed << std::setprecision(1) << (occupied * 100.0 / total) << "%" << std::endl;

    std::cout << "\n按科室统计:" << std::endl;
    for (auto &pair : deptTotal)
    {
        int occ = deptOccupied[pair.first];
        double rate = pair.second > 0 ? (occ * 100.0 / pair.second) : 0;
        std::cout << "  " << pair.first << ": " << pair.second << "张床位, " << occ << "张占用 ("
                  << std::fixed << std::setprecision(1) << rate << "%)" << std::endl;
    }

    std::cout << "\n【时间段统计】" << std::endl;
    std::cout << "  期间入院: " << admittedInPeriod << " 人次 | 期间出院: " << dischargedInPeriod << " 人次" << std::endl;
    std::cout << "  住院总收入: " << std::fixed << std::setprecision(2) << (totalHosRevenue / 100.0) << " 元" << std::endl;
    pause("统计报表 > 床位使用率");
}

void Admin::showMedicineInventoryReport(Medicine *&medHead, MedicationRecord *&medRecHead)
{
    // 时间范围选择
    int timeChoice = timeRangeMenu();
    if (timeChoice == 0) return;
    std::string startTime, endTime;
    if (timeChoice == 1) { startTime = getCurrentMonthStart(); }
    else if (timeChoice == 2) { startTime = getMonthsAgoStart(3); }
    else if (timeChoice == 3) { startTime = getMonthsAgoStart(6); }
    else if (timeChoice == 4)
    {
        startTime = inputDateCheck("请输入起始日期 (YYYY-MM-DD): ") + " 00:00:00";
        endTime = inputDateCheck("请输入结束日期 (YYYY-MM-DD): ") + " 23:59:59";
    }

    printTitle("药品库存统计");
    if (timeChoice != 5)
        std::cout << "时间范围: " << startTime << " ~ " << (endTime.empty() ? "至今" : endTime) << std::endl << std::endl;

    int totalTypes = 0, lowStock = 0, expiredCount = 0;
    long long totalValue = 0, totalPurchaseValue = 0;
    int outCount = 0, outQty = 0;

    // For low stock detail display
    struct LowStockItem { std::string id, name; int stock, safetyStock; };
    std::vector<LowStockItem> lowStockItems;

    Database &db = GetDB();
    if (db.isConnected())
    {
        // SQL path: aggregate via database queries
        std::string endBound = endTime.empty() ? "9999-12-31 23:59:59" : endTime;

        // Medicine overview
        std::string medSql =
            "SELECT COUNT(*) as total_types, "
            "  SUM(CASE WHEN stock <= safety_stock THEN 1 ELSE 0 END) as low_stock, "
            "  SUM(CASE WHEN expiry_date IS NOT NULL AND expiry_date != '#' AND expiry_date < CURDATE() THEN 1 ELSE 0 END) as expired, "
            "  COALESCE(SUM(stock * sale_price_cents), 0) as total_value, "
            "  COALESCE(SUM(stock * purchase_price_cents), 0) as total_purchase_value "
            "FROM medicines WHERE is_deleted = 0";
        MYSQL_RES *medRes = db.query(medSql);
        if (medRes)
        {
            MYSQL_ROW row = mysql_fetch_row(medRes);
            if (row)
            {
                totalTypes = Database::readInt(row, 0);
                lowStock = Database::readInt(row, 1);
                expiredCount = Database::readInt(row, 2);
                totalValue = Database::readInt(row, 3);
                totalPurchaseValue = Database::readInt(row, 4);
            }
            mysql_free_result(medRes);
        }

        // Low stock detail
        std::string lowSql =
            "SELECT medicine_id, name, stock, safety_stock "
            "FROM medicines WHERE is_deleted = 0 AND stock <= safety_stock";
        MYSQL_RES *lowRes = db.query(lowSql);
        if (lowRes)
        {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(lowRes)))
            {
                LowStockItem item;
                item.id = Database::readString(row, 0);
                item.name = Database::readString(row, 1);
                item.stock = Database::readInt(row, 2);
                item.safetyStock = Database::readInt(row, 3);
                lowStockItems.push_back(item);
            }
            mysql_free_result(lowRes);
        }

        // Outbound stats from medication_lines + medication_records
        // MedicationStatus: DISPENSED = 3
        std::string outSql =
            "SELECT COUNT(ml.id) as out_count, COALESCE(SUM(ml.quantity), 0) as out_qty "
            "FROM medication_lines ml "
            "JOIN medication_records mr ON ml.medication_record_id = mr.medication_record_id "
            "WHERE mr.is_deleted = 0 AND mr.status = 3 AND mr.dispense_time BETWEEN " +
            Database::quote(startTime) + " AND " + Database::quote(endBound);
        MYSQL_RES *outRes = db.query(outSql);
        if (outRes)
        {
            MYSQL_ROW row = mysql_fetch_row(outRes);
            if (row)
            {
                outCount = Database::readInt(row, 0);
                outQty = Database::readInt(row, 1);
            }
            mysql_free_result(outRes);
        }
    }
    else
    {
        // Fallback: linked-list traversal
        Medicine *current = medHead;
        while (current)
        {
            if (!current->isDeleted)
            {
                totalTypes++;
                totalValue += current->stock * current->salePrice;
                totalPurchaseValue += current->stock * current->purchasePrice;
                if (current->stock <= current->safetyStock) lowStock++;
                if (current->expiryDate != "#" && current->expiryDate < MyTime::getInstance().getTime().substr(0, 10))
                    expiredCount++;
            }
            current = current->next;
        }

        MedicationRecord *mr = medRecHead;
        while (mr)
        {
            if (!mr->isDeleted && mr->status == MedicationStatus::DISPENSED && isTimestampInRange(mr->dispenseTime, startTime, endTime))
            {
                for (const auto &line : mr->lines)
                {
                    outCount++;
                    outQty += line.quantity;
                }
            }
            mr = mr->next;
        }
    }

    std::cout << "药品种类: " << totalTypes << std::endl;
    std::cout << "库存预警(低于安全库存): " << lowStock << " 种" << std::endl;
    std::cout << "已过期药品: " << expiredCount << " 种" << std::endl;
    std::cout << "库存总价值(售价): " << std::fixed << std::setprecision(2) << (totalValue / 100.0) << " 元" << std::endl;
    std::cout << "库存总成本(进价): " << std::fixed << std::setprecision(2) << (totalPurchaseValue / 100.0) << " 元" << std::endl;
    std::cout << "潜在利润: " << std::fixed << std::setprecision(2) << ((totalValue - totalPurchaseValue) / 100.0) << " 元" << std::endl;

    if (lowStock > 0)
    {
        std::cout << "\n低库存药品列表:" << std::endl;
        if (!lowStockItems.empty())
        {
            // DB path: use pre-fetched items
            for (const auto &item : lowStockItems)
            {
                std::cout << "  " << item.id << " " << item.name
                          << " | 当前库存: " << item.stock
                          << " | 安全库存: " << item.safetyStock << std::endl;
            }
        }
        else
        {
            // Fallback: traverse linked list
            Medicine *current = medHead;
            while (current)
            {
                if (!current->isDeleted && current->stock <= current->safetyStock)
                {
                    std::cout << "  " << current->medicineID << " " << current->name
                              << " | 当前库存: " << current->stock
                              << " | 安全库存: " << current->safetyStock << std::endl;
                }
                current = current->next;
            }
        }
    }

    std::cout << "\n【时间段出库统计】" << std::endl;
    std::cout << "  出库: " << outCount << " 笔, 共 " << outQty << " 件" << std::endl;
    pause("统计报表 > 药品库存");
}

void Admin::showDataAnalysisReport(Hospitalization *&hosHead, bedInfo *&bedHead, Registration *&regHead, Medicine *&medHead)
{
    DataAnalysisReport report = runFullAnalysis(hosHead, bedHead, regHead, medHead);

    printTitle("数据分析与预测");
    std::cout << "报告生成时间: " << report.generatedAt << std::endl << std::endl;

    std::cout << "=== 表格报告 ===" << std::endl;
    displayTabularReport(report);

    std::cout << std::endl << "=== ASCII可视化图表 ===" << std::endl;
    displayAsciiChart(report);

    std::cout << std::endl << "=== 摘要报告 ===" << std::endl;
    displaySummaryReport(report);

    pause("统计报表 > 数据分析与预测");
}
