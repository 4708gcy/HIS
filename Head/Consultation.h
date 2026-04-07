/**
 * @file Consultation.h
 * @brief 医疗管理系统中的看诊信息结构体
 * @details 该头文件定义了一个 Consultation 结构体，用于存储患者的看诊信息，包括看诊ID、患者ID、医生ID、看诊时间、诊断结果和处方等
 * @author 郭承宇
 * @date 2026-4-7
 * @version 1.0
 */

#ifndef CONSULTATION_H
#define CONSULTATION_H

#include <string>

/**
 * @file Consultation.h
 * @brief 看诊/就诊记录结构体
 * @details 每条看诊记录包含主诉、初步诊断、医嘱等信息
 */

enum class ConsultationStatus
{
    OPEN,   // 正在处理
    CLOSED, // 已结束
    VOIDED  // 作废
};

struct Consultation
{
    std::string consultationID;   // 看诊记录唯一ID
    std::string registrationID;   // 对应挂号ID
    std::string patientID;        // 患者ID
    std::string doctorID;         // 医生ID
    std::string consultationTime; // 看诊时间戳

    std::string chiefComplaint;       // 主诉
    std::string preliminaryDiagnosis; // 初步诊断
    std::string medicalAdvice;        // 医嘱或处方摘要

    ConsultationStatus status = ConsultationStatus::OPEN; // 状态

    Consultation *prev = nullptr;
    Consultation *next = nullptr;
};

#endif // CONSULTATION_H