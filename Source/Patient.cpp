/**
 * @file Patient.cpp
 * @brief 医疗管理系统中的患者类实现
 * @details 该源文件实现了 Patient 类的构造、析构、文件操作方法以及患者业务接口。
 * @author 李阳旭 / 小组成员 郭承宇 / 小组成员
 * @date 2026-4-16
 * @version 1.0
 */

#include "../Head/Patient.h"
#include <ctime>
#include "../Head/Doctor.h"
#include "../Head/UI.h"
#include "../Head/GetTime.h"
#include <limits>

Patient::Patient()
{
    // 构造函数实现（如有需要）
}

Patient::~Patient()
{
}

bool Patient::patientSignUp(int &idCounter)
{
    bool success = signUp(5, idCounter); // 调用基类的注册方法，传入角色类型 5（患者）
    if (!success)
    {
        printError("患者注册失败！");
        return false;
    }
    this->patientID = this->userID; // 患者ID与用户ID保持一致

    // 其他患者特有信息的初始化在这里进行
    this->address = inputStringCheck("请输入家庭住址: ");
    this->idCardNumber = inputIDcardCheck("请输入身份证号: ");
    this->emergencyContactName = inputStringCheck("请输入紧急联系人姓名: ");
    this->emergencyContactPhone = inputTelephoneCheck("请输入紧急联系人电话: ");
    this->allergyHistory = inputStringCheck("请输入过敏史（如无请填写“无”）: ");
    this->pastMedicalHistory = inputStringCheck("请输入既往病史（如无请填写“无”）: ");

    std::cout << "请选择婚姻状况:" << std::endl;
    std::cout << "1. 未婚" << std::endl;
    std::cout << "2. 已婚" << std::endl;
    std::cout << "3. 离异" << std::endl;
    std::cout << "4. 丧偶" << std::endl;
    int maritalChoice = selectIntCheck(1, 4);
    this->maritalStatus = static_cast<MaritalStatus>(maritalChoice);

    printSuccess("患者注册成功! 您的用户ID是: " + this->userID);
    return true;
}

bool Patient::patientSignIn()
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
            printError("退出登录。");
            return false;
        }

        bool success = SHA256Verify(pwd, storedHash, kHashIterations);

        if (success)
        {
            loginAttempts = 0;
            isLoggedIn = true;
            printSuccess("患者登录成功！");
            return true;
        }
        else
        {
            loginAttempts++;
            std::cout << "密码错误! 请重新输入密码(当前失败次数: " << loginAttempts << ")" << std::endl;

            if (loginAttempts >= kMaxLoginAttempts)
            {
                isAccountActive = false;
                printError("连续登录失败次数过多，账户已锁定，请联系系统管理员解锁！");
            }
        }
    }

    return false; // 登录失败
}

std::string Patient::getPatientID()
{
    return patientID;
}
std::string Patient::getAddress()
{
    return address;
}
std::string Patient::getIdCardNumber()
{
    return idCardNumber;
}
std::string Patient::getEmergencyContactName()
{
    return emergencyContactName;
}
std::string Patient::getEmergencyContactPhone()
{
    return emergencyContactPhone;
}
std::string Patient::getAllergyHistory()
{
    return allergyHistory;
}
std::string Patient::getPastMedicalHistory()
{
    return pastMedicalHistory;
}
MaritalStatus Patient::getMaritalStatus()
{
    return maritalStatus;
}
int Patient::getRegistrationCount()
{
    return registrationCount;
}
int Patient::getConsultationCount()
{
    return consultationCount;
}
int Patient::getHospitalizationCount()
{
    return hospitalizationCount;
}
int Patient::getMedicationCount()
{
    return medicationCount;
}

void Patient::setPatientID(const std::string &id)
{
    patientID = id;
}
void Patient::setAddress(const std::string &addr)
{
    address = addr;
}
void Patient::setIdCardNumber(const std::string &idCard)
{
    idCardNumber = idCard;
}
void Patient::setEmergencyContactName(const std::string &name)
{
    emergencyContactName = name;
}
void Patient::setEmergencyContactPhone(const std::string &phone)
{
    emergencyContactPhone = phone;
}
void Patient::setAllergyHistory(const std::string &history)
{
    allergyHistory = history;
}
void Patient::setPastMedicalHistory(const std::string &history)
{
    pastMedicalHistory = history;
}
void Patient::setMaritalStatus(MaritalStatus status)
{
    maritalStatus = status;
}
void Patient::setRegistrationCount(int count)
{
    registrationCount = count;
}
void Patient::setConsultationCount(int count)
{
    consultationCount = count;
}
void Patient::setHospitalizationCount(int count)
{
    hospitalizationCount = count;
}
void Patient::setMedicationCount(int count)
{
    medicationCount = count;
}

