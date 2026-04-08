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

#include <string>
#include <vector>
#include "User.h"
#include "Registration.h"
#include "Consultation.h"
#include "Examination.h"
#include "Hospitalization.h"
#include "MedicationRecord.h"
#include "Medicine.h"

class Admin : public User
{
private:
    double totalRevenue = 0.0; // 医院总收入统计（可选）
    double totalExpenses = 0.0; // 医院总支出统计（可选）
    double netProfit = 0.0;    // 医院净利润统计（可选）


public:
    Admin *next; // 管理员链表的下一个节点指针

    Admin();

    Admin *adminSignUp(); // 管理员注册，调用基类的 signUp(1) 方法，并将管理员信息保存到文件中
    bool adminSignIn();   // 管理员登录，验证用户ID和密码，并设置登录状态

    template <typename T>
    void setAccountActive(T* head); // 激活或锁定账户

    // === 管理医疗记录 ===
    void manageRegistrations(Registration *reg, std::string department);            // 管理挂号记录（查看、修改状态等）
    void manageConsultations(Consultation *con, std::string department);            // 管理看诊记录（查看、修改诊断结果等）
    void manageExaminations(Examination *exam, std::string department);             // 管理检查记录（查看、修改报告摘要等）
    void manageHospitalizations(Hospitalization *hos, std::string department);      // 管理住院记录（查看、修改出院日期等）
    void manageMedicationRecords(MedicationRecord *medRec, std::string department); // 管理用药记录（查看、修改用药详情等）
    void manageMedicines(Medicine *med, std::string department);                    // 管理药品信息（查看、修改库存等）
};

#endif // ADMIN_H