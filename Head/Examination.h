/**
 * @file Examination.h
 * @brief 医疗管理系统中的检查信息结构体
 * @details 该头文件定义了一个 Examination 结构体，用于存储患者的检查信息，包括检查ID、患者ID、医生ID、检查时间、检查结果等
 * @author 郭承宇
 * @date 2026-4-7
 * @version 1.0
 */

#ifndef EXAMINATION_H
#define EXAMINATION_H

#include <string>

/**
 * @file Examination.h
 * @brief 检查/检验记录结构体
 * @details 包括检查项目、下单时间、报告时间与报告摘要
 */

enum class ExaminationStatus
{
    ORDERED,     // 已下单
    IN_PROGRESS, // 检查中
    REPORTED,    // 报告已出
    CANCELED     // 取消
};

struct Examination
{
    std::string examinationID;  // 检查记录唯一ID
    std::string consultationID; // 关联看诊记录ID
    std::string patientID;      // 患者ID
    std::string doctorID;       // 开单医生ID

    std::string itemName;      // 检查项目名称
    std::string orderTime;     // 下单时间
    std::string reportTime;    // 报告时间（若已出）
    std::string reportSummary; // 报告摘要或结果文本

    // 新增用于计费/支付的字段
    double fee = 0.0;    // 该检查的费用
    bool isPaid = false; // 是否已由患者缴费（缴费后医院可安排检查）

    ExaminationStatus status = ExaminationStatus::ORDERED;

    Examination *prev = nullptr;
    Examination *next = nullptr;
};

#endif // EXAMINATION_H