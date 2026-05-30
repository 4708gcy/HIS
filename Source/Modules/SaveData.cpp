#include "Modules/SaveData.h"
#include "Core/Database.h"
#include "Entities/Registration.h"
#include "Entities/Consultation.h"
#include "Entities/Examination.h"
#include "Entities/Hospitalization.h"
#include "Entities/MedicationRecord.h"
#include "Entities/Medicine.h"
#include "Entities/NursingRecord.h"
#include <iostream>

// ===================== MySQL 版本：人物数据保存函数 =====================
//
// \x01 前缀约定：
//   dbDateTime() 空值时返回 "\x01NULL"，buildSql() 识别 \x01 前缀后，
//   将后续内容（"NULL"）作为原始 SQL 片段直接拼接，不加引号、不转义。
//   这确保了 DATETIME 字段在数据库中存储为真正的 SQL NULL 而非字符串 'NULL'。

// Helper: convert "#" sentinel to empty string for SQL NULL coalescing
static inline std::string dbStr(const std::string &s)
{
    return (s == "#") ? std::string() : s;
}

// 空日期时间返回 \x01 前缀标记，buildSql() 识别后输出 SQL NULL（不带引号）
// 非空时正常返回日期时间字符串（由 buildSql() 转义并加引号）
static inline std::string dbDateTime(const std::string &s)
{
    if (s.empty() || s == "#" || s == "无") return std::string("\x01NULL");
    return s;
}

void saveAdminData(Admin *adminHead, int count)
{
    Database &db = GetDB();
    if (!db.isConnected()) return;

    try
    {
        db.beginTransaction();
        Admin *current = adminHead;
        while (current)
        {
            if (current->isDeleted)
            {
                db.executePrepared(
                    "UPDATE admins SET is_deleted = 1 WHERE admin_id = ?",
                    {current->getUserID()});
            }
            else
            {
                db.executePrepared(
                    "INSERT INTO admins (admin_id, username, stored_hash, salt, login_attempts, is_active, "
                    "create_time, gender, age, telephone, email, is_deleted) "
                    "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?) "
                    "ON DUPLICATE KEY UPDATE "
                    "username=VALUES(username), stored_hash=VALUES(stored_hash), salt=VALUES(salt), "
                    "login_attempts=VALUES(login_attempts), is_active=VALUES(is_active), "
                    "create_time=VALUES(create_time), gender=VALUES(gender), age=VALUES(age), "
                    "telephone=VALUES(telephone), email=VALUES(email), is_deleted=VALUES(is_deleted)",
                    {current->getUserID(), current->getUsername(), current->getStoredHash(),
                     current->getSalt(), std::to_string(current->getLoginAttempts()),
                     current->getIsAccountActive() ? "1" : "0",
                     dbDateTime(current->getCreateTime()),
                     current->getGender(),
                     std::to_string(current->getAge()),
                     current->getTelephone(),
                     current->getEmail(),
                     current->getIsDeleted() ? "1" : "0"});
            }
            current = current->next;
        }
        db.commit();
        std::cout << "管理员数据保存成功！" << std::endl;
    }
    catch (...)
    {
        db.rollback();
        std::cerr << "管理员数据保存失败，已回滚！" << std::endl;
    }
}

void saveDoctorData(Doctor *doctorHead, int count)
{
    Database &db = GetDB();
    if (!db.isConnected()) return;

    try
    {
        db.beginTransaction();
        Doctor *current = doctorHead;
        while (current)
        {
            if (current->isDeleted)
            {
                db.executePrepared(
                    "UPDATE doctors SET is_deleted = 1 WHERE doctor_id = ?",
                    {current->getUserID()});
            }
            else
            {
                db.executePrepared(
                    "INSERT INTO doctors (doctor_id, username, stored_hash, salt, login_attempts, is_active, "
                    "create_time, gender, age, telephone, email, department, title, specialty, schedule_info, "
                    "consultation_count, examination_count, hospitalization_apply_count, is_on_duty, is_deleted) "
                    "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?) "
                    "ON DUPLICATE KEY UPDATE "
                    "username=VALUES(username), stored_hash=VALUES(stored_hash), salt=VALUES(salt), "
                    "login_attempts=VALUES(login_attempts), is_active=VALUES(is_active), "
                    "create_time=VALUES(create_time), gender=VALUES(gender), age=VALUES(age), "
                    "telephone=VALUES(telephone), email=VALUES(email), department=VALUES(department), "
                    "title=VALUES(title), specialty=VALUES(specialty), schedule_info=VALUES(schedule_info), "
                    "consultation_count=VALUES(consultation_count), examination_count=VALUES(examination_count), "
                    "hospitalization_apply_count=VALUES(hospitalization_apply_count), "
                    "is_on_duty=VALUES(is_on_duty), is_deleted=VALUES(is_deleted)",
                    {current->getUserID(), current->getUsername(), current->getStoredHash(),
                     current->getSalt(), std::to_string(current->getLoginAttempts()),
                     current->getIsAccountActive() ? "1" : "0",
                     dbDateTime(current->getCreateTime()),
                     current->getGender(),
                     std::to_string(current->getAge()),
                     current->getTelephone(),
                     current->getEmail(),
                     dbStr(current->department),
                     std::to_string(static_cast<int>(current->title)),
                     dbStr(current->specialty),
                     dbStr(current->scheduleInfo),
                     std::to_string(current->consultationCount),
                     std::to_string(current->examinationCount),
                     std::to_string(current->hospitalizationApplyCount),
                     current->isOnDuty ? "1" : "0",
                     "0"});
            }
            current = current->next;
        }
        db.commit();
        std::cout << "医生数据保存成功！" << std::endl;
    }
    catch (...)
    {
        db.rollback();
        std::cerr << "医生数据保存失败，已回滚！" << std::endl;
    }
}

