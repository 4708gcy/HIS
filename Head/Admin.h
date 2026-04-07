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

/**
 * @file Admin.h
 * @brief 管理员类（继承 User），包含账号管理与全局查询方法
 */

class Admin : public User
{
private:
    std::string adminLevel;      // 管理员级别（例如 "SUPER","NORMAL"）
    double historyBalance = 0.0; // 医院流水/余额概览（示例字段）
    std::string userDataPath;    // 对应用户数据文件路径（用于 loadFromFile/saveToFile）

public:
    explicit Admin(const std::string &adminID,
                   const std::string &dataPath = "Data/UserData/AdminChainData/admin_users.txt")
        : User(adminID, UserRole::ADMIN, true), userDataPath(dataPath)
    {
        loadFromFile();
    }

    // 从 userDataPath 加载管理员 profile（级别/余额等）
    void loadFromFile() override;
    // 将管理员 profile 写回文件
    void saveToFile() override;

    // 账号管理接口
    bool unlockUser(const std::string &targetUserID); // 管理员解封账户
    bool lockUser(const std::string &targetUserID);   // 管理员封禁账户

    // 全局查询/报表
    void queryHospitalOverview() const;
    void queryDepartmentStats(const std::string &department) const;
    void queryFinanceReport() const;

    const std::string &getAdminLevel() const { return adminLevel; }
    double getHistoryBalance() const { return historyBalance; }
    void setAdminLevel(const std::string &level) { adminLevel = level; }
    void setHistoryBalance(double v) { historyBalance = v; }
};

#endif // ADMIN_H