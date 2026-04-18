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

bool Admin::adminSignUp(int &idCounter)
{
    bool flag = signUp(1, idCounter); // 1 - Admin
    if (flag)
    {
        std::cout << "管理员注册成功! 您的用户ID是: " << getUserID() << std::endl;
        return true;
    }
    else
    {
        std::cout << "管理员注册失败！" << std::endl;
        return false;
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
bool Admin::viewAllRegistrations(Registration *&reg, const std::string &department)
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
    return foundAny;
}
// 查看指定医生的挂号记录
bool Admin::viewRegistrationsByDoctor(Registration *&reg, const std::string &department)
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
    return found;
}
// 查看指定患者的挂号记录
bool Admin::viewRegistrationsByPatient(Registration *&reg, const std::string &department)
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
    return found;
}
// 查看指定状态的挂号记录
bool Admin::viewRegistrationsByStatus(Registration *&reg, const std::string &department)
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
    return found;
}
// 修改挂号记录状态（如支付、取消等）
void Admin::modifyRegistrationStatus(Registration *&target, const std::string &department)
{
    std::cout << "请输入新的挂号状态 (0 - 已预约, 1 - 已支付, 2 - 已取消, 3 - 已完成): ";
    int newStatus = selectIntCheck(0, 3);
    target->status = static_cast<RegistrationStatus>(newStatus);
    std::cout << "挂号记录状态已更新！" << std::endl;
}
// 删除挂号记录（逻辑删除，设置 isDeleted 标志）
void Admin::deleteRegistration(Registration *&target, const std::string &department)
{
    target->isDeleted = true; // 逻辑删除
    std::cout << "挂号记录已删除！" << std::endl;
}
// 添加挂号记录（根据输入信息创建新的 Registration 对象，并插入到链表中）
bool Admin::addRegistration(Registration *&reg, Doctor *&doc, const std::string &department, int &idCounter)
{
    Registration *newReg = new Registration();

    newReg->patientID = inputIDCheck("请输入患者ID: ");
    newReg->department = department;

    std::cout << "可用医生列表:" << std::endl;
    Doctor *currentDoc = doc;
    while (currentDoc != nullptr)
    {
        if (!currentDoc->isDeleted && currentDoc->department == department && currentDoc->isOnDuty) // 只显示在岗的医生
        {
            std::cout << "医生ID: " << currentDoc->getUserID() << ", 姓名: " << currentDoc->getUsername() << ", 擅长领域：" << currentDoc->specialty << ", 职称：" << doctorTitleToString(currentDoc->title) << ", 联系方式：" << currentDoc->getTelephone() << " / " << currentDoc->getEmail()
                      << std::endl;
        }
        currentDoc = currentDoc->next;
    }

    if (doc == nullptr)
    {
        std::cout << "该科室暂无医生可选！" << std::endl;
        delete newReg;
        return false;
    }

    std::string newDoctorID = inputIDCheck("请输入医生ID: ");

    currentDoc = doc;
    bool doctorExists = false;
    while (currentDoc != nullptr)
    {
        if (!currentDoc->isDeleted && currentDoc->getUserID() == newDoctorID && currentDoc->department == department)
        {
            doctorExists = true;
            newReg->doctorID = newDoctorID; // 更新医生ID
            break;
        }
        currentDoc = currentDoc->next;
    }

    if (!doctorExists)
    {
        std::cout << "未找到指定的医生！" << std::endl;
        delete newReg;
        return false;
    }

    newReg->fee = calculateRegistrationFee(currentDoc->title);

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
    return true;
}
// 根据挂号信息ID查看挂号记录
bool Admin::viewRegistrationsByID(Registration *&reg, const std::string &department)
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
    return found;
}
// 根据挂号记录修改医生信息（如重新分配医生等）
void Admin::modifyRegistrationDoctor(Registration *&target, Doctor *&doc, const std::string &department)
{
    std::cout << "可用医生列表:" << std::endl;
    Doctor *currentDoc = doc;
    while (currentDoc != nullptr)
    {
        if (!currentDoc->isDeleted && currentDoc->department == department && currentDoc->isOnDuty)
        {
            std::cout << "医生ID: " << currentDoc->getUserID() << ", 姓名: " << currentDoc->getUsername() << ", 擅长领域：" << currentDoc->specialty << ", 职称：" << doctorTitleToString(currentDoc->title) << ", 联系方式：" << currentDoc->getTelephone() << " / " << currentDoc->getEmail()
                      << std::endl;
        }
        currentDoc = currentDoc->next;
    }

    if (doc == nullptr)
    {
        std::cout << "该科室暂无医生可选！" << std::endl;
        return;
    }

    std::string newDoctorID = inputIDCheck("请输入新的医生ID: ");

    currentDoc = doc;
    bool doctorExists = false;
    while (currentDoc != nullptr)
    {
        if (currentDoc->getUserID() == newDoctorID && currentDoc->department == department)
        {
            if (currentDoc->isDeleted)
            {
                std::cout << "该医生已被删除，无法分配！" << std::endl;
                return;
            }
            else if (!currentDoc->isOnDuty)
            {
                std::cout << "该医生当前不在岗，无法分配！" << std::endl;
                return;
            }

            doctorExists = true;
            break;
        }
        currentDoc = currentDoc->next;
    }

    if (!doctorExists)
    {
        std::cout << "未找到指定的医生！" << std::endl;
        return;
    }

    target->doctorID = newDoctorID; // 更新医生ID
    std::cout << "挂号记录的医生信息已更新！" << std::endl;
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
            bool ishave = viewAllRegistrations(reg, department);
            if (ishave)
            {
                std::string regID = inputRecordIDCheck("请输入要修改的挂号记录ID: ", {"reg"});
                Registration *target = reg;
                while (target != nullptr)
                {
                    if (!target->isDeleted && target->department == department && target->registrationID == regID)
                    {
                        break;
                    }
                    target = target->next;
                }

                if (target)
                {
                    while (true)
                    {
                        std::cout << "当前挂号记录信息如下：" << std::endl;

                        std::string statusStr = regStatusToString(target->status);
                        std::cout << "ID: " << target->registrationID
                                  << ", 患者ID: " << target->patientID
                                  << ", 医生ID: " << target->doctorID
                                  << ", 时间: " << target->registerTime
                                  << ", 费用: " << target->fee
                                  << ", 状态: " << statusStr
                                  << ", 备注: " << target->note
                                  << std::endl;

                        std::cout << "请选择要修改的内容: " << std::endl;
                        std::cout << "1 - 看诊医生ID" << std::endl;
                        std::cout << "2 - 挂号状态" << std::endl;
                        std::cout << "0 - 返回上级菜单" << std::endl;

                        int modifyChoice = selectIntCheck(0, 2);
                        if (modifyChoice == 0)
                            break;
                        else if (modifyChoice == 1)
                        {
                            modifyRegistrationDoctor(target, doc, department);
                            pause();
                        }
                        else if (modifyChoice == 2)
                        {
                            modifyRegistrationStatus(target, department);
                            pause();
                        }
                        else
                        {
                            std::cout << "无效的选择! 请重新选择。" << std::endl;
                            pause();
                        }
                    }
                }
                else
                {
                    std::cout << "未找到指定的挂号记录！" << std::endl;
                    pause();
                }
            }
        }
        else if (choice == 3)
        {
            bool ishave = viewAllRegistrations(reg, department);
            if (ishave)
            {
                std::string regID = inputRecordIDCheck("请输入要删除的挂号记录ID: ", {"reg"});
                Registration *target = reg;
                while (target != nullptr)
                {
                    if (!target->isDeleted && target->department == department && target->registrationID == regID)
                    {
                        break;
                    }
                    target = target->next;
                }

                if (target)
                {
                    std::cout << "正在删除的挂号记录信息如下：" << std::endl;

                    std::string statusStr = regStatusToString(target->status);
                    std::cout << "ID: " << target->registrationID
                              << ", 患者ID: " << target->patientID
                              << ", 医生ID: " << target->doctorID
                              << ", 时间: " << target->registerTime
                              << ", 费用: " << target->fee
                              << ", 状态: " << statusStr
                              << ", 备注: " << target->note
                              << std::endl;

                    std::cout << "确认要删除这条挂号记录吗？: " << std::endl;
                    std::cout << "1. 确认" << std::endl;
                    std::cout << "0. 取消" << std::endl;

                    int confirmChoice = selectIntCheck(0, 1);

                    if (confirmChoice == 1)
                    {
                        deleteRegistration(target, department);
                        pause();
                    }
                    else
                    {
                        std::cout << "已取消删除操作。" << std::endl;
                        pause();
                    }
                }
                else
                {
                    std::cout << "未找到指定的挂号记录！" << std::endl;
                    pause();
                }
            }
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
bool Admin::viewAllConsultations(Consultation *&con, const std::string &department)
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
        return false;
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
    return true;
}
// 查看指定医生的看诊记录
bool Admin::viewConsultationsByDoctor(Consultation *&con, const std::string &department)
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
        return false;
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
    return true;
}
// 查看指定患者的看诊记录
bool Admin::viewConsultationsByPatient(Consultation *&con, const std::string &department)
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
        return false;
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
    return true;
}
// 查看指定状态的看诊记录
bool Admin::viewConsultationsByStatus(Consultation *&con, const std::string &department)
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
        return false;
    }
    return true;
}
// 查看指定挂号ID的看诊记录
bool Admin::viewConsultationByRegistrationID(Consultation *&con, const std::string &department)
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
        return false;
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
    return true;
}
// 修改看诊记录的状态
void Admin::modifyConsultationStatus(Consultation *&target, const std::string &department)
{
    std::cout << "请输入新的看诊状态 (0 - 待就诊, 1 - 正在处理, 2 - 已结束, 3 - 已作废 ): ";
    int newStatus = selectIntCheck(0, 3);
    target->status = static_cast<ConsultationStatus>(newStatus);
    std::cout << "看诊记录状态已更新！" << std::endl;
}
// 删除看诊记录（逻辑删除，设置 isDeleted 标志）
void Admin::deleteConsultation(Consultation *&target, const std::string &department)
{

    target->isDeleted = true; // 逻辑删除
    std::cout << "ID: " << target->consultationID << " " << "看诊记录已删除！" << std::endl;
}
// 添加看诊记录（根据输入信息创建新的 Consultation 对象，并插入到链表中）
bool Admin::addConsultation(Consultation *&con, const std::string &department, Registration *reg, int &conCounter)
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
                return false;
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
        return false;
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
    return true;
}
// 根据看诊记录ID查看看诊记录
bool Admin::viewConsultationByID(Consultation *&con, const std::string &department)
{
    std::string conID = inputRecordIDCheck("请输入要查看的看诊记录ID: ", {"con"}); // 输入看诊记录ID并检查格式

    bool found = false;

    Consultation *current = con;
    std::cout << "正在查找看诊记录ID: " << conID << " 的看诊记录..." << std::endl;

    std::vector<Consultation *> temp; // 用于存储正在处理状态的看诊记录，方便后续详细信息展示
    while (current != nullptr)
    {
        if (!current->isDeleted && current->consultationID == conID && current->department == department)
        {
            std::string statusStr = conStatusToString(current->status);
            if (statusStr == "正在处理")
            {
                temp.push_back(current);
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
        return false;
    }

    std::cout << "正在处理的看诊记录详细信息:" << std::endl;
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
        std::cout << "  计划用药" << (c->isPrecriptionReviewed ? "（已审核）" : "（未审核）") << ": " << std::endl;
        for (const auto &med : c->prescriptions)
        {
            std::cout << "药品ID: " << med.medicineID << ", 名称: " << med.name << ", 用量: " << med.dosage
                      << ", 频次: " << med.frequency << ", 疗程: " << med.duration
                      << ", 备注: " << med.note << std::endl;
        }

        std::cout << "是否建议住院: " << (c->isHospitalizationRecommended ? "是" : "否") << std::endl;

        std::cout << "  医生备注: " << c->note << std::endl;
    }
    return true;
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
            bool ishave = viewAllConsultations(con, department);

            if (ishave)
            {
                std::string conID = inputRecordIDCheck("请输入要修改状态的看诊记录ID: ", {"con"});
                Consultation *target = con;
                while (target != nullptr)
                {
                    if (!target->isDeleted && target->consultationID == conID && target->department == department)
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
                        std::string statusStr = conStatusToString(target->status);

                        if (statusStr == "正在处理")
                        {
                            std::cout << "ID: " << target->consultationID
                                      << ", 患者ID: " << target->patientID
                                      << ", 医生ID: " << target->doctorID
                                      << ", 时间: " << target->consultationTime
                                      << ", 科室: " << target->department
                                      << ", 状态: " << statusStr
                                      << std::endl;

                            std::cout << "  主诉: " << target->chiefComplaint << std::endl;
                            std::cout << "  现病史: " << target->historyOfPresentIllness << std::endl;
                            std::cout << "  既往史: " << target->pastMedicalHistory << std::endl;
                            std::cout << "  家族史: " << target->familyHistory << std::endl;

                            std::cout << "  初步诊断: " << target->preliminaryDiagnosis << std::endl;
                            std::cout << "  计划检查项目: ";
                            for (const auto &exam : target->examinationlist)
                            {
                                std::cout << exam << " ";
                            }
                            std::cout << std::endl;
                            std::cout << "  计划用药" << (target->isPrecriptionReviewed ? "（已审核）" : "（未审核）") << ": " << std::endl;
                            for (const auto &med : target->prescriptions)
                            {
                                std::cout << "药品ID: " << med.medicineID << ", 名称: " << med.name << ", 用量: " << med.dosage
                                          << ", 频次: " << med.frequency << ", 疗程: " << med.duration
                                          << ", 备注: " << med.note << std::endl;
                            }

                            std::cout << "是否建议住院: " << (target->isHospitalizationRecommended ? "是" : "否") << std::endl;

                            std::cout << "  医生备注: " << target->note << std::endl;
                        }
                        else
                        {
                            std::cout << "ID: " << target->consultationID
                                      << ", 患者ID: " << target->patientID
                                      << ", 医生ID: " << target->doctorID
                                      << ", 时间: " << target->consultationTime
                                      << ", 科室: " << target->department
                                      << ", 状态: " << statusStr
                                      << ", 备注: " << target->note
                                      << std::endl;
                        }

                        std::cout << "请选择你的操作：" << std::endl;
                        std::cout << "1. 修改看诊状态" << std::endl;
                        std::cout << "0. 返回上级菜单" << std::endl;

                        int actionChoice = selectIntCheck(0, 1);

                        if (actionChoice == 1)
                        {
                            modifyConsultationStatus(target, department);
                            pause();
                        }
                        else
                        {
                            break;
                        }
                    }
                }
                else
                {
                    std::cout << "未找到指定的看诊记录！" << std::endl;
                    pause();
                }
            }
        }
        else if (choice == 3)
        {
            bool ishave = viewAllConsultations(con, department);

            if (ishave)
            {
                std::string conID = inputRecordIDCheck("请输入要删除的看诊记录ID: ", {"con"});
                Consultation *target = con;
                while (target != nullptr)
                {
                    if (!target->isDeleted && target->consultationID == conID && target->department == department)
                    {
                        break;
                    }
                    target = target->next;
                }

                if (target)
                {
                    std::cout << "正在删除的看诊记录信息如下：" << std::endl;

                    std::string statusStr = conStatusToString(target->status);

                    if (statusStr == "正在处理")
                    {
                        std::cout << "ID: " << target->consultationID
                                  << ", 患者ID: " << target->patientID
                                  << ", 医生ID: " << target->doctorID
                                  << ", 时间: " << target->consultationTime
                                  << ", 科室: " << target->department
                                  << ", 状态: " << statusStr
                                  << std::endl;

                        std::cout << "  主诉: " << target->chiefComplaint << std::endl;
                        std::cout << "  现病史: " << target->historyOfPresentIllness << std::endl;
                        std::cout << "  既往史: " << target->pastMedicalHistory << std::endl;
                        std::cout << "  家族史: " << target->familyHistory << std::endl;

                        std::cout << "  初步诊断: " << target->preliminaryDiagnosis << std::endl;
                        std::cout << "  计划检查项目: ";
                        for (const auto &exam : target->examinationlist)
                        {
                            std::cout << exam << " ";
                        }
                        std::cout << std::endl;
                        std::cout << "  计划用药" << (target->isPrecriptionReviewed ? "（已审核）" : "（未审核）") << ": " << std::endl;
                        for (const auto &med : target->prescriptions)
                        {
                            std::cout << "药品ID: " << med.medicineID << ", 名称: " << med.name << ", 用量: " << med.dosage
                                      << ", 频次: " << med.frequency << ", 疗程: " << med.duration
                                      << ", 备注: " << med.note << std::endl;
                        }

                        std::cout << "是否建议住院: " << (target->isHospitalizationRecommended ? "是" : "否") << std::endl;

                        std::cout << "  医生备注: " << target->note << std::endl;
                    }
                    else
                    {
                        std::cout << "ID: " << target->consultationID
                                  << ", 患者ID: " << target->patientID
                                  << ", 医生ID: " << target->doctorID
                                  << ", 时间: " << target->consultationTime
                                  << ", 科室: " << target->department
                                  << ", 状态: " << statusStr
                                  << ", 备注: " << target->note
                                  << std::endl;
                    }

                    std::cout << "确认要删除该看诊记录吗？: " << std::endl;
                    std::cout << "1. 确认" << std::endl;
                    std::cout << "0. 取消" << std::endl;

                    int confirm = selectIntCheck(0, 1);

                    if (confirm == 1)
                    {
                        deleteConsultation(target, department);
                        pause();
                    }
                    else
                    {
                        std::cout << "已取消删除操作。" << std::endl;
                        pause();
                    }
                }
                else
                {
                    std::cout << "未找到指定的看诊记录！" << std::endl;
                    pause();
                }
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
}
// ==================================== 检查记录管理 =================================

// 查看所有的检查记录
bool Admin::viewAllExaminations(Examination *&exam, const std::string &department)
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

            if (statusStr == "报告已出")
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
        return false;
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
    return true;
}
// 查看指定医生的检查记录
bool Admin::viewExaminationsByDoctor(Examination *&exam, const std::string &department)
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

            if (statusStr == "报告已出")
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
        return false;
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
    return true;
}
// 查看指定患者的检查记录
bool Admin::viewExaminationsByPatient(Examination *&exam, const std::string &department)
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

            if (statusStr == "报告已出")
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
        return false;
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
    return true;
}
// 查看指定状态的检查记录
bool Admin::viewExaminationsByStatus(Examination *&exam, const std::string &department)
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

            if (statusStr == "报告已出")
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
    }
    return found;
}
// 修改检查记录的状态
void Admin::modifyExaminationStatus(Examination *&target, const std::string &department)
{
    std::cout << "请输入新的检查状态 (0 - 已下单, 1 - 已支付, 2 - 检查中, 3 - 检查完成, 4 - 报告已出, 5 - 已作废): ";
    int newStatus = selectIntCheck(0, 5);
    target->status = static_cast<ExaminationStatus>(newStatus);
    std::cout << "检查记录状态已更新！" << std::endl;
}
// 删除检查记录（逻辑删除，设置 isDeleted 标志）
void Admin::deleteExamination(Examination *&target, const std::string &department)
{
    target->isDeleted = true;
    std::cout << "检查记录已删除！" << std::endl;
}
// 添加检查记录（根据输入信息创建新的 Examination 对象，并插入到链表中）
bool Admin::addExamination(Examination *&exam, const std::string &department, Consultation *con, int &idCounter)
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
                return false;
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
                    return false;
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
    }
    return currentCon != nullptr;
}
// 根据检查记录ID查看检查记录
bool Admin::viewExaminationByID(Examination *&exam, const std::string &department)
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

            if (statusStr == "报告已出")
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
    return found;
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
            bool ishave = viewAllExaminations(exam, department);

            if (ishave)
            {
                std::string examID = inputRecordIDCheck("请输入要修改状态的检查记录ID: ", {"exa"});
                Examination *target = exam;
                while (target != nullptr)
                {
                    if (!target->isDeleted && target->examinationID == examID && target->department == department)
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

                        std::string statusStr = examStatusToString(target->status);

                        if (statusStr == "报告已出")
                        {
                            std::cout << "ID: " << target->examinationID
                                      << ", 患者ID: " << target->patientID
                                      << ", 医生ID: " << target->doctorID
                                      << ", 科室: " << target->department
                                      << ", 检查项目: " << target->itemName
                                      << ", 下单时间: " << target->orderTime
                                      << ", 状态: " << statusStr
                                      << ", 出报告时间: " << (target->reportTime.empty() ? "报告未出" : target->reportTime)
                                      << ", 费用: " << target->fee
                                      << std::endl;
                            std::cout << "检查结果：" << findVitalSignToString(target) << std::endl;
                            std::cout << "报告摘要：" << target->reportSummary << std::endl;
                            std::cout << "医生备注：" << target->note << std::endl;
                        }
                        else
                        {
                            std::cout << "ID: " << target->examinationID
                                      << ", 患者ID: " << target->patientID
                                      << ", 医生ID: " << target->doctorID
                                      << ", 科室: " << target->department
                                      << ", 检查项目: " << target->itemName
                                      << ", 下单时间: " << target->orderTime
                                      << ", 状态: " << statusStr
                                      << ", 出报告时间: " << (target->reportTime.empty() ? "报告未出" : target->reportTime)
                                      << ", 费用: " << target->fee
                                      << std::endl;
                        }

                        std::cout << "请选择你的操作：" << std::endl;
                        std::cout << "1. 修改检查状态" << std::endl;
                        std::cout << "0. 返回上级菜单" << std::endl;

                        int modifyChoice = selectIntCheck(0, 1);

                        if (modifyChoice == 1)
                        {
                            modifyExaminationStatus(target, department);
                            pause();
                        }
                        else
                        {
                            break;
                        }
                    }
                }
                else
                {
                    std::cout << "未找到指定的检查记录！" << std::endl;
                    pause();
                }
            }
        }
        else if (choice == 3)
        {

            bool ishave = viewAllExaminations(exam, department);

            if (ishave)
            {
                std::string examID = inputRecordIDCheck("请输入要删除的检查记录ID: ", {"exa"});
                Examination *target = exam;
                while (target != nullptr)
                {
                    if (!target->isDeleted && target->examinationID == examID && target->department == department)
                    {
                        break;
                    }
                    target = target->next;
                }

                if (target)
                {
                    std::cout << "正在删除的检查记录信息如下：" << std::endl;
                    std::string statusStr = examStatusToString(target->status);

                    if (statusStr == "报告已出")
                    {
                        std::cout << "ID: " << target->examinationID
                                  << ", 患者ID: " << target->patientID
                                  << ", 医生ID: " << target->doctorID
                                  << ", 科室: " << target->department
                                  << ", 检查项目: " << target->itemName
                                  << ", 下单时间: " << target->orderTime
                                  << ", 状态: " << statusStr
                                  << ", 出报告时间: " << (target->reportTime.empty() ? "报告未出" : target->reportTime)
                                  << ", 费用: " << target->fee
                                  << std::endl;
                        std::cout << "检查结果：" << findVitalSignToString(target) << std::endl;
                        std::cout << "报告摘要：" << target->reportSummary << std::endl;
                        std::cout << "医生备注：" << target->note << std::endl;
                    }
                    else
                    {
                        std::cout << "ID: " << target->examinationID
                                  << ", 患者ID: " << target->patientID
                                  << ", 医生ID: " << target->doctorID
                                  << ", 科室: " << target->department
                                  << ", 检查项目: " << target->itemName
                                  << ", 下单时间: " << target->orderTime
                                  << ", 状态: " << statusStr
                                  << ", 出报告时间: " << (target->reportTime.empty() ? "报告未出" : target->reportTime)
                                  << ", 费用: " << target->fee
                                  << std::endl;
                    }

                    std::cout << "确认要删除该检查记录吗？: " << std::endl;
                    std::cout << "1. 确认" << std::endl;
                    std::cout << "0. 取消" << std::endl;

                    int confirm = selectIntCheck(0, 1);

                    if (confirm == 1)
                    {

                        deleteExamination(target, department);
                        pause();
                    }
                    else
                    {
                        std::cout << "已取消删除操作。" << std::endl;
                        pause();
                    }
                }
                else
                {
                    std::cout << "未找到指定的检查记录！" << std::endl;
                    pause();
                }
            }
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
bool Admin::viewAllHospitalizations(Hospitalization *&hos, const std::string &department)
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
    return found;
}
// 根据患者ID查看住院记录
bool Admin::viewHospitalizationsByPatient(Hospitalization *&hos, const std::string &department)
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
    }
    return found;
}
// 根据医生ID查看住院记录
bool Admin::viewHospitalizationsByDoctor(Hospitalization *&hos, const std::string &department)
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
    }
    return found;
}
// 根据负责护士ID查看住院记录
bool Admin::viewHospitalizationByNurse(Hospitalization *&hos, const std::string &department)
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
    }
    return found;
}
// 根据住院记录ID查看住院记录
bool Admin::viewHospitalizationByID(Hospitalization *&hos, const std::string &department)
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
    return found;
}
// 根据住院状态查看住院记录
bool Admin::viewHospitalizationsByStatus(Hospitalization *&hos, const std::string &department)
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
    }
    return found;
}
// 根据病房类型查看住院记录
bool Admin::viewHospitalizationByWardType(Hospitalization *&hos, const std::string &department)
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

    if (!found)
    {
        std::cout << "未找到该病房类型的住院记录！" << std::endl;
    }
    return found;
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
bool Admin::addHospitalization(Hospitalization *&hos, Nurse *nurse, const std::string &department, Consultation *con, bedInfo *&bed, int &idCounter)
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
                return false;
            }
            if (!currentCon->isHospitalizationRecommended)
            {
                std::cout << "该看诊记录未建议住院，无法关联！" << std::endl;
                return false;
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
        return false;
    }

    newHos->wardType = HospitalizationWardTypeMenu(); // 选择病房类型
    if (newHos->wardType == "0")
    {
        std::cout << "取消添加住院记录。" << std::endl;
        delete newHos; // 释放内存
        return false;
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
        return false;
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
                return false;
            }
            else if (b->status == bedStatus::ClEANING)
            {
                std::cout << "该床位正在清洁中！无法添加住院记录。" << std::endl;
                delete newHos; // 释放内存
                return false;
            }
            else if (b->status == bedStatus::UNAVAILABLE)
            {
                std::cout << "该床位不可用！无法添加住院记录。" << std::endl;
                delete newHos; // 释放内存
                return false;
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
        return false;
    }

    Nurse *n = nurse;

    std::cout << "当前科室可以分配的护士列表:" << std::endl;
    while (n != nullptr)
    {
        if (!n->isDeleted && n->department == department && n->isOnDuty)
        {
            std::cout << "护士ID: " << n->getUserID() << ", 姓名: " << n->getUsername() << ", 是否在岗: " << (n->isOnDuty ? "是" : "否") << std::endl;
        }
        n = n->next;
    }

    newHos->nurseID = inputIDCheck("请输入负责护士ID: "); // 输入负责护士ID并检查格式

    bool nurseFound = false;
    while (n != nullptr)
    {
        if (!n->isDeleted && n->getUserID() == newHos->nurseID && n->department == department)
        {
            if (!n->isOnDuty)
            {
                std::cout << "该护士当前不在岗，无法分配！" << std::endl;
                delete newHos;
                return false;
            }

            nurseFound = true;
            break;
        }
        n = n->next;
    }

    if (!nurseFound)
    {
        std::cout << "未找到指定的护士！无法添加住院记录。" << std::endl;
        delete newHos; // 释放内存
        return false;
    }

    newHos->hospitalizationID = "hos" + std::to_string(idCounter++).insert(0, 6 - std::to_string(idCounter).length(), '0'); // 生成住院记录ID，格式为 "hos" + 6位数字
    MyTime &t = MyTime::getInstance();
    newHos->applyTime = t.getTime(); // 设置申请时间为当前时间

    newHos->deposit = inputFeeCheck("请输入押金金额: "); // 输入押金金额并检查格式

    // 插入到链表头部
    newHos->next = hos;
    if (hos != nullptr)
    {
        hos->prev = newHos;
    }
    hos = newHos;
    return true;
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
            bool ishave = viewAllHospitalizations(hos, department);

            if (ishave)
            {
                std::string hosID = inputRecordIDCheck("请输入要修改状态的住院记录ID: ", {"hos"}); // 输入住院记录ID并检查格式
                Hospitalization *target = hos;
                while (target != nullptr)
                {
                    if (!target->isDeleted && target->hospitalizationID == hosID && target->department == department)
                    {
                        break;
                    }
                    target = target->next;
                }

                if (target)
                {
                    while (true)
                    {
                        std::cout << "正在修改的住院记录信息如下：" << std::endl;

                        std::string statusStr = hosStatusToString(target->status);
                        std::string wardTypeStr = target->wardType;
                        std::cout << "ID: " << target->hospitalizationID
                                  << ", 患者ID: " << target->patientID
                                  << ", 医生ID: " << target->doctorID
                                  << ", 负责护士ID: " << target->nurseID
                                  << ", 科室: " << target->department
                                  << ", 病房类型: " << (wardTypeStr.empty() ? "未分配" : wardTypeStr)
                                  << ", 床位号: " << (target->bedNumber.empty() ? "未分配" : target->bedNumber)
                                  << ", 申请时间: " << target->applyTime
                                  << ", 允许入院时间: " << (target->availableAdmitTime.empty() ? "未分配" : target->availableAdmitTime)
                                  << ", 实际入院时间: " << (target->admitTime.empty() ? "未入院" : target->admitTime)
                                  << ", 出院时间: " << (target->dischargeTime.empty() ? "未出院" : target->dischargeTime)
                                  << ", 押金: " << target->deposit
                                  << ", 总费用: " << target->totalCost
                                  << ", 住院记录状态: " << statusStr
                                  << std::endl;

                        std::cout << "请选择你的操作：" << std::endl;
                        std::cout << "1.修改住院记录状态" << std::endl;
                        std::cout << "0.返回上级菜单" << std::endl;

                        int modifyChoice = selectIntCheck(0, 1);

                        if (modifyChoice == 1)
                        {
                            modifyHospitalizationStatus(target, department);
                            pause();
                        }
                        else
                        {
                            break;
                        }
                    }
                }
                else
                {
                    std::cout << "未找到指定的住院记录！" << std::endl;
                    pause();
                }
            }
        }
        else if (choice == 3)
        {
            bool ishave = viewAllHospitalizations(hos, department);

            if (ishave)
            {
                std::string hosID = inputRecordIDCheck("请输入要删除的住院记录ID: ", {"hos"}); // 输入住院记录ID并检查格式
                Hospitalization *target = hos;
                while (target != nullptr)
                {
                    if (!target->isDeleted && target->hospitalizationID == hosID && target->department == department)
                    {
                        break;
                    }
                    target = target->next;
                }

                if (target)
                {
                    std::cout << "正在删除的住院信息如下：" << std::endl;
                    std::string statusStr = hosStatusToString(target->status);
                    std::string wardTypeStr = target->wardType;
                    std::cout << "ID: " << target->hospitalizationID
                              << ", 患者ID: " << target->patientID
                              << ", 医生ID: " << target->doctorID
                              << ", 负责护士ID: " << target->nurseID
                              << ", 科室: " << target->department
                              << ", 病房类型: " << (wardTypeStr.empty() ? "未分配" : wardTypeStr)
                              << ", 床位号: " << (target->bedNumber.empty() ? "未分配" : target->bedNumber)
                              << ", 申请时间: " << target->applyTime
                              << ", 允许入院时间: " << (target->availableAdmitTime.empty() ? "未分配" : target->availableAdmitTime)
                              << ", 实际入院时间: " << (target->admitTime.empty() ? "未入院" : target->admitTime)
                              << ", 出院时间: " << (target->dischargeTime.empty() ? "未出院" : target->dischargeTime)
                              << ", 押金: " << target->deposit
                              << ", 总费用: " << target->totalCost
                              << ", 住院记录状态: " << statusStr
                              << std::endl;

                    std::cout << "确认要删除该住院记录吗？: " << std::endl;
                    std::cout << "1. 确认" << std::endl;
                    std::cout << "0. 取消" << std::endl;

                    int confirmChoice = selectIntCheck(0, 1);

                    if (confirmChoice == 1)
                    {

                        deleteHospitalization(target, department);
                        pause();
                    }
                    else
                    {
                        std::cout << "已取消删除操作。" << std::endl;
                        pause();
                    }
                }
                else
                {
                    std::cout << "未找到指定的住院记录！" << std::endl;
                    pause();
                }
            }
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
bool Admin::viewAllBeds(bedInfo *&bed, const std::string &department)
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
    return found;
}
// 根据病房类型查看床位信息
bool Admin::viewBedsByWardType(bedInfo *&bed, const std::string &department)
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
    }
    return found;
}
// 根据床位状态查看床位信息
bool Admin::viewBedsByStatus(bedInfo *&bed, const std::string &department)
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
    }
    return found;
}
// 根据床位ID查看床位信息
bool Admin::viewBedByID(bedInfo *&bed, const std::string &department)
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
    }
    return found;
}
// 根据护士ID查看床位信息
bool Admin::viewBedsByNurseID(bedInfo *&bed, const std::string &department)
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
    }
    return found;
}
// 根据患者ID查看床位信息
bool Admin::viewBedsByPatientID(bedInfo *&bed, const std::string &department)
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
    }
    return found;
}
// 根据住院记录ID查看床位信息
bool Admin::viewBedsByHospitalizationID(bedInfo *&bed, Hospitalization *&hos, const std::string &department)
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
        return false;
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
    return foundBed;
}
// 修改床位状态
void Admin::modifyBedStatus(bedInfo *&target, const std::string &department)
{
    std::cout << "请输入新的床位状态 (0 - 可用, 1 - 占用, 2 - 清洁中, 3 - 不可用): ";
    int newStatus = selectIntCheck(0, 3);
    target->status = static_cast<bedStatus>(newStatus);
    std::cout << "床位状态已更新！" << std::endl;
}
// 删除床位信息（逻辑删除，设置 isDeleted 标志）
void Admin::deleteBedInfo(bedInfo *&target, const std::string &department)
{
    target->isDeleted = true; // 逻辑删除
    std::cout << "床位ID: " << target->bedID << " 已删除！" << std::endl;
}
// 添加床位信息（根据输入信息创建新的 bedInfo 对象，并插入到链表中）
bool Admin::addBedInfo(bedInfo *&bed, const std::string &department)
{

    std::string wardType = HospitalizationWardTypeMenu(); // 选择病房类型

    if (wardType == "0")
    {
        std::cout << "取消添加床位信息。" << std::endl;
        return false;
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
    return true;
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
            bool ishave = viewAllBeds(bed, department);

            if (ishave)
            {
                std::string wardType = HospitalizationWardTypeMenu();                                        // 选择病房类型，作为输入床位ID的前缀检查
                std::string bedID = inputBedNumberCheck("请输入要修改状态的床位号: ", department, wardType); // 输入床位ID并检查格式

                bedInfo *target = bed;
                while (target != nullptr)
                {
                    if (!target->isDeleted && target->bedID == bedID && target->department == department)
                    {
                        break;
                    }
                    target = target->next;
                }

                if (target)
                {
                    while (true)
                    {
                        std::cout << "正在修改的床位信息如下：" << std::endl;
                        std::string statusStr = bedStatusToString(target->status);
                        std::cout << "床位ID: " << target->bedID
                                  << ", 科室: " << target->department
                                  << ", 区域: " << target->areaNumber
                                  << ", 病房号: " << target->wardNumber
                                  << ", 床位号: " << target->bedNumber
                                  << ", 病房类型: " << target->wardType
                                  << ", 床位状态: " << statusStr
                                  << ", 患者ID: " << (target->patientID.empty() ? "无" : target->patientID)
                                  << ", 护士ID: " << (target->nurseID.empty() ? "无" : target->nurseID)
                                  << ", 使用次数: " << target->useTimes
                                  << ", 占用天数: " << target->daysOccupied
                                  << std::endl;

                        std::cout << "请选择你的操作：" << std::endl;
                        std::cout << "1.修改床位状态" << std::endl;
                        std::cout << "0.返回上级菜单" << std::endl;

                        int modifyChoice = selectIntCheck(0, 1);

                        if (modifyChoice == 1)
                        {
                            modifyBedStatus(target, department);
                            pause();
                        }
                        else
                        {
                            break;
                        }
                    }
                }
                else
                {
                    std::cout << "未找到指定的床位信息！" << std::endl;
                    pause();
                }
            }
        }
        else if (choice == 3) // 删除床位信息
        {
            bool ishave = viewAllBeds(bed, department);

            if (ishave)
            {
                std::string wardType = HospitalizationWardTypeMenu();                                    // 选择病房类型，作为输入床位ID的前缀检查
                std::string bedID = inputBedNumberCheck("请输入要删除的床位号: ", department, wardType); // 输入床位ID并检查格式

                bedInfo *target = bed;
                while (target != nullptr)
                {
                    if (!target->isDeleted && target->bedID == bedID && target->department == department)
                    {
                        break;
                    }
                    target = target->next;
                }

                if (target)
                {
                    std::cout << "正在删除的床位信息如下：" << std::endl;
                    std::string statusStr = bedStatusToString(target->status);
                    std::cout << "床位ID: " << target->bedID
                              << ", 科室: " << target->department
                              << ", 区域: " << target->areaNumber
                              << ", 病房号: " << target->wardNumber
                              << ", 床位号: " << target->bedNumber
                              << ", 病房类型: " << target->wardType
                              << ", 床位状态: " << statusStr
                              << ", 患者ID: " << (target->patientID.empty() ? "无" : target->patientID)
                              << ", 护士ID: " << (target->nurseID.empty() ? "无" : target->nurseID)
                              << ", 使用次数: " << target->useTimes
                              << ", 占用天数: " << target->daysOccupied
                              << std::endl;

                    std::cout << "确认要删除该床位信息吗？: " << std::endl;
                    std::cout << "1. 确认" << std::endl;
                    std::cout << "0. 取消" << std::endl;

                    int confirmChoice = selectIntCheck(0, 1);

                    if (confirmChoice == 1)
                    {

                        deleteBedInfo(target, department);
                        pause();
                    }
                    else
                    {
                        std::cout << "已取消删除操作。" << std::endl;
                        pause();
                    }
                }
                else
                {
                    std::cout << "未找到指定的床位信息！" << std::endl;
                    pause();
                }
            }
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
bool Admin::viewAllMedicationRecords(MedicationRecord *&medRec, const std::string &department)
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
    return found;
}
// 根据患者ID查看用药记录
bool Admin::viewMedicationRecordsByPatient(MedicationRecord *&medRec, const std::string &department)
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
    return found;
}
// 根据医生ID查看用药记录
bool Admin::viewMedicationRecordsByDoctor(MedicationRecord *&medRec, const std::string &department)
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
    return found;
}
// 根据药师ID查看用药记录
bool Admin::viewMedicationRecordsByPharmacist(MedicationRecord *&medRec, const std::string &department)
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
    return found;
}
// 根据用药记录ID查看用药记录
bool Admin::viewMedicationRecordByID(MedicationRecord *&medRec, const std::string &department)
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
    return found;
}
// 根据药品ID查看用药记录
bool Admin::viewMedicationRecordsByMedicationID(MedicationRecord *&medRec, const std::string &department)
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
    return found;
}
// 根据看诊记录ID查看用药记录
bool Admin::viewMedicationRecordsByConsultationID(MedicationRecord *&medRec, const std::string &department)
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
    return found;
}
// 根据审核状态查看用药记录
bool Admin::viewMedicationRecordsByReviewStatus(MedicationRecord *&medRec, const std::string &department)
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
    return found;
}
// 根据用药状态查看用药记录
bool Admin::viewMedicationRecordsByStatus(MedicationRecord *&medRec, const std::string &department)
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
    return found;
}
// 修改用药记录的审核状态
void Admin::modifyMedicationRecordReviewStatus(MedicationRecord *&target, const std::string &department)
{
    int newStatus = MedicationRecordReviewResultMenu(); // 选择新的审核状态
    target->status = static_cast<MedicationStatus>(newStatus);
    std::cout << "用药记录状态已更新！" << std::endl;
}
// 删除用药记录（逻辑删除，设置 isDeleted 标志）
void Admin::deleteMedicationRecord(MedicationRecord *&target, const std::string &department)
{
    target->isDeleted = true; // 逻辑删除
    std::cout << "用药记录ID: " << target->medRecordID << " 已删除！" << std::endl;
}
// 添加用药记录（根据输入信息创建新的 MedicationRecord 对象，并插入到链表中）
bool Admin::addMedicationRecord(MedicationRecord *&medRec, Consultation *con, Pharmacist *pha, Medicine *med, const std::string &department, int &idCounter)
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
        return false;
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
                return false;
            }
            else if (currentCon->isPrecriptionReviewed)
            {
                std::cout << "选定的看诊记录的处方已经审核过了，无法再次创建用药记录！" << std::endl;
                delete newMedRec; // 释放之前创建的 MedicationRecord 对象，避免内存泄漏
                return false;
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
                return false;
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
                    return false;
                }
            }

            currentCon->isPrecriptionReviewed = true; // 标记该看诊记录的处方已经审核过了，避免重复创建用药记录

            int newStatus = MedicationRecordReviewResultMenu(); // 选择新的审核状态

            if (newStatus == 0)
            {
                std::cout << "用药记录创建已取消！" << std::endl;
                delete newMedRec; // 释放之前创建的 MedicationRecord 对象，避免内存泄漏
                return false;
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
        return false;
    }

    // 插入到链表头部
    newMedRec->next = medRec;
    if (medRec != nullptr)
    {
        medRec->prev = newMedRec;
    }
    medRec = newMedRec;
    std::cout << "用药记录已添加！用药记录ID: " << newMedRec->medRecordID << std::endl;
    return true;
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
            bool ishave = viewAllMedicationRecords(medRec, department);

            if (ishave)
            {
                std::string medRecordID = inputRecordIDCheck("请输入要修改的用药记录ID: ", {"mrd"});

                MedicationRecord *target = medRec;
                while (target != nullptr)
                {
                    if (!target->isDeleted && target->medRecordID == medRecordID && target->department == department)
                    {
                        break;
                    }
                    target = target->next;
                }

                if (target)
                {
                    while (true)
                    {
                        std::cout << "正在修改的用药记录信息如下：" << std::endl;
                        std::string reviewStatusStr = medicationReviewStatusToString(target->reviewStatus);
                        std::string statusStr = medicationStatusToString(target->status);
                        std::cout << "用药记录ID: " << target->medRecordID
                                  << ", 关联看诊记录ID: " << target->consultationID
                                  << ", 患者ID: " << target->patientID
                                  << ", 医生ID: " << target->doctorID
                                  << ", 药师ID: " << (target->pharmacistID.empty() ? "无" : target->pharmacistID)
                                  << ", 科室: " << target->department
                                  << ", 审核状态: " << statusStr
                                  << ", 总费用: " << target->totalCost
                                  << ", 支付时间: " << (target->paymentTime.empty() ? "未支付" : target->paymentTime)
                                  << ", 发药时间: " << (target->dispenseTime.empty() ? "未发药" : target->dispenseTime)
                                  << ", 创建时间: " << target->createTime
                                  << ", 备注: " << (target->note.empty() ? "无" : target->note)
                                  << std::endl;

                        std::cout << "用药明细: " << std::endl;
                        for (const auto &line : target->lines)
                        {
                            std::cout << "  - 药品ID: " << line.medicineID
                                      << ", 药品名称: " << line.medicineName
                                      << ", 数量: " << line.quantity
                                      << ", 单价: " << line.unitPrice
                                      << ", 用法备注: " << (line.note.empty() ? "无" : line.note)
                                      << std::endl;
                        }

                        std::cout << "请选择你的操作：" << std::endl;
                        std::cout << "1. 修改审核状态" << std::endl;
                        std::cout << "0. 返回上一级菜单" << std::endl;

                        int modifyChoice = selectIntCheck(0, 1);

                        if (modifyChoice == 1)
                        {
                            modifyMedicationRecordReviewStatus(target, department);
                            pause();
                        }
                        else
                        {
                            break;
                        }
                    }
                }
                else
                {
                    std::cout << "未找到ID为 " << medRecordID << " 的用药记录！" << std::endl;
                    pause();
                }
            }
        }
        else if (choice == 3) // 删除用药记录
        {
            bool ishave = viewAllMedicationRecords(medRec, department);

            if (ishave)
            {
                std::string medRecordID = inputRecordIDCheck("请输入要删除的用药记录ID: ", {"mrd"});

                MedicationRecord *target = medRec;
                while (target != nullptr)
                {
                    if (!target->isDeleted && target->medRecordID == medRecordID && target->department == department)
                    {
                        break;
                    }
                    target = target->next;
                }

                if (target)
                {
                    std::cout << "正在删除的用药记录信息如下：" << std::endl;
                    std::string reviewStatusStr = medicationReviewStatusToString(target->reviewStatus);
                    std::string statusStr = medicationStatusToString(target->status);
                    std::cout << "用药记录ID: " << target->medRecordID
                              << ", 关联看诊记录ID: " << target->consultationID
                              << ", 患者ID: " << target->patientID
                              << ", 医生ID: " << target->doctorID
                              << ", 药师ID: " << (target->pharmacistID.empty() ? "无" : target->pharmacistID)
                              << ", 科室: " << target->department
                              << ", 审核状态: " << statusStr
                              << ", 总费用: " << target->totalCost
                              << ", 支付时间: " << (target->paymentTime.empty() ? "未支付" : target->paymentTime)
                              << ", 发药时间: " << (target->dispenseTime.empty() ? "未发药" : target->dispenseTime)
                              << ", 创建时间: " << target->createTime
                              << ", 备注: " << (target->note.empty() ? "无" : target->note)
                              << std::endl;

                    std::cout << "用药明细: " << std::endl;
                    for (const auto &line : target->lines)
                    {
                        std::cout << "  - 药品ID: " << line.medicineID
                                  << ", 药品名称: " << line.medicineName
                                  << ", 数量: " << line.quantity
                                  << ", 单价: " << line.unitPrice
                                  << ", 用法备注: " << (line.note.empty() ? "无" : line.note)
                                  << std::endl;
                    }

                    std::cout << "确认要删除该用药记录吗？: " << std::endl;
                    std::cout << "1. 确认" << std::endl;
                    std::cout << "0. 取消" << std::endl;

                    int confirm = selectIntCheck(0, 1);

                    if (confirm == 1)
                    {
                        deleteMedicationRecord(target, department);
                        pause();
                    }
                    else
                    {
                        std::cout << "用药记录删除已取消！" << std::endl;
                        pause();
                    }
                }
                else
                {
                    std::cout << "未找到ID为 " << medRecordID << " 的用药记录！" << std::endl;
                    pause();
                }
            }
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
bool Admin::viewAllMedicines(Medicine *&med, const std::string &department)
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
    return found;
}
// 根据药品ID查看药品信息
bool Admin::viewMedicineByID(Medicine *&med, const std::string &department)
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
    return found;
}
// 根据药品名称查看药品信息
bool Admin::viewMedicinesByName(Medicine *&med, const std::string &department)
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
    return found;
}
// 根据药品状态查看药品信息
bool Admin::viewMedicinesByStatus(Medicine *&med, const std::string &department)
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
    return found;
}
// 根据生产厂家查看药品信息
bool Admin::viewMedicinesByManufacturer(Medicine *&med, const std::string &department)
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
    return found;
}
// 查看低于安全库存阈值的药品信息
bool Admin::viewMedicinesBySafetyStock(Medicine *&med, const std::string &department)
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
    return found;
}
// 修改药品状态
void Admin::modifyMedicineStatus(Medicine *&target, const std::string &department)
{
    std::cout << "当前药品状态: " << medicineStatusToString(target->status) << std::endl;
    int newStatus = MedicineStatusMenu(); // 选择新的药品状态
    target->status = static_cast<MedicineStatus>(newStatus);
    std::cout << "药品状态已更新！" << std::endl;
}
// 修改药品名称
void Admin::modifyMedicineName(Medicine *&target, const std::string &department)
{
    std::cout << "当前药品名称: " << target->name << std::endl;
    std::string newName = inputStringCheck("请输入新的药品名称: ");
    target->name = newName;
    std::cout << "药品名称已更新！" << std::endl;
}
// 修改药品规格
void Admin::modifyMedicineSpecification(Medicine *&target, const std::string &department)
{
    std::cout << "当前药品规格: " << target->specification << std::endl;
    std::string newSpecification = inputStringCheck("请输入新的药品规格: ");
    target->specification = newSpecification;
    std::cout << "药品规格已更新！" << std::endl;
}
// 修改药品进价
void Admin::modifyMedicinePurchasePrice(Medicine *&target, const std::string &department)
{
    std::cout << "当前药品进价: " << target->purchasePrice << std::endl;
    double newPurchasePrice = inputFeeCheck("请输入新的药品进价: ");
    target->purchasePrice = newPurchasePrice;
    std::cout << "药品进价已更新！" << std::endl;
}
// 修改药品售价
void Admin::modifyMedicineSalePrice(Medicine *&target, const std::string &department)
{
    std::cout << "当前药品售价: " << target->salePrice << std::endl;
    double newSalePrice = inputFeeCheck("请输入新的药品售价: ");
    target->salePrice = newSalePrice;
    std::cout << "药品售价已更新！" << std::endl;
}
// 修改药品库存数量
void Admin::modifyMedicineStock(Medicine *&target, const std::string &department)
{
    std::cout << "当前库存数量: " << target->stock << std::endl;
    std::cout << "输入新的库存数量 ";
    int newStock = selectIntCheck(0, INT_MAX);
    target->stock = newStock;
    std::cout << "药品库存数量已更新！" << std::endl;
}
// 修改药品安全库存阈值
void Admin::modifyMedicineSafetyStock(Medicine *&target, const std::string &department)
{
    std::cout << "当前安全库存阈值: " << target->safetyStock << std::endl;
    std::cout << "输入新的安全库存阈值 ";
    int newSafetyStock = selectIntCheck(1, INT_MAX);
    target->safetyStock = newSafetyStock;
    std::cout << "药品安全库存阈值已更新！" << std::endl;
}
// 修改药品生产日期
void Admin::modifyMedicineProductionDate(Medicine *&target, const std::string &department)
{
    std::cout << "当前药品生产日期: " << target->productionDate << std::endl;
    std::string newProductionDate = inputDateCheck("请输入新的药品生产日期: ");
    target->productionDate = newProductionDate;
    std::cout << "药品生产日期已更新！" << std::endl;
}
// 修改药品有效期
void Admin::modifyMedicineExpiryDate(Medicine *&target, const std::string &department)
{
    std::cout << "当前药品有效期: " << target->expiryDate << std::endl;
    std::string newExpiryDate = inputDateCheck("请输入新的药品有效期限: ");
    target->expiryDate = newExpiryDate;
    std::cout << "药品有效期限已更新！" << std::endl;
}
// 修改药品生产厂家
void Admin::modifyMedicineManufacturer(Medicine *&target, const std::string &department)
{
    std::cout << "当前药品生产厂家: " << target->manufacturer << std::endl;
    std::string newManufacturer = inputStringCheck("请输入新的药品生产厂家: ");
    target->manufacturer = newManufacturer;
    std::cout << "药品生产厂家已更新！" << std::endl;
}
// 修改药品备注信息
void Admin::modifyMedicineNote(Medicine *&target, const std::string &department)
{
    std::cout << "当前药品备注信息: " << target->note << std::endl;
    std::string newNote = inputStringCheck("请输入新的药品备注信息: ");
    target->note = newNote;
    std::cout << "药品备注信息已更新！" << std::endl;
}
// 删除药品（逻辑删除，设置 isDeleted 标志）
void Admin::deleteMedicine(Medicine *&target, const std::string &department)
{
    target->isDeleted = true;
    std::cout << "药品ID: " << target->medicineID << " 已删除！" << std::endl;
}
// 添加药品（根据输入信息创建新的 Medicine 对象，并插入到链表中）
bool Admin::addMedicine(Medicine *&med, const std::string &department, int &idCounter)
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
    return true;
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
            bool ishave = viewAllMedicines(med, department);

            if (ishave)
            {
                std::string medicineID = inputRecordIDCheck("请输入要修改的药品ID: ", {"med"});

                Medicine *target = med;
                while (target != nullptr)
                {
                    if (!target->isDeleted && target->medicineID == medicineID && target->department == department)
                    {
                        break;
                    }
                    target = target->next;
                }

                if (target)
                {
                    while (true)
                    {
                        std::cout << "正在修改的药品信息：" << std::endl;
                        std::string statusStr = medicineStatusToString(target->status);
                        std::cout << "药品ID: " << target->medicineID
                                  << ", 药品名称: " << target->name
                                  << ", 规格: " << target->specification
                                  << ", 进价: " << target->purchasePrice
                                  << ", 售价: " << target->salePrice
                                  << ", 状态: " << statusStr
                                  << ", 当前库存数量: " << target->stock
                                  << ", 安全库存阈值: " << target->safetyStock
                                  << ", 生产日期: " << target->productionDate
                                  << ", 有效期至: " << target->expiryDate
                                  << ", 生产厂家: " << target->manufacturer
                                  << ", 备注: " << (target->note.empty() ? "无" : target->note)
                                  << std::endl;

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
                else
                {
                    std::cout << "未找到ID为 " << medicineID << " 的药品信息！" << std::endl;
                    pause();
                }
            }
        }
        else if (choice == 3) // 删除药品
        {
            bool ishave = viewAllMedicines(med, department);

            if (ishave)
            {
                std::string medicineID = inputRecordIDCheck("请输入要删除的药品ID: ", {"med"});

                Medicine *target = med;
                while (target != nullptr)
                {
                    if (!target->isDeleted && target->medicineID == medicineID && target->department == department)
                    {
                        break;
                    }
                    target = target->next;
                }

                if (target)
                {
                    std::cout << "正在删除的药品信息如下：" << std::endl;
                    std::string statusStr = medicineStatusToString(target->status);
                    std::cout << "药品ID: " << target->medicineID
                              << ", 药品名称: " << target->name
                              << ", 规格: " << target->specification
                              << ", 进价: " << target->purchasePrice
                              << ", 售价: " << target->salePrice
                              << ", 状态: " << statusStr
                              << ", 当前库存数量: " << target->stock
                              << ", 安全库存阈值: " << target->safetyStock
                              << ", 生产日期: " << target->productionDate
                              << ", 有效期至: " << target->expiryDate
                              << ", 生产厂家: " << target->manufacturer
                              << ", 备注: " << (target->note.empty() ? "无" : target->note)
                              << std::endl;

                    std::cout << "确认要删除该药品吗？: " << std::endl;
                    std::cout << "1. 确认" << std::endl;
                    std::cout << "0. 取消" << std::endl;

                    int confirm = selectIntCheck(0, 1);

                    if (confirm == 1)
                    {

                        deleteMedicine(target, department);
                        pause();
                    }
                    else
                    {
                        std::cout << "药品删除已取消！" << std::endl;
                        pause();
                    }
                }
                else
                {
                    std::cout << "未找到ID为 " << medicineID << " 的药品信息！" << std::endl;
                    pause();
                }
            }
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

// ==================================== 医生信息管理 =================================

// 查看所有的医生信息
bool Admin::viewAllDoctors(Doctor *&doc, const std::string &department)
{
    Doctor *current = doc;
    bool found = false;
    std::cout << "正在查找科室 " << department << " 的医生信息..." << std::endl;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->department == department)
        {
            std::string titleStr = doctorTitleToString(current->title);
            std::cout << "医生ID: " << current->getUserID()
                      << ", 姓名: " << current->getUsername()
                      << ",性别: " << current->getGender()
                      << ", 职称: " << titleStr
                      << ", 科室: " << current->department
                      << ", 联系方式: " << current->getTelephone() << " / " << current->getEmail()
                      << ", 擅长领域: " << current->specialty
                      << ", 在岗状态: " << (current->isOnDuty ? "在岗" : "不在岗")
                      << ", 排班信息：" << current->scheduleInfo
                      << ", 累计看诊人数: " << current->consultationCount
                      << ", 累计开具检查次数: " << current->examinationCount
                      << ", 累计开具住院证次数: " << current->hospitalizationApplyCount
                      << std::endl;
            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "该科室暂无医生信息！" << std::endl;
    }
    return found;
}
// 根据医生ID查看医生信息
bool Admin::viewDoctorByID(Doctor *&doc, const std::string &department)
{
    std::string doctorID = inputIDCheck("请输入要查找的医生ID: ");

    Doctor *current = doc;
    bool found = false;
    std::cout << "正在查找ID: " << doctorID << " 的医生信息..." << std::endl;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->getUserID() == doctorID && current->department == department)
        {
            std::string titleStr = doctorTitleToString(current->title);
            std::cout << "医生ID: " << current->getUserID()
                      << ", 姓名: " << current->getUsername()
                      << ",性别: " << current->getGender()
                      << ", 职称: " << titleStr
                      << ", 科室: " << current->department
                      << ", 联系方式: " << current->getTelephone() << " / " << current->getEmail()
                      << ", 擅长领域: " << current->specialty
                      << ", 在岗状态: " << (current->isOnDuty ? "在岗" : "不在岗")
                      << ", 排班信息：" << current->scheduleInfo
                      << ", 累计看诊人数: " << current->consultationCount
                      << ", 累计开具检查次数: " << current->examinationCount
                      << ", 累计开具住院证次数: " << current->hospitalizationApplyCount
                      << std::endl;
            found = true;
            break;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到ID为 " << doctorID << " 的医生信息！" << std::endl;
    }
    return found;
}
// 根据医生姓名查看医生信息
bool Admin::viewDoctorsByName(Doctor *&doc, const std::string &department)
{
    std::string name = inputStringCheck("请输入医生姓名（支持模糊查询）: ");

    Doctor *current = doc;
    bool found = false;
    std::cout << "正在查找姓名包含 \"" << name << "\" 的医生信息..." << std::endl;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->getUsername().find(name) != std::string::npos && current->department == department)
        {
            std::string titleStr = doctorTitleToString(current->title);
            std::cout << "医生ID: " << current->getUserID()
                      << ", 姓名: " << current->getUsername()
                      << ",性别: " << current->getGender()
                      << ", 职称: " << titleStr
                      << ", 科室: " << current->department
                      << ", 联系方式: " << current->getTelephone() << " / " << current->getEmail()
                      << ", 擅长领域: " << current->specialty
                      << ", 在岗状态: " << (current->isOnDuty ? "在岗" : "不在岗")
                      << ", 排班信息：" << current->scheduleInfo
                      << ", 累计看诊人数: " << current->consultationCount
                      << ", 累计开具检查次数: " << current->examinationCount
                      << ", 累计开具住院证次数: " << current->hospitalizationApplyCount
                      << std::endl;
            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到姓名包含 \"" << name << "\" 的医生信息！" << std::endl;
    }
    return found;
}
// 根据医生职称查看医生信息
bool Admin::viewDoctorByTitle(Doctor *&doc, const std::string &department)
{
    int titleFilter = DoctorTitleMenu(); // 选择医生职称过滤条件

    if (titleFilter == 0)
    {
        return false; // 返回上一级菜单
    }

    bool found = false;

    DoctorTitle filterTitle = static_cast<DoctorTitle>(titleFilter);

    Doctor *current = doc;
    std::cout << "正在查找职称为: " << doctorTitleToString(filterTitle) << " 的医生信息..." << std::endl;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->department == department && current->title == filterTitle)
        {
            std::string titleStr = doctorTitleToString(current->title);
            std::cout << "医生ID: " << current->getUserID()
                      << ", 姓名: " << current->getUsername()
                      << ",性别: " << current->getGender()
                      << ", 职称: " << titleStr
                      << ", 科室: " << current->department
                      << ", 联系方式: " << current->getTelephone() << " / " << current->getEmail()
                      << ", 擅长领域: " << current->specialty
                      << ", 在岗状态: " << (current->isOnDuty ? "在岗" : "不在岗")
                      << ", 排班信息：" << current->scheduleInfo
                      << ", 累计看诊人数: " << current->consultationCount
                      << ", 累计开具检查次数: " << current->examinationCount
                      << ", 累计开具住院证次数: " << current->hospitalizationApplyCount
                      << std::endl;
            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到职称为 " << doctorTitleToString(filterTitle) << " 的医生信息！" << std::endl;
    }
    return found;
}
// 根据医生在岗状态查看医生信息
bool Admin::viewDoctorsByOnDutyStatus(Doctor *&doc, const std::string &department)
{

    bool found = false;

    std::cout << "请选择医生在岗状态过滤条件: " << std::endl;
    std::cout << "1. 在岗" << std::endl;
    std::cout << "2. 不在岗" << std::endl;
    std::cout << "0. 返回上一级菜单" << std::endl;
    int onDutyFilter = selectIntCheck(0, 2);

    if (onDutyFilter == 0)
    {
        return false; // 返回上一级菜单
    }

    bool filterOnDutyStatus = (onDutyFilter == 1); // 1表示在岗，2表示不在岗

    Doctor *current = doc;
    std::cout << "正在查找在岗状态为: " << (filterOnDutyStatus ? "在岗" : "不在岗") << " 的医生信息..." << std::endl;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->department == department && current->isOnDuty == filterOnDutyStatus)
        {
            std::string titleStr = doctorTitleToString(current->title);
            std::cout << "医生ID: " << current->getUserID()
                      << ", 姓名: " << current->getUsername()
                      << ",性别: " << current->getGender()
                      << ", 职称: " << titleStr
                      << ", 科室: " << current->department
                      << ", 联系方式: " << current->getTelephone() << " / " << current->getEmail()
                      << ", 擅长领域: " << current->specialty
                      << ", 在岗状态: " << (current->isOnDuty ? "在岗" : "不在岗")
                      << ", 排班信息：" << current->scheduleInfo
                      << ", 累计看诊人数: " << current->consultationCount
                      << ", 累计开具检查次数: " << current->examinationCount
                      << ", 累计开具住院证次数: " << current->hospitalizationApplyCount
                      << std::endl;
            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到在岗状态为 " << (filterOnDutyStatus ? "在岗" : "不在岗") << " 的医生信息！" << std::endl;
    }
    return found;
}
// 修改医生姓名
void Admin::modifyDoctorName(Doctor *&target, const std::string &department)
{
    std::cout << "当前医生姓名: " << target->getUsername() << std::endl;
    std::string newName = inputStringCheck("请输入新的医生姓名: ");
    target->setUsername(newName);
    std::cout << "医生姓名已更新！" << std::endl;
}
// 修改医生性别
void Admin::modifyDoctorGender(Doctor *&target, const std::string &department)
{
    std::cout << "当前医生性别: " << target->getGender() << std::endl;
    std::string newGender = inputGenderCheck("请输入新的医生性别: ");
    target->setGender(newGender);
    std::cout << "医生性别已更新！" << std::endl;
}
// 修改医生的年龄
void Admin::modifyDoctorAge(Doctor *&target, const std::string &department)
{
    std::cout << "当前医生年龄: " << target->getAge() << std::endl;
    int newAge = inputAgeCheck("请输入新的医生年龄: ");
    target->setAge(newAge);
    std::cout << "医生年龄已更新！" << std::endl;
}
// 修改医生所属科室
void Admin::modifyDoctorDepartment(Doctor *&target, const std::string &department)
{
    std::cout << "当前医生所属科室: " << target->department << std::endl;
    std::string newDepartment = adminDepartmentMenu();

    if (newDepartment == "0")
    {
        std::cout << "科室修改已取消！" << std::endl;
        return; // 取消修改，返回上一级菜单
    }
    else if (newDepartment.empty())
    {
        std::cout << "输入无效，科室修改已取消！" << std::endl;
        return; // 输入无效，取消修改，返回上一级菜单
    }

    target->department = newDepartment;
    std::cout << "医生所属科室已更新！" << std::endl;
}
// 修改医生职称
void Admin::modifyDoctorTitle(Doctor *&target, const std::string &department)
{
    std::cout << "当前医生职称: " << doctorTitleToString(target->title) << std::endl;
    int newTitle = DoctorTitleMenu(); // 选择新的医生职称

    if (newTitle == 0)
    {
        std::cout << "职称修改已取消！" << std::endl;
        return; // 取消修改，返回上一级菜单
    }

    target->title = static_cast<DoctorTitle>(newTitle);
    std::cout << "医生职称已更新！" << std::endl;
}
// 修改医生电话
void Admin::modifyDoctorTelephone(Doctor *&target, const std::string &department)
{
    std::cout << "当前医生电话: " << target->getTelephone() << std::endl;
    std::string newTelephone = inputTelephoneCheck("请输入新的医生电话: ");
    target->setTelephone(newTelephone);
    std::cout << "医生电话已更新！" << std::endl;
}
// 修改医生邮箱
void Admin::modifyDoctorEmail(Doctor *&target, const std::string &department)
{
    std::cout << "当前医生邮箱: " << target->getEmail() << std::endl;
    std::string newEmail = inputEmailCheck("请输入新的医生邮箱: ");
    target->setEmail(newEmail);
    std::cout << "医生邮箱已更新！" << std::endl;
}
// 修改医生在岗状态
void Admin::modifyDoctorOnDutyStatus(Doctor *&target, const std::string &department)
{
    std::cout << "当前医生在岗状态: " << (target->isOnDuty ? "在岗" : "不在岗") << std::endl;
    std::cout << "请选择新的在岗状态: " << std::endl;
    std::cout << "1. 在岗" << std::endl;
    std::cout << "2. 不在岗" << std::endl;
    std::cout << "0. 返回上一级菜单" << std::endl;

    int onDutyChoice = selectIntCheck(0, 2);
    if (onDutyChoice == 0)
    {
        std::cout << "在岗状态修改已取消！" << std::endl;
        return;
    }
    target->isOnDuty = (onDutyChoice == 1); // 1表示在岗，2表示不在岗
    std::cout << "医生在岗状态已更新！" << std::endl;
}
// 修改医生擅长领域
void Admin::modifyDoctorSpecialty(Doctor *&target, const std::string &department)
{
    std::cout << "当前医生擅长领域: " << target->specialty << std::endl;
    std::string newSpecialty = inputStringCheck("请输入新的医生擅长领域: ");
    target->specialty = newSpecialty;
    std::cout << "医生擅长领域已更新！" << std::endl;
}
// 修改医生排班信息
void Admin::modifyDoctorScheduleInfo(Doctor *&target, const std::string &department)
{
    std::cout << "当前医生排班信息: " << target->scheduleInfo << std::endl;
    std::string newScheduleInfo = inputStringCheck("请输入新的医生排班信息: ");
    target->scheduleInfo = newScheduleInfo;
    std::cout << "医生排班信息已更新！" << std::endl;
}
// 修改医生累计看诊人数
void Admin::modifyDoctorConsultationCount(Doctor *&target, const std::string &department)
{
    std::cout << "当前医生累计看诊人数: " << target->consultationCount << std::endl;
    std::cout << "输入新的累计看诊人数 ";
    int newConsultationCount = selectIntCheck(0, INT_MAX);
    target->consultationCount = newConsultationCount;
    std::cout << "医生累计看诊人数已更新！" << std::endl;
}
// 修改医生累计开具检查次数
void Admin::modifyDoctorExaminationCount(Doctor *&target, const std::string &department)
{
    std::cout << "当前医生累计开具检查次数: " << target->examinationCount << std::endl;
    std::cout << "输入新的累计开具检查次数 ";
    int newExaminationCount = selectIntCheck(0, INT_MAX);
    target->examinationCount = newExaminationCount;
    std::cout << "医生累计开具检查次数已更新！" << std::endl;
}
// 修改医生累计开具住院证次数
void Admin::modifyDoctorHospitalizationApplyCount(Doctor *&target, const std::string &department)
{
    std::cout << "当前医生累计开具住院证次数: " << target->hospitalizationApplyCount << std::endl;
    std::cout << "输入新的累计开具住院证次数 ";
    int newHospitalizationApplyCount = selectIntCheck(0, INT_MAX);
    target->hospitalizationApplyCount = newHospitalizationApplyCount;
    std::cout << "医生累计开具住院证次数已更新！" << std::endl;
}
// 删除医生（逻辑删除，设置 isDeleted 标志）
void Admin::deleteDoctor(Doctor *&target, const std::string &department)
{
    target->isDeleted = true; // 逻辑删除
    std::cout << "医生ID: " << target->getUserID() << " 已删除！" << std::endl;
}
// 添加医生（根据输入信息创建新的 Doctor 对象，并插入到链表中）
bool Admin::addDoctor(Doctor *&doc, int &idCounter)
{
    Doctor *newDoc = new Doctor();
    bool success = newDoc->doctorSignUp(idCounter);
    if (!success)
    {
        delete newDoc;
        return false;
    }
    // 插入到链表头部
    newDoc->next = doc;
    if (doc != nullptr)
    {
        doc->prev = newDoc;
    }
    doc = newDoc;
    return true;
}

