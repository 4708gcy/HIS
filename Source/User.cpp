#include "../Head/User.h"
#include <fstream>
#include <sstream>
#include <random>
#include <algorithm>
#include <iostream>

// 1 - Admin, 2 - Doctor, 3 - Nurse, 4 - Pharmacist, 5 - Patient
bool User::signUp(int choice)
{
    switch (choice)
    {
    case 1:
    {
        std::string apiKey;
        std::cout << "请输入 API 密钥（仅管理员注册需要）: ";
        std::cin >> apiKey;

        if (apiKey == "88888888")
        {
            role = UserRole::ADMIN;
            userID = "0" + std::to_string(adminIDnum++).insert(0, 5 - std::to_string(adminIDnum).length(), '0');
            salt = generateSalt();
            MyTime &myTime = MyTime::getInstance();
            createTime = myTime.getTime();
            isAccountActive = true; // 管理员账户默认激活

            std::cout << "请输入管理员姓名: ";
            std::cin >> username;

            std::string password;
            std::cout << "请输入密码: ";
            std::cin >> password;
            storedHash = SHA256Encrypt(password, salt, kHashIterations);

            return true; // 管理员注册成功
        }
        else
        {
            std::cout << "无效的 API 密钥，注册失败！" << std::endl;
        }
        return false; // 管理员注册失败
    }
    case 2:
    {
        role = UserRole::DOCTOR;
        userID = "1" + std::to_string(doctorIDnum++).insert(0, 5 - std::to_string(doctorIDnum).length(), '0');
        salt = generateSalt();
        MyTime &myTime = MyTime::getInstance();
        createTime = myTime.getTime();
        isAccountActive = true; // 医生账户默认激活

        std::cout << "请输入医生姓名: ";
        std::cin >> username;
        std::string password;
        std::cout << "请输入密码: ";
        std::cin >> password;
        storedHash = SHA256Encrypt(password, salt, kHashIterations);
        std::cout << "医生注册成功! 您的用户ID是: " << userID << std::endl;
        return true; // 医生注册成功
    }
    case 3:
    {
        role = UserRole::NURSE;
        userID = "2" + std::to_string(nurseIDnum++).insert(0, 5 - std::to_string(nurseIDnum).length(), '0');
        salt = generateSalt();
        MyTime &myTime = MyTime::getInstance();
        createTime = myTime.getTime();
        isAccountActive = true; // 护士账户默认激活
        std::cout << "请输入护士姓名: ";
        std::cin >> username;
        std::string password;
        std::cout << "请输入密码: ";
        std::cin >> password;
        storedHash = SHA256Encrypt(password, salt, kHashIterations);
        std::cout << "护士注册成功! 您的用户ID是: " << userID << std::endl;
        return true; // 护士注册成功
    }
    case 4:
    {
        role = UserRole::PHARMACIST;
        userID = "3" + std::to_string(pharmacistIDnum++).insert(0, 5 - std::to_string(pharmacistIDnum).length(), '0');
        salt = generateSalt();
        MyTime &myTime = MyTime::getInstance();
        createTime = myTime.getTime();
        isAccountActive = true; // 药剂师账户默认激活
        std::cout << "请输入药剂师姓名: ";
        std::cin >> username;
        std::string password;
        std::cout << "请输入密码: ";
        std::cin >> password;
        storedHash = SHA256Encrypt(password, salt, kHashIterations);
        std::cout << "药剂师注册成功! 您的用户ID是: " << userID << std::endl;
        return true; // 药剂师注册成功
    }
    case 5:
    {
        role = UserRole::PATIENT;
        userID = "4" + std::to_string(patientIDnum++).insert(0, 5 - std::to_string(patientIDnum).length(), '0');
        salt = generateSalt();
        MyTime &myTime = MyTime::getInstance();
        createTime = myTime.getTime();
        isAccountActive = true; // 患者账户默认激活
        std::cout << "请输入患者姓名: ";
        std::cin >> username;
        std::string password;
        std::cout << "请输入密码: ";
        std::cin >> password;
        storedHash = SHA256Encrypt(password, salt, kHashIterations);
        std::cout << "患者注册成功! 您的用户ID是: " << userID << std::endl;
        return true; // 患者注册成功
    }
    default:
        std::cout << "无效的选择！" << std::endl;
    }

    return false; // 注册失败
}






// 只有管理员可以获取迭代次数，其他角色返回 -1 表示不可用
int User::getKHashIterations() const
{
    return role == UserRole::ADMIN ? kHashIterations : -1;
}

// 返回登录状态
bool User::getIsLoggedIn() const { return isLoggedIn; }

// 返回账户是否激活/未锁定
bool User::getIsAccountActive() const { return isAccountActive; }

// 返回连续登录失败次数
int User::getLoginAttempts() const { return loginAttempts; }

// 返回用户ID
const std::string &User::getUserID() const { return userID; }

// 返回用户姓名
const std::string &User::getUsername() const { return username; }

// 返回用户的身份角色
UserRole User::getRole() const { return role; }

// 返回账户创建时间字符串
const std::string &User::getCreateTime() const { return createTime; }

// 设置姓名
void User::setUsername(const std::string &uname) { username = uname; }

User::~User()
{
}
