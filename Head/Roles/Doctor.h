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

#include "Core/User.h"
#include "Roles/Patient.h"
#include "Core/UI.h"

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

    Doctor *next = nullptr; // 链表指针
    Doctor *prev = nullptr; // 双向链表前向指针

    // ==================== 构造 / 析构 ====================
    Doctor();
    virtual ~Doctor();
    bool doctorSignUp(int &idCounter); // 医生注册，调用基类的 signUp(2) 方法，并将医生信息保存到文件中
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

    // ==================== 医生业务接口 ====================

    bool getAllRegistrations(Registration *&regHead);                                                             // 获取医生的是所有挂号记录
    bool getRegistrationsByStatus(Registration *&regHead);                                                        // 根据挂号状态获取挂号记录
    bool getRegistrationsByPatientID(Registration *&regHead);                                                     // 根据患者ID获取挂号记录
    bool getRegistrationsByTimeRange(Registration *&regHead);                                                     // 根据挂号时间范围获取挂号记录
    bool getRegistrationsByID(Registration *&regHead);                                                            // 根据挂号ID获取挂号记录
    void setRegistrationStatus(Registration *&target);                                                            // 修改挂号记录状态
    void deleteRegistration(Registration *&target);                                                               // 删除挂号记录（逻辑删除）
    bool createRegistrationByPatient(Registration *&regHead, Doctor *&doctor, Patient *&patient, int &idCounter); // 为患者创建新的挂号记录
    void manageRegistrations(Registration *&regHead, Doctor *&doctor, Patient *&patientHead, int &idCounter);     // 管理挂号记录（查看、修改状态等）

    bool getAllConsultations(Consultation *&conHead);                                                                          // 获取医生的所有看诊记录
    bool getConsultationsByStatus(Consultation *&conHead);                                                                     // 根据看诊状态获取看诊记录
    bool getConsultationsByPatientID(Consultation *&conHead);                                                                  // 根据患者ID获取看诊记录
    bool getConsultationsByTimeRange(Consultation *&conHead);                                                                  // 根据看诊时间范围获取看诊记录
    bool getConsultationsByID(Consultation *&conHead);                                                                         // 根据看诊ID获取看诊记录
    void setConsultationStatus(Consultation *&target);                                                                         // 修改看诊记录状态
    void setConsultationChiefComplaint(Consultation *&target);                                                                 // 修改看诊记录的主诉信息
    void setConsultationHistoryOfPresentIllness(Consultation *&target);                                                        // 修改看诊记录的现病史信息
    void setConsultationPastMedicalHistory(Consultation *&target);                                                             // 修改看诊记录的既往史信息
    void setConsultationFamilyHistory(Consultation *&target);                                                                  // 修改看诊记录的家族史信息
    void setConsultationPreliminaryDiagnosis(Consultation *&target);                                                           // 修改看诊记录的初步诊断信息
    void addConsultationExamination(Consultation *&target);                                                                    // 向看诊记录添加检查项目
    void addConsultationPrescription(Consultation *&target, Medicine *&medHead, Patient *patientHead);                         // 向看诊记录添加处方信息
    void initConsultationExamination(Consultation *&target);                                                                   // 初始化看诊记录的检查项目列表
    void initConsultationPrescription(Consultation *&target, Medicine *&medHead, Patient *patientHead);                        // 初始化看诊记录的处方列表（含药物安全检查）
    void setHospitalizationRecommendation(Consultation *&target);                                                              // 设置看诊记录的住院建议
    void deleteConsultation(Consultation *&target);                                                                            // 删除看诊记录（逻辑删除）
    bool createConsultationByRegistration(Registration *&regHead, Consultation *&conHead, Medicine *&medHead, Patient *patientHead, int &idCounter); // 从挂号记录创建看诊记录
    void manageConsultations(Consultation *&conHead, Registration *&regHead, Medicine *&medHead, Patient *patientHead, int &idCounter); // 管理看诊记录

    bool getAllExaminations(Examination *&exaHead);                                                      // 获取医生的所有检查记录
    bool getExaminationsByPatientID(Examination *&exaHead);                                              // 根据患者ID获取检查记录
    bool getExaminationsByConsultationID(Examination *&exaHead);                                         // 根据看诊ID获取检查记录
    bool getExaminationsByID(Examination *&exaHead);                                                     // 根据检查ID获取检查记录
    bool getExaminationsByTimeRange(Examination *&exaHead);                                              // 根据检查时间范围获取检查记录
    bool getExaminationsByItemName(Examination *&exaHead);                                               // 根据检查项目名称获取检查记录
    bool getExaminationsByStatus(Examination *&exaHead);                                                 // 根据检查状态获取检查记录
    void setExaminationStatus(Examination *&target);                                                     // 修改检查记录状态
    void setExaminationItemName(Examination *&target);                                                   // 修改检查记录的检查项目名称
    void setExaminationReport(Examination *&target);                                                     // 修改检查记录的检查报告信息
    void setExaminationReportSummary(Examination *&target);                                              // 修改检查记录的检查报告摘要信息
    void setExaminationAttachments(Examination *&target);                                                // 修改检查记录的检查相关附件信息
    void setExaminationNote(Examination *&target);                                                       // 修改检查记录的医生备注信息
    void deleteExamination(Examination *&target);                                                        // 删除检查记录（逻辑删除）
    bool createExaminationByConsultation(Consultation *&conHead, Examination *&exaHead, int &idCounter); // 从看诊记录创建检查记录
    void manageExaminations(Examination *&exaHead, Consultation *&conHead, int &idCounter);              // 管理检查记录（查看、修改状态等）

    void managePersonalInfo(); // 管理个人信息（查看、修改基本信息等）
};

#endif // DOCTOR_H
