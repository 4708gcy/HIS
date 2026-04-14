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
            std::cout << "医生ID: " << currentDoc->getUserID() << ", 姓名: " << currentDoc->getUsername() << ", 擅长领域：" << currentDoc->specialization << ", 职称：" << currentDoc->title << ", 联系方式：" << currentDoc->getTelephone() << " / " << currentDoc->getEmail()
                      << std::endl;
        }
        currentDoc = currentDoc->next;
    }

    newReg->doctorID = inputIDCheck("请输入医生ID: ");

    currentDoc = doc;
    bool doctorExists = false;
    while (currentDoc != nullptr)
    {
        if (currentDoc->getUserID() == newReg->doctorID && currentDoc->department == department)
        {
            doctorExists = true;
            break;
        }
        currentDoc = currentDoc->next;
    }

    if (!doctorExists)
    {
        std::cout << "未找到指定的医生！" << std::endl;
        delete newReg;
        return;
    }

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
                return;
            }

            std::cout << "看诊记录中的计划检查项目: ";
            for (const auto &item : currentCon->examinationlist)
            {
                Examination *newExam = new Examination();
                std::cout << item << std::endl;

                newExam->consultationID = conID;            // 关联看诊记录ID
                newExam->patientID = currentCon->patientID; // 从看诊记录获取患者ID
                newExam->doctorID = currentCon->doctorID;   // 从看诊记录获取医生ID
                newExam->department = department;           // 设置科室

                std::string itemName = ExaminationItemMenu();
                if (itemName == "0")
                {
                    std::cout << "结束添加检查记录。" << std::endl;
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

            break;
        }
        currentCon = currentCon->next;
    }

    if (currentCon == nullptr)
    {
        std::cout << "未找到指定的看诊记录！无法添加检查记录。" << std::endl;
        return;
    }
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
void Admin::addHospitalization(Hospitalization *&hos, Nurse *nurse, const std::string &department, Consultation *con, bedInfo *&bed, int &idCounter)
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
        if (!b->isDeleted && b->status == bedStatus::AVAILABLE && b->department == department && b->wardType == newHos->wardType)
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
        if (!b->isDeleted && b->department == department && b->wardType == newHos->wardType && b->bedID == newHos->bedNumber)
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
                b->status = bedStatus::OCCUPIED;  // 占用床位
                b->patientID = newHos->patientID; // 关联患者ID
                b->useTimes++;                    // 使用次数加1
                break;
            }

            bedFound = true;
        }
        b = b->next;
    }

    if (!bedFound)
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

    Nurse *n = nurse;
    bool nurseFound = false;
    while (n != nullptr)
    {
        if (!n->isDeleted && n->getUserID() == newHos->nurseID && n->department == department)
        {
            nurseFound = true;
            break;
        }
        n = n->next;
    }

    if (!nurseFound)
    {
        std::cout << "未找到指定的护士！无法添加住院记录。" << std::endl;
        delete newHos; // 释放内存
        return;
    }

    newHos->deposit = inputFeeCheck("请输入押金金额: "); // 输入押金金额并检查格式

    // 插入到链表头部
    newHos->next = hos;
    if (hos != nullptr)
    {
        hos->prev = newHos;
    }
    hos = newHos;
}

void Admin::manageHospitalizations(Hospitalization *&hos, Nurse *nurse, const std::string &department, Consultation *con, bedInfo *bed, int &idCounter)
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
            addHospitalization(hos, nurse, department, con, bed, idCounter);
            pause();
        }
        else
        {
            std::cout << "无效的选择! 请重新选择。" << std::endl;
            pause();
        }
    }
}

// ==================================== 床位信息管理 =================================

