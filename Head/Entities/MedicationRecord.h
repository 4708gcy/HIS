/**
 * @file MedicationRecord.h
 * @brief 医疗管理系统中的用药记录结构体定义
 * @details 该头文件定义了一个 MedicationRecord 结构体，用于存储患者的用药信息，包括用药ID、患者ID、医生ID、药品ID、用药时间、剂量和用法等
 * @author 郭承宇
 * @date 2026-4-7
 * @version 1.0
 */

#ifndef MEDICATIONRECORD_H
#define MEDICATIONRECORD_H

#include <vector>
#include <string>

/**
 * @brief 用药审核状态
 */
enum class MedicationReviewStatus
{
    PENDING_REVIEW = 1, // 待药师审核
    REJECTED,           // 审核拒绝
    APPROVED,           // 审核通过
    CANCELED            // 医生撤销
};

/**
 * @brief 用药支付/发药状态
 */
enum class MedicationStatus
{
    UNPAID = 1, // 未缴费
    PAID,       // 已缴费未发药
    DISPENSED,  // 已发药（已扣库存）
    RETURNED    // 已退药（已回库）
};

/**
 * @brief 用药明细项（处方行）
 */
struct MedicationLine
{
    std::string medicineID = "#";   // 药品ID
    std::string medicineName = "#"; // 药品名称（冗余以便展示）
    int quantity = 0;               // 药品数量
    int unitPrice = 0;         // 下单时单价（单位：分）
    std::string note = "#";         // 用法备注
};

/**
 * @brief 用药记录（即处方）
 */
struct MedicationRecord
{
    std::string consultationID = "#"; // 对应的看诊记录ID
    std::string medRecordID = "#";    // 用药记录ID
    std::string doctorID = "#";       // 医生ID
    std::string pharmacistID = "#";   // 药师ID
    std::string patientID = "#";      // 患者ID
    std::string department = "#";     // 科室
    std::string createTime = "#";     // 创建时间

    std::vector<MedicationLine> lines; // 用药明细
    int totalCost = 0;                 // 总费用（单位：分）

    MedicationReviewStatus reviewStatus = MedicationReviewStatus::PENDING_REVIEW; // 审核状态
    MedicationStatus status = MedicationStatus::UNPAID;                           // 支付/发药状态

    std::string paymentTime = "#";  // 支付时间
    std::string dispenseTime = "#"; // 发药时间
    std::string note = "#";         // 备注

    bool isDeleted = false; // 逻辑删除标志

    MedicationRecord *prev = nullptr; // 双向链表前指针
    MedicationRecord *next = nullptr; // 双向链表后指针
};

#endif // MEDICATIONRECORD_H