void Admin::manageDoctors(Doctor *&doc, const std::string &department, int &idCounter)
{
    while (true)
    {
        int choice = adminDoctorManagementMenu();

        if (choice == 1) // 查看医生信息
        {
            while (true)
            {
                int viewChoice = adminDoctorViewMenu();
                if (viewChoice == 0) // 返回上一级菜单
                {
                    break;
                }
                else if (viewChoice == 1) // 查看所有医生信息
                {
                    viewAllDoctors(doc, department);
                    pause();
                }
                else if (viewChoice == 2) // 根据医生ID查看医生信息
                {
                    viewDoctorByID(doc, department);
                    pause();
                }
                else if (viewChoice == 3) // 根据医生姓名查看医生信息
                {
                    viewDoctorsByName(doc, department);
                    pause();
                }
                else if (viewChoice == 4) // 根据医生在岗状态查看医生信息
                {
                    viewDoctorsByOnDutyStatus(doc, department);
                    pause();
                }
                else if (viewChoice == 5) // 根据医生职称查看医生信息
                {
                    viewDoctorByTitle(doc, department);
                    pause();
                }
            }
        }
        else if (choice == 2) // 修改医生信息
        {
            bool ishave = viewAllDoctors(doc, department);

            if (ishave)
            {
                std::string doctorID = inputIDCheck("请输入要进行修改的医生ID: ");

                Doctor *target = doc;
                while (target != nullptr)
                {
                    if (!target->isDeleted && target->getUserID() == doctorID && target->department == department)
                    {
                        break;
                    }
                    target = target->next;
                }

                if (target)
                {
                    while (true)
                    {
                        std::cout << "正在修改的医生信息：" << std::endl;

                        std::string titleStr = doctorTitleToString(target->title);
                        std::cout << "医生ID: " << target->getUserID()
                                  << ", 姓名: " << target->getUsername()
                                  << ",性别: " << target->getGender()
                                  << ", 职称: " << titleStr
                                  << ", 科室: " << target->department
                                  << ", 联系方式: " << target->getTelephone() << " / " << target->getEmail()
                                  << ", 擅长领域: " << target->specialty
                                  << ", 在岗状态: " << (target->isOnDuty ? "在岗" : "不在岗")
                                  << ", 排班信息：" << target->scheduleInfo
                                  << ", 累计看诊人数: " << target->consultationCount
                                  << ", 累计开具检查次数: " << target->examinationCount
                                  << ", 累计开具住院证次数: " << target->hospitalizationApplyCount
                                  << std::endl;

                        int modifyChoice = adminDoctorModificationMenu();

                        if (modifyChoice == 0) // 返回上一级菜单
                        {
                            break;
                        }
                        else if (modifyChoice == 1) // 修改医生姓名
                        {
                            modifyDoctorName(doc, department);
                            pause();
                        }
                        else if (modifyChoice == 2) // 修改医生性别
                        {
                            modifyDoctorGender(doc, department);
                            pause();
                        }
                        else if (modifyChoice == 3) // 修改医生年龄
                        {
                            modifyDoctorAge(doc, department);
                            pause();
                        }
                        else if (modifyChoice == 4) // 修改医生所属科室
                        {
                            modifyDoctorDepartment(doc, department);
                            pause();
                        }
                        else if (modifyChoice == 5) // 修改医生职称
                        {
                            modifyDoctorTitle(doc, department);
                            pause();
                        }
                        else if (modifyChoice == 6) // 修改医生电话
                        {
                            modifyDoctorTelephone(doc, department);
                            pause();
                        }
                        else if (modifyChoice == 7) // 修改医生邮箱
                        {
                            modifyDoctorEmail(doc, department);
                            pause();
                        }
                        else if (modifyChoice == 8) // 修改医生在岗状态
                        {
                            modifyDoctorOnDutyStatus(doc, department);
                            pause();
                        }
                        else if (modifyChoice == 9) // 修改医生擅长领域
                        {
                            modifyDoctorSpecialty(doc, department);
                            pause();
                        }
                        else if (modifyChoice == 10) // 修改医生排班信息
                        {
                            modifyDoctorScheduleInfo(doc, department);
                            pause();
                        }
                        else if (modifyChoice == 11) // 修改医生累计看诊人数
                        {
                            modifyDoctorConsultationCount(doc, department);
                            pause();
                        }
                        else if (modifyChoice == 12) // 修改医生累计开具检查次数
                        {
                            modifyDoctorExaminationCount(doc, department);
                            pause();
                        }
                        else if (modifyChoice == 13) // 修改医生累计开具住院证次数
                        {
                            modifyDoctorHospitalizationApplyCount(doc, department);
                            pause();
                        }
                    }
                }
                else
                {
                    std::cout << "未找到ID为 " << doctorID << " 的医生信息！" << std::endl;
                    pause();
                }
            }
        }
        else if (choice == 3) // 删除医生
        {
            bool ishave = viewAllDoctors(doc, department);

            if (ishave)
            {
                std::string doctorID = inputIDCheck("请输入要删除的医生ID: ");

                Doctor *target = doc;
                while (target != nullptr)
                {
                    if (!target->isDeleted && target->getUserID() == doctorID && target->department == department)
                    {
                        break;
                    }
                    target = target->next;
                }

                if (target)
                {
                    std::cout << "正在删除的医生信息如下：" << std::endl;
                    std::string titleStr = doctorTitleToString(target->title);
                    std::cout << "医生ID: " << target->getUserID()
                              << ", 姓名: " << target->getUsername()
                              << ",性别: " << target->getGender()
                              << ", 职称: " << titleStr
                              << ", 科室: " << target->department
                              << ", 联系方式: " << target->getTelephone() << " / " << target->getEmail()
                              << ", 擅长领域: " << target->specialty
                              << ", 在岗状态: " << (target->isOnDuty ? "在岗" : "不在岗")
                              << ", 排班信息：" << target->scheduleInfo
                              << ", 累计看诊人数: " << target->consultationCount
                              << ", 累计开具检查次数: " << target->examinationCount
                              << ", 累计开具住院证次数: " << target->hospitalizationApplyCount
                              << std::endl;

                    std::cout << "确认要删除该医生吗？: " << std::endl;
                    std::cout << "1. 确认" << std::endl;
                    std::cout << "0. 取消" << std::endl;

                    int confirmChoice = selectIntCheck(0, 1);
                    if (confirmChoice == 1)
                    {
                        deleteDoctor(target, department);
                        pause();
                    }
                    else
                    {
                        std::cout << "医生删除已取消！" << std::endl;
                        pause();
                    }
                }
                else
                {
                    std::cout << "未找到ID为 " << doctorID << " 的医生信息！" << std::endl;
                    pause();
                }
            }
        }
        else if (choice == 4) // 添加医生
        {
            addDoctor(doc, idCounter);
            pause();
        }
        else if (choice == 0) // 返回上一级菜单
        {
            break;
        }
    }
}

