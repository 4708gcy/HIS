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
#include "UI.h"

Admin* adminLogin(Admin *&adminHead); // 管理员登录函数，验证管理员身份并返回登录结果

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
void deleteAdmin(Admin *&admin);                  // 删除管理员信息（真实删除）
void addAdmin(Admin *&admin, int &idCounter);     // 添加管理员信息（根据输入信息创建新的 Admin 对象，并插入到链表中）

#endif // LOGIN_H