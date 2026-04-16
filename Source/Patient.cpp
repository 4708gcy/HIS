/**
 * @file Patient.cpp
 * @brief 医疗管理系统中的患者类实现
 * @details 该源文件实现了 Patient 类的构造、析构、文件操作方法以及患者业务接口。
 * @author 李阳旭 / 小组成员 郭承宇 / 小组成员
 * @date 2026-4-16
 * @version 1.0
 */

#include "../Head/Patient.h"

Patient::Patient()
{
    // 构造函数实现（如有需要）
}

Patient::~Patient()
{
    // 析构函数实现（如有需要）
}

void Patient::patientSignUp(int &idCounter){
    bool success = signUp(5, idCounter); // 调用基类的注册方法，传入角色类型 5（患者）
    if (!success)
    {
        std::cout << "患者注册失败！" << std::endl;
        return;
    }
    this->patientID = this->userID; // 患者ID与用户ID保持一致
    
    // 其他患者特有信息的初始化在这里进行
    this->address = inputStringCheck("请输入家庭住址: ");
    this->idCardNumber = inputIDcardCheck("请输入身份证号: ");
    this->emergencyContactName = inputStringCheck("请输入紧急联系人姓名: ");
    this->emergencyContactPhone = inputTelephoneCheck("请输入紧急联系人电话: ");
    this->allergyHistory = inputStringCheck("请输入过敏史（如无请填写“无”）: ");
    this->pastMedicalHistory = inputStringCheck("请输入既往病史（如无请填写“无”）: ");

    std::cout << "请选择婚姻状况:" << std::endl;
    std::cout << "1. 未婚" << std::endl;
    std::cout << "2. 已婚" << std::endl;
    std::cout << "3. 离异" << std::endl;
    std::cout << "4. 丧偶" << std::endl;
    int maritalChoice = selectIntCheck(1, 4);
    this->maritalStatus = static_cast<MaritalStatus>(maritalChoice);

    std::cout << "患者注册成功! 您的用户ID是: " << this->userID << std::endl;
}

bool Patient::patientSignIn()
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
            std::cout << "患者登录成功！" << std::endl;
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

std::string Patient::getPatientID()
{
    return patientID;
}
std::string Patient::getAddress() 
{
    return address;
}
std::string Patient::getIdCardNumber() 
{
    return idCardNumber;
}
std::string Patient::getEmergencyContactName() 
{
    return emergencyContactName;
}
std::string Patient::getEmergencyContactPhone() 
{
    return emergencyContactPhone;
}
std::string Patient::getAllergyHistory() 
{
    return allergyHistory;
}
std::string Patient::getPastMedicalHistory() 
{
    return pastMedicalHistory;
}
MaritalStatus Patient::getMaritalStatus()
{
    return maritalStatus;
}
int Patient::getRegistrationCount() 
{
    return registrationCount;
}
int Patient::getConsultationCount() 
{
    return consultationCount;
}
int Patient::getHospitalizationCount() 
{
    return hospitalizationCount;
}
int Patient::getMedicationCount() 
{
    return medicationCount;
}

void Patient::setPatientID(const std::string &id)
{
    patientID = id;
}
void Patient::setAddress(const std::string &addr)
{
    address = addr;
}
void Patient::setIdCardNumber(const std::string &idCard)
{
    idCardNumber = idCard;
}
void Patient::setEmergencyContactName(const std::string &name)
{
    emergencyContactName = name;
}
void Patient::setEmergencyContactPhone(const std::string &phone)
{
    emergencyContactPhone = phone;
}
void Patient::setAllergyHistory(const std::string &history)
{
    allergyHistory = history;
}
void Patient::setPastMedicalHistory(const std::string &history)
{
    pastMedicalHistory = history;
}
void Patient::setMaritalStatus(MaritalStatus status)
{
    maritalStatus = status;
}
void Patient::setRegistrationCount(int count)
{
    registrationCount = count;
}
void Patient::setConsultationCount(int count)
{
    consultationCount = count;
}
void Patient::setHospitalizationCount(int count)
{
    hospitalizationCount = count;
}
void Patient::setMedicationCount(int count)
{
    medicationCount = count;
}
