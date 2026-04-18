/**
 * @file Pharmacist.h
 * @brief 医疗管理系统中的药剂师类
 * @details 该头文件定义了 Pharmacist 类，继承自 User，
 *          用于描述药剂师的基础信息、岗位职责及药房业务接口。
 * @author 李阳旭 / 小组成员
 * @date 2026-4-15
 * @version 1.0
 */

#ifndef PHARMACIST_H
#define PHARMACIST_H

#include "User.h"

/**
 * @brief 药剂师类
 * @details 继承自 User，包含药剂师特有的个人信息、岗位信息及药房业务接口。
 */
class Pharmacist : public User
{
public:
    std::string pharmacistID;     // 药剂师ID
    std::string department;       // 所属药房/科室
    PharmacistLevel level;        // 药剂师等级
    std::string scheduleInfo;     // 排班信息
    int reviewCount = 0;          // 审核处方次数
    int dispenseCount = 0;        // 发药次数
    int inventoryManageCount = 0; // 库存管理次数
    bool isOnDuty = true;         // 是否在岗

    bool isDeleted = false; // 逻辑删除标志（实际删除时设置为 true）


    Pharmacist *next = nullptr; // 链表指针
    Pharmacist *prev = nullptr; // 双向链表前一个节点指针

    // ==================== 构造 / 析构 ====================
    Pharmacist();
    virtual ~Pharmacist();
    bool pharmacistSignUp(int &idCounter); // 药剂师注册接口
    bool pharmacistSignIn();               // 药剂师登录接口

    // ==================== Getter ====================
    const std::string &getPharmacistID() const;
    const std::string &getDepartment() const;
    PharmacistLevel getLevel() const;
    const std::string &getScheduleInfo() const;
    int getReviewCount() const;
    int getDispenseCount() const;
    int getInventoryManageCount() const;
    bool getIsOnDuty() const;

    // ==================== Setter ====================
    void setPharmacistID(const std::string &id);
    void setDepartment(const std::string &dept);
    void setLevel(PharmacistLevel lvl);
    void setScheduleInfo(const std::string &info);
    void setReviewCount(int count);
    void setDispenseCount(int count);
    void setInventoryManageCount(int count);
    void setIsOnDuty(bool onDuty);

    // // ==================== 统计辅助 ====================
    // void increaseReviewCount();
    // void increaseDispenseCount();
    // void increaseInventoryManageCount();

    // // ==================== 文件操作 ====================
    // bool loadFromFile(const std::string &pharmacistID);
    // bool saveToFile() const;
    // bool updateToFile() const;
    // bool deleteFromFile();

    // // ==================== 药房业务接口 ====================
    // /**
    //  * @brief 审核用药记录
    //  * @param recordID 用药记录ID
    //  * @return 审核成功返回 true，否则返回 false
    //  */
    // bool reviewMedicationRecord(const std::string &recordID);

    // /**
    //  * @brief 发放药品
    //  * @param prescriptionID 处方ID
    //  * @return 发药成功返回 true，否则返回 false
    //  */
    // bool dispenseMedicine(const std::string &prescriptionID);

    // /**
    //  * @brief 增加药品库存
    //  * @param medicineID 药品ID
    //  * @param amount 增加数量
    //  * @return 操作成功返回 true，否则返回 false
    //  */
    // bool addMedicineStock(const std::string &medicineID, int amount);

    // /**
    //  * @brief 减少药品库存
    //  * @param medicineID 药品ID
    //  * @param amount 减少数量
    //  * @return 操作成功返回 true，否则返回 false
    //  */
    // bool reduceMedicineStock(const std::string &medicineID, int amount);

    // /**
    //  * @brief 查看药剂师工作量
    //  */
    // void showWorkload() const;

    // /**
    //  * @brief 查看排班信息
    //  */
    // void showSchedule() const;

    // std::string getRoleName() const;
};

#endif // PHARMACIST_H