void saveNurseData(Nurse *nurseHead, int count)
{
    Database &db = GetDB();
    if (!db.isConnected()) return;

    try
    {
        db.beginTransaction();
        Nurse *current = nurseHead;
        while (current)
        {
            if (current->isDeleted)
            {
                db.executePrepared(
                    "UPDATE nurses SET is_deleted = 1 WHERE nurse_id = ?",
                    {current->getUserID()});
            }
            else
            {
                db.executePrepared(
                    "INSERT INTO nurses (nurse_id, username, stored_hash, salt, login_attempts, is_active, "
                    "create_time, gender, age, telephone, email, department, level, schedule_info, "
                    "patient_care_count, bed_manage_count, is_on_duty, is_deleted) "
                    "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?) "
                    "ON DUPLICATE KEY UPDATE "
                    "username=VALUES(username), stored_hash=VALUES(stored_hash), salt=VALUES(salt), "
                    "login_attempts=VALUES(login_attempts), is_active=VALUES(is_active), "
                    "create_time=VALUES(create_time), gender=VALUES(gender), age=VALUES(age), "
                    "telephone=VALUES(telephone), email=VALUES(email), department=VALUES(department), "
                    "level=VALUES(level), schedule_info=VALUES(schedule_info), "
                    "patient_care_count=VALUES(patient_care_count), bed_manage_count=VALUES(bed_manage_count), "
                    "is_on_duty=VALUES(is_on_duty), is_deleted=VALUES(is_deleted)",
                    {current->getUserID(), current->getUsername(), current->getStoredHash(),
                     current->getSalt(), std::to_string(current->getLoginAttempts()),
                     current->getIsAccountActive() ? "1" : "0",
                     dbDateTime(current->getCreateTime()),
                     current->getGender(),
                     std::to_string(current->getAge()),
                     current->getTelephone(),
                     current->getEmail(),
                     dbStr(current->department),
                     std::to_string(static_cast<int>(current->level)),
                     dbStr(current->scheduleInfo),
                     std::to_string(current->patientCareCount),
                     std::to_string(current->bedManageCount),
                     current->isOnDuty ? "1" : "0",
                     "0"});
            }
            current = current->next;
        }
        db.commit();
        std::cout << "护士数据保存成功！" << std::endl;
    }
    catch (...)
    {
        db.rollback();
        std::cerr << "护士数据保存失败，已回滚！" << std::endl;
    }
}

void savePharmacistData(Pharmacist *pharmacistHead, int count)
{
    Database &db = GetDB();
    if (!db.isConnected()) return;

    try
    {
        db.beginTransaction();
        Pharmacist *current = pharmacistHead;
        while (current)
        {
            if (current->isDeleted)
            {
                db.executePrepared(
                    "UPDATE pharmacists SET is_deleted = 1 WHERE pharmacist_id = ?",
                    {current->getUserID()});
            }
            else
            {
                db.executePrepared(
                    "INSERT INTO pharmacists (pharmacist_id, username, stored_hash, salt, login_attempts, is_active, "
                    "create_time, gender, age, telephone, email, department, level, schedule_info, "
                    "review_count, dispense_count, inventory_manage_count, is_on_duty, is_deleted) "
                    "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?) "
                    "ON DUPLICATE KEY UPDATE "
                    "username=VALUES(username), stored_hash=VALUES(stored_hash), salt=VALUES(salt), "
                    "login_attempts=VALUES(login_attempts), is_active=VALUES(is_active), "
                    "create_time=VALUES(create_time), gender=VALUES(gender), age=VALUES(age), "
                    "telephone=VALUES(telephone), email=VALUES(email), department=VALUES(department), "
                    "level=VALUES(level), schedule_info=VALUES(schedule_info), "
                    "review_count=VALUES(review_count), dispense_count=VALUES(dispense_count), "
                    "inventory_manage_count=VALUES(inventory_manage_count), "
                    "is_on_duty=VALUES(is_on_duty), is_deleted=VALUES(is_deleted)",
                    {current->getUserID(), current->getUsername(), current->getStoredHash(),
                     current->getSalt(), std::to_string(current->getLoginAttempts()),
                     current->getIsAccountActive() ? "1" : "0",
                     dbDateTime(current->getCreateTime()),
                     current->getGender(),
                     std::to_string(current->getAge()),
                     current->getTelephone(),
                     current->getEmail(),
                     dbStr(current->department),
                     std::to_string(static_cast<int>(current->level)),
                     dbStr(current->scheduleInfo),
                     std::to_string(current->reviewCount),
                     std::to_string(current->dispenseCount),
                     std::to_string(current->inventoryManageCount),
                     current->isOnDuty ? "1" : "0",
                     "0"});
            }
            current = current->next;
        }
        db.commit();
        std::cout << "药剂师数据保存成功！" << std::endl;
    }
    catch (...)
    {
        db.rollback();
        std::cerr << "药剂师数据保存失败，已回滚！" << std::endl;
    }
}

