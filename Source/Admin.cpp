#include "../Head/Admin.h"
#include <fstream>
#include <iostream>

Admin::Admin()
{
}

double &Admin::getTotalRevenue()
{
    return totalRevenue;
}

double &Admin::getTotalExpenses()
{
    return totalExpenses;
}

double &Admin::getNetProfit()
{
    return netProfit;
}

Admin *Admin::adminSignUp(int &idCounter)
{
    bool flag = signUp(1, idCounter); // 1 - Admin
    if (flag)
    {
        std::cout << "管理员注册成功! 您的用户ID是: " << getUserID() << std::endl;
        return this; // 返回当前对象以便链式调用
    }
    else
    {
        std::cout << "管理员注册失败！" << std::endl;
        return nullptr;
    }
}

bool Admin::adminSignIn()
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
            std::cout << "管理员登录成功！" << std::endl;
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

// ==================================== 挂号记录管理 =================================

// 查看所有的挂号记录
void Admin::viewAllRegistrations(Registration *&reg, const std::string &department)
{
    Registration *current = reg;
    std::cout << "正在查找所有挂号记录..." << std::endl;

    bool foundAny = false;

    while (current != nullptr)
    {
        if (!current->isDeleted && current->department == department)
        {
            std::string statusStr = regStatusToString(current->status);
            std::cout << "ID: " << current->registrationID
                      << ", 患者ID: " << current->patientID
                      << ", 医生ID: " << current->doctorID
                      << ", 时间: " << current->registerTime
                      << ", 费用: " << current->fee
                      << ", 状态: " << statusStr
                      << ", 备注: " << current->note
                      << std::endl;
            foundAny = true;
        }
        current = current->next;
    }

    if (!foundAny)
    {
        std::cout << "该科室暂无挂号记录！" << std::endl;
    }
}
// 查看指定医生的挂号记录
void Admin::viewRegistrationsByDoctor(Registration *&reg, const std::string &department)
{
    std::string doctorID = inputIDCheck("请输入医生ID: ");

    bool found = false;

    Registration *current = reg;
    std::cout << "正在查找医生ID: " << doctorID << " 的挂号记录..." << std::endl;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->department == department && current->doctorID == doctorID)
        {
            std::string statusStr = regStatusToString(current->status);
            std::cout << "ID: " << current->registrationID
                      << ", 患者ID: " << current->patientID
                      << ", 医生ID: " << current->doctorID
                      << ", 时间: " << current->registerTime
                      << ", 费用: " << current->fee
                      << ", 状态: " << statusStr
                      << ", 备注: " << current->note
                      << std::endl;
            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到该医生的挂号记录！" << std::endl;
    }
}
// 查看指定患者的挂号记录
void Admin::viewRegistrationsByPatient(Registration *&reg, const std::string &department)
{
    std::string patientID = inputIDCheck("请输入患者ID: ");

    bool found = false;

    Registration *current = reg;
    std::cout << "正在查找患者ID: " << patientID << " 的挂号记录..." << std::endl;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->department == department && current->patientID == patientID)
        {
            std::string statusStr = regStatusToString(current->status);
            std::cout << "ID: " << current->registrationID
                      << ", 患者ID: " << current->patientID
                      << ", 医生ID: " << current->doctorID
                      << ", 时间: " << current->registerTime
                      << ", 费用: " << current->fee
                      << ", 状态: " << statusStr
                      << ", 备注: " << current->note
                      << std::endl;
            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到该患者的挂号记录！" << std::endl;
    }
}
// 查看指定状态的挂号记录
void Admin::viewRegistrationsByStatus(Registration *&reg, const std::string &department)
{
    std::cout << "请输入要过滤的挂号状态 (0 - 已预约, 1 - 已支付, 2 - 已取消, 3 - 已完成): ";
    int statusFilter = selectIntCheck(0, 3);

    bool found = false;

    RegistrationStatus filterStatus = static_cast<RegistrationStatus>(statusFilter);

    Registration *current = reg;
    std::cout << "正在查找状态为: " << regStatusToString(filterStatus) << " 的挂号记录..." << std::endl;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->department == department && current->status == filterStatus)
        {
            std::string statusStr = regStatusToString(current->status);
            std::cout << "ID: " << current->registrationID
                      << ", 患者ID: " << current->patientID
                      << ", 医生ID: " << current->doctorID
                      << ", 时间: " << current->registerTime
                      << ", 费用: " << current->fee
                      << ", 状态: " << statusStr
                      << ", 备注: " << current->note
                      << std::endl;
            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到该状态的挂号记录！" << std::endl;
    }
}
// 修改挂号记录状态（如支付、取消等）
void Admin::modifyRegistrationStatus(Registration *&reg, const std::string &department)
{
    std::string regID = inputRecordIDCheck("请输入要修改状态的挂号记录ID: ", {"reg"});
    bool found = false;

    Registration *current = reg;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->registrationID == regID && current->department == department)
        {
            std::cout << "请输入新的挂号状态 (0 - 已预约, 1 - 已支付, 2 - 已取消, 3 - 已完成): ";
            int newStatus = selectIntCheck(0, 3);
            current->status = static_cast<RegistrationStatus>(newStatus);
            std::cout << "挂号记录状态已更新！" << std::endl;
            found = true;
            break;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到指定的挂号记录！" << std::endl;
    }
}
// 删除挂号记录（逻辑删除，设置 isDeleted 标志）
void Admin::deleteRegistration(Registration *&reg, const std::string &department)
{
    std::string regID = inputRecordIDCheck("请输入要删除的挂号记录ID: ", {"reg"});
    bool found = false;

    Registration *current = reg;
    while (current != nullptr)
    {
        if (current->registrationID == regID && current->department == department)
        {
            current->isDeleted = true; // 逻辑删除
            std::cout << "挂号记录已删除！" << std::endl;
            found = true;
            break;
        }
        current = current->next;
    }
    if (!found)
    {
        std::cout << "未找到指定的挂号记录！" << std::endl;
    }
}
// 添加挂号记录（根据输入信息创建新的 Registration 对象，并插入到链表中）
void Admin::addRegistration(Registration *&reg, Doctor *&doc, const std::string &department, int &idCounter)
{
    Registration *newReg = new Registration();

    newReg->patientID = inputIDCheck("请输入患者ID: ");
    newReg->department = department;

    std::cout << "可用医生列表:" << std::endl;
    Doctor *currentDoc = doc;
    while (currentDoc != nullptr)
    {
        if (currentDoc->department == department)
        {
            std::cout << "医生ID: " << currentDoc->getUserID() << ", 姓名: " << currentDoc->getUsername() << std::endl;
        }
        currentDoc = currentDoc->next;
    }

    newReg->doctorID = inputIDCheck("请输入医生ID: ");

    newReg->fee = inputFeeCheck("请输入挂号费用: "); // 输入费用并检查有效性

    // 生成唯一的挂号ID（可以根据实际需求改为更复杂的生成方式）
    newReg->registrationID = "reg" + std::to_string(idCounter++).insert(0, 6 - std::to_string(idCounter).length(), '0');
    MyTime &t = MyTime::getInstance();
    newReg->registerTime = t.getTime(); // 获取当前时间字符串

    // 插入到链表头部
    newReg->next = reg;
    if (reg != nullptr)
        reg->prev = newReg;
    reg = newReg;

    std::cout << "挂号记录已添加！新挂号ID: " << newReg->registrationID << std::endl;
}
// 根据挂号信息ID查看挂号记录
void Admin::viewRegistrationsByID(Registration *&reg, const std::string &department)
{
    std::string regID = inputRecordIDCheck("请输入要查看的挂号记录ID: ", {"reg"});

    bool found = false;

    Registration *current = reg;
    std::cout << "正在查找挂号记录ID: " << regID << " 的挂号记录..." << std::endl;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->department == department && current->registrationID == regID)
        {
            std::string statusStr = regStatusToString(current->status);
            std::cout << "ID: " << current->registrationID
                      << ", 患者ID: " << current->patientID
                      << ", 医生ID: " << current->doctorID
                      << ", 时间: " << current->registerTime
                      << ", 费用: " << current->fee
                      << ", 状态: " << statusStr
                      << ", 备注: " << current->note
                      << std::endl;
            found = true;
            break;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到指定的挂号记录！" << std::endl;
    }
}

