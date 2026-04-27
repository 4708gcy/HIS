/**
 * @file Patient.h
 * @brief 医疗管理系统中的患者类
 * @details 该头文件定义了 Patient 类，继承自 User，
 *          用于描述患者的基础信息、就诊信息及相关业务接口。
 * @author 李阳旭 / 小组成员
 * @date 2026-4-14
 * @version 1.0
 */

#ifndef PATIENT_H
#define PATIENT_H

#include "User.h"

/**
 * @brief 患者类
 * @details 继承自 User，包含患者特有的个人信息、联系人信息、
 *          病史摘要及就医相关操作接口。
 */
class Doctor;

class Patient : public User
{
public:
    std::string patientID;             // 患者ID（语义区分）
    std::string department = "急诊科"; // 就诊科室(默认急诊科)
    std::string address;               // 家庭住址
    std::string idCardNumber;          // 身份证号
    std::string emergencyContactName;  // 紧急联系人姓名
    std::string emergencyContactPhone; // 紧急联系人电话
    std::string allergyHistory;        // 过敏史
    std::string pastMedicalHistory;    // 既往病史
    MaritalStatus maritalStatus;       // 婚姻状况
    int registrationCount = 0;         // 挂号次数
    int consultationCount = 0;         // 就诊次数
    int hospitalizationCount = 0;      // 住院次数
    int medicationCount = 0;           // 用药记录次数

    double balance = 0.0; // 账户余额

    bool isHospitalized = false; // 是否住院中

    Patient *next = nullptr; // 链表指针
    Patient *prev = nullptr; // 双向链表前一个节点指针

    // ==================== 构造 / 析构 ====================
    Patient();
    virtual ~Patient();
    bool patientSignUp(int &idCounter); // 患者注册接口
    bool patientSignIn();               // 患者登录接口

    // ==================== Getter ====================
    std::string getPatientID();
    std::string getAddress();
    std::string getIdCardNumber();
    std::string getEmergencyContactName();
    std::string getEmergencyContactPhone();
    std::string getAllergyHistory();
    std::string getPastMedicalHistory();
    MaritalStatus getMaritalStatus();
    int getRegistrationCount();
    int getConsultationCount();
    int getHospitalizationCount();
    int getMedicationCount();

    // ==================== Setter ====================
    void setPatientID(const std::string &id);
    void setAddress(const std::string &addr);
    void setIdCardNumber(const std::string &idCard);
    void setEmergencyContactName(const std::string &name);
    void setEmergencyContactPhone(const std::string &phone);
    void setAllergyHistory(const std::string &history);
    void setPastMedicalHistory(const std::string &history);
    void setMaritalStatus(MaritalStatus status);
    void setRegistrationCount(int count);
    void setConsultationCount(int count);
    void setHospitalizationCount(int count);
    void setMedicationCount(int count);

    // ==================== 患者业务接口 ====================

    bool getAllRegistrations(Registration *&regHead);                                      // 获取患者的所有挂号记录
    bool getRegistrationsByStatus(Registration *&regHead, int select = -1);                // 根据挂号状态获取挂号记录
    bool getRegistrationsByDepartment(Registration *&regHead);                             // 根据挂号科室获取挂号记录
    bool getRegistrationsByDoctorID(Registration *&regHead);                               // 根据医生ID获取挂号记录
    bool getRegistrationsByTimeRange(Registration *&regHead);                              // 根据挂号时间范围获取挂号记录
    bool getRegistrationsByID(Registration *&regHead);                                     // 根据挂号记录ID获取挂号记录
    bool appointRegistration(Registration *&regHead, Doctor *&doctorHead, int &idCounter); // 预约挂号接口，创建新的挂号记录
    void cancelRegistration(Registration *&target);                                        // 取消挂号接口，更新挂号记录状态为已取消
    void payRegistrationFee(Registration *&target);                                        // 支付挂号费用接口，更新挂号记录状态为已支付
    void manageRegistrations(Registration *&regHead, Doctor *&doctorHead, int &idCounter); // 挂号信息管理入口

