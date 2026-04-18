/** 
 * @file Pharmacist.cpp
 * @brief 医疗管理系统中的药剂师类实现
 * @details 该源文件实现了 Pharmacist 类的构造、析构、文件操作方法以及药剂师业务接口。
 * @author 李阳旭 / 小组成员 郭承宇 / 小组成员
 * @date 2026-4-16
 * @version 1.0
 */

#include "../Head/Pharmacist.h"

Pharmacist::Pharmacist()
{
    // 构造函数实现（如有需要）
}

Pharmacist::~Pharmacist()
{
    // 析构函数实现（如有需要）
}

bool Pharmacist::pharmacistSignUp(int &idCounter){
    bool success = signUp(4, idCounter); // 调用基类的注册方法，传入角色类型 4（药剂师）
    if (!success)
    {
        std::cout << "药剂师注册失败！" << std::endl;
        return false;
    }
    this->pharmacistID = this->userID; // 药剂师ID与用户ID保持一致
    
    // 其他药剂师特有信息的初始化在这里进行
    this->department = inputDepartmentCheck("请输入所属药房/科室: ");

    std::cout << "请选择药剂师等级:" << std::endl;
    std::cout << "1. 实习药剂师" << std::endl;
    std::cout << "2. 初级药剂师" << std::endl;
    std::cout << "3. 高级药剂师" << std::endl;
    std::cout << "4. 主管药剂师" << std::endl;
    int levelChoice = selectIntCheck(1, 4);
    this->level = static_cast<PharmacistLevel>(levelChoice);

    this->scheduleInfo = inputStringCheck("请输入排班信息: ");

    std::cout << "请输入你现在是否在岗 (1-是, 0-否): " << std::endl;
    int onDutyChoice = selectIntCheck(0, 1);
    this->isOnDuty = (onDutyChoice == 1);

    std::cout << "药剂师注册成功! 您的用户ID是: " << this->userID << std::endl;
    return true;
}

bool Pharmacist::pharmacistSignIn()
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
            std::cout << "药剂师登录成功！" << std::endl;
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

const std::string &Pharmacist::getPharmacistID() const
{
    return pharmacistID;
}
const std::string &Pharmacist::getDepartment() const
{
    return department;
}
PharmacistLevel Pharmacist::getLevel() const
{
    return level;
}
const std::string &Pharmacist::getScheduleInfo() const
{
    return scheduleInfo;
}
int Pharmacist::getReviewCount() const
{
    return reviewCount;
}
int Pharmacist::getDispenseCount() const
{
    return dispenseCount;
}
int Pharmacist::getInventoryManageCount() const
{
    return inventoryManageCount;
}
bool Pharmacist::getIsOnDuty() const
{
    return isOnDuty;
}

void Pharmacist::setPharmacistID(const std::string &id)
{
    this->pharmacistID = id;
}
void Pharmacist::setDepartment(const std::string &dept){
    this->department = dept;
}
void Pharmacist::setLevel(PharmacistLevel lvl){
    this->level = lvl;
}
void Pharmacist::setScheduleInfo(const std::string &info){
    this->scheduleInfo = info;
}
void Pharmacist::setReviewCount(int count){
    this->reviewCount = count;
}
void Pharmacist::setDispenseCount(int count){
    this->dispenseCount = count;
}
void Pharmacist::setInventoryManageCount(int count){
    this->inventoryManageCount = count;
}
void Pharmacist::setIsOnDuty(bool onDuty){
    this->isOnDuty = onDuty;
}