// 管理挂号记录的主函数，提供查看、修改状态、删除和添加等功能
void Admin::manageRegistrations(Registration *&reg, Doctor *&doc, const std::string &department, int &idCounter)
{
    while (true)
    {
        int choice = adminRegistrationManagementMenu();
        if (choice == 0)
            break;

        if (choice == 1)
        {
            while (true) // 查看方式选择循环，直到用户选择返回上级菜单
            {
                int viewChoice = adminRegistrationViewMenu();

                if (viewChoice == 0)
                    break;
                else if (viewChoice == 1)
                {
                    viewAllRegistrations(reg, department);
                    pause();
                }
                else if (viewChoice == 2)
                {
                    viewRegistrationsByStatus(reg, department);
                    pause();
                }
                else if (viewChoice == 3)
                {
                    viewRegistrationsByPatient(reg, department);
                    pause();
                }
                else if (viewChoice == 4)
                {
                    viewRegistrationsByDoctor(reg, department);
                    pause();
                }
                else if (viewChoice == 5)
                {
                    viewRegistrationsByID(reg, department);
                    pause();
                }
                else
                {
                    std::cout << "无效的选择! 请重新选择。" << std::endl;
                    pause();
                }
            }
        }
        else if (choice == 2)
        {
            modifyRegistrationStatus(reg, department);
            pause();
        }
        else if (choice == 3)
        {
            deleteRegistration(reg, department);
            pause();
        }
        else if (choice == 4)
        {
            addRegistration(reg, doc, department, idCounter);
            pause();
        }
        else
        {
            std::cout << "无效的选择! 请重新选择。" << std::endl;
            pause();
        }
    }
}

// ==================================== 看诊记录管理 =================================

