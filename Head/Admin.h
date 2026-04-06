/**
 * @file Admin.h
 * @brief 管理员类的定义
 * @details 该头文件定义了一个 Admin 类，继承自 User 类，包含管理员特有的功能和权限管理方法
 * @author 郭承宇
 * @date 2026-4-6
 * @version 1.0
 */

#ifndef ADMIN_H
#define ADMIN_H

#include "User.h"

class Admin : public User { 
    private:
    double historyBalance; // 医院的流动资金余额

    public:
    void loadFromFile() override; // 从文件加载管理员数据
    void saveToFile() override; // 将管理员数据保存到文件

    // 医生管理模块
    void addDoctor();    // 添加医生
    void deleteDoctor(); // 删除医生
    void modifyDoctor(); // 修改医生信息
    void queryDoctor();  // 查询医生信息
    void listDoctors();  // 列出所有医生

    // 护士管理模块
    void addNurse();    // 添加护士
    void deleteNurse(); // 删除护士
    void modifyNurse(); // 修改护士信息
    void queryNurse();  // 查询护士信息
    void listNurses();  // 列出所有护士

    // 药剂师管理模块
    void addPharmacist();    // 添加药剂师
    void deletePharmacist(); // 删除药剂师
    void modifyPharmacist(); // 修改药剂师信息
    void queryPharmacist();  // 查询药剂师信息
    void listPharmacists();  // 列出所有药剂师

    // 患者管理模块
    void addPatient();    // 添加患者
    void deletePatient(); // 删除患者
    void modifyPatient(); // 修改患者信息
    void queryPatient();  // 查询患者信息
    void listPatients();  // 列出所有患者

    // 病房管理模块
    void addWard();    // 添加病房
    void deleteWard(); // 删除病房
    void modifyWard(); // 修改病房信息
    void queryWard();  // 查询病房信息
    void listWards();  // 列出所有病房

    // 医疗记录管理模块
    // 挂号记录
    void addRegistrationRecord();    // 添加挂号记录
    void deleteRegistrationRecord(); // 删除挂号记录
    void modifyRegistrationRecord(); // 修改挂号记录
    void queryRegistrationRecord();  // 查询挂号记录
    void listRegistrationRecords();  // 列出所有挂号记录

    // 看诊记录
    void addConsultationRecord();    // 添加看诊记录
    void deleteConsultationRecord(); // 删除看诊记录
    void modifyConsultationRecord(); // 修改看诊记录
    void queryConsultationRecord();  // 查询看诊记录
    void listConsultationRecords();  // 列出所有看诊记录

    // 检查记录
    void addExaminationRecord();    // 添加检查记录
    void deleteExaminationRecord(); // 删除检查记录
    void modifyExaminationRecord(); // 修改检查记录
    void queryExaminationRecord();  // 查询检查记录
    void listExaminationRecords();  // 列出所有检查记录

    // 住院记录
    void addHospitalizationRecord();    // 添加住院记录
    void deleteHospitalizationRecord(); // 删除住院记录
    void modifyHospitalizationRecord(); // 修改住院记录
    void queryHospitalizationRecord();  // 查询住院记录
    void listHospitalizationRecords();  // 列出所有住院记录

    // 药品管理模块
    void restockMedication();        // 药品入库补充
    void addMedication();            // 添加新药品种类
    void deleteMedication();         // 删除药品种类
    void modifyMedication();         // 修改药品信息
    void queryMedication();          // 查询药品信息
    void listMedications();          // 列出所有药品
    void viewMedicationUseHistory(); // 查看药品使用历史记录

    // 财务管理模块
    void viewFinancialReport(); // 查看财务报表
    void viewBillingRecords();    // 查看账单记录
};

#endif // ADMIN_H