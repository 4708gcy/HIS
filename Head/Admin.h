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
#include "Doctor.h"
#include "Nurse.h"
#include "Pharmacist.h"

class Admin : public User
{
private:
    double totalRevenue = 0.0;  // 医院总收入统计（可选）
    double totalExpenses = 0.0; // 医院总支出统计（可选）
    double netProfit = 0.0;     // 医院净利润统计（可选）

public:
    Admin *next = nullptr; // 管理员链表的下一个节点指针

    Admin();

    Admin *adminSignUp(int &idCounter); // 管理员注册，调用基类的 signUp(1) 方法，并将管理员信息保存到文件中
    bool adminSignIn();                 // 管理员登录，验证用户ID和密码，并设置登录状态
    double &getTotalRevenue();          // 获取医院总收入
    double &getTotalExpenses();         // 获取医院总支出
    double &getNetProfit();             // 获取医院净利润

    template <typename T>
    void setAccountActive(T *&head)
    {
        std::string targetID;
        std::cout << "请输入要操作的用户ID: ";
        std::cin >> targetID;

        T *current = head;
        while (current != nullptr)
        {
            if (current->getUserID() == targetID)
            {
                bool newStatus;
                std::cout << "请输入新的账户状态 (1 - 激活, 0 - 锁定): ";
                std::cin >> newStatus;
                if (newStatus)
                {
                    current->isAccountActive = true;
                    current->loginAttempts = 0; // 重置登录失败次数
                    std::cout << "账户已激活！" << std::endl;
                }
                else
                {
                    current->isAccountActive = false;
                    std::cout << "账户已锁定！" << std::endl;
                }
                return;
            }
            current = current->next; // 假设 T 中有 next 指针
        }

        std::cout << "未找到指定用户！" << std::endl;
    }

    // === 管理医疗记录 ===
    void manageRegistrations(Registration *&reg, Doctor *&doc, const std::string &department, int &idCounter); // 管理挂号记录（查看、修改状态等）
    void viewAllRegistrations(Registration *&reg, const std::string &department);                              // 查看所有挂号记录（可按患者ID、医生ID、状态过滤）
    void viewRegistrationsByID(Registration *&reg, const std::string &department);                             // 根据挂号ID查看挂号记录
    void viewRegistrationsByDoctor(Registration *&reg, const std::string &department);                         // 查看指定医生的挂号记录
    void viewRegistrationsByPatient(Registration *&reg, const std::string &department);                        // 查看指定患者的挂号记录
    void viewRegistrationsByStatus(Registration *&reg, const std::string &department);                         // 查看指定状态的挂号记录
    void modifyRegistrationStatus(Registration *&reg, const std::string &department);                          // 修改挂号记录状态（如支付、取消等）
    void deleteRegistration(Registration *&reg, const std::string &department);                                // 删除挂号记录（逻辑删除，设置 isDeleted 标志）
    void addRegistration(Registration *&reg, Doctor *&doc, const std::string &department, int &idCounter);     // 添加挂号记录（根据输入信息创建新的 Registration 对象，并插入到链表中）

    void manageConsultations(Consultation *&con, const std::string &department, Registration *reg, int &conCounter); // 管理看诊记录（查看、修改诊断结果等）
    void viewAllConsultations(Consultation *&con, const std::string &department);                                    // 查看所有看诊记录（可按患者ID、医生ID、状态过滤）
    void viewConsultationByID(Consultation *&con, const std::string &department);                                    // 根据看诊ID查看看诊记录
    void viewConsultationsByDoctor(Consultation *&con, const std::string &department);                               // 查看指定医生的看诊记录
    void viewConsultationsByPatient(Consultation *&con, const std::string &department);                              // 查看指定患者的看诊记录
    void viewConsultationsByStatus(Consultation *&con, const std::string &department);                               // 查看指定状态的看诊记录
    void viewConsultationByRegistrationID(Consultation *&con, const std::string &department);                        // 根据挂号ID查看看诊记录
    void modifyConsultation(Consultation *&con, const std::string &department);                                      // 修改看诊记录（如修改诊断结果、添加医生备注等）
    void deleteConsultation(Consultation *&con, const std::string &department);                                      // 删除看诊记录（逻辑删除，设置 isDeleted 标志）
    void addConsultation(Consultation *&con, const std::string &department, Registration *reg, int &conCounter);     // 添加看诊记录（根据输入信息创建新的 Consultation 对象，并插入到链表中）

    void manageExaminations(Examination *&exam, const std::string &department, Consultation *con, int &idCounter); // 管理检查记录（查看、修改报告摘要等）
    void viewAllExaminations(Examination *&exam, const std::string &department);                                   // 查看所有检查记录（可按患者ID、医生ID、状态过滤）
    void viewExaminationByID(Examination *&exam, const std::string &department);                                   // 根据检查ID查看检查记录
    void viewExaminationsByDoctor(Examination *&exam, const std::string &department);                              // 查看指定医生的检查记录
    void viewExaminationsByPatient(Examination *&exam, const std::string &department);                             // 查看指定患者的检查记录
    void viewExaminationsByStatus(Examination *&exam, const std::string &department);                              // 查看指定状态的检查记录
    void modifyExaminationStatus(Examination *&exam, const std::string &department);                               // 修改检查记录状态（如支付、取消等）
    void deleteExamination(Examination *&exam, const std::string &department);                                     // 删除检查记录（逻辑删除，设置 isDeleted 标志）
    void addExamination(Examination *&exam, const std::string &department, Consultation *con, int &idCounter);     // 添加检查记录（根据输入信息创建新的 Examination 对象，并插入到链表中）

