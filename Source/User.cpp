#include "../Head/User.h"
#include <fstream>
#include <sstream>
#include <random>
#include <algorithm>

// 简单 getter
bool User::getIsLoggedIn() const { return isLoggedIn; }
bool User::getIsAccountActive() const { return isAccountActive; }
int User::getLoginAttempts() const { return loginAttempts; }
const std::string &User::getUserID() const { return userID; }
const std::string &User::getUsername() const { return username; }
const std::string &User::getStoredHash() const { return storedHash; }
UserRole User::getRole() const { return role; }
const std::string &User::getCreateTime() const { return createTime; }
MedicationRecord *User::getMedicationHead() const { return medHead; }
Medicine *User::getMedicineHead() const { return medicineHead; }



// setter / 管理方法
void User::setUsername(const std::string &uname) { username = uname; }
void User::setStoredHash(const std::string &hashValue) { storedHash = hashValue; }
void User::setAccountActive(bool active)
{
    isAccountActive = active;
    if (!active)
    {
        isLoggedIn = false; // 禁用账户时登出
    }
}
void User::resetLoginAttempts() { loginAttempts = 0; }

int User::getKHashIterations() const { return kHashIterations; }
void User::setUserID(const std::string &uid) { userID = uid; }
void User::setRole(UserRole r) { role = r; }




User::~User()
{
}