// 查看所有的看诊记录
void Admin::viewAllConsultations(Consultation *&con, const std::string &department)
{
    Consultation *current = con;
    std::vector<Consultation *> temp;
    std::cout << "正在查找所有看诊记录..." << std::endl;

    bool foundAny = false;

    while (current != nullptr)
    {
        if (!current->isDeleted && current->department == department)
        {
            std::string statusStr = conStatusToString(current->status);
            std::cout << "ID: " << current->consultationID
                      << ", 患者ID: " << current->patientID
                      << ", 医生ID: " << current->doctorID
                      << ", 时间: " << current->consultationTime
                      << ", 科室: " << current->department
                      << ", 状态: " << statusStr
                      << ", 备注: " << current->note
                      << std::endl;
            foundAny = true;

            if (statusStr == "正在处理")
            {
                temp.push_back(current);
            }
        }
        current = current->next;
    }

    if (!foundAny)
    {
        std::cout << "该科室暂无看诊记录！" << std::endl;
        return;
    }

    std::cout << "正在处理的看诊记录:" << std::endl;
    for (Consultation *c : temp)
    {
        std::string statusStr = conStatusToString(c->status);
        std::cout << "ID: " << c->consultationID
                  << ", 患者ID: " << c->patientID
                  << ", 医生ID: " << c->doctorID
                  << ", 时间: " << c->consultationTime
                  << ", 科室: " << c->department
                  << ", 状态: " << statusStr
                  << std::endl;

        std::cout << "  主诉: " << c->chiefComplaint << std::endl;
        std::cout << "  现病史: " << c->historyOfPresentIllness << std::endl;
        std::cout << "  既往史: " << c->pastMedicalHistory << std::endl;
        std::cout << "  家族史: " << c->familyHistory << std::endl;

        std::cout << "  初步诊断: " << c->preliminaryDiagnosis << std::endl;
        std::cout << "  计划检查项目: ";
        for (const auto &exam : c->examinationlist)
        {
            std::cout << exam << " ";
        }
        std::cout << std::endl;
        std::cout << "  计划用药" << (current->isPrecriptionReviewed ? "（已审核）" : "（未审核）") << ": " << std::endl;
        for (const auto &med : current->prescriptions)
        {
            std::cout << "药品ID: " << med.medicineID << ", 名称: " << med.name << ", 用量: " << med.dosage
                      << ", 频次: " << med.frequency << ", 疗程: " << med.duration
                      << ", 备注: " << med.note << std::endl;
        }

        std::cout << "是否建议住院: " << (current->isHospitalizationRecommended ? "是" : "否") << std::endl;

        std::cout << "  医生备注: " << current->note << std::endl;
    }
}
// 查看指定医生的看诊记录
void Admin::viewConsultationsByDoctor(Consultation *&con, const std::string &department)
{
    std::string doctorID = inputIDCheck("请输入医生ID: ");

    bool found = false;

    Consultation *current = con;
    std::vector<Consultation *> temp;
    std::cout << "正在查找医生ID: " << doctorID << " 的看诊记录..." << std::endl;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->department == department && current->doctorID == doctorID)
        {
            std::string statusStr = conStatusToString(current->status);
            std::cout << "ID: " << current->consultationID
                      << ", 患者ID: " << current->patientID
                      << ", 医生ID: " << current->doctorID
                      << ", 时间: " << current->consultationTime
                      << ", 科室: " << current->department
                      << ", 状态: " << statusStr
                      << ", 备注: " << current->note
                      << std::endl;
            if (statusStr == "正在处理")
            {
                temp.push_back(current);
            }
            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到该医生的看诊记录！" << std::endl;
        return;
    }

    std::cout << "正在处理的看诊记录:" << std::endl;
    for (Consultation *c : temp)
    {
        std::string statusStr = conStatusToString(c->status);
        std::cout << "ID: " << c->consultationID
                  << ", 患者ID: " << c->patientID
                  << ", 医生ID: " << c->doctorID
                  << ", 时间: " << c->consultationTime
                  << ", 科室: " << c->department
                  << ", 状态: " << statusStr
                  << std::endl;

        std::cout << "  主诉: " << c->chiefComplaint << std::endl;
        std::cout << "  现病史: " << c->historyOfPresentIllness << std::endl;
        std::cout << "  既往史: " << c->pastMedicalHistory << std::endl;
        std::cout << "  家族史: " << c->familyHistory << std::endl;

        std::cout << "  初步诊断: " << c->preliminaryDiagnosis << std::endl;
        std::cout << "  计划检查项目: ";
        for (const auto &exam : c->examinationlist)
        {
            std::cout << exam << " ";
        }
        std::cout << std::endl;
        std::cout << "  计划用药" << (current->isPrecriptionReviewed ? "（已审核）" : "（未审核）") << ": " << std::endl;
        for (const auto &med : current->prescriptions)
        {
            std::cout << "药品ID: " << med.medicineID << ", 名称: " << med.name << ", 用量: " << med.dosage
                      << ", 频次: " << med.frequency << ", 疗程: " << med.duration
                      << ", 备注: " << med.note << std::endl;
        }

        std::cout << "是否建议住院: " << (current->isHospitalizationRecommended ? "是" : "否") << std::endl;

        std::cout << "  医生备注: " << current->note << std::endl;
    }
}
// 查看指定患者的看诊记录
void Admin::viewConsultationsByPatient(Consultation *&con, const std::string &department)
{
    std::string patientID = inputIDCheck("请输入患者ID: ");

    bool found = false;

    Consultation *current = con;
    std::vector<Consultation *> temp;
    std::cout << "正在查找患者ID: " << patientID << " 的看诊记录..." << std::endl;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->department == department && current->patientID == patientID)
        {
            std::string statusStr = conStatusToString(current->status);
            std::cout << "ID: " << current->consultationID
                      << ", 患者ID: " << current->patientID
                      << ", 医生ID: " << current->doctorID
                      << ", 时间: " << current->consultationTime
                      << ", 科室: " << current->department
                      << ", 状态: " << statusStr
                      << ", 备注: " << current->note
                      << std::endl;
            if (statusStr == "正在处理")
            {
                temp.push_back(current);
            }
            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到该患者的看诊记录！" << std::endl;
        return;
    }

    std::cout << "正在处理的看诊记录:" << std::endl;
    for (Consultation *c : temp)
    {
        std::string statusStr = conStatusToString(c->status);
        std::cout << "ID: " << c->consultationID
                  << ", 患者ID: " << c->patientID
                  << ", 医生ID: " << c->doctorID
                  << ", 时间: " << c->consultationTime
                  << ", 科室: " << c->department
                  << ", 状态: " << statusStr
                  << std::endl;

        std::cout << "  主诉: " << c->chiefComplaint << std::endl;
        std::cout << "  现病史: " << c->historyOfPresentIllness << std::endl;
        std::cout << "  既往史: " << c->pastMedicalHistory << std::endl;
        std::cout << "  家族史: " << c->familyHistory << std::endl;

        std::cout << "  初步诊断: " << c->preliminaryDiagnosis << std::endl;
        std::cout << "  计划检查项目: ";
        for (const auto &exam : c->examinationlist)
        {
            std::cout << exam << " ";
        }
        std::cout << std::endl;

        std::cout << "  计划用药" << (current->isPrecriptionReviewed ? "（已审核）" : "（未审核）") << ": " << std::endl;
        for (const auto &med : current->prescriptions)
        {
            std::cout << "药品ID: " << med.medicineID << ", 名称: " << med.name << ", 用量: " << med.dosage
                      << ", 频次: " << med.frequency << ", 疗程: " << med.duration
                      << ", 备注: " << med.note << std::endl;
        }

        std::cout << "是否建议住院: " << (current->isHospitalizationRecommended ? "是" : "否") << std::endl;

        std::cout << "  医生备注: " << current->note << std::endl;
    }
}
// 查看指定状态的看诊记录
void Admin::viewConsultationsByStatus(Consultation *&con, const std::string &department)
{
    std::cout << "请输入要过滤的看诊状态 (0 - 待处理, 1 - 正在处理, 2 - 已完成, 3 - 已作废): ";
    int statusFilter = selectIntCheck(0, 3);

    bool found = false;

    ConsultationStatus filterStatus = static_cast<ConsultationStatus>(statusFilter);

    Consultation *current = con;
    std::cout << "正在查找状态为: " << conStatusToString(filterStatus) << " 的看诊记录..." << std::endl;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->department == department && current->status == filterStatus)
        {
            std::string statusStr = conStatusToString(current->status);

            if (statusStr == "正在处理")
            {
                std::string statusStr = conStatusToString(current->status);
                std::cout << "ID: " << current->consultationID
                          << ", 患者ID: " << current->patientID
                          << ", 医生ID: " << current->doctorID
                          << ", 时间: " << current->consultationTime
                          << ", 科室: " << current->department
                          << ", 状态: " << statusStr
                          << std::endl;

                std::cout << "  主诉: " << current->chiefComplaint << std::endl;
                std::cout << "  现病史: " << current->historyOfPresentIllness << std::endl;
                std::cout << "  既往史: " << current->pastMedicalHistory << std::endl;
                std::cout << "  家族史: " << current->familyHistory << std::endl;

                std::cout << "  初步诊断: " << current->preliminaryDiagnosis << std::endl;
                std::cout << "  计划检查项目: ";
                for (const auto &exam : current->examinationlist)
                {
                    std::cout << exam << " ";
                }
                std::cout << std::endl;
                std::cout << "  计划用药" << (current->isPrecriptionReviewed ? "（已审核）" : "（未审核）") << ": " << std::endl;
                for (const auto &med : current->prescriptions)
                {
                    std::cout << "药品ID: " << med.medicineID << ", 名称: " << med.name << ", 用量: " << med.dosage
                              << ", 频次: " << med.frequency << ", 疗程: " << med.duration
                              << ", 备注: " << med.note << std::endl;
                }

                std::cout << "是否建议住院: " << (current->isHospitalizationRecommended ? "是" : "否") << std::endl;

                std::cout << "  医生备注: " << current->note << std::endl;
            }
            else
            {
                std::cout << "ID: " << current->consultationID
                          << ", 患者ID: " << current->patientID
                          << ", 医生ID: " << current->doctorID
                          << ", 时间: " << current->consultationTime
                          << ", 科室: " << current->department
                          << ", 状态: " << statusStr
                          << ", 备注: " << current->note
                          << std::endl;
            }

            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到该状态的看诊记录！" << std::endl;
        return;
    }
}
// 查看指定挂号ID的看诊记录
void Admin::viewConsultationByRegistrationID(Consultation *&con, const std::string &department)
{
    std::string registrationID = inputRecordIDCheck("请输入挂号记录ID: ", {"reg"});

    bool found = false;

    Consultation *current = con;
    std::vector<Consultation *> temp;
    std::cout << "正在查找挂号ID: " << registrationID << " 的看诊记录..." << std::endl;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->department == department && current->registrationID == registrationID)
        {
            std::string statusStr = conStatusToString(current->status);
            std::cout << "ID: " << current->consultationID
                      << ", 患者ID: " << current->patientID
                      << ", 医生ID: " << current->doctorID
                      << ", 时间: " << current->consultationTime
                      << ", 科室: " << current->department
                      << ", 状态: " << statusStr
                      << ", 备注: " << current->note
                      << std::endl;
            if (statusStr == "正在处理")
            {
                temp.push_back(current);
            }
            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到与该挂号ID关联的看诊记录！" << std::endl;
        return;
    }

    std::cout << "正在处理的看诊记录:" << std::endl;
    for (Consultation *c : temp)
    {
        std::string statusStr = conStatusToString(c->status);
        std::cout << "ID: " << c->consultationID
                  << ", 患者ID: " << c->patientID
                  << ", 医生ID: " << c->doctorID
                  << ", 时间: " << c->consultationTime
                  << ", 科室: " << c->department
                  << ", 状态: " << statusStr
                  << std::endl;

        std::cout << "  主诉: " << c->chiefComplaint << std::endl;
        std::cout << "  现病史: " << c->historyOfPresentIllness << std::endl;
        std::cout << "  既往史: " << c->pastMedicalHistory << std::endl;
        std::cout << "  家族史: " << c->familyHistory << std::endl;

        std::cout << "  初步诊断: " << c->preliminaryDiagnosis << std::endl;
        std::cout << "  计划检查项目: ";
        for (const auto &exam : c->examinationlist)
        {
            std::cout << exam << " ";
        }
        std::cout << std::endl;

        std::cout << "  计划用药" << (current->isPrecriptionReviewed ? "（已审核）" : "（未审核）") << ": " << std::endl;
        for (const auto &med : current->prescriptions)
        {
            std::cout << "药品ID: " << med.medicineID << ", 名称: " << med.name << ", 用量: " << med.dosage
                      << ", 频次: " << med.frequency << ", 疗程: " << med.duration
                      << ", 备注: " << med.note << std::endl;
        }

        std::cout << "是否建议住院: " << (current->isHospitalizationRecommended ? "是" : "否") << std::endl;

        std::cout << "  医生备注: " << current->note << std::endl;
    }
}
// 修改看诊记录的状态
void Admin::modifyConsultation(Consultation *&con, const std::string &department)
{
    std::string conID = inputRecordIDCheck("请输入要修改状态的看诊记录ID: ", {"con"}); // 输入看诊记录ID并检查格式

    bool found = false;

    Consultation *current = con;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->consultationID == conID && current->department == department)
        {

            std::cout << "请输入新的看诊状态 (0 - 待就诊, 1 - 正在处理, 2 - 已结束, 3 - 已作废 ): ";
            int newStatus = selectIntCheck(0, 3);
            current->status = static_cast<ConsultationStatus>(newStatus);
            std::cout << "看诊记录状态已更新！" << std::endl;
            found = true;
            break;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到指定的看诊记录！" << std::endl;
    }
}
// 删除看诊记录（逻辑删除，设置 isDeleted 标志）
void Admin::deleteConsultation(Consultation *&con, const std::string &department)
{
    std::string conID = inputRecordIDCheck("请输入要删除的看诊记录ID: ", {"con"}); // 输入看诊记录ID并检查格式

    bool found = false;

    Consultation *current = con;
    while (current != nullptr)
    {
        if (current->consultationID == conID && current->department == department)
        {
            current->isDeleted = true; // 逻辑删除
            std::cout << "ID: " << current->consultationID << " " << "看诊记录已删除！" << std::endl;
            found = true;
            break;
        }
        current = current->next;
    }
    if (!found)
    {
        std::cout << "未找到指定的看诊记录！" << std::endl;
    }
}
// 添加看诊记录（根据输入信息创建新的 Consultation 对象，并插入到链表中）
void Admin::addConsultation(Consultation *&con, const std::string &department, Registration *reg, int &conCounter)
{
    Consultation *newCon = new Consultation();

    std::cout << department << " 当前可关联的挂号记录列表:" << std::endl;
    Registration *currentReg = reg;
    while (currentReg != nullptr)
    { // 只显示当前科室的挂号记录，并且状态为已支付的记录
        if (!currentReg->isDeleted && currentReg->status == RegistrationStatus::PAID && currentReg->department == department)
        {
            std::string statusStr = regStatusToString(currentReg->status);
            std::cout << "ID: " << currentReg->registrationID
                      << ", 患者ID: " << currentReg->patientID
                      << ", 医生ID: " << currentReg->doctorID
                      << ", 科室: " << currentReg->department
                      << ", 时间: " << currentReg->registerTime
                      << ", 费用: " << currentReg->fee
                      << ", 状态: " << statusStr
                      << std::endl;
        }
        currentReg = currentReg->next;
    }

    std::string regID = inputRecordIDCheck("请输入挂号ID以关联看诊记录: ", {"reg"}); // 输入挂号ID并检查格式

    currentReg = reg;
    while (currentReg != nullptr)
    {
        if (!currentReg->isDeleted && currentReg->registrationID == regID && currentReg->department == department)
        {
            if (currentReg->status != RegistrationStatus::PAID)
            {
                std::cout << "只能关联已支付的挂号记录！" << std::endl;
                delete newCon; // 释放内存
                return;
            }
            newCon->registrationID = regID;            // 关联挂号记录ID
            newCon->patientID = currentReg->patientID; // 从挂号记录获取患者ID
            newCon->department = department;           // 设置科室
            newCon->doctorID = currentReg->doctorID;   // 从挂号记录获取医生ID
            break;
        }
        currentReg = currentReg->next;
    }

    if (currentReg == nullptr)
    {
        std::cout << "未找到指定的挂号记录！无法添加看诊记录。" << std::endl;
        delete newCon; // 释放内存
        return;
    }

    // 生成唯一的看诊ID（可以根据实际需求改为更复杂的生成方式）
    newCon->consultationID = "con" + std::to_string(conCounter++).insert(0, 6 - std::to_string(conCounter).length(), '0');
    MyTime &t = MyTime::getInstance();
    newCon->consultationTime = t.getTime(); // 获取当前时间字符串

    // 插入到链表头部
    newCon->next = con;
    if (con != nullptr)
        con->prev = newCon;
    con = newCon;

    std::cout << "看诊记录已添加！新看诊ID: " << newCon->consultationID << std::endl;
}
// 根据看诊记录ID查看看诊记录
void Admin::viewConsultationByID(Consultation *&con, const std::string &department)
{
    std::string conID = inputRecordIDCheck("请输入要查看的看诊记录ID: ", {"con"}); // 输入看诊记录ID并检查格式

    bool found = false;

    Consultation *current = con;
    std::cout << "正在查找看诊记录ID: " << conID << " 的看诊记录..." << std::endl;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->consultationID == conID && current->department == department)
        {
            std::string statusStr = conStatusToString(current->status);
            if (statusStr == "正在处理")
            {
                std::string statusStr = conStatusToString(current->status);
                std::cout << "ID: " << current->consultationID
                          << ", 患者ID: " << current->patientID
                          << ", 医生ID: " << current->doctorID
                          << ", 时间: " << current->consultationTime
                          << ", 科室: " << current->department
                          << ", 状态: " << statusStr
                          << std::endl;

                std::cout << "  主诉: " << current->chiefComplaint << std::endl;
                std::cout << "  现病史: " << current->historyOfPresentIllness << std::endl;
                std::cout << "  既往史: " << current->pastMedicalHistory << std::endl;
                std::cout << "  家族史: " << current->familyHistory << std::endl;

                std::cout << "  初步诊断: " << current->preliminaryDiagnosis << std::endl;
                std::cout << "  计划检查项目: ";
                for (const auto &exam : current->examinationlist)
                {
                    std::cout << exam << " ";
                }
                std::cout << std::endl;
                std::cout << "  计划用药" << (current->isPrecriptionReviewed ? "（已审核）" : "（未审核）") << ": " << std::endl;
                for (const auto &med : current->prescriptions)
                {
                    std::cout << "药品ID: " << med.medicineID << ", 名称: " << med.name << ", 用量: " << med.dosage
                              << ", 频次: " << med.frequency << ", 疗程: " << med.duration
                              << ", 备注: " << med.note << std::endl;
                }

                std::cout << "是否建议住院: " << (current->isHospitalizationRecommended ? "是" : "否") << std::endl;

                std::cout << "  医生备注: " << current->note << std::endl;
            }
            else
            {
                std::cout << "ID: " << current->consultationID
                          << ", 患者ID: " << current->patientID
                          << ", 医生ID: " << current->doctorID
                          << ", 时间: " << current->consultationTime
                          << ", 科室: " << current->department
                          << ", 状态: " << statusStr
                          << ", 备注: " << current->note
                          << std::endl;
            }

            found = true;
            break;
        }
    }
    if (!found)
    {
        std::cout << "未找到指定的看诊记录！" << std::endl;
    }
}

