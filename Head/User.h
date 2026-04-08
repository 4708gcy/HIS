/** 
 * @file User.h
 * @brief 用户基类的定义
 * @details 该头文件定义了一个 User 类，作为所有用户类型的基类，包含用户的基本信息、安全认证和权限管理功能
 * @author 郭承宇
 * @date 2026-4-6
 * @version 1.0
 */

#ifndef USER_H
#define USER_H

#include <string>
#include <vector>
#include "SHA-256.h"
#include "GetTime.h"
#include "Registration.h"
#include "Consultation.h"
#include "Examination.h"
#include "Hospitalization.h"
#include "MedicationRecord.h"
#include "Medicine.h"

/**
 * @file User.h
 * @brief 用户基类（含通用属性、登录逻辑以及业务记录链表头）
 * @details User 负责初始化/释放四类记录链表（挂号/看诊/检查/住院）。
 *          子类在构造时会调用 loadFromFile() 加载角色特有字段。
 */

enum class UserRole
{
    ADMIN,
    DOCTOR,
    NURSE,
    PHARMACIST,
    PATIENT
};

class User
{
protected:
    bool isLoggedIn = false;     // 登录状态
    std::string userID;          // 用户唯一ID
    std::string username;        // 显示用户名
    std::string storedHash;      // 存储的密码串（格式 salt$hash）
    int loginAttempts = 0;       // 连续失败次数
    bool isAccountActive = true; // 账户是否被激活/未锁定
    UserRole role;               // 角色类型
    std::string createTime;      // 账户创建时间字符串

    // 四类记录链表头（protected，子类可访问）
    Registration *regHead = nullptr;
    Consultation *conHead = nullptr;
    Examination *examHead = nullptr;
    Hospitalization *hospHead = nullptr;
    MedicationRecord *medHead = nullptr; // 用药记录链表头
    Medicine *medicineHead = nullptr;    // 药品链表头

    static constexpr int kMaxLoginAttempts = 5;  // 锁定门槛
    static constexpr int kHashIterations = 1000; // 哈希迭代次数

public:


    virtual ~User(); // 析构函数负责释放链表内存

    virtual void loadFromFile(const std::string &path);
    virtual void saveToFile(const std::string &path);

    // 简单 getter
    bool getIsLoggedIn() const;
    bool getIsAccountActive() const;
    int getLoginAttempts() const;
    const std::string &getUserID() const;
    const std::string &getUsername() const;
    const std::string &getStoredHash() const;
    UserRole getRole() const;
    const std::string &getCreateTime() const;
    MedicationRecord *getMedicationHead() const;
    Medicine *getMedicineHead() const;
    int getKHashIterations() const;

    // setter / 管理方法
    void setRole(UserRole r);
    void setUserID(const std::string &uid);
    void setUsername(const std::string &uname);
    void setStoredHash(const std::string &hashValue);
    void setAccountActive(bool active);
    void resetLoginAttempts();

};

#endif // USER_H