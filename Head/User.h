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

enum class UserRole{
    ADMIN,
    DOCTOR,
    NURSE,
    PHARMACIST,
    PATIENT
};

class User{
    protected:
        bool isloggedIn = false; // 登录状态
        std::string userID; // 用户ID
        std::string username; // 用户名
        std::string SHA256password; // 密文密码
        std::string salt; // 密码盐
        int loginAttempts; // 登录尝试次数
        bool isActive; // 账户是否激活(如果输入错误密码过多，账户将被锁定，也就是isActive变为false)
        UserRole role; // 用户角色

        std::string createTime; // 账户创建时间
    
    public:

    // getter
    bool getIsLoggedIn() const; // 获取登录状态 
    const std::string& getUserID() const; // 获取用户ID
    const std::string& getUsername() const; // 获取用户名
    const std::string& getPassword() const; // 密文
    const std::string& getSalt() const; // 获取密码盐
    int getLoginAttempts() const; // 获取登录尝试次数
    bool isActive() const; // 获取账户激活状态
    UserRole getRole() const; // 获取用户角色
    std::string getCreateTime() const; // 获取账户创建时间

    // setter
    void setUserId(const std::string& userID); // 设置用户ID
    void setUsername(const std::string& username);  // 设置用户名
    void setPassword(const std::string& password); // 设置密文
    void setLoginAttempts(int loginAttempts); // 设置登录尝试次数
    void setRole(UserRole role); // 设置用户角色

    virtual void loadFromFile() = 0; // 从文件加载用户数据
    virtual void saveToFile() = 0; // 将用户数据保存到文件

    bool login(const std::string& password); // 用户登录方法，验证密码并更新登录状态
    void logout(); // 用户登出方法，更新登录状态

};


#endif // USER_H