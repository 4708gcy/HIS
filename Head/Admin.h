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
public:
    Admin *next = nullptr; // 管理员链表的下一个节点指针
    Admin *prev = nullptr; // 管理员链表的上一个节点指针

    Admin();

    bool adminSignUp(int &idCounter); // 管理员注册，调用基类的 signUp(1) 方法，并将管理员信息保存到文件中
    bool adminSignIn();                 // 管理员登录，验证用户ID和密码，并设置登录状态

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
    void modifyConsultationStatus(Consultation *&target, const std::string &department);                             // 修改看诊记录状态（如修改为已完成、取消等）
    void deleteConsultation(Consultation *&target, const std::string &department);                                   // 删除看诊记录（逻辑删除，设置 isDeleted 标志）
    bool addConsultation(Consultation *&con, const std::string &department, Registration *reg, int &conCounter);     // 添加看诊记录（根据输入信息创建新的 Consultation 对象，并插入到链表中）

    void manageExaminations(Examination *&exam, const std::string &department, Consultation *con, int &idCounter); // 管理检查记录（查看、修改报告摘要等）
    bool viewAllExaminations(Examination *&exam, const std::string &department);                                   // 查看所有检查记录（可按患者ID、医生ID、状态过滤）
    bool viewExaminationByID(Examination *&exam, const std::string &department);                                   // 根据检查ID查看检查记录
    bool viewExaminationsByDoctor(Examination *&exam, const std::string &department);                              // 查看指定医生的检查记录
    bool viewExaminationsByPatient(Examination *&exam, const std::string &department);                             // 查看指定患者的检查记录
    bool viewExaminationsByStatus(Examination *&exam, const std::string &department);                              // 查看指定状态的检查记录
    void modifyExaminationStatus(Examination *&target, const std::string &department);                             // 修改检查记录状态（如支付、取消等）
    void deleteExamination(Examination *&target, const std::string &department);                                   // 删除检查记录（逻辑删除，设置 isDeleted 标志）
    bool addExamination(Examination *&exam, const std::string &department, Consultation *con, int &idCounter);     // 添加检查记录（根据输入信息创建新的 Examination 对象，并插入到链表中）

    void manageHospitalizations(Hospitalization *&hos, Nurse *nurse, const std::string &department, Consultation *con, bedInfo *bed, int &idCounter); // 管理住院记录（查看、修改出院日期等）
    bool viewAllHospitalizations(Hospitalization *&hos, const std::string &department);                                                               // 查看所有住院记录（可按患者ID、医生ID、状态过滤）
    bool viewHospitalizationByID(Hospitalization *&hos, const std::string &department);                                                               // 根据住院ID查看住院记录
    bool viewHospitalizationsByDoctor(Hospitalization *&hos, const std::string &department);                                                          // 查看指定医生的住院记录
    bool viewHospitalizationsByPatient(Hospitalization *&hos, const std::string &department);                                                         // 查看指定患者的住院记录
    bool viewHospitalizationByNurse(Hospitalization *&hos, const std::string &department);                                                            // 根据负责护士ID查看住院记录
    bool viewHospitalizationByWardType(Hospitalization *&hos, const std::string &department);                                                         // 根据病房类型查看住院记录
    bool viewHospitalizationsByStatus(Hospitalization *&hos, const std::string &department);                                                          // 查看指定状态的住院记录
    void modifyHospitalizationStatus(Hospitalization *&target, const std::string &department);                                                        // 修改住院记录状态（如修改出院日期、分床等）
    void deleteHospitalization(Hospitalization *&target, const std::string &department);                                                              // 删除住院记录（逻辑删除，设置 isDeleted 标志）
    bool addHospitalization(Hospitalization *&hos, Nurse *nurse, const std::string &department, Consultation *con, bedInfo *&bed, int &idCounter);    // 添加住院记录（根据输入信息创建新的 Hospitalization 对象，并插入到链表中）

    void manageBedInfo(bedInfo *&bed, Hospitalization *&hos, const std::string &department);               // 管理床位信息（查看、修改状态等）
    bool viewAllBeds(bedInfo *&bed, const std::string &department);                                        // 查看所有床位信息（可按病房类型、状态过滤）
    bool viewBedsByWardType(bedInfo *&bed, const std::string &department);                                 // 根据病房类型查看床位信息
    bool viewBedsByStatus(bedInfo *&bed, const std::string &department);                                   // 根据状态查看床位信息
    bool viewBedByID(bedInfo *&bed, const std::string &department);                                        // 根据床位ID查看床位信息
    bool viewBedsByNurseID(bedInfo *&bed, const std::string &department);                                  // 根据负责护士ID查看床位信息
    bool viewBedsByPatientID(bedInfo *&bed, const std::string &department);                                // 根据占用患者ID查看床位信息
    bool viewBedsByHospitalizationID(bedInfo *&bed, Hospitalization *&hos, const std::string &department); // 根据关联住院记录ID查看床位信息
    void modifyBedStatus(bedInfo *&target, const std::string &department);                                 // 修改床位状态（如修改为占用、空闲等）
    void deleteBedInfo(bedInfo *&target, const std::string &department);                                   // 删除床位信息（逻辑删除，设置 isDeleted 标志）
    bool addBedInfo(bedInfo *&bed, const std::string &department);                                         // 添加床位信息（根据输入信息创建新的 bedInfo 对象，并插入到链表中）

    void manageMedicationRecords(MedicationRecord *&medRec, Consultation *con, Pharmacist *pha, Medicine *med, const std::string &department, int &idCounter); // 管理用药记录（查看、修改用药详情等）
    bool viewAllMedicationRecords(MedicationRecord *&medRec, const std::string &department);                                                                   // 查看所有用药记录（可按患者ID、医生ID、状态过滤）
    bool viewMedicationRecordByID(MedicationRecord *&medRec, const std::string &department);                                                                   // 根据用药记录ID查看用药记录
    bool viewMedicationRecordsByDoctor(MedicationRecord *&medRec, const std::string &department);                                                              // 根据医生ID查看用药记录
    bool viewMedicationRecordsByPatient(MedicationRecord *&medRec, const std::string &department);                                                             // 根据患者ID查看用药记录
    bool viewMedicationRecordsByPharmacist(MedicationRecord *&medRec, const std::string &department);                                                          // 根据审核药师ID查看用药记录
    bool viewMedicationRecordsByStatus(MedicationRecord *&medRec, const std::string &department);                                                              // 查看指定状态的用药记录
    bool viewMedicationRecordsByReviewStatus(MedicationRecord *&medRec, const std::string &department);                                                        // 根据审核状态查看用药记录
    bool viewMedicationRecordsByMedicationID(MedicationRecord *&medRec, const std::string &department);                                                        // 根据药品ID查看用药记录
    bool viewMedicationRecordsByConsultationID(MedicationRecord *&medRec, const std::string &department);                                                      // 根据看诊记录ID查看用药记录
    void modifyMedicationRecordReviewStatus(MedicationRecord *&medRec, const std::string &department);                                                         // 修改用药记录审核状态（如审核通过、驳回等）
    void deleteMedicationRecord(MedicationRecord *&medRec, const std::string &department);                                                                     // 删除用药记录（逻辑删除，设置 isDeleted 标志）
    bool addMedicationRecord(MedicationRecord *&medRec, Consultation *con, Pharmacist *pha, Medicine *med, const std::string &department, int &idCounter);     // 添加用药记录（根据输入信息创建新的 MedicationRecord 对象，并插入到链表中）

    void manageMedicines(Medicine *&med, MedicineFlow *&medFlow, const std::string &department, int &idCounter, int &flowCounter); // 管理药品信息（查看、修改库存等）
    bool viewAllMedicines(Medicine *&med, const std::string &department);                // 查看所有药品信息（可按科室、状态过滤）
    bool viewMedicineByID(Medicine *&med, const std::string &department);                // 根据药品ID查看药品信息
    bool viewMedicinesByName(Medicine *&med, const std::string &department);             // 根据药品名称查看药品信息
    bool viewMedicinesByStatus(Medicine *&med, const std::string &department);           // 根据药品状态查看药品信息
    bool viewMedicinesBySafetyStock(Medicine *&med, const std::string &department);      // 查看库存低于安全库存的药品信息
    bool viewMedicinesByManufacturer(Medicine *&med, const std::string &department);     // 根据生产厂家查看药品信息
    void modifyMedicine(Medicine *&target, const std::string &department);               // 修改药品信息（如修改库存、价格等）
    void modifyMedicineName(Medicine *&target, const std::string &department);           // 修改药品名称
    void modifyMedicineSpecification(Medicine *&target, const std::string &department);  // 修改药品规格
    void modifyMedicinePurchasePrice(Medicine *&target, const std::string &department);  // 修改药品进价
    void modifyMedicineSalePrice(Medicine *&target, const std::string &department);      // 修改药品售价
    void modifyMedicineStock(Medicine *&target, const std::string &department);          // 修改药品库存
    void modifyMedicineSafetyStock(Medicine *&target, const std::string &department);    // 修改药品安全库存阈值
    void modifyMedicineStatus(Medicine *&target, const std::string &department);         // 修改药品状态（如修改为停用等）
    void modifyMedicineProductionDate(Medicine *&target, const std::string &department); // 修改药品生产日期
    void modifyMedicineExpiryDate(Medicine *&target, const std::string &department);     // 修改药品有效期限
    void modifyMedicineNote(Medicine *&target, const std::string &department);           // 修改药品备注
    void modifyMedicineManufacturer(Medicine *&target, const std::string &department);   // 修改药品生产厂家
    void deleteMedicine(Medicine *&target, const std::string &department);               // 删除药品信息（逻辑删除，设置 isDeleted 标志）
    bool addMedicine(Medicine *&med, const std::string &department, int &idCounter);     // 添加药品信息（根据输入信息创建新的 Medicine 对象，并插入到链表中）

    // === 药品出入库流水管理 ===
    void manageMedicineFlows(MedicineFlow *&flowHead, Medicine *&medHead, const std::string &department, int &flowCounter);
    void addMedicineFlow(MedicineFlow *&flowHead, Medicine *&medHead, const std::string &department, int &flowCounter);
    void deleteMedicineFlow(MedicineFlow *&flowHead, MedicineFlow *&target);
    bool viewAllMedicineFlows(MedicineFlow *&flowHead, const std::string &department);
    bool viewMedicineFlowsByMedicineID(MedicineFlow *&flowHead, const std::string &department);
    bool viewMedicineFlowsByType(MedicineFlow *&flowHead, const std::string &department);
    bool viewMedicineFlowsByOperator(MedicineFlow *&flowHead, const std::string &department);

    // 管理人员信息
    void manageDoctors(Doctor *&doc, const std::string &department, int &idCounter);            // 管理医生信息（查看、修改职称等）
    bool viewAllDoctors(Doctor *&doc, const std::string &department);                           // 查看所有医生信息（可按科室、职称过滤）
    bool viewDoctorByID(Doctor *&doc, const std::string &department);                           // 根据医生ID查看医生信息
    bool viewDoctorsByOnDutyStatus(Doctor *&doc, const std::string &department);                // 根据在岗状态查看医生信息
    bool viewDoctorsByName(Doctor *&doc, const std::string &department);                        // 根据医生姓名查看医生信息
    bool viewDoctorByTitle(Doctor *&doc, const std::string &department);                        // 根据医生职称查看医生信息
    void modifyDoctorName(Doctor *&target, const std::string &department);                      // 修改医生姓名
    void modifyDoctorDepartment(Doctor *&target, const std::string &department);                // 修改医生科室
    void modifyDoctorGender(Doctor *&target, const std::string &department);                    // 修改医生性别
    void modifyDoctorAge(Doctor *&target, const std::string &department);                       // 修改医生年龄
    void modifyDoctorTitle(Doctor *&target, const std::string &department);                     // 修改医生职称
    void modifyDoctorTelephone(Doctor *&target, const std::string &department);                 // 修改医生联系电话
    void modifyDoctorEmail(Doctor *&target, const std::string &department);                     // 修改医生邮箱地址
    void modifyDoctorOnDutyStatus(Doctor *&target, const std::string &department);              // 修改医生在岗状态
    void modifyDoctorSpecialty(Doctor *&target, const std::string &department);                 // 修改医生擅长方向
    void modifyDoctorScheduleInfo(Doctor *&target, const std::string &department);              // 修改医生排班信息
    void modifyDoctorConsultationCount(Doctor *&target, const std::string &department);         // 修改医生累计接诊患者数量
    void modifyDoctorExaminationCount(Doctor *&target, const std::string &department);          // 修改医生累计完成的检查数量
    void modifyDoctorHospitalizationApplyCount(Doctor *&target, const std::string &department); // 修改医生累计开具住院证的数量
    void deleteDoctor(Doctor *&target, const std::string &department);                          // 删除医生信息（逻辑删除，设置 isDeleted 标志）
    bool addDoctor(Doctor *&doc, int &idCounter);                                               // 添加医生信息（根据输入信息创建新的 Doctor 对象，并插入到链表中）

    void manageNurses(Nurse *&nurse, const std::string &department, int &idCounter); // 管理护士信息（查看、修改职称等）
    bool viewAllNurses(Nurse *&nurse, const std::string &department);                // 查看所有护士信息（可按科室、职称过滤）
    bool viewNurseByID(Nurse *&nurse, const std::string &department);                // 根据护士ID查看护士信息
    bool viewNursesByOnDutyStatus(Nurse *&nurse, const std::string &department);     // 根据在岗状态查看护士信息
    bool viewNursesByName(Nurse *&nurse, const std::string &department);             // 根据护士姓名查看护士信息
    bool viewNurseByLevel(Nurse *&nurse, const std::string &department);             // 根据护士等级查看护士信息
    void modifyNurseName(Nurse *&target, const std::string &department);             // 修改护士姓名
    void modifyNurseDepartment(Nurse *&target, const std::string &department);       // 修改护士科室
    void modifyNurseGender(Nurse *&target, const std::string &department);           // 修改护士性别
    void modifyNurseAge(Nurse *&target, const std::string &department);              // 修改护士年龄
    void modifyNurseLevel(Nurse *&target, const std::string &department);            // 修改护士等级
    void modifyNurseTelephone(Nurse *&target, const std::string &department);        // 修改护士联系电话
    void modifyNurseEmail(Nurse *&target, const std::string &department);            // 修改护士邮箱地址
    void modifyNurseOnDutyStatus(Nurse *&target, const std::string &department);     // 修改护士在岗状态
    void modifyNurseScheduleInfo(Nurse *&target, const std::string &department);     // 修改护士排班信息
    void modifyNursePatientCareCount(Nurse *&target, const std::string &department); // 修改护士累计护理患者数量
    void modifyNurseBedManageCount(Nurse *&target, const std::string &department);   // 修改护士累计床位管理次数
    void deleteNurse(Nurse *&target, const std::string &department);                 // 删除护士信息（逻辑删除，设置 isDeleted 标志）
    bool addNurse(Nurse *&nurse, int &idCounter);                                    // 添加护士信息（根据输入信息创建新的 Nurse 对象，并插入到链表中）

    void managePharmacists(Pharmacist *&pha, const std::string &department, int &idCounter);        // 管理药师信息（查看、修改职称等）
    bool viewAllPharmacists(Pharmacist *&pha, const std::string &department);                       // 查看所有药师信息（可按科室、职称过滤）
    bool viewPharmacistByID(Pharmacist *&pha, const std::string &department);                       // 根据药师ID查看药师信息
    bool viewPharmacistsByOnDutyStatus(Pharmacist *&pha, const std::string &department);            // 根据在岗状态查看药师信息
    bool viewPharmacistsByName(Pharmacist *&pha, const std::string &department);                    // 根据药师姓名查看药师信息
    bool viewPharmacistByLevel(Pharmacist *&pha, const std::string &department);                    // 根据药师等级查看药师信息
    void modifyPharmacistName(Pharmacist *&target, const std::string &department);                  // 修改药师姓名
    void modifyPharmacistDepartment(Pharmacist *&target, const std::string &department);            // 修改药师科室
    void modifyPharmacistGender(Pharmacist *&target, const std::string &department);                // 修改药师性别
    void modifyPharmacistAge(Pharmacist *&target, const std::string &department);                   // 修改药师年龄
    void modifyPharmacistLevel(Pharmacist *&target, const std::string &department);                 // 修改药师等级
    void modifyPharmacistTelephone(Pharmacist *&target, const std::string &department);             // 修改药师联系电话
    void modifyPharmacistEmail(Pharmacist *&target, const std::string &department);                 // 修改药师邮箱地址
    void modifyPharmacistOnDutyStatus(Pharmacist *&target, const std::string &department);          // 修改药师在岗状态
    void modifyPharmacistScheduleInfo(Pharmacist *&target, const std::string &department);          // 修改药师排班信息
    void modifyPharmacistMedicationReviewCount(Pharmacist *&target, const std::string &department); // 修改药师累计审核用药记录数量
    void modifyPharmacistDispenseCount(Pharmacist *&target, const std::string &department);         // 修改药师累计配药数量
    void modifyPharmacistInventoryManageCount(Pharmacist *&target, const std::string &department);  // 修改药师累计库存管理次数
    void deletePharmacist(Pharmacist *&target, const std::string &department);                      // 删除药师信息（逻辑删除，设置 isDeleted 标志）
    bool addPharmacist(Pharmacist *&pha, int &idCounter);                                           // 添加药师信息（根据输入信息创建新的 Pharmacist 对象，并插入到链表中）

    void managePatients(Patient *&patient, const std::string &department, int &idCounter);      // 管理患者信息（查看、修改联系方式等）
    bool viewAllPatients(Patient *&patient, const std::string &department);                     // 查看所有患者信息（可按科室、年龄过滤）
    bool viewPatientByID(Patient *&patient, const std::string &department);                     // 根据患者ID查看患者信息
    bool viewPatientsByName(Patient *&patient, const std::string &department);                  // 根据患者姓名查看患者信息
    bool viewPatientsByGender(Patient *&patient, const std::string &department);                // 根据患者性别查看患者信息
    bool viewPatientsByIDCard(Patient *&patient, const std::string &department);                // 根据患者身份证号查看患者信息
    bool viewPatientsByAgeGroup(Patient *&patient, const std::string &department);              // 根据患者年龄段查看患者信息
    bool viewPatientsByContactInfo(Patient *&patient, const std::string &department);           // 根据患者联系方式查看患者信息
    bool viewPatientsByHospitalizationStatus(Patient *&patient, const std::string &department); // 根据患者住院状态查看患者信息
    bool viewPatientsByAddress(Patient *&patient, const std::string &department);               // 根据患者地址查看患者信息
    bool viewPatientsByMaritalStatus(Patient *&patient, const std::string &department);         // 根据患者婚姻状况查看患者信息
    void modifyPatientName(Patient *&target, const std::string &department);                    // 修改患者姓名
    void modifyPatientGender(Patient *&target, const std::string &department);                  // 修改患者性别
    void modifyPatientAge(Patient *&target, const std::string &department);                     // 修改患者年龄
    void modifyPatientDepartment(Patient *&target, const std::string &department);              // 修改患者所属科室
    void modifyPatientIDCard(Patient *&target, const std::string &department);                  // 修改患者身份证号
    void modifyPatientTelephone(Patient *&target, const std::string &department);               // 修改患者联系电话
    void modifyPatientEmail(Patient *&target, const std::string &department);                   // 修改患者邮箱地址
    void modifyPatientAddress(Patient *&target, const std::string &department);                 // 修改患者地址
    void modifyPatientMaritalStatus(Patient *&target, const std::string &department);           // 修改患者婚姻状况
    void modifyPatientEmergencyContact(Patient *&target, const std::string &department);        // 修改患者紧急联系人信息
    void modifyPatientRegistrationCount(Patient *&target, const std::string &department);       // 修改患者累计挂号次数
    void modifyPatientConsultationCount(Patient *&target, const std::string &department);       // 修改患者累计就诊次数
    void modifyPatientHospitalizationCount(Patient *&target, const std::string &department);    // 修改患者累计住院次数
    void modifyPatientMedicationCount(Patient *&target, const std::string &department);         // 修改患者累计用药次数
    void deletePatient(Patient *&target, const std::string &department);                        // 删除患者信息（逻辑删除，设置 isDeleted 标志）
    bool addPatient(Patient *&patient, int &idCounter);                                         // 添加患者信息（根据输入信息创建新的 Patient 对象，并插入到链表中）

    void AdminPersonalInfo(); // 管理员个人信息管理（查看、修改密码等）

    // 统计报表功能
    void showDepartmentReport(Doctor *&docHead, Registration *&regHead, Consultation *&conHead);
    void showDoctorWorkloadReport(Doctor *&docHead);
    void showPatientReport(Patient *&patientHead, Registration *&regHead, Consultation *&conHead);
    void showBedUtilizationReport(bedInfo *&bedHead, Hospitalization *&hosHead);
    void showMedicineInventoryReport(Medicine *&medHead, MedicineFlow *&flowHead);
    void showDataAnalysisReport(Hospitalization *&hosHead, bedInfo *&bedHead, Registration *&regHead, Medicine *&medHead, MedicineFlow *&flowHead);
};

#endif // ADMIN_H