void savePatientData(Patient *patientHead, int count)
{
    Database &db = GetDB();
    if (!db.isConnected()) return;

    try
    {
        db.beginTransaction();
        Patient *current = patientHead;
        while (current)
        {
            if (current->isDeleted)
            {
                db.executePrepared(
                    "UPDATE patients SET is_deleted = 1 WHERE patient_id = ?",
                    {current->getUserID()});
            }
            else
            {
                db.executePrepared(
                    "INSERT INTO patients (patient_id, username, stored_hash, salt, login_attempts, is_active, "
                    "create_time, gender, age, telephone, email, department, address, id_card_number, "
                    "emergency_contact_name, emergency_contact_phone, allergy_history, past_medical_history, "
                    "marital_status, registration_count, consultation_count, hospitalization_count, "
                    "medication_count, balance_cents, is_hospitalized, is_deleted) "
                    "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?) "
                    "ON DUPLICATE KEY UPDATE "
                    "username=VALUES(username), stored_hash=VALUES(stored_hash), salt=VALUES(salt), "
                    "login_attempts=VALUES(login_attempts), is_active=VALUES(is_active), "
                    "create_time=VALUES(create_time), gender=VALUES(gender), age=VALUES(age), "
                    "telephone=VALUES(telephone), email=VALUES(email), department=VALUES(department), "
                    "address=VALUES(address), id_card_number=VALUES(id_card_number), "
                    "emergency_contact_name=VALUES(emergency_contact_name), "
                    "emergency_contact_phone=VALUES(emergency_contact_phone), "
                    "allergy_history=VALUES(allergy_history), past_medical_history=VALUES(past_medical_history), "
                    "marital_status=VALUES(marital_status), registration_count=VALUES(registration_count), "
                    "consultation_count=VALUES(consultation_count), hospitalization_count=VALUES(hospitalization_count), "
                    "medication_count=VALUES(medication_count), balance_cents=VALUES(balance_cents), "
                    "is_hospitalized=VALUES(is_hospitalized), is_deleted=VALUES(is_deleted)",
                    {current->getUserID(), current->getUsername(), current->getStoredHash(),
                     current->getSalt(), std::to_string(current->getLoginAttempts()),
                     current->getIsAccountActive() ? "1" : "0",
                     dbDateTime(current->getCreateTime()),
                     current->getGender(),
                     std::to_string(current->getAge()),
                     current->getTelephone(),
                     current->getEmail(),
                     dbStr(current->department),
                     dbStr(current->address),
                     dbStr(current->idCardNumber),
                     dbStr(current->emergencyContactName),
                     dbStr(current->emergencyContactPhone),
                     dbStr(current->allergyHistory),
                     dbStr(current->pastMedicalHistory),
                     std::to_string(static_cast<int>(current->maritalStatus)),
                     std::to_string(current->registrationCount),
                     std::to_string(current->consultationCount),
                     std::to_string(current->hospitalizationCount),
                     std::to_string(current->medicationCount),
                     std::to_string(current->balance),
                     current->isHospitalized ? "1" : "0",
                     "0"});
            }
            current = current->next;
        }
        db.commit();
        std::cout << "患者数据保存成功！" << std::endl;
    }
    catch (...)
    {
        db.rollback();
        std::cerr << "患者数据保存失败，已回滚！" << std::endl;
    }
}

// =====================MySQL版本：医疗记录数据保存函数=====================

