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
#include "InputCheck.h"
#include "SHA-256.h"
#include "GetTime.h"
#include "Registration.h"
#include "Consultation.h"
#include "Examination.h"
#include "Hospitalization.h"

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

    static constexpr int kMaxLoginAttempts = 5;  // 锁定门槛
    static constexpr int kHashIterations = 1000; // 哈希迭代次数

public:
    /**
     * @brief 已有账号构造（仅uid和role），可选择是否自动加载全量记录
     * @param uid 用户ID
     * @param r   角色类型
     * @param autoLoadRecords 是否自动加载 RecordData 下的链表文件
     */
    User(const std::string &uid, UserRole r, bool autoLoadRecords = true);

    /**
     * @brief 新账号构造：提供用户名和明文密码，会生成 salt 并存储哈希
     * @param uid 用户ID
     * @param uname 用户名
     * @param plainPassword 明文密码（构造函数中会加盐哈希）
     * @param r 角色类型
     * @param autoLoadRecords 是否自动加载记录链表
     */
    User(const std::string &uid,
         const std::string &uname,
         const std::string &plainPassword,
         UserRole r,
         bool autoLoadRecords = true);

    virtual ~User(); // 析构函数负责释放链表内存

    // 登录/登出相关
    bool login(const std::string &inputPassword); // 校验后更新状态并处理锁定逻辑
    void logout();

    // 简单 getter
    bool getIsLoggedIn() const;
    bool getIsAccountActive() const;
    int getLoginAttempts() const;
    const std::string &getUserID() const;
    const std::string &getUsername() const;
    const std::string &getStoredHash() const;
    UserRole getRole() const;
    const std::string &getCreateTime() const;

    // setter / 管理方法
    void setUsername(const std::string &uname);
    void setStoredHash(const std::string &hashValue);
    void setAccountActive(bool active);
    void resetLoginAttempts();

    // 记录链表头访问，用于外部查询/遍历
    Registration *getRegistrationHead() const;
    Consultation *getConsultationHead() const;
    Examination *getExaminationHead() const;
    Hospitalization *getHospitalizationHead() const;

    // 子类必须实现用于加载/保存用户 profile（角色特有字段）
    virtual void loadFromFile() = 0;
    virtual void saveToFile() = 0;

protected:
    // 初始化/清理链表头与加载实现
    void initRecordHeads();  // 分配/置空头节点（或置为 nullptr）
    void clearRecordLists(); // 释放所有链表内存

    // 从磁盘加载全部记录（User 构造时调用）
    void loadAllRecordLists();
    void loadRegistrationList(const std::string &filePath);
    void loadConsultationList(const std::string &filePath);
    void loadExaminationList(const std::string &filePath);
    void loadHospitalizationList(const std::string &filePath);

    // 辅助静态工具
    static std::vector<std::string> split(const std::string &line, char sep);
    static std::string generateSalt(unsigned int len = 16); // 用于新建账号时生成随机盐
};

#endif // USER_H