// 患者挂号信息管理
bool Patient::getAllRegistrations(Registration *&regHead)
{
    std::cout << "正在获取所有挂号信息..." << std::endl;
    Registration *current = regHead;
    bool found = false;

    while (current != nullptr)
    {
        if (!current->isDeleted && current->patientID == this->patientID)
        {
            std::cout << "挂号ID: " << current->registrationID
                      << ", 医生ID: " << current->doctorID
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
        printError("没有找到任何挂号记录。");
    }
    return found;
}
bool Patient::getRegistrationsByStatus(Registration *&regHead, int select)
{
    int statusChoice;
    if (select == -1)
    {
        std::cout << "请输入要查询的挂号状态 (0-已预约, 1-已支付, 2-已取消, 3-已完成): ";
        statusChoice = selectIntCheck(0, 3);
    }
    else
    {
        statusChoice = select;
    }

    RegistrationStatus targetStatus = static_cast<RegistrationStatus>(statusChoice);

    std::cout << "正在获取挂号状态为 " << regStatusToString(targetStatus) << " 的记录..." << std::endl;

    Registration *current = regHead;
    bool found = false;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->patientID == this->patientID && current->status == targetStatus)
        {
            std::cout << "挂号ID: " << current->registrationID
                      << ", 医生ID: " << current->doctorID
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
        printError("没有找到状态为 " + std::string(regStatusToString(targetStatus)) + " 的挂号记录。");
    }
    return found;
}
bool Patient::getRegistrationsByDepartment(Registration *&regHead)
{
    std::string targetDept = inputDepartmentCheck("请输入要查询的科室: ");

    std::cout << "正在获取科室为 " << targetDept << " 的挂号记录..." << std::endl;

    Registration *current = regHead;
    bool found = false;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->patientID == this->patientID && current->department == targetDept)
        {
            std::cout << "挂号ID: " << current->registrationID
                      << ", 医生ID: " << current->doctorID
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
        printError("没有找到科室为 " + targetDept + " 的挂号记录。");
    }
    return found;
}
bool Patient::getRegistrationsByDoctorID(Registration *&regHead)
{
    std::string docID = inputIDCheck("请输入要查询的医生ID: ");

    std::cout << "正在获取医生ID为 " << docID << " 的挂号记录..." << std::endl;

    Registration *current = regHead;
    bool found = false;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->patientID == this->patientID && current->doctorID == docID)
        {
            std::cout << "挂号ID: " << current->registrationID
                      << ", 医生ID: " << current->doctorID
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
        printError("没有找到医生ID为 " + docID + " 的挂号记录。");
    }
    return found;
}
bool Patient::getRegistrationsByTimeRange(Registration *&regHead)
{
    std::string startDate = inputDateCheck("请输入查询的开始日期: ");
    std::string endDate = inputDateCheck("请输入查询的结束日期: ");

    std::cout << "正在获取挂号日期在 " << startDate << " 到 " << endDate << " 之间的挂号记录..." << std::endl;

    Registration *current = regHead;
    bool found = false;
    while (current != nullptr)
    {
        std::string regDate = current->registerTime.substr(0, 10);
        if (!current->isDeleted && current->patientID == this->patientID && regDate >= startDate && regDate <= endDate)
        {
            std::cout << "挂号ID: " << current->registrationID
                      << ", 医生ID: " << current->doctorID
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
        printError("没有找到指定时间范围内的挂号记录。");
    }
    return found;
}
bool Patient::getRegistrationsByID(Registration *&regHead)
{
    std::string regID = inputRecordIDCheck("请输入要查询的挂号ID: ", {"reg"});

    std::cout << "正在获取挂号ID为 " << regID << " 的挂号记录..." << std::endl;

    Registration *current = regHead;
    bool found = false;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->patientID == this->patientID && current->registrationID == regID)
        {
            std::cout << "挂号ID: " << current->registrationID
                      << ", 医生ID: " << current->doctorID
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
        printError("没找到挂号ID为 " + regID + " 的挂号记录。");
    }
    return found;
}
bool Patient::appointRegistration(Registration *&regHead, Doctor *&doctorHead, int &idCounter)
{
    std::string dept = inputDepartmentCheck("请输入要挂号的科室: ");

    Doctor *docCurrent = doctorHead;
    bool hasDoctor = false;
    std::cout << "该科室可以挂号的医生列表:" << std::endl;
    while (docCurrent != nullptr)
    {
        if (!docCurrent->isDeleted && docCurrent->isOnDuty && docCurrent->department == dept)
        {
            std::cout << "医生ID: " << docCurrent->doctorID
                      << "，姓名: " << docCurrent->getUsername()
                      << "，职称: " << doctorTitleToString(docCurrent->getTitle())
                      << "，科室: " << docCurrent->department << std::endl;
            hasDoctor = true;
        }
        docCurrent = docCurrent->next;
    }
    if (!hasDoctor)
    {
        printError("该科室暂时没有可预约的医生！");
        return false;
    }

    std::string docID = inputIDCheck("请输入要预约的医生ID: ");
    docCurrent = doctorHead;
    bool docFound = false;
    while (docCurrent != nullptr)
    {
        if (!docCurrent->isDeleted && docCurrent->doctorID == docID && docCurrent->department == dept)
        {
            docFound = true;
            break;
        }
        docCurrent = docCurrent->next;
    }
    if (!docFound)
    {
        printError("医生ID不存在或不属于该科室！");
        return false;
    }

    Registration *newReg = new Registration();
    newReg->registrationID = "reg" + std::to_string(idCounter).insert(0, 6 - std::to_string(idCounter).length(), '0');
    idCounter++;
    newReg->patientID = this->patientID;
    newReg->doctorID = docID;
    newReg->department = dept;
    MyTime &t = MyTime::getInstance();
    newReg->registerTime = t.getTime();
    newReg->fee = calculateRegistrationFee(docCurrent->getTitle());

    newReg->status = RegistrationStatus::BOOKED;
    newReg->note = inputStringCheck("请输入备注(如果没有请填“无”): ");
    newReg->isDeleted = false;

    // 将新挂号记录插入到挂号链表头部
    newReg->next = regHead;
    if (regHead != nullptr)
    {
        regHead->prev = newReg;
    }
    regHead = newReg;

    this->registrationCount++;

    printSuccess("预约挂号成功！挂号记录ID为: " + std::string(newReg->registrationID));
    return true;
}
void Patient::cancelRegistration(Registration *&target)
{
    if (target->status == RegistrationStatus::FINISHED || target->status == RegistrationStatus::CANCELED)
    {
        printError("该挂号记录已经完成或取消，无法撤回申请！");
        return;
    }
    else if (target->status == RegistrationStatus::PAID)
    {
        printError("该挂号记录已经支付，无法撤回申请！");
        return;
    }

    std::cout << "是否确定撤回申请? : " << std::endl;
    std::cout << "1. 确定" << std::endl;
    std::cout << "0. 取消" << std::endl;

    int c = selectIntCheck(0, 1);
    if (c == 1)
    {
        target->status = RegistrationStatus::CANCELED;
        printSuccess("您已成功撤回该挂号申请。");
    }
}
void Patient::payRegistrationFee(Registration *&target)
{
    if (target->status == RegistrationStatus::PAID)
    {
        printWarning("您已经支付过费用了，无需再次支付。");
        return;
    }
    else if (target->status == RegistrationStatus::CANCELED)
    {
        printError("此记录已取消，无法支付。");
        return;
    }
    else if (target->status == RegistrationStatus::FINISHED)
    {
        printWarning("此记录已经完成并支付过了。");
        return;
    }

    std::cout << "需支付的费用为 " << target->fee << " 元。您当前账户余额为 " << this->balance << " 元。" << std::endl;
    if (this->balance < target->fee)
    {
        printError("余额不足，无法支付挂号费用！请先进行充值。");
        return;
    }

    std::cout << "是否确认支付? : " << std::endl;
    std::cout << "1. 确定" << std::endl;
    std::cout << "0. 取消" << std::endl;

    int c = selectIntCheck(0, 1);
    if (c == 1)
    {
        this->balance -= target->fee;
        target->status = RegistrationStatus::PAID;
        printSuccess("支付成功！当前账户余额: " + std::to_string(this->balance) + " 元。");
    }
}

void Patient::manageRegistrations(Registration *&regHead, Doctor *&doctorHead, int &idCounter)
{
    while (true)
    {
        int choice = patientRegistrationManagementMenu();
        switch (choice)
        {
        case 1:
        {
            int viewChoice = patientRegistrationViewMenu();
            switch (viewChoice)
            {
            case 1:
                getAllRegistrations(regHead);
                break;
            case 2:
                getRegistrationsByID(regHead);
                break;
            case 3:
                getRegistrationsByDoctorID(regHead);
                break;
            case 4:
                getRegistrationsByStatus(regHead);
                break;
            case 5:
                getRegistrationsByTimeRange(regHead);
                break;
            case 6:
                getRegistrationsByDepartment(regHead);
                break;
            case 0:
                break;
            }
            break;
        }
        case 2:
        {
            appointRegistration(regHead, doctorHead, idCounter);
            break;
        }
        case 3:
        {
            bool ishave = getRegistrationsByStatus(regHead, 0); // 查询已预约状态的挂号记录

            if (ishave)
            {
                std::string regID = inputRecordIDCheck("请输入要撤回申请的挂号ID: ", {"reg"});
                Registration *current = regHead;

                while (current != nullptr)
                {
                    if (!current->isDeleted && current->patientID == this->patientID && current->registrationID == regID)
                    {
                        break;
                    }
                    current = current->next;
                }
                if (current)
                {
                    std::cout << "当前要撤销的挂号记录信息如下:" << std::endl;

                    std::cout << "挂号ID: " << current->registrationID
                              << ", 医生ID: " << current->doctorID
                              << ", 科室: " << current->department
                              << ", 挂号时间: " << current->registerTime
                              << ", 费用: " << current->fee
                              << ", 状态: " << regStatusToString(current->status)
                              << ", 备注: " << current->note
                              << std::endl;

                    std::cout << "您确定要撤销该挂号申请吗？" << std::endl;
                    std::cout << "1. 确定" << std::endl;
                    std::cout << "0. 取消" << std::endl;
                    int c = selectIntCheck(0, 1);

                    if (c == 1)
                    {
                        cancelRegistration(current);
                    }
                    else
                    {
                        std::cout << "已取消撤销操作。" << std::endl;
                    }
                }
                else
                {
                    printError("没有找到属于您的挂号ID为 " + regID + " 的挂号记录。");
                }
                break;
            }
        }
        case 4:
        {
            bool ishave = getRegistrationsByStatus(regHead, 0); // 查询已预约状态的挂号记录

            if (ishave)
            {
                std::string regID = inputRecordIDCheck("请输入要缴费的挂号ID: ", {"reg"});
                Registration *current = regHead;
                while (current != nullptr)
                {
                    if (!current->isDeleted && current->patientID == this->patientID && current->registrationID == regID)
                    {
                        break;
                    }
                    current = current->next;
                }
                if (current)
                {
                    std::cout << "当前要支付的挂号记录信息如下:" << std::endl;
                    std::cout << "挂号ID: " << current->registrationID
                              << ", 医生ID: " << current->doctorID
                              << ", 科室: " << current->department
                              << ", 挂号时间: " << current->registerTime
                              << ", 费用: " << current->fee
                              << ", 状态: " << regStatusToString(current->status)
                              << ", 备注: " << current->note
                              << std::endl;

                    std::cout << "您确定要支付该挂号费用吗？" << std::endl;
                    std::cout << "1. 确定" << std::endl;
                    std::cout << "0. 取消" << std::endl;
                    int c = selectIntCheck(0, 1);

                    if (c == 1)
                    {
                        payRegistrationFee(current);
                    }
                    else
                    {
                        printWarning("已取消支付操作。");
                    }
                }
                else
                {
                    printError("没有找到属于您的挂号ID为 " + regID + " 的挂号记录。");
                }
            }

            break;
        }
        case 0:
            return;
        }
    }
}

