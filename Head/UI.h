/** 
 * @file UI.h
 * @brief 用户界面相关函数的声明
 * @details 该头文件声明了与用户界面交互相关的函数，包括登录、注册以及管理员的账户管理等功能
 * @author 郭承宇
 * @date 2026-4-8
 * @version 1.0
 */


#ifndef UI_H
#define UI_H

#include <iostream>
#include <string>
#include <limits>
#include <algorithm>
#include <sstream>
#include <vector>

void pause(); // 暂停函数，等待用户按键继续

//  ======================================== 输入校验函数区域 =======================================

int selectIntCheck(const int min, const int max); // 检查输入的选择是否在指定范围内
double inputFeeCheck(); // 检查输入的费用是否为有效的正数
std::string inputStringCheck(const std::string &prompt); // 检查输入的字符串是否符合要求（如非空等）
std::string inputIDCheck(const std::string &prompt); // 检查输入的用户ID是否符合格式要求（如长度、前缀等）
std::string inputRecordIDCheck(const std::string &prompt); // 检查输入的记录ID是否符合格式要求（如长度、前缀等）
std::string inputPwdCheck(const std::string &prompt); // 检查输入的密码是否符合安全要求（如长度、复杂度等）





// ======================================== 菜单显示函数区域 =======================================

int loginRegisterUI(); // 登录和注册选择界面

int beginUI(); // 启动界面，显示欢迎信息和主菜单

int adminMenu(); // 管理员菜单，提供账户管理和医疗记录管理选项

std::string adminDepartmentMenu(); // 管理员科室管理菜单

int adminMedicalRecordMenu(); // 管理员医疗记录管理菜单

int adminRegistrationManagementMenu(); // 管理员挂号记录管理菜单

int adminRegistrationViewMenu(); // 管理员挂号记录查看方式选择菜单




#endif