    void manageHospitalizations(Hospitalization *&hos, Nurse *nurse, const std::string &department, Consultation *con, bedInfo *bed, int &idCounter); // 管理住院记录（查看、修改出院日期等）
    void viewAllHospitalizations(Hospitalization *&hos, const std::string &department);                                                               // 查看所有住院记录（可按患者ID、医生ID、状态过滤）
    void viewHospitalizationByID(Hospitalization *&hos, const std::string &department);                                                               // 根据住院ID查看住院记录
    void viewHospitalizationsByDoctor(Hospitalization *&hos, const std::string &department);                                                          // 查看指定医生的住院记录
    void viewHospitalizationsByPatient(Hospitalization *&hos, const std::string &department);                                                         // 查看指定患者的住院记录
    void viewHospitalizationByNurse(Hospitalization *&hos, const std::string &department);                                                            // 根据负责护士ID查看住院记录
    void viewHospitalizationByWardType(Hospitalization *&hos, const std::string &department);                                                         // 根据病房类型查看住院记录
    void viewHospitalizationsByStatus(Hospitalization *&hos, const std::string &department);                                                          // 查看指定状态的住院记录
    void modifyHospitalizationStatus(Hospitalization *&hos, const std::string &department);                                                           // 修改住院记录状态（如修改出院日期、分床等）
    void deleteHospitalization(Hospitalization *&hos, const std::string &department);                                                                 // 删除住院记录（逻辑删除，设置 isDeleted 标志）
    void addHospitalization(Hospitalization *&hos, Nurse *nurse, const std::string &department, Consultation *con, bedInfo *&bed, int &idCounter);    // 添加住院记录（根据输入信息创建新的 Hospitalization 对象，并插入到链表中）

    void manageBedInfo(bedInfo *&bed, Hospitalization *&hos, const std::string &department);               // 管理床位信息（查看、修改状态等）
    void viewAllBeds(bedInfo *&bed, const std::string &department);                                        // 查看所有床位信息（可按病房类型、状态过滤）
    void viewBedsByWardType(bedInfo *&bed, const std::string &department);                                 // 根据病房类型查看床位信息
    void viewBedsByStatus(bedInfo *&bed, const std::string &department);                                   // 根据状态查看床位信息
    void viewBedByID(bedInfo *&bed, const std::string &department);                                        // 根据床位ID查看床位信息
    void viewBedsByNurseID(bedInfo *&bed, const std::string &department);                                  // 根据负责护士ID查看床位信息
    void viewBedsByPatientID(bedInfo *&bed, const std::string &department);                                // 根据占用患者ID查看床位信息
    void viewBedsByHospitalizationID(bedInfo *&bed, Hospitalization *&hos, const std::string &department); // 根据关联住院记录ID查看床位信息
    void modifyBedStatus(bedInfo *&bed, const std::string &department);                                    // 修改床位状态（如修改为占用、空闲等）
    void deleteBedInfo(bedInfo *&bed, const std::string &department);                                      // 删除床位信息（逻辑删除，设置 isDeleted 标志）
    void addBedInfo(bedInfo *&bed, const std::string &department);                                         // 添加床位信息（根据输入信息创建新的 bedInfo 对象，并插入到链表中）

    void manageMedicationRecords(MedicationRecord *&medRec, Consultation *con, Pharmacist *pha, const std::string &department, int &idCounter); // 管理用药记录（查看、修改用药详情等）
    void viewAllMedicationRecords(MedicationRecord *&medRec, const std::string &department);                                                    // 查看所有用药记录（可按患者ID、医生ID、状态过滤）
    void viewMedicationRecordByID(MedicationRecord *&medRec, const std::string &department);                                                    // 根据用药记录ID查看用药记录
    void viewMedicationRecordsByDoctor(MedicationRecord *&medRec, const std::string &department);                                               // 根据医生ID查看用药记录
    void viewMedicationRecordsByPatient(MedicationRecord *&medRec, const std::string &department);                                              // 根据患者ID查看用药记录
    void viewMedicationRecordsByPharmacist(MedicationRecord *&medRec, const std::string &department);                                           // 根据审核药师ID查看用药记录
    void viewMedicationRecordsByStatus(MedicationRecord *&medRec, const std::string &department);                                               // 查看指定状态的用药记录
    void viewMedicationRecordsByReviewStatus(MedicationRecord *&medRec, const std::string &department);                                         // 根据审核状态查看用药记录
    void viewMedicationRecordsByMedicationID(MedicationRecord *&medRec, const std::string &department);                                         // 根据药品ID查看用药记录
    void viewMedicationRecordsByConsultationID(MedicationRecord *&medRec, const std::string &department);                                       // 根据看诊记录ID查看用药记录
    void modifyMedicationRecordReviewStatus(MedicationRecord *&medRec, const std::string &department);                                          // 修改用药记录审核状态（如审核通过、驳回等）
    void deleteMedicationRecord(MedicationRecord *&medRec, const std::string &department);                                                      // 删除用药记录（逻辑删除，设置 isDeleted 标志）
    void addMedicationRecord(MedicationRecord *&medRec, Consultation *con, Pharmacist *pha, const std::string &department, int &idCounter);                      // 添加用药记录（根据输入信息创建新的 MedicationRecord 对象，并插入到链表中）


    
    void manageMedicines(Medicine *&med, const std::string &department, int &idCounter); // 管理药品信息（查看、修改库存等）
};

#endif // ADMIN_H