    bool getAllConsultations(Consultation *&conHead);              // 查看看诊记录：所有
    bool getConsultationsByID(Consultation *&conHead);             // 查看看诊记录：根据看诊记录ID
    bool getConsultationsByDoctorID(Consultation *&conHead);       // 查看看诊记录：根据医生ID
    bool getConsultationsByStatus(Consultation *&conHead);         // 查看看诊记录：根据看诊状态
    bool getConsultationsByTimeRange(Consultation *&conHead);      // 查看看诊记录：根据看诊时间范围
    bool getConsultationsByChiefComplaint(Consultation *&conHead); // 查看看诊记录：根据主诉内容关键词
    bool getConsultationsByDepartment(Consultation *&conHead);     // 查看看诊记录：根据看诊科室查看
    void manageConsultations(Consultation *&conHead);              // 看诊信息管理入口

    bool getAllExaminations(Examination *&examHead);                       // 1. 查看所有检查信息
    bool getExaminationsByID(Examination *&examHead);                      // 2. 根据检查记录ID查看
    bool getExaminationsByConsultationID(Examination *&examHead);          // 3. 根据看诊记录ID查看
    bool getExaminationsByItemName(Examination *&examHead);                // 4. 根据检查项目查看
    bool getExaminationsByStatus(Examination *&examHead, int select = -1); // 5. 根据检查状态查看
    bool getExaminationsByTimeRange(Examination *&examHead);               // 6. 根据检查时间范围查看
    bool getExaminationsByDepartment(Examination *&examHead);              // 7. 根据检查科室查看
    void payExaminationFee(Examination *&target);                          // 缴纳检查费用
    void manageExaminations(Examination *&examHead);                       // 检查信息管理入口

    void printMedicationRecord(MedicationRecord *current);                    // 打印用药记录信息
    bool getAllMedications(MedicationRecord *&medHead);                       // 1. 查看所有用药信息
    bool getMedicationsByID(MedicationRecord *&medHead);                      // 2. 根据用药记录ID查看
    bool getMedicationsByConsultationID(MedicationRecord *&medHead);          // 3. 根据看诊记录ID查看
    bool getMedicationsByMedicineName(MedicationRecord *&medHead);            // 4. 根据药品名称查看
    bool getMedicationsByStatus(MedicationRecord *&medHead, int select = -1); // 5. 根据用药状态查看
    bool getMedicationsByTimeRange(MedicationRecord *&medHead);               // 6. 根据用药时间范围查看
    bool getMedicationsByDepartment(MedicationRecord *&medHead);              // 7. 根据用药科室查看
    void payMedicationFee(MedicationRecord *&target);                         // 缴纳药品费用
    void manageMedications(MedicationRecord *&medHead);                       // 用药信息管理入口

    // ---------------- 住院信息管理功能 ----------------
    void manageHospitalizations(Hospitalization *&hosHead, bedInfo *&bedHead);
    bool getAllHospitalizations(Hospitalization *&hosHead);
    bool getHospitalizationByID(Hospitalization *&hosHead);
    bool getHospitalizationsByAdmitTimeRange(Hospitalization *&hosHead);
    bool getHospitalizationsByDischargeTimeRange(Hospitalization *&hosHead);
    bool getHospitalizationsByStatus(Hospitalization *&hosHead, int select = -1);
    bool getHospitalizationsByDepartment(Hospitalization *&hosHead);
    bool getHospitalizationsByWardType(Hospitalization *&hosHead);
    bool getHospitalizationsByBedNumber(Hospitalization *&hosHead);
    bool getHospitalizationsByDoctorID(Hospitalization *&hosHead);
    bool getHospitalizationsByConsultationID(Hospitalization *&hosHead);

    void applyForDischarge(Hospitalization *&hosHead, bedInfo *&bedHead);
    void payHospitalizationDeposit(Hospitalization *&hosHead);

    void managePersonalInfo();
};

#endif // PATIENT_H