/**
 * @file Doctor.cpp
 * @brief 医疗管理系统中的医生类实现
 * @details 该源文件实现了 Doctor 类的构造、析构、文件操作方法以及医生业务接口。
 * @author 李阳旭 / 小组成员 郭承宇 / 小组成员
 * @date 2026-4-15
 * @version 1.0
 */

#include "../Head/Doctor.h"

Doctor::Doctor()
{
    // 构造函数实现（如有需要）
}

Doctor::~Doctor()
{
}

bool Doctor::doctorSignUp(int &idCounter)
{
    bool success = signUp(2, idCounter); // 调用基类的注册方法，传入角色类型 2（医生）
    if (!success)
    {
        printError("医生注册失败！");
        return false;
    }
    this->doctorID = this->userID; // 医生ID与用户ID保持一致

    // 其他医生特有信息的初始化在这里进行
    this->department = inputDepartmentCheck("请输入所属科室: ");

    std::cout << "请选择医生职称:" << std::endl;
    std::cout << "1. 实习医师" << std::endl;
    std::cout << "2. 住院医师" << std::endl;
    std::cout << "3. 主治医师" << std::endl;
    std::cout << "4. 副主任医师" << std::endl;
    std::cout << "5. 主任医师" << std::endl;
    int titleChoice = selectIntCheck(1, 5);
    this->title = static_cast<DoctorTitle>(titleChoice);

    this->specialty = inputStringCheck("请输入擅长方向: ");
    this->scheduleInfo = inputStringCheck("请输入排班信息: ");

    std::cout << "请输入你现在是否在岗 (1-是, 0-否): " << std::endl;
    int onDutyChoice = selectIntCheck(0, 1);
    this->isOnDuty = (onDutyChoice == 1);

    printSuccess("医生注册成功! 您的用户ID是: " + this->userID);
    return true;
}

bool Doctor::doctorSignIn()
{
    if (isAccountActive == false)
    {
        printError("账户已锁定，请联系系统管理员解锁！");
        return false;
    }

    while (loginAttempts < kMaxLoginAttempts)
    {
        std::string pwd = inputHiddenPwdCheck("请输入密码(输入\"quit\"退出登录): ");

        if (pwd == "quit")
        {
            printWarning("退出登录。");
            return false;
        }

        bool success = SHA256Verify(pwd, storedHash, kHashIterations);

        if (success)
        {
            loginAttempts = 0;
            isLoggedIn = true;
            printSuccess("医生登录成功！");
            return true;
        }
        else
        {
            loginAttempts++;
            printError("密码错误! 请重新输入密码(当前失败次数: " + std::to_string(loginAttempts) + ")");

            if (loginAttempts >= kMaxLoginAttempts)
            {
                isAccountActive = false;
                printError("连续登录失败次数过多，账户已锁定，请联系系统管理员解锁！");
            }
        }
    }

    return false; // 登录失败
}

const std::string &Doctor::getDoctorID() const
{
    return doctorID;
}
const std::string &Doctor::getDepartment() const
{
    return department;
}
DoctorTitle Doctor::getTitle() const
{
    return title;
}
const std::string &Doctor::getSpecialty() const
{
    return specialty;
}
const std::string &Doctor::getScheduleInfo() const
{
    return scheduleInfo;
}
int Doctor::getConsultationCount() const
{
    return consultationCount;
}
int Doctor::getExaminationCount() const
{
    return examinationCount;
}
int Doctor::getHospitalizationApplyCount() const
{
    return hospitalizationApplyCount;
}
bool Doctor::getIsOnDuty() const
{
    return isOnDuty;
}

void Doctor::setDoctorID(const std::string &id)
{
    doctorID = id;
}
void Doctor::setDepartment(const std::string &dept)
{
    department = dept;
}
void Doctor::setTitle(DoctorTitle doctorTitle)
{
    title = doctorTitle;
}
void Doctor::setSpecialty(const std::string &spec)
{
    specialty = spec;
}
void Doctor::setScheduleInfo(const std::string &schedule)
{
    scheduleInfo = schedule;
}
void Doctor::setConsultationCount(int count)
{
    consultationCount = count;
}
void Doctor::setExaminationCount(int count)
{
    examinationCount = count;
}
void Doctor::setHospitalizationApplyCount(int count)
{
    hospitalizationApplyCount = count;
}
void Doctor::setIsOnDuty(bool onDuty)
{
    isOnDuty = onDuty;
}

// ========================================= 医生业务接口 ==========================================

// ------------------------------------ 挂号记录相关操作 ----------------------------------

// 获取医生的是所有挂号记录
bool Doctor::getAllRegistrations(Registration *&regHead)
{
    std::cout << "正在获取所有挂号记录..." << std::endl;

    Registration *current = regHead;
    bool found = false;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->doctorID == this->doctorID)
        {
            std::cout << "挂号ID: " << current->registrationID
                      << ", 患者ID: " << current->patientID
                      << ", 科室: " << current->department
                      << ", 挂号时间: " << current->registerTime
                      << ", 费用: " << current->fee
                      << ", 状态: " << regStatusToString(current->status)
                      << ", 备注: " << current->note
                      << std::endl;
            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "没有找到任何挂号记录。" << std::endl;
    }
    return found;
}
// 根据挂号状态获取挂号记录
bool Doctor::getRegistrationsByStatus(Registration *&regHead)
{
    std::cout << "请输入要查询的挂号状态 (0-已预约, 1-已支付, 2-已取消, 3-已完成): ";
    int statusChoice = selectIntCheck(0, 3);
    RegistrationStatus targetStatus = static_cast<RegistrationStatus>(statusChoice);

    std::cout << "正在获取挂号状态为 " << regStatusToString(targetStatus) << " 的记录..." << std::endl;

    Registration *current = regHead;
    bool found = false;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->doctorID == this->doctorID && current->status == targetStatus)
        {
            std::cout << "挂号ID: " << current->registrationID
                      << ", 患者ID: " << current->patientID
                      << ", 科室: " << current->department
                      << ", 挂号时间: " << current->registerTime
                      << ", 费用: " << current->fee
                      << ", 状态: " << regStatusToString(current->status)
                      << ", 备注: " << current->note
                      << std::endl;
            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "没有找到状态为 " << regStatusToString(targetStatus) << " 的挂号记录。" << std::endl;
    }
    return found;
}
// 根据患者ID获取挂号记录
bool Doctor::getRegistrationsByPatientID(Registration *&regHead)
{
    std::string patientID = inputIDCheck("请输入要查询的患者ID: ");

    std::cout << "正在获取患者ID为 " << patientID << " 的挂号记录..." << std::endl;

    Registration *current = regHead;
    bool found = false;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->doctorID == this->doctorID && current->patientID == patientID)
        {
            std::cout << "挂号ID: " << current->registrationID
                      << ", 患者ID: " << current->patientID
                      << ", 科室: " << current->department
                      << ", 挂号时间: " << current->registerTime
                      << ", 费用: " << current->fee
                      << ", 状态: " << regStatusToString(current->status)
                      << ", 备注: " << current->note
                      << std::endl;
            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "没有找到患者ID为 " << patientID << " 的挂号记录。" << std::endl;
    }
    return found;
}
// 根据挂号ID获取挂号记录
bool Doctor::getRegistrationsByID(Registration *&regHead)
{
    std::string regID = inputRecordIDCheck("请输入要查询的挂号ID: ", {"reg"});

    std::cout << "正在获取挂号ID为 " << regID << " 的挂号记录..." << std::endl;

    Registration *current = regHead;
    bool found = false;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->doctorID == this->doctorID && current->registrationID == regID)
        {
            std::cout << "挂号ID: " << current->registrationID
                      << ", 患者ID: " << current->patientID
                      << ", 科室: " << current->department
                      << ", 挂号时间: " << current->registerTime
                      << ", 费用: " << current->fee
                      << ", 状态: " << regStatusToString(current->status)
                      << ", 备注: " << current->note
                      << std::endl;
            found = true;
            break; // 按ID查询应该只有一条记录，找到后可以退出循环
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到挂号ID为 " << regID << " 的挂号记录。" << std::endl;
    }
    return found;
}
// 根据挂号日期范围获取挂号记录（只支持到天）
bool Doctor::getRegistrationsByTimeRange(Registration *&regHead)
{
    std::string startDate = inputDateCheck("请输入查询的开始日期 (格式 YYYY-MM-DD): ");
    std::string endDate = inputDateCheck("请输入查询的结束日期 (格式 YYYY-MM-DD): ");

    std::cout << "正在获取挂号日期在 " << startDate << " 到 " << endDate << " 之间的记录..." << std::endl;

    Registration *current = regHead;
    bool found = false;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->doctorID == this->doctorID)
        {
            // 只取日期部分进行比较
            std::string regDate = current->registerTime.substr(0, 10);
            if (regDate >= startDate && regDate <= endDate)
            {
                std::cout << "挂号ID: " << current->registrationID
                          << ", 患者ID: " << current->patientID
                          << ", 科室: " << current->department
                          << ", 挂号时间: " << current->registerTime
                          << ", 费用: " << current->fee
                          << ", 状态: " << regStatusToString(current->status)
                          << ", 备注: " << current->note
                          << std::endl;
                found = true;
            }
        }
        current = current->next;
    }

    if (!found)
    {
        printWarning("没有找到挂号日期在指定范围内的记录。");
    }
    return found;
}
// 修改挂号记录状态
void Doctor::setRegistrationStatus(Registration *&target)
{
    std::cout << "当前挂号状态: " << regStatusToString(target->status) << std::endl;
    std::cout << "请输入新的挂号状态 (0-已预约, 1-已支付, 2-已取消, 3-已完成): ";
    int statusChoice = selectIntCheck(0, 3);
    target->status = static_cast<RegistrationStatus>(statusChoice);
    printSuccess("挂号状态已更新为: " + regStatusToString(target->status));
}
// 删除挂号记录（逻辑删除）
void Doctor::deleteRegistration(Registration *&target)
{
    target->isDeleted = true; // 逻辑删除
    printSuccess("挂号记录已删除！");
}
// 为患者创建新的挂号记录
bool Doctor::createRegistrationByPatient(Registration *&regHead, Doctor *&doctor, Patient *&patient, int &idCounter)
{
    std::string patientID = inputIDCheck("请输入要挂号的患者ID: ");

    Patient *currentPatient = patient;
    while (currentPatient != nullptr)
    {
        if (!currentPatient->isDeleted && currentPatient->patientID == patientID)
        {
            Doctor *doc = doctor;
            bool foundOnDutyDoctor = false;

            std::cout << "正在查找同科室在岗医生信息..." << std::endl;
            while (doc != nullptr)
            {
                if (!doc->isDeleted && doc->isOnDuty && doc->department == this->department)
                {
                    std::cout << "ID: " << doc->doctorID
                              << ", 姓名: " << doc->username
                              << ", 职称: " << doctorTitleToString(doc->title)
                              << ", 擅长: " << doc->specialty
                              << std::endl;
                    foundOnDutyDoctor = true;
                }
                doc = doc->next;
            }

            if (!foundOnDutyDoctor)
            {
                printError("未找到同科室在岗医生，无法为患者挂号！");
                return false;
            }

            std::string assignedDoctorID = inputIDCheck("请输入要分配的医生ID: ");

            doc = doctor;

            while (doc != nullptr)
            {
                if (!doc->isDeleted && doc->isOnDuty && doc->department == this->department && doc->doctorID == assignedDoctorID)
                {
                    Registration *newReg = new Registration();
                    newReg->registrationID = "reg" + std::to_string(idCounter).insert(0, 6 - std::to_string(idCounter).length(), '0'); // 生成新的挂号ID，格式为 reg000001
                    idCounter++;
                    newReg->patientID = patientID;

                    currentPatient->department = this->department; // 将患者的就诊科室设置为医生所在科室

                    newReg->department = this->department;

                    MyTime &t = MyTime::getInstance();
                    newReg->registerTime = t.getTime();
                    newReg->fee = calculateRegistrationFee(doc->title);
                    newReg->status = RegistrationStatus::BOOKED;

                    newReg->note = inputStringCheck("请输入挂号备注信息: ");

                    // 插入挂号记录到链表头部
                    newReg->next = regHead;
                    if (regHead)
                        regHead->prev = newReg;
                    regHead = newReg;

                    printSuccess("挂号成功！新挂号ID: " + newReg->registrationID);
                    return true;
                }
                doc = doc->next;
            }
            printError("未找到指定的医生，无法为患者挂号！");
            return false;
        }
        currentPatient = currentPatient->next;
    }

    printError("未找到指定的患者信息！");
    return false;
}

