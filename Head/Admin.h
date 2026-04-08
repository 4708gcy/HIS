/**
 * @file Admin.h
 * @brief 管理员类的定义
 * @details 该头文件定义了一个 Admin 类，继承自 User 类，包含管理员特有的功能和权限管理方法
 * @author 郭承宇
 * @date 2026-4-6
 * @version 1.0
 */

#ifndef ADMIN_H
#define ADMIN_H

#include <string>
#include <vector>
#include "User.h"

/**
 * @brief 管理端人员节点（医生/护士/药师/患者统一结构）
 */
struct StaffNode
{
    std::string userID;         // 账号ID
    std::string username;       // 用户名
    std::string hashedPassword; // 密码哈希
    std::string department;     // 科室（患者可为空）
    bool isActive = true;       // 是否启用

    StaffNode *prev = nullptr; // 双向链表前指针
    StaffNode *next = nullptr; // 双向链表后指针
};

class Admin : public User
{
private:

    StaffNode *doctorHead = nullptr;     // 医生链表头
    StaffNode *nurseHead = nullptr;      // 护士链表头
    StaffNode *pharmacistHead = nullptr; // 药师链表头
    StaffNode *patientHead = nullptr;    // 患者链表头

public:
    Admin() = default; // 默认构造函数

    ~Admin() override;

    void loadFromFile(const std::string &path) override; // 加载管理员档案
    void saveToFile(const std::string &path) override;   // 保存管理员档案

    // 人员链表加载/释放
    void loadAllStaffLists();
    void clearAllStaffLists();

    // 人员管理（管理员创建账号）
    bool addDoctorAccount(const std::string &id, const std::string &name, const std::string &plainPassword, const std::string &department);
    bool addNurseAccount(const std::string &id, const std::string &name, const std::string &plainPassword, const std::string &department);
    bool addPharmacistAccount(const std::string &id, const std::string &name, const std::string &plainPassword, const std::string &department);
    bool addPatientAccount(const std::string &id, const std::string &name, const std::string &plainPassword);

    // 封号/解锁账号
    bool unlockUser(const std::string &targetUserID);
    bool lockUser(const std::string &targetUserID); 

    // 按科室列出人员
    std::vector<std::string> listDoctorsByDepartment(const std::string &department) const;
    std::vector<std::string> listNursesByDepartment(const std::string &department) const;
    std::vector<std::string> listPharmacistsByDepartment(const std::string &department) const;

    // 医疗/药品/患者查询
    std::vector<std::string> listMedicinesByDepartment(const std::string &department) const;
    std::vector<std::string> listRecordsByDepartment(const std::string &department, const std::string &recordType) const;
    bool updatePatientProfile(const std::string &patientID, const std::string &field, const std::string &newValue);
    bool deletePatient(const std::string &patientID);
    std::vector<std::string> searchPatients(const std::string &keyword) const;

    // 报表
    void queryHospitalOverview() const;
    void queryDepartmentStats(const std::string &department) const;
    void queryFinanceReport() const;

};

#endif // ADMIN_H