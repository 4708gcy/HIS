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
#include <fstream>
#include <mutex>

void pause(const std::string &breadcrumb = ""); // 暂停函数，等待用户按键继续（支持面包屑导航）
std::string trim(const std::string &str);       // 去除字符串首尾空格的辅助函数
void clearScreen();                              // 清屏函数
int getDisplayWidth(const std::string &str);     // 获取字符串显示宽度（CJK字符占2列）

// ======================================== 终端颜色与工具函数区域 =======================================

enum class ConsoleColor { RED, GREEN, YELLOW, CYAN, WHITE, DEFAULT };
void setConsoleColor(ConsoleColor color); // 设置终端文本颜色
void resetConsoleColor();                 // 重置终端文本颜色
void printTitle(const std::string &title); // 打印带颜色的标题分隔线
void printSuccess(const std::string &msg); // 打印成功消息（绿色）
void printError(const std::string &msg);   // 打印错误消息（红色）
void printWarning(const std::string &msg); // 打印警告消息（黄色）
void printInfoCard(const std::string &title, const std::vector<std::pair<std::string, std::string>> &fields); // 打印个人信息卡片
void printMenuBorder();                                        // 打印菜单顶部边框（Unicode双线）
void printMenuBottom();                                        // 打印菜单底部边框
void printMenuTitle(const std::string &title);                 // 打印居中标题（CJK宽度感知）
void printMenuItem(int num, const std::string &text);          // 打印格式化菜单项（自动对齐）
void printMenuLine(const std::string &text);                   // 打印无编号的菜单行

// ======================================== 分页显示工具 =======================================================================

void printWithPagination(const std::vector<std::string> &lines, int pageSize = 10); // 分页打印字符串列表，返回是否被用户中断

// ======================================== 时间范围工具 =====================================================================

// 检查时间戳是否在指定范围内（格式: "YYYY-MM-DD hh:mm:ss"）
bool isTimestampInRange(const std::string &timestamp, const std::string &startTime, const std::string &endTime);
// 获取当前月的起始时间 "YYYY-MM-01 00:00:00"
std::string getCurrentMonthStart();
// 获取近 N 个月的起始时间
std::string getMonthsAgoStart(int monthsAgo);

int timeRangeMenu(); // 时间范围选择菜单（本月/近三月/近半年/自定义/全部）

// ======================================== 操作日志系统 =====================================================================

class LogManager {
public:
    static LogManager &getInstance();
    void info(const std::string &msg);
    void warn(const std::string &msg);
    void error(const std::string &msg);
    void logOperation(const std::string &userId, const std::string &role, const std::string &operation, const std::string &detail);

private:
    LogManager();
    std::string logFilePath;
    std::mutex logMutex;
    void writeLog(const std::string &level, const std::string &msg);
};

// ======================================== 操作日志系统 =====================================================================

//  ======================================== 输入校验函数区域 =======================================

std::string padId(int counter, int digits); // 安全的零填充ID生成，防止计数器溢出
bool deptMatch(const std::string &entityDept, const std::string &filter); // 科室匹配，支持"全院"过滤

