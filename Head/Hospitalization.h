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

enum class HospitalizationStatus
{
    APPLIED,
    ADMITTED,
    DISCHARGED,
    CANCELED
};

struct Hospitalization
{
    std::string hospitalizationID;
    std::string patientID;
    std::string doctorID;
    std::string nurseID;

    std::string department;
    std::string wardType;
    std::string bedNo;

    std::string admitTime;
    std::string dischargeTime;

    double deposit = 0.0;
    double totalCost = 0.0;

    HospitalizationStatus status = HospitalizationStatus::APPLIED;

    Hospitalization *prev = nullptr;
    Hospitalization *next = nullptr;
};

#endif