// 查看所有床位信息
void Admin::viewAllBeds(bedInfo *&bed, const std::string &department)
{
    bedInfo *current = bed;
    bool found = false;
    std::cout << "正在查找所有床位信息..." << std::endl;

    while (current != nullptr)
    {
        if (!current->isDeleted && current->department == department)
        {
            std::string statusStr = bedStatusToString(current->status);
            std::cout << "床位ID: " << current->bedID
                      << ", 科室: " << current->department
                      << ", 区域: " << current->areaNumber
                      << ", 病房号: " << current->wardNumber
                      << ", 床位号: " << current->bedNumber
                      << ", 病房类型: " << current->wardType
                      << ", 床位状态: " << statusStr
                      << ", 患者ID: " << (current->patientID.empty() ? "无" : current->patientID)
                      << ", 护士ID: " << (current->nurseID.empty() ? "无" : current->nurseID)
                      << ", 使用次数: " << current->useTimes
                      << ", 占用天数: " << current->daysOccupied
                      << std::endl;

            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "该科室暂无床位信息！" << std::endl;
    }
}
// 根据病房类型查看床位信息
void Admin::viewBedsByWardType(bedInfo *&bed, const std::string &department)
{
    std::string wardTypeStr = HospitalizationWardTypeMenu();

    bool found = false;

    bedInfo *current = bed;
    std::cout << "正在查找病房类型为: " << wardTypeStr << " 的床位信息..." << std::endl;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->department == department && current->wardType == wardTypeStr)
        {
            std::string statusStr = bedStatusToString(current->status);
            std::cout << "床位ID: " << current->bedID
                      << ", 科室: " << current->department
                      << ", 区域: " << current->areaNumber
                      << ", 病房号: " << current->wardNumber
                      << ", 床位号: " << current->bedNumber
                      << ", 病房类型: " << current->wardType
                      << ", 床位状态: " << statusStr
                      << ", 患者ID: " << (current->patientID.empty() ? "无" : current->patientID)
                      << ", 护士ID: " << (current->nurseID.empty() ? "无" : current->nurseID)
                      << ", 使用次数: " << current->useTimes
                      << ", 占用天数: " << current->daysOccupied
                      << std::endl;

            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到该病房类型的床位信息！" << std::endl;
        return;
    }
}
// 根据床位状态查看床位信息
void Admin::viewBedsByStatus(bedInfo *&bed, const std::string &department)
{
    std::cout << "请输入要过滤的床位状态 (0 - 可用, 1 - 占用, 2 - 清洁中, 3 - 不可用): ";
    int statusFilter = selectIntCheck(0, 3);

    bool found = false;

    bedStatus filterStatus = static_cast<bedStatus>(statusFilter);

    bedInfo *current = bed;
    std::cout << "正在查找状态为: " << bedStatusToString(filterStatus) << " 的床位信息..." << std::endl;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->department == department && current->status == filterStatus)
        {
            std::string statusStr = bedStatusToString(current->status);
            std::cout << "床位ID: " << current->bedID
                      << ", 科室: " << current->department
                      << ", 区域: " << current->areaNumber
                      << ", 病房号: " << current->wardNumber
                      << ", 床位号: " << current->bedNumber
                      << ", 病房类型: " << current->wardType
                      << ", 床位状态: " << statusStr
                      << ", 患者ID: " << (current->patientID.empty() ? "无" : current->patientID)
                      << ", 护士ID: " << (current->nurseID.empty() ? "无" : current->nurseID)
                      << ", 使用次数: " << current->useTimes
                      << ", 占用天数: " << current->daysOccupied
                      << std::endl;

            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到该状态的床位信息！" << std::endl;
        return;
    }
}
// 根据床位ID查看床位信息
void Admin::viewBedByID(bedInfo *&bed, const std::string &department)
{
    std::string wardType = HospitalizationWardTypeMenu();                            // 选择病房类型，作为输入床位ID的前缀检查
    std::string bedID = inputBedNumberCheck("请输入床位号: ", department, wardType); // 输入床位ID并检查格式

    bool found = false;

    bedInfo *current = bed;
    std::cout << "正在查找床位ID: " << bedID << " 的床位信息..." << std::endl;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->bedID == bedID && current->department == department)
        {
            std::string statusStr = bedStatusToString(current->status);
            std::cout << "床位ID: " << current->bedID
                      << ", 科室: " << current->department
                      << ", 区域: " << current->areaNumber
                      << ", 病房号: " << current->wardNumber
                      << ", 床位号: " << current->bedNumber
                      << ", 病房类型: " << current->wardType
                      << ", 床位状态: " << statusStr
                      << ", 患者ID: " << (current->patientID.empty() ? "无" : current->patientID)
                      << ", 护士ID: " << (current->nurseID.empty() ? "无" : current->nurseID)
                      << ", 使用次数: " << current->useTimes
                      << ", 占用天数: " << current->daysOccupied
                      << std::endl;

            found = true;
            break;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到指定的床位信息！" << std::endl;
        return;
    }
}
// 根据护士ID查看床位信息
void Admin::viewBedsByNurseID(bedInfo *&bed, const std::string &department)
{
    std::string nurseID = inputIDCheck("请输入负责护士ID: ");

    bool found = false;

    bedInfo *current = bed;
    std::cout << "正在查找负责护士ID: " << nurseID << " 的床位信息..." << std::endl;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->department == department && current->nurseID == nurseID)
        {
            std::string statusStr = bedStatusToString(current->status);
            std::cout << "床位ID: " << current->bedID
                      << ", 科室: " << current->department
                      << ", 区域: " << current->areaNumber
                      << ", 病房号: " << current->wardNumber
                      << ", 床位号: " << current->bedNumber
                      << ", 病房类型: " << current->wardType
                      << ", 床位状态: " << statusStr
                      << ", 患者ID: " << (current->patientID.empty() ? "无" : current->patientID)
                      << ", 护士ID: " << (current->nurseID.empty() ? "无" : current->nurseID)
                      << ", 使用次数: " << current->useTimes
                      << ", 占用天数: " << current->daysOccupied
                      << std::endl;

            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到该负责护士的床位信息！" << std::endl;
        return;
    }
}
// 根据患者ID查看床位信息
void Admin::viewBedsByPatientID(bedInfo *&bed, const std::string &department)
{
    std::string patientID = inputIDCheck("请输入患者ID: ");

    bool found = false;

    bedInfo *current = bed;
    std::cout << "正在查找患者ID: " << patientID << " 的床位信息..." << std::endl;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->department == department && current->patientID == patientID)
        {
            std::string statusStr = bedStatusToString(current->status);
            std::cout << "床位ID: " << current->bedID
                      << ", 科室: " << current->department
                      << ", 区域: " << current->areaNumber
                      << ", 病房号: " << current->wardNumber
                      << ", 床位号: " << current->bedNumber
                      << ", 病房类型: " << current->wardType
                      << ", 床位状态: " << statusStr
                      << ", 患者ID: " << (current->patientID.empty() ? "无" : current->patientID)
                      << ", 护士ID: " << (current->nurseID.empty() ? "无" : current->nurseID)
                      << ", 使用次数: " << current->useTimes
                      << ", 占用天数: " << current->daysOccupied
                      << std::endl;

            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到该患者的床位信息！" << std::endl;
        return;
    }
}
// 根据住院记录ID查看床位信息
void Admin::viewBedsByHospitalizationID(bedInfo *&bed, Hospitalization *&hos, const std::string &department)
{
    std::string hosID = inputRecordIDCheck("请输入住院记录ID: ", {"hos"});

    Hospitalization *currentHos = hos;
    std::string bedID;
    bool foundHos = false;

    while (currentHos != nullptr)
    {
        if (!currentHos->isDeleted && currentHos->hospitalizationID == hosID && currentHos->department == department)
        {
            bedID = currentHos->bedNumber;
            foundHos = true;
            break;
        }
        currentHos = currentHos->next;
    }

    if (!foundHos)
    {
        std::cout << "未找到指定的住院记录！" << std::endl;
        return;
    }

    bedInfo *currentBed = bed;
    bool foundBed = false;

    while (currentBed != nullptr)
    {
        if (!currentBed->isDeleted && currentBed->bedID == bedID && currentBed->department == department)
        {
            std::string statusStr = bedStatusToString(currentBed->status);
            std::cout << "床位ID: " << currentBed->bedID
                      << ", 科室: " << currentBed->department
                      << ", 区域: " << currentBed->areaNumber
                      << ", 病房号: " << currentBed->wardNumber
                      << ", 床位号: " << currentBed->bedNumber
                      << ", 病房类型: " << currentBed->wardType
                      << ", 床位状态: " << statusStr
                      << ", 患者ID: " << (currentBed->patientID.empty() ? "无" : currentBed->patientID)
                      << ", 护士ID: " << (currentBed->nurseID.empty() ? "无" : currentBed->nurseID)
                      << ", 使用次数: " << currentBed->useTimes
                      << ", 占用天数: " << currentBed->daysOccupied
                      << std::endl;

            foundBed = true;
            break;
        }
        currentBed = currentBed->next;
    }

    if (!foundBed)
    {
        std::cout << "未找到对应床位的信息！" << std::endl;
    }
}
// 修改床位状态
void Admin::modifyBedStatus(bedInfo *&bed, const std::string &department)
{
    std::string wardType = HospitalizationWardTypeMenu();                                        // 选择病房类型，作为输入床位ID的前缀检查
    std::string bedID = inputBedNumberCheck("请输入要修改状态的床位号: ", department, wardType); // 输入床位ID并检查格式

    bool found = false;

    bedInfo *current = bed;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->bedID == bedID && current->department == department)
        {
            std::cout << "请输入新的床位状态 (0 - 可用, 1 - 占用, 2 - 清洁中, 3 - 不可用): ";
            int newStatus = selectIntCheck(0, 3);
            current->status = static_cast<bedStatus>(newStatus);
            std::cout << "床位状态已更新！" << std::endl;
            found = true;
            break;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到指定的床位信息！" << std::endl;
    }
}
// 删除床位信息（逻辑删除，设置 isDeleted 标志）
void Admin::deleteBedInfo(bedInfo *&bed, const std::string &department)
{
    std::string wardType = HospitalizationWardTypeMenu();                                    // 选择病房类型，作为输入床位ID的前缀检查
    std::string bedID = inputBedNumberCheck("请输入要删除的床位号: ", department, wardType); // 输入床位ID并检查格式

    bool found = false;

    bedInfo *current = bed;
    while (current != nullptr)
    {
        if (current->bedID == bedID && current->department == department)
        {
            current->isDeleted = true; // 逻辑删除
            std::cout << "床位ID: " << current->bedID << " 已删除！" << std::endl;
            found = true;
            break;
        }
        current = current->next;
    }
    if (!found)
    {
        std::cout << "未找到指定的床位信息！" << std::endl;
    }
}
// 添加床位信息（根据输入信息创建新的 bedInfo 对象，并插入到链表中）
void Admin::addBedInfo(bedInfo *&bed, const std::string &department)
{

    std::string wardType = HospitalizationWardTypeMenu(); // 选择病房类型

    if (wardType == "0")
    {
        std::cout << "取消添加床位信息。" << std::endl;
        return;
    }

    bedInfo *newBed = new bedInfo();

    newBed->wardType = wardType; // 设置病房类型

    newBed->department = department; // 设置科室

    std::cout << "请输入新添加病房所在的区域号(0-10)：" << std::endl;
    newBed->areaNumber = selectIntCheck(0, 10); // 输入区域号并检查格式，假设区域号在0-10之间

    std::cout << "请输入新添加病房的病房号(0-100)：" << std::endl;
    newBed->wardNumber = selectIntCheck(0, 100); // 输入病房号并检查格式，假设病房号在0-100之间

    std::cout << "请输入新添加病房的床位号(0-10)：" << std::endl;
    newBed->bedNumber = selectIntCheck(0, 10); // 输入床位号并检查格式，假设床位号在0-10之间

    newBed->bedID = autoGenerateBedID(department, wardType, newBed->areaNumber, newBed->wardNumber, newBed->bedNumber); // 自动生成床位ID

    newBed->status = bedStatus::AVAILABLE; // 新增床位默认为可用状态

    // 插入到链表头部
    newBed->next = bed;
    if (bed != nullptr)
    {
        bed->prev = newBed;
    }
    bed = newBed;

    std::cout << "床位信息已添加！床位ID: " << newBed->bedID << std::endl;
}