// 管理挂号记录（查看、修改状态等）
void Doctor::manageRegistrations(Registration *&regHead, Doctor *&doctor, Patient *&patientHead, int &idCounter)
{
    while (true)
    {
        int choice = doctorRegistrationManagementMenu();
        if (choice == 0)
        {
            break;
        }
        else if (choice == 1)
        {
            while (true)
            {
                int viewChoice = doctorRegistrationViewMenu();
                if (viewChoice == 0)
                {
                    break;
                }
                else if (viewChoice == 1)
                {
                    getAllRegistrations(regHead);
                    pause("医生 > 挂号管理");
                }
                else if (viewChoice == 2)
                {
                    getRegistrationsByID(regHead);
                    pause("医生 > 挂号管理");
                }
                else if (viewChoice == 3)
                {
                    getRegistrationsByPatientID(regHead);
                    pause("医生 > 挂号管理");
                }
                else if (viewChoice == 4)
                {
                    getRegistrationsByTimeRange(regHead);
                    pause("医生 > 挂号管理");
                }
                else if (viewChoice == 5)
                {
                    getRegistrationsByStatus(regHead);
                    pause("医生 > 挂号管理");
                }
            }
        }
        else if (choice == 2)
        {
            bool ishave = getAllRegistrations(regHead);
            if (ishave)
            {
                std::string regID = inputRecordIDCheck("请输入要修改状态的挂号ID: ", {"reg"});
                Registration *target = regHead;
                while (target)
                {
                    if (!target->isDeleted && target->registrationID == regID && target->doctorID == this->doctorID)
                    {
                        break;
                    }
                    target = target->next;
                }
                if (target)
                {
                    while (true)
                    {
                        std::cout << "正在修改的挂号记录信息如下：" << std::endl;

                        std::cout << "挂号ID: " << target->registrationID
                                  << ", 患者ID: " << target->patientID
                                  << ", 科室: " << target->department
                                  << ", 挂号时间: " << target->registerTime
                                  << ", 费用: " << target->fee
                                  << ", 状态: " << regStatusToString(target->status)
                                  << ", 备注: " << target->note
                                  << std::endl;

                        std::cout << "请选择你的操作: " << std::endl;
                        std::cout << "1. 修改挂号状态" << std::endl;
                        std::cout << "0. 返回上一级" << std::endl;

                        int modifyChoice = selectIntCheck(0, 1);

                        if (modifyChoice == 1)
                        {
                            setRegistrationStatus(target);
                            pause("医生 > 挂号管理");
                        }
                        else
                        {
                            break;
                        }
                    }
                }
                else
                {
                    printError("未找到挂号ID为 " + regID + " 的挂号记录，无法修改状态！");
                    pause("医生 > 挂号管理");
                }
            }
        }
        else if (choice == 3)
        {
            bool ishave = getAllRegistrations(regHead);
            if (ishave)
            {
                std::string regID = inputRecordIDCheck("请输入要删除的挂号ID: ", {"reg"});
                Registration *target = regHead;
                while (target)
                {
                    if (!target->isDeleted && target->registrationID == regID && target->doctorID == this->doctorID)
                    {
                        break;
                    }
                    target = target->next;
                }
                if (target)
                {
                    std::cout << "正在删除的挂号记录信息如下：" << std::endl;

                    std::cout << "挂号ID: " << target->registrationID
                              << ", 患者ID: " << target->patientID
                              << ", 科室: " << target->department
                              << ", 挂号时间: " << target->registerTime
                              << ", 费用: " << target->fee
                              << ", 状态: " << regStatusToString(target->status)
                              << ", 备注: " << target->note
                              << std::endl;

                    std::cout << "确认要删除这条挂号记录吗？: " << std::endl;
                    std::cout << "1. 确认" << std::endl;
                    std::cout << "0. 取消" << std::endl;

                    int confirmChoice = selectIntCheck(0, 1);

                    if (confirmChoice == 1)
                    {
                        deleteRegistration(target);
                        pause("医生 > 挂号管理");
                    }
                    else
                    {
                        printWarning("已取消删除操作。");
                        pause("医生 > 挂号管理");
                    }
                }
                else
                {
                    printError("未找到挂号ID为 " + regID + " 的挂号记录，无法删除！");
                    pause("医生 > 挂号管理");
                }
            }
        }
        else if (choice == 4)
        {
            createRegistrationByPatient(regHead, doctor, patientHead, idCounter); // 这里传入 nullptr，因为在 createRegistrationByPatient 内部会再次输入患者ID并查找患者信息
            pause("医生 > 挂号管理");
        }
    }
}

// ------------------------------------ 看诊记录相关操作 ----------------------------------

