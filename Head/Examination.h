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

enum class ExaminationStatus
{
    ORDERED,
    IN_PROGRESS,
    REPORTED,
    CANCELED
};

struct Examination
{
    std::string examinationID;
    std::string consultationID;
    std::string patientID;
    std::string doctorID;

    std::string itemName;
    std::string orderTime;
    std::string reportTime;
    std::string reportSummary;

    ExaminationStatus status = ExaminationStatus::ORDERED;

    Examination *prev = nullptr;
    Examination *next = nullptr;
};

#endif