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
    PENDING,     // 待就诊
    IN_PROGRESS, // 正在处理
    COMPLETED,   // 已结束
    VOIDED       // 已作废
};

// 简化处方/医嘱结构（仅摘要用）
struct Prescription
{
    std::string medicineID; // 药品ID（若无可留空）
    std::string name;       // 药品名（冗余以便展示）
    std::string dosage;     // 用量，例如 "500 mg"
    std::string frequency;  // 频次，例如 "每日3次"
    std::string duration;   // 疗程，例如 "7 天"
    std::string note;       // 备注/注意事项
};

struct Consultation
{
    std::string consultationID;   // 看诊记录唯一ID
    std::string registrationID;   // 对应挂号ID
    std::string patientID;        // 患者ID
    std::string doctorID;         // 医生ID
    std::string consultationTime; // 看诊时间戳
    std::string department;       // 科室名称

    std::string chiefComplaint;          // 主诉(患者或家属描述的症状, 简短)
    std::string historyOfPresentIllness; // 现病史(对主诉的详细描述, 包括发病时间, 症状发展、伴随症状等)
    std::string pastMedicalHistory;      // 既往史(患者的相关病史, 包括慢性病、手术史、过敏史等)
    std::string familyHistory;           // 家族史(患者家族成员的相关病史, 如遗传病、慢性病等)

    std::string preliminaryDiagnosis;         // 初步诊断
    std::vector<std::string> examinationlist; // 计划检查项目列表
    std::vector<Prescription> prescriptions;  // 处方列表（可包含多条医嘱）
    bool isPrecriptionReviewed = false; // 处方是否已审核（由药师或医生审核后设置为 true）

    bool isHospitalizationRecommended = false; // 是否建议住院（根据病情严重程度等因素评估得出）

    // 附件和备注
    std::vector<std::string> attachments; // 看诊相关的附件文件路径列表
    std::string note;                     // 医生备注或特殊说明

    ConsultationStatus status = ConsultationStatus::PENDING; // 状态

    // 关联之前的看诊记录（如同一挂号中多次看诊）
    std::vector<std::string> relatedConsultationIDs; // 相关看诊记录ID列表

    bool isDeleted = false; // 逻辑删除标志（实际删除时设置为 true）

    Consultation *prev = nullptr;
    Consultation *next = nullptr;
};

#endif // CONSULTATION_H