void saveRegistrations(Registration *regHead, int count)
{
    Database &db = GetDB();
    if (!db.isConnected()) return;

    try
    {
        db.beginTransaction();
        Registration *current = regHead;
        while (current)
        {
            if (current->isDeleted)
            {
                db.executePrepared(
                    "UPDATE registrations SET is_deleted = 1 WHERE registration_id = ?",
                    {current->registrationID});
            }
            else
            {
                db.executePrepared(
                    "INSERT INTO registrations (registration_id, patient_id, department, doctor_id, "
                    "register_time, fee_cents, status, note, is_deleted) "
                    "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?) "
                    "ON DUPLICATE KEY UPDATE "
                    "patient_id=VALUES(patient_id), department=VALUES(department), "
                    "doctor_id=VALUES(doctor_id), register_time=VALUES(register_time), "
                    "fee_cents=VALUES(fee_cents), status=VALUES(status), note=VALUES(note), "
                    "is_deleted=VALUES(is_deleted)",
                    {current->registrationID,
                     current->patientID,
                     dbStr(current->department),
                     current->doctorID,
                     dbDateTime(current->registerTime),
                     std::to_string(current->fee),
                     std::to_string(static_cast<int>(current->status)),
                     dbStr(current->note),
                     "0"});

                // Sub-table: related_record_ids — delete-then-insert strategy
                db.executePrepared(
                    "DELETE FROM related_record_ids WHERE record_type = 'registration' AND record_id = ?",
                    {current->registrationID});
                for (const auto &rid : current->relatedRegistrationIDs)
                {
                    db.executePrepared(
                        "INSERT INTO related_record_ids (record_type, record_id, related_id) VALUES ('registration', ?, ?)",
                        {current->registrationID, rid});
                }
            }
            current = current->next;
        }
        db.commit();
        std::cout << "挂号记录数据保存成功！" << std::endl;
    }
    catch (...)
    {
        db.rollback();
        std::cerr << "挂号记录数据保存失败，已回滚！" << std::endl;
    }
}

void saveConsultations(Consultation *conHead, int count)
{
    Database &db = GetDB();
    if (!db.isConnected()) return;

    try
    {
        db.beginTransaction();
        Consultation *current = conHead;
        while (current)
        {
            if (current->isDeleted)
            {
                db.executePrepared(
                    "UPDATE consultations SET is_deleted = 1 WHERE consultation_id = ?",
                    {current->consultationID});
            }
            else
            {
                db.executePrepared(
                    "INSERT INTO consultations (consultation_id, registration_id, patient_id, doctor_id, "
                    "consultation_time, department, chief_complaint, history_present_illness, "
                    "past_medical_history, family_history, preliminary_diagnosis, "
                    "is_prescription_reviewed, is_hospitalization_recommended, status, note, is_deleted) "
                    "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?) "
                    "ON DUPLICATE KEY UPDATE "
                    "registration_id=VALUES(registration_id), patient_id=VALUES(patient_id), "
                    "doctor_id=VALUES(doctor_id), consultation_time=VALUES(consultation_time), "
                    "department=VALUES(department), chief_complaint=VALUES(chief_complaint), "
                    "history_present_illness=VALUES(history_present_illness), "
                    "past_medical_history=VALUES(past_medical_history), family_history=VALUES(family_history), "
                    "preliminary_diagnosis=VALUES(preliminary_diagnosis), "
                    "is_prescription_reviewed=VALUES(is_prescription_reviewed), "
                    "is_hospitalization_recommended=VALUES(is_hospitalization_recommended), "
                    "status=VALUES(status), note=VALUES(note), is_deleted=VALUES(is_deleted)",
                    {current->consultationID,
                     current->registrationID,
                     current->patientID,
                     current->doctorID,
                     dbDateTime(current->consultationTime),
                     dbStr(current->department),
                     dbStr(current->chiefComplaint),
                     dbStr(current->historyOfPresentIllness),
                     dbStr(current->pastMedicalHistory),
                     dbStr(current->familyHistory),
                     dbStr(current->preliminaryDiagnosis),
                     current->isPrescriptionReviewed ? "1" : "0",
                     current->isHospitalizationRecommended ? "1" : "0",
                     std::to_string(static_cast<int>(current->status)),
                     dbStr(current->note),
                     "0"});

                // Sub-table: prescription_items — delete-then-insert
                db.executePrepared(
                    "DELETE FROM prescription_items WHERE consultation_id = ?",
                    {current->consultationID});
                for (const auto &pres : current->prescriptions)
                {
                    db.executePrepared(
                        "INSERT INTO prescription_items (consultation_id, medicine_id, name, dosage, frequency, duration, note) "
                        "VALUES (?, ?, ?, ?, ?, ?, ?)",
                        {current->consultationID,
                         pres.medicineID,
                         pres.name,
                         pres.dosage,
                         pres.frequency,
                         pres.duration,
                         dbStr(pres.note)});
                }

                // Sub-table: examination_items — delete-then-insert
                db.executePrepared(
                    "DELETE FROM examination_items WHERE consultation_id = ?",
                    {current->consultationID});
                for (const auto &item : current->examinationlist)
                {
                    db.executePrepared(
                        "INSERT INTO examination_items (consultation_id, item_name) VALUES (?, ?)",
                        {current->consultationID, item});
                }

                // Sub-table: attachment_files — delete-then-insert
                db.executePrepared(
                    "DELETE FROM attachment_files WHERE record_type = 'consultation' AND record_id = ?",
                    {current->consultationID});
                for (const auto &att : current->attachments)
                {
                    db.executePrepared(
                        "INSERT INTO attachment_files (record_type, record_id, file_path) VALUES ('consultation', ?, ?)",
                        {current->consultationID, att});
                }

                // Sub-table: related_record_ids — delete-then-insert
                db.executePrepared(
                    "DELETE FROM related_record_ids WHERE record_type = 'consultation' AND record_id = ?",
                    {current->consultationID});
                for (const auto &rid : current->relatedConsultationIDs)
                {
                    db.executePrepared(
                        "INSERT INTO related_record_ids (record_type, record_id, related_id) VALUES ('consultation', ?, ?)",
                        {current->consultationID, rid});
                }
            }
            current = current->next;
        }
        db.commit();
        std::cout << "看诊记录数据保存成功！" << std::endl;
    }
    catch (...)
    {
        db.rollback();
        std::cerr << "看诊记录数据保存失败，已回滚！" << std::endl;
    }
}