// =================================== 护士信息管理 =================================

// 查看所有的护士信息
bool Admin::viewAllNurses(Nurse *&nurse, const std::string &department)
{
    Nurse *current = nurse;
    bool found = false;
    std::cout << "正在查找科室 " << department << " 的护士信息..." << std::endl;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->department == department)
        {
            std::string level = nurseLevelToString(current->level);
            std::cout << "护士ID: " << current->getUserID()
                      << ", 姓名: " << current->getUsername()
                      << ",性别: " << current->getGender()
                      << ", 职称: " << level
                      << ", 科室: " << current->department
                      << ", 联系方式: " << current->getTelephone() << " / " << current->getEmail()
                      << ", 排班信息：" << current->scheduleInfo
                      << ", 在岗状态: " << (current->isOnDuty ? "在岗" : "不在岗")
                      << ", 累计护理人数: " << current->patientCareCount
                      << ", 床位管理次数: " << current->bedManageCount
                      << std::endl;
            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "该科室暂无护士信息！" << std::endl;
    }
    return found;
}
// 根据护士ID查看护士信息
bool Admin::viewNurseByID(Nurse *&nurse, const std::string &department)
{
    std::string nurseID = inputIDCheck("请输入要查找的护士ID: ");

    Nurse *current = nurse;
    bool found = false;
    std::cout << "正在查找ID: " << nurseID << " 的护士信息..." << std::endl;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->getUserID() == nurseID && current->department == department)
        {
            std::string level = nurseLevelToString(current->level);
            std::cout << "护士ID: " << current->getUserID()
                      << ", 姓名: " << current->getUsername()
                      << ",性别: " << current->getGender()
                      << ", 职称: " << level
                      << ", 科室: " << current->department
                      << ", 联系方式: " << current->getTelephone() << " / " << current->getEmail()
                      << ", 排班信息：" << current->scheduleInfo
                      << ", 在岗状态: " << (current->isOnDuty ? "在岗" : "不在岗")
                      << ", 累计护理人数: " << current->patientCareCount
                      << ", 床位管理次数: " << current->bedManageCount
                      << std::endl;
            found = true;
            break;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到ID为 " << nurseID << " 的护士信息！" << std::endl;
    }
    return found;
}
// 根据护士姓名查看护士信息
bool Admin::viewNursesByName(Nurse *&nurse, const std::string &department)
{
    std::string name = inputStringCheck("请输入护士姓名（支持模糊查询）: ");

    Nurse *current = nurse;
    bool found = false;
    std::cout << "正在查找姓名包含 \"" << name << "\" 的护士信息..." << std::endl;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->getUsername().find(name) != std::string::npos && current->department == department)
        {
            std::string level = nurseLevelToString(current->level);

            std::cout << "护士ID: " << current->getUserID()
                      << ", 姓名: " << current->getUsername()
                      << ",性别: " << current->getGender()
                      << ", 职称: " << level
                      << ", 科室: " << current->department
                      << ", 联系方式: " << current->getTelephone() << " / " << current->getEmail()
                      << ", 排班信息：" << current->scheduleInfo
                      << ", 在岗状态: " << (current->isOnDuty ? "在岗" : "不在岗")
                      << ", 累计护理人数: " << current->patientCareCount
                      << ", 床位管理次数: " << current->bedManageCount
                      << std::endl;
            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到姓名包含 \"" << name << "\" 的护士信息！" << std::endl;
    }
    return found;
}
// 根据护士在岗状态查看护士信息
bool Admin::viewNursesByOnDutyStatus(Nurse *&nurse, const std::string &department)
{

    bool found = false;

    std::cout << "请选择护士在岗状态过滤条件: " << std::endl;
    std::cout << "1. 在岗" << std::endl;
    std::cout << "2. 不在岗" << std::endl;
    std::cout << "0. 返回上一级菜单" << std::endl;
    int onDutyFilter = selectIntCheck(0, 2);

    if (onDutyFilter == 0)
    {
        return false; // 返回上一级菜单
    }

    bool filterOnDutyStatus = (onDutyFilter == 1); // 1表示在岗，2表示不在岗

    Nurse *current = nurse;
    std::cout << "正在查找在岗状态为: " << (filterOnDutyStatus ? "在岗" : "不在岗") << " 的护士信息..." << std::endl;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->department == department && current->isOnDuty == filterOnDutyStatus)
        {
            std::string level = nurseLevelToString(current->level);
            std::cout << "护士ID: " << current->getUserID()
                      << ", 姓名: " << current->getUsername()
                      << ",性别: " << current->getGender()
                      << ", 职称: " << level
                      << ", 科室: " << current->department
                      << ", 联系方式: " << current->getTelephone() << " / " << current->getEmail()
                      << ", 排班信息：" << current->scheduleInfo
                      << ", 在岗状态: " << (current->isOnDuty ? "在岗" : "不在岗")
                      << ", 累计护理人数: " << current->patientCareCount
                      << ", 床位管理次数: " << current->bedManageCount
                      << std::endl;
            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到在岗状态为 " << (filterOnDutyStatus ? "在岗" : "不在岗") << " 的护士信息！" << std::endl;
    }
    return found;
}
// 根据护士等级查看护士信息
bool Admin::viewNurseByLevel(Nurse *&nurse, const std::string &department)
{
    int levelFilter = NurseTitleMenu(); // 选择护士职称过滤条件

    if (levelFilter == 0)
    {
        return false; // 返回上一级菜单
    }

    bool found = false;

    NurseLevel filterLevel = static_cast<NurseLevel>(levelFilter);

    Nurse *current = nurse;
    std::cout << "正在查找等级为: " << nurseLevelToString(filterLevel) << " 的护士信息..." << std::endl;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->department == department && current->level == filterLevel)
        {
            std::cout << "护士ID: " << current->getUserID()
                      << ", 姓名: " << current->getUsername()
                      << ",性别: " << current->getGender()
                      << ", 职称: " << nurseLevelToString(current->level)
                      << ", 科室: " << current->department
                      << ", 联系方式: " << current->getTelephone() << " / " << current->getEmail()
                      << ", 排班信息：" << current->scheduleInfo
                      << ", 在岗状态: " << (current->isOnDuty ? "在岗" : "不在岗")
                      << ", 累计护理人数: " << current->patientCareCount
                      << ", 床位管理次数: " << current->bedManageCount
                      << std::endl;
            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到等级为 " << nurseLevelToString(filterLevel) << " 的护士信息！" << std::endl;
    }
    return found;
}
// 修改护士姓名
void Admin::modifyNurseName(Nurse *&target, const std::string &department)
{
    std::cout << "当前护士姓名: " << target->getUsername() << std::endl;
    std::string newName = inputStringCheck("请输入新的护士姓名: ");
    target->setUsername(newName);
    std::cout << "护士姓名已更新！" << std::endl;
}
// 修改护士性别
void Admin::modifyNurseGender(Nurse *&target, const std::string &department)
{
    std::cout << "当前护士性别: " << target->getGender() << std::endl;
    std::string newGender = inputGenderCheck("请输入新的护士性别: ");
    target->setGender(newGender);
    std::cout << "护士性别已更新！" << std::endl;
}
// 修改护士年龄
void Admin::modifyNurseAge(Nurse *&target, const std::string &department)
{
    std::cout << "当前护士年龄: " << target->getAge() << std::endl;
    int newAge = inputAgeCheck("请输入新的护士年龄: ");
    target->setAge(newAge);
    std::cout << "护士年龄已更新！" << std::endl;
}
// 修改护士所属科室
void Admin::modifyNurseDepartment(Nurse *&target, const std::string &department)
{
    std::cout << "当前护士所属科室: " << target->department << std::endl;
    std::string newDepartment = adminDepartmentMenu();

    if (newDepartment == "0")
    {
        std::cout << "科室修改已取消！" << std::endl;
        return; // 取消修改，返回上一级菜单
    }
    else if (newDepartment.empty())
    {
        std::cout << "输入无效，科室修改已取消！" << std::endl;
        return; // 输入无效，取消修改，返回上一级菜单
    }

    target->department = newDepartment;
    std::cout << "护士所属科室已更新！" << std::endl;
}
// 修改护士等级
void Admin::modifyNurseLevel(Nurse *&target, const std::string &department)
{
    std::cout << "当前护士等级: " << nurseLevelToString(target->level) << std::endl;
    int newLevel = NurseTitleMenu(); // 选择新的护士等级

    if (newLevel == 0)
    {
        std::cout << "等级修改已取消！" << std::endl;
        return; // 取消修改，返回上一级菜单
    }

    target->level = static_cast<NurseLevel>(newLevel);
    std::cout << "护士等级已更新！" << std::endl;
}
// 修改护士电话
void Admin::modifyNurseTelephone(Nurse *&target, const std::string &department)
{
    std::cout << "当前护士电话: " << target->getTelephone() << std::endl;
    std::string newTelephone = inputTelephoneCheck("请输入新的护士电话: ");
    target->setTelephone(newTelephone);
    std::cout << "护士电话已更新！" << std::endl;
}
// 修改护士邮箱
void Admin::modifyNurseEmail(Nurse *&target, const std::string &department)
{
    std::cout << "当前护士邮箱: " << target->getEmail() << std::endl;
    std::string newEmail = inputEmailCheck("请输入新的护士邮箱: ");
    target->setEmail(newEmail);
    std::cout << "护士邮箱已更新！" << std::endl;
}
// 修改护士在岗状态
void Admin::modifyNurseOnDutyStatus(Nurse *&target, const std::string &department)
{
    std::cout << "当前护士在岗状态: " << (target->isOnDuty ? "在岗" : "不在岗") << std::endl;
    std::cout << "请选择新的在岗状态: " << std::endl;
    std::cout << "1. 在岗" << std::endl;
    std::cout << "2. 不在岗" << std::endl;
    std::cout << "0. 返回上一级菜单" << std::endl;

    int onDutyChoice = selectIntCheck(0, 2);
    if (onDutyChoice == 0)
    {
        std::cout << "在岗状态修改已取消！" << std::endl;
        return;
    }
    target->isOnDuty = (onDutyChoice == 1); // 1表示在岗，2表示不在岗
    std::cout << "护士在岗状态已更新！" << std::endl;
}
// 修改护士排班信息
void Admin::modifyNurseScheduleInfo(Nurse *&target, const std::string &department)
{
    std::cout << "当前护士排班信息: " << target->scheduleInfo << std::endl;
    std::string newScheduleInfo = inputStringCheck("请输入新的护士排班信息: ");
    target->scheduleInfo = newScheduleInfo;
    std::cout << "护士排班信息已更新！" << std::endl;
}
// 修改护士累计护理人数
void Admin::modifyNursePatientCareCount(Nurse *&target, const std::string &department)
{
    std::cout << "当前护士累计护理人数: " << target->patientCareCount << std::endl;
    std::cout << "输入新的累计护理人数 ";
    int newPatientCareCount = selectIntCheck(0, INT_MAX);
    target->patientCareCount = newPatientCareCount;
    std::cout << "护士累计护理人数已更新！" << std::endl;
}
// 修改护士床位管理次数
void Admin::modifyNurseBedManageCount(Nurse *&target, const std::string &department)
{
    std::cout << "当前护士床位管理次数: " << target->bedManageCount << std::endl;
    std::cout << "输入新的床位管理次数 ";
    int newBedManageCount = selectIntCheck(0, INT_MAX);
    target->bedManageCount = newBedManageCount;
    std::cout << "护士床位管理次数已更新！" << std::endl;
}
// 删除护士（逻辑删除，设置 isDeleted 标志）
void Admin::deleteNurse(Nurse *&target, const std::string &department)
{
    target->isDeleted = true; // 逻辑删除
    std::cout << "护士ID: " << target->getUserID() << " 已删除！" << std::endl;
}
// 添加护士（根据输入信息创建新的 Nurse 对象，并插入到链表中）
bool Admin::addNurse(Nurse *&nurse, int &idCounter)
{
    Nurse *newNurse = new Nurse();
    bool success = newNurse->nurseSignUp(idCounter);
    if (!success)
    {
        delete newNurse;
        return false;
    }
    // 插入到链表头部
    newNurse->next = nurse;
    if (nurse != nullptr)
    {
        nurse->prev = newNurse;
    }
    nurse = newNurse;
    return true;
}