int selectIntCheck(const int min, const int max);                                                                                         // 检查输入的选择是否在指定范围内
double inputFeeCheck(const std::string &prompt);                                                                                          // 检查输入的费用是否为有效的正数
std::string inputStringCheck(const std::string &prompt);                                                                                  // 检查输入的字符串是否符合要求（如非空等）
std::string inputIDCheck(const std::string &prompt);                                                                                      // 检查输入的用户ID是否符合格式要求（如长度、前缀等）
std::string inputRecordIDCheck(const std::string &prompt, const std::vector<std::string> &validPrefixes);                                 // 检查输入的记录ID是否符合格式要求（如长度、前缀等）
std::string inputPwdCheck(const std::string &prompt);                                                                                     // 检查输入的密码是否符合安全要求（如长度、复杂度等）
std::string inputHiddenPwdCheck(const std::string &prompt);                                                                               // 隐藏输入密码（用于登录场景）
std::string inputBedNumberCheck(const std::string &prompt, std::string department, std::string wardType);                                 // 检查输入的床位号是否符合格式要求（如非空、特定格式等）
std::string autoGenerateBedID(const std::string &department, const std::string &wardType, int areaNumber, int wardNumber, int bedNumber); // 根据输入信息自动生成床位ID
std::string inputGenderCheck(const std::string &prompt);                                                                                  // 检查输入的性别是否为有效选项（如男、女、其他等）
std::string inputTelephoneCheck(const std::string &prompt);                                                                               // 检查输入的电话号码是否符合格式要求（如长度、数字等）
std::string inputEmailCheck(const std::string &prompt);                                                                                   // 检查输入的邮箱地址是否符合格式要求（如包含@和.等）
int inputAgeCheck(const std::string &prompt);                                                                                             // 检查输入的年龄是否为有效的正整数
std::string inputDateCheck(const std::string &prompt);                                                                                    // 检查输入的日期是否符合格式要求（如YYYY-MM-DD等）
std::string inputDepartmentCheck(const std::string &prompt);                                                                              // 检查输入的科室是否为有效选项（如内科、外科等）
std::string inputIDcardCheck(const std::string &prompt);                                                                                  // 检查输入的身份证号是否符合格式要求（如长度、数字等）
int inputIntCheck(const std::string &prompt, int min, int max);                                                                           // 检查输入的整数是否为有效的整数
double inputDoubleCheck(const std::string &prompt, double min, double max);                                                               // 检查输入的浮点数是否为有效的浮点数

// ======================================== 菜单显示函数区域 =======================================

int loginRegisterUI(); // 登录和注册选择界面

int beginUI(); // 启动界面，显示欢迎信息和主菜单

int adminMenu(); // 管理员菜单，提供账户管理和医疗记录管理选项
int adminReportMenu(); // 管理员统计报表菜单

std::string adminDepartmentMenu(); // 管理员科室管理菜单

int identitySelectionMenu(); // 身份选择菜单，提供管理员、医生、护士、药师、患者等选项

// ======================================== 管理员医疗记录管理菜单区域 =======================================

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
int adminMedicineFlowManagementMenu(); // 药品出入库流水管理菜单
int adminMedicineFlowViewMenu();       // 药品流水查看方式选择菜单
int MedicineFlowTypeMenu();            // 流水类型选择（入库/出库）
int MedicineFlowReasonMenu();          // 流水原因选择菜单

// ========================================= 管理员账户管理菜单区域 =======================================

int adminUserManagementMenu(); // 管理员用户信息管理菜单

int adminDoctorManagementMenu();   // 管理员医生信息管理菜单
int adminDoctorViewMenu();         // 管理员医生信息查看方式选择菜单
int DoctorTitleMenu();             // 医生职称选择菜单
int adminDoctorModificationMenu(); // 管理员医生信息修改菜单

int adminNurseManagementMenu();   // 管理员护士信息管理菜单
int adminNurseViewMenu();         // 管理员护士信息查看方式选择菜单
int NurseTitleMenu();             // 护士职称选择菜单
int adminNurseModificationMenu(); // 管理员护士信息修改菜单

int adminPharmacistManagementMenu();   // 管理员药师信息管理菜单
int adminPharmacistViewMenu();         // 管理员药师信息查看方式选择菜单
int PharmacistTitleMenu();             // 药师职称选择菜单
int adminPharmacistModificationMenu(); // 管理员药师信息修改菜单

int adminPatientManagementMenu();   // 管理员患者信息管理菜单
int adminPatientViewMenu();         // 管理员患者信息查看方式选择菜单
int PatientMaritalStatusMenu();     // 患者婚姻状态选择菜单
int adminPatientModificationMenu(); // 管理员患者信息修改菜单

int adminAdminManagementMenu();   // 管理员信息管理菜单
int adminAdminViewMenu();         // 管理员信息查看方式选择菜单
int adminAdminModificationMenu(); // 管理员信息修改菜单

int adminPersonalInfoManagementMenu();   // 管理员个人信息管理菜单
int adminPersonalInfoViewMenu();         // 管理员个人信息查看选择菜单
int adminPersonalInfoModificationMenu(); // 管理员个人信息修改选择菜单

// ========================================= 医生功能菜单区域 =======================================
int doctorMenu(); // 医生菜单，提供看诊记录管理、挂号记录管理等选项