// 管理看诊记录的主函数，提供查看、修改状态、删除和添加等功能
void Admin::manageConsultations(Consultation *&con, const std::string &department, Registration *reg, int &conCounter)
{
    while (true) // 管理员管理看诊记录主循环，直到用户选择退出
    {
        int choice = adminConsultationManagementMenu();
        if (choice == 0)
        {
            break;
        }
        else if (choice == 1)
        {
            while (true) // 查看方式选择循环，直到用户选择返回上级菜单
            {
                int viewChoice = adminConsultationViewMenu();
                if (viewChoice == 0)
                {
                    break;
                }
                else if (viewChoice == 1)
                {
                    viewAllConsultations(con, department);
                    pause();
                }
                else if (viewChoice == 2)
                {
                    viewConsultationsByPatient(con, department);
                    pause();
                }
                else if (viewChoice == 3)
                {
                    viewConsultationsByDoctor(con, department);
                    pause();
                }
                else if (viewChoice == 4)
                {
                    viewConsultationsByStatus(con, department);
                    pause();
                }
                else if (viewChoice == 5)
                {
                    viewConsultationByRegistrationID(con, department);
                    pause();
                }
                else if (viewChoice == 6)
                {
                    viewConsultationByID(con, department);
                    pause();
                }
                else
                {
                    std::cout << "无效的选择! 请重新选择。" << std::endl;
                    pause();
                }
            }
        }
        else if (choice == 2)
        {
            modifyConsultation(con, department);
            pause();
        }
        else if (choice == 3)
        {
            deleteConsultation(con, department);
            pause();
        }
        else if (choice == 4)
        {
            addConsultation(con, department, reg, conCounter);
            pause();
        }
        else
        {
            std::cout << "无效的选择! 请重新选择。" << std::endl;
            pause();
        }
    }
}

// ==================================== 检查记录管理 =================================

