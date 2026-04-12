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

// 包含必要的头文件
#include <string>
#include <vector>
#include "SHA-256.h"
#include "GetTime.h"
#include "Registration.h"
#include "Consultation.h"
#include "Examination.h"
#include "Hospitalization.h"
#include "MedicationRecord.h"
#include "Medicine.h"
#include "UI.h"

// 定义存储人物信息文件的路径
#define ADMIN_FILE "../Data/UserData/AdminChainData/admin_users.txt"
#define DOCTOR_FILE "../Data/UserData/DoctorChainData/doctor_users.txt"
#define NURSE_FILE "../Data/UserData/NurseChainData/nurse_users.txt"
#define PHARMACIST_FILE "../Data/UserData/PharmacistChainData/pharmacist_users.txt"
#define PATIENT_FILE "../Data/UserData/PatientChainData/patient_users.txt"

// 定义存储医疗记录文件的路径
#define REGISTRATION_FILE "../Data/RecordData/RegistrationChainData/registrations.txt"
#define CONSULTATION_FILE "../Data/RecordData/ConsultationChainData/consultations.txt"
#define EXAMINATION_FILE "../Data/RecordData/ExaminationChainData/examinations.txt"
#define HOSPITALIZATION_FILE "../Data/RecordData/HospitalizationChainData/hospitalizations.txt"
#define MEDICATION_RECORD_FILE "../Data/RecordData/MedicineChainData/medication_records.txt"
#define MEDICINE_FILE "../Data/RecordData/MedicineChainData/medicines.txt"
#define BED_FILE "../Data/RecordData/HospitalizationChainData/bed_info.txt"

// 定义全局常量
#define failedLoginLimit 5  // 连续登录失败次数限制
#define hashIterations 1000 // 密码哈希迭代次数

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

// 人物的ID一共6位数字，前一位表示身份（0-管理员，1-医生，2-护士，3-药剂师，4-患者），后五位为递增数字

class User
{
protected:
    bool isLoggedIn = false;      // 登录状态
    std::string userID;           // 用户唯一ID
    std::string username;         // 显示用户名
    std::string storedHash;       // 存储的密码串（格式 salt$hash）
    std::string salt;             // 密码盐值
    int loginAttempts = 0;        // 连续失败次数
    bool isAccountActive = false; // 账户是否被激活/未锁定
    UserRole role;                // 角色类型
    std::string createTime;       // 账户创建时间字符串

    bool isDeleted = false; // 逻辑删除标志（实际删除时设置为 true）

    static constexpr int kMaxLoginAttempts = failedLoginLimit; // 锁定门槛
    static constexpr int kHashIterations = hashIterations;     // 哈希迭代次数

public:
    // 注册新用户 : 1 - Admin, 2 - Doctor, 3 - Nurse, 4 - Pharmacist, 5 - Patient
    bool signUp(int choice, int &idCounter); // 注册新用户

    virtual ~User(); // 虚析构函数，确保子类资源正确释放

    bool getIsLoggedIn() const;
    bool getIsAccountActive() const;
    int getLoginAttempts() const;
    const std::string &getUserID() const;
    const std::string &getUsername() const;
    const std::string &getStoredHash() const;
    UserRole getRole() const;
    const std::string &getCreateTime() const;
    int getKHashIterations() const;
    std::string getSalt() const;
    bool getIsDeleted() const;

    void setUserID(const std::string &uid);
    void setUsername(const std::string &uname);
    void setSalt(const std::string &s);
    void setStoredHash(const std::string &hash);
    void setIsAccountActive(bool active);
    void setRole(UserRole r);
    void setCreateTime(const std::string &time);
    void setIsDeleted(bool deleted);

    std::string regStatusToString(RegistrationStatus status);                  // 将挂号状态枚举转换为字符串表示
    std::string conStatusToString(ConsultationStatus status);                  // 将看诊状态枚举转换为字符串表示
    std::string examStatusToString(ExaminationStatus status);                  // 将检查状态枚举转换为字符串表示
    std::string hosStatusToString(HospitalizationStatus status);               // 将住院状态枚举转换为字符串表示
    std::string medicationStatusToString(MedicationStatus status);             // 将用药状态枚举转换为字符串表示
    std::string medicationReviewStatusToString(MedicationReviewStatus status); // 将用药审核状态枚举转换为字符串表示
    std::string medicineStatusToString(MedicineStatus status);                 // 将药品状态枚举转换为字符串表示
    std::string findVitalSignToString(Examination* exa); // 将体征信息转换为字符串表示
    std::string bedStatusToString(bedStatus status); // 将床位状态枚举转换为字符串表示
};

#endif // USER_H