// 获取医生的所有看诊记录
bool Doctor::getAllConsultations(Consultation *&conHead)
{
    std::cout << "正在获取所有看诊记录..." << std::endl;

    Consultation *current = conHead;
    bool found = false;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->doctorID == this->doctorID)
        {
            std::cout << "看诊ID: " << current->consultationID
                      << ", 挂号ID: " << current->registrationID
                      << ", 患者ID: " << current->patientID
                      << ", 看诊时间: " << current->consultationTime
                      << ", 科室: " << current->department
                      << ", 主诉: " << current->chiefComplaint
                      << ", 现病史: " << current->historyOfPresentIllness
                      << ", 既往史: " << current->pastMedicalHistory
                      << ", 家族史: " << current->familyHistory
                      << ", 初步诊断: " << current->preliminaryDiagnosis
                      << ", 检查项目数: " << current->examinationlist.size()
                      << ", 处方数: " << current->prescriptions.size()
                      << ", 处方审核状态: " << (current->isPrecriptionReviewed ? "已审核" : "未审核")
                      << ", 住院建议: " << (current->isHospitalizationRecommended ? "是" : "否")
                      << ", 备注: " << current->note
                      << std::endl;
            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "没有找到任何看诊记录。" << std::endl;
    }
    return found;
}
// 根据看诊状态获取看诊记录
bool Doctor::getConsultationsByStatus(Consultation *&conHead)
{
    std::cout << "请输入要查询的看诊状态 (0-待就诊, 1-正在处理, 2-已结束, 3-已作废): ";
    int statusChoice = selectIntCheck(0, 3);
    ConsultationStatus targetStatus = static_cast<ConsultationStatus>(statusChoice);

    std::cout << "正在获取看诊状态为 " << conStatusToString(targetStatus) << " 的记录..." << std::endl;

    Consultation *current = conHead;
    bool found = false;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->doctorID == this->doctorID && current->status == targetStatus)
        {
            std::cout << "看诊ID: " << current->consultationID
                      << ", 挂号ID: " << current->registrationID
                      << ", 患者ID: " << current->patientID
                      << ", 看诊时间: " << current->consultationTime
                      << ", 科室: " << current->department
                      << ", 主诉: " << current->chiefComplaint
                      << ", 现病史: " << current->historyOfPresentIllness
                      << ", 既往史: " << current->pastMedicalHistory
                      << ", 家族史: " << current->familyHistory
                      << ", 初步诊断: " << current->preliminaryDiagnosis
                      << ", 检查项目数: " << current->examinationlist.size()
                      << ", 处方数: " << current->prescriptions.size()
                      << ", 处方审核状态: " << (current->isPrecriptionReviewed ? "已审核" : "未审核")
                      << ", 住院建议: " << (current->isHospitalizationRecommended ? "是" : "否")
                      << ", 备注: " << current->note
                      << std::endl;
            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "没有找到状态为 " << conStatusToString(targetStatus) << " 的看诊记录。" << std::endl;
    }
    return found;
}
// 根据患者ID获取看诊记录
bool Doctor::getConsultationsByPatientID(Consultation *&conHead)
{

    std::string patientID = inputIDCheck("请输入要查询的患者ID: ");

    std::cout << "正在获取患者ID为 " << patientID << " 的看诊记录..." << std::endl;

    Consultation *current = conHead;
    bool found = false;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->doctorID == this->doctorID && current->patientID == patientID)
        {
            std::cout << "看诊ID: " << current->consultationID
                      << ", 挂号ID: " << current->registrationID
                      << ", 患者ID: " << current->patientID
                      << ", 看诊时间: " << current->consultationTime
                      << ", 科室: " << current->department
                      << ", 主诉: " << current->chiefComplaint
                      << ", 现病史: " << current->historyOfPresentIllness
                      << ", 既往史: " << current->pastMedicalHistory
                      << ", 家族史: " << current->familyHistory
                      << ", 初步诊断: " << current->preliminaryDiagnosis
                      << ", 检查项目数: " << current->examinationlist.size()
                      << ", 处方数: " << current->prescriptions.size()
                      << ", 处方审核状态: " << (current->isPrecriptionReviewed ? "已审核" : "未审核")
                      << ", 住院建议: " << (current->isHospitalizationRecommended ? "是" : "否")
                      << ", 备注: " << current->note
                      << std::endl;
            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "没有找到患者ID为 " << patientID << " 的看诊记录。" << std::endl;
    }
    return found;
}
// 根据看诊时间范围获取看诊记录
bool Doctor::getConsultationsByTimeRange(Consultation *&conHead)
{
    std::string startDate = inputDateCheck("请输入查询的开始日期 (格式 YYYY-MM-DD): ");
    std::string endDate = inputDateCheck("请输入查询的结束日期 (格式 YYYY-MM-DD): ");

    std::cout << "正在获取看诊日期在 " << startDate << " 到 " << endDate << " 之间的记录..." << std::endl;

    Consultation *current = conHead;
    bool found = false;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->doctorID == this->doctorID)
        {
            // 只取日期部分进行比较
            std::string conDate = current->consultationTime.substr(0, 10);
            if (conDate >= startDate && conDate <= endDate)
            {
                std::cout << "看诊ID: " << current->consultationID
                          << ", 挂号ID: " << current->registrationID
                          << ", 患者ID: " << current->patientID
                          << ", 看诊时间: " << current->consultationTime
                          << ", 科室: " << current->department
                          << ", 主诉: " << current->chiefComplaint
                          << ", 现病史: " << current->historyOfPresentIllness
                          << ", 既往史: " << current->pastMedicalHistory
                          << ", 家族史: " << current->familyHistory
                          << ", 初步诊断: " << current->preliminaryDiagnosis
                          << ", 检查项目数: " << current->examinationlist.size()
                          << ", 处方数: " << current->prescriptions.size()
                          << ", 处方审核状态: " << (current->isPrecriptionReviewed ? "已审核" : "未审核")
                          << ", 住院建议: " << (current->isHospitalizationRecommended ? "是" : "否")
                          << ", 备注: " << current->note
                          << std::endl;
                found = true;
            }
        }
        current = current->next;
    }

    if (!found)
    {
        printWarning("没有找到看诊日期在指定范围内的记录。");
    }
    return found;
}
// 根据看诊ID获取看诊记录
bool Doctor::getConsultationsByID(Consultation *&conHead)
{
    std::string conID = inputRecordIDCheck("请输入要查询的看诊ID: ", {"con"});

    std::cout << "正在获取看诊ID为 " << conID << " 的看诊记录..." << std::endl;

    Consultation *current = conHead;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->doctorID == this->doctorID && current->consultationID == conID)
        {
            std::cout << "看诊ID: " << current->consultationID
                      << ", 挂号ID: " << current->registrationID
                      << ", 患者ID: " << current->patientID
                      << ", 看诊时间: " << current->consultationTime
                      << ", 科室: " << current->department
                      << ", 主诉: " << current->chiefComplaint
                      << ", 现病史: " << current->historyOfPresentIllness
                      << ", 既往史: " << current->pastMedicalHistory
                      << ", 家族史: " << current->familyHistory
                      << ", 初步诊断: " << current->preliminaryDiagnosis
                      << ", 检查项目数: " << current->examinationlist.size()
                      << ", 处方数: " << current->prescriptions.size()
                      << ", 处方审核状态: " << (current->isPrecriptionReviewed ? "已审核" : "未审核")
                      << ", 住院建议: " << (current->isHospitalizationRecommended ? "是" : "否")
                      << ", 备注: " << current->note
                      << std::endl;
            if (current->examinationlist.size() > 0)
            {
                std::cout << "检查项目列表:" << std::endl;
                for (const auto &exam : current->examinationlist)
                {
                    std::cout << "  - " << exam << std::endl;
                }
            }
            if (current->prescriptions.size() > 0)
            {
                std::cout << "处方列表:" << std::endl;
                for (const auto &pres : current->prescriptions)
                {
                    std::cout << "  - 药品ID: " << pres.medicineID
                              << ", 药品名称: " << pres.name
                              << ", 药品数量: " << pres.quantity
                              << ", 用量: " << pres.dosage
                              << ", 频次: " << pres.frequency
                              << ", 疗程: " << pres.duration
                              << ", 备注: " << pres.note
                              << std::endl;
                }
            }
            return true;
        }
        current = current->next;
    }

    std::cout << "未找到看诊ID为 " << conID << " 的看诊记录。" << std::endl;
    return false;
}
// 修改看诊记录状态
void Doctor::setConsultationStatus(Consultation *&target)
{
    std::cout << "当前看诊状态: " << conStatusToString(target->status) << std::endl;
    std::cout << "请输入新的看诊状态 (0-待就诊, 1-正在处理, 2-已结束, 3-已作废): ";
    int statusChoice = selectIntCheck(0, 3);
    target->status = static_cast<ConsultationStatus>(statusChoice);
    printSuccess("看诊状态已更新为: " + conStatusToString(target->status));
}
// 修改看诊记录的主诉信息
void Doctor::setConsultationChiefComplaint(Consultation *&target)
{
    std::cout << "当前主诉信息: " << target->chiefComplaint << std::endl;
    std::string newChiefComplaint = inputStringCheck("请输入新的主诉信息: ");
    target->chiefComplaint = newChiefComplaint;
    printSuccess("主诉信息已更新！");
}
// 修改看诊记录的现病史信息
void Doctor::setConsultationHistoryOfPresentIllness(Consultation *&target)
{
    std::cout << "当前现病史信息: " << target->historyOfPresentIllness << std::endl;
    std::string newHistory = inputStringCheck("请输入新的现病史信息: ");
    target->historyOfPresentIllness = newHistory;
    printSuccess("现病史信息已更新！");
}
// 修改看诊记录的既往史信息
void Doctor::setConsultationPastMedicalHistory(Consultation *&target)
{
    std::cout << "当前既往史信息: " << target->pastMedicalHistory << std::endl;
    std::string newHistory = inputStringCheck("请输入新的既往史信息: ");
    target->pastMedicalHistory = newHistory;
    printSuccess("既往史信息已更新！");
}
// 修改看诊记录的家族史信息
void Doctor::setConsultationFamilyHistory(Consultation *&target)
{
    std::cout << "当前家族史信息: " << target->familyHistory << std::endl;
    std::string newHistory = inputStringCheck("请输入新的家族史信息: ");
    target->familyHistory = newHistory;
    printSuccess("家族史信息已更新！");
}
// 修改看诊记录的初步诊断信息
void Doctor::setConsultationPreliminaryDiagnosis(Consultation *&target)
{
    std::cout << "当前初步诊断信息: " << target->preliminaryDiagnosis << std::endl;
    std::string newDiagnosis = inputStringCheck("请输入新的初步诊断信息: ");
    target->preliminaryDiagnosis = newDiagnosis;
    printSuccess("初步诊断信息已更新！");
}
// 向看诊记录添加检查项目（避免重复，类似 set 功能）
void Doctor::addConsultationExamination(Consultation *&target)
{
    while (true)
    {
        if (target->examinationlist.size() > 0)
        {
            std::cout << "当前已有的检查项目:" << std::endl;
            for (const auto &exam : target->examinationlist)
            {
                std::cout << exam << " ";
            }
            std::cout << std::endl;
        }

        std::string newExam = ExaminationItemMenu();

        if (newExam == "0")
        {
            break;
        }

        // 检查是否已存在
        bool exists = false;
        for (const auto &exam : target->examinationlist)
        {
            if (exam == newExam)
            {
                exists = true;
                break;
            }
        }

        if (exists)
        {
            printError("该检查项目已存在，不能重复添加！");
        }
        else
        {
            target->examinationlist.push_back(newExam);
            printSuccess("检查项目信息已添加！");
        }
    }
}
// 向看诊记录添加处方信息
void Doctor::addConsultationPrescription(Consultation *&target, Medicine *&medHead)
{
    while (true)
    {
        // 显示当前所有处方信息
        if (!target->prescriptions.empty())
        {
            std::cout << "当前已有的处方信息:" << std::endl;
            for (const auto &pres : target->prescriptions)
            {
                std::cout << "  - 药品ID: " << pres.medicineID
                          << ", 名称: " << pres.name
                          << ", 数量: " << pres.quantity
                          << ", 用量: " << pres.dosage
                          << ", 频次: " << pres.frequency
                          << ", 疗程: " << pres.duration
                          << ", 备注: " << pres.note << std::endl;
            }
        }

        Medicine *medCurrent = medHead;
        std::cout << "可用药品列表:" << std::endl;
        while (medCurrent != nullptr)
        {
            if (!medCurrent->isDeleted && medCurrent->status == MedicineStatus::NORMAL)
            {
                std::cout << "药品ID: " << medCurrent->medicineID << ", 名称: " << medCurrent->name << ", 规格: " << medCurrent->specification << std::endl;
            }
            medCurrent = medCurrent->next;
        }

        Prescription newPres;
        newPres.medicineID = inputRecordIDCheck("请输入要添加的药品ID: ", {"med"});

        // 检查是否已存在该药品ID
        bool exists = false;
        for (const auto &pres : target->prescriptions)
        {
            if (pres.medicineID == newPres.medicineID)
            {
                exists = true;
                break;
            }
        }

        if (exists)
        {
            printError("该药品已存在于处方中，不能重复添加！");
        }
        else
        {
            medCurrent = medHead;
            bool medFound = false;
            while (medCurrent != nullptr)
            {
                if (!medCurrent->isDeleted && medCurrent->medicineID == newPres.medicineID && medCurrent->status == MedicineStatus::NORMAL)
                {
                    newPres.name = medCurrent->name; // 自动填充药品名称
                    std::cout << "请输入要使用的药品数量：" << std::endl;
                    newPres.quantity = selectIntCheck(1, 10000);

                    newPres.dosage = inputStringCheck("请输入用量信息: ");
                    newPres.frequency = inputStringCheck("请输入频次信息: ");
                    newPres.duration = inputStringCheck("请输入疗程信息: ");
                    newPres.note = inputStringCheck("请输入处方备注信息: ");

                    printSuccess("处方信息已添加！");

                    target->prescriptions.push_back(newPres);
                    medFound = true;
                    break;
                }
                medCurrent = medCurrent->next;
            }

            if (!medFound)
            {
                printError("未找到指定的药品！");
            }
        }

        std::cout << "是否继续添加？(1-继续, 0-结束): ";
        int choice = selectIntCheck(0, 1);

        if (choice != 1)
            break;
    }
}
// 设置看诊记录的住院建议
void Doctor::setHospitalizationRecommendation(Consultation *&target)
{
    std::cout << "当前住院建议: " << (target->isHospitalizationRecommended ? "是" : "否") << std::endl;
    std::cout << "请输入新的住院建议 (1-建议住院, 0-不建议住院): ";
    int choice = selectIntCheck(0, 1);
    target->isHospitalizationRecommended = (choice == 1);
    printSuccess("住院建议已更新！");
}
// 删除看诊记录（逻辑删除）
void Doctor::deleteConsultation(Consultation *&target)
{
    target->isDeleted = true; // 逻辑删除
    printSuccess("看诊记录已删除！");
}
// 初始化看诊记录的检查项目列表
void Doctor::initConsultationExamination(Consultation *&target)
{
    while (true)
    {
        if (target->examinationlist.size() > 0)
        {
            std::cout << "当前已有的检查项目:" << std::endl;
            for (const auto &exam : target->examinationlist)
            {
                std::cout << exam << " ";
            }
            std::cout << std::endl;
        }

        std::string newExam = ExaminationItemMenu();

        if (newExam == "0")
        {
            break;
        }

        // 检查是否已存在
        bool exists = false;
        for (const auto &exam : target->examinationlist)
        {
            if (exam == newExam)
            {
                exists = true;
                break;
            }
        }

        if (exists)
        {
            printError("该检查项目已存在，不能重复添加！");
        }
        else
        {
            target->examinationlist.push_back(newExam);
            printSuccess("检查项目信息已添加！");
        }
    }
    return;
}
// 初始化看诊记录的处方列表
void Doctor::initConsultationPrescription(Consultation *&target, Medicine *&medHead)
{
    while (true)
    {
        Medicine *medCurrent = medHead;
        std::cout << "可用药品列表:" << std::endl;
        while (medCurrent != nullptr)
        {
            if (!medCurrent->isDeleted && medCurrent->status == MedicineStatus::NORMAL)
            {
                std::cout << "药品ID: " << medCurrent->medicineID << ", 名称: " << medCurrent->name << ", 规格: " << medCurrent->specification << std::endl;
            }
            medCurrent = medCurrent->next;
        }

        Prescription newPres;
        newPres.medicineID = inputRecordIDCheck("请输入要添加的药品ID: ", {"med"});

        // 检查是否已存在该药品ID
        bool exists = false;
        for (const auto &pres : target->prescriptions)
        {
            if (pres.medicineID == newPres.medicineID)
            {
                exists = true;
                break;
            }
        }

        if (exists)
        {
            printError("该药品已存在于处方中，不能重复添加！");
        }
        else
        {
            medCurrent = medHead;
            bool medFound = false;
            while (medCurrent != nullptr)
            {
                if (!medCurrent->isDeleted && medCurrent->medicineID == newPres.medicineID && medCurrent->status == MedicineStatus::NORMAL)
                {
                    newPres.name = medCurrent->name; // 自动填充药品名称
                    std::cout << "请输入要使用的药品数量：" << std::endl;
                    newPres.quantity = selectIntCheck(1, 10000);

                    newPres.dosage = inputStringCheck("请输入用量信息: ");
                    newPres.frequency = inputStringCheck("请输入频次信息: ");
                    newPres.duration = inputStringCheck("请输入疗程信息: ");
                    newPres.note = inputStringCheck("请输入处方备注信息: ");

                    printSuccess("处方信息已添加！");

                    target->prescriptions.push_back(newPres);
                    medFound = true;
                    break;
                }
                medCurrent = medCurrent->next;
            }

            if (!medFound)
            {
                printError("未找到指定的药品！");
            }
        }
    }
    return;
}
// 从挂号记录创建看诊记录
bool Doctor::createConsultationByRegistration(Registration *&regHead, Consultation *&conHead, Medicine *&medHead, int &idCounter)
{
    Registration *currentReg = regHead;
    bool foundPendingReg = false;

    std::cout << "正在查找医生ID为 " << this->doctorID << " 的待就诊挂号记录..." << std::endl;

    while (currentReg != nullptr)
    {
        // 只显示医生ID匹配且状态为已支付的挂号记录，提示医生选择要创建看诊记录的挂号ID
        if (!currentReg->isDeleted && currentReg->doctorID == this->doctorID && currentReg->status == RegistrationStatus::PAID)
        {
            std::cout << "挂号ID: " << currentReg->registrationID
                      << ", 患者ID: " << currentReg->patientID
                      << ", 科室: " << currentReg->department
                      << ", 挂号时间: " << currentReg->registerTime
                      << ", 费用: " << currentReg->fee
                      << ", 状态: " << regStatusToString(currentReg->status)
                      << ", 备注: " << currentReg->note
                      << std::endl;
            foundPendingReg = true;
            break;
        }
        currentReg = currentReg->next;
    }

    if (!foundPendingReg)
    {
        printError("没有找到任何待就诊的挂号记录，无法创建看诊记录！");
        return false;
    }

    std::string regID = inputRecordIDCheck("请输入要创建看诊记录的挂号ID: ", {"reg"});

    currentReg = regHead;
    foundPendingReg = false;
    while (currentReg != nullptr)
    {
        if (!currentReg->isDeleted && currentReg->doctorID == this->doctorID && currentReg->registrationID == regID && currentReg->status == RegistrationStatus::PAID)
        {
            foundPendingReg = true;
            break;
        }
        currentReg = currentReg->next;
    }

    if (!foundPendingReg)
    {
        printError("未找到指定的待就诊挂号记录，无法创建看诊记录！");
        return false;
    }

    currentReg = regHead;
    while (currentReg != nullptr)
    {
        if (!currentReg->isDeleted && currentReg->doctorID == this->doctorID && currentReg->registrationID == regID)
        {
            Consultation *newCon = new Consultation();
            newCon->consultationID = "con" + std::to_string(idCounter).insert(0, 6 - std::to_string(idCounter).length(), '0'); // 生成新的看诊ID，格式为 con000001
            idCounter++;
            newCon->registrationID = regID;
            newCon->patientID = currentReg->patientID;
            newCon->doctorID = this->doctorID;
            newCon->department = currentReg->department;

            newCon->chiefComplaint = inputStringCheck("请输入主诉信息: ");
            newCon->historyOfPresentIllness = inputStringCheck("请输入现病史信息: ");
            newCon->pastMedicalHistory = inputStringCheck("请输入既往史信息: ");
            newCon->familyHistory = inputStringCheck("请输入家族史信息: ");
            newCon->preliminaryDiagnosis = inputStringCheck("请输入初步诊断信息: ");

            initConsultationExamination(newCon);
            initConsultationPrescription(newCon, medHead);

            MyTime &t = MyTime::getInstance();
            newCon->consultationTime = t.getTime();

            // 插入看诊记录到链表头部
            newCon->next = conHead;
            if (conHead)
                conHead->prev = newCon;
            conHead = newCon;

            printSuccess("看诊记录已创建！新看诊ID: " + newCon->consultationID);
            return true;
        }
        currentReg = currentReg->next;
    }

    printError("未找到指定的挂号记录，无法创建看诊记录！");
    return false;
}