// 查看所有的检查记录
void Admin::viewAllExaminations(Examination *&exam, const std::string &department)
{
    Examination *current = exam;
    bool found = false;
    std::cout << "正在查找所有检查记录..." << std::endl;

    std::vector<Examination *> temp;

    while (current != nullptr)
    {
        if (!current->isDeleted && current->department == department)
        {
            std::string statusStr = examStatusToString(current->status);
            std::cout << "ID: " << current->examinationID
                      << ", 患者ID: " << current->patientID
                      << ", 医生ID: " << current->doctorID
                      << ", 科室: " << current->department
                      << ", 检查项目: " << current->itemName
                      << ", 下单时间: " << current->orderTime
                      << ", 状态: " << statusStr
                      << ", 出报告时间: " << (current->reportTime.empty() ? "报告未出" : current->reportTime)
                      << ", 费用: " << current->fee
                      << std::endl;

            if (!current->reportTime.empty())
            {
                temp.push_back(current);
            }

            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "该科室暂无检查记录！" << std::endl;
    }

    std::cout << "已出报告的检查记录:" << std::endl;
    for (Examination *e : temp)
    {
        std::string statusStr = examStatusToString(e->status);
        std::cout << "ID: " << e->examinationID
                  << ", 患者ID: " << e->patientID
                  << ", 医生ID: " << e->doctorID
                  << ", 科室: " << e->department
                  << ", 检查项目: " << e->itemName
                  << ", 下单时间: " << e->orderTime
                  << ", 状态: " << statusStr
                  << ", 出报告时间: " << e->reportTime
                  << ", 费用: " << e->fee
                  << std::endl;
        std::cout << "检查结果：" << findVitalSignToString(e) << std::endl;
        std::cout << "报告摘要：" << e->reportSummary << std::endl;
        std::cout << "医生备注：" << e->note << std::endl;
    }
}
// 查看指定医生的检查记录
void Admin::viewExaminationsByDoctor(Examination *&exam, const std::string &department)
{
    std::string doctorID = inputIDCheck("请输入医生ID: ");

    bool found = false;

    Examination *current = exam;
    std::vector<Examination *> temp;
    std::cout << "正在查找医生ID: " << doctorID << " 的检查记录..." << std::endl;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->department == department && current->doctorID == doctorID)
        {
            std::string statusStr = examStatusToString(current->status);
            std::cout << "ID: " << current->examinationID
                      << ", 患者ID: " << current->patientID
                      << ", 医生ID: " << current->doctorID
                      << ", 科室: " << current->department
                      << ", 检查项目: " << current->itemName
                      << ", 下单时间: " << current->orderTime
                      << ", 状态: " << statusStr
                      << ", 出报告时间: " << (current->reportTime.empty() ? "报告未出" : current->reportTime)
                      << ", 费用: " << current->fee
                      << std::endl;

            if (!current->reportTime.empty())
            {
                temp.push_back(current);
            }

            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到该医生的检查记录！" << std::endl;
        return;
    }

    std::cout << "已出报告的检查记录:" << std::endl;
    for (Examination *e : temp)
    {
        std::string statusStr = examStatusToString(e->status);
        std::cout << "ID: " << e->examinationID
                  << ", 患者ID: " << e->patientID
                  << ", 医生ID: " << e->doctorID
                  << ", 科室: " << e->department
                  << ", 检查项目: " << e->itemName
                  << ", 下单时间: " << e->orderTime
                  << ", 状态: " << statusStr
                  << ", 出报告时间: " << (e->reportTime.empty() ? "报告未出" : e->reportTime)
                  << ", 费用: " << e->fee
                  << std::endl;
        std::cout << "检查结果：" << findVitalSignToString(e) << std::endl;
        std::cout << "报告摘要：" << e->reportSummary << std::endl;
        std::cout << "医生备注：" << e->note << std::endl;
    }
}
// 查看指定患者的检查记录
void Admin::viewExaminationsByPatient(Examination *&exam, const std::string &department)
{
    std::string patientID = inputIDCheck("请输入患者ID: ");

    bool found = false;

    Examination *current = exam;
    std::vector<Examination *> temp;
    std::cout << "正在查找患者ID: " << patientID << " 的检查记录..." << std::endl;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->department == department && current->patientID == patientID)
        {
            std::string statusStr = examStatusToString(current->status);
            std::cout << "ID: " << current->examinationID
                      << ", 患者ID: " << current->patientID
                      << ", 医生ID: " << current->doctorID
                      << ", 科室: " << current->department
                      << ", 检查项目: " << current->itemName
                      << ", 下单时间: " << current->orderTime
                      << ", 状态: " << statusStr
                      << ", 出报告时间: " << (current->reportTime.empty() ? "报告未出" : current->reportTime)
                      << ", 费用: " << current->fee
                      << std::endl;

            if (!current->reportTime.empty())
            {
                temp.push_back(current);
            }

            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到该患者的检查记录！" << std::endl;
        return;
    }

    std::cout << "已出报告的检查记录:" << std::endl;
    for (Examination *e : temp)
    {
        std::string statusStr = examStatusToString(e->status);
        std::cout << "ID: " << e->examinationID
                  << ", 患者ID: " << e->patientID
                  << ", 医生ID: " << e->doctorID
                  << ", 科室: " << e->department
                  << ", 检查项目: " << e->itemName
                  << ", 下单时间: " << e->orderTime
                  << ", 状态: " << statusStr
                  << ", 出报告时间: " << (e->reportTime.empty() ? "报告未出" : e->reportTime)
                  << ", 费用: " << e->fee
                  << std::endl;
        std::cout << "检查结果：" << findVitalSignToString(e) << std::endl;
        std::cout << "报告摘要：" << e->reportSummary << std::endl;
        std::cout << "医生备注：" << e->note << std::endl;
    }
}
// 查看指定状态的检查记录
void Admin::viewExaminationsByStatus(Examination *&exam, const std::string &department)
{
    std::cout << "请输入要过滤的检查状态 (0 - 已下单, 1 - 已支付, 2 - 检查中, 3 - 检查完成, 4 - 报告已出, 5 - 已作废): ";
    int statusFilter = selectIntCheck(0, 5);

    bool found = false;

    ExaminationStatus filterStatus = static_cast<ExaminationStatus>(statusFilter);

    Examination *current = exam;
    std::cout << "正在查找状态为: " << examStatusToString(filterStatus) << " 的检查记录..." << std::endl;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->department == department && current->status == filterStatus)
        {
            std::string statusStr = examStatusToString(current->status);

            if (statusStr == "检查完成")
            {
                std::string statusStr = examStatusToString(current->status);
                std::cout << "ID: " << current->examinationID
                          << ", 患者ID: " << current->patientID
                          << ", 医生ID: " << current->doctorID
                          << ", 科室: " << current->department
                          << ", 检查项目: " << current->itemName
                          << ", 下单时间: " << current->orderTime
                          << ", 状态: " << statusStr
                          << ", 出报告时间: " << (current->reportTime.empty() ? "报告未出" : current->reportTime)
                          << ", 费用: " << current->fee
                          << std::endl;
                std::cout << "检查结果：" << findVitalSignToString(current) << std::endl;
                std::cout << "报告摘要：" << current->reportSummary << std::endl;
                std::cout << "医生备注：" << current->note << std::endl;
            }
            else
            {
                std::cout << "ID: " << current->examinationID
                          << ", 患者ID: " << current->patientID
                          << ", 医生ID: " << current->doctorID
                          << ", 科室: " << current->department
                          << ", 检查项目: " << current->itemName
                          << ", 下单时间: " << current->orderTime
                          << ", 状态: " << statusStr
                          << ", 出报告时间: " << (current->reportTime.empty() ? "报告未出" : current->reportTime)
                          << ", 费用: " << current->fee
                          << std::endl;
            }

            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到该状态的检查记录！" << std::endl;
        return;
    }
}
// 修改检查记录的状态
void Admin::modifyExaminationStatus(Examination *&exam, const std::string &department)
{
    std::string examID = inputRecordIDCheck("请输入要修改状态的检查记录ID: ", {"exa"}); // 输入检查记录ID并检查格式

    bool found = false;

    Examination *current = exam;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->examinationID == examID && current->department == department)
        {

            std::cout << "请输入新的检查状态 (0 - 已下单, 1 - 已支付, 2 - 检查中, 3 - 检查完成, 4 - 报告已出, 5 - 已作废): ";
            int newStatus = selectIntCheck(0, 5);
            current->status = static_cast<ExaminationStatus>(newStatus);
            std::cout << "检查记录状态已更新！" << std::endl;
            found = true;
            break;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到指定的检查记录！" << std::endl;
    }
}
// 删除检查记录（逻辑删除，设置 isDeleted 标志）
void Admin::deleteExamination(Examination *&exam, const std::string &department)
{
    std::string examID = inputRecordIDCheck("请输入要删除的检查记录ID: ", {"exa"}); // 输入检查记录ID并检查格式

    bool found = false;

    Examination *current = exam;
    while (current != nullptr)
    {
        if (current->examinationID == examID && current->department == department)
        {
            current->isDeleted = true; // 逻辑删除
            std::cout << "ID: " << current->examinationID << " " << "检查记录已删除！" << std::endl;
            found = true;
            break;
        }
        current = current->next;
    }
    if (!found)
    {
        std::cout << "未找到指定的检查记录！" << std::endl;
    }
}
// 添加检查记录（根据输入信息创建新的 Examination 对象，并插入到链表中）
void Admin::addExamination(Examination *&exam, const std::string &department, Consultation *con, int &idCounter)
{
    Examination *newExam = new Examination();
    std::cout << department << " 当前可关联的看诊记录列表:" << std::endl;

    Consultation *currentCon = con;
    while (currentCon != nullptr)
    { // 只显示当前科室的看诊记录，并且状态为正在处理的记录
        if (!currentCon->isDeleted && currentCon->status == ConsultationStatus::IN_PROGRESS && currentCon->department == department)
        {
            std::string statusStr = conStatusToString(currentCon->status);
            std::cout << "ID: " << currentCon->consultationID
                      << ", 患者ID: " << currentCon->patientID
                      << ", 医生ID: " << currentCon->doctorID
                      << ", 时间: " << currentCon->consultationTime
                      << ", 科室: " << currentCon->department
                      << ", 状态: " << statusStr
                      << std::endl;
        }
        currentCon = currentCon->next;
    }

    std::string conID = inputRecordIDCheck("请输入要添加检查记录的看诊记录ID: ", {"con"}); // 输入看诊记录ID并检查格式

    currentCon = con;
    while (currentCon != nullptr)
    {
        if (!currentCon->isDeleted && currentCon->consultationID == conID && currentCon->department == department)
        {

            if (currentCon->status != ConsultationStatus::IN_PROGRESS)
            {
                std::cout << "只能关联正在处理的看诊记录！" << std::endl;
                delete newExam; // 释放内存
                return;
            }
            newExam->consultationID = conID;            // 关联看诊记录ID
            newExam->patientID = currentCon->patientID; // 从看诊记录获取患者ID
            newExam->doctorID = currentCon->doctorID;   // 从看诊记录获取医生ID
            newExam->department = department;           // 设置科室
            break;
        }
        currentCon = currentCon->next;
    }

    if (currentCon == nullptr)
    {
        std::cout << "未找到指定的看诊记录！无法添加检查记录。" << std::endl;
        delete newExam; // 释放内存
        return;
    }

    std::string itemName = ExaminationItemMenu();
    if (itemName == "0")
    {
        std::cout << "取消添加检查记录。" << std::endl;
        delete newExam; // 释放内存
        return;
    }

    newExam->itemName = itemName; // 设置检查项目名称
    // 生成唯一的检查ID（可以根据实际需求改为更复杂的生成方式）
    newExam->examinationID = "exa" + std::to_string(idCounter++).insert(0, 6 - std::to_string(idCounter).length(), '0');
    MyTime &t = MyTime::getInstance();
    newExam->orderTime = t.getTime(); // 获取当前时间字符串

    newExam->fee = inputFeeCheck("请输入检查费用: "); // 输入检查费用并检查格式

    // 插入到链表头部
    newExam->next = exam;
    if (exam != nullptr)
        exam->prev = newExam;
    exam = newExam;

    std::cout << "检查记录已添加！新检查ID: " << newExam->examinationID << std::endl;
}
// 根据检查记录ID查看检查记录
void Admin::viewExaminationByID(Examination *&exam, const std::string &department)
{
    std::string examID = inputRecordIDCheck("请输入要查看的检查记录ID: ", {"exa"}); // 输入检查记录ID并检查格式

    bool found = false;

    Examination *current = exam;
    std::cout << "正在查找检查记录ID: " << examID << " 的检查记录..." << std::endl;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->examinationID == examID && current->department == department)
        {
            std::string statusStr = examStatusToString(current->status);

            if (statusStr == "检查完成")
            {
                std::string statusStr = examStatusToString(current->status);
                std::cout << "ID: " << current->examinationID
                          << ", 患者ID: " << current->patientID
                          << ", 医生ID: " << current->doctorID
                          << ", 科室: " << current->department
                          << ", 检查项目: " << current->itemName
                          << ", 下单时间: " << current->orderTime
                          << ", 状态: " << statusStr
                          << ", 出报告时间: " << (current->reportTime.empty() ? "报告未出" : current->reportTime)
                          << ", 费用: " << current->fee
                          << std::endl;
                std::cout << "检查结果：" << findVitalSignToString(current) << std::endl;
                std::cout << "报告摘要：" << current->reportSummary << std::endl;
                std::cout << "医生备注：" << current->note << std::endl;
            }
            else
            {
                std::cout << "ID: " << current->examinationID
                          << ", 患者ID: " << current->patientID
                          << ", 医生ID: " << current->doctorID
                          << ", 科室: " << current->department
                          << ", 检查项目: " << current->itemName
                          << ", 下单时间: " << current->orderTime
                          << ", 状态: " << statusStr
                          << ", 出报告时间: " << (current->reportTime.empty() ? "报告未出" : current->reportTime)
                          << ", 费用: " << current->fee
                          << std::endl;
            }

            found = true;
            break;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到指定的检查记录！" << std::endl;
    }
}