// 床位管理功能汇总
void Admin::manageBedInfo(bedInfo *&bed, Hospitalization *&hos, const std::string &department)
{
    while (true)
    {
        int choice = bedManagementMenu();
        if (choice == 0)
        {
            break;
        }
        else if (choice == 1)
        {
            while (true)
            {
                int viewChoice = bedViewMenu();
                if (viewChoice == 0)
                {
                    break;
                }
                else if (viewChoice == 1) // 查看所有床位信息
                {
                    viewAllBeds(bed, department);
                    pause();
                }
                else if (viewChoice == 2) // 根据床位状态查看床位信息
                {
                    viewBedsByStatus(bed, department);

                    pause();
                }
                else if (viewChoice == 3) // 根据病房类型查看床位信息
                {
                    viewBedsByWardType(bed, department);
                    pause();
                }
                else if (viewChoice == 4) // 根据床位ID查看床位信息
                {
                    viewBedByID(bed, department);
                    pause();
                }
                else if (viewChoice == 5) // 根据患者ID查看床位信息
                {
                    viewBedsByPatientID(bed, department);

                    pause();
                }
                else if (viewChoice == 6) // 根据住院记录ID查看床位信息
                {
                    viewBedsByHospitalizationID(bed, hos, department);
                    pause();
                }
                else if (viewChoice == 7) // 根据护士ID查看床位信息
                {
                    viewBedsByNurseID(bed, department);

                    pause();
                }
                else
                {
                    std::cout << "无效的选择! 请重新选择。" << std::endl;
                    pause();
                }
            }
        }
        else if (choice == 2) // 修改床位状态
        {
            modifyBedStatus(bed, department);
            pause();
        }
        else if (choice == 3) // 删除床位信息
        {
            deleteBedInfo(bed, department);
            pause();
        }
        else if (choice == 4) // 添加床位信息
        {
            addBedInfo(bed, department);
            pause();
        }
        else
        {
            std::cout << "无效的选择! 请重新选择。" << std::endl;
            pause();
        }
    }
}

// ==================================== 用药记录管理 =================================

