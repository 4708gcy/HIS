#include "../Head/User.h"
#include <fstream>
#include <sstream>
#include <random>
#include <algorithm>
#include <iostream>

// 1 - Admin, 2 - Doctor, 3 - Nurse, 4 - Pharmacist, 5 - Patient
bool User::signUp(int choice, int &idCounter)
{
    switch (choice)
    {
    case 1:
    {
        std::string apiKey = inputStringCheck("请输入管理员注册API密钥: ");

        if (apiKey == "88888888")
        {
            role = UserRole::ADMIN;
            userID = "0" + std::to_string(idCounter++).insert(0, 5 - std::to_string(idCounter).length(), '0');
            salt = generateSalt();
            MyTime &myTime = MyTime::getInstance();
            createTime = myTime.getTime();
            isAccountActive = true; // 管理员账户默认激活

            username = inputStringCheck("请输入管理员姓名: ");

            std::string password = inputPwdCheck("请输入密码: ");

            storedHash = SHA256Encrypt(password, salt, kHashIterations);

            return true; // 管理员注册成功
        }
        else
        {
            std::cout << "无效的 API 密钥！" << std::endl;
        }
        return false; // 管理员注册失败
    }
    case 2:
    {
        role = UserRole::DOCTOR;
        userID = "1" + std::to_string(idCounter++).insert(0, 5 - std::to_string(idCounter).length(), '0');
        salt = generateSalt();
        MyTime &myTime = MyTime::getInstance();
        createTime = myTime.getTime();
        isAccountActive = true; // 医生账户默认激活

        username = inputStringCheck("请输入医生姓名: ");
        std::string password = inputPwdCheck("请输入密码: ");
        storedHash = SHA256Encrypt(password, salt, kHashIterations);
        std::cout << "医生注册成功! 您的用户ID是: " << userID << std::endl;
        return true; // 医生注册成功
    }
    case 3:
    {
        role = UserRole::NURSE;
        userID = "2" + std::to_string(idCounter++).insert(0, 5 - std::to_string(idCounter).length(), '0');
        salt = generateSalt();
        MyTime &myTime = MyTime::getInstance();
        createTime = myTime.getTime();
        isAccountActive = true; // 护士账户默认激活
        username = inputStringCheck("请输入护士姓名: ");
        std::string password = inputPwdCheck("请输入密码: ");
        storedHash = SHA256Encrypt(password, salt, kHashIterations);

        std::cout << "护士注册成功! 您的用户ID是: " << userID << std::endl;

        return true; // 护士注册成功
    }
    case 4:
    {
        role = UserRole::PHARMACIST;
        userID = "3" + std::to_string(idCounter++).insert(0, 5 - std::to_string(idCounter).length(), '0');
        salt = generateSalt();
        MyTime &myTime = MyTime::getInstance();
        createTime = myTime.getTime();
        isAccountActive = true; // 药剂师账户默认激活
        username = inputStringCheck("请输入药剂师姓名: ");
        std::string password = inputPwdCheck("请输入密码: ");
        storedHash = SHA256Encrypt(password, salt, kHashIterations);

        std::cout << "药剂师注册成功! 您的用户ID是: " << userID << std::endl;

        return true; // 药剂师注册成功
    }
    case 5:
    {
        role = UserRole::PATIENT;
        userID = "4" + std::to_string(idCounter++).insert(0, 5 - std::to_string(idCounter).length(), '0');
        salt = generateSalt();
        MyTime &myTime = MyTime::getInstance();
        createTime = myTime.getTime();
        isAccountActive = true; // 患者账户默认激活
        username = inputStringCheck("请输入患者姓名: ");
        std::string password = inputPwdCheck("请输入密码: ");
        storedHash = SHA256Encrypt(password, salt, kHashIterations);
        std::cout << "患者注册成功! 您的用户ID是: " << userID << std::endl;
        return true; // 患者注册成功
    }
    default:
        std::cout << "无效的选择！" << std::endl;
    }

    return false; // 注册失败
}

// 只有管理员可以获取迭代次数，其他角色返回 -1 表示不可用
int User::getKHashIterations() const
{
    return role == UserRole::ADMIN ? kHashIterations : -1;
}
// 返回登录状态
bool User::getIsLoggedIn() const { return isLoggedIn; }
// 返回账户是否激活/未锁定
bool User::getIsAccountActive() const { return isAccountActive; }
// 返回连续登录失败次数
int User::getLoginAttempts() const { return loginAttempts; }
// 返回用户ID
const std::string &User::getUserID() const { return userID; }
// 返回用户姓名
const std::string &User::getUsername() const { return username; }
// 返回用户的身份角色
UserRole User::getRole() const { return role; }
// 返回账户创建时间字符串
const std::string &User::getCreateTime() const { return createTime; }
// 返回盐值
std::string User::getSalt() const { return salt; }
// 返回存储的密码哈希值
const std::string &User::getStoredHash() const { return storedHash; }
// 返回逻辑删除标志
bool User::getIsDeleted() const { return isDeleted; }

// 设置姓名
void User::setUsername(const std::string &uname) { username = uname; }
// 设置用户ID
void User::setUserID(const std::string &uid) { userID = uid; }
// 设置盐值
void User::setSalt(const std::string &s) { salt = s; }
// 设置存储的密码哈希值
void User::setStoredHash(const std::string &hash) { storedHash = hash; }
// 设置账户激活状态
void User::setIsAccountActive(bool active) { isAccountActive = active; }
// 设置角色
void User::setRole(UserRole r) { role = r; }
// 设置账户创建时间字符串
void User::setCreateTime(const std::string &time) { createTime = time; }
// 设置逻辑删除标志
void User::setIsDeleted(bool deleted) { isDeleted = deleted; }