// ---------------- 患者看诊信息管理 ----------------

bool Patient::getAllConsultations(Consultation *&conHead)
{
    std::cout << "正在获取所有看诊信息..." << std::endl;
    Consultation *current = conHead;
    bool found = false;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->patientID == this->patientID)
        {
            std::cout << "看诊ID: " << current->consultationID
                      << ", 挂号ID: " << current->registrationID
                      << ", 医生ID: " << current->doctorID
                      << ", 科室: " << current->department
                      << ", 时间: " << current->consultationTime
                      << ", 状态: " << conStatusToString(current->status)
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
            found = true;
        }
        current = current->next;
    }
    if (!found)
        printError("没有找到任何看诊记录。");
    return found;
}
bool Patient::getConsultationsByID(Consultation *&conHead)
{
    std::string conID = inputRecordIDCheck("请输入要查询的看诊ID: ", {"con"});
    Consultation *current = conHead;
    bool found = false;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->patientID == this->patientID && current->consultationID == conID)
        {
            std::cout << "看诊ID: " << current->consultationID
                      << ", 挂号ID: " << current->registrationID
                      << ", 医生ID: " << current->doctorID
                      << ", 科室: " << current->department
                      << ", 时间: " << current->consultationTime
                      << ", 状态: " << conStatusToString(current->status)
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
            found = true;
        }
        current = current->next;
    }
    if (!found)
        printError("没找到看诊ID为 " + conID + " 的看诊记录。");
    return found;
}
bool Patient::getConsultationsByDoctorID(Consultation *&conHead)
{
    std::string docID = inputIDCheck("请输入要查询的医生ID: ");
    Consultation *current = conHead;
    bool found = false;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->patientID == this->patientID && current->doctorID == docID)
        {
            std::cout << "看诊ID: " << current->consultationID
                      << ", 挂号ID: " << current->registrationID
                      << ", 医生ID: " << current->doctorID
                      << ", 科室: " << current->department
                      << ", 时间: " << current->consultationTime
                      << ", 状态: " << conStatusToString(current->status)
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
            found = true;
        }
        current = current->next;
    }
    if (!found)
        printError("没找到医生ID为 " + docID + " 的看诊记录。");
    return found;
}
bool Patient::getConsultationsByStatus(Consultation *&conHead)
{
    std::cout << "请输入查询的看诊状态 (0-待就诊, 1-正在处理, 2-已结束, 3-已作废): ";
    int choice = selectIntCheck(0, 3);
    ConsultationStatus targetStatus = static_cast<ConsultationStatus>(choice);
    Consultation *current = conHead;
    bool found = false;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->patientID == this->patientID && current->status == targetStatus)
        {
            std::cout << "看诊ID: " << current->consultationID
                      << ", 挂号ID: " << current->registrationID
                      << ", 医生ID: " << current->doctorID
                      << ", 科室: " << current->department
                      << ", 时间: " << current->consultationTime
                      << ", 状态: " << conStatusToString(current->status)
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
            found = true;
        }
        current = current->next;
    }
    if (!found)
        printError("没找到对应状态的看诊记录。");
    return found;
}
bool Patient::getConsultationsByTimeRange(Consultation *&conHead)
{
    std::string startDate = inputDateCheck("请输入查询的开始日期: ");
    std::string endDate = inputDateCheck("请输入查询的结束日期: ");
    Consultation *current = conHead;
    bool found = false;
    while (current != nullptr)
    {
        std::string conDate = current->consultationTime.substr(0, 10);
        if (!current->isDeleted && current->patientID == this->patientID && conDate >= startDate && conDate <= endDate)
        {
            std::cout << "看诊ID: " << current->consultationID
                      << ", 挂号ID: " << current->registrationID
                      << ", 医生ID: " << current->doctorID
                      << ", 科室: " << current->department
                      << ", 时间: " << current->consultationTime
                      << ", 状态: " << conStatusToString(current->status)
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
            found = true;
        }
        current = current->next;
    }
    if (!found)
        printError("没找到指定时间范围的看诊记录。");
    return found;
}
bool Patient::getConsultationsByChiefComplaint(Consultation *&conHead)
{
    std::string keyword = inputStringCheck("请输入主诉包含的关键词: ");
    Consultation *current = conHead;
    bool found = false;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->patientID == this->patientID && current->chiefComplaint.find(keyword) != std::string::npos)
        {
            std::cout << "看诊ID: " << current->consultationID
                      << ", 挂号ID: " << current->registrationID
                      << ", 医生ID: " << current->doctorID
                      << ", 科室: " << current->department
                      << ", 时间: " << current->consultationTime
                      << ", 状态: " << conStatusToString(current->status)
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
            found = true;
        }
        current = current->next;
    }
    if (!found)
        printError("没找到主诉包含该关键词的看诊记录。");
    return found;
}
bool Patient::getConsultationsByDepartment(Consultation *&conHead)
{
    std::string dept = inputDepartmentCheck("请输入要查询的科室: ");

    Consultation *current = conHead;
    bool found = false;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->patientID == this->patientID && current->department.find(dept) != std::string::npos)
        {
            std::cout << "看诊ID: " << current->consultationID
                      << ", 挂号ID: " << current->registrationID
                      << ", 医生ID: " << current->doctorID
                      << ", 科室: " << current->department
                      << ", 时间: " << current->consultationTime
                      << ", 状态: " << conStatusToString(current->status)
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
            found = true;
        }
        current = current->next;
    }
    if (!found)
        printError("没找到科室包含 " + dept + " 的看诊记录。");
    return found;
}

void Patient::manageConsultations(Consultation *&conHead)
{
    while (true)
    {
        int choice = patientConsultationManagementMenu();
        if (choice == 0)
            return;
        if (choice == 1)
        {
            int viewChoice = patientConsultationViewMenu();
            switch (viewChoice)
            {
            case 1:
                getAllConsultations(conHead);
                break;
            case 2:
                getConsultationsByID(conHead);
                break;
            case 3:
                getConsultationsByDoctorID(conHead);
                break;
            case 4:
                getConsultationsByStatus(conHead);
                break;
            case 5:
                getConsultationsByTimeRange(conHead);
                break;
            case 6:
                getConsultationsByChiefComplaint(conHead);
                break;
            case 7:
                getConsultationsByDepartment(conHead);
                break;
            case 0:
                break;
            }
        }
    }
}

// ---------------- 患者检查信息管理 ----------------

