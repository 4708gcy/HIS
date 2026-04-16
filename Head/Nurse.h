/**
 * @file Nurse.h
 * @brief 医疗管理系统中的护士类
 * @details 该头文件定义了 Nurse 类，继承自 User，
 *          用于描述护士的基础信息、科室信息、护理职责及相关业务接口。
 * @author 李阳旭 / 小组成员
 * @date 2026-4-14
 * @version 1.0
 */

#ifndef NURSE_H
#define NURSE_H

#include "User.h"

/**
 * @brief 护士类
 * @details 继承自 User，包含护士特有的等级、科室、护理任务等信息，
 *          并提供护理操作、床位管理及菜单接口。
 */
class Nurse : public User
{
public:
    std::string nurseID;      // 护士ID（语义区分）
    std::string department;   // 所属科室
    NurseLevel level;         // 护士等级
    std::string scheduleInfo; // 排班信息
    int patientCareCount = 0; // 护理患者数量
    int bedManageCount = 0;   // 床位管理次数
    bool isOnDuty = false;    // 是否在岗

    bool isDeleted = false; // 逻辑删除标志（实际删除时设置为 true）

    Nurse *next = nullptr; // 链表指针
    Nurse *prev = nullptr; // 双向链表前一个节点指针

    // ==================== 构造 / 析构 ====================
    Nurse();
    virtual ~Nurse();
    void nurseSignUp(int &idCounter); // 护士注册接口
    bool nurseSignIn();               // 护士登录接口

    // ==================== Getter ====================
    const std::string &getNurseID() const;
    const std::string &getDepartment() const;
    NurseLevel getLevel() const;
    const std::string &getScheduleInfo() const;
    int getPatientCareCount() const;
    int getBedManageCount() const;
    bool getIsOnDuty() const;

    // ==================== Setter ====================
    void setNurseID(const std::string &id);
    void setDepartment(const std::string &dept);
    void setLevel(NurseLevel lvl);
    void setScheduleInfo(const std::string &schedule);
    void setPatientCareCount(int count);
    void setBedManageCount(int count);
    void setIsOnDuty(bool onDuty);

    // // ==================== 统计辅助 ====================
    // void increasePatientCareCount();
    // void increaseBedManageCount();

    // // ==================== 文件操作 ====================
    // bool loadFromFile(const std::string &nurseID);
    // bool saveToFile() const;
    // bool updateToFile() const;
    // bool deleteFromFile();

    // // ==================== 护士业务接口 ====================

    // /**
    //  * @brief 记录患者生命体征
    //  * @param patientID 患者ID
    //  * @param examinationID 检查记录ID
    //  * @return 成功返回 true，否则返回 false
    //  */
    // bool recordVitalSigns(const std::string &patientID, const std::string &examinationID);

    // /**
    //  * @brief 分配床位
    //  * @param patientID 患者ID
    //  * @param bedID 床位ID
    //  * @return 成功返回 true，否则返回 false
    //  */
    // bool assignBed(const std::string &patientID, const std::string &bedID);

    // /**
    //  * @brief 调整床位（转床）
    //  * @param patientID 患者ID
    //  * @param oldBedID 原床位ID
    //  * @param newBedID 新床位ID
    //  * @return 成功返回 true，否则返回 false
    //  */
    // bool transferBed(const std::string &patientID,
    //                  const std::string &oldBedID,
    //                  const std::string &newBedID);

    // /**
    //  * @brief 释放床位（患者出院）
    //  * @param bedID 床位ID
    //  * @return 成功返回 true，否则返回 false
    //  */
    // bool releaseBed(const std::string &bedID);

    // /**
    //  * @brief 查看护理工作量
    //  */
    // void showWorkload() const;

    // /**
    //  * @brief 查看排班信息
    //  */
    // void showSchedule() const;

    // /**
    //  * @brief 获取角色名称
    //  * @return 返回字符串 "Nurse"
    //  */
    // std::string getRoleName() const;
};

#endif // NURSE_H