void Admin::manageNurses(Nurse *&nurse, const std::string &department, int &idCounter)
{
    while (true)
    {
        int choice = adminNurseManagementMenu();

        if (choice == 1) // 查看护士信息
        {
            while (true)
            {
                int viewChoice = adminNurseViewMenu();
                if (viewChoice == 0) // 返回上一级菜单
                {
                    break;
                }
                else if (viewChoice == 1) // 查看所有护士信息
                {
                    viewAllNurses(nurse, department);
                    pause();
                }
                else if (viewChoice == 2) // 根据护士ID查看护士信息
                {
                    viewNurseByID(nurse, department);
                    pause();
                }
                else if (viewChoice == 3) // 根据护士姓名查看护士信息
                {
                    viewNursesByName(nurse, department);
                    pause();
                }
                else if (viewChoice == 4) // 根据护士在岗状态查看护士信息
                {
                    viewNursesByOnDutyStatus(nurse, department);
                    pause();
                }
                else if (viewChoice == 5) // 根据护士等级查看护士信息
                {
                    viewNurseByLevel(nurse, department);
                    pause();
                }
            }
        }
        else if (choice == 2) // 修改护士信息
        {
            bool ishave = viewAllNurses(nurse, department);

            if (ishave)
            {
                std::string nurseID = inputIDCheck("请输入要修改的护士ID: ");

                Nurse *target = nurse;
                while (target != nullptr)
                {
                    if (!target->isDeleted && target->getUserID() == nurseID && target->department == department)
                    {
                        break;
                    }
                    target = target->next;
                }

                if (target)
                {
                    while (true)
                    {
                        std::cout << "正在修改的护士信息：" << std::endl;
                        std::cout << "护士ID: " << target->getUserID()
                                  << ", 姓名: " << target->getUsername()
                                  << ",性别: " << target->getGender()
                                  << ", 职称: " << nurseLevelToString(target->level)
                                  << ", 科室: " << target->department
                                  << ", 联系方式: " << target->getTelephone() << " / " << target->getEmail()
                                  << ", 排班信息：" << target->scheduleInfo
                                  << ", 在岗状态: " << (target->isOnDuty ? "在岗" : "不在岗")
                                  << ", 累计护理人数: " << target->patientCareCount
                                  << ", 床位管理次数: " << target->bedManageCount
                                  << std::endl;

                        int modifyChoice = adminNurseModificationMenu();

                        if (modifyChoice == 0) // 返回上一级菜单
                        {
                            break;
                        }
                        else if (modifyChoice == 1) // 修改护士姓名
                        {
                            modifyNurseName(nurse, department);
                            pause();
                        }
                        else if (modifyChoice == 2) // 修改护士性别
                        {
                            modifyNurseGender(nurse, department);
                            pause();
                        }
                        else if (modifyChoice == 3) // 修改护士年龄
                        {
                            modifyNurseAge(nurse, department);
                            pause();
                        }
                        else if (modifyChoice == 4) // 修改护士所属科室
                        {
                            modifyNurseDepartment(nurse, department);
                            pause();
                        }
                        else if (modifyChoice == 5) // 修改护士等级
                        {
                            modifyNurseLevel(nurse, department);
                            pause();
                        }
                        else if (modifyChoice == 6) // 修改护士电话
                        {
                            modifyNurseTelephone(nurse, department);
                            pause();
                        }
                        else if (modifyChoice == 7) // 修改护士邮箱
                        {
                            modifyNurseEmail(nurse, department);
                            pause();
                        }
                        else if (modifyChoice == 8) // 修改护士在岗状态
                        {
                            modifyNurseOnDutyStatus(nurse, department);
                            pause();
                        }
                        else if (modifyChoice == 9) // 修改护士排班信息
                        {
                            modifyNurseScheduleInfo(nurse, department);
                            pause();
                        }
                        else if (modifyChoice == 10) // 修改护士累计护理人数
                        {
                            modifyNursePatientCareCount(nurse, department);
                            pause();
                        }
                        else if (modifyChoice == 11) // 修改护士床位管理次数
                        {
                            modifyNurseBedManageCount(nurse, department);
                            pause();
                        }
                    }
                }
                else
                {
                    std::cout << "未找到ID为 " << nurseID << " 的护士信息！" << std::endl;
                    pause();
                }
            }
        }
        else if (choice == 3) // 删除护士
        {
            bool ishave = viewAllNurses(nurse, department);

            if (ishave)
            {
                std::string nurseID = inputIDCheck("请输入要删除的护士ID: ");

                Nurse *target = nurse;
                while (target != nullptr)
                {
                    if (!target->isDeleted && target->getUserID() == nurseID && target->department == department)
                    {
                        break;
                    }
                    target = target->next;
                }

                if (target)
                {
                    std::cout << "正在删除的护士信息如下：" << std::endl;
                    std::cout << "护士ID: " << target->getUserID()
                              << ", 姓名: " << target->getUsername()
                              << ",性别: " << target->getGender()
                              << ", 职称: " << nurseLevelToString(target->level)
                              << ", 科室: " << target->department
                              << ", 联系方式: " << target->getTelephone() << " / " << target->getEmail()
                              << ", 排班信息：" << target->scheduleInfo
                              << ", 在岗状态: " << (target->isOnDuty ? "在岗" : "不在岗")
                              << ", 累计护理人数: " << target->patientCareCount
                              << ", 床位管理次数: " << target->bedManageCount
                              << std::endl;

                    std::cout << "确认要删除该护士吗？: " << std::endl;
                    std::cout << "1. 确认" << std::endl;
                    std::cout << "0. 取消" << std::endl;

                    int confirmChoice = selectIntCheck(0, 1);
                    if (confirmChoice == 1)
                    {

                        deleteNurse(target, department);
                        pause();
                    }
                    else
                    {
                        std::cout << "护士删除已取消！" << std::endl;
                        pause();
                    }
                }
                else
                {
                    std::cout << "未找到ID为 " << nurseID << " 的护士信息！" << std::endl;
                    pause();
                }
            }
        }
        else if (choice == 4) // 添加护士
        {
            addNurse(nurse, idCounter);
            pause();
        }
        else if (choice == 0) // 返回上一级菜单
        {
            break;
        }
    }
}

