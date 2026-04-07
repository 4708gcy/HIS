/** 
 * @file Patient.h
 * @brief 患者类的定义
 * @details 该头文件定义了一个 Patient 类，继承自 User 类，包含患者特有的功能和权限管理方法
 * @author 郭承宇
 * @date 2026-4-7
 * @version 1.0
 */

#ifndef PATIENT_H
#define PATIENT_H

#include <string>
#include "User.h"

/**
 * @file Patient.h
 * @brief 患者类，包含身份证、联系电话、过敏史等属性
 */

class Patient : public User
{
private:
    std::string idCardNo;         // 身份证号
    std::string phone;            // 联系电话
    std::string allergyHistory;   // 过敏史
    std::string emergencyContact; // 紧急联系人信息
    std::string insuranceNo;      // 医保号
    std::string userDataPath;     // Data/UserData/PatientChainData/patient_users.txt

public:
    explicit Patient(const std::string &patientID,
                     const std::string &dataPath = "Data/UserData/PatientChainData/patient_users.txt")
        : User(patientID, UserRole::PATIENT, true), userDataPath(dataPath)
    {
        loadFromFile();
    }

    void loadFromFile() override; // 从 patient_users.txt 中读取个人字段
    void saveToFile() override;   // 保存个人字段回文件

    // 患者业务
    bool bookRegistration(const std::string &department, const std::string &doctorID);
    void queryMyTimeline() const; // 打印个人就诊时间轴（遍历链表）
    void queryMyCost() const;     // 统计个人费用（遍历挂号/住院账单）

    const std::string &getIdCardNo() const { return idCardNo; }
    const std::string &getPhone() const { return phone; }
    const std::string &getAllergyHistory() const { return allergyHistory; }
    const std::string &getEmergencyContact() const { return emergencyContact; }
    const std::string &getInsuranceNo() const { return insuranceNo; }

    void setIdCardNo(const std::string &v) { idCardNo = v; }
    void setPhone(const std::string &v) { phone = v; }
    void setAllergyHistory(const std::string &v) { allergyHistory = v; }
    void setEmergencyContact(const std::string &v) { emergencyContact = v; }
    void setInsuranceNo(const std::string &v) { insuranceNo = v; }
};

#endif // PATIENT_H