// 将挂号状态枚举转换为字符串表示
std::string User::regStatusToString(RegistrationStatus status)
{
    switch (status)
    {
    case RegistrationStatus::BOOKED:
        return "已预约";
    case RegistrationStatus::PAID:
        return "已支付";
    case RegistrationStatus::CANCELED:
        return "已取消";
    case RegistrationStatus::FINISHED:
        return "已完成";
    default:
        return "未知状态";
    }
}

// 将看诊状态枚举转换为字符串表示
std::string User::conStatusToString(ConsultationStatus status)
{
    switch (status)
    {
    case ConsultationStatus::PENDING:
        return "待就诊";
    case ConsultationStatus::IN_PROGRESS:
        return "正在处理";
    case ConsultationStatus::COMPLETED:
        return "已结束";
    case ConsultationStatus::VOIDED:
        return "已作废";
    default:
        return "未知状态";
    }
}

// 将检查状态枚举转换为字符串表示
std::string User::examStatusToString(ExaminationStatus status)
{
    switch (status)
    {
    case ExaminationStatus::ORDERED:
        return "已下单";
    case ExaminationStatus::PAID:
        return "已支付";
    case ExaminationStatus::IN_PROGRESS:
        return "检查中";
    case ExaminationStatus::COMPLETED:
        return "检查完成";
    case ExaminationStatus::REPORTED:
        return "报告已出";
    case ExaminationStatus::VOIDED:
        return "已作废";
    default:
        return "未知状态";
    }
}
// 将住院状态枚举转换为字符串表示
std::string User::hosStatusToString(HospitalizationStatus status)
{
    switch (status)
    {
    case HospitalizationStatus::APPLIED:
        return "申请中";
    case HospitalizationStatus::ADMITTED:
        return "已入院";
    case HospitalizationStatus::DISCHARGED:
        return "已出院";
    case HospitalizationStatus::CANCELED:
        return "已取消";
    default:
        return "未知状态";
    }
}
// 将用药状态枚举转换为字符串表示
std::string User::medicationStatusToString(MedicationStatus status)
{
    switch (status)
    {
    case MedicationStatus::UNPAID:
        return "未支付";
    case MedicationStatus::PAID:
        return "已支付未发药";
    case MedicationStatus::DISPENSED:
        return "已发药";
    case MedicationStatus::RETURNED:
        return "已退药";
    default:
        return "未知状态";
    }
}
// 将用药审核状态枚举转换为字符串表示
std::string User::medicationReviewStatusToString(MedicationReviewStatus status)
{
    switch (status)
    {
    case MedicationReviewStatus::PENDING_REVIEW:
        return "待审核";
    case MedicationReviewStatus::REJECTED:
        return "审核拒绝";
    case MedicationReviewStatus::APPROVED:
        return "审核通过";
    case MedicationReviewStatus::CANCELED:
        return "已撤销";
    default:
        return "未知状态";
    }
}
// 将药品状态枚举转换为字符串表示
std::string User::medicineStatusToString(MedicineStatus status)
{
    switch (status)
    {
    case MedicineStatus::NORMAL:
        return "正常可用";
    case MedicineStatus::LOW_STOCK:
        return "库存低于安全阈值";
    case MedicineStatus::EXPIRED:
        return "已过期";
    case MedicineStatus::DISCONTINUED:
        return "已停用";
    default:
        return "未知状态";
    }
}

// 将体征信息转换为字符串表示
std::string User::findVitalSignToString(Examination* exa)
{
    const auto& vs = exa->vitalSigns;
    const auto& itemName = exa->itemName;

    if (itemName == "体温测量" && vs.temperatureC)
        return "体温(" + std::to_string(*vs.temperatureC) + "°C)";
    if (itemName == "血压测量" && vs.systolicBP && vs.diastolicBP)
        return "血压(" + std::to_string(*vs.systolicBP) + "/" + std::to_string(*vs.diastolicBP) + " mmHg)";
    if (itemName == "心率测量" && vs.heartRate)
        return "心率(" + std::to_string(*vs.heartRate) + "次/分钟)";
    if (itemName == "呼吸频率测量" && vs.respiratoryRate)
        return "呼吸频率(" + std::to_string(*vs.respiratoryRate) + "次/分钟)";
    if (itemName == "脉搏血氧测量" && vs.spo2)
        return "血氧饱和度(" + std::to_string(*vs.spo2) + "%)";
    if (itemName == "身高测量" && vs.height)
        return "身高(" + std::to_string(*vs.height) + "cm)";
    if (itemName == "体重测量" && vs.weight)
        return "体重(" + std::to_string(*vs.weight) + "kg)";
    if (itemName == "BMI计算" && vs.bmi)
        return "BMI(" + std::to_string(*vs.bmi) + ")";
    if (itemName == "疼痛评估" && vs.painScore)
        return "疼痛评分(" + std::to_string(*vs.painScore) + ")";
    if (itemName == "腰围测量" && vs.waistCircumference)
        return "腰围(" + std::to_string(*vs.waistCircumference) + "cm)";
    if (itemName == "血糖测量" && vs.bloodSugar)
        return "血糖(" + std::to_string(*vs.bloodSugar) + "mmol/L)";
    if (itemName == "体脂测量" && vs.bodyFat)
        return "体脂率(" + std::to_string(*vs.bodyFat) + "%)";
    if (itemName == "尿酸测定" && vs.uricAcid)
        return "尿酸(" + std::to_string(*vs.uricAcid) + "μmol/L)";
    if (itemName == "血脂测定" && vs.cholesterol)
        return "总胆固醇(" + std::to_string(*vs.cholesterol) + "mmol/L)";

    return "无体征信息";
}

User::~User()
{
}
