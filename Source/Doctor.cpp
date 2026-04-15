/** 
 * @file Doctor.cpp
 * @brief 医疗管理系统中的医生类实现
 * @details 该源文件实现了 Doctor 类的构造、析构、文件操作方法以及医生业务接口。
 * @author 李阳旭 / 小组成员 郭承宇 / 小组成员
 * @date 2026-4-15
 * @version 1.0
 */

#include "Doctor.h"

void Doctor::doctorSignUp(int &idCounter){
    signUp(2, idCounter); // 调用基类的注册方法，传入角色类型 2（医生）
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