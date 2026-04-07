/**
 * @file Doctor.h
 * @brief 医生类的定义
 * @details 该头文件定义了一个 Doctor 类，继承自 User 类，包含医生特有的功能和权限管理方法
 * @author 郭承宇
 * @date 2026-4-7
 * @version 1.0
 */

#ifndef DOCTOR_H
#define DOCTOR_H

#include <string>
#include "User.h"

class Doctor : public User
{
private:
    std::string department;
    std::string title;     // 主治/副主任等
    std::string licenseNo; // 执业证编号
    std::string specialty; // 擅长方向
    bool onDuty = false;
    std::string userDataPath; // Data/UserData/DoctorChainData/doctor_users.txt

public:
    explicit Doctor(const std::string &doctorID,
                    const std::string &dataPath = "Data/UserData/DoctorChainData/doctor_users.txt")
        : User(doctorID, UserRole::DOCTOR, true), userDataPath(dataPath)
    {
        loadFromFile();
    }

    void loadFromFile() override;
    void saveToFile() override;

    // 业务动作
    bool createConsultation(const std::string &patientID, const std::string &regID);
    bool createExamination(const std::string &consultationID, const std::string &itemName);
    bool applyHospitalization(const std::string &patientID, const std::string &dept, const std::string &wardType);

    const std::string &getDepartment() const { return department; }
    const std::string &getTitle() const { return title; }
    const std::string &getLicenseNo() const { return licenseNo; }
    const std::string &getSpecialty() const { return specialty; }
    bool getOnDuty() const { return onDuty; }

    void setDepartment(const std::string &v) { department = v; }
    void setTitle(const std::string &v) { title = v; }
    void setLicenseNo(const std::string &v) { licenseNo = v; }
    void setSpecialty(const std::string &v) { specialty = v; }
    void setOnDuty(bool v) { onDuty = v; }
};

#endif