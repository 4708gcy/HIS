/**
 * @file LoadData.h
 * @brief 数据加载相关函数的声明
 * @details 从 MySQL 数据库加载用户信息和医疗记录到内存链表
 * @author 郭承宇
 * @date 2026-5-29
 * @version 2.0
 */

#ifndef LOADDATA_H
#define LOADDATA_H

#include "Roles/Admin.h"

// =====================人物数据加载函数=====================
Admin *loadAdminData(int &count);           // 从 MySQL 加载管理员数据
Doctor *loadDoctorData(int &count);         // 从 MySQL 加载医生数据
Nurse *loadNurseData(int &count);           // 从 MySQL 加载护士数据
Pharmacist *loadPharmacistData(int &count); // 从 MySQL 加载药师数据
Patient *loadPatientData(int &count);       // 从 MySQL 加载患者数据

// =====================医疗记录数据加载函数=====================
Registration *loadRegistrations(int &count);         // 从 MySQL 加载挂号记录
Consultation *loadConsultations(int &count);         // 从 MySQL 加载看诊记录
Examination *loadExaminations(int &count);           // 从 MySQL 加载检查记录
Hospitalization *loadHospitalizations(int &count);   // 从 MySQL 加载住院记录
MedicationRecord *loadMedicationRecords(int &count); // 从 MySQL 加载用药记录
Medicine *loadMedicines(int &count);                 // 从 MySQL 加载药品信息
bedInfo *loadBedInfos(int &count);                   // 从 MySQL 加载床位信息
struct NursingRecord;
NursingRecord *loadNursingRecords(int &count); // 从 MySQL 加载护理记录

#endif // LOADDATA_H