// 查看所有用药记录
void Admin::viewAllMedicationRecords(MedicationRecord *&medRec, const std::string &department)
{
    MedicationRecord *current = medRec;
    bool found = false;
    std::cout << "正在查找所有用药记录..." << std::endl;

    while (current != nullptr)
    {
        if (!current->isDeleted && current->department == department)
        {
            std::string reviewStatusStr = medicationReviewStatusToString(current->reviewStatus);
            std::string statusStr = medicationStatusToString(current->status);
            std::cout << "用药记录ID: " << current->medRecordID
                      << ", 关联看诊记录ID: " << current->consultationID
                      << ", 患者ID: " << current->patientID
                      << ", 医生ID: " << current->doctorID
                      << ", 药师ID: " << (current->pharmacistID.empty() ? "无" : current->pharmacistID)
                      << ", 科室: " << current->department
                      << ", 审核状态: " << statusStr
                      << ", 总费用: " << current->totalCost
                      << ", 支付时间: " << (current->paymentTime.empty() ? "未支付" : current->paymentTime)
                      << ", 发药时间: " << (current->dispenseTime.empty() ? "未发药" : current->dispenseTime)
                      << ", 创建时间: " << current->createTime
                      << ", 备注: " << (current->note.empty() ? "无" : current->note)
                      << std::endl;

            std::cout << "用药明细: " << std::endl;
            for (const auto &line : current->lines)
            {
                std::cout << "  - 药品ID: " << line.medicineID
                          << ", 药品名称: " << line.medicineName
                          << ", 数量: " << line.quantity
                          << ", 单价: " << line.unitPrice
                          << ", 用法备注: " << (line.note.empty() ? "无" : line.note)
                          << std::endl;
            }

            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "该科室暂无用药记录！" << std::endl;
    }
}
// 根据患者ID查看用药记录
void Admin::viewMedicationRecordsByPatient(MedicationRecord *&medRec, const std::string &department)
{
    std::string patientID = inputIDCheck("请输入患者ID: ");

    MedicationRecord *current = medRec;
    bool found = false;
    std::cout << "正在查找患者ID: " << patientID << " 的用药记录..." << std::endl;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->patientID == patientID && current->department == department)
        {
            std::string reviewStatusStr = medicationReviewStatusToString(current->reviewStatus);
            std::string statusStr = medicationStatusToString(current->status);
            std::cout << "用药记录ID: " << current->medRecordID
                      << ", 关联看诊记录ID: " << current->consultationID
                      << ", 患者ID: " << current->patientID
                      << ", 医生ID: " << current->doctorID
                      << ", 药师ID: " << (current->pharmacistID.empty() ? "无" : current->pharmacistID)
                      << ", 科室: " << current->department
                      << ", 审核状态: " << statusStr
                      << ", 总费用: " << current->totalCost
                      << ", 支付时间: " << (current->paymentTime.empty() ? "未支付" : current->paymentTime)
                      << ", 发药时间: " << (current->dispenseTime.empty() ? "未发药" : current->dispenseTime)
                      << ", 创建时间: " << current->createTime
                      << ", 备注: " << (current->note.empty() ? "无" : current->note)
                      << std::endl;

            std::cout << "用药明细: " << std::endl;
            for (const auto &line : current->lines)
            {
                std::cout << "  - 药品ID: " << line.medicineID
                          << ", 药品名称: " << line.medicineName
                          << ", 数量: " << line.quantity
                          << ", 单价: " << line.unitPrice
                          << ", 用法备注: " << (line.note.empty() ? "无" : line.note)
                          << std::endl;
            }

            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "该患者在指定科室暂无用药记录！" << std::endl;
    }
}
// 根据医生ID查看用药记录
void Admin::viewMedicationRecordsByDoctor(MedicationRecord *&medRec, const std::string &department)
{
    std::string doctorID = inputIDCheck("请输入医生ID: ");

    MedicationRecord *current = medRec;
    bool found = false;
    std::cout << "正在查找医生ID: " << doctorID << " 的用药记录..." << std::endl;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->doctorID == doctorID && current->department == department)
        {
            std::string reviewStatusStr = medicationReviewStatusToString(current->reviewStatus);
            std::string statusStr = medicationStatusToString(current->status);
            std::cout << "用药记录ID: " << current->medRecordID
                      << ", 关联看诊记录ID: " << current->consultationID
                      << ", 患者ID: " << current->patientID
                      << ", 医生ID: " << current->doctorID
                      << ", 药师ID: " << (current->pharmacistID.empty() ? "无" : current->pharmacistID)
                      << ", 科室: " << current->department
                      << ", 审核状态: " << statusStr
                      << ", 总费用: " << current->totalCost
                      << ", 支付时间: " << (current->paymentTime.empty() ? "未支付" : current->paymentTime)
                      << ", 发药时间: " << (current->dispenseTime.empty() ? "未发药" : current->dispenseTime)
                      << ", 创建时间: " << current->createTime
                      << ", 备注: " << (current->note.empty() ? "无" : current->note)
                      << std::endl;

            std::cout << "用药明细: " << std::endl;
            for (const auto &line : current->lines)
            {
                std::cout << "  - 药品ID: " << line.medicineID
                          << ", 药品名称: " << line.medicineName
                          << ", 数量: " << line.quantity
                          << ", 单价: " << line.unitPrice
                          << ", 用法备注: " << (line.note.empty() ? "无" : line.note)
                          << std::endl;
            }

            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "该医生在指定科室暂无用药记录！" << std::endl;
    }
}
// 根据药师ID查看用药记录
void Admin::viewMedicationRecordsByPharmacist(MedicationRecord *&medRec, const std::string &department)
{
    std::string pharmacistID = inputIDCheck("请输入药师ID: ");

    MedicationRecord *current = medRec;
    bool found = false;
    std::cout << "正在查找药师ID: " << pharmacistID << " 的用药记录..." << std::endl;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->pharmacistID == pharmacistID && current->department == department)
        {
            std::string reviewStatusStr = medicationReviewStatusToString(current->reviewStatus);
            std::string statusStr = medicationStatusToString(current->status);
            std::cout << "用药记录ID: " << current->medRecordID
                      << ", 关联看诊记录ID: " << current->consultationID
                      << ", 患者ID: " << current->patientID
                      << ", 医生ID: " << current->doctorID
                      << ", 药师ID: " << (current->pharmacistID.empty() ? "无" : current->pharmacistID)
                      << ", 科室: " << current->department
                      << ", 审核状态: " << statusStr
                      << ", 总费用: " << current->totalCost
                      << ", 支付时间: " << (current->paymentTime.empty() ? "未支付" : current->paymentTime)
                      << ", 发药时间: " << (current->dispenseTime.empty() ? "未发药" : current->dispenseTime)
                      << ", 创建时间: " << current->createTime
                      << ", 备注: " << (current->note.empty() ? "无" : current->note)
                      << std::endl;

            std::cout << "用药明细: " << std::endl;
            for (const auto &line : current->lines)
            {
                std::cout << "  - 药品ID: " << line.medicineID
                          << ", 药品名称: " << line.medicineName
                          << ", 数量: " << line.quantity
                          << ", 单价: " << line.unitPrice
                          << ", 用法备注: " << (line.note.empty() ? "无" : line.note)
                          << std::endl;
            }

            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "该药师在指定科室暂无用药记录！" << std::endl;
    }
}
// 根据用药记录ID查看用药记录
void Admin::viewMedicationRecordByID(MedicationRecord *&medRec, const std::string &department)
{
    std::string medRecordID = inputRecordIDCheck("请输入用药记录ID: ", {"mrd"});

    MedicationRecord *current = medRec;
    bool found = false;
    std::cout << "正在查找用药记录ID: " << medRecordID << " 的用药记录..." << std::endl;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->medRecordID == medRecordID && current->department == department)
        {
            std::string reviewStatusStr = medicationReviewStatusToString(current->reviewStatus);
            std::string statusStr = medicationStatusToString(current->status);
            std::cout << "用药记录ID: " << current->medRecordID
                      << ", 关联看诊记录ID: " << current->consultationID
                      << ", 患者ID: " << current->patientID
                      << ", 医生ID: " << current->doctorID
                      << ", 药师ID: " << (current->pharmacistID.empty() ? "无" : current->pharmacistID)
                      << ", 科室: " << current->department
                      << ", 审核状态: " << statusStr
                      << ", 总费用: " << current->totalCost
                      << ", 支付时间: " << (current->paymentTime.empty() ? "未支付" : current->paymentTime)
                      << ", 发药时间: " << (current->dispenseTime.empty() ? "未发药" : current->dispenseTime)
                      << ", 创建时间: " << current->createTime
                      << ", 备注: " << (current->note.empty() ? "无" : current->note)
                      << std::endl;

            std::cout << "用药明细: " << std::endl;
            for (const auto &line : current->lines)
            {
                std::cout << "  - 药品ID: " << line.medicineID
                          << ", 药品名称: " << line.medicineName
                          << ", 数量: " << line.quantity
                          << ", 单价: " << line.unitPrice
                          << ", 用法备注: " << (line.note.empty() ? "无" : line.note)
                          << std::endl;
            }

            found = true;
            break;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到ID为 " << medRecordID << " 的用药记录！" << std::endl;
    }
}
// 根据药品ID查看用药记录
void Admin::viewMedicationRecordsByMedicationID(MedicationRecord *&medRec, const std::string &department)
{
    std::string medicineID = inputRecordIDCheck("请输入药品ID: ", {"med"});

    MedicationRecord *current = medRec;
    bool found = false;
    std::cout << "正在查找药品ID: " << medicineID << " 的用药记录..." << std::endl;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->department == department)
        {
            for (const auto &line : current->lines)
            {
                if (line.medicineID == medicineID)
                {
                    std::string reviewStatusStr = medicationReviewStatusToString(current->reviewStatus);
                    std::string statusStr = medicationStatusToString(current->status);
                    std::cout << "用药记录ID: " << current->medRecordID
                              << ", 关联看诊记录ID: " << current->consultationID
                              << ", 患者ID: " << current->patientID
                              << ", 医生ID: " << current->doctorID
                              << ", 药师ID: " << (current->pharmacistID.empty() ? "无" : current->pharmacistID)
                              << ", 科室: " << current->department
                              << ", 审核状态: " << statusStr
                              << ", 总费用: " << current->totalCost
                              << ", 支付时间: " << (current->paymentTime.empty() ? "未支付" : current->paymentTime)
                              << ", 发药时间: " << (current->dispenseTime.empty() ? "未发药" : current->dispenseTime)
                              << ", 创建时间: " << current->createTime
                              << ", 备注: " << (current->note.empty() ? "无" : current->note)
                              << std::endl;

                    std::cout << "用药明细: " << std::endl;
                    for (const auto &line : current->lines)
                    {
                        if (line.medicineID == medicineID)
                        {
                            std::cout << "==>";
                        }
                        std::cout << "  - 药品ID: " << line.medicineID
                                  << ", 药品名称: " << line.medicineName
                                  << ", 数量: " << line.quantity
                                  << ", 单价: " << line.unitPrice
                                  << ", 用法备注: " << (line.note.empty() ? "无" : line.note)
                                  << std::endl;
                    }
                    found = true;
                    break;
                }
            }
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到药品ID为 " << medicineID << " 的用药记录！" << std::endl;
    }
}
// 根据看诊记录ID查看用药记录
void Admin::viewMedicationRecordsByConsultationID(MedicationRecord *&medRec, const std::string &department)
{
    std::string consultationID = inputRecordIDCheck("请输入看诊记录ID: ", {"con"});

    MedicationRecord *current = medRec;
    bool found = false;
    std::cout << "正在查找看诊记录ID: " << consultationID << " 的用药记录..." << std::endl;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->consultationID == consultationID && current->department == department)
        {
            std::string reviewStatusStr = medicationReviewStatusToString(current->reviewStatus);
            std::string statusStr = medicationStatusToString(current->status);
            std::cout << "用药记录ID: " << current->medRecordID
                      << ", 关联看诊记录ID: " << current->consultationID
                      << ", 患者ID: " << current->patientID
                      << ", 医生ID: " << current->doctorID
                      << ", 药师ID: " << (current->pharmacistID.empty() ? "无" : current->pharmacistID)
                      << ", 科室: " << current->department
                      << ", 审核状态: " << statusStr
                      << ", 总费用: " << current->totalCost
                      << ", 支付时间: " << (current->paymentTime.empty() ? "未支付" : current->paymentTime)
                      << ", 发药时间: " << (current->dispenseTime.empty() ? "未发药" : current->dispenseTime)
                      << ", 创建时间: " << current->createTime
                      << ", 备注: " << (current->note.empty() ? "无" : current->note)
                      << std::endl;

            std::cout << "用药明细: " << std::endl;
            for (const auto &line : current->lines)
            {
                std::cout << "  - 药品ID: " << line.medicineID
                          << ", 药品名称: " << line.medicineName
                          << ", 数量: " << line.quantity
                          << ", 单价: " << line.unitPrice
                          << ", 用法备注: " << (line.note.empty() ? "无" : line.note)
                          << std::endl;
            }

            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到看诊记录ID为 " << consultationID << " 的用药记录！" << std::endl;
    }
}
// 根据审核状态查看用药记录
void Admin::viewMedicationRecordsByReviewStatus(MedicationRecord *&medRec, const std::string &department)
{

    int reviewStatusFilter = MedicationRecordReviewResultMenu(); // 选择审核状态过滤条件

    bool found = false;

    MedicationReviewStatus filterStatus = static_cast<MedicationReviewStatus>(reviewStatusFilter);

    MedicationRecord *current = medRec;
    std::cout << "正在查找审核状态为: " << medicationReviewStatusToString(filterStatus) << " 的用药记录..." << std::endl;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->department == department && current->reviewStatus == filterStatus)
        {
            std::string reviewStatusStr = medicationReviewStatusToString(current->reviewStatus);
            std::string statusStr = medicationStatusToString(current->status);
            std::cout << "用药记录ID: " << current->medRecordID
                      << ", 关联看诊记录ID: " << current->consultationID
                      << ", 患者ID: " << current->patientID
                      << ", 医生ID: " << current->doctorID
                      << ", 药师ID: " << (current->pharmacistID.empty() ? "无" : current->pharmacistID)
                      << ", 科室: " << current->department
                      << ", 审核状态: " << statusStr
                      << ", 总费用: " << current->totalCost
                      << ", 支付时间: " << (current->paymentTime.empty() ? "未支付" : current->paymentTime)
                      << ", 发药时间: " << (current->dispenseTime.empty() ? "未发药" : current->dispenseTime)
                      << ", 创建时间: " << current->createTime
                      << ", 备注: " << (current->note.empty() ? "无" : current->note)
                      << std::endl;

            std::cout << "用药明细: " << std::endl;
            for (const auto &line : current->lines)
            {
                std::cout << "  - 药品ID: " << line.medicineID
                          << ", 药品名称: " << line.medicineName
                          << ", 数量: " << line.quantity
                          << ", 单价: " << line.unitPrice
                          << ", 用法备注: " << (line.note.empty() ? "无" : line.note)
                          << std::endl;
            }

            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到审核状态为 " << medicationReviewStatusToString(filterStatus) << " 的用药记录！" << std::endl;
    }
}
// 根据用药状态查看用药记录
void Admin::viewMedicationRecordsByStatus(MedicationRecord *&medRec, const std::string &department)
{
    int statusFilter = MedicationRecordStatusMenu(); // 选择用药状态过滤条件

    bool found = false;

    MedicationStatus filterStatus = static_cast<MedicationStatus>(statusFilter);

    MedicationRecord *current = medRec;
    std::cout << "正在查找用药状态为: " << medicationStatusToString(filterStatus) << " 的用药记录..." << std::endl;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->department == department && current->status == filterStatus)
        {
            std::string reviewStatusStr = medicationReviewStatusToString(current->reviewStatus);
            std::string statusStr = medicationStatusToString(current->status);
            std::cout << "用药记录ID: " << current->medRecordID
                      << ", 关联看诊记录ID: " << current->consultationID
                      << ", 患者ID: " << current->patientID
                      << ", 医生ID: " << current->doctorID
                      << ", 药师ID: " << (current->pharmacistID.empty() ? "无" : current->pharmacistID)
                      << ", 科室: " << current->department
                      << ", 审核状态: " << statusStr
                      << ", 总费用: " << current->totalCost
                      << ", 支付时间: " << (current->paymentTime.empty() ? "未支付" : current->paymentTime)
                      << ", 发药时间: " << (current->dispenseTime.empty() ? "未发药" : current->dispenseTime)
                      << ", 创建时间: " << current->createTime
                      << ", 备注: " << (current->note.empty() ? "无" : current->note)
                      << std::endl;

            std::cout << "用药明细: " << std::endl;
            for (const auto &line : current->lines)
            {
                std::cout << "  - 药品ID: " << line.medicineID
                          << ", 药品名称: " << line.medicineName
                          << ", 数量: " << line.quantity
                          << ", 单价: " << line.unitPrice
                          << ", 用法备注: " << (line.note.empty() ? "无" : line.note)
                          << std::endl;
            }

            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到用药状态为 " << medicationStatusToString(filterStatus) << " 的用药记录！" << std::endl;
    }
}
// 修改用药记录的审核状态
void Admin::modifyMedicationRecordReviewStatus(MedicationRecord *&medRec, const std::string &department)
{
    std::string medRecordID = inputRecordIDCheck("请输入要修改状态的用药记录ID: ", {"mrd"});

    bool found = false;

    MedicationRecord *current = medRec;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->medRecordID == medRecordID && current->department == department)
        {
            int newStatus = MedicationRecordReviewResultMenu(); // 选择新的审核状态
            current->status = static_cast<MedicationStatus>(newStatus);
            std::cout << "用药记录状态已更新！" << std::endl;
            found = true;
            break;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到指定的用药记录信息！" << std::endl;
    }
}
// 删除用药记录（逻辑删除，设置 isDeleted 标志）
void Admin::deleteMedicationRecord(MedicationRecord *&medRec, const std::string &department)
{
    std::string medRecordID = inputRecordIDCheck("请输入要删除的用药记录ID: ", {"mrd"});

    bool found = false;

    MedicationRecord *current = medRec;
    while (current != nullptr)
    {
        if (current->medRecordID == medRecordID && current->department == department)
        {
            current->isDeleted = true; // 逻辑删除
            std::cout << "用药记录ID: " << current->medRecordID << " 已删除！" << std::endl;
            found = true;
            break;
        }
        current = current->next;
    }
    if (!found)
    {
        std::cout << "未找到指定的用药记录信息！" << std::endl;
    }
}
// 添加用药记录（根据输入信息创建新的 MedicationRecord 对象，并插入到链表中）
void Admin::addMedicationRecord(MedicationRecord *&medRec, Consultation *con, Pharmacist *pha, Medicine *med, const std::string &department, int &idCounter)
{
    Consultation *currentCon = con;
    bool foundCon = false;

    std::cout << "当前科室可以关联的看诊记录列表(还未审核处方的看诊记录): " << std::endl;
    while (currentCon != nullptr)
    {
        if (!currentCon->isDeleted && !currentCon->prescriptions.empty() && !currentCon->isPrecriptionReviewed && currentCon->department == department)
        {
            std::cout << "看诊记录ID: " << currentCon->consultationID
                      << ", 患者ID: " << currentCon->patientID
                      << ", 医生ID: " << currentCon->doctorID
                      << ", 科室: " << currentCon->department
                      << ", 看诊时间: " << currentCon->consultationTime
                      << ", 备注: " << (currentCon->note.empty() ? "无" : currentCon->note)
                      << std::endl;
            foundCon = true;
        }
        currentCon = currentCon->next;
    }
    if (!foundCon)
    {
        std::cout << "未找到当前科室可关联的看诊记录！" << std::endl;
        return;
    }

    std::string consultationID = inputRecordIDCheck("请输入要关联的看诊记录ID: ", {"con"});

    currentCon = con;

    MedicationRecord *newMedRec = new MedicationRecord();

    foundCon = false;

    while (currentCon != nullptr)
    {
        if (!currentCon->isDeleted && currentCon->consultationID == consultationID && currentCon->department == department)
        {
            if (currentCon->prescriptions.empty())
            {
                std::cout << "选定的看诊记录没有处方信息，无法创建用药记录！" << std::endl;
                delete newMedRec; // 释放之前创建的 MedicationRecord 对象，避免内存泄漏
                return;
            }
            else if (currentCon->isPrecriptionReviewed)
            {
                std::cout << "选定的看诊记录的处方已经审核过了，无法再次创建用药记录！" << std::endl;
                delete newMedRec; // 释放之前创建的 MedicationRecord 对象，避免内存泄漏
                return;
            }

            Pharmacist *currentPha = pha;
            std::cout << "当前科室的药师列表: " << std::endl;

            while (currentPha != nullptr)
            {
                if (!currentPha->isDeleted && currentPha->department == department)
                {
                    std::cout << "药师ID: " << currentPha->getUserID() << ", 姓名: " << currentPha->getUsername() << std::endl;
                }
                currentPha = currentPha->next;
            }

            newMedRec->pharmacistID = inputIDCheck("请输入审核该处方的药师ID: ");

            currentPha = pha;
            bool foundPha = false;
            while (currentPha != nullptr)
            {
                if (!currentPha->isDeleted && currentPha->getUserID() == newMedRec->pharmacistID && currentPha->department == department)
                {
                    foundPha = true;
                    break;
                }
                currentPha = currentPha->next;
            }
            if (!foundPha)
            {
                std::cout << "未找到指定的药师信息！用药记录创建失败。" << std::endl;
                delete newMedRec; // 释放之前创建的 MedicationRecord 对象，避免内存泄漏
                return;
            }

            newMedRec->consultationID = consultationID;
            newMedRec->patientID = currentCon->patientID;
            newMedRec->doctorID = currentCon->doctorID;
            newMedRec->department = currentCon->department;

            std::cout << "看诊记录ID: " << currentCon->consultationID
                      << ", 患者ID: " << currentCon->patientID
                      << ", 医生ID: " << currentCon->doctorID
                      << ", 科室: " << currentCon->department
                      << ", 看诊时间: " << currentCon->consultationTime
                      << ", 备注: " << (currentCon->note.empty() ? "无" : currentCon->note)
                      << std::endl;

            std::cout << "诊断结论: " << (currentCon->preliminaryDiagnosis.empty() ? "无" : currentCon->preliminaryDiagnosis) << std::endl;

            std::cout << "开具的处方信息: " << std::endl;
            for (const auto &pres : currentCon->prescriptions)
            {
                std::cout << "  - 药品ID: " << pres.medicineID
                          << ", 药品名称: " << pres.name
                          << ", 数量: " << pres.quantity
                          << ", 用量: " << pres.dosage
                          << ", 频次: " << pres.frequency
                          << ", 疗程: " << pres.duration
                          << ", 用法备注: " << (pres.note.empty() ? "无" : pres.note)
                          << std::endl;
                Medicine *currentMed = med;
                bool foundMed = false;
                while (currentMed != nullptr)
                {
                    if (!currentMed->isDeleted && currentMed->medicineID == pres.medicineID && currentMed->department == department)
                    {
                        MedicationLine line;
                        line.medicineID = pres.medicineID;
                        line.medicineName = pres.name;
                        line.quantity = pres.quantity;
                        line.unitPrice = currentMed->salePrice;
                        line.note = "用量: " + pres.dosage + ", 频次: " + pres.frequency + ", 疗程: " + pres.duration + (pres.note.empty() ? "" : ", 备注: " + pres.note);

                        newMedRec->totalCost += line.quantity * line.unitPrice; // 计算总费用
                        newMedRec->lines.push_back(line);

                        foundMed = true;
                        break;
                    }
                    currentMed = currentMed->next;
                }
                if (!foundMed)
                {
                    std::cout << "科室库房中没有药品ID：" << pres.medicineID << " 药品名：" << pres.name << " 的药品信息！" << std::endl;
                    return;
                }
            }

            currentCon->isPrecriptionReviewed = true; // 标记该看诊记录的处方已经审核过了，避免重复创建用药记录

            int newStatus = MedicationRecordReviewResultMenu(); // 选择新的审核状态

            if (newStatus == 0)
            {
                std::cout << "用药记录创建已取消！" << std::endl;
                delete newMedRec; // 释放之前创建的 MedicationRecord 对象，避免内存泄漏
                return;
            }

            newMedRec->medRecordID = "mrd" + std::to_string(idCounter++).insert(0, 6 - std::to_string(idCounter).length(), '0');
            MyTime &t = MyTime::getInstance();
            newMedRec->createTime = t.getTime();

            newMedRec->status = static_cast<MedicationStatus>(newStatus);

            foundCon = true;
            break;
        }
        currentCon = currentCon->next;
    }
    if (!foundCon)
    {
        std::cout << "未找到指定的看诊记录！" << std::endl;
        delete newMedRec; // 释放之前创建的 MedicationRecord 对象，避免内存泄漏
        return;
    }

    // 插入到链表头部
    newMedRec->next = medRec;
    if (medRec != nullptr)
    {
        medRec->prev = newMedRec;
    }
    medRec = newMedRec;
    std::cout << "用药记录已添加！用药记录ID: " << newMedRec->medRecordID << std::endl;
}