bool Patient::getAllExaminations(Examination *&examHead)
{
    std::cout << "正在获取所有检查信息..." << std::endl;
    Examination *current = examHead;
    bool found = false;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->patientID == this->patientID)
        {
            std::cout << "检查ID: " << current->examinationID
                      << ", 看诊ID: " << current->consultationID
                      << ", 医生ID: " << current->doctorID
                      << ", 科室: " << current->department
                      << ", 检查项目: " << current->itemName
                      << ", 检查结果: " << findVitalSignToString(current)
                      << ", 下单时间: " << current->orderTime
                      << ", 报告时间: " << current->reportTime
                      << ", 费用: " << current->fee
                      << ", 状态: " << examStatusToString(current->status)
                      << ", 报告摘要: " << current->reportSummary
                      << ", 备注: " << current->note;

            std::cout << ", 附件信息: ";
            for (const auto &file : current->attachments)
            {
                std::cout << file << " ";
            }
            std::cout << std::endl;

            found = true;
        }
        current = current->next;
    }
    if (!found)
        printError("没有找到任何检查记录。");
    return found;
}
bool Patient::getExaminationsByID(Examination *&examHead)
{
    std::string examID = inputRecordIDCheck("请输入要查询的检查ID: ", {"exa"});
    Examination *current = examHead;
    bool found = false;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->patientID == this->patientID && current->examinationID == examID)
        {
            std::cout << "检查ID: " << current->examinationID
                      << ", 看诊ID: " << current->consultationID
                      << ", 医生ID: " << current->doctorID
                      << ", 科室: " << current->department
                      << ", 检查项目: " << current->itemName
                      << ", 检查结果: " << findVitalSignToString(current)
                      << ", 下单时间: " << current->orderTime
                      << ", 报告时间: " << current->reportTime
                      << ", 费用: " << current->fee
                      << ", 状态: " << examStatusToString(current->status)
                      << ", 报告摘要: " << current->reportSummary
                      << ", 备注: " << current->note;

            std::cout << ", 附件信息: ";
            for (const auto &file : current->attachments)
            {
                std::cout << file << " ";
            }
            std::cout << std::endl;

            found = true;
            break;
        }
        current = current->next;
    }
    if (!found)
        printError("没找到检查ID为 " + examID + " 的检查记录。");
    return found;
}
bool Patient::getExaminationsByConsultationID(Examination *&examHead)
{
    std::string conID = inputRecordIDCheck("请输入要查询的看诊ID: ", {"con"});
    Examination *current = examHead;
    bool found = false;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->patientID == this->patientID && current->consultationID == conID)
        {
            std::cout << "检查ID: " << current->examinationID
                      << ", 看诊ID: " << current->consultationID
                      << ", 医生ID: " << current->doctorID
                      << ", 科室: " << current->department
                      << ", 检查项目: " << current->itemName
                      << ", 检查结果: " << findVitalSignToString(current)
                      << ", 下单时间: " << current->orderTime
                      << ", 报告时间: " << current->reportTime
                      << ", 费用: " << current->fee
                      << ", 状态: " << examStatusToString(current->status)
                      << ", 报告摘要: " << current->reportSummary
                      << ", 备注: " << current->note
                      << ", 附件信息: ";
            for (const auto &file : current->attachments)
            {
                std::cout << file << " ";
            }
            std::cout << std::endl;
            found = true;
        }
        current = current->next;
    }
    if (!found)
        printError("没找到看诊ID为 " + conID + " 的检查记录。");
    return found;
}
bool Patient::getExaminationsByItemName(Examination *&examHead)
{
    std::string item = ExaminationItemMenu();

    if (item == "0")
    {
        std::cout << "已取消查询。" << std::endl;
        return false;
    }

    Examination *current = examHead;
    bool found = false;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->patientID == this->patientID && current->itemName.find(item) != std::string::npos)
        {
            std::cout << "检查ID: " << current->examinationID
                      << ", 看诊ID: " << current->consultationID
                      << ", 医生ID: " << current->doctorID
                      << ", 科室: " << current->department
                      << ", 检查项目: " << current->itemName
                      << ", 检查结果: " << findVitalSignToString(current)
                      << ", 下单时间: " << current->orderTime
                      << ", 报告时间: " << current->reportTime
                      << ", 费用: " << current->fee
                      << ", 状态: " << examStatusToString(current->status)
                      << ", 报告摘要: " << current->reportSummary
                      << ", 备注: " << current->note
                      << ", 附件信息: ";
            for (const auto &file : current->attachments)
            {
                std::cout << file << " ";
            }
            std::cout << std::endl;
            found = true;
        }
        current = current->next;
    }
    if (!found)
        printError("没找到检查项目包含该关键词的检查记录。");
    return found;
}
bool Patient::getExaminationsByStatus(Examination *&examHead, int select)
{
    int choice = select;
    if (choice == -1)
    {
        std::cout << "请输入查询的检查状态: ";
        std::cout << "1. 待支付" << std::endl;
        std::cout << "2. 已支付" << std::endl;
        std::cout << "3. 检查中" << std::endl;
        std::cout << "4. 检查完成" << std::endl;
        std::cout << "5. 报告已出" << std::endl;
        std::cout << "6. 已作废" << std::endl;
        std::cout << "0. 取消查询" << std::endl;

        choice = selectIntCheck(0, 6);

        if (choice == 0)
        {
            std::cout << "已取消查询。" << std::endl;
            return false;
        }
    }
    ExaminationStatus targetStatus = static_cast<ExaminationStatus>(choice);
    Examination *current = examHead;
    bool found = false;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->patientID == this->patientID && current->status == targetStatus)
        {
            std::cout << "检查ID: " << current->examinationID
                      << ", 看诊ID: " << current->consultationID
                      << ", 医生ID: " << current->doctorID
                      << ", 科室: " << current->department
                      << ", 检查项目: " << current->itemName
                      << ", 检查结果: " << findVitalSignToString(current)
                      << ", 下单时间: " << current->orderTime
                      << ", 报告时间: " << current->reportTime
                      << ", 费用: " << current->fee
                      << ", 状态: " << examStatusToString(current->status)
                      << ", 报告摘要: " << current->reportSummary
                      << ", 备注: " << current->note
                      << ", 附件信息: ";
            for (const auto &file : current->attachments)
            {
                std::cout << file << " ";
            }
            std::cout << std::endl;
            found = true;
        }
        current = current->next;
    }
    if (!found)
        printError("没找到状态为 " + std::string(examStatusToString(targetStatus)) + " 的检查记录。");
    return found;
}
bool Patient::getExaminationsByTimeRange(Examination *&examHead)
{
    std::string startDate = inputDateCheck("请输入查询的开始日期: ");
    std::string endDate = inputDateCheck("请输入查询的结束日期: ");
    Examination *current = examHead;
    bool found = false;
    while (current != nullptr)
    {
        std::string orderDate = current->orderTime.substr(0, 10);
        if (!current->isDeleted && current->patientID == this->patientID && orderDate >= startDate && orderDate <= endDate)
        {
            std::cout << "检查ID: " << current->examinationID
                      << ", 看诊ID: " << current->consultationID
                      << ", 医生ID: " << current->doctorID
                      << ", 科室: " << current->department
                      << ", 检查项目: " << current->itemName
                      << ", 检查结果: " << findVitalSignToString(current)
                      << ", 下单时间: " << current->orderTime
                      << ", 报告时间: " << current->reportTime
                      << ", 费用: " << current->fee
                      << ", 状态: " << examStatusToString(current->status)
                      << ", 报告摘要: " << current->reportSummary
                      << ", 备注: " << current->note
                      << ", 附件信息: ";
            for (const auto &file : current->attachments)
            {
                std::cout << file << " ";
            }
            std::cout << std::endl;
            found = true;
        }
        current = current->next;
    }
    if (!found)
        printError("没找到指定时间范围内的检查记录。");
    return found;
}
void Patient::payExaminationFee(Examination *&target)
{
    if (target->status == ExaminationStatus::PAID)
    {
        printWarning("您已经支付过该检查的费用了，无需再次支付。");
        return;
    }
    else if (target->status != ExaminationStatus::ORDERED)
    {
        printError("此记录当前状态为 " + std::string(examStatusToString(target->status)) + "，无法进行支付。");
        return;
    }

    std::cout << "需支付的费用为 " << target->fee << " 元。您当前账户余额为 " << this->balance << " 元。" << std::endl;
    if (this->balance < target->fee)
    {
        printError("余额不足，无法支付检查费用！请先进行充值。");
        return;
    }

    std::cout << "是否确认支付? : " << std::endl;
    std::cout << "1. 确定" << std::endl;
    std::cout << "0. 取消" << std::endl;

    int c = selectIntCheck(0, 1);
    if (c == 1)
    {
        this->balance -= target->fee;
        target->status = ExaminationStatus::PAID;
        printSuccess("支付成功！当前账户余额: " + std::to_string(this->balance) + " 元。");
    }
    else
    {
        printWarning("已取消支付操作。");
    }
}
bool Patient::getExaminationsByDepartment(Examination *&examHead)
{
    std::string dept = inputDepartmentCheck("请输入要查询的科室: ");
    Examination *current = examHead;
    bool found = false;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->patientID == this->patientID && current->department.find(dept) != std::string::npos)
        {
            std::cout << "检查ID: " << current->examinationID
                      << ", 看诊ID: " << current->consultationID
                      << ", 医生ID: " << current->doctorID
                      << ", 科室: " << current->department
                      << ", 检查项目: " << current->itemName
                      << ", 检查结果: " << findVitalSignToString(current)
                      << ", 下单时间: " << current->orderTime
                      << ", 报告时间: " << current->reportTime
                      << ", 费用: " << current->fee
                      << ", 状态: " << examStatusToString(current->status)
                      << ", 报告摘要: " << current->reportSummary
                      << ", 备注: " << current->note
                      << ", 附件信息: ";
            for (const auto &file : current->attachments)
            {
                std::cout << file << " ";
            }
            std::cout << std::endl;
            found = true;
        }
        current = current->next;
    }
    if (!found)
        printError("没找到科室包含 " + dept + " 的检查记录。");
    return found;
}