void Doctor::manageConsultations(Consultation *&conHead, Registration *&regHead, Medicine *&medHead, int &idCounter) // 管理看诊记录（查看、修改状态等）
{
    while (true)
    {
        int choice = doctorConsultationManagementMenu();
        if (choice == 0)
        {
            break;
        }
        else if (choice == 1)
        {
            while (true)
            {
                int viewChoice = doctorConsultationViewMenu();
                if (viewChoice == 0)
                {
                    break;
                }
                else if (viewChoice == 1)
                {
                    getAllConsultations(conHead);
                    pause("医生 > 看诊管理");
                }
                else if (viewChoice == 2)
                {
                    getConsultationsByPatientID(conHead);
                    pause("医生 > 看诊管理");
                }
                else if (viewChoice == 3)
                {
                    getConsultationsByID(conHead);
                    pause("医生 > 看诊管理");
                }
                else if (viewChoice == 4)
                {
                    getConsultationsByTimeRange(conHead);
                    pause("医生 > 看诊管理");
                }
                else if (viewChoice == 5)
                {
                    getConsultationsByStatus(conHead);
                    pause("医生 > 看诊管理");
                }
            }
        }
        else if (choice == 2)
        {
            bool ishave = getAllConsultations(conHead);
            if (ishave)
            {
                std::string conID = inputRecordIDCheck("请输入要修改状态的看诊ID: ", {"con"});
                Consultation *target = conHead;
                while (target)
                {
                    if (!target->isDeleted && target->consultationID == conID && target->doctorID == this->doctorID)
                    {
                        break;
                    }
                    target = target->next;
                }
                if (target)
                {
                    while (true)
                    {
                        std::cout << "正在修改的看诊记录信息如下：" << std::endl;
                        std::cout << "看诊ID: " << target->consultationID
                                  << ", 挂号ID: " << target->registrationID
                                  << ", 患者ID: " << target->patientID
                                  << ", 看诊时间: " << target->consultationTime
                                  << ", 科室: " << target->department
                                  << ", 主诉: " << target->chiefComplaint
                                  << ", 现病史: " << target->historyOfPresentIllness
                                  << ", 既往史: " << target->pastMedicalHistory
                                  << ", 家族史: " << target->familyHistory
                                  << ", 初步诊断: " << target->preliminaryDiagnosis
                                  << ", 检查项目数: " << target->examinationlist.size()
                                  << ", 处方数: " << target->prescriptions.size()
                                  << ", 处方审核状态: " << (target->isPrecriptionReviewed ? "已审核" : "未审核")
                                  << ", 住院建议: " << (target->isHospitalizationRecommended ? "是" : "否")
                                  << ", 备注: " << target->note
                                  << std::endl;
                        if (target->examinationlist.size() > 0)
                        {
                            std::cout << "检查项目列表:" << std::endl;
                            for (const auto &exam : target->examinationlist)
                            {
                                std::cout << "  - " << exam << std::endl;
                            }
                        }
                        if (target->prescriptions.size() > 0)
                        {
                            std::cout << "处方列表:" << std::endl;
                            for (const auto &pres : target->prescriptions)
                            {
                                std::cout << "  - 药品ID: " << pres.medicineID
                                          << ", 药品名称: " << pres.name
                                          << ", 药品数量: " << pres.quantity
                                          << ", 用量: " << pres.dosage
                                          << ", 频次: " << pres.frequency
                                          << ", 疗程: " << pres.duration
                                          << ", 备注: " << pres.note
                                          << std::endl;
                            }
                        }

                        int modifyChoice = doctorConsultationModificationMenu();
                        if (modifyChoice == 0)
                        {
                            break;
                        }
                        else if (modifyChoice == 1)
                        {
                            setConsultationStatus(target);
                            pause("医生 > 看诊管理");
                        }
                        else if (modifyChoice == 2)
                        {
                            setConsultationChiefComplaint(target);
                            pause("医生 > 看诊管理");
                        }
                        else if (modifyChoice == 3)
                        {
                            setConsultationHistoryOfPresentIllness(target);
                            pause("医生 > 看诊管理");
                        }
                        else if (modifyChoice == 4)
                        {
                            setConsultationPastMedicalHistory(target);
                            pause("医生 > 看诊管理");
                        }
                        else if (modifyChoice == 5)
                        {
                            setConsultationFamilyHistory(target);
                            pause("医生 > 看诊管理");
                        }
                        else if (modifyChoice == 6)
                        {
                            setConsultationPreliminaryDiagnosis(target);
                            pause("医生 > 看诊管理");
                        }
                        else if (modifyChoice == 7)
                        {
                            addConsultationExamination(target);
                            pause("医生 > 看诊管理");
                        }
                        else if (modifyChoice == 8)
                        {
                            addConsultationPrescription(target, medHead);
                            pause("医生 > 看诊管理");
                        }
                        else if (modifyChoice == 9)
                        {
                            setHospitalizationRecommendation(target);
                            pause("医生 > 看诊管理");
                        }
                    }
                }
                else
                {
                    printError("未找到看诊ID为 " + conID + " 的看诊记录，无法进行修改操作！");
                    pause("医生 > 看诊管理");
                }
            }
        }
        else if (choice == 3)
        {
            bool ishave = getAllConsultations(conHead);
            if (ishave)
            {
                std::string conID = inputRecordIDCheck("请输入要删除的看诊ID: ", {"con"});
                Consultation *target = conHead;
                while (target)
                {
                    if (!target->isDeleted && target->consultationID == conID && target->doctorID == this->doctorID)
                    {
                        break;
                    }
                    target = target->next;
                }
                if (target)
                {
                    std::cout << "正在删除的看诊记录信息如下：" << std::endl;

                    std::cout << "看诊ID: " << target->consultationID
                              << ", 挂号ID: " << target->registrationID
                              << ", 患者ID: " << target->patientID
                              << ", 看诊时间: " << target->consultationTime
                              << ", 科室: " << target->department
                              << ", 主诉: " << target->chiefComplaint
                              << ",现病史: " << target->historyOfPresentIllness
                              << ", 既往史: " << target->pastMedicalHistory
                              << ", 家族史: " << target->familyHistory
                              << ", 初步诊断: " << target->preliminaryDiagnosis
                              << ", 检查项目数: " << target->examinationlist.size()
                              << ", 处方数: " << target->prescriptions.size()
                              << ", 处方审核状态: " << (target->isPrecriptionReviewed ? "已审核" : "未审核")
                              << ", 住院建议: " << (target->isHospitalizationRecommended ? "是" : "否")
                              << ", 备注: " << target->note
                              << std::endl;
                    if (target->examinationlist.size() > 0)
                    {
                        std::cout << "检查项目列表:" << std::endl;
                        for (const auto &exam : target->examinationlist)
                        {
                            std::cout << "  - " << exam << std::endl;
                        }
                    }
                    if (target->prescriptions.size() > 0)
                    {
                        std::cout << "处方列表:" << std::endl;
                        for (const auto &pres : target->prescriptions)
                        {
                            std::cout << "  - 药品ID: " << pres.medicineID
                                      << ", 药品名称: " << pres.name
                                      << ", 药品数量: " << pres.quantity
                                      << ", 用量: " << pres.dosage
                                      << ", 频次: " << pres.frequency
                                      << ", 疗程: " << pres.duration
                                      << ", 备注: " << pres.note
                                      << std::endl;
                        }
                    }

                    std::cout << "确定要删除该看诊记录吗？: " << std::endl;
                    std::cout << "1. 确定" << std::endl;
                    std::cout << "0. 取消" << std::endl;

                    int deleteChoice = selectIntCheck(0, 1);
                    if (deleteChoice == 1)
                    {
                        deleteConsultation(target);
                        pause("医生 > 看诊管理");
                    }
                    else
                    {
                        printWarning("已取消删除操作！");
                        pause("医生 > 看诊管理");
                    }
                }
                else
                {
                    printError("未找到看诊ID为 " + conID + " 的看诊记录，无法删除！");
                    pause("医生 > 看诊管理");
                }
            }
        }
        else if (choice == 4)
        {
            createConsultationByRegistration(regHead, conHead, medHead, idCounter);
            pause("医生 > 看诊管理");
        }
    }
}

