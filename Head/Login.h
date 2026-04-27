/**
 * @file Login.h
 * @brief 医院信息系统中的人员登录管理相关函数声明
 * @details 该头文件声明了与人员登录管理相关的函数，包括管理员登录验证和账户管理功能的接口
 * @date 2026-4-9
 * @version 1.0
 */

#ifndef LOGIN_H
#define LOGIN_H

#include "Admin.h"
#include "Doctor.h"
#include "Pharmacist.h"
#include "UI.h"

Admin *adminLogin(Admin *&adminHead);                     // 管理员登录函数，验证管理员身份并返回登录结果
Doctor *doctorLogin(Doctor *&doctorHead);                 // 医生登录函数，验证医生身份并返回登录结果
Patient *patientLogin(Patient *&patientHead);             // 患者登录函数
Pharmacist *pharmacistLogin(Pharmacist *&pharmacistHead); // 药剂师登录函数 // 患者登录函数，验证患者身份并返回登录结果
Nurse *nurseLogin(Nurse *&nurseHead);                     // 护士登录函数，验证护士身份并返回登录结果

void manageAdmins(Admin *&admin, int &idCounter); // 管理管理员信息
void viewAllAdmins(Admin *&admin);                // 查看所有管理员信息
void viewAdminByID(Admin *&admin);                // 根据管理员ID查看管理员信息
void viewAdminsByName(Admin *&admin);             // 根据管理员姓名查看管理员信息
void viewAdminsByGender(Admin *&admin);           // 根据管理员性别查看管理员信息
void viewAdminsByAgeGroup(Admin *&admin);         // 根据管理员年龄段查看管理员信息
void viewAdminsByContactInfo(Admin *&admin);      // 根据管理员联系方式查看管理员信息
void modifyAdminName(Admin *&admin);              // 修改管理员姓名
void modifyAdminGender(Admin *&admin);            // 修改管理员性别
void modifyAdminAge(Admin *&admin);               // 修改管理员年龄
void modifyAdminTelephone(Admin *&admin);         // 修改管理员联系电话
void modifyAdminEmail(Admin *&admin);             // 修改管理员邮箱地址
void deleteAdmin(Admin *&admin);                  // 删除管理员信息（逻辑删除）
void addAdmin(Admin *&admin, int &idCounter);     // 添加管理员信息（根据输入信息创建新的 Admin 对象，并插入到链表中）

void AccountManagement(Admin *&adminHead, Doctor *&doctorHead, Nurse *&nurseHead, Pharmacist *&pharmacistHead, Patient *&patientHead); // 账号激活/封锁管理函数，允许管理员激活或封锁其他用户的账户

template <typename UserType>
void AccountManageGeneric(UserType *&userHead, const std::string &roleName, const std::string &idPrompt)
{
    if (userHead == nullptr)
    {
        std::cout << "当前没有" << roleName << "账户可供管理。" << std::endl;
        return;
    }
    UserType *current = userHead;
    std::cout << "正在查询" << roleName << "账户列表..." << std::endl;
    while (current != nullptr)
    {
        if (!current->getIsDeleted())
        {
            std::cout << roleName << "ID: " << current->getUserID()
                      << ", 姓名: " << current->getUsername()
                      << ", 账户状态: " << (current->getIsAccountActive() ? "激活" : "锁定") << std::endl;
        }
        current = current->next;
    }
    std::string targetID = inputIDCheck(idPrompt);
    current = userHead;
    bool found = false;
    while (current != nullptr)
    {
        if (!current->getIsDeleted() && current->getUserID() == targetID)
        {
            std::cout << "当前" << roleName << "账户状态: " << (current->getIsAccountActive() ? "激活" : "锁定") << std::endl;
            std::cout << "请选择操作: " << std::endl;
            std::cout << "1. 激活账户" << std::endl;
            std::cout << "2. 封锁账户" << std::endl;
            std::cout << "0. 返回上一级菜单" << std::endl;
            int actionChoice = selectIntCheck(0, 2);
            if (actionChoice == 1)
            {
                current->setIsAccountActive(true);
                current->setLoginAttempts(0);
                std::cout << roleName << "账户已激活！" << std::endl;
            }
            else if (actionChoice == 2)
            {
                current->setIsAccountActive(false);
                current->setLoginAttempts(failedLoginLimit);
                std::cout << roleName << "账户已封锁！" << std::endl;
            }
            pause();
            found = true;
            break;
        }
        current = current->next;
    }
    if (!found)
    {
        std::cout << "未找到指定ID的" << roleName << "账户。" << std::endl;
    }
} // 管理患者账户状态（激活/封锁）

#endif // LOGIN_H