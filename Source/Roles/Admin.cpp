#include "Roles/Admin.h"
#include <iostream>

Admin::Admin()
{
}

bool Admin::adminSignUp(int &idCounter)
{
    bool flag = signUp(1, idCounter); // 1 - Admin
    if (flag)
    {
        printSuccess("管理员注册成功! 您的用户ID是: " + getUserID());
        return true;
    }
    else
    {
        printError("管理员注册失败！");
        return false;
    }
}

bool Admin::adminSignIn()
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
            std::cout << "退出登录。" << std::endl;
            return false;
        }

        bool success = verifyPasswordCompat(pwd, storedHash);

        if (success)
        {
            loginAttempts = 0;
            isLoggedIn = true;
            printSuccess("管理员登录成功！");
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