int doctorRegistrationManagementMenu(); // 医生挂号记录管理菜单
int doctorRegistrationViewMenu();       // 医生挂号记录查看方式选择菜单

int doctorConsultationManagementMenu();   // 医生看诊记录管理菜单
int doctorConsultationViewMenu();         // 医生看诊记录查看方式选择菜单
int doctorConsultationModificationMenu(); // 医生看诊记录修改菜单

int doctorExaminationManagementMenu();   // 医生检查记录管理菜单
int doctorExaminationViewMenu();         // 医生检查记录查看方式选择菜单
int doctorExaminationModificationMenu(); // 医生检查记录修改菜单

int doctorPersonalInfoManagementMenu();   // 医生个人信息管理菜单
int doctorPersonalInfoViewMenu();         // 医生个人信息查看选择菜单
int doctorPersonalInfoModificationMenu(); // 医生个人信息修改选择菜单

// ========================================= 护士功能菜单区域 =======================================
int nurseMenu(); // 护士菜单，提供住院记录管理、床位管理等选项

int nurseExaminationManagementMenu();   // 护士检查记录管理菜单
int nurseExaminationViewMenu();         // 护士检查记录查看方式选择菜单
int nurseExaminationModificationMenu(); // 护士检查记录修改菜单

int nurseHospitalizationManagementMenu();   // 护士住院记录管理菜单
int nurseHospitalizationViewMenu();         // 护士住院记录查看方式选择菜单
int nurseHospitalizationModificationMenu(); // 护士住院记录修改菜单

int nurseBedManagementMenu();   // 护士床位管理菜单
int nurseBedViewMenu();         // 护士床位查看方式选择菜单
int nurseBedModificationMenu(); // 护士床位修改菜单

int nursePersonalInfoManagementMenu();   // 护士个人信息管理菜单
int nursePersonalInfoViewMenu();         // 护士个人信息查看选择菜单
int nursePersonalInfoModificationMenu(); // 护士个人信息修改选择菜单

// ========================================= 药师功能菜单区域 =======================================
int pharmacistMenu(); // 药师菜单，提供药物记录管理、药品信息管理等选项

int pharmacistMedicationRecordManagementMenu();   // 药师用药记录管理菜单
int pharmacistMedicationRecordViewMenu();         // 药师用药记录查看方式选择菜单
int pharmacistMedicationRecordModificationMenu(); // 药师用药记录修改菜单

int pharmacistMedicineManagementMenu();   // 药师药品信息管理菜单
int pharmacistMedicineViewMenu();         // 药师药品信息查看方式选择菜单
int pharmacistMedicineModificationMenu(); // 药师药品信息修改菜单

int pharmacistPersonalInfoManagementMenu();   // 药师个人信息管理菜单
int pharmacistPersonalInfoViewMenu();         // 药师个人信息查看选择菜单
int pharmacistPersonalInfoModificationMenu(); // 药师个人信息修改选择菜单

// ========================================= 患者功能菜单区域 =======================================
int patientMenu(); // 患者菜单，提供个人信息管理、挂号记录查看等选项

int patientRegistrationManagementMenu(); // 患者挂号记录管理菜单
int patientRegistrationViewMenu();       // 患者挂号记录查看方式选择菜单

int patientConsultationManagementMenu(); // 患者看诊记录管理菜单
int patientConsultationViewMenu();       // 患者看诊记录查看方式选择菜单

int patientExaminationManagementMenu(); // 患者检查记录管理菜单
int patientExaminationViewMenu();       // 患者检查记录查看方式选择菜单

int patientMedicationManagementMenu(); // 患者用药记录管理菜单
int patientMedicationViewMenu();       // 患者用药记录查看方式选择菜单

int patientHospitalizationManagementMenu(); // 患者住院记录管理菜单
int patientHospitalizationViewMenu();       // 患者住院记录查看方式选择菜单

int patientPersonalInfoManagementMenu();   // 患者个人信息管理菜单
int patientPersonalInfoViewMenu();         // 患者个人信息查看选择菜单
int patientPersonalInfoModificationMenu(); // 患者个人信息修改选择菜单

#endif