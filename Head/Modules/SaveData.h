/**
 * @file SaveData.h
 * @brief 数据保存相关函数的声明
 * @details 将内存链表中的数据同步到 MySQL 数据库（事务化、支持回滚）
 * @author 郭承宇
 * @date 2026-5-29
 * @version 2.0
 */

#ifndef SAVEDATA_H
#define SAVEDATA_H

#include "Roles/Admin.h"

// =====================人物数据保存函数=====================
void saveAdminData(Admin *adminHead, int count);    // 保存到 MySQL（事务）
void saveDoctorData(Doctor *doctorHead, int count); // 保存到 MySQL（事务）
void saveNurseData(Nurse *nurseHead, int count);    // 保存到 MySQL（事务）
void savePharmacistData(Pharmacist *pharmacistHead,
                        int count);                    // 保存到 MySQL（事务）
void savePatientData(Patient *patientHead, int count); // 保存到 MySQL（事务）

// =====================医疗记录数据保存函数=====================
void saveRegistrations(Registration *regHead,
                       int count); // 保存到 MySQL（事务）
void saveConsultations(Consultation *conHead,
                       int count);                       // 保存到 MySQL（事务）
void saveExaminations(Examination *examHead, int count); // 保存到 MySQL（事务）
void saveHospitalizations(Hospitalization *hosHead,
                          int count); // 保存到 MySQL（事务）
void saveMedicationRecords(MedicationRecord *medRecHead,
                           int count);            // 保存到 MySQL（事务）
void saveMedicines(Medicine *medHead, int count); // 保存到 MySQL（事务）
void saveBedInfos(bedInfo *bedHead, int count);   // 保存到 MySQL（事务）
struct NursingRecord;
void saveNursingRecords(NursingRecord *head, int count); // 保存到 MySQL（事务）

#endif