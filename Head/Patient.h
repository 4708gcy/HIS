/**
 * @file Patient.h
 * @brief 医疗管理系统中的患者类
 * @details 该头文件定义了 Patient 类，继承自 User，
 *          用于描述患者的基础信息、就诊信息及相关业务接口。
 * @author 李阳旭 / 小组成员
 * @date 2026-4-14
 * @version 1.0
 */

#ifndef PATIENT_H
#define PATIENT_H

#include "User.h"

/**
 * @brief 患者类
 * @details 继承自 User，包含患者特有的个人信息、联系人信息、
 *          病史摘要及就医相关操作接口。
 */
class Patient : public User
{
public:
    std::string patientID;             // 患者ID（语义区分）
    std::string department = "急诊科"; // 就诊科室(默认急诊科)
    std::string address;               // 家庭住址
    std::string idCardNumber;          // 身份证号
    std::string emergencyContactName;  // 紧急联系人姓名
    std::string emergencyContactPhone; // 紧急联系人电话
    std::string allergyHistory;        // 过敏史
    std::string pastMedicalHistory;    // 既往病史
    MaritalStatus maritalStatus;       // 婚姻状况
    int registrationCount = 0;         // 挂号次数
    int consultationCount = 0;         // 就诊次数
    int hospitalizationCount = 0;      // 住院次数
    int medicationCount = 0;           // 用药记录次数

    bool isHospitalized = false; // 是否住院中

    bool isDeleted = false; // 逻辑删除标志（实际删除时设置为 true）

    Patient *next = nullptr; // 链表指针
    Patient *prev = nullptr; // 双向链表前一个节点指针

    // ==================== 构造 / 析构 ====================
    Patient();
    virtual ~Patient();
    void patientSignUp(int &idCounter); // 患者注册接口
    bool patientSignIn();               // 患者登录接口

    // ==================== Getter ====================
    std::string getPatientID();
    std::string getAddress();
    std::string getIdCardNumber();
    std::string getEmergencyContactName();
    std::string getEmergencyContactPhone();
    std::string getAllergyHistory();
    std::string getPastMedicalHistory();
    MaritalStatus getMaritalStatus();
    int getRegistrationCount();
    int getConsultationCount();
    int getHospitalizationCount();
    int getMedicationCount();

    // ==================== Setter ====================
    void setPatientID(const std::string &id);
    void setAddress(const std::string &addr);
    void setIdCardNumber(const std::string &idCard);
    void setEmergencyContactName(const std::string &name);
    void setEmergencyContactPhone(const std::string &phone);
    void setAllergyHistory(const std::string &history);
    void setPastMedicalHistory(const std::string &history);
    void setMaritalStatus(MaritalStatus status);
    void setRegistrationCount(int count);
    void setConsultationCount(int count);
    void setHospitalizationCount(int count);
    void setMedicationCount(int count);

    // // ==================== 统计辅助 ====================
    // void increaseRegistrationCount();
    // void increaseConsultationCount();
    // void increaseHospitalizationCount();
    // void increaseMedicationCount();

    // // ==================== 文件操作 ====================
    // bool loadFromFile(const std::string &patientID);
    // bool saveToFile() const;
    // bool updateToFile() const;
    // bool deleteFromFile();

    // // ==================== 患者业务接口 ====================

    // /**
    //  * @brief 挂号
    //  * @param department 科室名称
    //  * @param doctorID 医生ID
    //  * @return 挂号成功返回 true，否则返回 false
    //  */
    // bool registerForConsultation(const std::string &department, const std::string &doctorID);

    // /**
    //  * @brief 缴纳挂号/住院/治疗相关费用
    //  * @param recordID 业务记录ID
    //  * @param amount 缴费金额
    //  * @return 缴费成功返回 true，否则返回 false
    //  */
    // bool payFee(const std::string &recordID, double amount);

    // /**
    //  * @brief 查看个人挂号记录
    //  */
    // void showRegistrationRecords() const;

    // /**
    //  * @brief 查看个人看诊记录
    //  */
    // void showConsultationRecords() const;

    // /**
    //  * @brief 查看个人检查记录
    //  */
    // void showExaminationRecords() const;

    // /**
    //  * @brief 查看个人住院记录
    //  */
    // void showHospitalizationRecords() const;

    // /**
    //  * @brief 查看个人用药记录
    //  */
    // void showMedicationRecords() const;

    // /**
    //  * @brief 查看个人基本信息
    //  */
    // void showPersonalInfo() const;

    // /**
    //  * @brief 获取角色名称
    //  * @return 返回字符串 "Patient"
    //  */
    // std::string getRoleName() const;
};

#endif // PATIENT_H