void Patient::manageExaminations(Examination *&examHead)
{
    while (true)
    {
        int choice = patientExaminationManagementMenu();
        if (choice == 0)
        {
            return;
        }
        else if (choice == 1)
        {
            int viewChoice = patientExaminationViewMenu();
            switch (viewChoice)
            {
            case 1:
                getAllExaminations(examHead);
                break;
            case 2:
                getExaminationsByID(examHead);
                break;
            case 3:
                getExaminationsByConsultationID(examHead);
                break;
            case 4:
                getExaminationsByItemName(examHead);
                break;
            case 5:
                getExaminationsByStatus(examHead);
                break;
            case 6:
                getExaminationsByTimeRange(examHead);
                break;
            case 7:
                getExaminationsByDepartment(examHead);
                break;
            case 0:
                break;
            }
        }
        else if (choice == 2)
        {
            bool ishave = getExaminationsByStatus(examHead, 0); // 查询已下单状态（0）的检查记录
            if (ishave)
            {
                std::string examID = inputRecordIDCheck("请输入要缴纳费用的检查ID: ", {"exa"});
                Examination *current = examHead;
                while (current != nullptr)
                {
                    if (!current->isDeleted && current->patientID == this->patientID && current->examinationID == examID)
                    {
                        break;
                    }
                    current = current->next;
                }
                if (current)
                {
                    std::cout << "当前要支付的检查记录信息如下:" << std::endl;
                    std::cout << "检查ID: " << current->examinationID
                              << ", 看诊ID: " << current->consultationID
                              << ", 医生ID: " << current->doctorID
                              << ", 科室: " << current->department
                              << ", 检查项目: " << current->itemName
                              << ", 检查结果: " << findVitalSignToString(current)
                              << ", 下单时间: " << current->orderTime
                              << ", 报告时间: " << current->reportTime
                              << ", 费用: " << current->fee
                              << ", 状态: " << examStatusToString(current->status)
                              << ", 报告摘要: " << current->reportSummary
                              << ", 备注: " << current->note
                              << ", 附件信息: ";
                    for (const auto &file : current->attachments)
                    {
                        std::cout << file << " ";
                    }
                    std::cout << std::endl;

                    std::cout << "您确定要支付该检查费用吗？" << std::endl;
                    std::cout << "1. 确定" << std::endl;
                    std::cout << "0. 取消" << std::endl;
                    int c = selectIntCheck(0, 1);

                    if (c == 1)
                    {
                        payExaminationFee(current);
                    }
                    else
                    {
                        printWarning("已取消支付操作。");
                    }
                }
                else
                {
                    printError("没有找到属于您的检查ID为 " + examID + " 的待支付记录。");
                }
            }
        }
    }
}

// ---------------- 患者用药信息管理 ----------------

void Patient::printMedicationRecord(MedicationRecord *current)
{
    std::cout << "========================================" << std::endl;
    std::cout << "用药记录ID: " << current->medRecordID << std::endl
              << "看诊ID: " << current->consultationID << std::endl
              << "医生ID: " << current->doctorID << std::endl
              << "药师ID: " << current->pharmacistID << std::endl
              << "科室: " << current->department << std::endl
              << "创建时间: " << current->createTime << std::endl
              << "总费用: " << current->totalCost << " 元" << std::endl
              << "审核状态: " << medicationReviewStatusToString(current->reviewStatus) << std::endl
              << "支付发药状态: " << medicationStatusToString(current->status) << std::endl
              << "支付时间: " << current->paymentTime << std::endl
              << "发药时间: " << current->dispenseTime << std::endl
              << "备注: " << current->note << std::endl;

    if (!current->lines.empty())
    {
        std::cout << "  [处方药品明细]" << std::endl;
        for (const auto &line : current->lines)
        {
            std::cout << "    =======================" << std::endl
                      << "    药品ID: " << line.medicineID << std::endl
                      << "    药品名称: " << line.medicineName << std::endl
                      << "    数量: " << line.quantity << std::endl
                      << "    单价: " << line.unitPrice << " 元" << std::endl
                      << "    总价: " << line.quantity * line.unitPrice << " 元" << std::endl
                      << "    用法备注: " << line.note << std::endl;
        }
        std::cout << "    =======================" << std::endl;
    }
    else
    {
        std::cout << "  [暂无药品明细]" << std::endl;
    }
    std::cout << "========================================" << std::endl;
}

bool Patient::getAllMedications(MedicationRecord *&medHead)
{
    std::cout << "正在获取所有用药信息..." << std::endl;
    MedicationRecord *current = medHead;
    bool found = false;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->patientID == this->patientID)
        {
            printMedicationRecord(current);
            found = true;
        }
        current = current->next;
    }
    if (!found)
        printError("没找到任何用药记录。");
    return found;
}
bool Patient::getMedicationsByID(MedicationRecord *&medHead)
{
    std::string medID = inputRecordIDCheck("请输入要查询的用药记录ID: ", {"mrd"});
    MedicationRecord *current = medHead;
    bool found = false;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->patientID == this->patientID && current->medRecordID == medID)
        {
            printMedicationRecord(current);
            found = true;
            break;
        }
        current = current->next;
    }
    if (!found)
        printError("没找到用药记录ID为 " + medID + " 的记录。");
    return found;
}
bool Patient::getMedicationsByConsultationID(MedicationRecord *&medHead)
{
    std::string conID = inputRecordIDCheck("请输入要查询的看诊记录ID: ", {"con"});
    MedicationRecord *current = medHead;
    bool found = false;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->patientID == this->patientID && current->consultationID == conID)
        {
            printMedicationRecord(current);
            found = true;
        }
        current = current->next;
    }
    if (!found)
        printError("没找到看诊记录ID为 " + conID + " 的用药记录。");
    return found;
}
bool Patient::getMedicationsByMedicineName(MedicationRecord *&medHead)
{
    std::string medName = inputStringCheck("请输入要查询的药品名称关键词: ");

    MedicationRecord *current = medHead;
    bool found = false;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->patientID == this->patientID)
        {
            bool hasMedicine = false;
            for (const auto &line : current->lines)
            {
                if (line.medicineName.find(medName) != std::string::npos)
                {
                    hasMedicine = true;
                    break;
                }
            }
            if (hasMedicine)
            {
                printMedicationRecord(current);
                found = true;
            }
        }
        current = current->next;
    }
    if (!found)
        printError("没有找到包含药品 '" + medName + "' 的用药记录。");
    return found;
}
bool Patient::getMedicationsByStatus(MedicationRecord *&medHead, int select)
{
    int sChoice;
    if (select == -1)
    {
        std::cout << "请选择要查询的用药发药状态: " << std::endl;
        std::cout << "1. 未缴费" << std::endl;
        std::cout << "2. 待发药" << std::endl;
        std::cout << "3. 已发药" << std::endl;
        std::cout << "4. 已退药" << std::endl;
        sChoice = selectIntCheck(1, 4);
    }
    else
    {
        sChoice = select;
    }

    MedicationRecord *current = medHead;
    bool found = false;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->patientID == this->patientID && static_cast<int>(current->status) == sChoice)
        {
            printMedicationRecord(current);
            found = true;
        }
        current = current->next;
    }
    if (!found)
        printError("没有找到相关状态的用药记录。");
    return found;
}
bool Patient::getMedicationsByTimeRange(MedicationRecord *&medHead)
{
    std::string startTime = inputDateCheck("请输入查询的开始时间: ");
    std::string endTime = inputDateCheck("请输入查询的结束时间: ");

    MedicationRecord *current = medHead;
    bool found = false;
    while (current != nullptr)
    {
        std::string orderDate = current->createTime.substr(0, 10);

        if (!current->isDeleted && current->patientID == this->patientID && current->createTime >= startTime && current->createTime <= endTime)
        {
            printMedicationRecord(current);
            found = true;
        }
        current = current->next;
    }
    if (!found)
        printError("没找到从 " + startTime + " 到 " + endTime + " 的用药记录。");
    return found;
}
bool Patient::getMedicationsByDepartment(MedicationRecord *&medHead)
{
    std::string dept = inputDepartmentCheck("请输入要查询的科室: ");
    MedicationRecord *current = medHead;
    bool found = false;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->patientID == this->patientID && current->department.find(dept) != std::string::npos)
        {
            printMedicationRecord(current);
            found = true;
        }
        current = current->next;
    }
    if (!found)
        printError("没找到科室 " + dept + " 的用药记录。");
    return found;
}
void Patient::payMedicationFee(MedicationRecord *&target)
{
    if (target->status == MedicationStatus::PAID || target->status == MedicationStatus::DISPENSED)
    {
        printWarning("您已经支付过该药品的费用了，无需再次支付。");
        return;
    }
    else if (target->status != MedicationStatus::UNPAID)
    {
        printError("此记录当前发药状态为 " + std::string(medicationStatusToString(target->status)) + "，无法进行支付。");
        return;
    }

    if (target->reviewStatus != MedicationReviewStatus::APPROVED)
    {
        printError("此记录审核状态为 " + std::string(medicationReviewStatusToString(target->reviewStatus)) + "，无法进行支付。");
        return;
    }

    std::cout << "需支付的费用为 " << target->totalCost << " 元。您当前账户余额为 " << this->balance << " 元。" << std::endl;
    if (this->balance < target->totalCost)
    {
        printError("余额不足，无法支付用药费用！请先进行充值。");
        return;
    }

    std::cout << "是否确认支付? (1. 确定 / 0. 取消): " << std::endl;
    int confirm = selectIntCheck(0, 1);
    if (confirm == 1)
    {
        this->balance -= target->totalCost;
        target->status = MedicationStatus::PAID; // 已缴费未发药

        MyTime &t = MyTime::getInstance();
        target->paymentTime = t.getTime();

        printSuccess("支付成功！扣除费用 " + std::to_string(target->totalCost) + " 元。");
        printSuccess("当前账户余额: " + std::to_string(this->balance) + " 元");
        printSuccess("请移步药房窗口取药。");
    }
    else
    {
        printWarning("已取消支付操作。");
    }
}