void Admin::manageExaminations(Examination *&exam, const std::string &department, Consultation *con, int &idCounter)
{
    while (true)
    {
        int choice = adminExaminationManagementMenu();
        if (choice == 0)
        {
            break;
        }
        else if (choice == 1)
        {
            while (true)
            {
                int viewChoice = adminExaminationViewMenu();
                if (viewChoice == 0)
                {
                    break;
                }
                else if (viewChoice == 1)
                {
                    viewAllExaminations(exam, department);
                    pause();
                }
                else if (viewChoice == 2)
                {
                    viewExaminationsByPatient(exam, department);
                    pause();
                }
                else if (viewChoice == 3)
                {
                    viewExaminationsByDoctor(exam, department);
                    pause();
                }
                else if (viewChoice == 4)
                {
                    viewExaminationsByStatus(exam, department);
                    pause();
                }
                else if (viewChoice == 5)
                {
                    viewExaminationByID(exam, department);
                    pause();
                }
                else
                {
                    std::cout << "无效的选择! 请重新选择。" << std::endl;
                    pause();
                }
            }
        }
        else if (choice == 2)
        {
            modifyExaminationStatus(exam, department);
            pause();
        }
        else if (choice == 3)
        {
            deleteExamination(exam, department);
            pause();
        }
        else if (choice == 4)
        {
            addExamination(exam, department, con, idCounter);
            pause();
        }
        else
        {
            std::cout << "无效的选择! 请重新选择。" << std::endl;
            pause();
        }
    }
}

// ==================================== 住院记录管理 =================================

