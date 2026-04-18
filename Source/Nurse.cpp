/**
 * @file Nurse.cpp
 * @brief 医疗管理系统中的护士类实现
 * @details 该源文件实现了 Nurse 类的构造、析构、文件操作方法以及护士业务接口。
 * @author 李阳旭 / 小组成员 郭承宇 / 小组成员
 * @date 2026-4-16
 * @version 1.0
 */

#include "../Head/Nurse.h"

Nurse::Nurse()
{
    // 构造函数实现（如有需要）
}

Nurse::~Nurse()
{
    // 析构函数实现（如有需要）
}

bool Nurse::nurseSignUp(int &idCounter){
    bool success = signUp(3, idCounter); // 调用基类的注册方法，传入角色类型 3（护士）
    if (!success)
    {
        std::cout << "护士注册失败！" << std::endl;
        return false;
    }

    this->nurseID = this->userID; // 护士ID与用户ID保持一致
    
    // 其他护士特有信息的初始化在这里进行
    this->department = inputDepartmentCheck("请输入所属科室: ");

    std::cout << "请选择护士等级:" << std::endl;
    std::cout << "1. 实习护士" << std::endl;
    std::cout << "2. 初级护士" << std::endl;
    std::cout << "3. 高级护士" << std::endl;
    std::cout << "4. 护士长" << std::endl;
    int levelChoice = selectIntCheck(1, 4);
    this->level = static_cast<NurseLevel>(levelChoice);

    this->scheduleInfo = inputStringCheck("请输入排班信息: ");

    std::cout << "请输入你现在是否在岗 (1-是, 0-否): " << std::endl;
    int onDutyChoice = selectIntCheck(0, 1);
    this->isOnDuty = (onDutyChoice == 1);

    std::cout << "护士注册成功! 您的用户ID是: " << this->userID << std::endl;
    return true;
}

bool Nurse::nurseSignIn()
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
            std::cout << "护士登录成功！" << std::endl;
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

const std::string &Nurse::getNurseID() const
{
    return nurseID;
}
const std::string &Nurse::getDepartment() const
{
    return department;
}
NurseLevel Nurse::getLevel() const
{
    return level;
}
const std::string &Nurse::getScheduleInfo() const
{
    return scheduleInfo;
}
int Nurse::getPatientCareCount() const
{
    return patientCareCount;
}
int Nurse::getBedManageCount() const
{
    return bedManageCount;
}
bool Nurse::getIsOnDuty() const
{
    return isOnDuty;
}

void Nurse::setNurseID(const std::string &id){
    this->nurseID = id;
}
void Nurse::setDepartment(const std::string &dept){
    this->department = dept;
}
void Nurse::setLevel(NurseLevel lvl){
    this->level = lvl;
}
void Nurse::setScheduleInfo(const std::string &schedule){
    this->scheduleInfo = schedule;
}
void Nurse::setPatientCareCount(int count){
    this->patientCareCount = count;
}
void Nurse::setBedManageCount(int count){
    this->bedManageCount = count;
}
void Nurse::setIsOnDuty(bool onDuty){
    this->isOnDuty = onDuty;
}