void Patient::manageMedications(MedicationRecord *&medHead)
{
    while (true)
    {
        int choice = patientMedicationManagementMenu();
        if (choice == 0)
        {
            return;
        }
        else if (choice == 1)
        {
            int viewChoice = patientMedicationViewMenu();
            switch (viewChoice)
            {
            case 1:
                getAllMedications(medHead);
                break;
            case 2:
                getMedicationsByID(medHead);
                break;
            case 3:
                getMedicationsByConsultationID(medHead);
                break;
            case 4:
                getMedicationsByMedicineName(medHead);
                break;
            case 5:
                getMedicationsByStatus(medHead);
                break;
            case 6:
                getMedicationsByTimeRange(medHead);
                break;
            case 7:
                getMedicationsByDepartment(medHead);
                break;
            case 0:
                break;
            }
        }
        else if (choice == 2)
        {
            bool ishave = getMedicationsByStatus(medHead, 1); // 1 对应 UNPAID 未缴费状态
            if (ishave)
            {
                std::string medID = inputRecordIDCheck("请输入要缴纳费用的用药记录ID: ", {"med"});
                MedicationRecord *current = medHead;
                while (current != nullptr)
                {
                    if (!current->isDeleted && current->patientID == this->patientID && current->medRecordID == medID)
                    {
                        break;
                    }
                    current = current->next;
                }
                if (current)
                {
                    std::cout << "当前要支付的用药记录信息如下:" << std::endl;
                    printMedicationRecord(current);

                    std::cout << "您确定要支付该用药费用吗？" << std::endl;
                    std::cout << "1. 确定" << std::endl;
                    std::cout << "0. 取消" << std::endl;
                    int c = selectIntCheck(0, 1);

                    if (c == 1)
                    {
                        payMedicationFee(current);
                    }
                    else
                    {
                        printWarning("已取消支付操作。");
                    }
                }
                else
                {
                    printError("没有找到属于您的用药记录ID为 " + medID + " 的记录。");
                }
            }
        }
    }
}

// ---------------- 住院信息管理功能实现 ----------------

static void printHospitalizationRecord(Hospitalization *current, User *u)
{
    std::cout << "========================================" << std::endl;
    std::cout << "住院记录ID: " << current->hospitalizationID << std::endl
              << "关联看诊ID: " << current->consultationID << std::endl
              << "申请医生ID: " << current->doctorID << std::endl
              << "负责护士ID: " << current->nurseID << std::endl
              << "所属科室: " << current->department << std::endl
              << "病房类型: " << current->wardType << std::endl
              << "床位号: " << current->bedNumber << std::endl
              << "申请时间: " << current->applyTime << std::endl
              << "可入住时间: " << current->availableAdmitTime << std::endl
              << "实际入院时间: " << current->admitTime << std::endl
              << "出院时间: " << current->dischargeTime << std::endl
              << "押金: " << current->deposit << " 元" << std::endl
              << "总费用: " << current->totalCost << " 元" << std::endl
              << "当前状态: " << u->hosStatusToString(current->status) << std::endl;
    std::cout << "========================================" << std::endl;
}