void saveExaminations(Examination *examHead, int count)
{
    Database &db = GetDB();
    if (!db.isConnected()) return;

    try
    {
        db.beginTransaction();
        Examination *current = examHead;
        while (current)
        {
            if (current->isDeleted)
            {
                db.executePrepared(
                    "UPDATE examinations SET is_deleted = 1 WHERE examination_id = ?",
                    {current->examinationID});
            }
            else
            {
                std::string examReportTime = dbDateTime(current->reportTime);
                db.executePrepared(
                    "INSERT INTO examinations (examination_id, consultation_id, patient_id, doctor_id, "
                    "department, item_name, order_time, report_time, report_summary, fee_cents, status, is_deleted) "
                    "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?) "
                    "ON DUPLICATE KEY UPDATE "
                    "consultation_id=VALUES(consultation_id), patient_id=VALUES(patient_id), "
                    "doctor_id=VALUES(doctor_id), department=VALUES(department), "
                    "item_name=VALUES(item_name), order_time=VALUES(order_time), "
                    "report_time=VALUES(report_time), report_summary=VALUES(report_summary), "
                    "fee_cents=VALUES(fee_cents), status=VALUES(status), is_deleted=VALUES(is_deleted)",
                    {current->examinationID,
                     current->consultationID,
                     current->patientID,
                     current->doctorID,
                     dbStr(current->department),
                     dbStr(current->itemName),
                     dbDateTime(current->orderTime),
                     examReportTime,
                     dbStr(current->reportSummary),
                     std::to_string(current->fee),
                     std::to_string(static_cast<int>(current->status)),
                     "0"},
                    {false, false, false, false, false, false, false,
                     examReportTime.empty(),  // reportTime → SQL NULL when empty
                     false, false, false, false});

                // Sub-table: vital_signs — delete-then-insert
                const VitalSigns &vs = current->vitalSigns;
                db.executePrepared(
                    "DELETE FROM vital_signs WHERE record_type = 'examination' AND record_id = ?",
                    {current->examinationID});
                db.executePrepared(
                    "INSERT INTO vital_signs (record_type, record_id, temperature_c, systolic_bp, diastolic_bp, "
                    "heart_rate, respiratory_rate, spo2, height, weight, bmi, pain_score, "
                    "waist_circumference, blood_sugar, body_fat, uric_acid, cholesterol) "
                    "VALUES ('examination', ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)",
                    {current->examinationID,
                     std::to_string(vs.temperatureC),
                     std::to_string(vs.systolicBP),
                     std::to_string(vs.diastolicBP),
                     std::to_string(vs.heartRate),
                     std::to_string(vs.respiratoryRate),
                     std::to_string(vs.spo2),
                     std::to_string(vs.height),
                     std::to_string(vs.weight),
                     std::to_string(vs.bmi),
                     std::to_string(vs.painScore),
                     std::to_string(vs.waistCircumference),
                     std::to_string(vs.bloodSugar),
                     std::to_string(vs.bodyFat),
                     std::to_string(vs.uricAcid),
                     std::to_string(vs.cholesterol)});

                // Sub-table: attachment_files — delete-then-insert
                db.executePrepared(
                    "DELETE FROM attachment_files WHERE record_type = 'examination' AND record_id = ?",
                    {current->examinationID});
                for (const auto &att : current->attachments)
                {
                    db.executePrepared(
                        "INSERT INTO attachment_files (record_type, record_id, file_path) VALUES ('examination', ?, ?)",
                        {current->examinationID, att});
                }

                // Sub-table: related_record_ids — delete-then-insert
                db.executePrepared(
                    "DELETE FROM related_record_ids WHERE record_type = 'examination' AND record_id = ?",
                    {current->examinationID});
                for (const auto &rid : current->relatedExaminationIDs)
                {
                    db.executePrepared(
                        "INSERT INTO related_record_ids (record_type, record_id, related_id) VALUES ('examination', ?, ?)",
                        {current->examinationID, rid});
                }
            }
            current = current->next;
        }
        db.commit();
        std::cout << "检查记录数据保存成功！" << std::endl;
    }
    catch (...)
    {
        db.rollback();
        std::cerr << "检查记录数据保存失败，已回滚！" << std::endl;
    }
}

