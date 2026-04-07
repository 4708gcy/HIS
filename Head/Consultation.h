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

enum class ConsultationStatus
{
    OPEN,
    CLOSED,
    VOIDED
};

struct Consultation
{
    std::string consultationID;
    std::string registrationID;
    std::string patientID;
    std::string doctorID;
    std::string consultationTime;

    std::string chiefComplaint;
    std::string preliminaryDiagnosis;
    std::string medicalAdvice;

    ConsultationStatus status = ConsultationStatus::OPEN;

    Consultation *prev = nullptr;
    Consultation *next = nullptr;
};

#endif