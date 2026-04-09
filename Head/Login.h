/**
 * @file StaffManagement.h
 * @brief 医院信息系统中的人员管理相关类和函数的定义
 * @details 该头文件定义了医院信息系统中与人员管理相关的类和函数
 * @date 2026-4-9
 * @version 1.0
 */

#ifndef STAFFMANAGEMENT_H
#define STAFFMANAGEMENT_H

#include "Admin.h"
#include "UI.h"

Admin* adminLogin(Admin *&adminHead); // 管理员登录函数，验证管理员身份并返回登录结果

#endif // STAFFMANAGEMENT_H