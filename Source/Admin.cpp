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
        std::string pwd;
        std::cout << "请输入密码:" << std::endl;
        std::cin >> pwd;

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

template <typename T>
void Admin::setAccountActive(T *head)
{
    std::string targetID;
    std::cout << "请输入要操作的用户ID: ";
    std::cin >> targetID;

    T *current = head;
    while (current != nullptr)
    {
        if (current->getUserID() == targetID)
        {
            bool newStatus;
            std::cout << "请输入新的账户状态 (1 - 激活, 0 - 锁定): ";
            std::cin >> newStatus;
            if (newStatus)
            {
                current->isAccountActive = true;
                current->loginAttempts = 0; // 重置登录失败次数
                std::cout << "账户已激活！" << std::endl;
            }
            else
            {
                current->isAccountActive = false;
                std::cout << "账户已锁定！" << std::endl;
            }
            return;
        }
        current = current->next; // 假设 T 中有 next 指针
    }

    std::cout << "未找到指定用户！" << std::endl;
}

void Admin::manageRegistrations(Registration *reg, std::string department)
{
    while(true){
        int choice = adminRegistrationManagementMenu();
        if(choice == 0) break;

        if(choice == 1){
            Registration *current = reg;
            std::cout << "挂号记录列表:" << std::endl;
            while(current != nullptr){
                if(current->department == department){
                    std::string statusStr = (current->status == RegistrationStatus::BOOKED) ? "已预约" :
                                            (current->status == RegistrationStatus::PAID) ? "已支付" :
                                            (current->status == RegistrationStatus::CANCELED) ? "已取消" : "已完成";
                    std::cout << "ID: " << current->registrationID 
                              << ", 患者ID: " << current->patientID 
                              << ", 医生ID: " << current->doctorID 
                              << ", 时间: " << current->registerTime 
                              << ", 费用: " << current->fee 
                              << ", 状态: " << statusStr
                              << ", 备注: " << current->note 
                              << std::endl;
                }
                current = current->next;
            }
        }else if(choice == 2){
            std::string regID;
            std::cout << "请输入要修改状态的挂号记录ID: ";
            std::cin >> regID;

            Registration *current = reg;
            while(current != nullptr){
                if(current->registrationID == regID && current->department == department){
                    int newStatus;
                    std::cout << "请输入新的挂号状态 (0 - 已预约, 1 - 已支付, 2 - 已取消, 3 - 已完成): ";
                    std::cin >> newStatus;
                    if(newStatus >= 0 && newStatus <= 3){
                        current->status = static_cast<RegistrationStatus>(newStatus);
                        std::cout << "挂号状态已更新！" << std::endl;
                    }else{
                        std::cout << "无效的状态输入！" << std::endl;
                    }
                    break;
                }
                current = current->next;
            }
    }else if(choice == 3){
            std::string regID;
            std::cout << "请输入要删除的挂号记录ID: ";
            std::cin >> regID;

            bool found = false;

            Registration *current = reg;
            while(current != nullptr){
                if(current->registrationID == regID && current->department == department){
                    // 从链表中删除 current
                    if(current->prev) current->prev->next = current->next;
                    if(current->next) current->next->prev = current->prev;
                    delete current; // 释放内存
                    std::cout << "挂号记录已删除！" << std::endl;
                    found = true;
                    break;
                }
                current = current->next;
            }
            if (!found) {
                std::cout << "未找到指定的挂号记录！" << std::endl;
            }
        }else{
            std::cout << "无效的选择! 请重新选择。" << std::endl;
            system("pause");
        }
    }

}