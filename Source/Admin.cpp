#include "../Head/Admin.h"
#include <fstream>
#include <iostream>

Admin::Admin()
{
}

Admin *Admin::adminSignUp()
{
    bool flag = signUp(1); // 1 - Admin
    if (flag)
    {
        totalRevenue = 0.0;
        totalExpenses = 0.0;
        netProfit = 0.0;

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

// 查看所有的挂号记录（可按患者ID、医生ID、状态过滤）
void Admin::viewAllRegistrations(Registration *&reg, const std::string &department)
{
    Registration *current = reg;
    std::cout << "挂号记录列表:" << std::endl;

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
    std::string doctorID;
    std::cout << "请输入医生ID: ";
    std::cin >> doctorID;

    bool found = false;

    Registration *current = reg;
    std::cout << "挂号记录列表 (医生ID: " << doctorID << "):" << std::endl;
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
    std::string patientID;
    std::cout << "请输入患者ID: ";
    std::cin >> patientID;

    bool found = false;

    Registration *current = reg;
    std::cout << "挂号记录列表 (患者ID: " << patientID << "):" << std::endl;
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
    std::cout << "挂号记录列表 (状态过滤: " << regStatusToString(filterStatus) << "):" << std::endl;
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
    std::string regID;
    std::cout << "请输入要修改状态的挂号记录ID: ";
    std::cin >> regID;

    bool found = false;

    Registration *current = reg;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->registrationID == regID && current->department == department)
        {
            int newStatus;
            std::cout << "请输入新的挂号状态 (0 - 已预约, 1 - 已支付, 2 - 已取消, 3 - 已完成): ";
            std::cin >> newStatus;
            if (newStatus >= 0 && newStatus <= 3)
            {
                current->status = static_cast<RegistrationStatus>(newStatus);
                std::cout << "挂号状态已更新！" << std::endl;
            }
            else
            {
                std::cout << "无效的状态输入！" << std::endl;
            }
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
    std::string regID;
    std::cout << "请输入要删除的挂号记录ID: ";
    std::cin >> regID;

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
void Admin::addRegistration(Registration *&reg, const std::string &department)
{
    Registration *newReg = new Registration();

    std::cout << "请输入患者ID: ";
    std::cin >> newReg->patientID;
    newReg->department = department;

    std::cout << "请输入医生ID (可留空表示未指定): ";
    std::cin.ignore(); // 清除输入缓冲区
    std::getline(std::cin, newReg->doctorID);

    newReg->fee = inputFeeCheck(); // 输入费用并检查有效性

    // 生成唯一的挂号ID（可以根据实际需求改为更复杂的生成方式）
    newReg->registrationID = "reg" + std::to_string(registrationCount++).insert(0, 6 - std::to_string(registrationCount).length(), '0');
    MyTime &t = MyTime::getInstance();
    newReg->registerTime = t.getTime(); // 获取当前时间字符串

    // 插入到链表头部
    newReg->next = reg;
    if (reg != nullptr)
        reg->prev = newReg;
    reg = newReg;

    std::cout << "挂号记录已添加！新挂号ID: " << newReg->registrationID << std::endl;
}

void Admin::manageRegistrations(Registration *&reg, const std::string &department)
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
            addRegistration(reg, department);
            pause();
        }
        else
        {
            std::cout << "无效的选择! 请重新选择。" << std::endl;
            pause();
        }
    }
}


void Admin::manageConsultations(Consultation *&con, const std::string &department)
{    // 管理看诊记录的函数实现（类似于 manageRegistrations，可以根据实际需求添加查看、修改等功能）
    std::cout << "管理看诊记录功能尚未实现！" << std::endl;
}

void Admin::manageExaminations(Examination *&exam, const std::string &department)
{    // 管理检查记录的函数实现（类似于 manageRegistrations，可以根据实际需求添加查看、修改等功能）
    std::cout << "管理检查记录功能尚未实现！" << std::endl;
}

void Admin::manageHospitalizations(Hospitalization *&hos, const std::string &department)
{    // 管理住院记录的函数实现（类似于 manageRegistrations，可以根据实际需求添加查看、修改等功能）
    std::cout << "管理住院记录功能尚未实现！" << std::endl;
}

void Admin::manageMedicationRecords(MedicationRecord *&medRec, const std::string &department)
{    // 管理用药记录的函数实现（类似于 manageRegistrations，可以根据实际需求添加查看、修改等功能）
    std::cout << "管理用药记录功能尚未实现！" << std::endl;
}