// =================================== 药剂师信息管理 =================================

// 查看所有的药剂师信息
bool Admin::viewAllPharmacists(Pharmacist *&pharmacist, const std::string &department)
{
    Pharmacist *current = pharmacist;
    bool found = false;
    std::cout << "正在查找科室 " << department << " 的药剂师信息..." << std::endl;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->department == department)
        {
            std::cout << "药剂师ID: " << current->getUserID()
                      << ", 姓名: " << current->getUsername()
                      << ",性别: " << current->getGender()
                      << ", 科室: " << current->department
                      << ", 职称: " << pharmacistLevelToString(current->level)
                      << ", 排班信息：" << current->scheduleInfo
                      << ", 联系方式: " << current->getTelephone() << " / " << current->getEmail()
                      << ", 在岗状态: " << (current->isOnDuty ? "在岗" : "不在岗")
                      << ", 累计审核用药记录数量: " << current->reviewCount
                      << ", 累计发药数量: " << current->dispenseCount
                      << ", 累计库存管理次数: " << current->inventoryManageCount
                      << std::endl;
            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "该科室暂无药剂师信息！" << std::endl;
    }
    return found;
}
// 根据药剂师ID查看药剂师信息
bool Admin::viewPharmacistByID(Pharmacist *&pharmacist, const std::string &department)
{
    std::string pharmacistID = inputIDCheck("请输入要查找的药剂师ID: ");

    Pharmacist *current = pharmacist;
    bool found = false;
    std::cout << "正在查找ID: " << pharmacistID << " 的药剂师信息..." << std::endl;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->getUserID() == pharmacistID && current->department == department)
        {
            std::cout << "药剂师ID: " << current->getUserID()
                      << ", 姓名: " << current->getUsername()
                      << ",性别: " << current->getGender()
                      << ", 科室: " << current->department
                      << ", 职称: " << pharmacistLevelToString(current->level)
                      << ", 排班信息：" << current->scheduleInfo
                      << ", 联系方式: " << current->getTelephone() << " / " << current->getEmail()
                      << ", 在岗状态: " << (current->isOnDuty ? "在岗" : "不在岗")
                      << ", 累计审核用药记录数量: " << current->reviewCount
                      << ", 累计发药数量: " << current->dispenseCount
                      << ", 累计库存管理次数: " << current->inventoryManageCount
                      << std::endl;
            found = true;
            break;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到ID为 " << pharmacistID << " 的药剂师信息！" << std::endl;
    }
    return found;
}
// 根据药剂师姓名查看药剂师信息
bool Admin::viewPharmacistsByName(Pharmacist *&pharmacist, const std::string &department)
{
    std::string name = inputStringCheck("请输入药剂师姓名（支持模糊查询）: ");

    Pharmacist *current = pharmacist;
    bool found = false;
    std::cout << "正在查找姓名包含 \"" << name << "\" 的药剂师信息..." << std::endl;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->getUsername().find(name) != std::string::npos && current->department == department)
        {
            std::cout << "药剂师ID: " << current->getUserID()
                      << ", 姓名: " << current->getUsername()
                      << ",性别: " << current->getGender()
                      << ", 科室: " << current->department
                      << ", 职称: " << pharmacistLevelToString(current->level)
                      << ", 排班信息：" << current->scheduleInfo
                      << ", 联系方式: " << current->getTelephone() << " / " << current->getEmail()
                      << ", 在岗状态: " << (current->isOnDuty ? "在岗" : "不在岗")
                      << ", 累计审核用药记录数量: " << current->reviewCount
                      << ", 累计发药数量: " << current->dispenseCount
                      << ", 累计库存管理次数: " << current->inventoryManageCount
                      << std::endl;
            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到姓名包含 \"" << name << "\" 的药剂师信息！" << std::endl;
    }
    return found;
}
// 根据药剂师在岗状态查看药剂师信息
bool Admin::viewPharmacistsByOnDutyStatus(Pharmacist *&pharmacist, const std::string &department)
{

    bool found = false;

    std::cout << "请选择药剂师在岗状态过滤条件: " << std::endl;
    std::cout << "1. 在岗" << std::endl;
    std::cout << "2. 不在岗" << std::endl;
    std::cout << "0. 返回上一级菜单" << std::endl;
    int onDutyFilter = selectIntCheck(0, 2);

    if (onDutyFilter == 0)
    {
        return false; // 返回上一级菜单
    }

    bool filterOnDutyStatus = (onDutyFilter == 1); // 1表示在岗，2表示不在岗

    Pharmacist *current = pharmacist;
    std::cout << "正在查找在岗状态为: " << (filterOnDutyStatus ? "在岗" : "不在岗") << " 的药剂师信息..." << std::endl;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->department == department && current->isOnDuty == filterOnDutyStatus)
        {
            std::cout << "药剂师ID: " << current->getUserID()
                      << ", 姓名: " << current->getUsername()
                      << ",性别: " << current->getGender()
                      << ", 科室: " << current->department
                      << ", 职称: " << pharmacistLevelToString(current->level)
                      << ", 排班信息：" << current->scheduleInfo
                      << ", 联系方式: " << current->getTelephone() << " / " << current->getEmail()
                      << ", 在岗状态: " << (current->isOnDuty ? "在岗" : "不在岗")
                      << ", 累计审核用药记录数量: " << current->reviewCount
                      << ", 累计发药数量: " << current->dispenseCount
                      << ", 累计库存管理次数: " << current->inventoryManageCount
                      << std::endl;
            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到在岗状态为 " << (filterOnDutyStatus ? "在岗" : "不在岗") << " 的药剂师信息！" << std::endl;
    }
    return found;
}
// 根据药剂师职称查看药剂师信息
bool Admin::viewPharmacistByLevel(Pharmacist *&pharmacist, const std::string &department)
{
    int levelFilter = PharmacistTitleMenu(); // 选择药剂师职称过滤条件

    if (levelFilter == 0)
    {
        return false; // 返回上一级菜单
    }

    bool found = false;

    PharmacistLevel filterLevel = static_cast<PharmacistLevel>(levelFilter);

    Pharmacist *current = pharmacist;
    std::cout << "正在查找职称为: " << pharmacistLevelToString(filterLevel) << " 的药剂师信息..." << std::endl;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->department == department && current->level == filterLevel)
        {
            std::cout << "药剂师ID: " << current->getUserID()
                      << ", 姓名: " << current->getUsername()
                      << ",性别: " << current->getGender()
                      << ", 科室: " << current->department
                      << ", 职称: " << pharmacistLevelToString(current->level)
                      << ", 排班信息：" << current->scheduleInfo
                      << ", 联系方式: " << current->getTelephone() << " / " << current->getEmail()
                      << ", 在岗状态: " << (current->isOnDuty ? "在岗" : "不在岗")
                      << ", 累计审核用药记录数量: " << current->reviewCount
                      << ", 累计发药数量: " << current->dispenseCount
                      << ", 累计库存管理次数: " << current->inventoryManageCount
                      << std::endl;
            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到职称为 " << pharmacistLevelToString(filterLevel) << " 的药剂师信息！" << std::endl;
    }
    return found;
}
// 修改药剂师姓名
void Admin::modifyPharmacistName(Pharmacist *&target, const std::string &department)
{
    std::cout << "当前药剂师姓名: " << target->getUsername() << std::endl;
    std::string newName = inputStringCheck("请输入新的药剂师姓名: ");
    target->setUsername(newName);
    std::cout << "药剂师姓名已更新！" << std::endl;
}
// 修改药剂师性别
void Admin::modifyPharmacistGender(Pharmacist *&target, const std::string &department)
{
    std::cout << "当前药剂师性别: " << target->getGender() << std::endl;
    std::string newGender = inputGenderCheck("请输入新的药剂师性别: ");
    target->setGender(newGender);
    std::cout << "药剂师性别已更新！" << std::endl;
}
// 修改药剂师年龄
void Admin::modifyPharmacistAge(Pharmacist *&target, const std::string &department)
{
    std::cout << "当前药剂师年龄: " << target->getAge() << std::endl;
    int newAge = inputAgeCheck("请输入新的药剂师年龄: ");
    target->setAge(newAge);
    std::cout << "药剂师年龄已更新！" << std::endl;
}
// 修改药剂师所属科室
void Admin::modifyPharmacistDepartment(Pharmacist *&target, const std::string &department)
{
    std::cout << "当前药剂师所属科室: " << target->department << std::endl;
    std::string newDepartment = adminDepartmentMenu();

    if (newDepartment == "0")
    {
        std::cout << "科室修改已取消！" << std::endl;
        return; // 取消修改，返回上一级菜单
    }
    else if (newDepartment.empty())
    {
        std::cout << "输入无效，科室修改已取消！" << std::endl;
        return; // 输入无效，取消修改，返回上一级菜单
    }

    target->department = newDepartment;
    std::cout << "药剂师所属科室已更新！" << std::endl;
}
// 修改药剂师职称
void Admin::modifyPharmacistLevel(Pharmacist *&target, const std::string &department)
{
    std::cout << "当前药剂师职称: " << pharmacistLevelToString(target->level) << std::endl;
    int newLevel = PharmacistTitleMenu(); // 选择新的药剂师职称

    if (newLevel == 0)
    {
        std::cout << "职称修改已取消！" << std::endl;
        return; // 取消修改，返回上一级菜单
    }

    target->level = static_cast<PharmacistLevel>(newLevel);
    std::cout << "药剂师职称已更新！" << std::endl;
}
// 修改药剂师电话
void Admin::modifyPharmacistTelephone(Pharmacist *&target, const std::string &department)
{
    std::cout << "当前药剂师电话: " << target->getTelephone() << std::endl;
    std::string newTelephone = inputTelephoneCheck("请输入新的药剂师电话: ");
    target->setTelephone(newTelephone);
    std::cout << "药剂师电话已更新！" << std::endl;
}
// 修改药剂师邮箱
void Admin::modifyPharmacistEmail(Pharmacist *&target, const std::string &department)
{
    std::cout << "当前药剂师邮箱: " << target->getEmail() << std::endl;
    std::string newEmail = inputEmailCheck("请输入新的药剂师邮箱: ");
    target->setEmail(newEmail);
    std::cout << "药剂师邮箱已更新！" << std::endl;
}
// 修改药剂师在岗状态
void Admin::modifyPharmacistOnDutyStatus(Pharmacist *&target, const std::string &department)
{
    std::cout << "当前药剂师在岗状态: " << (target->isOnDuty ? "在岗" : "不在岗") << std::endl;
    std::cout << "请选择新的在岗状态: " << std::endl;
    std::cout << "1. 在岗" << std::endl;
    std::cout << "2. 不在岗" << std::endl;
    std::cout << "0. 返回上一级菜单" << std::endl;

    int onDutyChoice = selectIntCheck(0, 2);
    if (onDutyChoice == 0)
    {
        std::cout << "在岗状态修改已取消！" << std::endl;
        return;
    }
    target->isOnDuty = (onDutyChoice == 1); // 1表示在岗，2表示不在岗
    std::cout << "药剂师在岗状态已更新！" << std::endl;
}
// 修改药剂师排班信息
void Admin::modifyPharmacistScheduleInfo(Pharmacist *&target, const std::string &department)
{
    std::cout << "当前药剂师排班信息: " << target->scheduleInfo << std::endl;
    std::string newScheduleInfo = inputStringCheck("请输入新的药剂师排班信息: ");
    target->scheduleInfo = newScheduleInfo;
    std::cout << "药剂师排班信息已更新！" << std::endl;
}
// 修改药剂师累计审核用药记录数量
void Admin::modifyPharmacistMedicationReviewCount(Pharmacist *&target, const std::string &department)
{
    std::cout << "当前药剂师累计审核用药记录数量: " << target->reviewCount << std::endl;
    std::cout << "输入新的累计审核用药记录数量 ";
    int newReviewCount = selectIntCheck(0, INT_MAX);
    target->reviewCount = newReviewCount;
    std::cout << "药剂师累计审核用药记录数量已更新！" << std::endl;
}
// 修改药剂师累计发药数量
void Admin::modifyPharmacistDispenseCount(Pharmacist *&target, const std::string &department)
{
    std::cout << "当前药剂师累计发药数量: " << target->dispenseCount << std::endl;
    std::cout << "输入新的累计发药数量 ";
    int newDispenseCount = selectIntCheck(0, INT_MAX);
    target->dispenseCount = newDispenseCount;
    std::cout << "药剂师累计发药数量已更新！" << std::endl;
}
// 修改药剂师累计库存管理次数
void Admin::modifyPharmacistInventoryManageCount(Pharmacist *&target, const std::string &department)
{
    std::cout << "当前药剂师累计库存管理次数: " << target->inventoryManageCount << std::endl;
    std::cout << "输入新的累计库存管理次数 ";
    int newInventoryManageCount = selectIntCheck(0, INT_MAX);
    target->inventoryManageCount = newInventoryManageCount;
    std::cout << "药剂师累计库存管理次数已更新！" << std::endl;
}
// 删除药剂师（逻辑删除，设置 isDeleted 标志）
void Admin::deletePharmacist(Pharmacist *&target, const std::string &department)
{
    target->isDeleted = true; // 逻辑删除
    std::cout << "药剂师ID: " << target->getUserID() << " 已删除！" << std::endl;
}
// 添加药剂师（根据输入信息创建新的 Pharmacist 对象，并插入到链表中）
bool Admin::addPharmacist(Pharmacist *&pharmacist, int &idCounter)
{
    Pharmacist *newPharmacist = new Pharmacist();
    bool success = newPharmacist->pharmacistSignUp(idCounter);
    if (!success)
    {
        delete newPharmacist;
        return false;
    }
    // 插入到链表头部
    newPharmacist->next = pharmacist;
    if (pharmacist != nullptr)
    {
        pharmacist->prev = newPharmacist;
    }
    pharmacist = newPharmacist;
    return true;
}

