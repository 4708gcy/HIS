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
 *          并提供护理操作、床位管理、检查记录管理及个人信息管理接口。
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
    bool nurseSignUp(int &idCounter); // 护士注册接口
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

    // ==================== 统计辅助 ====================
    void increasePatientCareCount();
    void increaseBedManageCount();

    // ==================== 检查记录管理 ====================
    bool getAllExaminations(Examination *&exaHead);
    bool getExaminationsByPatientID(Examination *&exaHead);
    bool getExaminationsByConsultationID(Examination *&exaHead);
    bool getExaminationsByID(Examination *&exaHead);
    bool getExaminationsByTimeRange(Examination *&exaHead);
    bool getExaminationsByItemName(Examination *&exaHead);
    bool getExaminationsByStatus(Examination *&exaHead);

    void setExaminationStatus(Examination *&target);
    void setExaminationReport(Examination *&target); // 护士录入/更新体征
    void setExaminationReportSummary(Examination *&target);
    void setExaminationAttachments(Examination *&target);
    void setExaminationNote(Examination *&target);
    void deleteExamination(Examination *&target);
    void manageExaminations(Examination *&exaHead);

    // ==================== 住院/床位管理 ====================
    bool getAllHospitalizations(Hospitalization *&hosHead);
    bool getHospitalizationsByPatientID(Hospitalization *&hosHead);
    bool getHospitalizationsByConsultationID(Hospitalization *&hosHead);
    bool getHospitalizationsByID(Hospitalization *&hosHead);
    bool getHospitalizationsByStatus(Hospitalization *&hosHead);
    bool getHospitalizationsByTimeRange(Hospitalization *&hosHead);

    void setHospitalizationStatus(Hospitalization *&target);
    void assignNurseToHospitalization(Hospitalization *&target);
    void assignBed(Hospitalization *&target, bedInfo *&bedHead);
    void transferBed(Hospitalization *&target, bedInfo *&bedHead);
    void dischargePatient(Hospitalization *&target, bedInfo *&bedHead);
    void deleteHospitalization(Hospitalization *&target);
    void manageHospitalizations(Hospitalization *&hosHead, bedInfo *&bedHead);

    // ==================== 床位信息管理 ====================
    bool getAllBeds(bedInfo *&bedHead);
    bool getBedsByStatus(bedInfo *&bedHead);
    bool getBedsByPatientID(bedInfo *&bedHead);
    bool getBedsByNurseID(bedInfo *&bedHead);
    bool getBedsByDepartment(bedInfo *&bedHead);
    bool getBedByID(bedInfo *&bedHead);

    void setBedStatus(bedInfo *&target);
    void setBedNote(bedInfo *&target);
    void setBedVitalSigns(bedInfo *&target);
    void deleteBed(bedInfo *&target);
    bool createBed(bedInfo *&bedHead, int &idCounter);
    void manageBeds(bedInfo *&bedHead, int &idCounter);

    // ==================== 个人信息管理 ====================
    void managePersonalInfo();
};

#endif // NURSE_H