void Admin::manageMedicationRecords(MedicationRecord *&medRec, Consultation *con, Pharmacist *pha, Medicine *med, const std::string &department, int &idCounter)
{
    while (true)
    {
        int choice = adminMedicationRecordManagementMenu();

        if (choice == 1) // 查看用药记录
        {
            while (true)
            {
                int viewChoice = adminMedicationRecordViewMenu();

                if (viewChoice == 1) // 查看所有用药记录
                {
                    viewAllMedicationRecords(medRec, department);
                    pause();
                }
                else if (viewChoice == 2) // 根据患者ID查看用药记录
                {
                    viewMedicationRecordsByPatient(medRec, department);
                    pause();
                }
                else if (viewChoice == 3) // 根据医生ID查看用药记录
                {
                    viewMedicationRecordsByDoctor(medRec, department);
                    pause();
                }
                else if (viewChoice == 4) // 根据药师ID查看用药记录
                {
                    viewMedicationRecordsByPharmacist(medRec, department);
                    pause();
                }
                else if (viewChoice == 5) // 根据用药记录ID查看用药记录
                {
                    viewMedicationRecordByID(medRec, department);
                    pause();
                }
                else if (viewChoice == 6) // 根据药品ID查看用药记录
                {
                    viewMedicationRecordsByMedicationID(medRec, department);
                    pause();
                }
                else if (viewChoice == 7) // 根据看诊记录ID查看用药记录
                {
                    viewMedicationRecordsByConsultationID(medRec, department);
                    pause();
                }
                else if (viewChoice == 8) // 根据审核状态查看用药记录
                {
                    viewMedicationRecordsByReviewStatus(medRec, department);
                    pause();
                }
                else if (viewChoice == 9) // 根据用药状态查看用药记录
                {
                    viewMedicationRecordsByStatus(medRec, department);
                    pause();
                }
            }
        }
        else if (choice == 2) // 修改用药记录状态
        {
            modifyMedicationRecordReviewStatus(medRec, department);
            pause();
        }
        else if (choice == 3) // 删除用药记录
        {
            deleteMedicationRecord(medRec, department);
            pause();
        }
        else if (choice == 4) // 添加用药记录
        {
            addMedicationRecord(medRec, con, pha, med, department, idCounter);
            pause();
        }
        else if (choice == 0) // 返回上一级菜单
        {
            break;
        }
    }
}

