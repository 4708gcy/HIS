/**     
 * @file LoadData.h
 * @brief 数据加载相关函数的声明
 * @details 该头文件声明了与数据加载相关的函数，包括从文件中加载用户信息和医疗记录的功能
 * @author 郭承宇
 * @date 2026-4-9
 * @version 1.0
 */



#ifndef LOADDATA_H
#define LOADDATA_H

#include "Admin.h"
#include <fstream>

// =====================人物数据加载函数=====================
Admin *loadAdminData(int& count); // 从文件中加载管理员数据并返回管理员链表的头指针
Doctor *loadDoctorData(int &count); // 从文件中加载医生数据并返回医生链表的头指针
Nurse *loadNurseData(int &count); // 从文件中加载护士数据并返回护士链表的头指针
Pharmacist *loadPharmacistData(int &count); // 从文件中加载药师数据并返回药师链表的头指针

// =====================医疗记录数据加载函数=====================
Registration *loadRegistrations(int& count); // 从文件中加载挂号记录并返回挂号链表的头指针
Consultation *loadConsultations(int& count); // 从文件中加载看诊记录并返回看诊链表的头指针
Examination *loadExaminations(int& count); // 从文件中加载检查记录并返回检查链表的头指针
Hospitalization *loadHospitalizations(int& count); // 从文件中加载住院记录并返回住院链表的头指针
MedicationRecord *loadMedicationRecords(int& count); // 从文件中加载用药记录并返回用药链表的头指针
Medicine *loadMedicines(int& count); // 从文件中加载药品信息并返回药品链表的头指针
bedInfo *loadBedInfos(int& count); // 从文件中加载床位信息并返回床位链表的头指针


#endif // LOADDATA_H