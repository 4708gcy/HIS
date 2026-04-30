/**
 * @file SaveData.h
 * @brief 数据保存相关函数的声明
 * @details 该头文件声明了与数据保存相关的函数，包括将用户信息和医疗记录保存到文件中的功能
 * @author 郭承宇
 * @date 2026-4-9
 * @version 1.0
 */

#ifndef SAVEDATA_H
#define SAVEDATA_H

#include "Admin.h"
#include <fstream>

// =====================人物数据保存函数=====================
void saveAdminData(Admin *adminHead, int count);                // 将管理员数据保存到文件中
void saveDoctorData(Doctor *doctorHead, int count);             // 将医生数据保存到文件中
void saveNurseData(Nurse *nurseHead, int count);                // 将护士数据保存到文件中
void savePharmacistData(Pharmacist *pharmacistHead, int count); // 将药剂师数据保存到文件中
void savePatientData(Patient *patientHead, int count);          // 将患者数据保存到文件中

// =====================医疗记录数据保存函数=====================
void saveRegistrations(Registration *regHead, int count);            // 将挂号记录保存到文件中
void saveConsultations(Consultation *conHead, int count);            // 将看诊记录保存到文件中
void saveExaminations(Examination *examHead, int count);             // 将检查记录保存到文件中
void saveHospitalizations(Hospitalization *hosHead, int count);      // 将住院记录保存到文件中
void saveMedicationRecords(MedicationRecord *medRecHead, int count); // 将用药记录保存到文件中
void saveMedicines(Medicine *medHead, int count);                    // 将药品信息保存到文件中
void saveMedicineFlows(MedicineFlow *flowHead, int count);          // 将药品流水记录保存到文件中
void saveBedInfos(bedInfo *bedHead, int count);                      // 将床位信息保存到文件中

#endif