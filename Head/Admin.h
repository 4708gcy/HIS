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
#include "Doctor.h"

class Admin : public User
{
private:
    double totalRevenue = 0.0;  // 医院总收入统计（可选）
    double totalExpenses = 0.0; // 医院总支出统计（可选）
    double netProfit = 0.0;     // 医院净利润统计（可选）

    bool isDeleted = false; // 逻辑删除标志（实际删除时设置为 true）

public:
    Admin *next = nullptr; // 管理员链表的下一个节点指针

    Admin();

    Admin *adminSignUp(int &idCounter); // 管理员注册，调用基类的 signUp(1) 方法，并将管理员信息保存到文件中
    bool adminSignIn();                 // 管理员登录，验证用户ID和密码，并设置登录状态

    template <typename T>
    void setAccountActive(T *&head)
    {
        std::string targetID;
        std::cout << "请输入要操作的用户ID: ";
        std::cin >> targetID;

        T *current = head;
        while (current != nullptr)
        {
            if (current->getUserID() == targetID)
            {
                bool newStatus;
                std::cout << "请输入新的账户状态 (1 - 激活, 0 - 锁定): ";
                std::cin >> newStatus;
                if (newStatus)
                {
                    current->isAccountActive = true;
                    current->loginAttempts = 0; // 重置登录失败次数
                    std::cout << "账户已激活！" << std::endl;
                }
                else
                {
                    current->isAccountActive = false;
                    std::cout << "账户已锁定！" << std::endl;
                }
                return;
            }
            current = current->next; // 假设 T 中有 next 指针
        }

        std::cout << "未找到指定用户！" << std::endl;
    }

    // === 管理医疗记录 ===
    void manageRegistrations(Registration *&reg, Doctor *&doc, const std::string &department, int &idCounter); // 管理挂号记录（查看、修改状态等）
    void viewAllRegistrations(Registration *&reg, const std::string &department);                              // 查看所有挂号记录（可按患者ID、医生ID、状态过滤）
    void viewRegistrationsByDoctor(Registration *&reg, const std::string &department);                         // 查看指定医生的挂号记录
    void viewRegistrationsByPatient(Registration *&reg, const std::string &department);                        // 查看指定患者的挂号记录
    void viewRegistrationsByStatus(Registration *&reg, const std::string &department);                         // 查看指定状态的挂号记录
    void modifyRegistrationStatus(Registration *&reg, const std::string &department);                          // 修改挂号记录状态（如支付、取消等）
    void deleteRegistration(Registration *&reg, const std::string &department);                                // 删除挂号记录（逻辑删除，设置 isDeleted 标志）
    void addRegistration(Registration *&reg, Doctor *&doc, const std::string &department, int &idCounter);     // 添加挂号记录（根据输入信息创建新的 Registration 对象，并插入到链表中）

    void manageConsultations(Consultation *&con, const std::string &department, Registration *reg, int &conCounter);               // 管理看诊记录（查看、修改诊断结果等）
    void viewAllConsultations(Consultation *&con, const std::string &department);                                            // 查看所有看诊记录（可按患者ID、医生ID、状态过滤）
    void viewConsultationsByDoctor(Consultation *&con, const std::string &department);                                       // 查看指定医生的看诊记录
    void viewConsultationsByPatient(Consultation *&con, const std::string &department);                                      // 查看指定患者的看诊记录
    void viewConsultationsByStatus(Consultation *&con, const std::string &department);                                       // 查看指定状态的看诊记录
    void viewConsultationByRegistrationID(Consultation *&con, const std::string &department);                                // 根据挂号ID查看看诊记录
    void modifyConsultation(Consultation *&con, const std::string &department);                                              // 修改看诊记录（如修改诊断结果、添加医生备注等）
    void deleteConsultation(Consultation *&con, const std::string &department);                                              // 删除看诊记录（逻辑删除，设置 isDeleted 标志）
    void addConsultation(Consultation *&con, const std::string &department, Registration *reg, int &conCounter); // 添加看诊记录（根据输入信息创建新的 Consultation 对象，并插入到链表中）

    void manageExaminations(Examination *&exam, const std::string &department, int &idCounter);             // 管理检查记录（查看、修改报告摘要等）
    void manageHospitalizations(Hospitalization *&hos, const std::string &department, int &idCounter);      // 管理住院记录（查看、修改出院日期等）
    void manageMedicationRecords(MedicationRecord *&medRec, const std::string &department, int &idCounter); // 管理用药记录（查看、修改用药详情等）
    void manageMedicines(Medicine *&med, const std::string &department, int &idCounter);                    // 管理药品信息（查看、修改库存等）
};

#endif // ADMIN_H