void saveHospitalizations(Hospitalization *hosHead, int count)
{
    Database &db = GetDB();
    if (!db.isConnected()) return;

    try
    {
        db.beginTransaction();
        Hospitalization *current = hosHead;
        while (current)
        {
            if (current->isDeleted)
            {
                db.executePrepared(
                    "UPDATE hospitalizations SET is_deleted = 1 WHERE hospitalization_id = ?",
                    {current->hospitalizationID});
            }
            else
            {
                std::string hosAdmitTime = dbDateTime(current->admitTime);
                std::string hosDischargeTime = dbDateTime(current->dischargeTime);
                std::string hosAvailableAdmitTime = dbDateTime(current->availableAdmitTime);
                db.executePrepared(
                    "INSERT INTO hospitalizations (hospitalization_id, consultation_id, patient_id, doctor_id, "
                    "nurse_id, department, ward_type, bed_number, apply_time, admit_time, discharge_time, "
                    "available_admit_time, deposit_cents, total_cost_cents, status, is_deleted) "
                    "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?) "
                    "ON DUPLICATE KEY UPDATE "
                    "consultation_id=VALUES(consultation_id), patient_id=VALUES(patient_id), "
                    "doctor_id=VALUES(doctor_id), nurse_id=VALUES(nurse_id), department=VALUES(department), "
                    "ward_type=VALUES(ward_type), bed_number=VALUES(bed_number), apply_time=VALUES(apply_time), "
                    "admit_time=VALUES(admit_time), discharge_time=VALUES(discharge_time), "
                    "available_admit_time=VALUES(available_admit_time), deposit_cents=VALUES(deposit_cents), "
                    "total_cost_cents=VALUES(total_cost_cents), status=VALUES(status), is_deleted=VALUES(is_deleted)",
                    {current->hospitalizationID,
                     current->consultationID,
                     current->patientID,
                     current->doctorID,
                     current->nurseID,
                     dbStr(current->department),
                     dbStr(current->wardType),
                     current->bedNumber,
                     dbDateTime(current->applyTime),
                     hosAdmitTime,
                     hosDischargeTime,
                     hosAvailableAdmitTime,
                     std::to_string(current->deposit),
                     std::to_string(current->totalCost),
                     std::to_string(static_cast<int>(current->status)),
                     "0"},
                    {false, false, false, false, false, false, false, false, false,
                     hosAdmitTime.empty(),           // admitTime → SQL NULL when empty
                     hosDischargeTime.empty(),        // dischargeTime → SQL NULL when empty
                     hosAvailableAdmitTime.empty(),   // availableAdmitTime → SQL NULL when empty
                     false, false, false, false});

                // Sub-table: related_record_ids — delete-then-insert
                db.executePrepared(
                    "DELETE FROM related_record_ids WHERE record_type = 'hospitalization' AND record_id = ?",
                    {current->hospitalizationID});
                for (const auto &rid : current->relatedHospitalizationIDs)
                {
                    db.executePrepared(
                        "INSERT INTO related_record_ids (record_type, record_id, related_id) VALUES ('hospitalization', ?, ?)",
                        {current->hospitalizationID, rid});
                }
            }
            current = current->next;
        }
        db.commit();
        std::cout << "住院记录数据保存成功！" << std::endl;
    }
    catch (...)
    {
        db.rollback();
        std::cerr << "住院记录数据保存失败，已回滚！" << std::endl;
    }
}