void Admin::managePharmacists(Pharmacist *&pharmacist, const std::string &department, int &idCounter)
{
    while (true)
    {
        int choice = adminPharmacistManagementMenu();

        if (choice == 1) // 查看药剂师信息
        {
            while (true)
            {
                int viewChoice = adminPharmacistViewMenu();
                if (viewChoice == 0) // 返回上一级菜单
                {
                    break;
                }
                else if (viewChoice == 1) // 查看所有药剂师信息
                {
                    viewAllPharmacists(pharmacist, department);
                    pause();
                }
                else if (viewChoice == 2) // 根据药剂师ID查看药剂师信息
                {
                    viewPharmacistByID(pharmacist, department);
                    pause();
                }
                else if (viewChoice == 3) // 根据药剂师姓名查看药剂师信息
                {
                    viewPharmacistsByName(pharmacist, department);
                    pause();
                }
                else if (viewChoice == 4) // 根据药剂师在岗状态查看药剂师信息
                {
                    viewPharmacistsByOnDutyStatus(pharmacist, department);
                    pause();
                }
                else if (viewChoice == 5) // 根据药剂师职称查看药剂师信息
                {
                    viewPharmacistByLevel(pharmacist, department);
                    pause();
                }
            }
        }
        else if (choice == 2) // 修改药剂师信息
        {
            bool ishave = viewAllPharmacists(pharmacist, department);

            if (ishave)
            {
                std::string pharmacistID = inputIDCheck("请输入要修改的药剂师ID: ");

                Pharmacist *target = pharmacist;
                while (target != nullptr)
                {
                    if (!target->isDeleted && target->getUserID() == pharmacistID && target->department == department)
                    {
                        break;
                    }
                    target = target->next;
                }

                if (target)
                {
                    while (true)
                    {
                        std::cout << "正在修改的药剂师信息：" << std::endl;
                        std::cout << "药剂师ID: " << target->getUserID()
                                  << ", 姓名: " << target->getUsername()
                                  << ",性别: " << target->getGender()
                                  << ", 科室: " << target->department
                                  << ", 职称: " << pharmacistLevelToString(target->level)
                                  << ", 排班信息：" << target->scheduleInfo
                                  << ", 联系方式: " << target->getTelephone() << " / " << target->getEmail()
                                  << ", 在岗状态: " << (target->isOnDuty ? "在岗" : "不在岗")
                                  << ", 累计审核用药记录数量: " << target->reviewCount
                                  << ", 累计发药数量: " << target->dispenseCount
                                  << ", 累计库存管理次数: " << target->inventoryManageCount
                                  << std::endl;

                        int modifyChoice = adminPharmacistModificationMenu();

                        if (modifyChoice == 0) // 返回上一级菜单
                        {
                            break;
                        }
                        else if (modifyChoice == 1) // 修改药剂师姓名
                        {
                            modifyPharmacistName(pharmacist, department);
                            pause();
                        }
                        else if (modifyChoice == 2) // 修改药剂师性别
                        {
                            modifyPharmacistGender(pharmacist, department);
                            pause();
                        }
                        else if (modifyChoice == 3) // 修改药剂师年龄
                        {
                            modifyPharmacistAge(pharmacist, department);
                            pause();
                        }
                        else if (modifyChoice == 4) // 修改药剂师所属科室
                        {
                            modifyPharmacistDepartment(pharmacist, department);
                            pause();
                        }
                        else if (modifyChoice == 5) // 修改药剂师职称
                        {
                            modifyPharmacistLevel(pharmacist, department);
                            pause();
                        }
                        else if (modifyChoice == 6) // 修改药剂师电话
                        {
                            modifyPharmacistTelephone(pharmacist, department);
                            pause();
                        }
                        else if (modifyChoice == 7) // 修改药剂师邮箱
                        {
                            modifyPharmacistEmail(pharmacist, department);
                            pause();
                        }
                        else if (modifyChoice == 8) // 修改药剂师在岗状态
                        {
                            modifyPharmacistOnDutyStatus(pharmacist, department);
                            pause();
                        }
                        else if (modifyChoice == 9) // 修改药剂师排班信息
                        {
                            modifyPharmacistScheduleInfo(pharmacist, department);
                            pause();
                        }
                        else if (modifyChoice == 10) // 修改药剂师累计审核用药记录数量
                        {
                            modifyPharmacistMedicationReviewCount(pharmacist, department);
                            pause();
                        }
                        else if (modifyChoice == 11) // 修改药剂师累计发药数量
                        {
                            modifyPharmacistDispenseCount(pharmacist, department);
                            pause();
                        }
                        else if (modifyChoice == 12) // 修改药剂师累计库存管理次数
                        {
                            modifyPharmacistInventoryManageCount(pharmacist, department);
                            pause();
                        }
                    }
                }
                else
                {
                    std::cout << "未找到ID为 " << pharmacistID << " 的药剂师信息！" << std::endl;
                    pause();
                }
            }
        }
        else if (choice == 3) // 删除药剂师
        {
            bool ishave = viewAllPharmacists(pharmacist, department);

            if (ishave)
            {
                std::string pharmacistID = inputIDCheck("请输入要删除的药剂师ID: ");

                Pharmacist *target = pharmacist;
                while (target != nullptr)
                {
                    if (!target->isDeleted && target->getUserID() == pharmacistID && target->department == department)
                    {
                        break;
                    }
                    target = target->next;
                }

                if (target)
                {
                    std::cout << "正在删除的药剂师信息如下：" << std::endl;
                    std::cout << "药剂师ID: " << target->getUserID()
                              << ", 姓名: " << target->getUsername()
                              << ",性别: " << target->getGender()
                              << ", 科室: " << target->department
                              << ", 职称: " << pharmacistLevelToString(target->level)
                              << ", 排班信息：" << target->scheduleInfo
                              << ", 联系方式: " << target->getTelephone() << " / " << target->getEmail()
                              << ", 在岗状态: " << (target->isOnDuty ? "在岗" : "不在岗")
                              << ", 累计审核用药记录数量: " << target->reviewCount
                              << ", 累计发药数量: " << target->dispenseCount
                              << ", 累计库存管理次数: " << target->inventoryManageCount
                              << std::endl;

                    std::cout << "确定要删除该药剂师吗？: ";
                    std::cout << "1. 确定" << std::endl;
                    std::cout << "0. 取消" << std::endl;

                    int confirmChoice = selectIntCheck(0, 1);

                    if (confirmChoice == 1)
                    {
                        deletePharmacist(pharmacist, department);
                        pause();
                    }
                    else if (confirmChoice == 0)
                    {
                        std::cout << "药剂师删除已取消！" << std::endl;
                        pause();
                    }
                }
                else
                {
                    std::cout << "未找到ID为 " << pharmacistID << " 的药剂师信息！" << std::endl;
                    pause();
                }
            }
        }
        else if (choice == 4) // 添加药剂师
        {
            addPharmacist(pharmacist, idCounter);
            pause();
        }
        else if (choice == 0) // 返回上一级菜单
        {
            break;
        }
    }
}