// ==================================== 药品管理 =================================

// 查看科室库房中所有药品信息
void Admin::viewAllMedicines(Medicine *&med, const std::string &department)
{
    Medicine *current = med;
    bool found = false;
    std::cout << "正在查找科室 " << department << " 库房中的药品信息..." << std::endl;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->department == department)
        {
            std::string statusStr = medicineStatusToString(current->status);
            std::cout << "药品ID: " << current->medicineID
                      << ", 药品名称: " << current->name
                      << ", 规格: " << current->specification
                      << ", 进价: " << current->purchasePrice
                      << ", 售价: " << current->salePrice
                      << ", 状态: " << statusStr
                      << ", 当前库存数量: " << current->stock
                      << ", 安全库存阈值: " << current->safetyStock
                      << ", 生产日期: " << current->productionDate
                      << ", 有效期至: " << current->expiryDate
                      << ", 生产厂家: " << current->manufacturer
                      << ", 备注: " << (current->note.empty() ? "无" : current->note)
                      << std::endl;
            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "该科室库房暂无药品信息！" << std::endl;
    }
}
// 根据药品ID查看药品信息
void Admin::viewMedicineByID(Medicine *&med, const std::string &department)
{
    std::string medicineID = inputRecordIDCheck("请输入药品ID: ", {"med"});

    Medicine *current = med;
    bool found = false;
    std::cout << "正在查找药品ID: " << medicineID << " 的药品信息..." << std::endl;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->medicineID == medicineID && current->department == department)
        {
            std::string statusStr = medicineStatusToString(current->status);
            std::cout << "药品ID: " << current->medicineID
                      << ", 药品名称: " << current->name
                      << ", 规格: " << current->specification
                      << ", 进价: " << current->purchasePrice
                      << ", 售价: " << current->salePrice
                      << ", 状态: " << statusStr
                      << ", 当前库存数量: " << current->stock
                      << ", 安全库存阈值: " << current->safetyStock
                      << ", 生产日期: " << current->productionDate
                      << ", 有效期至: " << current->expiryDate
                      << ", 生产厂家: " << current->manufacturer
                      << ", 备注: " << (current->note.empty() ? "无" : current->note)
                      << std::endl;
            found = true;
            break;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到ID为 " << medicineID << " 的药品信息！" << std::endl;
    }
}
// 根据药品名称查看药品信息
void Admin::viewMedicinesByName(Medicine *&med, const std::string &department)
{
    std::string name = inputStringCheck("请输入药品名称（支持模糊查询）: ");

    Medicine *current = med;
    bool found = false;
    std::cout << "正在查找药品名称包含 \"" << name << "\" 的药品信息..." << std::endl;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->name.find(name) != std::string::npos && current->department == department)
        {
            std::string statusStr = medicineStatusToString(current->status);
            std::cout << "药品ID: " << current->medicineID
                      << ", 药品名称: " << current->name
                      << ", 规格: " << current->specification
                      << ", 进价: " << current->purchasePrice
                      << ", 售价: " << current->salePrice
                      << ", 状态: " << statusStr
                      << ", 当前库存数量: " << current->stock
                      << ", 安全库存阈值: " << current->safetyStock
                      << ", 生产日期: " << current->productionDate
                      << ", 有效期至: " << current->expiryDate
                      << ", 生产厂家: " << current->manufacturer
                      << ", 备注: " << (current->note.empty() ? "无" : current->note)
                      << std::endl;
            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到名称包含 \"" << name << "\" 的药品信息！" << std::endl;
    }
}
// 根据药品状态查看药品信息
void Admin::viewMedicinesByStatus(Medicine *&med, const std::string &department)
{
    int statusFilter = MedicineStatusMenu(); // 选择药品状态过滤条件

    bool found = false;

    MedicineStatus filterStatus = static_cast<MedicineStatus>(statusFilter);

    Medicine *current = med;
    std::cout << "正在查找药品状态为: " << medicineStatusToString(filterStatus) << " 的药品信息..." << std::endl;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->department == department && current->status == filterStatus)
        {
            std::string statusStr = medicineStatusToString(current->status);
            std::cout << "药品ID: " << current->medicineID
                      << ", 药品名称: " << current->name
                      << ", 规格: " << current->specification
                      << ", 进价: " << current->purchasePrice
                      << ", 售价: " << current->salePrice
                      << ", 状态: " << statusStr
                      << ", 当前库存数量: " << current->stock
                      << ", 安全库存阈值: " << current->safetyStock
                      << ", 生产日期: " << current->productionDate
                      << ", 有效期至: " << current->expiryDate
                      << ", 生产厂家: " << current->manufacturer
                      << ", 备注: " << (current->note.empty() ? "无" : current->note)
                      << std::endl;
            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到药品状态为 " << medicineStatusToString(filterStatus) << " 的药品信息！" << std::endl;
    }
}
// 根据生产厂家查看药品信息
void Admin::viewMedicinesByManufacturer(Medicine *&med, const std::string &department)
{
    std::string manufacturer = inputStringCheck("请输入生产厂家（支持模糊查询）: ");

    Medicine *current = med;
    bool found = false;
    std::cout << "正在查找生产厂家包含 \"" << manufacturer << "\" 的药品信息..." << std::endl;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->manufacturer.find(manufacturer) != std::string::npos && current->department == department)
        {
            std::string statusStr = medicineStatusToString(current->status);
            std::cout << "药品ID: " << current->medicineID
                      << ", 药品名称: " << current->name
                      << ", 规格: " << current->specification
                      << ", 进价: " << current->purchasePrice
                      << ", 售价: " << current->salePrice
                      << ", 状态: " << statusStr
                      << ", 当前库存数量: " << current->stock
                      << ", 安全库存阈值: " << current->safetyStock
                      << ", 生产日期: " << current->productionDate
                      << ", 有效期至: " << current->expiryDate
                      << ", 生产厂家: " << current->manufacturer
                      << ", 备注: " << (current->note.empty() ? "无" : current->note)
                      << std::endl;
            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到生产厂家包含 \"" << manufacturer << "\" 的药品信息！" << std::endl;
    }
}
// 查看低于安全库存阈值的药品信息
void Admin::viewMedicinesBySafetyStock(Medicine *&med, const std::string &department)
{
    Medicine *current = med;
    bool found = false;
    std::cout << "正在查找当前库存数量低于安全库存阈值的药品信息..." << std::endl;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->department == department && current->stock < current->safetyStock)
        {
            std::string statusStr = medicineStatusToString(current->status);
            std::cout << "药品ID: " << current->medicineID
                      << ", 药品名称: " << current->name
                      << ", 规格: " << current->specification
                      << ", 进价: " << current->purchasePrice
                      << ", 售价: " << current->salePrice
                      << ", 状态: " << statusStr
                      << ", 当前库存数量: " << current->stock
                      << ", 安全库存阈值: " << current->safetyStock
                      << ", 生产日期: " << current->productionDate
                      << ", 有效期至: " << current->expiryDate
                      << ", 生产厂家: " << current->manufacturer
                      << ", 备注: " << (current->note.empty() ? "无" : current->note)
                      << std::endl;
            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "当前没有药品的库存数量低于安全库存阈值！" << std::endl;
    }
}
// 修改药品状态
void Admin::modifyMedicineStatus(Medicine *&med, const std::string &department)
{
    std::string medicineID = inputRecordIDCheck("请输入要修改状态的药品ID: ", {"med"});
    Medicine *current = med;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->medicineID == medicineID && current->department == department)
        {
            std::cout << "当前药品状态: " << medicineStatusToString(current->status) << std::endl;
            int newStatus = MedicineStatusMenu(); // 选择新的药品状态
            current->status = static_cast<MedicineStatus>(newStatus);
            std::cout << "药品状态已更新！" << std::endl;
            return;
        }
        current = current->next;
    }
    std::cout << "未找到指定的药品！" << std::endl;
}
// 修改药品名称
void Admin::modifyMedicineName(Medicine *&med, const std::string &department)
{
    std::string medicineID = inputRecordIDCheck("请输入要修改名称的药品ID: ", {"med"});
    Medicine *current = med;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->medicineID == medicineID && current->department == department)
        {
            std::cout << "当前药品名称: " << current->name << std::endl;
            std::string newName = inputStringCheck("请输入新的药品名称: ");
            current->name = newName;
            std::cout << "药品名称已更新！" << std::endl;
            return;
        }
        current = current->next;
    }
    std::cout << "未找到指定的药品！" << std::endl;
}
// 修改药品规格
void Admin::modifyMedicineSpecification(Medicine *&med, const std::string &department)
{
    std::string medicineID = inputRecordIDCheck("请输入要修改规格的药品ID: ", {"med"});
    Medicine *current = med;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->medicineID == medicineID && current->department == department)
        {
            std::cout << "当前药品规格: " << current->specification << std::endl;
            std::string newSpecification = inputStringCheck("请输入新的药品规格: ");
            current->specification = newSpecification;
            std::cout << "药品规格已更新！" << std::endl;
            return;
        }
        current = current->next;
    }
    std::cout << "未找到指定的药品！" << std::endl;
}
// 修改药品进价
void Admin::modifyMedicinePurchasePrice(Medicine *&med, const std::string &department)
{
    std::string medicineID = inputRecordIDCheck("请输入要修改进价的药品ID: ", {"med"});
    Medicine *current = med;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->medicineID == medicineID && current->department == department)
        {
            std::cout << "当前药品进价: " << current->purchasePrice << std::endl;
            double newPurchasePrice = inputFeeCheck("请输入新的药品进价: ");
            current->purchasePrice = newPurchasePrice;
            std::cout << "药品进价已更新！" << std::endl;
            return;
        }
        current = current->next;
    }
    std::cout << "未找到指定的药品！" << std::endl;
}
// 修改药品售价
void Admin::modifyMedicineSalePrice(Medicine *&med, const std::string &department)
{
    std::string medicineID = inputRecordIDCheck("请输入要修改售价的药品ID: ", {"med"});
    Medicine *current = med;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->medicineID == medicineID && current->department == department)
        {
            std::cout << "当前药品售价: " << current->salePrice << std::endl;
            double newSalePrice = inputFeeCheck("请输入新的药品售价: ");
            current->salePrice = newSalePrice;
            std::cout << "药品售价已更新！" << std::endl;
            return;
        }
        current = current->next;
    }
    std::cout << "未找到指定的药品！" << std::endl;
}
// 修改药品库存数量
void Admin::modifyMedicineStock(Medicine *&med, const std::string &department)
{
    std::string medicineID = inputRecordIDCheck("请输入要修改库存数量的药品ID: ", {"med"});
    Medicine *current = med;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->medicineID == medicineID && current->department == department)
        {
            std::cout << "当前库存数量: " << current->stock << std::endl;
            std::cout << "输入新的库存数量 ";
            int newStock = selectIntCheck(0, INT_MAX);
            current->stock = newStock;
            std::cout << "药品库存数量已更新！" << std::endl;
            return;
        }
        current = current->next;
    }
    std::cout << "未找到指定的药品！" << std::endl;
}
// 修改药品安全库存阈值
void Admin::modifyMedicineSafetyStock(Medicine *&med, const std::string &department)
{
    std::string medicineID = inputRecordIDCheck("请输入要修改安全库存阈值的药品ID: ", {"med"});
    Medicine *current = med;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->medicineID == medicineID && current->department == department)
        {
            std::cout << "当前安全库存阈值: " << current->safetyStock << std::endl;
            std::cout << "输入新的安全库存阈值 ";
            int newSafetyStock = selectIntCheck(1, INT_MAX);
            current->safetyStock = newSafetyStock;
            std::cout << "药品安全库存阈值已更新！" << std::endl;
            return;
        }
        current = current->next;
    }
    std::cout << "未找到指定的药品！" << std::endl;
}
// 修改药品生产日期
void Admin::modifyMedicineProductionDate(Medicine *&med, const std::string &department)
{
    std::string medicineID = inputRecordIDCheck("请输入要修改生产日期的药品ID: ", {"med"});
    Medicine *current = med;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->medicineID == medicineID && current->department == department)
        {
            std::cout << "当前药品生产日期: " << current->productionDate << std::endl;
            std::string newProductionDate = inputDateCheck("请输入新的药品生产日期: ");
            current->productionDate = newProductionDate;
            std::cout << "药品生产日期已更新！" << std::endl;
            return;
        }
        current = current->next;
    }
    std::cout << "未找到指定的药品！" << std::endl;
}
// 修改药品有效期
void Admin::modifyMedicineExpiryDate(Medicine *&med, const std::string &department)
{
    std::string medicineID = inputRecordIDCheck("请输入要修改有效期的药品ID: ", {"med"});
    Medicine *current = med;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->medicineID == medicineID && current->department == department)
        {
            std::cout << "当前药品有效期: " << current->expiryDate << std::endl;
            std::string newExpiryDate = inputDateCheck("请输入新的药品有效期限: ");
            current->expiryDate = newExpiryDate;
            std::cout << "药品有效期限已更新！" << std::endl;
            return;
        }
        current = current->next;
    }
    std::cout << "未找到指定的药品！" << std::endl;
}
// 修改药品生产厂家
void Admin::modifyMedicineManufacturer(Medicine *&med, const std::string &department)
{
    std::string medicineID = inputRecordIDCheck("请输入要修改生产厂家的药品ID: ", {"med"});
    Medicine *current = med;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->medicineID == medicineID && current->department == department)
        {
            std::cout << "当前药品生产厂家: " << current->manufacturer << std::endl;
            std::string newManufacturer = inputStringCheck("请输入新的药品生产厂家: ");
            current->manufacturer = newManufacturer;
            std::cout << "药品生产厂家已更新！" << std::endl;
            return;
        }
        current = current->next;
    }
    std::cout << "未找到指定的药品！" << std::endl;
}
// 修改药品备注信息
void Admin::modifyMedicineNote(Medicine *&med, const std::string &department)
{
    std::string medicineID = inputRecordIDCheck("请输入要修改备注信息的药品ID: ", {"med"});
    Medicine *current = med;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->medicineID == medicineID && current->department == department)
        {
            std::cout << "当前药品备注信息: " << current->note << std::endl;

            std::string newNote = inputStringCheck("请输入新的药品备注信息: ");
            current->note = newNote;
            std::cout << "药品备注信息已更新！" << std::endl;
            return;
        }
        current = current->next;
    }
    std::cout << "未找到指定的药品！" << std::endl;
}
// 删除药品（逻辑删除，设置 isDeleted 标志）
void Admin::deleteMedicine(Medicine *&med, const std::string &department)
{
    std::string medicineID = inputRecordIDCheck("请输入要删除的药品ID: ", {"med"});
    Medicine *current = med;
    while (current != nullptr)
    {
        if (current->medicineID == medicineID && current->department == department)
        {
            current->isDeleted = true; // 逻辑删除
            std::cout << "药品ID: " << current->medicineID << " 已删除！" << std::endl;
            return;
        }
        current = current->next;
    }
    std::cout << "未找到指定的药品！" << std::endl;
}
// 添加药品（根据输入信息创建新的 Medicine 对象，并插入到链表中）
void Admin::addMedicine(Medicine *&med, const std::string &department, int &idCounter)
{
    Medicine *newMed = new Medicine();

    newMed->department = department;

    newMed->name = inputStringCheck("请输入药品名称: ");

    newMed->specification = inputStringCheck("请输入药品规格: ");

    newMed->purchasePrice = inputFeeCheck("请输入药品进价: ");

    newMed->salePrice = inputFeeCheck("请输入药品售价: ");

    std::cout << "填写药品初始库存数量 ";
    newMed->stock = selectIntCheck(0, INT_MAX);

    std::cout << "填写药品安全库存阈值 ";
    newMed->safetyStock = selectIntCheck(1, INT_MAX);

    newMed->productionDate = inputDateCheck("请输入药品生产日期: ");

    newMed->expiryDate = inputDateCheck("请输入药品有效期限: ");

    newMed->manufacturer = inputStringCheck("请输入药品生产厂家: ");

    newMed->note = inputStringCheck("请输入药品备注信息（可选）: ");

    newMed->medicineID = "med" + std::to_string(idCounter++).insert(0, 6 - std::to_string(idCounter).length(), '0');

    // 插入到链表头部
    newMed->next = med;
    if (med != nullptr)
    {
        med->prev = newMed;
    }
    med = newMed;
    std::cout << "药品已添加！药品ID: " << newMed->medicineID << std::endl;
}