void saveMedicationRecords(MedicationRecord *medRecHead, int count)
{
    Database &db = GetDB();
    if (!db.isConnected()) return;

    try
    {
        db.beginTransaction();
        MedicationRecord *current = medRecHead;
        while (current)
        {
            if (current->isDeleted)
            {
                db.executePrepared(
                    "UPDATE medication_records SET is_deleted = 1 WHERE medication_record_id = ?",
                    {current->medRecordID});
            }
            else
            {
                std::string medPaymentTime = dbDateTime(current->paymentTime);
                std::string medDispenseTime = dbDateTime(current->dispenseTime);
                db.executePrepared(
                    "INSERT INTO medication_records (medication_record_id, consultation_id, doctor_id, "
                    "pharmacist_id, patient_id, department, create_time, total_cost_cents, "
                    "review_status, status, payment_time, dispense_time, note, is_deleted) "
                    "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?) "
                    "ON DUPLICATE KEY UPDATE "
                    "consultation_id=VALUES(consultation_id), doctor_id=VALUES(doctor_id), "
                    "pharmacist_id=VALUES(pharmacist_id), patient_id=VALUES(patient_id), "
                    "department=VALUES(department), create_time=VALUES(create_time), "
                    "total_cost_cents=VALUES(total_cost_cents), review_status=VALUES(review_status), "
                    "status=VALUES(status), payment_time=VALUES(payment_time), "
                    "dispense_time=VALUES(dispense_time), note=VALUES(note), is_deleted=VALUES(is_deleted)",
                    {current->medRecordID,
                     current->consultationID,
                     current->doctorID,
                     current->pharmacistID,
                     current->patientID,
                     dbStr(current->department),
                     dbDateTime(current->createTime),
                     std::to_string(current->totalCost),
                     std::to_string(static_cast<int>(current->reviewStatus)),
                     std::to_string(static_cast<int>(current->status)),
                     medPaymentTime,
                     medDispenseTime,
                     dbStr(current->note),
                     "0"},
                    {false, false, false, false, false, false, false, false, false, false,
                     medPaymentTime.empty(),   // paymentTime → SQL NULL when empty
                     medDispenseTime.empty(),  // dispenseTime → SQL NULL when empty
                     false, false});

                // Sub-table: medication_lines — delete-then-insert
                db.executePrepared(
                    "DELETE FROM medication_lines WHERE medication_record_id = ?",
                    {current->medRecordID});
                for (const auto &line : current->lines)
                {
                    db.executePrepared(
                        "INSERT INTO medication_lines (medication_record_id, medicine_id, medicine_name, "
                        "quantity, unit_price_cents, note) VALUES (?, ?, ?, ?, ?, ?)",
                        {current->medRecordID,
                         line.medicineID,
                         line.medicineName,
                         std::to_string(line.quantity),
                         std::to_string(line.unitPrice),
                         dbStr(line.note)});
                }
            }
            current = current->next;
        }
        db.commit();
        std::cout << "用药记录数据保存成功！" << std::endl;
    }
    catch (...)
    {
        db.rollback();
        std::cerr << "用药记录数据保存失败，已回滚！" << std::endl;
    }
}

void saveMedicines(Medicine *medHead, int count)
{
    Database &db = GetDB();
    if (!db.isConnected()) return;

    try
    {
        db.beginTransaction();
        Medicine *current = medHead;
        while (current)
        {
            if (current->isDeleted)
            {
                db.executePrepared(
                    "UPDATE medicines SET is_deleted = 1 WHERE medicine_id = ?",
                    {current->medicineID});
            }
            else
            {
                db.executePrepared(
                    "INSERT INTO medicines (medicine_id, name, specification, manufacturer, "
                    "purchase_price_cents, sale_price_cents, stock, safety_stock, "
                    "production_date, expiry_date, department, is_special, generic_name, "
                    "status, note, is_deleted) "
                    "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?) "
                    "ON DUPLICATE KEY UPDATE "
                    "name=VALUES(name), specification=VALUES(specification), manufacturer=VALUES(manufacturer), "
                    "purchase_price_cents=VALUES(purchase_price_cents), sale_price_cents=VALUES(sale_price_cents), "
                    "stock=VALUES(stock), safety_stock=VALUES(safety_stock), "
                    "production_date=VALUES(production_date), expiry_date=VALUES(expiry_date), "
                    "department=VALUES(department), is_special=VALUES(is_special), "
                    "generic_name=VALUES(generic_name), status=VALUES(status), note=VALUES(note), "
                    "is_deleted=VALUES(is_deleted)",
                    {current->medicineID,
                     current->name,
                     dbStr(current->specification),
                     dbStr(current->manufacturer),
                     std::to_string(current->purchasePrice),
                     std::to_string(current->salePrice),
                     std::to_string(current->stock),
                     std::to_string(current->safetyStock),
                     current->productionDate,
                     current->expiryDate,
                     dbStr(current->department),
                     current->isSpecial ? "1" : "0",
                     dbStr(current->genericName),
                     std::to_string(static_cast<int>(current->status)),
                     dbStr(current->note),
                     "0"});

                // Sub-table: medicine_aliases — delete-then-insert
                db.executePrepared(
                    "DELETE FROM medicine_aliases WHERE medicine_id = ?",
                    {current->medicineID});
                for (const auto &alias : current->aliases)
                {
                    db.executePrepared(
                        "INSERT INTO medicine_aliases (medicine_id, alias_name) VALUES (?, ?)",
                        {current->medicineID, alias});
                }
            }
            current = current->next;
        }
        db.commit();
        std::cout << "药品信息数据保存成功！" << std::endl;
    }
    catch (...)
    {
        db.rollback();
        std::cerr << "药品信息数据保存失败，已回滚！" << std::endl;
    }
}

