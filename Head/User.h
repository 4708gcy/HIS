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
    bool isLoggedIn = false;
    std::string userID;
    std::string username;
    std::string storedHash; // 格式: salt$hash
    int loginAttempts = 0;
    bool isAccountActive = true;
    UserRole role;
    std::string createTime;

    Registration *regHead = nullptr;
    Consultation *conHead = nullptr;
    Examination *examHead = nullptr;
    Hospitalization *hospHead = nullptr;

    static constexpr int kMaxLoginAttempts = 5;
    static constexpr int kHashIterations = 1000;

public:
    // 已有账号: 先有 userID，再从文件加载 profile
    User(const std::string &uid, UserRole r, bool autoLoadRecords = true);

    // 新建账号: 直接给用户名和明文密码
    User(const std::string &uid,
         const std::string &uname,
         const std::string &plainPassword,
         UserRole r,
         bool autoLoadRecords = true);

    virtual ~User();

    bool login(const std::string &inputPassword);
    void logout();

    bool getIsLoggedIn() const;
    bool getIsAccountActive() const;
    int getLoginAttempts() const;
    const std::string &getUserID() const;
    const std::string &getUsername() const;
    const std::string &getStoredHash() const;
    UserRole getRole() const;
    const std::string &getCreateTime() const;

    void setUsername(const std::string &uname);
    void setStoredHash(const std::string &hashValue);
    void setAccountActive(bool active);
    void resetLoginAttempts();

    // 记录链表头访问
    Registration *getRegistrationHead() const;
    Consultation *getConsultationHead() const;
    Examination *getExaminationHead() const;
    Hospitalization *getHospitalizationHead() const;

    // 子类必须实现: 从角色文件加载本人数据
    virtual void loadFromFile() = 0;
    virtual void saveToFile() = 0;

protected:
    void initRecordHeads();
    void clearRecordLists();

    void loadAllRecordLists();
    void loadRegistrationList(const std::string &filePath);
    void loadConsultationList(const std::string &filePath);
    void loadExaminationList(const std::string &filePath);
    void loadHospitalizationList(const std::string &filePath);

    static std::vector<std::string> split(const std::string &line, char sep);
    static std::string generateSalt(unsigned int len = 16);
};

#endif