// 查看所有的住院记录
void Admin::viewAllHospitalizations(Hospitalization *&hos, const std::string &department)
{
    Hospitalization *current = hos;
    bool found = false;
    std::cout << "正在查找所有住院记录..." << std::endl;

    while (current != nullptr)
    {
        if (!current->isDeleted && current->department == department)
        {
            std::string statusStr = hosStatusToString(current->status);
            std::string wardTypeStr = current->wardType;
            std::cout << "ID: " << current->hospitalizationID
                      << ", 患者ID: " << current->patientID
                      << ", 医生ID: " << current->doctorID
                      << ", 负责护士ID: " << current->nurseID
                      << ", 科室: " << current->department
                      << ", 病房类型: " << (wardTypeStr.empty() ? "未分配" : wardTypeStr)
                      << ", 床位号: " << (current->bedNumber.empty() ? "未分配" : current->bedNumber)
                      << ", 申请时间: " << current->applyTime
                      << ", 允许入院时间: " << (current->availableAdmitTime.empty() ? "未分配" : current->availableAdmitTime)
                      << ", 实际入院时间: " << (current->admitTime.empty() ? "未入院" : current->admitTime)
                      << ", 出院时间: " << (current->dischargeTime.empty() ? "未出院" : current->dischargeTime)
                      << ", 押金: " << current->deposit
                      << ", 总费用: " << current->totalCost
                      << ", 住院记录状态: " << statusStr
                      << std::endl;

            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "该科室暂无住院记录！" << std::endl;
    }
}
// 根据患者ID查看住院记录
void Admin::viewHospitalizationsByPatient(Hospitalization *&hos, const std::string &department)
{
    std::string patientID = inputIDCheck("请输入患者ID: ");

    bool found = false;

    Hospitalization *current = hos;
    std::cout << "正在查找患者ID: " << patientID << " 的住院记录..." << std::endl;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->department == department && current->patientID == patientID)
        {
            std::string statusStr = hosStatusToString(current->status);
            std::string wardTypeStr = current->wardType;
            std::cout << "ID: " << current->hospitalizationID
                      << ", 患者ID: " << current->patientID
                      << ", 医生ID: " << current->doctorID
                      << ", 负责护士ID: " << current->nurseID
                      << ", 科室: " << current->department
                      << ", 病房类型: " << (wardTypeStr.empty() ? "未分配" : wardTypeStr)
                      << ", 床位号: " << (current->bedNumber.empty() ? "未分配" : current->bedNumber)
                      << ", 申请时间: " << current->applyTime
                      << ", 允许入院时间: " << (current->availableAdmitTime.empty() ? "未分配" : current->availableAdmitTime)
                      << ", 实际入院时间: " << (current->admitTime.empty() ? "未入院" : current->admitTime)
                      << ", 出院时间: " << (current->dischargeTime.empty() ? "未出院" : current->dischargeTime)
                      << ", 押金: " << current->deposit
                      << ", 总费用: " << current->totalCost
                      << ", 住院记录状态: " << statusStr
                      << std::endl;

            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到该患者的住院记录！" << std::endl;
        return;
    }
}
// 根据医生ID查看住院记录
void Admin::viewHospitalizationsByDoctor(Hospitalization *&hos, const std::string &department)
{
    std::string doctorID = inputIDCheck("请输入医生ID: ");

    bool found = false;

    Hospitalization *current = hos;
    std::cout << "正在查找医生ID: " << doctorID << " 的住院记录..." << std::endl;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->department == department && current->doctorID == doctorID)
        {
            std::string statusStr = hosStatusToString(current->status);
            std::string wardTypeStr = current->wardType;
            std::cout << "ID: " << current->hospitalizationID
                      << ", 患者ID: " << current->patientID
                      << ", 医生ID: " << current->doctorID
                      << ", 负责护士ID: " << current->nurseID
                      << ", 科室: " << current->department
                      << ", 病房类型: " << (wardTypeStr.empty() ? "未分配" : wardTypeStr)
                      << ", 床位号: " << (current->bedNumber.empty() ? "未分配" : current->bedNumber)
                      << ", 申请时间: " << current->applyTime
                      << ", 允许入院时间: " << (current->availableAdmitTime.empty() ? "未分配" : current->availableAdmitTime)
                      << ", 实际入院时间: " << (current->admitTime.empty() ? "未入院" : current->admitTime)
                      << ", 出院时间: " << (current->dischargeTime.empty() ? "未出院" : current->dischargeTime)
                      << ", 押金: " << current->deposit
                      << ", 总费用: " << current->totalCost
                      << ", 住院记录状态: " << statusStr
                      << std::endl;

            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到该医生的住院记录！" << std::endl;
        return;
    }
}
// 根据负责护士ID查看住院记录
void Admin::viewHospitalizationByNurse(Hospitalization *&hos, const std::string &department)
{
    std::string nurseID = inputIDCheck("请输入负责护士ID: ");

    bool found = false;

    Hospitalization *current = hos;
    std::cout << "正在查找负责护士ID: " << nurseID << " 的住院记录..." << std::endl;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->department == department && current->nurseID == nurseID)
        {
            std::string statusStr = hosStatusToString(current->status);
            std::string wardTypeStr = current->wardType;
            std::cout << "ID: " << current->hospitalizationID
                      << ", 患者ID: " << current->patientID
                      << ", 医生ID: " << current->doctorID
                      << ", 负责护士ID: " << current->nurseID
                      << ", 科室: " << current->department
                      << ", 病房类型: " << (wardTypeStr.empty() ? "未分配" : wardTypeStr)
                      << ", 床位号: " << (current->bedNumber.empty() ? "未分配" : current->bedNumber)
                      << ", 申请时间: " << current->applyTime
                      << ", 允许入院时间: " << (current->availableAdmitTime.empty() ? "未分配" : current->availableAdmitTime)
                      << ", 实际入院时间: " << (current->admitTime.empty() ? "未入院" : current->admitTime)
                      << ", 出院时间: " << (current->dischargeTime.empty() ? "未出院" : current->dischargeTime)
                      << ", 押金: " << current->deposit
                      << ", 总费用: " << current->totalCost
                      << ", 住院记录状态: " << statusStr
                      << std::endl;

            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到该负责护士的住院记录！" << std::endl;
        return;
    }
}
// 根据住院记录ID查看住院记录
void Admin::viewHospitalizationByID(Hospitalization *&hos, const std::string &department)
{
    std::string hosID = inputRecordIDCheck("请输入要查看的住院记录ID: ", {"hos"}); // 输入住院记录ID并检查格式

    bool found = false;

    Hospitalization *current = hos;
    std::cout << "正在查找住院记录ID: " << hosID << " 的住院记录..." << std::endl;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->hospitalizationID == hosID && current->department == department)
        {
            std::string statusStr = hosStatusToString(current->status);
            std::string wardTypeStr = current->wardType;
            std::cout << "ID: " << current->hospitalizationID
                      << ", 患者ID: " << current->patientID
                      << ", 医生ID: " << current->doctorID
                      << ", 负责护士ID: " << current->nurseID
                      << ", 科室: " << current->department
                      << ", 病房类型: " << (wardTypeStr.empty() ? "未分配" : wardTypeStr)
                      << ", 床位号: " << (current->bedNumber.empty() ? "未分配" : current->bedNumber)
                      << ", 申请时间: " << current->applyTime
                      << ", 允许入院时间: " << (current->availableAdmitTime.empty() ? "未分配" : current->availableAdmitTime)
                      << ", 实际入院时间: " << (current->admitTime.empty() ? "未入院" : current->admitTime)
                      << ", 出院时间: " << (current->dischargeTime.empty() ? "未出院" : current->dischargeTime)
                      << ", 押金: " << current->deposit
                      << ", 总费用: " << current->totalCost
                      << ", 住院记录状态: " << statusStr
                      << std::endl;

            found = true;
            break;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到指定的住院记录！" << std::endl;
    }
}
// 根据住院状态查看住院记录
void Admin::viewHospitalizationsByStatus(Hospitalization *&hos, const std::string &department)
{
    std::cout << "请输入要过滤的住院记录状态 (0 - 申请中, 1 - 已缴费待分床, 2 - 已入院, 3 - 已出院, 4 - 已作废): ";
    int statusFilter = selectIntCheck(0, 4);

    bool found = false;

    HospitalizationStatus filterStatus = static_cast<HospitalizationStatus>(statusFilter);

    Hospitalization *current = hos;
    std::cout << "正在查找状态为: " << hosStatusToString(filterStatus) << " 的住院记录..." << std::endl;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->department == department && current->status == filterStatus)
        {
            std::string statusStr = hosStatusToString(current->status);
            std::string wardTypeStr = current->wardType;
            std::cout << "ID: " << current->hospitalizationID
                      << ", 患者ID: " << current->patientID
                      << ", 医生ID: " << current->doctorID
                      << ", 负责护士ID: " << current->nurseID
                      << ", 科室: " << current->department
                      << ", 病房类型: " << (wardTypeStr.empty() ? "未分配" : wardTypeStr)
                      << ", 床位号: " << (current->bedNumber.empty() ? "未分配" : current->bedNumber)
                      << ", 申请时间: " << current->applyTime
                      << ", 允许入院时间: " << (current->availableAdmitTime.empty() ? "未分配" : current->availableAdmitTime)
                      << ", 实际入院时间: " << (current->admitTime.empty() ? "未入院" : current->admitTime)
                      << ", 出院时间: " << (current->dischargeTime.empty() ? "未出院" : current->dischargeTime)
                      << ", 押金: " << current->deposit
                      << ", 总费用: " << current->totalCost
                      << ", 住院记录状态: " << statusStr
                      << std::endl;

            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到该状态的住院记录！" << std::endl;
        return;
    }
}
// 根据病房类型查看住院记录
void Admin::viewHospitalizationByWardType(Hospitalization *&hos, const std::string &department)
{
    std::string wardTypeStr = HospitalizationWardTypeMenu();

    bool found = false;

    Hospitalization *current = hos;
    std::cout << "正在查找病房类型为: " << wardTypeStr << " 的住院记录..." << std::endl;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->department == department && current->wardType == wardTypeStr)
        {
            std::string statusStr = hosStatusToString(current->status);
            std::cout << "ID: " << current->hospitalizationID
                      << ", 患者ID: " << current->patientID
                      << ", 医生ID: " << current->doctorID
                      << ", 负责护士ID: " << current->nurseID
                      << ", 科室: " << current->department
                      << ", 病房类型: " << current->wardType
                      << ", 床位号: " << (current->bedNumber.empty() ? "未分配" : current->bedNumber)
                      << ", 申请时间: " << current->applyTime
                      << ", 允许入院时间: " << (current->availableAdmitTime.empty() ? "未分配" : current->availableAdmitTime)
                      << ", 实际入院时间: " << (current->admitTime.empty() ? "未入院" : current->admitTime)
                      << ", 出院时间: " << (current->dischargeTime.empty() ? "未出院" : current->dischargeTime)
                      << ", 押金: " << current->deposit
                      << ", 总费用: " << current->totalCost
                      << ", 住院记录状态: " << statusStr
                      << std::endl;

            found = true;
        }
        current = current->next;
    }
}
// 修改住院记录的状态
void Admin::modifyHospitalizationStatus(Hospitalization *&hos, const std::string &department)
{
    std::string hosID = inputRecordIDCheck("请输入要修改状态的住院记录ID: ", {"hos"}); // 输入住院记录ID并检查格式

    bool found = false;

    Hospitalization *current = hos;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->hospitalizationID == hosID && current->department == department)
        {

            std::cout << "请输入新的住院记录状态 (0 - 申请中, 1 - 已缴费待分床, 2 - 已入院, 3 - 已出院, 4 - 已作废): ";
            int newStatus = selectIntCheck(0, 4);
            current->status = static_cast<HospitalizationStatus>(newStatus);
            std::cout << "住院记录状态已更新！" << std::endl;
            found = true;
            break;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到指定的住院记录！" << std::endl;
    }
}
// 删除住院记录（逻辑删除，设置 isDeleted 标志）
void Admin::deleteHospitalization(Hospitalization *&hos, const std::string &department)
{
    std::string hosID = inputRecordIDCheck("请输入要删除的住院记录ID: ", {"hos"}); // 输入住院记录ID并检查格式

    bool found = false;

    Hospitalization *current = hos;
    while (current != nullptr)
    {
        if (current->hospitalizationID == hosID && current->department == department)
        {
            current->isDeleted = true; // 逻辑删除
            std::cout << "ID: " << current->hospitalizationID << " " << "住院记录已删除！" << std::endl;
            found = true;
            break;
        }
        current = current->next;
    }
    if (!found)
    {
        std::cout << "未找到指定的住院记录！" << std::endl;
    }
}
// 添加住院记录（根据输入信息创建新的 Hospitalization 对象，并插入到链表中）
void Admin::addHospitalization(Hospitalization *&hos, const std::string &department, Consultation *con, bedInfo *&bed, int &idCounter)
{
    Consultation *currentCon = con;
    std::cout << department << " 当前待处理的关联的看诊记录列表:" << std::endl;
    while (currentCon != nullptr)
    { // 只显示当前科室的看诊记录，并且状态为正在处理的记录
        if (!currentCon->isDeleted && currentCon->status == ConsultationStatus::IN_PROGRESS && currentCon->isHospitalizationRecommended && currentCon->department == department)
        {
            std::string statusStr = conStatusToString(currentCon->status);
            std::cout << "ID: " << currentCon->consultationID
                      << ", 患者ID: " << currentCon->patientID
                      << ", 医生ID: " << currentCon->doctorID
                      << ", 时间: " << currentCon->consultationTime
                      << ", 科室: " << currentCon->department
                      << ", 状态: " << statusStr
                      << ", 是否建议住院: " << (currentCon->isHospitalizationRecommended ? "是" : "否")
                      << std::endl;
        }
        currentCon = currentCon->next;
    }

    std::string conID = inputRecordIDCheck("请输入要添加住院记录的看诊记录ID: ", {"con"}); // 输入看诊记录ID并检查格式

    Hospitalization *newHos = new Hospitalization();

    currentCon = con;
    while (currentCon != nullptr)
    {
        if (!currentCon->isDeleted && currentCon->consultationID == conID && currentCon->department == department)
        {

            if (currentCon->status != ConsultationStatus::IN_PROGRESS)
            {
                std::cout << "只能关联正在处理的看诊记录！" << std::endl;
                return;
            }
            if (!currentCon->isHospitalizationRecommended)
            {
                std::cout << "该看诊记录未建议住院，无法关联！" << std::endl;
                return;
            }

            newHos->consultationID = conID;            // 关联看诊记录ID
            newHos->patientID = currentCon->patientID; // 从看诊记录获取患者ID
            newHos->doctorID = currentCon->doctorID;   // 从看诊记录获取医生ID
            newHos->department = department;           // 设置科室

            break;
        }
        currentCon = currentCon->next;
    }

    if (currentCon == nullptr)
    {
        std::cout << "未找到指定的看诊记录！无法添加住院记录。" << std::endl;
        delete newHos; // 释放内存
        return;
    }

    newHos->wardType = HospitalizationWardTypeMenu(); // 选择病房类型
    if (newHos->wardType == "0")
    {
        std::cout << "取消添加住院记录。" << std::endl;
        delete newHos; // 释放内存
        return;
    }

    bedInfo *b = bed;
    bool bedFound = false;

    std::cout << "正在检查病房类型为: " << newHos->wardType << " 的空闲床位..." << std::endl;
    while (b != nullptr)
    {
        if (b->status == bedStatus::AVAILABLE && b->department == department && b->wardType == newHos->wardType)
        {
            std::cout << "区域：" << b->areaNumber << "病房号：" << b->wardNumber << "床位号：" << b->bedNumber << " 空闲" << std::endl;
            bedFound = true;
        }
        b = b->next;
    }

    if (!bedFound)
    {
        std::cout << "未找到空闲床位！无法添加住院记录。" << std::endl;
        delete newHos; // 释放内存
        return;
    }

    bedFound = false; // 重置标志，准备检查用户输入的床位号

    newHos->bedNumber = inputBedNumberCheck("请输入床位号: ", department, newHos->wardType); // 输入床位号并检查格式

    b = bed;
    while (b != nullptr)
    {
        if (b->department == department && b->wardType == newHos->wardType && b->bedID == newHos->bedNumber)
        {
            if (b->status == bedStatus::OCCUPIED)
            {
                std::cout << "该床位已被占用！无法添加住院记录。" << std::endl;
                delete newHos; // 释放内存
                return;
            }
            else if (b->status == bedStatus::ClEANING)
            {
                std::cout << "该床位正在清洁中！无法添加住院记录。" << std::endl;
                delete newHos; // 释放内存
                return;
            }
            else if (b->status == bedStatus::UNAVAILABLE)
            {
                std::cout << "该床位不可用！无法添加住院记录。" << std::endl;
                delete newHos; // 释放内存
                return;
            }
            else
            {
                b->status = bedStatus::OCCUPIED; // 占用床位
                break;
            }

            bedFound = true;
        }
        b = b->next;
    }

    if(!bedFound)
    {
        std::cout << "未找到指定的床位！无法添加住院记录。" << std::endl;
        delete newHos; // 释放内存
        return;
    }


    // 生成唯一的住院记录ID（可以根据实际需求改为更复杂的生成方式）
    newHos->hospitalizationID = "hos" + std::to_string(idCounter++).insert(0, 6 - std::to_string(idCounter).length(), '0');
    MyTime &t = MyTime::getInstance();
    newHos->applyTime = t.getTime(); // 获取当前时间字符串

    newHos->nurseID = inputIDCheck("请输入负责护士ID: "); // 输入负责护士ID并检查格式
    newHos->deposit = inputFeeCheck("请输入押金金额: ");  // 输入押金金额并检查格式

    // 插入到链表头部
    newHos->next = hos;
    if (hos != nullptr)
    {
        hos->prev = newHos;
    }
    hos = newHos;
}