bool Patient::getAllHospitalizations(Hospitalization *&hosHead)
{
    std::cout << "正在获取您的所有住院信息..." << std::endl;
    Hospitalization *current = hosHead;
    bool found = false;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->patientID == this->patientID)
        {
            printHospitalizationRecord(current, this);
            found = true;
        }
        current = current->next;
    }
    if (!found)
        printError("没找到任何住院记录。");
    return found;
}
bool Patient::getHospitalizationByID(Hospitalization *&hosHead)
{
    std::string hosID = inputRecordIDCheck("请输入要查询的住院记录ID: ", {"hos"});
    Hospitalization *current = hosHead;
    bool found = false;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->patientID == this->patientID && current->hospitalizationID == hosID)
        {
            printHospitalizationRecord(current, this);
            found = true;
            break;
        }
        current = current->next;
    }
    if (!found)
        printError("没找到指定的住院记录。");
    return found;
}
bool Patient::getHospitalizationsByAdmitTimeRange(Hospitalization *&hosHead)
{
    std::string startTime = inputDateCheck("请输入查询的申请开始时间: ");
    std::string endTime = inputDateCheck("请输入查询的申请结束时间: ");

    Hospitalization *current = hosHead;
    bool found = false;
    while (current != nullptr)
    {
        std::string orderDate = current->applyTime.substr(0, 10);

        if (!current->isDeleted && current->patientID == this->patientID && orderDate >= startTime && orderDate <= endTime && orderDate != "#")
        {
            printHospitalizationRecord(current, this);
            found = true;
        }
        current = current->next;
    }
    if (!found)
        printError("没找到对应申请时间范围的记录。");
    return found;
}
bool Patient::getHospitalizationsByDischargeTimeRange(Hospitalization *&hosHead)
{
    std::string startTime = inputDateCheck("请输入查询的出院开始时间: ");
    std::string endTime = inputDateCheck("请输入查询的出院结束时间: ");

    Hospitalization *current = hosHead;
    bool found = false;
    while (current != nullptr)
    {
        std::string orderDate = current->dischargeTime.substr(0, 10);

        if (!current->isDeleted && current->patientID == this->patientID && orderDate >= startTime && orderDate <= endTime && orderDate != "#")
        {
            printHospitalizationRecord(current, this);
            found = true;
        }
        current = current->next;
    }
    if (!found)
        printError("没找到对应出院时间范围的记录。");
    return found;
}
bool Patient::getHospitalizationsByStatus(Hospitalization *&hosHead, int select)
{
    int sChoice;
    if (select == -1)
    {
        std::cout << "请选择要查询的住院状态: " << std::endl;
        std::cout << "1. 申请中" << std::endl;
        std::cout << "2. 已缴费" << std::endl;
        std::cout << "3. 已入院" << std::endl;
        std::cout << "4. 已出院" << std::endl;
        std::cout << "5. 已作废" << std::endl;
        sChoice = selectIntCheck(1, 5);
    }
    else
    {
        sChoice = select;
    }

    Hospitalization *current = hosHead;
    bool found = false;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->patientID == this->patientID && static_cast<int>(current->status) == sChoice)
        {
            printHospitalizationRecord(current, this);
            found = true;
        }
        current = current->next;
    }
    if (!found)
        printError("没有找到对应状态的住院记录。");
    return found;
}
bool Patient::getHospitalizationsByDepartment(Hospitalization *&hosHead)
{
    std::string dept = inputDepartmentCheck("请输入要查询的科室: ");
    Hospitalization *current = hosHead;
    bool found = false;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->patientID == this->patientID && current->department.find(dept) != std::string::npos)
        {
            printHospitalizationRecord(current, this);
            found = true;
        }
        current = current->next;
    }
    if (!found)
        printError("没找到对应科室的住院记录。");
    return found;
}
bool Patient::getHospitalizationsByWardType(Hospitalization *&hosHead)
{
    std::string type = HospitalizationWardTypeMenu();
    if (type == "0")
        return false;
    Hospitalization *current = hosHead;
    bool found = false;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->patientID == this->patientID && current->wardType == type)
        {
            printHospitalizationRecord(current, this);
            found = true;
        }
        current = current->next;
    }
    if (!found)
        printError("没找到对应病房类型的住院记录。");
    return found;
}
bool Patient::getHospitalizationsByBedNumber(Hospitalization *&hosHead)
{
    std::string dept = inputDepartmentCheck("请输入要查询的科室: ");
    std::string wType = HospitalizationWardTypeMenu();
    if (wType == "0")
        return false;
    std::string bedID = inputBedNumberCheck("请输入要查询的床位号信息: ", dept, wType);
    if (bedID == "")
        return false;

    Hospitalization *current = hosHead;
    bool found = false;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->patientID == this->patientID && current->bedNumber == bedID)
        {
            printHospitalizationRecord(current, this);
            found = true;
        }
        current = current->next;
    }
    if (!found)
        printError("没找到对应床位号的住院记录。");
    return found;
}
bool Patient::getHospitalizationsByDoctorID(Hospitalization *&hosHead)
{
    std::string docID = inputIDCheck("请输入申请医生的ID: ");
    Hospitalization *current = hosHead;
    bool found = false;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->patientID == this->patientID && current->doctorID == docID)
        {
            printHospitalizationRecord(current, this);
            found = true;
        }
        current = current->next;
    }
    if (!found)
        printError("没找到对应医生的住院记录。");
    return found;
}
bool Patient::getHospitalizationsByConsultationID(Hospitalization *&hosHead)
{
    std::string conID = inputRecordIDCheck("请输入看诊记录ID: ", {"con"});
    Hospitalization *current = hosHead;
    bool found = false;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->patientID == this->patientID && current->consultationID == conID)
        {
            printHospitalizationRecord(current, this);
            found = true;
        }
        current = current->next;
    }
    if (!found)
        printError("没找到对应看诊记录的住院记录。");
    return found;
}
void Patient::applyForDischarge(Hospitalization *&hosHead, bedInfo *&bedHead)
{
    getHospitalizationsByStatus(hosHead, 2); // 2 对应已入院状态

    std::string hosID = inputRecordIDCheck("请输入您要申请出院的住院记录ID: ", {"hos"});
    Hospitalization *current = hosHead;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->patientID == this->patientID && current->hospitalizationID == hosID)
            break;
        current = current->next;
    }
    if (!current)
    {
        printError("未找到该住院记录。");
        return;
    }
    if (current->status != HospitalizationStatus::ADMITTED)
    {
        printError("当前记录状态不仅是[已入院]，无法申请出院。");
        return;
    }

    std::cout << "正在为您计算出院费用结算..." << std::endl;
    // 使用正确的日期差计算，如果不足1天按1天计算
    int days = 1;
    if (current->admitTime.length() >= 10)
    {
        int y1 = 0, m1 = 0, d1 = 0, y2 = 0, m2 = 0, d2 = 0;
        sscanf(current->admitTime.c_str(), "%d-%d-%d", &y1, &m1, &d1);
        std::string nowTime = MyTime::getInstance().getTime();
        sscanf(nowTime.c_str(), "%d-%d-%d", &y2, &m2, &d2);
        std::tm tm1 = {}, tm2 = {};
        tm1.tm_year = y1 - 1900; tm1.tm_mon = m1 - 1; tm1.tm_mday = d1;
        tm2.tm_year = y2 - 1900; tm2.tm_mon = m2 - 1; tm2.tm_mday = d2;
        std::time_t t1 = std::mktime(&tm1);
        std::time_t t2 = std::mktime(&tm2);
        double diffSec = std::difftime(t2, t1);
        days = static_cast<int>(diffSec / 86400.0);
        if (days <= 0)
            days = 1;
    }
    double calculatedCost = calculateHospitalizationFee(current->wardType, days);
    current->totalCost = calculatedCost;

    std::cout << "根据您的入出院天数(" << days << "天)，以及病房类型(" << current->wardType << ")，" << std::endl;
    std::cout << "总费用为 " << calculatedCost << " 元。您已缴纳了押金 " << current->deposit << " 元。" << std::endl;

    double diff = calculatedCost - current->deposit;
    if (diff > 0)
    {
        std::cout << "您还需补交欠费 " << diff << " 元。您的账户余额为 " << this->balance << " 元。" << std::endl;
        if (this->balance < diff)
        {
            printError("余额不足，无法为其办理出院，请先充值!");
            return;
        }
        std::cout << "是否确认扣款并办理出院?: ";
        std::cout << "1. 确认" << std::endl;
        std::cout << "0. 取消" << std::endl;

        int confirm = selectIntCheck(0, 1);
        if (confirm == 1)
        {
            this->balance -= diff;
            printSuccess("扣款成功。新余额：" + std::to_string(this->balance) + " 元。");
        }
        else
        {
            std::cout << "出院申请已取消。" << std::endl;
            return;
        }
    }
    else if (diff <= 0)
    {
        double refund = -diff;
        this->balance += refund;
        printSuccess("您的押金已足够支付，系统将为您退还余额 " + std::to_string(refund) + " 元。");
        printSuccess("退款成功。新余额：" + std::to_string(this->balance) + " 元。");
    }

    current->status = HospitalizationStatus::DISCHARGED;
    current->dischargeTime = MyTime::getInstance().getTime();
    printSuccess("已成功为您办理出院！状态已更改为 [出院]。");

    // 清理床位
    if (current->bedNumber != "#")
    {
        bedInfo *b = bedHead;
        while (b)
        {
            if (b->bedID == current->bedNumber && b->patientID == this->patientID)
            {
                b->status = bedStatus::ClEANING; // 设置为清洁中
                b->patientID = "#";
                printSuccess("相应床位已释放，交由护士进行清洁！");
                break;
            }
            b = b->next;
        }
    }
}
void Patient::payHospitalizationDeposit(Hospitalization *&hosHead)
{
    bool ishave = getHospitalizationsByStatus(hosHead, 0); // 0 对应 APPLIED
    if (!ishave)
    {
        std::cout << "如果想为已入院的记录补交款，可以先联系护士或出院时一并结算" << std::endl;
        return;
    }

    std::string hosID = inputRecordIDCheck("请输入要缴纳押金信息的住院记录ID: ", {"hos"});
    Hospitalization *current = hosHead;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->patientID == this->patientID && current->hospitalizationID == hosID)
            break;
        current = current->next;
    }
    if (!current)
    {
        printError("没有找到对应的记录。");
        return;
    }
    if (current->status != HospitalizationStatus::APPLIED)
    {
        printError("该记录不是 [申请中] 状态，不能补交入账押金。");
        return;
    }

    double requiredDeposit = 1000.0; // 默认收取 1000 元作为基础押金
    if (current->wardType == "ICU病房" || current->wardType == "VIP病房")
    {
        requiredDeposit = 5000.0;
    }
    std::cout << "根据该记录(" << current->wardType << ")，您需要先缴纳床位押金: " << requiredDeposit << " 元。" << std::endl;
    std::cout << "您当前的余额为 " << this->balance << " 元。" << std::endl;

    if (this->balance < requiredDeposit)
    {
        printError("余额不足！请先回到账户中心充值。");
        return;
    }

    std::cout << "是否确认缴纳?: ";
    std::cout << "1. 确认" << std::endl;
    std::cout << "0. 取消" << std::endl;

    int confirm = selectIntCheck(0, 1);
    if (confirm == 1)
    {
        this->balance -= requiredDeposit;
        current->deposit += requiredDeposit;
        current->status = HospitalizationStatus::PAID; // 已缴费可以等护士分床了
        printSuccess("缴纳成功！余额 " + std::to_string(this->balance) + " 元。待护士为您排床处理。");
    }
    else
    {
        printWarning("已取消缴纳。");
    }
}

