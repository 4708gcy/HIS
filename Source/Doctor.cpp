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
    // 析构函数实现（如有需要）
}

void Doctor::doctorSignUp(int &idCounter)
{
    bool success = signUp(2, idCounter); // 调用基类的注册方法，传入角色类型 2（医生）
    if (!success)
    {
        std::cout << "医生注册失败！" << std::endl;
        return;
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

    std::cout << "医生注册成功! 您的用户ID是: " << this->userID << std::endl;
}

bool Doctor::doctorSignIn()
{
    if (isAccountActive == false)
    {
        std::cout << "账户已锁定，请联系系统管理员解锁！" << std::endl;
        return false;
    }

    while (loginAttempts < kMaxLoginAttempts)
    {
        std::string pwd = inputStringCheck("请输入密码(输入\"quit\"退出登录): ");

        if (pwd == "quit")
        {
            std::cout << "退出登录。" << std::endl;
            return false;
        }

        bool success = SHA256Verify(pwd, storedHash, kHashIterations);

        if (success)
        {
            isLoggedIn = true;
            std::cout << "医生登录成功！" << std::endl;
            return true;
        }
        else
        {
            loginAttempts++;
            std::cout << "密码错误! 请重新输入密码(当前失败次数: " << loginAttempts << ")" << std::endl;

            if (loginAttempts >= kMaxLoginAttempts)
            {
                isAccountActive = false;
                std::cout << "连续登录失败次数过多，账户已锁定，请联系系统管理员解锁！" << std::endl;
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
        std::cout << "没有找到挂号日期在指定范围内的记录。" << std::endl;
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
    std::cout << "挂号状态已更新为: " << regStatusToString(target->status) << std::endl;
}
// 删除挂号记录（逻辑删除）
void Doctor::deleteRegistration(Registration *&target)
{
    target->isDeleted = true; // 逻辑删除
    std::cout << "挂号记录已删除！" << std::endl;
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
                std::cout << "未找到同科室在岗医生，无法为患者挂号！" << std::endl;
                return false;
            }

            std::string assignedDoctorID = inputIDCheck("请输入要分配的医生ID: ");

            doc = doctor;

            while (doc != nullptr)
            {
                if (!doc->isDeleted && doc->isOnDuty && doc->department == this->department && doc->doctorID == assignedDoctorID)
                {
                    Registration *newReg = new Registration();
                    newReg->registrationID = "reg" + std::to_string(idCounter++).insert(0, 6 - std::to_string(idCounter).length(), '0'); // 生成新的挂号ID，格式为 reg000001
                    newReg->patientID = patientID;

                    currentPatient->department = this->department; // 将患者的就诊科室设置为医生所在科室

                    newReg->department = this->department;

                    MyTime &t = MyTime::getInstance();
                    newReg->registerTime = t.getTime();
                    newReg->fee = calculateRegistrationFee(this->title);
                    newReg->status = RegistrationStatus::BOOKED;

                    newReg->note = inputStringCheck("请输入挂号备注信息: ");

                    // 插入挂号记录到链表头部
                    newReg->next = regHead;
                    if (regHead)
                        regHead->prev = newReg;
                    regHead = newReg;

                    std::cout << "挂号成功！新挂号ID: " << newReg->registrationID << std::endl;
                    return true;
                }
                doc = doc->next;
            }
            std::cout << "未找到指定的医生，无法为患者挂号！" << std::endl;
            return false;
        }
        currentPatient = currentPatient->next;
    }

    std::cout << "未找到指定的患者信息！" << std::endl;
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
                }
                else if (viewChoice == 2)
                {
                    getRegistrationsByID(regHead);
                }
                else if (viewChoice == 3)
                {
                    getRegistrationsByPatientID(regHead);
                }
                else if (viewChoice == 4)
                {
                    getRegistrationsByTimeRange(regHead);
                }
                else if (viewChoice == 5)
                {
                    getRegistrationsByStatus(regHead);
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
                    setRegistrationStatus(target);
                }
                else
                {
                    std::cout << "未找到挂号ID为 " << regID << " 的挂号记录，无法修改状态！" << std::endl;
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
                    deleteRegistration(target);
                }
                else
                {
                    std::cout << "未找到挂号ID为 " << regID << " 的挂号记录，无法删除！" << std::endl;
                }
            }
        }
        else if (choice == 4)
        {
            createRegistrationByPatient(regHead, doctor, patientHead, idCounter); // 这里传入 nullptr，因为在 createRegistrationByPatient 内部会再次输入患者ID并查找患者信息
        }
    }
}

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
        std::cout << "没有找到看诊日期在指定范围内的记录。" << std::endl;
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
                    std::cout << "  - 药品名称: " << pres.medicineID
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
    std::cout << "看诊状态已更新为: " << conStatusToString(target->status) << std::endl;
}
// 修改看诊记录的主诉信息
void Doctor::setConsultationChiefComplaint(Consultation *&target)
{
    std::cout << "当前主诉信息: " << target->chiefComplaint << std::endl;
    std::string newChiefComplaint = inputStringCheck("请输入新的主诉信息: ");
    target->chiefComplaint = newChiefComplaint;
    std::cout << "主诉信息已更新！" << std::endl;
}
// 修改看诊记录的现病史信息
void Doctor::setConsultationHistoryOfPresentIllness(Consultation *&target)
{
    std::cout << "当前现病史信息: " << target->historyOfPresentIllness << std::endl;
    std::string newHistory = inputStringCheck("请输入新的现病史信息: ");
    target->historyOfPresentIllness = newHistory;
    std::cout << "现病史信息已更新！" << std::endl;
}
// 修改看诊记录的既往史信息
void Doctor::setConsultationPastMedicalHistory(Consultation *&target)
{
    std::cout << "当前既往史信息: " << target->pastMedicalHistory << std::endl;
    std::string newHistory = inputStringCheck("请输入新的既往史信息: ");
    target->pastMedicalHistory = newHistory;
    std::cout << "既往史信息已更新！" << std::endl;
}
// 修改看诊记录的家族史信息
void Doctor::setConsultationFamilyHistory(Consultation *&target)
{
    std::cout << "当前家族史信息: " << target->familyHistory << std::endl;
    std::string newHistory = inputStringCheck("请输入新的家族史信息: ");
    target->familyHistory = newHistory;
    std::cout << "家族史信息已更新！" << std::endl;
}
// 修改看诊记录的初步诊断信息
void Doctor::setConsultationPreliminaryDiagnosis(Consultation *&target)
{
    std::cout << "当前初步诊断信息: " << target->preliminaryDiagnosis << std::endl;
    std::string newDiagnosis = inputStringCheck("请输入新的初步诊断信息: ");
    target->preliminaryDiagnosis = newDiagnosis;
    std::cout << "初步诊断信息已更新！" << std::endl;
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
            std::cout << "该检查项目已存在，不能重复添加！" << std::endl;
        }
        else
        {
            target->examinationlist.push_back(newExam);
            std::cout << "检查项目信息已添加！" << std::endl;
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
            std::cout << "该药品已存在于处方中，不能重复添加！" << std::endl;
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
                    newPres.quantity = selectIntCheck(1, INT_MAX);

                    newPres.dosage = inputStringCheck("请输入用量信息: ");
                    newPres.frequency = inputStringCheck("请输入频次信息: ");
                    newPres.duration = inputStringCheck("请输入疗程信息: ");
                    newPres.note = inputStringCheck("请输入处方备注信息: ");

                    std::cout << "处方信息已添加！" << std::endl;

                    target->prescriptions.push_back(newPres);
                    medFound = true;
                    break;
                }
                medCurrent = medCurrent->next;
            }

            if (!medFound)
            {
                std::cout << "未找到指定的药品！" << std::endl;
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
    std::cout << "住院建议已更新！" << std::endl;
}
// 删除看诊记录（逻辑删除）
void Doctor::deleteConsultation(Consultation *&target)
{
    target->isDeleted = true; // 逻辑删除
    std::cout << "看诊记录已删除！" << std::endl;
}
// 初始化看诊记录的检查项目列表
void initConsultationExamination(Consultation *&target)
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
            std::cout << "该检查项目已存在，不能重复添加！" << std::endl;
        }
        else
        {
            target->examinationlist.push_back(newExam);
            std::cout << "检查项目信息已添加！" << std::endl;
        }
    }
    return;
}
// 初始化看诊记录的处方列表
void initConsultationPrescription(Consultation *&target, Medicine *&medHead)
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
            std::cout << "该药品已存在于处方中，不能重复添加！" << std::endl;
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
                    newPres.quantity = selectIntCheck(1, INT_MAX);

                    newPres.dosage = inputStringCheck("请输入用量信息: ");
                    newPres.frequency = inputStringCheck("请输入频次信息: ");
                    newPres.duration = inputStringCheck("请输入疗程信息: ");
                    newPres.note = inputStringCheck("请输入处方备注信息: ");

                    std::cout << "处方信息已添加！" << std::endl;

                    target->prescriptions.push_back(newPres);
                    medFound = true;
                    break;
                }
                medCurrent = medCurrent->next;
            }

            if (!medFound)
            {
                std::cout << "未找到指定的药品！" << std::endl;
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
        std::cout << "没有找到任何待就诊的挂号记录，无法创建看诊记录！" << std::endl;
        return false;
    }

    std::string regID = inputRecordIDCheck("请输入要创建看诊记录的挂号ID: ", {"reg"});

    currentReg = regHead;
    foundPendingReg = false;
    while (true)
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
        std::cout << "未找到指定的待就诊挂号记录，无法创建看诊记录！" << std::endl;
        return false;
    }

    currentReg = regHead;
    while (currentReg != nullptr)
    {
        if (!currentReg->isDeleted && currentReg->doctorID == this->doctorID && currentReg->registrationID == regID)
        {
            Consultation *newCon = new Consultation();
            newCon->consultationID = "con" + std::to_string(idCounter++).insert(0, 6 - std::to_string(idCounter).length(), '0'); // 生成新的看诊ID，格式为 con000001
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

            std::cout << "看诊记录已创建！新看诊ID: " << newCon->consultationID << std::endl;
            return true;
        }
        currentReg = currentReg->next;
    }

    std::cout << "未找到指定的挂号记录，无法创建看诊记录！" << std::endl;
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
                }
                else if (viewChoice == 2)
                {
                    getConsultationsByPatientID(conHead);
                }
                else if (viewChoice == 3)
                {
                    getConsultationsByID(conHead);
                }
                else if (viewChoice == 4)
                {
                    getConsultationsByTimeRange(conHead);
                }
                else if (viewChoice == 5)
                {
                    getConsultationsByStatus(conHead);
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
                        int modifyChoice = doctorConsultationModificationMenu();
                        if (modifyChoice == 0)
                        {
                            break;
                        }
                        else if (modifyChoice == 1)
                        {
                            setConsultationStatus(target);
                        }
                        else if (modifyChoice == 2)
                        {
                            setConsultationChiefComplaint(target);
                        }
                        else if (modifyChoice == 3)
                        {
                            setConsultationHistoryOfPresentIllness(target);
                        }
                        else if (modifyChoice == 4)
                        {
                            setConsultationPastMedicalHistory(target);
                        }
                        else if (modifyChoice == 5)
                        {
                            setConsultationFamilyHistory(target);
                        }
                        else if (modifyChoice == 6)
                        {
                            setConsultationPreliminaryDiagnosis(target);
                        }
                        else if (modifyChoice == 7)
                        {
                            addConsultationExamination(target);
                        }
                        else if (modifyChoice == 8)
                        {
                            addConsultationPrescription(target, medHead);
                        }
                        else if (modifyChoice == 9)
                        {
                            setHospitalizationRecommendation(target);
                        }
                    }
                }
                else
                {
                    std::cout << "未找到看诊ID为 " << conID << " 的看诊记录，无法进行修改操作！" << std::endl;
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
                    deleteConsultation(target);
                }
                else
                {
                    std::cout << "未找到看诊ID为 " << conID << " 的看诊记录，无法删除！" << std::endl;
                }
            }
        }
        else if (choice == 4)
        {
            createConsultationByRegistration(regHead, conHead, medHead, idCounter);
        }
    }
}