void Admin::manageHospitalizations(Hospitalization *&hos, const std::string &department, Consultation *con, bedInfo *bed, int &idCounter)
{
    while (true)
    {
        int choice = adminHospitalizationManagementMenu();
        if (choice == 0)
        {
            break;
        }
        else if (choice == 1)
        {
            while (true)
            {
                int viewChoice = adminHospitalizationViewMenu();
                if (viewChoice == 0)
                {
                    break;
                }
                else if (viewChoice == 1)
                {
                    viewAllHospitalizations(hos, department);
                    pause();
                }
                else if (viewChoice == 2)
                {
                    viewHospitalizationsByPatient(hos, department);
                    pause();
                }
                else if (viewChoice == 3)
                {
                    viewHospitalizationsByDoctor(hos, department);
                    pause();
                }
                else if (viewChoice == 4)
                {
                    viewHospitalizationByNurse(hos, department);
                    pause();
                }
                else if (viewChoice == 5)
                {
                    viewHospitalizationByID(hos, department);
                    pause();
                }
                else if (viewChoice == 6)
                {
                    viewHospitalizationsByStatus(hos, department);
                    pause();
                }
                else if (viewChoice == 7)
                {
                    viewHospitalizationByWardType(hos, department);
                    pause();
                }
                else
                {
                    std::cout << "无效的选择! 请重新选择。" << std::endl;
                    pause();
                }
            }
        }
        else if (choice == 2)
        {
            modifyHospitalizationStatus(hos, department);
            pause();
        }
        else if (choice == 3)
        {
            deleteHospitalization(hos, department);
            pause();
        }
        else if (choice == 4)
        {
            addHospitalization(hos, department, con, bed, idCounter);
            pause();
        }
        else
        {
            std::cout << "无效的选择! 请重新选择。" << std::endl;
            pause();
        }
    }
}

void Admin::manageMedicationRecords(MedicationRecord *&medRec, const std::string &department, int &idCounter)
{ // 管理用药记录的函数实现（类似于 manageRegistrations，可以根据实际需求添加查看、修改等功能）
    std::cout << "管理用药记录功能尚未实现！" << std::endl;
}