void Patient::manageHospitalizations(Hospitalization *&hosHead, bedInfo *&bedHead)
{
    while (true)
    {
        int choice = patientHospitalizationManagementMenu();
        if (choice == 0)
        {
            return;
        }
        else if (choice == 1)
        {
            int viewChoice = patientHospitalizationViewMenu();
            switch (viewChoice)
            {
            case 1:
                getAllHospitalizations(hosHead);
                break;
            case 2:
                getHospitalizationByID(hosHead);
                break;
            case 3:
                getHospitalizationsByAdmitTimeRange(hosHead);
                break;
            case 4:
                getHospitalizationsByDischargeTimeRange(hosHead);
                break;
            case 5:
                getHospitalizationsByStatus(hosHead);
                break;
            case 6:
                getHospitalizationsByDepartment(hosHead);
                break;
            case 7:
                getHospitalizationsByWardType(hosHead);
                break;
            case 8:
                getHospitalizationsByBedNumber(hosHead);
                break;
            case 9:
                getHospitalizationsByDoctorID(hosHead);
                break;
            case 10:
                getHospitalizationsByConsultationID(hosHead);
                break;
            case 0:
                break;
            }
        }
        else if (choice == 2)
        {
            applyForDischarge(hosHead, bedHead);
        }
        else if (choice == 3)
        {
            payHospitalizationDeposit(hosHead);
        }
    }
}

// ---------------- 患者个人信息管理 ----------------

void Patient::managePersonalInfo()
{
    while (true)
    {
        int choice = patientPersonalInfoManagementMenu();
        if (choice == 0)
        {
            break;
        }
        else if (choice == 1)
        {
            while (true)
            {
                int viewChoice = patientPersonalInfoViewMenu();
                if (viewChoice == 0)
                {
                    break;
                }
                else if (viewChoice == 1)
                {
                    std::cout << "ID: " << this->patientID << std::endl;
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
                    std::cout << "身份证号: " << this->idCardNumber << std::endl;
                }
                else if (viewChoice == 6)
                {
                    std::cout << "联系电话: " << this->telephone << std::endl;
                }
                else if (viewChoice == 7)
                {
                    std::cout << "邮箱地址: " << this->email << std::endl;
                }
                else if (viewChoice == 8)
                {
                    std::cout << "家庭住址: " << this->address << std::endl;
                }
                else if (viewChoice == 9)
                {
                    std::cout << "紧急联系人姓名: " << this->emergencyContactName << std::endl;
                }
                else if (viewChoice == 10)
                {
                    std::cout << "紧急联系人电话: " << this->emergencyContactPhone << std::endl;
                }
                else if (viewChoice == 11)
                {
                    std::cout << "过敏史: " << this->allergyHistory << std::endl;
                }
                else if (viewChoice == 12)
                {
                    std::cout << "既往病史: " << this->pastMedicalHistory << std::endl;
                }
                else if (viewChoice == 13)
                {
                    std::cout << "婚姻状况: " << patientMaritalStatusToString(this->maritalStatus) << std::endl;
                }
                else if (viewChoice == 14)
                {
                    std::cout << "账户余额: " << this->balance << " 元" << std::endl;
                }
                else if (viewChoice == 15)
                {
                    std::cout << "是否住院中: " << (this->isHospitalized ? "是" : "否") << std::endl;
                }
                else if (viewChoice == 16)
                {
                    std::cout << "累计挂号次数: " << this->registrationCount << std::endl;
                }
                else if (viewChoice == 17)
                {
                    std::cout << "累计就诊次数: " << this->consultationCount << std::endl;
                }
                else if (viewChoice == 18)
                {
                    std::cout << "累计住院次数: " << this->hospitalizationCount << std::endl;
                }
                else if (viewChoice == 19)
                {
                    std::cout << "累计用药次数: " << this->medicationCount << std::endl;
                }
                else if (viewChoice == 20)
                {
                    std::cout << "账户创建时间: " << this->createTime << std::endl;
                }
                pause("患者 > 个人信息管理");
            }
        }
        else if (choice == 2)
        {
            while (true)
            {
                int modifyChoice = patientPersonalInfoModificationMenu();

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
                    std::cout << "当前的身份证号: " << this->idCardNumber << std::endl;
                    this->idCardNumber = inputIDcardCheck("请输入新的身份证号: ");
                    printSuccess("身份证号已更新！");
                }
                else if (modifyChoice == 5)
                {
                    std::cout << "当前的联系电话: " << this->telephone << std::endl;
                    this->telephone = inputTelephoneCheck("请输入新的联系电话: ");
                    printSuccess("联系电话已更新！");
                }
                else if (modifyChoice == 6)
                {
                    std::cout << "当前的邮箱地址: " << this->email << std::endl;
                    this->email = inputEmailCheck("请输入新的邮箱地址: ");
                    printSuccess("邮箱地址已更新！");
                }
                else if (modifyChoice == 7)
                {
                    std::cout << "当前的家庭住址: " << this->address << std::endl;
                    this->address = inputStringCheck("请输入新的家庭住址: ");
                    printSuccess("家庭住址已更新！");
                }
                else if (modifyChoice == 8)
                {
                    std::cout << "当前的紧急联系人姓名: " << this->emergencyContactName << std::endl;
                    this->emergencyContactName = inputStringCheck("请输入新的紧急联系人姓名: ");
                    printSuccess("紧急联系人姓名已更新！");
                }
                else if (modifyChoice == 9)
                {
                    std::cout << "当前的紧急联系人电话: " << this->emergencyContactPhone << std::endl;
                    this->emergencyContactPhone = inputTelephoneCheck("请输入新的紧急联系人电话: ");
                    printSuccess("紧急联系人电话已更新！");
                }
                else if (modifyChoice == 10)
                {
                    std::cout << "当前的过敏史: " << this->allergyHistory << std::endl;
                    this->allergyHistory = inputStringCheck("请输入新的过敏史: ");
                    printSuccess("过敏史已更新！");
                }
                else if (modifyChoice == 11)
                {
                    std::cout << "当前的既往病史: " << this->pastMedicalHistory << std::endl;
                    this->pastMedicalHistory = inputStringCheck("请输入新的既往病史: ");
                    printSuccess("既往病史已更新！");
                }
                else if (modifyChoice == 12)
                {
                    std::cout << "当前的婚姻状况: " << patientMaritalStatusToString(this->maritalStatus) << std::endl;
                    std::cout << "请选择新的婚姻状况:" << std::endl;
                    std::cout << "1. 未婚" << std::endl;
                    std::cout << "2. 已婚" << std::endl;
                    std::cout << "3. 离异" << std::endl;
                    std::cout << "4. 丧偶" << std::endl;
                    std::cout << "0. 取消修改" << std::endl;

                    int maritalChoice = selectIntCheck(0, 4);
                    if (maritalChoice == 0)
                    {
                        printWarning("已取消修改操作！");
                        continue;
                    }
                    this->maritalStatus = static_cast<MaritalStatus>(maritalChoice);
                    printSuccess("婚姻状况已更新！");
                }
                else if (modifyChoice == 13)
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
                pause("患者 > 个人信息管理");
            }
        }
        else if (choice == 3)
        {
            std::cout << "当前的账户余额: " << this->balance << " 元" << std::endl;
            double amount = inputFeeCheck("请输入充值金额 (输入0取消充值): ");
            if (amount > 0)
            {
                this->balance += amount;
                printSuccess("成功充值 " + std::to_string(amount) + " 元。当前余额: " + std::to_string(this->balance) + " 元");
            }
            else
            {
                printWarning("已取消充值。");
            }
            pause("患者 > 个人信息管理");
        }
    }
}
