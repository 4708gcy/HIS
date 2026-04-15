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

void pause();                             // 暂停函数，等待用户按键继续
std::string trim(const std::string &str); // 去除字符串首尾空格的辅助函数

//  ======================================== 输入校验函数区域 =======================================

int selectIntCheck(const int min, const int max);                                                                                         // 检查输入的选择是否在指定范围内
double inputFeeCheck(const std::string &prompt);                                                                                          // 检查输入的费用是否为有效的正数
std::string inputStringCheck(const std::string &prompt);                                                                                  // 检查输入的字符串是否符合要求（如非空等）
std::string inputIDCheck(const std::string &prompt);                                                                                      // 检查输入的用户ID是否符合格式要求（如长度、前缀等）
std::string inputRecordIDCheck(const std::string &prompt, const std::vector<std::string> &validPrefixes);                                 // 检查输入的记录ID是否符合格式要求（如长度、前缀等）
std::string inputPwdCheck(const std::string &prompt);                                                                                     // 检查输入的密码是否符合安全要求（如长度、复杂度等）
std::string inputBedNumberCheck(const std::string &prompt, std::string department, std::string wardType);                                 // 检查输入的床位号是否符合格式要求（如非空、特定格式等）
std::string autoGenerateBedID(const std::string &department, const std::string &wardType, int areaNumber, int wardNumber, int bedNumber); // 根据输入信息自动生成床位ID
std::string inputGenderCheck(const std::string &prompt);                                                                                  // 检查输入的性别是否为有效选项（如男、女、其他等）
std::string inputTelephoneCheck(const std::string &prompt);                                                                               // 检查输入的电话号码是否符合格式要求（如长度、数字等）
std::string inputEmailCheck(const std::string &prompt);                                                                                   // 检查输入的邮箱地址是否符合格式要求（如包含@和.等）
int inputAgeCheck(const std::string &prompt);                                                                                             // 检查输入的年龄是否为有效的正整数
std::string inputDateCheck(const std::string &prompt);                                                                                    // 检查输入的日期是否符合格式要求（如YYYY-MM-DD等）
std::string inputDepartmentCheck(const std::string &prompt);                                                                              // 检查输入的科室是否为有效选项（如内科、外科等）

// ======================================== 菜单显示函数区域 =======================================

int loginRegisterUI(); // 登录和注册选择界面

int beginUI(); // 启动界面，显示欢迎信息和主菜单

int adminMenu(); // 管理员菜单，提供账户管理和医疗记录管理选项

std::string adminDepartmentMenu(); // 管理员科室管理菜单

// ======================================== 医疗记录管理菜单区域 =======================================

int adminMedicalRecordMenu(); // 管理员医疗记录管理菜单

int adminRegistrationManagementMenu(); // 管理员挂号记录管理菜单
int adminRegistrationViewMenu();       // 管理员挂号记录查看方式选择菜单

int adminConsultationManagementMenu(); // 管理员看诊记录管理菜单
int adminConsultationViewMenu();       // 管理员看诊记录查看方式选择菜单
int adminExaminationManagementMenu();  // 管理员检查记录管理菜单
int adminExaminationViewMenu();        // 管理员检查记录查看方式选择菜单
std::string ExaminationItemMenu();     // 检查项目选择菜单

int adminHospitalizationManagementMenu();  // 管理员住院记录管理菜单
int adminHospitalizationViewMenu();        // 管理员住院记录查看方式选择菜单
std::string HospitalizationWardTypeMenu(); // 住院记录病房类型选择菜单
int bedManagementMenu();                   // 床位管理菜单
int bedViewMenu();                         // 床位查看方式选择菜单

int adminMedicationRecordManagementMenu(); // 管理员药物记录管理菜单
int adminMedicationRecordViewMenu();       // 管理员药物记录查看方式选择菜单
int MedicationRecordReviewResultMenu();    // 用药审核结果选择菜单
int MedicationRecordStatusMenu();          // 用药状态选择菜单

int adminMedicineManagementMenu(); // 管理员药品信息管理菜单
int adminMedicineViewMenu();       // 管理员药品信息查看方式选择菜单
int MedicineStatusMenu();          // 药品状态选择菜单
int MedicineModificationMenu();    // 药品信息修改菜单

// ========================================= 管理员账户管理菜单区域 =======================================

int adminUserManagementMenu(); // 管理员用户信息管理菜单

int adminDoctorManagementMenu();   // 管理员医生信息管理菜单
int adminDoctorViewMenu();         // 管理员医生信息查看方式选择菜单
int DoctorTitleMenu();             // 医生职称选择菜单
int adminDoctorModificationMenu(); // 管理员医生信息修改菜单

#endif