void saveBedInfos(bedInfo *bedHead, int count)
{
    Database &db = GetDB();
    if (!db.isConnected()) return;

    try
    {
        db.beginTransaction();
        bedInfo *current = bedHead;
        while (current)
        {
            if (current->isDeleted)
            {
                db.executePrepared(
                    "UPDATE bed_info SET is_deleted = 1 WHERE bed_id = ?",
                    {current->bedID});
            }
            else
            {
                std::string bedPatientID = (current->patientID == "#" || current->patientID.empty()) ? "" : current->patientID;
                std::string bedNurseID = (current->nurseID == "#" || current->nurseID.empty()) ? "" : current->nurseID;
                db.executePrepared(
                    "INSERT INTO bed_info (bed_id, status, ward_type, department, area_number, "
                    "ward_number, bed_number, note, patient_id, nurse_id, is_deleted, use_times, days_occupied) "
                    "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?) "
                    "ON DUPLICATE KEY UPDATE "
                    "status=VALUES(status), ward_type=VALUES(ward_type), department=VALUES(department), "
                    "area_number=VALUES(area_number), ward_number=VALUES(ward_number), "
                    "bed_number=VALUES(bed_number), note=VALUES(note), patient_id=VALUES(patient_id), "
                    "nurse_id=VALUES(nurse_id), is_deleted=VALUES(is_deleted), "
                    "use_times=VALUES(use_times), days_occupied=VALUES(days_occupied)",
                    {current->bedID,
                     std::to_string(static_cast<int>(current->status)),
                     dbStr(current->wardType),
                     dbStr(current->department),
                     std::to_string(current->areaNumber),
                     std::to_string(current->wardNumber),
                     std::to_string(current->bedNumber),
                     dbStr(current->note),
                     bedPatientID,
                     bedNurseID,
                     "0",
                     std::to_string(current->useTimes),
                     std::to_string(current->daysOccupied)},
                    {false, false, false, false, false, false, false, false,
                     bedPatientID.empty(),  // patient_id → SQL NULL when empty
                     bedNurseID.empty(),    // nurse_id → SQL NULL when empty
                     false, false, false});

                // Sub-table: vital_signs — delete-then-insert
                const VitalSigns &vs = current->vitalSigns;
                db.executePrepared(
                    "DELETE FROM vital_signs WHERE record_type = 'bed' AND record_id = ?",
                    {current->bedID});
                db.executePrepared(
                    "INSERT INTO vital_signs (record_type, record_id, temperature_c, systolic_bp, diastolic_bp, "
                    "heart_rate, respiratory_rate, spo2, height, weight, bmi, pain_score, "
                    "waist_circumference, blood_sugar, body_fat, uric_acid, cholesterol) "
                    "VALUES ('bed', ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)",
                    {current->bedID,
                     std::to_string(vs.temperatureC),
                     std::to_string(vs.systolicBP),
                     std::to_string(vs.diastolicBP),
                     std::to_string(vs.heartRate),
                     std::to_string(vs.respiratoryRate),
                     std::to_string(vs.spo2),
                     std::to_string(vs.height),
                     std::to_string(vs.weight),
                     std::to_string(vs.bmi),
                     std::to_string(vs.painScore),
                     std::to_string(vs.waistCircumference),
                     std::to_string(vs.bloodSugar),
                     std::to_string(vs.bodyFat),
                     std::to_string(vs.uricAcid),
                     std::to_string(vs.cholesterol)});
            }
            current = current->next;
        }
        db.commit();
        std::cout << "床位信息数据保存成功！" << std::endl;
    }
    catch (...)
    {
        db.rollback();
        std::cerr << "床位信息数据保存失败，已回滚！" << std::endl;
    }
}

void saveNursingRecords(NursingRecord *head, int count)
{
    Database &db = GetDB();
    if (!db.isConnected()) return;

    try
    {
        db.beginTransaction();
        NursingRecord *current = head;
        while (current)
        {
            if (current->isDeleted)
            {
                db.executePrepared(
                    "UPDATE nursing_records SET is_deleted = 1 WHERE record_id = ?",
                    {current->recordID});
            }
            else
            {
                db.executePrepared(
                    "INSERT INTO nursing_records (record_id, patient_id, nurse_id, department, "
                    "operation_time, operation_type, detail, vital_signs_snapshot, note, is_deleted) "
                    "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?) "
                    "ON DUPLICATE KEY UPDATE "
                    "patient_id=VALUES(patient_id), nurse_id=VALUES(nurse_id), "
                    "department=VALUES(department), operation_time=VALUES(operation_time), "
                    "operation_type=VALUES(operation_type), detail=VALUES(detail), "
                    "vital_signs_snapshot=VALUES(vital_signs_snapshot), note=VALUES(note), "
                    "is_deleted=VALUES(is_deleted)",
                    {current->recordID,
                     current->patientID,
                     current->nurseID,
                     dbStr(current->department),
                     dbDateTime(current->operationTime),
                     std::to_string(current->operationType),
                     dbStr(current->detail),
                     dbStr(current->vitalSignsSnapshot),
                     dbStr(current->note),
                     "0"});
            }
            current = current->next;
        }
        db.commit();
    }
    catch (...)
    {
        db.rollback();
        std::cerr << "护理记录数据保存失败，已回滚！" << std::endl;
    }
}