// =================================== 患者信息管理 =================================

// 查看所有患者的信息
bool Admin::viewAllPatients(Patient *&patient, const std::string &department)
{
    Patient *current = patient;
    bool found = false;
    std::cout << "正在查找科室 " << department << " 的患者信息..." << std::endl;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->department == department)
        {
            std::cout << "患者ID: " << current->getUserID()
                      << ", 姓名: " << current->getUsername()
                      << ",性别: " << current->getGender()
                      << ", 年龄: " << current->getAge()
                      << ", 科室: " << current->department
                      << ", 身份证号: " << current->idCardNumber
                      << ", 联系方式: " << current->getTelephone() << " / " << current->getEmail()
                      << ", 紧急联系人信息：" << current->emergencyContactName << " / " << current->emergencyContactPhone
                      << ", 过敏史: " << current->allergyHistory
                      << ", 既往病史: " << current->pastMedicalHistory
                      << ", 婚姻状况: " << patientMaritalStatusToString(current->maritalStatus)
                      << ", 住院状态: " << (current->isHospitalized ? "已住院" : "未住院")
                      << ", 挂号次数: " << current->registrationCount
                      << ", 看诊次数: " << current->consultationCount
                      << ", 住院次数: " << current->hospitalizationCount
                      << ", 用药记录数量: " << current->medicationCount
                      << std::endl;
            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "该科室暂无患者信息！" << std::endl;
    }
    return found;
}
// 根据患者ID查看患者信息
bool Admin::viewPatientByID(Patient *&patient, const std::string &department)
{
    std::string patientID = inputIDCheck("请输入要查找的患者ID: ");

    Patient *current = patient;
    bool found = false;
    std::cout << "正在查找ID: " << patientID << " 的患者信息..." << std::endl;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->getUserID() == patientID && current->department == department)
        {
            std::cout << "患者ID: " << current->getUserID()
                      << ", 姓名: " << current->getUsername()
                      << ",性别: " << current->getGender()
                      << ", 年龄: " << current->getAge()
                      << ", 科室: " << current->department
                      << ", 身份证号: " << current->idCardNumber
                      << ", 联系方式: " << current->getTelephone() << " / " << current->getEmail()
                      << ", 紧急联系人信息：" << current->emergencyContactName << " / " << current->emergencyContactPhone
                      << ", 过敏史: " << current->allergyHistory
                      << ", 既往病史: " << current->pastMedicalHistory
                      << ", 婚姻状况: " << patientMaritalStatusToString(current->maritalStatus)
                      << ", 住院状态: " << (current->isHospitalized ? "已住院" : "未住院")
                      << ", 挂号次数: " << current->registrationCount
                      << ", 看诊次数: " << current->consultationCount
                      << ", 住院次数: " << current->hospitalizationCount
                      << ", 用药记录数量: " << current->medicationCount
                      << std::endl;
            found = true;
            break;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到ID为 " << patientID << " 的患者信息！" << std::endl;
    }
    return found;
}
// 根据患者姓名查看患者信息
bool Admin::viewPatientsByName(Patient *&patient, const std::string &department)
{
    std::string name = inputStringCheck("请输入患者姓名（支持模糊查询）: ");

    Patient *current = patient;
    bool found = false;
    std::cout << "正在查找姓名包含 \"" << name << "\" 的患者信息..." << std::endl;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->getUsername().find(name) != std::string::npos && current->department == department)
        {
            std::cout << "患者ID: " << current->getUserID()
                      << ", 姓名: " << current->getUsername()
                      << ",性别: " << current->getGender()
                      << ", 年龄: " << current->getAge()
                      << ", 科室: " << current->department
                      << ", 身份证号: " << current->idCardNumber
                      << ", 联系方式: " << current->getTelephone() << " / " << current->getEmail()
                      << ", 紧急联系人信息：" << current->emergencyContactName << " / " << current->emergencyContactPhone
                      << ", 过敏史: " << current->allergyHistory
                      << ", 既往病史: " << current->pastMedicalHistory
                      << ", 婚姻状况: " << patientMaritalStatusToString(current->maritalStatus)
                      << ", 住院状态: " << (current->isHospitalized ? "已住院" : "未住院")
                      << ", 挂号次数: " << current->registrationCount
                      << ", 看诊次数: " << current->consultationCount
                      << ", 住院次数: " << current->hospitalizationCount
                      << ", 用药记录数量: " << current->medicationCount
                      << std::endl;
            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到姓名包含 \"" << name << "\" 的患者信息！" << std::endl;
    }
    return found;
}
// 根据患者身份证号查看患者信息
bool Admin::viewPatientsByIDCard(Patient *&patient, const std::string &department)
{
    std::string idCardNumber = inputIDcardCheck("请输入患者身份证号: ");

    Patient *current = patient;
    bool found = false;
    std::cout << "正在查找身份证号: " << idCardNumber << " 的患者信息..." << std::endl;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->idCardNumber == idCardNumber && current->department == department)
        {
            std::cout << "患者ID: " << current->getUserID()
                      << ", 姓名: " << current->getUsername()
                      << ",性别: " << current->getGender()
                      << ", 年龄: " << current->getAge()
                      << ", 科室: " << current->department
                      << ", 身份证号: " << current->idCardNumber
                      << ", 联系方式: " << current->getTelephone() << " / " << current->getEmail()
                      << ", 紧急联系人信息：" << current->emergencyContactName << " / " << current->emergencyContactPhone
                      << ", 过敏史: " << current->allergyHistory
                      << ", 既往病史: " << current->pastMedicalHistory
                      << ", 婚姻状况: " << patientMaritalStatusToString(current->maritalStatus)
                      << ", 住院状态: " << (current->isHospitalized ? "已住院" : "未住院")
                      << ", 挂号次数: " << current->registrationCount
                      << ", 看诊次数: " << current->consultationCount
                      << ", 住院次数: " << current->hospitalizationCount
                      << ", 用药记录数量: " << current->medicationCount
                      << std::endl;
            found = true;
            break;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到身份证号为 " << idCardNumber << " 的患者信息！" << std::endl;
    }
    return found;
}
// 根据患者性别查看患者信息
bool Admin::viewPatientsByGender(Patient *&patient, const std::string &department)
{
    std::string gender = inputGenderCheck("请输入患者性别: ");

    Patient *current = patient;
    bool found = false;
    std::cout << "正在查找性别为 \"" << gender << "\" 的患者信息..." << std::endl;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->getGender() == gender && current->department == department)
        {
            std::cout << "患者ID: " << current->getUserID()
                      << ", 姓名: " << current->getUsername()
                      << ",性别: " << current->getGender()
                      << ", 年龄: " << current->getAge()
                      << ", 科室: " << current->department
                      << ", 身份证号: " << current->idCardNumber
                      << ", 联系方式: " << current->getTelephone() << " / " << current->getEmail()
                      << ", 紧急联系人信息：" << current->emergencyContactName << " / " << current->emergencyContactPhone
                      << ", 过敏史: " << current->allergyHistory
                      << ", 既往病史: " << current->pastMedicalHistory
                      << ", 婚姻状况: " << patientMaritalStatusToString(current->maritalStatus)
                      << ", 住院状态: " << (current->isHospitalized ? "已住院" : "未住院")
                      << ", 挂号次数: " << current->registrationCount
                      << ", 看诊次数: " << current->consultationCount
                      << ", 住院次数: " << current->hospitalizationCount
                      << ", 用药记录数量: " << current->medicationCount
                      << std::endl;
            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到性别为 \"" << gender << "\" 的患者信息！" << std::endl;
    }
    return found;
}
// 根据患者年龄范围查看患者信息
bool Admin::viewPatientsByAgeGroup(Patient *&patient, const std::string &department)
{
    std::cout << "请输入患者年龄范围: " << std::endl;
    int minAge = inputAgeCheck("最小年龄: ");
    int maxAge = inputAgeCheck("最大年龄: ");

    if (minAge > maxAge)
    {
        std::cout << "输入的年龄范围无效！" << std::endl;
        return false;
    }

    Patient *current = patient;
    bool found = false;
    std::cout << "正在查找年龄在 " << minAge << " 到 " << maxAge << " 岁之间的患者信息..." << std::endl;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->getAge() >= minAge && current->getAge() <= maxAge && current->department == department)
        {
            std::cout << "患者ID: " << current->getUserID()
                      << ", 姓名: " << current->getUsername()
                      << ",性别: " << current->getGender()
                      << ", 年龄: " << current->getAge()
                      << ", 科室: " << current->department
                      << ", 身份证号: " << current->idCardNumber
                      << ", 联系方式: " << current->getTelephone() << " / " << current->getEmail()
                      << ", 紧急联系人信息：" << current->emergencyContactName << " / " << current->emergencyContactPhone
                      << ", 过敏史: " << current->allergyHistory
                      << ", 既往病史: " << current->pastMedicalHistory
                      << ", 婚姻状况: " << patientMaritalStatusToString(current->maritalStatus)
                      << ", 住院状态: " << (current->isHospitalized ? "已住院" : "未住院")
                      << ", 挂号次数: " << current->registrationCount
                      << ", 看诊次数: " << current->consultationCount
                      << ", 住院次数: " << current->hospitalizationCount
                      << ", 用药记录数量: " << current->medicationCount
                      << std::endl;
            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到年龄在 " << minAge << " 到 " << maxAge << " 岁之间的患者信息！" << std::endl;
    }
    return found;
}
// 根据患者住院状态查看患者信息
bool Admin::viewPatientsByHospitalizationStatus(Patient *&patient, const std::string &department)
{
    std::cout << "请选择住院状态: " << std::endl;
    std::cout << "1. 已住院" << std::endl;
    std::cout << "2. 未住院" << std::endl;
    std::cout << "0. 返回上一级菜单" << std::endl;

    int hospitalizationChoice = selectIntCheck(0, 2);
    if (hospitalizationChoice == 0)
    {
        return false; // 返回上一级菜单
    }
    bool filterHospitalized = (hospitalizationChoice == 1); // 1表示已住院，2表示未住院

    Patient *current = patient;
    bool found = false;
    std::cout << "正在查找住院状态为 " << (filterHospitalized ? "已住院" : "未住院") << " 的患者信息..." << std::endl;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->isHospitalized == filterHospitalized && current->department == department)
        {
            std::cout << "患者ID: " << current->getUserID()
                      << ", 姓名: " << current->getUsername()
                      << ",性别: " << current->getGender()
                      << ", 年龄: " << current->getAge()
                      << ", 科室: " << current->department
                      << ", 身份证号: " << current->idCardNumber
                      << ", 联系方式: " << current->getTelephone() << " / " << current->getEmail()
                      << ", 紧急联系人信息：" << current->emergencyContactName << " / " << current->emergencyContactPhone
                      << ", 过敏史: " << current->allergyHistory
                      << ", 既往病史: " << current->pastMedicalHistory
                      << ", 婚姻状况: " << patientMaritalStatusToString(current->maritalStatus)
                      << ", 住院状态: " << (current->isHospitalized ? "已住院" : "未住院")
                      << ", 挂号次数: " << current->registrationCount
                      << ", 看诊次数: " << current->consultationCount
                      << ", 住院次数: " << current->hospitalizationCount
                      << ", 用药记录数量: " << current->medicationCount
                      << std::endl;
            found = true;
        }
        current = current->next;
    }
    if (!found)
    {
        std::cout << "未找到住院状态为 " << (filterHospitalized ? "已住院" : "未住院") << " 的患者信息！" << std::endl;
    }
    return found;
}
// 根据患者婚姻状况查看患者信息
bool Admin::viewPatientsByMaritalStatus(Patient *&patient, const std::string &department)
{

    int maritalStatusChoice = PatientMaritalStatusMenu();

    if (maritalStatusChoice == 0)
    {
        return false; // 返回上一级菜单
    }
    MaritalStatus filterMaritalStatus = static_cast<MaritalStatus>(maritalStatusChoice - 1);

    Patient *current = patient;
    bool found = false;
    std::cout << "正在查找婚姻状况为 " << patientMaritalStatusToString(filterMaritalStatus) << " 的患者信息..." << std::endl;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->maritalStatus == filterMaritalStatus && current->department == department)
        {
            std::cout << "患者ID: " << current->getUserID()
                      << ", 姓名: " << current->getUsername()
                      << ",性别: " << current->getGender()
                      << ", 年龄: " << current->getAge()
                      << ", 科室: " << current->department
                      << ", 身份证号: " << current->idCardNumber
                      << ", 联系方式: " << current->getTelephone() << " / " << current->getEmail()
                      << ", 紧急联系人信息：" << current->emergencyContactName << " / " << current->emergencyContactPhone
                      << ", 过敏史: " << current->allergyHistory
                      << ", 既往病史: " << current->pastMedicalHistory
                      << ", 婚姻状况: " << patientMaritalStatusToString(current->maritalStatus)
                      << ", 住院状态: " << (current->isHospitalized ? "已住院" : "未住院")
                      << ", 挂号次数: " << current->registrationCount
                      << ", 看诊次数: " << current->consultationCount
                      << ", 住院次数: " << current->hospitalizationCount
                      << ", 用药记录数量: " << current->medicationCount
                      << std::endl;
            found = true;
        }
        current = current->next;
    }
    if (!found)
    {
        std::cout << "未找到婚姻状况为 " << patientMaritalStatusToString(filterMaritalStatus) << " 的患者信息！" << std::endl;
    }
    return found;
}
// 根据联系方式查看患者信息
bool Admin::viewPatientsByContactInfo(Patient *&patient, const std::string &department)
{
    std::cout << "请选择联系方式类型: " << std::endl;
    std::cout << "1. 电话号码" << std::endl;
    std::cout << "2. 电子邮箱" << std::endl;
    std::cout << "0. 返回上一级菜单" << std::endl;
    int contactChoice = selectIntCheck(0, 2);

    if (contactChoice == 0)
    {
        return false; // 返回上一级菜单
    }

    bool found = false;

    if (contactChoice == 1)
    {
        std::string telephone = inputTelephoneCheck("请输入患者电话号码: ");
        Patient *current = patient;

        std::cout << "正在查找电话号码为 " << telephone << " 的患者信息..." << std::endl;
        while (current != nullptr)
        {
            if (!current->isDeleted && current->getTelephone() == telephone && current->department == department)
            {
                std::cout << "患者ID: " << current->getUserID()
                          << ", 姓名: " << current->getUsername()
                          << ",性别: " << current->getGender()
                          << ", 年龄: " << current->getAge()
                          << ", 科室: " << current->department
                          << ", 身份证号: " << current->idCardNumber
                          << ", 联系方式: " << current->getTelephone() << " / " << current->getEmail()
                          << ", 紧急联系人信息：" << current->emergencyContactName << " / " << current->emergencyContactPhone
                          << ", 过敏史: " << current->allergyHistory
                          << ", 既往病史: " << current->pastMedicalHistory
                          << ", 婚姻状况: " << patientMaritalStatusToString(current->maritalStatus)
                          << ", 住院状态: " << (current->isHospitalized ? "已住院" : "未住院")
                          << ", 挂号次数: " << current->registrationCount
                          << ", 看诊次数: " << current->consultationCount
                          << ", 住院次数: " << current->hospitalizationCount
                          << ", 用药记录数量: " << current->medicationCount
                          << std::endl;
                found = true;
            }
            current = current->next;
        }
        if (!found)
        {
            std::cout << "未找到电话号码为 " << telephone << " 的患者信息！" << std::endl;
        }
    }
    else if (contactChoice == 2)
    {
        std::string email = inputEmailCheck("请输入患者电子邮箱: ");
        Patient *current = patient;
        bool found = false;
        std::cout << "正在查找电子邮箱为 " << email << " 的患者信息..." << std::endl;
        while (current != nullptr)
        {
            if (!current->isDeleted && current->getEmail() == email && current->department == department)
            {
                std::cout << "患者ID: " << current->getUserID()
                          << ", 姓名: " << current->getUsername()
                          << ",性别: " << current->getGender()
                          << ", 年龄: " << current->getAge()
                          << ", 科室: " << current->department
                          << ", 身份证号: " << current->idCardNumber
                          << ", 联系方式: " << current->getTelephone() << " / " << current->getEmail()
                          << ", 紧急联系人信息：" << current->emergencyContactName << " / " << current->emergencyContactPhone
                          << ", 过敏史: " << current->allergyHistory
                          << ", 既往病史: " << current->pastMedicalHistory
                          << ", 婚姻状况: " << patientMaritalStatusToString(current->maritalStatus)
                          << ", 住院状态: " << (current->isHospitalized ? "已住院" : "未住院")
                          << ", 挂号次数: " << current->registrationCount
                          << ", 看诊次数: " << current->consultationCount
                          << ", 住院次数: " << current->hospitalizationCount
                          << ", 用药记录数量: " << current->medicationCount
                          << std::endl;
                found = true;
            }
            current = current->next;
        }
        if (!found)
        {
            std::cout << "未找到电子邮箱为 " << email << " 的患者信息！" << std::endl;
        }
    }
    return found;
}
// 根据患者家庭住址查看患者信息
bool Admin::viewPatientsByAddress(Patient *&patient, const std::string &department)
{
    std::string address = inputStringCheck("请输入患者家庭住址（支持模糊查询）: ");

    Patient *current = patient;
    bool found = false;
    std::cout << "正在查找家庭住址包含 \"" << address << "\" 的患者信息..." << std::endl;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->address.find(address) != std::string::npos && current->department == department)
        {
            std::cout << "患者ID: " << current->getUserID()
                      << ", 姓名: " << current->getUsername()
                      << ",性别: " << current->getGender()
                      << ", 年龄: " << current->getAge()
                      << ", 科室: " << current->department
                      << ", 身份证号: " << current->idCardNumber
                      << ", 联系方式: " << current->getTelephone() << " / " << current->getEmail()
                      << ", 紧急联系人信息：" << current->emergencyContactName << " / " << current->emergencyContactPhone
                      << ", 过敏史: " << current->allergyHistory
                      << ", 既往病史: " << current->pastMedicalHistory
                      << ", 婚姻状况: " << patientMaritalStatusToString(current->maritalStatus)
                      << ", 住院状态: " << (current->isHospitalized ? "已住院" : "未住院")
                      << ", 挂号次数: " << current->registrationCount
                      << ", 看诊次数: " << current->consultationCount
                      << ", 住院次数: " << current->hospitalizationCount
                      << ", 用药记录数量: " << current->medicationCount
                      << std::endl;
            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到家庭住址包含 \"" << address << "\" 的患者信息！" << std::endl;
    }
    return found;
}
// 修改患者姓名
void Admin::modifyPatientName(Patient *&patient, const std::string &department)
{
    std::string patientID = inputIDCheck("请输入要修改姓名的患者ID: ");
    Patient *current = patient;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->getUserID() == patientID && current->department == department)
        {
            std::cout << "当前患者姓名: " << current->getUsername() << std::endl;
            std::string newName = inputStringCheck("请输入新的患者姓名: ");
            current->setUsername(newName);
            std::cout << "患者姓名已更新！" << std::endl;
            return;
        }
        current = current->next;
    }
    std::cout << "未找到指定的患者！" << std::endl;
}
// 修改患者性别
void Admin::modifyPatientGender(Patient *&patient, const std::string &department)
{
    std::string patientID = inputIDCheck("请输入要修改性别的患者ID: ");
    Patient *current = patient;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->getUserID() == patientID && current->department == department)
        {
            std::cout << "当前患者性别: " << current->getGender() << std::endl;
            std::string newGender = inputGenderCheck("请输入新的患者性别: ");
            current->setGender(newGender);
            std::cout << "患者性别已更新！" << std::endl;
            return;
        }
        current = current->next;
    }
    std::cout << "未找到指定的患者！" << std::endl;
}
// 修改患者年龄
void Admin::modifyPatientAge(Patient *&patient, const std::string &department)
{
    std::string patientID = inputIDCheck("请输入要修改年龄的患者ID: ");
    Patient *current = patient;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->getUserID() == patientID && current->department == department)
        {
            std::cout << "当前患者年龄: " << current->getAge() << std::endl;
            int newAge = inputAgeCheck("请输入新的患者年龄: ");
            current->setAge(newAge);
            std::cout << "患者年龄已更新！" << std::endl;
            return;
        }
        current = current->next;
    }
    std::cout << "未找到指定的患者！" << std::endl;
}
// 修改患者所属科室
void Admin::modifyPatientDepartment(Patient *&patient, const std::string &department)
{
    std::string patientID = inputIDCheck("请输入要修改所属科室的患者ID: ");
    Patient *current = patient;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->getUserID() == patientID && current->department == department)
        {
            std::cout << "当前患者所属科室: " << current->department << std::endl;

            std::string newDepartment = adminDepartmentMenu();

            if (newDepartment == "0")
            {
                std::cout << "已取消操作！" << std::endl;
                return;
            }
            else if (newDepartment.empty())
            {
                std::cout << "输入的科室名称无效！" << std::endl;
                return;
            }

            current->department = newDepartment;
            std::cout << "患者所属科室已更新！" << std::endl;
            return;
        }
        current = current->next;
    }
    std::cout << "未找到指定的患者！" << std::endl;
}
// 修改患者身份证号
void Admin::modifyPatientIDCard(Patient *&patient, const std::string &department)
{
    std::string patientID = inputIDCheck("请输入要修改身份证号的患者ID: ");
    Patient *current = patient;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->getUserID() == patientID && current->department == department)
        {
            std::cout << "当前患者身份证号: " << current->idCardNumber << std::endl;
            std::string newIDCardNumber = inputIDcardCheck("请输入新的患者身份证号: ");
            current->idCardNumber = newIDCardNumber;
            std::cout << "患者身份证号已更新！" << std::endl;
            return;
        }
        current = current->next;
    }
    std::cout << "未找到指定的患者！" << std::endl;
}
// 修改患者联系电话
void Admin::modifyPatientTelephone(Patient *&patient, const std::string &department)
{
    std::string patientID = inputIDCheck("请输入要修改联系电话的患者ID: ");
    Patient *current = patient;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->getUserID() == patientID && current->department == department)
        {
            std::cout << "当前患者联系电话: " << current->getTelephone() << std::endl;
            std::string newTelephone = inputTelephoneCheck("请输入新的患者联系电话: ");
            current->setTelephone(newTelephone);
            std::cout << "患者联系电话已更新！" << std::endl;
            return;
        }
        current = current->next;
    }
    std::cout << "未找到指定的患者！" << std::endl;
}
// 修改患者电子邮箱
void Admin::modifyPatientEmail(Patient *&patient, const std::string &department)
{
    std::string patientID = inputIDCheck("请输入要修改电子邮箱的患者ID: ");
    Patient *current = patient;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->getUserID() == patientID && current->department == department)
        {
            std::cout << "当前患者电子邮箱: " << current->getEmail() << std::endl;
            std::string newEmail = inputEmailCheck("请输入新的患者电子邮箱: ");
            current->setEmail(newEmail);
            std::cout << "患者电子邮箱已更新！" << std::endl;
            return;
        }
        current = current->next;
    }
    std::cout << "未找到指定的患者！" << std::endl;
}
// 修改家庭住址
void Admin::modifyPatientAddress(Patient *&patient, const std::string &department)
{
    std::string patientID = inputIDCheck("请输入要修改家庭住址的患者ID: ");
    Patient *current = patient;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->getUserID() == patientID && current->department == department)
        {
            std::cout << "当前患者家庭住址: " << current->address << std::endl;
            std::string newAddress = inputStringCheck("请输入新的患者家庭住址: ");
            current->address = newAddress;
            std::cout << "患者家庭住址已更新！" << std::endl;
            return;
        }
        current = current->next;
    }
    std::cout << "未找到指定的患者！" << std::endl;
}
// 修改患者婚姻状况
void Admin::modifyPatientMaritalStatus(Patient *&patient, const std::string &department)
{
    std::string patientID = inputIDCheck("请输入要修改婚姻状况的患者ID: ");
    Patient *current = patient;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->getUserID() == patientID && current->department == department)
        {
            std::cout << "当前患者婚姻状况: " << patientMaritalStatusToString(current->maritalStatus) << std::endl;

            int maritalStatusChoice = PatientMaritalStatusMenu();

            if (maritalStatusChoice == 0)
            {
                std::cout << "已取消操作！" << std::endl;
                return; // 返回上一级菜单
            }

            MaritalStatus newMaritalStatus = static_cast<MaritalStatus>(maritalStatusChoice - 1);
            current->maritalStatus = newMaritalStatus;
            std::cout << "患者婚姻状况已更新！" << std::endl;
            return;
        }
        current = current->next;
    }
    std::cout << "未找到指定的患者！" << std::endl;
}
// 修改紧急联系人信息
void Admin::modifyPatientEmergencyContact(Patient *&patient, const std::string &department)
{
    std::string patientID = inputIDCheck("请输入要修改紧急联系人信息的患者ID: ");
    Patient *current = patient;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->getUserID() == patientID && current->department == department)
        {
            std::cout << "当前患者紧急联系人姓名: " << current->emergencyContactName << std::endl;
            std::cout << "当前患者紧急联系人电话: " << current->emergencyContactPhone << std::endl;

            std::string newEmergencyContactName = inputStringCheck("请输入新的紧急联系人姓名: ");
            std::string newEmergencyContactPhone = inputTelephoneCheck("请输入新的紧急联系人电话: ");

            current->emergencyContactName = newEmergencyContactName;
            current->emergencyContactPhone = newEmergencyContactPhone;

            std::cout << "患者紧急联系人信息已更新！" << std::endl;
            return;
        }
        current = current->next;
    }
    std::cout << "未找到指定的患者！" << std::endl;
}
// 修改患者累计挂号次数
void Admin::modifyPatientRegistrationCount(Patient *&patient, const std::string &department)
{
    std::string patientID = inputIDCheck("请输入要修改累计挂号次数的患者ID: ");
    Patient *current = patient;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->getUserID() == patientID && current->department == department)
        {
            std::cout << "当前患者累计挂号次数: " << current->registrationCount << std::endl;
            std::cout << "请输入新的累计挂号次数: ";
            int newRegistrationCount = selectIntCheck(0, INT_MAX);
            current->registrationCount = newRegistrationCount;
            std::cout << "患者累计挂号次数已更新！" << std::endl;
            return;
        }
        current = current->next;
    }
    std::cout << "未找到指定的患者！" << std::endl;
}
// 修改患者累计看诊次数
void Admin::modifyPatientConsultationCount(Patient *&patient, const std::string &department)
{
    std::string patientID = inputIDCheck("请输入要修改累计看诊次数的患者ID: ");
    Patient *current = patient;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->getUserID() == patientID && current->department == department)
        {
            std::cout << "当前患者累计看诊次数: " << current->consultationCount << std::endl;
            std::cout << "请输入新的累计看诊次数: ";
            int newConsultationCount = selectIntCheck(0, INT_MAX);
            current->consultationCount = newConsultationCount;
            std::cout << "患者累计看诊次数已更新！" << std::endl;
            return;
        }
        current = current->next;
    }
    std::cout << "未找到指定的患者！" << std::endl;
}
// 修改患者累计住院次数
void Admin::modifyPatientHospitalizationCount(Patient *&patient, const std::string &department)
{
    std::string patientID = inputIDCheck("请输入要修改累计住院次数的患者ID: ");
    Patient *current = patient;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->getUserID() == patientID && current->department == department)
        {
            std::cout << "当前患者累计住院次数: " << current->hospitalizationCount << std::endl;
            std::cout << "请输入新的累计住院次数: ";
            int newHospitalizationCount = selectIntCheck(0, INT_MAX);
            current->hospitalizationCount = newHospitalizationCount;
            std::cout << "患者累计住院次数已更新！" << std::endl;
            return;
        }
        current = current->next;
    }
    std::cout << "未找到指定的患者！" << std::endl;
}
// 修改患者累计用药记录数量
void Admin::modifyPatientMedicationCount(Patient *&patient, const std::string &department)
{
    std::string patientID = inputIDCheck("请输入要修改累计用药记录数量的患者ID: ");
    Patient *current = patient;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->getUserID() == patientID && current->department == department)
        {
            std::cout << "当前患者累计用药记录数量: " << current->medicationCount << std::endl;
            std::cout << "请输入新的累计用药记录数量: ";
            int newMedicationCount = selectIntCheck(0, INT_MAX);
            current->medicationCount = newMedicationCount;
            std::cout << "患者累计用药记录数量已更新！" << std::endl;
            return;
        }
        current = current->next;
    }
    std::cout << "未找到指定的患者！" << std::endl;
}
// 删除患者信息（逻辑删除）
void Admin::deletePatient(Patient *&patient, const std::string &department)
{
    std::string patientID = inputIDCheck("请输入要删除的患者ID: ");
    Patient *current = patient;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->getUserID() == patientID && current->department == department)
        {
            current->isDeleted = true; // 逻辑删除
            std::cout << "患者信息已删除！" << std::endl;
            return;
        }
        current = current->next;
    }
    std::cout << "未找到指定的患者！" << std::endl;
}
// 添加患者信息
bool Admin::addPatient(Patient *&patient, int &idCounter)
{
    Patient *newPatient = new Patient();
    bool success = newPatient->patientSignUp(idCounter);
    if (!success)
    {
        delete newPatient;
        return false;
    }
    // 插入到链表头部
    newPatient->next = patient;
    if (patient != nullptr)
    {
        patient->prev = newPatient;
    }
    patient = newPatient;
    return true;
}

