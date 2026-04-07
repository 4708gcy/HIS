/** 
 * @file Pharmacist.h
 * @brief 药师类的定义
 * @details 该头文件定义了一个 Pharmacist 类，继承自 User 类，包含药师特有的功能和权限管理方法
 * @author 郭承宇
 * @date 2026-4-7
 * @version 1.0
 */

#ifndef PHARMACIST_H
#define PHARMACIST_H

#include <string>
#include "User.h"

/**
 * @file Pharmacist.h
 * @brief 药师类，包含药房属性与发药/审核方法声明
 */

class Pharmacist : public User
{
private:
    std::string pharmacyDepartment;   // 药房类型（门诊/住院）
    std::string qualificationLevel;   // 资质等级
    bool canAuditSpecialDrug = false; // 是否可审核专科/限制药
    std::string userDataPath;         // Data/UserData/PharmacistChainData/...

public:
    explicit Pharmacist(const std::string &pharmacistID,
                        const std::string &dataPath = "Data/UserData/PharmacistChainData/pharmacist_users.txt")
        : User(pharmacistID, UserRole::PHARMACIST, true), userDataPath(dataPath)
    {
        loadFromFile();
    }

    void loadFromFile() override;
    void saveToFile() override;

    // 药师业务
    bool auditPrescription(const std::string &prescriptionID);
    bool dispenseMedicine(const std::string &prescriptionID);
    bool confirmDrugReturn(const std::string &prescriptionID, const std::string &medicineID, int quantity);

    const std::string &getPharmacyDepartment() const { return pharmacyDepartment; }
    const std::string &getQualificationLevel() const { return qualificationLevel; }
    bool getCanAuditSpecialDrug() const { return canAuditSpecialDrug; }

    void setPharmacyDepartment(const std::string &v) { pharmacyDepartment = v; }
    void setQualificationLevel(const std::string &v) { qualificationLevel = v; }
    void setCanAuditSpecialDrug(bool v) { canAuditSpecialDrug = v; }
};

#endif // PHARMACIST_H