// -------------------------------检查记录管理--------------------------------

// 获取医生的所有检查记录信息
bool Doctor::getAllExaminations(Examination *&exaHead)
{
    std::cout << "正在获取医生ID为 " << this->doctorID << " 的所有检查记录..." << std::endl;

    Examination *current = exaHead;
    bool found = false;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->doctorID == this->doctorID)
        {
            std::cout << "检查ID: " << current->examinationID
                      << ", 看诊ID: " << current->consultationID
                      << ", 患者ID: " << current->patientID
                      << ", 医生ID: " << current->doctorID
                      << ", 科室: " << current->department
                      << ", 开单时间: " << current->orderTime
                      << ", 检查项目: " << current->itemName
                      << ", 检查结果: " << findVitalSignToString(current)
                      << ", 报告摘要: " << current->reportSummary
                      << ", 检查费用: " << current->fee
                      << ", 出报告时间: " << current->reportTime
                      << ", 状态: " << examStatusToString(current->status)
                      << ", 备注: " << current->note;

            std::cout << ", 相关附件: ";
            for (size_t i = 0; i < current->attachments.size(); ++i)
            {
                std::cout << "[" << (i + 1) << "] " << current->attachments[i] << "  ";
            }
            std::cout << std::endl;

            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到医生ID为 " << this->doctorID << " 的检查记录。" << std::endl;
    }

    return found;
}
// 根据患者ID获取检查记录信息
bool Doctor::getExaminationsByPatientID(Examination *&exaHead)
{
    std::string patientID = inputRecordIDCheck("请输入要查询的患者ID: ", {"pat"});

    std::cout << "正在获取患者ID为 " << patientID << " 的检查记录..." << std::endl;

    Examination *current = exaHead;
    bool found = false;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->doctorID == this->doctorID && current->patientID == patientID)
        {
            std::cout << "检查ID: " << current->examinationID
                      << ", 看诊ID: " << current->consultationID
                      << ", 患者ID: " << current->patientID
                      << ", 医生ID: " << current->doctorID
                      << ", 科室: " << current->department
                      << ", 开单时间: " << current->orderTime
                      << ", 检查项目: " << current->itemName
                      << ", 检查结果: " << findVitalSignToString(current)
                      << ", 报告摘要: " << current->reportSummary
                      << ", 检查费用: " << current->fee
                      << ", 出报告时间: " << current->reportTime
                      << ", 状态: " << examStatusToString(current->status)
                      << ", 备注: " << current->note;

            std::cout << ", 相关附件: ";
            for (size_t i = 0; i < current->attachments.size(); ++i)
            {
                std::cout << "[" << (i + 1) << "] " << current->attachments[i] << "  ";
            }
            std::cout << std::endl;

            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "没有找到患者ID为 " << patientID << " 的检查记录。" << std::endl;
    }
    return found;
}
// 根据看诊ID获取检查记录信息
bool Doctor::getExaminationsByConsultationID(Examination *&exaHead)
{
    std::string conID = inputRecordIDCheck("请输入要查询的看诊ID: ", {"con"});

    std::cout << "正在获取看诊ID为 " << conID << " 的检查记录..." << std::endl;

    Examination *current = exaHead;
    bool found = false;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->doctorID == this->doctorID && current->consultationID == conID)
        {
            std::cout << "检查ID: " << current->examinationID
                      << ", 看诊ID: " << current->consultationID
                      << ", 患者ID: " << current->patientID
                      << ", 医生ID: " << current->doctorID
                      << ", 科室: " << current->department
                      << ", 开单时间: " << current->orderTime
                      << ", 检查项目: " << current->itemName
                      << ", 检查结果: " << findVitalSignToString(current)
                      << ", 报告摘要: " << current->reportSummary
                      << ", 检查费用: " << current->fee
                      << ", 出报告时间: " << current->reportTime
                      << ", 状态: " << examStatusToString(current->status)
                      << ", 备注: " << current->note;

            std::cout << ", 相关附件: ";
            for (size_t i = 0; i < current->attachments.size(); ++i)
            {
                std::cout << "[" << (i + 1) << "] " << current->attachments[i] << "  ";
            }
            std::cout << std::endl;

            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到看诊ID为 " << conID << " 的检查记录。" << std::endl;
    }

    return found;
}
// 根据检查记录ID获取检查记录信息
bool Doctor::getExaminationsByID(Examination *&exaHead)
{
    std::string examID = inputRecordIDCheck("请输入要查询的检查ID: ", {"exa"});

    std::cout << "正在获取检查ID为 " << examID << " 的检查记录..." << std::endl;

    Examination *current = exaHead;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->doctorID == this->doctorID && current->examinationID == examID)
        {
            std::cout << "检查ID: " << current->examinationID
                      << ", 看诊ID: " << current->consultationID
                      << ", 患者ID: " << current->patientID
                      << ", 医生ID: " << current->doctorID
                      << ", 科室: " << current->department
                      << ", 开单时间: " << current->orderTime
                      << ", 检查项目: " << current->itemName
                      << ", 检查结果: " << findVitalSignToString(current)
                      << ", 报告摘要: " << current->reportSummary
                      << ", 检查费用: " << current->fee
                      << ", 出报告时间: " << current->reportTime
                      << ", 状态: " << examStatusToString(current->status)
                      << ", 备注: " << current->note;

            std::cout << ", 相关附件: ";
            for (size_t i = 0; i < current->attachments.size(); ++i)
            {
                std::cout << "[" << (i + 1) << "] " << current->attachments[i] << "  ";
            }
            std::cout << std::endl;

            return true;
        }
        current = current->next;
    }

    std::cout << "未找到检查ID为 " << examID << " 的检查记录。" << std::endl;
    return false;
}
// 根据时间范围获取检查记录信息
bool Doctor::getExaminationsByTimeRange(Examination *&exaHead)
{
    std::cout << "请输入要查询的时间范围：" << std::endl;
    std::string startTime = inputDateCheck("请输入开始时间");
    std::string endTime = inputDateCheck("请输入结束时间");

    std::cout << "正在获取 " << startTime << " 到 " << endTime << " 之间的检查记录..." << std::endl;

    Examination *current = exaHead;
    bool found = false;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->doctorID == this->doctorID)
        {

            if (current->orderTime.substr(0, 10) >= startTime && current->orderTime.substr(0, 10) <= endTime)
            {
                std::cout << "检查ID: " << current->examinationID
                          << ", 看诊ID: " << current->consultationID
                          << ", 患者ID: " << current->patientID
                          << ", 医生ID: " << current->doctorID
                          << ", 科室: " << current->department
                          << ", 开单时间: " << current->orderTime
                          << ", 检查项目: " << current->itemName
                          << ", 检查结果: " << findVitalSignToString(current)
                          << ", 报告摘要: " << current->reportSummary
                          << ", 检查费用: " << current->fee
                          << ", 出报告时间: " << current->reportTime
                          << ", 状态: " << examStatusToString(current->status)
                          << ", 备注: " << current->note;

                std::cout << ", 相关附件: ";
                for (size_t i = 0; i < current->attachments.size(); ++i)
                {
                    std::cout << "[" << (i + 1) << "] " << current->attachments[i] << "  ";
                }
                std::cout << std::endl;

                found = true;
            }
        }
        current = current->next;
    }

    if (!found)
    {
        printWarning("未找到指定时间范围内的检查记录。");
    }

    return found;
}
// 根据检查项目名称获取检查记录信息
bool Doctor::getExaminationsByItemName(Examination *&exaHead)
{
    std::string itemName = ExaminationItemMenu();

    if (itemName == "0")
    {
        printWarning("已取消查询操作！");
        return false;
    }

    std::cout << "正在获取检查项目名称为 '" << itemName << "' 的检查记录..." << std::endl;

    Examination *current = exaHead;
    bool found = false;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->doctorID == this->doctorID && current->itemName == itemName)
        {
            std::cout << "检查ID: " << current->examinationID
                      << ", 看诊ID: " << current->consultationID
                      << ", 患者ID: " << current->patientID
                      << ", 医生ID: " << current->doctorID
                      << ", 科室: " << current->department
                      << ", 开单时间: " << current->orderTime
                      << ", 检查项目: " << current->itemName
                      << ", 检查结果: " << findVitalSignToString(current)
                      << ", 报告摘要: " << current->reportSummary
                      << ", 检查费用: " << current->fee
                      << ", 出报告时间: " << current->reportTime
                      << ", 状态: " << examStatusToString(current->status)
                      << ", 备注: " << current->note;

            std::cout << ", 相关附件: ";
            for (size_t i = 0; i < current->attachments.size(); ++i)
            {
                std::cout << "[" << (i + 1) << "] " << current->attachments[i] << "  ";
            }
            std::cout << std::endl;

            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到检查项目名称为 '" << itemName << "' 的检查记录。" << std::endl;
    }

    return found;
}
// 根据检查状态获取检查记录信息
bool Doctor::getExaminationsByStatus(Examination *&exaHead)
{
    std::cout << "请选择要查询的检查状态：" << std::endl;
    std::cout << "1. 已下单" << std::endl;
    std::cout << "2. 已支付" << std::endl;
    std::cout << "3. 检查中" << std::endl;
    std::cout << "4. 检查完成" << std::endl;
    std::cout << "5. 报告已出" << std::endl;
    std::cout << "6. 已作废" << std::endl;
    std::cout << "0. 取消查询" << std::endl;

    int statusChoice = selectIntCheck(0, 6);

    if (statusChoice == 0)
    {
        return false;
    }

    ExaminationStatus statusFilter = static_cast<ExaminationStatus>(statusChoice);

    std::cout << "正在获取检查状态为 '" << examStatusToString(statusFilter) << "' 的检查记录..." << std::endl;

    Examination *current = exaHead;
    bool found = false;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->doctorID == this->doctorID && current->status == statusFilter)
        {
            std::cout << "检查ID: " << current->examinationID
                      << ", 看诊ID: " << current->consultationID
                      << ", 患者ID: " << current->patientID
                      << ", 医生ID: " << current->doctorID
                      << ", 科室: " << current->department
                      << ", 开单时间: " << current->orderTime
                      << ", 检查项目: " << current->itemName
                      << ", 检查结果: " << findVitalSignToString(current)
                      << ", 报告摘要: " << current->reportSummary
                      << ", 检查费用: " << current->fee
                      << ", 出报告时间: " << current->reportTime
                      << ", 状态: " << examStatusToString(current->status)
                      << ", 备注: " << current->note;

            std::cout << ", 相关附件: ";
            for (size_t i = 0; i < current->attachments.size(); ++i)
            {
                std::cout << "[" << (i + 1) << "] " << current->attachments[i] << "  ";
            }
            std::cout << std::endl;

            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到检查状态为 '" << examStatusToString(statusFilter) << "' 的检查记录。" << std::endl;
    }
    return found;
}
// 修改检查记录的状态
void Doctor::setExaminationStatus(Examination *&target)
{
    std::cout << "当前检查状态: " << examStatusToString(target->status) << std::endl;
    std::cout << "请选择新的检查状态：" << std::endl;
    std::cout << "1. 已下单" << std::endl;
    std::cout << "2. 已支付" << std::endl;
    std::cout << "3. 检查中" << std::endl;
    std::cout << "4. 检查完成" << std::endl;
    std::cout << "5. 报告已出" << std::endl;
    std::cout << "6. 已作废" << std::endl;
    std::cout << "0. 取消修改" << std::endl;

    int statusChoice = selectIntCheck(0, 6);

    if (statusChoice == 0)
    {
        printWarning("已取消修改操作！");
        return;
    }

    target->status = static_cast<ExaminationStatus>(statusChoice);
    printSuccess("检查状态已更新为: " + examStatusToString(target->status));
}
// 修改检查项目名称
void Doctor::setExaminationItemName(Examination *&target)
{
    std::cout << "当前检查项目名称: " << target->itemName << std::endl;
    std::string newItemName = ExaminationItemMenu();

    if (newItemName == "0")
    {
        printWarning("已取消修改操作！");
        return;
    }

    target->itemName = newItemName;
    printSuccess("检查项目名称已更新！");
}
// 修改检查结果报告
void Doctor::setExaminationReport(Examination *&target)
{
    std::cout << "当前检查结果报告内容: " << findVitalSignToString(target) << std::endl;

    setVitalSigns(target->vitalSigns, target->itemName); // 根据检查项目名称设置相应的生命体征信息

    printSuccess("检查结果报告已更新！");
}
// 修改检查报告摘要
void Doctor::setExaminationReportSummary(Examination *&target)
{
    std::cout << "当前检查报告摘要: " << target->reportSummary << std::endl;
    std::string newSummary = inputStringCheck("请输入新的检查报告摘要: ");
    target->reportSummary = newSummary;
    printSuccess("检查报告摘要已更新！");
}
// 修改检查记录的相关附件信息
void Doctor::setExaminationAttachments(Examination *&target)
{
    while (true)
    {
        std::cout << "当前相关附件: ";
        if (target->attachments.empty())
        {
            std::cout << "无";
        }
        else
        {
            for (size_t i = 0; i < target->attachments.size(); ++i)
            {
                std::cout << "[" << (i + 1) << "] " << target->attachments[i] << "  ";
            }
        }
        std::cout << std::endl;

        std::cout << "请选择操作:" << std::endl;
        std::cout << "1. 添加附件" << std::endl;
        std::cout << "2. 删除附件" << std::endl;
        std::cout << "0. 返回上级菜单" << std::endl;
        int op = selectIntCheck(0, 2);

        if (op == 0)
        {
            break;
        }
        else if (op == 1)
        {
            std::string newAttach = inputStringCheck("请输入要添加的附件名称/路径: ");
            // 检查是否重复
            bool exists = false;
            for (const auto &a : target->attachments)
            {
                if (a == newAttach)
                {
                    exists = true;
                    break;
                }
            }
            if (exists)
            {
                printError("该附件已存在，不能重复添加！");
            }
            else
            {
                target->attachments.push_back(newAttach);
                printSuccess("附件已添加！");
            }
        }
        else if (op == 2)
        {
            if (target->attachments.empty())
            {
                printWarning("当前没有附件可删除！");
                continue;
            }
            std::cout << "请输入要删除的附件序号（1-" << target->attachments.size() << "，0返回）: ";
            int idx = selectIntCheck(0, static_cast<int>(target->attachments.size()));
            if (idx == 0)
                continue;
            target->attachments.erase(target->attachments.begin() + idx - 1);
            printSuccess("附件已删除！");
        }
    }
}
// 设置检查记录的备注信息
void Doctor::setExaminationNote(Examination *&target)
{
    std::cout << "当前备注信息: " << target->note << std::endl;
    std::string newNote = inputStringCheck("请输入要添加的备注信息: ");
    target->note = newNote;
    printSuccess("备注信息已更新！");
}
// 删除检查记录（逻辑删除）
void Doctor::deleteExamination(Examination *&target)
{
    target->isDeleted = true;
    printSuccess("检查记录已删除！");
}
// 创建检查记录（从看诊记录创建）
bool Doctor::createExaminationByConsultation(Consultation *&conHead, Examination *&exaHead, int &idCounter) // 从看诊记录创建检查记录
{
    std::cout << "正在查找医生ID为 " << this->doctorID << " 的待检查看诊记录..." << std::endl;

    Consultation *currentCon = conHead;
    bool foundPendingCon = false;
    while (currentCon != nullptr)
    {
        if (!currentCon->isDeleted && currentCon->department == this->department && currentCon->status == ConsultationStatus::IN_PROGRESS && !currentCon->examinationlist.empty())
        {
            std::cout << "看诊ID: " << currentCon->consultationID
                      << ", 挂号ID: " << currentCon->registrationID
                      << ", 患者ID: " << currentCon->patientID
                      << ", 看诊医生ID: " << currentCon->doctorID
                      << ", 科室: " << currentCon->department
                      << ", 看诊时间: " << currentCon->consultationTime
                      << ", 主诉: " << currentCon->chiefComplaint
                      << ", 现病史: " << currentCon->historyOfPresentIllness
                      << ", 既往史: " << currentCon->pastMedicalHistory
                      << ", 家族史: " << currentCon->familyHistory
                      << ", 初步诊断: " << currentCon->preliminaryDiagnosis
                      << ", 检查项目数: " << currentCon->examinationlist.size()
                      << ", 处方数: " << currentCon->prescriptions.size()
                      << ", 处方审核状态: " << (currentCon->isPrecriptionReviewed ? "已审核" : "未审核")
                      << ", 住院建议: " << (currentCon->isHospitalizationRecommended ? "是" : "否")
                      << ", 备注: " << currentCon->note
                      << std::endl;
            foundPendingCon = true;
            break;
        }
        currentCon = currentCon->next;
    }

    if (!foundPendingCon)
    {
        printError("没有找到任何需要创建检查记录的看诊记录，无法创建检查记录！");
        return false;
    }

    std::string conID = inputRecordIDCheck("请输入要创建检查记录的看诊ID: ", {"con"});
    Consultation *targetCon = conHead;
    while (targetCon != nullptr)
    {
        if (!targetCon->isDeleted && targetCon->department == this->department && targetCon->status == ConsultationStatus::IN_PROGRESS && !targetCon->examinationlist.empty() && targetCon->consultationID == conID)
        {
            for (const auto &examItem : targetCon->examinationlist)
            {
                // 创建新的检查记录
                Examination *newExa = new Examination();

                newExa->consultationID = targetCon->consultationID;
                newExa->patientID = targetCon->patientID;
                newExa->doctorID = this->doctorID;
                newExa->department = targetCon->department;

                newExa->itemName = examItem; // 从看诊记录的检查项目列表中获取检查项目名称

                newExa->fee = calculateExaminationFee(newExa->itemName); // 根据检查项目名称计算检查费用

                newExa->examinationID = "exa" + std::to_string(idCounter).insert(0, 6 - std::to_string(idCounter).length(), '0'); // 生成新的检查ID，格式为 exa000001
                idCounter++;
                MyTime &t = MyTime::getInstance();
                newExa->orderTime = t.getTime();

                // 插入检查记录到链表头部
                newExa->next = exaHead;
                if (exaHead)
                    exaHead->prev = newExa;
                exaHead = newExa;
            }

            printSuccess("成功为看诊ID为 " + conID + " 的看诊记录创建了 " + std::to_string(targetCon->examinationlist.size()) + " 条检查记录！");
            return true;
        }
        targetCon = targetCon->next;
    }
    printError("未找到看诊ID为 " + conID + " 的看诊记录，无法创建检查记录！");
    return false;
}

