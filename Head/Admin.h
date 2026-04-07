/**
 * @file Admin.h
 * @brief 管理员类的定义
 * @details 该头文件定义了一个 Admin 类，继承自 User 类，包含管理员特有的功能和权限管理方法
 * @author 郭承宇
 * @date 2026-4-6
 * @version 1.0
 */

#ifndef ADMIN_H
#define ADMIN_H

#include <string>
#include "User.h"

class Admin : public User
{
private:
    std::string adminLevel; // SUPER / NORMAL
    double historyBalance = 0.0;
    std::string userDataPath; // Data/UserData/AdminChainData/admin_users.txt

public:
    explicit Admin(const std::string &adminID,
                   const std::string &dataPath = "Data/UserData/AdminChainData/admin_users.txt")
        : User(adminID, UserRole::ADMIN, true), userDataPath(dataPath)
    {
        loadFromFile();
    }

    void loadFromFile() override;
    void saveToFile() override;

    // 账号管理
    bool unlockUser(const std::string &targetUserID);
    bool lockUser(const std::string &targetUserID);

    // 数据字典与全局查询
    void queryHospitalOverview() const;
    void queryDepartmentStats(const std::string &department) const;
    void queryFinanceReport() const;

    const std::string &getAdminLevel() const { return adminLevel; }
    double getHistoryBalance() const { return historyBalance; }
    void setAdminLevel(const std::string &level) { adminLevel = level; }
    void setHistoryBalance(double v) { historyBalance = v; }
};

#endif