void Admin::manageMedicines(Medicine *&med, const std::string &department, int &idCounter)
{
    while (true)
    {
        int choice = adminMedicineManagementMenu();

        if (choice == 1) // 查看药品信息
        {
            while (true)
            {
                int viewChoice = adminMedicineViewMenu();
                if (viewChoice == 0) // 返回上一级菜单
                {
                    break;
                }
                else if (viewChoice == 1) // 查看所有药品信息
                {
                    viewAllMedicines(med, department);
                    pause();
                }
                else if (viewChoice == 2) // 根据药品ID查看药品信息
                {
                    viewMedicineByID(med, department);
                    pause();
                }
                else if (viewChoice == 3) // 根据药品名称查看药品信息
                {
                    viewMedicinesByName(med, department);
                    pause();
                }
                else if (viewChoice == 4) // 根据药品状态查看药品信息
                {
                    viewMedicinesByStatus(med, department);
                    pause();
                }
                else if (viewChoice == 5) // 根据生产厂家查看药品信息
                {
                    viewMedicinesByManufacturer(med, department);
                    pause();
                }
                else if (viewChoice == 6) // 查看低于安全库存阈值的药品信息
                {
                    viewMedicinesBySafetyStock(med, department);
                    pause();
                }
            }
        }
        else if (choice == 2) // 修改药品信息
        {
            while (true)
            {
                int modifyChoice = MedicineModificationMenu();

                if (modifyChoice == 0) // 返回上一级菜单
                {
                    break;
                }
                else if (modifyChoice == 1) // 修改药品状态
                {
                    modifyMedicineStatus(med, department);
                    pause();
                }
                else if (modifyChoice == 2) // 修改药品名称
                {
                    modifyMedicineName(med, department);
                    pause();
                }
                else if (modifyChoice == 3) // 修改药品规格
                {
                    modifyMedicineSpecification(med, department);
                    pause();
                }
                else if (modifyChoice == 4) // 修改药品进价
                {
                    modifyMedicinePurchasePrice(med, department);
                    pause();
                }
                else if (modifyChoice == 5) // 修改药品售价
                {
                    modifyMedicineSalePrice(med, department);
                    pause();
                }
                else if (modifyChoice == 6) // 修改药品库存数量
                {
                    modifyMedicineStock(med, department);
                    pause();
                }
                else if (modifyChoice == 7) // 修改药品安全库存阈值
                {
                    modifyMedicineSafetyStock(med, department);
                    pause();
                }
                else if (modifyChoice == 8) // 修改药品生产日期
                {
                    modifyMedicineProductionDate(med, department);
                    pause();
                }
                else if (modifyChoice == 9) // 修改药品有效期
                {
                    modifyMedicineExpiryDate(med, department);
                    pause();
                }
                else if (modifyChoice == 10) // 修改药品生产厂家
                {
                    modifyMedicineManufacturer(med, department);
                    pause();
                }
                else if (modifyChoice == 11) // 修改药品备注信息
                {
                    modifyMedicineNote(med, department);
                    pause();
                }
            }
        }
        else if (choice == 3) // 删除药品
        {
            deleteMedicine(med, department);
            pause();
        }
        else if (choice == 4) // 添加药品
        {
            addMedicine(med, department, idCounter);
            pause();
        }
        else if (choice == 0) // 返回上一级菜单
        {
            break;
        }
    }
}