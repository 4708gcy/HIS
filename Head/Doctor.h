/**
 * @file Doctor.h
 * @brief 医疗管理系统中的医生类
 * @details 该头文件定义了 Doctor 类，继承自 User，
 *          用于描述医生的基础信息、科室信息、排班信息及其业务操作接口。
 * @author 李阳旭 / 小组成员
 * @date 2026-4-14
 * @version 1.0
 */

#ifndef DOCTOR_H
#define DOCTOR_H

#include "User.h"


/**
 * @brief 医生类
 * @details 继承自 User，包含医生特有的职称、所属科室、排班信息、
 *          接诊数量统计等内容，并提供医生角色菜单及相关业务接口。
 */
class Doctor : public User
{
public:
    std::string doctorID;              // 医生ID（一般与 userID 一致，单独保留便于语义区分）
    std::string department;            // 所属科室
    DoctorTitle title;                 // 医生职称
    std::string specialty;             // 擅长方向
    std::string scheduleInfo;          // 排班信息
    int consultationCount = 0;         // 累计接诊次数
    int examinationCount = 0;          // 累计开具检查次数
    int hospitalizationApplyCount = 0; // 累计发起住院申请次数
    bool isOnDuty = false;             // 当前是否在岗

    bool isDeleted = false; // 逻辑删除标志（实际删除时设置为 true）


    Doctor *next = nullptr; // 链表指针
    Doctor *prev = nullptr; // 双向链表前向指针

    // ==================== 构造 / 析构 ====================
    Doctor();
    virtual ~Doctor();
    void doctorSignUp(int &idCounter); // 医生注册，调用基类的 signUp(2) 方法，并将医生信息保存到文件中
    bool doctorSignIn();               // 医生登录，验证用户ID和密码，并设置登录状态

    // ==================== Getter ====================
    const std::string &getDoctorID() const;
    const std::string &getDepartment() const;
    DoctorTitle getTitle() const;
    const std::string &getSpecialty() const;
    const std::string &getScheduleInfo() const;
    int getConsultationCount() const;
    int getExaminationCount() const;
    int getHospitalizationApplyCount() const;
    bool getIsOnDuty() const;

    // ==================== Setter ====================
    void setDoctorID(const std::string &id);
    void setDepartment(const std::string &dept);
    void setTitle(DoctorTitle doctorTitle);
    void setSpecialty(const std::string &spec);
    void setScheduleInfo(const std::string &schedule);
    void setConsultationCount(int count);
    void setExaminationCount(int count);
    void setHospitalizationApplyCount(int count);
    void setIsOnDuty(bool onDuty);

    // ==================== 业务计数辅助 ====================
    void increaseConsultationCount();
    void increaseExaminationCount();
    void increaseHospitalizationApplyCount();

    // ==================== 文件读写 ====================
    bool loadFromFile(const std::string &doctorID);
    bool saveToFile() const;
    bool updateToFile() const;
    bool deleteFromFile();

    // ==================== 医生业务接口 ====================

    /**
     * @brief 接诊患者
     * @param patientID 患者ID
     * @param registrationID 挂号记录ID
     * @return 接诊成功返回 true，否则返回 false
     */
    bool receivePatient(const std::string &patientID, const std::string &registrationID);

    /**
     * @brief 开具检查单
     * @param patientID 患者ID
     * @param consultationID 看诊记录ID
     * @return 开具成功返回 true，否则返回 false
     */
    bool createExamination(const std::string &patientID, const std::string &consultationID);

    /**
     * @brief 开具处方
     * @param patientID 患者ID
     * @param consultationID 看诊记录ID
     * @return 开具成功返回 true，否则返回 false
     */
    bool createMedicationRecord(const std::string &patientID, const std::string &consultationID);

    /**
     * @brief 发起住院申请
     * @param patientID 患者ID
     * @param consultationID 看诊记录ID
     * @return 发起成功返回 true，否则返回 false
     */
    bool applyHospitalization(const std::string &patientID, const std::string &consultationID);

    /**
     * @brief 查看个人接诊工作量
     */
    void showWorkload() const;

    /**
     * @brief 查看个人排班信息
     */
    void showSchedule() const;


    /**
     * @brief 获取角色名称
     * @return 返回字符串 "Doctor"
     */
    std::string getRoleName() const;
};


#endif // DOCTOR_H