void Doctor::manageExaminations(Examination *&exaHead, Consultation *&conHead, int &idCounter) // 管理检查记录（查看、修改状态等）
{
    while (true)
    {
        int choice = doctorExaminationManagementMenu();
        if (choice == 0)
        {
            break;
        }
        else if (choice == 1)
        {
            while (true)
            {
                int viewChoice = doctorExaminationViewMenu();
                if (viewChoice == 0)
                {
                    break;
                }
                else if (viewChoice == 1)
                {
                    getAllExaminations(exaHead);
                    pause("医生 > 检查管理");
                }
                else if (viewChoice == 2)
                {
                    getExaminationsByPatientID(exaHead);
                    pause("医生 > 检查管理");
                }
                else if (viewChoice == 3)
                {
                    getExaminationsByConsultationID(exaHead);
                    pause("医生 > 检查管理");
                }
                else if (viewChoice == 4)
                {
                    getExaminationsByID(exaHead);
                    pause("医生 > 检查管理");
                }
                else if (viewChoice == 5)
                {
                    getExaminationsByTimeRange(exaHead);
                    pause("医生 > 检查管理");
                }
                else if (viewChoice == 6)
                {
                    getExaminationsByItemName(exaHead);
                    pause("医生 > 检查管理");
                }
                else if (viewChoice == 7)
                {
                    getExaminationsByStatus(exaHead);
                    pause("医生 > 检查管理");
                }
            }
        }
        else if (choice == 2)
        {
            bool ishave = getAllExaminations(exaHead);
            if (ishave)
            {
                std::string examID = inputRecordIDCheck("请输入要修改的检查ID: ", {"exa"});
                Examination *target = exaHead;
                while (target)
                {
                    if (!target->isDeleted && target->examinationID == examID && target->doctorID == this->doctorID)
                    {
                        break;
                    }
                    target = target->next;
                }
                if (target)
                {
                    while (true)
                    {
                        std::cout << "正在修改的检查记录信息如下：" << std::endl;

                        std::cout << "检查ID: " << target->examinationID
                                  << ", 看诊ID: " << target->consultationID
                                  << ", 患者ID: " << target->patientID
                                  << ", 医生ID: " << target->doctorID
                                  << ", 科室: " << target->department
                                  << ", 开单时间: " << target->orderTime
                                  << ", 检查项目: " << target->itemName
                                  << ", 检查结果: " << findVitalSignToString(target)
                                  << ", 报告摘要: " << target->reportSummary
                                  << ", 检查费用: " << target->fee
                                  << ", 出报告时间: " << target->reportTime
                                  << ", 状态: " << examStatusToString(target->status)
                                  << ", 备注: " << target->note;

                        std::cout << ", 相关附件: ";
                        for (size_t i = 0; i < target->attachments.size(); ++i)
                        {
                            std::cout << "[" << (i + 1) << "] " << target->attachments[i] << "  ";
                        }
                        std::cout << std::endl;

                        int modifyChoice = doctorExaminationModificationMenu();
                        if (modifyChoice == 0)
                        {
                            break;
                        }
                        else if (modifyChoice == 1)
                        {
                            setExaminationStatus(target);
                            pause("医生 > 检查管理");
                        }
                        else if (modifyChoice == 2)
                        {
                            setExaminationItemName(target);
                            pause("医生 > 检查管理");
                        }
                        else if (modifyChoice == 3)
                        {
                            setExaminationReport(target);
                            pause("医生 > 检查管理");
                        }
                        else if (modifyChoice == 4)
                        {
                            setExaminationReportSummary(target);
                            pause("医生 > 检查管理");
                        }
                        else if (modifyChoice == 5)
                        {
                            setExaminationAttachments(target);
                            pause("医生 > 检查管理");
                        }
                        else if (modifyChoice == 6)
                        {
                            setExaminationNote(target);
                            pause("医生 > 检查管理");
                        }
                    }
                }
                else
                {
                    printError("未找到检查ID为 " + examID + " 的检查记录，无法进行修改操作！");
                    pause("医生 > 检查管理");
                }
            }
        }
        else if (choice == 3)
        {
            bool ishave = getAllExaminations(exaHead);
            if (ishave)
            {
                std::string examID = inputRecordIDCheck("请输入要删除的检查ID: ", {"exa"});
                Examination *target = exaHead;
                while (target)
                {
                    if (!target->isDeleted && target->examinationID == examID && target->doctorID == this->doctorID)
                    {
                        break;
                    }
                    target = target->next;
                }
                if (target)
                {
                    std::cout << "正在删除的检查记录信息如下：" << std::endl;

                    std::cout << "检查ID: " << target->examinationID
                              << ", 看诊ID: " << target->consultationID
                              << ", 患者ID: " << target->patientID
                              << ", 医生ID: " << target->doctorID
                              << ", 科室: " << target->department
                              << ", 开单时间: " << target->orderTime
                              << ", 检查项目: " << target->itemName
                              << ", 检查结果: " << findVitalSignToString(target)
                              << ", 报告摘要: " << target->reportSummary
                              << ", 检查费用: " << target->fee
                              << ", 出报告时间: " << target->reportTime
                              << ", 状态: " << examStatusToString(target->status)
                              << ", 备注: " << target->note;

                    std::cout << ", 相关附件: ";
                    for (size_t i = 0; i < target->attachments.size(); ++i)
                    {
                        std::cout << "[" << (i + 1) << "] " << target->attachments[i] << "  ";
                    }
                    std::cout << std::endl;

                    std::cout << "确定要删除该检查记录吗？: " << std::endl;
                    std::cout << "1. 确定" << std::endl;
                    std::cout << "0. 取消" << std::endl;

                    int deleteChoice = selectIntCheck(0, 1);
                    if (deleteChoice == 1)
                    {
                        deleteExamination(target);
                        pause("医生 > 检查管理");
                    }
                    else
                    {
                        printWarning("已取消删除操作！");
                        pause("医生 > 检查管理");
                    }
                }
            }
        }
        else if (choice == 4)
        {
            createExaminationByConsultation(conHead, exaHead, idCounter);
            pause("医生 > 检查管理");
        }
    }
}

