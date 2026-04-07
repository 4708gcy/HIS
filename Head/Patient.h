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

class Patient : public User
{
private:
    std::string idCardNo;
    std::string phone;
    std::string allergyHistory;
    std::string emergencyContact;
    std::string insuranceNo;
    std::string userDataPath; // Data/UserData/PatientChainData/patient_users.txt

public:
    explicit Patient(const std::string &patientID,
                     const std::string &dataPath = "Data/UserData/PatientChainData/patient_users.txt")
        : User(patientID, UserRole::PATIENT, true), userDataPath(dataPath)
    {
        loadFromFile();
    }

    void loadFromFile() override;
    void saveToFile() override;

    bool bookRegistration(const std::string &department, const std::string &doctorID);
    void queryMyTimeline() const;
    void queryMyCost() const;

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

#endif