/** 
 * @file Nurse.h
 * @brief 护士类的定义
 * @details 该头文件定义了一个 Nurse 类，继承自 User 类，包含护士特有的功能和权限管理方法
 * @author 郭承宇
 * @date 2026-4-7
 * @version 1.0
 */

#ifndef NURSE_H
#define NURSE_H

#include <string>
#include "User.h"

class Nurse : public User
{
private:
    std::string department;
    std::string level;        // N1/N2/主管护师
    std::string shift;        // 白班/夜班
    std::string skillTag;     // 如 ICU, PICC
    std::string userDataPath; // Data/UserData/NurseChainData/nurse_users.txt

public:
    explicit Nurse(const std::string &nurseID,
                   const std::string &dataPath = "Data/UserData/NurseChainData/nurse_users.txt")
        : User(nurseID, UserRole::NURSE, true), userDataPath(dataPath)
    {
        loadFromFile();
    }

    void loadFromFile() override;
    void saveToFile() override;

    bool recordVitals(const std::string &patientID, const std::string &vitalsText);
    bool updateBedRound(const std::string &hospitalizationID, const std::string &note);

    const std::string &getDepartment() const { return department; }
    const std::string &getLevel() const { return level; }
    const std::string &getShift() const { return shift; }
    const std::string &getSkillTag() const { return skillTag; }

    void setDepartment(const std::string &v) { department = v; }
    void setLevel(const std::string &v) { level = v; }
    void setShift(const std::string &v) { shift = v; }
    void setSkillTag(const std::string &v) { skillTag = v; }
};

#endif