// ------------------------以下是医生个人信息管理相关函数实现-----------------------

void Doctor::managePersonalInfo()
{
    while (true)
    {
        int choice = doctorPersonalInfoManagementMenu();
        if (choice == 0)
        {
            break;
        }
        else if (choice == 1)
        {
            while (true)
            {
                int viewChoice = doctorPersonalInfoViewMenu();
                if (viewChoice == 0)
                {
                    break;
                }
                else if (viewChoice == 1)
                {
                    std::cout << "医生ID: " << this->doctorID << std::endl;
                }
                else if (viewChoice == 2)
                {
                    std::cout << "姓名: " << this->username << std::endl;
                }
                else if (viewChoice == 3)
                {
                    std::cout << "性别: " << this->gender << std::endl;
                }
                else if (viewChoice == 4)
                {
                    std::cout << "年龄: " << this->age << std::endl;
                }
                else if (viewChoice == 5)
                {
                    std::cout << "科室: " << this->department << std::endl;
                }
                else if (viewChoice == 6)
                {
                    std::cout << "职称: " << doctorTitleToString(this->title) << std::endl;
                }
                else if (viewChoice == 7)
                {
                    std::cout << "联系电话: " << this->telephone << std::endl;
                }
                else if (viewChoice == 8)
                {
                    std::cout << "邮箱: " << this->email << std::endl;
                }
                else if (viewChoice == 9)
                {
                    std::cout << "在岗状态: " << (this->isOnDuty ? "在岗" : "不在岗") << std::endl;
                }
                else if (viewChoice == 10)
                {
                    std::cout << "擅长方向: " << this->specialty << std::endl;
                }
                else if (viewChoice == 11)
                {
                    std::cout << "排班信息: " << this->scheduleInfo << std::endl;
                }
                else if (viewChoice == 12)
                {
                    std::cout << "累计接诊患者人数: " << this->consultationCount << std::endl;
                }
                else if (viewChoice == 13)
                {
                    std::cout << "累计完成的检查数量: " << this->examinationCount << std::endl;
                }
                else if (viewChoice == 14)
                {
                    std::cout << "累计开具住院证数量: " << this->hospitalizationApplyCount << std::endl;
                }
                else if (viewChoice == 15)
                {
                    std::cout << "账户创建时间: " << this->createTime << std::endl;
                }
                pause("医生 > 个人信息管理");
            }
        }
        else if (choice == 2)
        {
            while (true)
            {
                int modifyChoice = doctorPersonalInfoModificationMenu();

                if (modifyChoice == 0)
                {
                    break;
                }
                else if (modifyChoice == 1)
                {
                    std::cout << "当前的姓名: " << this->username << std::endl;
                    this->username = inputStringCheck("请输入新的姓名: ");
                    printSuccess("姓名已更新！");
                }
                else if (modifyChoice == 2)
                {
                    std::cout << "当前的性别: " << this->gender << std::endl;
                    this->gender = inputGenderCheck("请输入新的性别: ");
                    printSuccess("性别已更新！");
                }
                else if (modifyChoice == 3)
                {
                    std::cout << "当前的年龄: " << this->age << std::endl;
                    this->age = inputAgeCheck("请输入新的年龄: ");
                    printSuccess("年龄已更新！");
                }
                else if (modifyChoice == 4)
                {
                    std::cout << "当前的科室: " << this->department << std::endl;
                    this->department = inputDepartmentCheck("请输入新的科室: ");
                    printSuccess("科室已更新！");
                }
                else if (modifyChoice == 5)
                {
                    std::cout << "当前的职称: " << doctorTitleToString(this->title) << std::endl;

                    int titleChoice = DoctorTitleMenu();

                    if (titleChoice == 0)
                    {
                        printWarning("已取消修改操作！");
                        continue;
                    }

                    this->title = static_cast<DoctorTitle>(titleChoice);
                    printSuccess("职称已更新！");
                }
                else if (modifyChoice == 6)
                {
                    std::cout << "当前的联系电话: " << this->telephone << std::endl;
                    this->telephone = inputTelephoneCheck("请输入新的联系电话: ");
                    printSuccess("联系电话已更新！");
                }
                else if (modifyChoice == 7)
                {
                    std::cout << "当前的邮箱: " << this->email << std::endl;
                    this->email = inputEmailCheck("请输入新的邮箱: ");
                    printSuccess("邮箱已更新！");
                }
                else if (modifyChoice == 8)
                {
                    std::cout << "当前的在岗状态: " << (this->isOnDuty ? "在岗" : "不在岗") << std::endl;
                    std::cout << "请选择新的在岗状态: " << std::endl;
                    std::cout << "1. 在岗" << std::endl;
                    std::cout << "2. 不在岗" << std::endl;
                    std::cout << "0. 取消修改" << std::endl;

                    int dutyChoice = selectIntCheck(0, 2);
                    if (dutyChoice == 0)
                    {
                        printWarning("已取消修改操作！");
                        continue;
                    }

                    this->isOnDuty = (dutyChoice == 1);
                    printSuccess("在岗状态已更新！");
                }
                else if (modifyChoice == 9)
                {
                    std::cout << "当前的擅长方向: " << this->specialty << std::endl;
                    this->specialty = inputStringCheck("请输入新的擅长方向: ");
                    printSuccess("擅长方向已更新！");
                }
                else if (modifyChoice == 10)
                {
                    std::cout << "当前的排班信息: " << this->scheduleInfo << std::endl;
                    this->scheduleInfo = inputStringCheck("请输入新的排班信息: ");
                    printSuccess("排班信息已更新！");
                }
                else if (modifyChoice == 11)
                {
                    std::string oldpwd = inputPwdCheck("请输入当前密码以验证身份: ");

                    if (SHA256Verify(oldpwd, this->storedHash, this->kHashIterations))
                    {
                        std::string newpwd1 = inputPwdCheck("请输入新的密码: ");
                        std::string newpwd2 = inputPwdCheck("请再次输入新的密码以确认: ");

                        if (newpwd1 == newpwd2)
                        {
                            std::string newSalt = generateSalt();
                            std::string newHash = SHA256Encrypt(newpwd1, newSalt, this->kHashIterations);
                            this->salt = newSalt;
                            this->storedHash = newHash;

                            printSuccess("密码更新成功！");
                        }
                        else
                        {
                            printError("两次输入的新密码不一致，密码更新失败！");
                        }
                    }
                    else
                    {
                        printError("密码验证失败，无法修改密码！");
                    }
                }
                pause("医生 > 个人信息管理");
            }
        }
    }
}
