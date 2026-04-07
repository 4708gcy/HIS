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

#include <string>

/**
 * @file Hospitalization.h
 * @brief 住院记录结构体
 * @details 包含病房、床位、时间和费用字段
 */

enum class HospitalizationStatus
{
    APPLIED,    // 申请中/待处理
    ADMITTED,   // 已入院
    DISCHARGED, // 已出院
    CANCELED    // 已取消
};

struct Hospitalization
{
    std::string hospitalizationID; // 住院记录唯一ID
    std::string patientID;         // 患者ID
    std::string doctorID;          // 申请医生ID
    std::string nurseID;           // 负责护士ID（可选）

    std::string department; // 所属科室
    std::string wardType;   // 病房类型（普通/隔离/VIP）
    std::string bedNo;      // 床位号

    std::string admitTime;     // 入院时间
    std::string dischargeTime; // 出院时间（若已出院）

    double deposit = 0.0;   // 押金
    double totalCost = 0.0; // 总费用

    HospitalizationStatus status = HospitalizationStatus::APPLIED;

    Hospitalization *prev = nullptr;
    Hospitalization *next = nullptr;
};

#endif // HOSPITALIZATION_H