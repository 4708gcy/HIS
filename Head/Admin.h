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

#include "User.h"
#include "Doctor.h"
#include "Nurse.h"
#include "Pharmacist.h"
#include "Patient.h"

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
    bool viewAllRegistrations(Registration *&reg, const std::string &department);                              // 查看所有挂号记录（可按患者ID、医生ID、状态过滤）
    bool viewRegistrationsByID(Registration *&reg, const std::string &department);                             // 根据挂号ID查看挂号记录
    bool viewRegistrationsByDoctor(Registration *&reg, const std::string &department);                         // 查看指定医生的挂号记录
    bool viewRegistrationsByPatient(Registration *&reg, const std::string &department);                        // 查看指定患者的挂号记录
    bool viewRegistrationsByStatus(Registration *&reg, const std::string &department);                         // 查看指定状态的挂号记录
    void modifyRegistrationDoctor(Registration *&target, Doctor *&doc, const std::string &department);         // 修改挂号记录的医生信息（如重新分配医生等）
    void modifyRegistrationStatus(Registration *&target, const std::string &department);                       // 修改挂号记录状态（如支付、取消等）
    void deleteRegistration(Registration *&target, const std::string &department);                             // 删除挂号记录（逻辑删除，设置 isDeleted 标志）
    bool addRegistration(Registration *&reg, Doctor *&doc, const std::string &department, int &idCounter);     // 添加挂号记录（根据输入信息创建新的 Registration 对象，并插入到链表中）

    void manageConsultations(Consultation *&con, const std::string &department, Registration *reg, int &conCounter); // 管理看诊记录（查看、修改诊断结果等）
    bool viewAllConsultations(Consultation *&con, const std::string &department);                                    // 查看所有看诊记录（可按患者ID、医生ID、状态过滤）
    bool viewConsultationByID(Consultation *&con, const std::string &department);                                    // 根据看诊ID查看看诊记录
    bool viewConsultationsByDoctor(Consultation *&con, const std::string &department);                               // 查看指定医生的看诊记录
    bool viewConsultationsByPatient(Consultation *&con, const std::string &department);                              // 查看指定患者的看诊记录
    bool viewConsultationsByStatus(Consultation *&con, const std::string &department);                               // 查看指定状态的看诊记录
    bool viewConsultationByRegistrationID(Consultation *&con, const std::string &department);                        // 根据挂号ID查看看诊记录
    void modifyConsultationStatus(Consultation *&target, const std::string &department);                                // 修改看诊记录状态（如修改为已完成、取消等）
    void deleteConsultation(Consultation *&target, const std::string &department);                                      // 删除看诊记录（逻辑删除，设置 isDeleted 标志）
    bool addConsultation(Consultation *&con, const std::string &department, Registration *reg, int &conCounter);     // 添加看诊记录（根据输入信息创建新的 Consultation 对象，并插入到链表中）

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

    void manageMedicationRecords(MedicationRecord *&medRec, Consultation *con, Pharmacist *pha, Medicine *med, const std::string &department, int &idCounter); // 管理用药记录（查看、修改用药详情等）
    void viewAllMedicationRecords(MedicationRecord *&medRec, const std::string &department);                                                                   // 查看所有用药记录（可按患者ID、医生ID、状态过滤）
    void viewMedicationRecordByID(MedicationRecord *&medRec, const std::string &department);                                                                   // 根据用药记录ID查看用药记录
    void viewMedicationRecordsByDoctor(MedicationRecord *&medRec, const std::string &department);                                                              // 根据医生ID查看用药记录
    void viewMedicationRecordsByPatient(MedicationRecord *&medRec, const std::string &department);                                                             // 根据患者ID查看用药记录
    void viewMedicationRecordsByPharmacist(MedicationRecord *&medRec, const std::string &department);                                                          // 根据审核药师ID查看用药记录
    void viewMedicationRecordsByStatus(MedicationRecord *&medRec, const std::string &department);                                                              // 查看指定状态的用药记录
    void viewMedicationRecordsByReviewStatus(MedicationRecord *&medRec, const std::string &department);                                                        // 根据审核状态查看用药记录
    void viewMedicationRecordsByMedicationID(MedicationRecord *&medRec, const std::string &department);                                                        // 根据药品ID查看用药记录
    void viewMedicationRecordsByConsultationID(MedicationRecord *&medRec, const std::string &department);                                                      // 根据看诊记录ID查看用药记录
    void modifyMedicationRecordReviewStatus(MedicationRecord *&medRec, const std::string &department);                                                         // 修改用药记录审核状态（如审核通过、驳回等）
    void deleteMedicationRecord(MedicationRecord *&medRec, const std::string &department);                                                                     // 删除用药记录（逻辑删除，设置 isDeleted 标志）
    void addMedicationRecord(MedicationRecord *&medRec, Consultation *con, Pharmacist *pha, Medicine *med, const std::string &department, int &idCounter);     // 添加用药记录（根据输入信息创建新的 MedicationRecord 对象，并插入到链表中）

    void manageMedicines(Medicine *&med, const std::string &department, int &idCounter); // 管理药品信息（查看、修改库存等）
    void viewAllMedicines(Medicine *&med, const std::string &department);                // 查看所有药品信息（可按科室、状态过滤）
    void viewMedicineByID(Medicine *&med, const std::string &department);
    void viewMedicinesByName(Medicine *&med, const std::string &department);          // 根据药品名称查看药品信息
    void viewMedicinesByStatus(Medicine *&med, const std::string &department);        // 根据药品状态查看药品信息
    void viewMedicinesBySafetyStock(Medicine *&med, const std::string &department);   // 查看库存低于安全库存的药品信息
    void viewMedicinesByManufacturer(Medicine *&med, const std::string &department);  // 根据生产厂家查看药品信息
    void modifyMedicine(Medicine *&med, const std::string &department);               // 修改药品信息（如修改库存、价格等）
    void modifyMedicineName(Medicine *&med, const std::string &department);           // 修改药品名称
    void modifyMedicineSpecification(Medicine *&med, const std::string &department);  // 修改药品规格
    void modifyMedicinePurchasePrice(Medicine *&med, const std::string &department);  // 修改药品进价
    void modifyMedicineSalePrice(Medicine *&med, const std::string &department);      // 修改药品售价
    void modifyMedicineStock(Medicine *&med, const std::string &department);          // 修改药品库存
    void modifyMedicineSafetyStock(Medicine *&med, const std::string &department);    // 修改药品安全库存阈值
    void modifyMedicineStatus(Medicine *&med, const std::string &department);         // 修改药品状态（如修改为停用等）
    void modifyMedicineProductionDate(Medicine *&med, const std::string &department); // 修改药品生产日期
    void modifyMedicineExpiryDate(Medicine *&med, const std::string &department);
    void modifyMedicineNote(Medicine *&med, const std::string &department);          // 修改药品备注
    void modifyMedicineManufacturer(Medicine *&med, const std::string &department);  // 修改药品生产厂家
    void deleteMedicine(Medicine *&med, const std::string &department);              // 删除药品信息（逻辑删除，设置 isDeleted 标志）
    void addMedicine(Medicine *&med, const std::string &department, int &idCounter); // 添加药品信息（根据输入信息创建新的 Medicine 对象，并插入到链表中）

    // 管理人员信息
    void manageDoctors(Doctor *&doc, const std::string &department, int &idCounter);         // 管理医生信息（查看、修改职称等）
    void viewAllDoctors(Doctor *&doc, const std::string &department);                        // 查看所有医生信息（可按科室、职称过滤）
    void viewDoctorByID(Doctor *&doc, const std::string &department);                        // 根据医生ID查看医生信息
    void viewDoctorsByOnDutyStatus(Doctor *&doc, const std::string &department);             // 根据在岗状态查看医生信息
    void viewDoctorsByName(Doctor *&doc, const std::string &department);                     // 根据医生姓名查看医生信息
    void viewDoctorByTitle(Doctor *&doc, const std::string &department);                     // 根据医生职称查看医生信息
    void modifyDoctorName(Doctor *&doc, const std::string &department);                      // 修改医生姓名
    void modifyDoctorDepartment(Doctor *&doc, const std::string &department);                // 修改医生科室
    void modifyDoctorGender(Doctor *&doc, const std::string &department);                    // 修改医生性别
    void modifyDoctorAge(Doctor *&doc, const std::string &department);                       // 修改医生年龄
    void modifyDoctorTitle(Doctor *&doc, const std::string &department);                     // 修改医生职称
    void modifyDoctorTelephone(Doctor *&doc, const std::string &department);                 // 修改医生联系电话
    void modifyDoctorEmail(Doctor *&doc, const std::string &department);                     // 修改医生邮箱地址
    void modifyDoctorOnDutyStatus(Doctor *&doc, const std::string &department);              // 修改医生在岗状态
    void modifyDoctorSpecialty(Doctor *&doc, const std::string &department);                 // 修改医生擅长方向
    void modifyDoctorScheduleInfo(Doctor *&doc, const std::string &department);              // 修改医生排班信息
    void modifyDoctorConsultationCount(Doctor *&doc, const std::string &department);         // 修改医生累计接诊患者数量
    void modifyDoctorExaminationCount(Doctor *&doc, const std::string &department);          // 修改医生累计完成的检查数量
    void modifyDoctorHospitalizationApplyCount(Doctor *&doc, const std::string &department); // 修改医生累计开具住院证的数量
    void deleteDoctor(Doctor *&doc, const std::string &department);                          // 删除医生信息（逻辑删除，设置 isDeleted 标志）
    void addDoctor(Doctor *&doc, int &idCounter);                                            // 添加医生信息（根据输入信息创建新的 Doctor 对象，并插入到链表中）

    void manageNurses(Nurse *&nurse, const std::string &department, int &idCounter); // 管理护士信息（查看、修改职称等）
    void viewAllNurses(Nurse *&nurse, const std::string &department);                // 查看所有护士信息（可按科室、职称过滤）
    void viewNurseByID(Nurse *&nurse, const std::string &department);                // 根据护士ID查看护士信息
    void viewNursesByOnDutyStatus(Nurse *&nurse, const std::string &department);     // 根据在岗状态查看护士信息
    void viewNursesByName(Nurse *&nurse, const std::string &department);             // 根据护士姓名查看护士信息
    void viewNurseByLevel(Nurse *&nurse, const std::string &department);             // 根据护士等级查看护士信息
    void modifyNurseName(Nurse *&nurse, const std::string &department);              // 修改护士姓名
    void modifyNurseDepartment(Nurse *&nurse, const std::string &department);        // 修改护士科室
    void modifyNurseGender(Nurse *&nurse, const std::string &department);            // 修改护士性别
    void modifyNurseAge(Nurse *&nurse, const std::string &department);               // 修改护士年龄
    void modifyNurseLevel(Nurse *&nurse, const std::string &department);             // 修改护士等级
    void modifyNurseTelephone(Nurse *&nurse, const std::string &department);         // 修改护士联系电话
    void modifyNurseEmail(Nurse *&nurse, const std::string &department);             // 修改护士邮箱地址
    void modifyNurseOnDutyStatus(Nurse *&nurse, const std::string &department);      // 修改护士在岗状态
    void modifyNurseScheduleInfo(Nurse *&nurse, const std::string &department);      // 修改护士排班信息
    void modifyNursePatientCareCount(Nurse *&nurse, const std::string &department);  // 修改护士累计护理患者数量
    void modifyNurseBedManageCount(Nurse *&nurse, const std::string &department);    // 修改护士累计床位管理次数
    void deleteNurse(Nurse *&nurse, const std::string &department);                  // 删除护士信息（逻辑删除，设置 isDeleted 标志）
    void addNurse(Nurse *&nurse, int &idCounter);                                    // 添加护士信息（根据输入信息创建新的 Nurse 对象，并插入到链表中）

    void managePharmacists(Pharmacist *&pha, const std::string &department, int &idCounter);     // 管理药师信息（查看、修改职称等）
    void viewAllPharmacists(Pharmacist *&pha, const std::string &department);                    // 查看所有药师信息（可按科室、职称过滤）
    void viewPharmacistByID(Pharmacist *&pha, const std::string &department);                    // 根据药师ID查看药师信息
    void viewPharmacistsByOnDutyStatus(Pharmacist *&pha, const std::string &department);         // 根据在岗状态查看药师信息
    void viewPharmacistsByName(Pharmacist *&pha, const std::string &department);                 // 根据药师姓名查看药师信息
    void viewPharmacistByLevel(Pharmacist *&pha, const std::string &department);                 // 根据药师等级查看药师信息
    void modifyPharmacistName(Pharmacist *&pha, const std::string &department);                  // 修改药师姓名
    void modifyPharmacistDepartment(Pharmacist *&pha, const std::string &department);            // 修改药师科室
    void modifyPharmacistGender(Pharmacist *&pha, const std::string &department);                // 修改药师性别
    void modifyPharmacistAge(Pharmacist *&pha, const std::string &department);                   // 修改药师年龄
    void modifyPharmacistLevel(Pharmacist *&pha, const std::string &department);                 // 修改药师等级
    void modifyPharmacistTelephone(Pharmacist *&pha, const std::string &department);             // 修改药师联系电话
    void modifyPharmacistEmail(Pharmacist *&pha, const std::string &department);                 // 修改药师邮箱地址
    void modifyPharmacistOnDutyStatus(Pharmacist *&pha, const std::string &department);          // 修改药师在岗状态
    void modifyPharmacistScheduleInfo(Pharmacist *&pha, const std::string &department);          // 修改药师排班信息
    void modifyPharmacistMedicationReviewCount(Pharmacist *&pha, const std::string &department); // 修改药师累计审核用药记录数量
    void modifyPharmacistDispenseCount(Pharmacist *&pha, const std::string &department);         // 修改药师累计配药数量
    void modifyPharmacistInventoryManageCount(Pharmacist *&pha, const std::string &department);  // 修改药师累计库存管理次数
    void deletePharmacist(Pharmacist *&pha, const std::string &department);                      // 删除药师信息（逻辑删除，设置 isDeleted 标志）
    void addPharmacist(Pharmacist *&pha, int &idCounter);                                        // 添加药师信息（根据输入信息创建新的 Pharmacist 对象，并插入到链表中）

    void managePatients(Patient *&patient, const std::string &department, int &idCounter);      // 管理患者信息（查看、修改联系方式等）
    void viewAllPatients(Patient *&patient, const std::string &department);                     // 查看所有患者信息（可按科室、年龄过滤）
    void viewPatientByID(Patient *&patient, const std::string &department);                     // 根据患者ID查看患者信息
    void viewPatientsByName(Patient *&patient, const std::string &department);                  // 根据患者姓名查看患者信息
    void viewPatientsByGender(Patient *&patient, const std::string &department);                // 根据患者性别查看患者信息
    void viewPatientsByIDCard(Patient *&patient, const std::string &department);                // 根据患者身份证号查看患者信息
    void viewPatientsByAgeGroup(Patient *&patient, const std::string &department);              // 根据患者年龄段查看患者信息
    void viewPatientsByContactInfo(Patient *&patient, const std::string &department);           // 根据患者联系方式查看患者信息
    void viewPatientsByHospitalizationStatus(Patient *&patient, const std::string &department); // 根据患者住院状态查看患者信息
    void viewPatientsByAddress(Patient *&patient, const std::string &department);               // 根据患者地址查看患者信息
    void viewPatientsByMaritalStatus(Patient *&patient, const std::string &department);         // 根据患者婚姻状况查看患者信息
    void modifyPatientName(Patient *&patient, const std::string &department);                   // 修改患者姓名
    void modifyPatientGender(Patient *&patient, const std::string &department);                 // 修改患者性别
    void modifyPatientAge(Patient *&patient, const std::string &department);                    // 修改患者年龄
    void modifyPatientDepartment(Patient *&patient, const std::string &department);             // 修改患者所属科室
    void modifyPatientIDCard(Patient *&patient, const std::string &department);                 // 修改患者身份证号
    void modifyPatientTelephone(Patient *&patient, const std::string &department);              // 修改患者联系电话
    void modifyPatientEmail(Patient *&patient, const std::string &department);                  // 修改患者邮箱地址
    void modifyPatientAddress(Patient *&patient, const std::string &department);                // 修改患者地址
    void modifyPatientMaritalStatus(Patient *&patient, const std::string &department);          // 修改患者婚姻状况
    void modifyPatientEmergencyContact(Patient *&patient, const std::string &department);       // 修改患者紧急联系人信息
    void modifyPatientRegistrationCount(Patient *&patient, const std::string &department);      // 修改患者累计挂号次数
    void modifyPatientConsultationCount(Patient *&patient, const std::string &department);      // 修改患者累计就诊次数
    void modifyPatientHospitalizationCount(Patient *&patient, const std::string &department);   // 修改患者累计住院次数
    void modifyPatientMedicationCount(Patient *&patient, const std::string &department);        // 修改患者累计用药次数
    void deletePatient(Patient *&patient, const std::string &department);                       // 删除患者信息（逻辑删除，设置 isDeleted 标志）
    void addPatient(Patient *&patient, int &idCounter);                                         // 添加患者信息（根据输入信息创建新的 Patient 对象，并插入到链表中）
};

#endif // ADMIN_H