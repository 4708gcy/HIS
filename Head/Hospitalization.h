/**
 * @file Hospitalization.h
 * @brief 医疗管理系统中的住院信息结构体
 * @details 该头文件定义了一个 Hospitalization 结构体，用于存储患者的住院信息，包括住院ID、患者ID、医生ID、住院时间、出院时间等
 * @author 郭承宇
 * @date 2026-4-7
 * @version 1.0
 */

#ifndef HOSPITALIZATION_H
#define HOSPITALIZATION_H

#include "LinkedList.h"
#include <string>

/**
 * @file Hospitalization.h
 * @brief 住院记录结构体
 * @details 包含病房、床位、时间和费用字段
 */

enum class HospitalizationStatus
{
    APPLIED = 1,    // 申请中/待处理
    PAID,       // 已缴费,待分床
    ADMITTED,   // 已入院
    DISCHARGED, // 已出院
    VOIDED      // 已作废
};

enum class bedStatus
{
    OCCUPIED = 1,   // 已占用
    ClEANING,   // 清洁中
    AVAILABLE,  // 可分配
    UNAVAILABLE // 不可用（如维修中等）
};

struct bedInfo
{
    std::string bedID = "#"; // 生成的床位ID（格式：科室-区域-类型-病房-床位）

    bedStatus status = bedStatus::AVAILABLE; // 床位状态
    std::string wardType = "#";                    // 病房类型（如普通病房/隔离病房/VIP病房/ICU等）
    std::string department = "#";                  // 所属科室
    int areaNumber = 0;                          // 区域号（如1-10）
    int wardNumber = 0;                          // 病房号（如1-999）
    int bedNumber = 0;                           // 床位号（如1-10）

    std::string note = "#"; // 备注信息（如特殊护理需求等）

    VitalSigns vitalSigns; // 病人的生命体征

    std::string patientID = "#"; // 占用该床位的患者ID
    std::string nurseID = "#";   // 看护该床位的护士ID

    bool isDeleted = false; // 逻辑删除标志

    int useTimes = 0; // 床位使用次数统计（可选）
    int daysOccupied = 0; // 床位占用天数统计（可选）

    bedInfo *prev = nullptr;
    bedInfo *next = nullptr; // 链表指针
};

// 普通病房 50元/天，隔离病房 100元/天，VIP病房 200元/天，ICU病房 500元/天

struct Hospitalization
{
    std::string consultationID = "#";    // 关联看诊记录ID
    std::string hospitalizationID = "#"; // 住院记录唯一ID
    std::string patientID = "#";         // 患者ID
    std::string doctorID = "#";          // 申请医生ID
    std::string nurseID = "#";           // 负责护士ID（可选）

    std::string department = "#"; // 所属科室
    std::string wardType = "#";   // 病房类型（如普通病房/隔离病房/VIP病房/ICU等）
    std::string bedNumber = "#";  // 床位号

    std::string applyTime = "#";          // 申请时间
    std::string admitTime = "#";          // 实际入院时间
    std::string dischargeTime = "#";      // 出院时间
    std::string availableAdmitTime = "#"; // 护士分床后给出的可入住时间

    double deposit = 0.0;   // 押金
    double totalCost = 0.0; // 总费用

    HospitalizationStatus status = HospitalizationStatus::APPLIED;

    bool isDeleted = false; // 逻辑删除标志

    // 关联的住院记录ID列表（如转科、转院等情况）
    LinkedList<std::string> relatedHospitalizationIDs = {}; // 相关住院记录ID列表

    Hospitalization *prev = nullptr;
    Hospitalization *next = nullptr;
};

#endif // HOSPITALIZATION_H