void Admin::managePatients(Patient *&patient, const std::string &department, int &idCounter)
{
    while (true)
    {
        int choice = adminPatientManagementMenu();
        if (choice == 0)
        {
            break; // 返回上一级菜单
        }
        else if (choice == 1)
        {
            while (true)
            {
                int viewChoice = adminPatientViewMenu();

                if (viewChoice == 0)
                {
                    break; // 返回患者信息查看菜单
                }
                else if (viewChoice == 1)
                {
                    viewAllPatients(patient, department);
                    pause();
                }
                else if (viewChoice == 2)
                {
                    viewPatientByID(patient, department);
                    pause();
                }
                else if (viewChoice == 3)
                {
                    viewPatientsByName(patient, department);
                    pause();
                }
                else if (viewChoice == 4)
                {
                    viewPatientsByIDCard(patient, department);
                    pause();
                }
                else if (viewChoice == 5)
                {
                    viewPatientsByGender(patient, department);
                    pause();
                }
                else if (viewChoice == 6)
                {
                    viewPatientsByAgeGroup(patient, department);
                    pause();
                }
                else if (viewChoice == 7)
                {
                    viewPatientsByContactInfo(patient, department);
                    pause();
                }
                else if (viewChoice == 8)
                {
                    viewPatientsByHospitalizationStatus(patient, department);
                    pause();
                }
                else if (viewChoice == 9)
                {
                    viewPatientsByAddress(patient, department);
                    pause();
                }
                else if (viewChoice == 10)
                {
                    viewPatientsByMaritalStatus(patient, department);
                    pause();
                }
            }
        }
        else if (choice == 2)
        {
            bool hasPatients = viewAllPatients(patient, department);

            if (hasPatients)
            {
                std::string patientID = inputIDCheck("请输入要修改信息的患者ID: ");
                Patient *target = patient;
                while (target != nullptr)
                {
                    if (!target->isDeleted && target->getUserID() == patientID && target->department == department)
                    {
                        break; // 找到目标患者，跳出循环
                    }
                    target = target->next;
                }

                if (target)
                {
                    while (true)
                    {
                        std::cout << "正在进行修改的患者信息: " << std::endl;
                        std::cout << "患者ID: " << target->getUserID()
                                  << ", 姓名: " << target->getUsername()
                                  << ",性别: " << target->getGender()
                                  << ", 年龄: " << target->getAge()
                                  << ", 科室: " << target->department
                                  << ", 身份证号: " << target->idCardNumber
                                  << ", 联系方式: " << target->getTelephone() << " / " << target->getEmail()
                                  << ", 紧急联系人信息：" << target->emergencyContactName << " / " << target->emergencyContactPhone
                                  << ", 过敏史: " << target->allergyHistory
                                  << ", 既往病史: " << target->pastMedicalHistory
                                  << ", 婚姻状况: " << patientMaritalStatusToString(target->maritalStatus)
                                  << ", 住院状态: " << (target->isHospitalized ? "已住院" : "未住院")
                                  << ", 挂号次数: " << target->registrationCount
                                  << ", 看诊次数: " << target->consultationCount
                                  << ", 住院次数: " << target->hospitalizationCount
                                  << ", 用药记录数量: " << target->medicationCount
                                  << std::endl;

                        int modifyChoice = adminPatientModificationMenu();

                        if (modifyChoice == 0)
                        {
                            break; // 返回患者信息修改菜单
                        }
                        else if (modifyChoice == 1)
                        {
                            modifyPatientName(patient, department);
                            pause();
                        }
                        else if (modifyChoice == 2)
                        {
                            modifyPatientGender(patient, department);
                            pause();
                        }
                        else if (modifyChoice == 3)
                        {
                            modifyPatientAge(patient, department);
                            pause();
                        }
                        else if (modifyChoice == 4)
                        {
                            modifyPatientDepartment(patient, department);
                            pause();
                        }
                        else if (modifyChoice == 5)
                        {
                            modifyPatientIDCard(patient, department);
                            pause();
                        }
                        else if (modifyChoice == 6)
                        {
                            modifyPatientTelephone(patient, department);
                            pause();
                        }
                        else if (modifyChoice == 7)
                        {
                            modifyPatientEmail(patient, department);
                            pause();
                        }
                        else if (modifyChoice == 8)
                        {
                            modifyPatientAddress(patient, department);
                            pause();
                        }
                        else if (modifyChoice == 9)
                        {
                            modifyPatientMaritalStatus(patient, department);
                            pause();
                        }
                        else if (modifyChoice == 10)
                        {
                            modifyPatientEmergencyContact(patient, department);
                            pause();
                        }
                        else if (modifyChoice == 11)
                        {
                            modifyPatientRegistrationCount(patient, department);
                            pause();
                        }
                        else if (modifyChoice == 12)
                        {
                            modifyPatientConsultationCount(patient, department);
                            pause();
                        }
                        else if (modifyChoice == 13)
                        {
                            modifyPatientHospitalizationCount(patient, department);
                            pause();
                        }
                        else if (modifyChoice == 14)
                        {
                            modifyPatientMedicationCount(patient, department);
                            pause();
                        }
                    }
                }
                else
                {
                    std::cout << "未找到ID为 " << patientID << " 的患者！" << std::endl;
                    pause();
                }
            }
        }
        else if (choice == 3)
        {
            bool hasPatients = viewAllPatients(patient, department);

            if (hasPatients)
            {
                std::string patientID = inputIDCheck("请输入要删除的患者ID: ");
                Patient *target = patient;
                while (target != nullptr)
                {
                    if (!target->isDeleted && target->getUserID() == patientID && target->department == department)
                    {
                        break; // 找到目标患者，跳出循环
                    }
                    target = target->next;
                }

                if (target)
                {
                    std::cout << "正在删除的患者信息: " << std::endl;
                    std::cout << "患者ID: " << target->getUserID()
                              << ", 姓名: " << target->getUsername()
                              << ",性别: " << target->getGender()
                              << ", 年龄: " << target->getAge()
                              << ", 科室: " << target->department
                              << ", 身份证号: " << target->idCardNumber
                              << ", 联系方式: " << target->getTelephone() << " / " << target->getEmail()
                              << ", 紧急联系人信息：" << target->emergencyContactName << " / " << target->emergencyContactPhone
                              << ", 过敏史: " << target->allergyHistory
                              << ", 既往病史: " << target->pastMedicalHistory
                              << ", 婚姻状况: " << patientMaritalStatusToString(target->maritalStatus)
                              << ", 住院状态: " << (target->isHospitalized ? "已住院" : "未住院")
                              << ", 挂号次数: " << target->registrationCount
                              << ", 看诊次数: " << target->consultationCount
                              << ", 住院次数: " << target->hospitalizationCount
                              << ", 用药记录数量: " << target->medicationCount
                              << std::endl;

                    std::cout << "确定要删除该患者信息吗？: ";
                    std::cout << "1. 确定" << std::endl;
                    std::cout << "0. 取消" << std::endl;
                    int confirmChoice = selectIntCheck(0, 1);
                    if (confirmChoice == 1)
                    {
                        deletePatient(patient, department);
                        pause();
                    }
                    else
                    {
                        std::cout << "已取消删除操作！" << std::endl;
                        pause();
                    }
                }
                else
                {
                    std::cout << "未找到ID为 " << patientID << " 的患者！" << std::endl;
                    pause();
                }
            }
        }
        else if (choice == 4)
        {
            addPatient(patient, idCounter);
            pause();
        }
    }
}
