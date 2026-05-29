#include "Modules/LoadData.h"
#include "Core/Database.h"

// =====================Static helper for MySQL vital signs parsing=====================

// Fill VitalSigns from a single row in the vital_signs table
// Column order: temperature_c, systolic_bp, diastolic_bp, heart_rate, respiratory_rate,
//               spo2, height, weight, bmi, pain_score, waist_circumference,
//               blood_sugar, body_fat, uric_acid, cholesterol
static void fillVitalSignsFromRow(VitalSigns &vs, MYSQL_ROW row)
{
    vs.temperatureC       = Database::readDouble(row, 0);
    vs.systolicBP         = Database::readDouble(row, 1);
    vs.diastolicBP        = Database::readDouble(row, 2);
    vs.heartRate          = Database::readDouble(row, 3);
    vs.respiratoryRate    = Database::readDouble(row, 4);
    vs.spo2               = Database::readDouble(row, 5);
    vs.height             = Database::readDouble(row, 6);
    vs.weight             = Database::readDouble(row, 7);
    vs.bmi                = Database::readDouble(row, 8);
    vs.painScore          = Database::readDouble(row, 9);
    vs.waistCircumference = Database::readDouble(row, 10);
    vs.bloodSugar         = Database::readDouble(row, 11);
    vs.bodyFat            = Database::readDouble(row, 12);
    vs.uricAcid           = Database::readDouble(row, 13);
    vs.cholesterol        = Database::readDouble(row, 14);
}

// Common User base fields shared by all 5 role tables
// Column mapping (0-indexed): 0=id, 1=username, 2=stored_hash, 3=salt,
// 4=login_attempts, 5=is_active, 6=create_time, 7=gender, 8=age, 9=telephone, 10=email
static void fillUserBase(class User *u, MYSQL_ROW row)
{
    u->setUserID(Database::readString(row, 0));
    u->setUsername(Database::readString(row, 1));
    u->setStoredHash(Database::readString(row, 2));
    u->setSalt(Database::readString(row, 3));
    u->setLoginAttempts(Database::readInt(row, 4));
    u->setIsAccountActive(Database::readBool(row, 5));
    u->setCreateTime(Database::readString(row, 6));
    u->setGender(Database::readString(row, 7));
    u->setAge(Database::readInt(row, 8));
    u->setTelephone(Database::readString(row, 9));
    u->setEmail(Database::readString(row, 10));
}

// =====================User data loading functions (MySQL)=====================

Admin *loadAdminData(int &count)
{
    Database &db = GetDB();
    Admin *head = nullptr;
    count = 0;

    if (!db.isConnected()) return nullptr;

    MYSQL_RES *res = db.query("SELECT admin_id, username, stored_hash, salt, "
                               "login_attempts, is_active, create_time, gender, age, "
                               "telephone, email "
                               "FROM admins WHERE is_deleted = 0");
    if (!res) return nullptr;

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res)))
    {
        Admin *node = new Admin();
        // 0=admin_id, 1=username, 2=stored_hash, 3=salt, 4=login_attempts,
        // 5=is_active, 6=create_time, 7=gender, 8=age, 9=telephone, 10=email
        node->setUserID(Database::readString(row, 0));
        node->setUsername(Database::readString(row, 1));
        node->setStoredHash(Database::readString(row, 2));
        node->setSalt(Database::readString(row, 3));
        node->setLoginAttempts(Database::readInt(row, 4));
        node->setIsAccountActive(Database::readBool(row, 5));
        node->setCreateTime(Database::readString(row, 6));
        node->setGender(Database::readString(row, 7));
        node->setAge(Database::readInt(row, 8));
        node->setTelephone(Database::readString(row, 9));
        node->setEmail(Database::readString(row, 10));
        node->setIsDeleted(false);
        node->setRole(UserRole::ADMIN);

        // Head insertion
        node->next = head;
        if (head) head->prev = node;
        head = node;
    }
    mysql_free_result(res);

    // Get max ID counter: admin IDs are "0" + 5 digits
    MYSQL_RES *cntRes = db.query(
        "SELECT COALESCE(MAX(CAST(SUBSTRING(admin_id, 2) AS UNSIGNED)), 0) FROM admins");
    if (cntRes)
    {
        MYSQL_ROW cntRow = mysql_fetch_row(cntRes);
        if (cntRow) count = Database::readInt(cntRow, 0);
        mysql_free_result(cntRes);
    }

    return head;
}

Doctor *loadDoctorData(int &count)
{
    Database &db = GetDB();
    Doctor *head = nullptr;
    count = 0;

    if (!db.isConnected()) return nullptr;

    MYSQL_RES *res = db.query(
        "SELECT doctor_id, username, stored_hash, salt, "
        "login_attempts, is_active, create_time, gender, age, "
        "telephone, email, department, title, specialty, schedule_info, "
        "consultation_count, examination_count, hospitalization_apply_count, is_on_duty "
        "FROM doctors WHERE is_deleted = 0");
    if (!res) return nullptr;

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res)))
    {
        Doctor *node = new Doctor();
        // 0-10: base user fields
        node->setUserID(Database::readString(row, 0));
        node->setUsername(Database::readString(row, 1));
        node->setStoredHash(Database::readString(row, 2));
        node->setSalt(Database::readString(row, 3));
        node->setLoginAttempts(Database::readInt(row, 4));
        node->setIsAccountActive(Database::readBool(row, 5));
        node->setCreateTime(Database::readString(row, 6));
        node->setGender(Database::readString(row, 7));
        node->setAge(Database::readInt(row, 8));
        node->setTelephone(Database::readString(row, 9));
        node->setEmail(Database::readString(row, 10));
        // 11-18: doctor-specific fields
        node->doctorID = Database::readString(row, 0);
        node->department = Database::readString(row, 11);
        node->title = static_cast<DoctorTitle>(Database::readInt(row, 12));
        node->specialty = Database::readString(row, 13);
        node->scheduleInfo = Database::readString(row, 14);
        node->consultationCount = Database::readInt(row, 15);
        node->examinationCount = Database::readInt(row, 16);
        node->hospitalizationApplyCount = Database::readInt(row, 17);
        node->isOnDuty = Database::readBool(row, 18);
        node->setIsDeleted(false);
        node->setRole(UserRole::DOCTOR);

        // Head insertion
        node->next = head;
        if (head) head->prev = node;
        head = node;
    }
    mysql_free_result(res);

    // Get max ID counter: doctor IDs are "1" + 5 digits
    MYSQL_RES *cntRes = db.query(
        "SELECT COALESCE(MAX(CAST(SUBSTRING(doctor_id, 2) AS UNSIGNED)), 0) FROM doctors");
    if (cntRes)
    {
        MYSQL_ROW cntRow = mysql_fetch_row(cntRes);
        if (cntRow) count = Database::readInt(cntRow, 0);
        mysql_free_result(cntRes);
    }

    return head;
}

Nurse *loadNurseData(int &count)
{
    Database &db = GetDB();
    Nurse *head = nullptr;
    count = 0;

    if (!db.isConnected()) return nullptr;

    MYSQL_RES *res = db.query(
        "SELECT nurse_id, username, stored_hash, salt, "
        "login_attempts, is_active, create_time, gender, age, "
        "telephone, email, department, level, schedule_info, "
        "patient_care_count, bed_manage_count, is_on_duty "
        "FROM nurses WHERE is_deleted = 0");
    if (!res) return nullptr;

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res)))
    {
        Nurse *node = new Nurse();
        // 0-10: base user fields
        node->setUserID(Database::readString(row, 0));
        node->setUsername(Database::readString(row, 1));
        node->setStoredHash(Database::readString(row, 2));
        node->setSalt(Database::readString(row, 3));
        node->setLoginAttempts(Database::readInt(row, 4));
        node->setIsAccountActive(Database::readBool(row, 5));
        node->setCreateTime(Database::readString(row, 6));
        node->setGender(Database::readString(row, 7));
        node->setAge(Database::readInt(row, 8));
        node->setTelephone(Database::readString(row, 9));
        node->setEmail(Database::readString(row, 10));
        // 11-16: nurse-specific fields
        node->nurseID = Database::readString(row, 0);
        node->department = Database::readString(row, 11);
        node->level = static_cast<NurseLevel>(Database::readInt(row, 12));
        node->scheduleInfo = Database::readString(row, 13);
        node->patientCareCount = Database::readInt(row, 14);
        node->bedManageCount = Database::readInt(row, 15);
        node->isOnDuty = Database::readBool(row, 16);
        node->setIsDeleted(false);
        node->setRole(UserRole::NURSE);

        // Head insertion
        node->next = head;
        if (head) head->prev = node;
        head = node;
    }
    mysql_free_result(res);

    // Get max ID counter: nurse IDs are "2" + 5 digits
    MYSQL_RES *cntRes = db.query(
        "SELECT COALESCE(MAX(CAST(SUBSTRING(nurse_id, 2) AS UNSIGNED)), 0) FROM nurses");
    if (cntRes)
    {
        MYSQL_ROW cntRow = mysql_fetch_row(cntRes);
        if (cntRow) count = Database::readInt(cntRow, 0);
        mysql_free_result(cntRes);
    }

    return head;
}

Pharmacist *loadPharmacistData(int &count)
{
    Database &db = GetDB();
    Pharmacist *head = nullptr;
    count = 0;

    if (!db.isConnected()) return nullptr;

    MYSQL_RES *res = db.query(
        "SELECT pharmacist_id, username, stored_hash, salt, "
        "login_attempts, is_active, create_time, gender, age, "
        "telephone, email, department, level, schedule_info, "
        "review_count, dispense_count, inventory_manage_count, is_on_duty "
        "FROM pharmacists WHERE is_deleted = 0");
    if (!res) return nullptr;

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res)))
    {
        Pharmacist *node = new Pharmacist();
        // 0-10: base user fields
        node->setUserID(Database::readString(row, 0));
        node->setUsername(Database::readString(row, 1));
        node->setStoredHash(Database::readString(row, 2));
        node->setSalt(Database::readString(row, 3));
        node->setLoginAttempts(Database::readInt(row, 4));
        node->setIsAccountActive(Database::readBool(row, 5));
        node->setCreateTime(Database::readString(row, 6));
        node->setGender(Database::readString(row, 7));
        node->setAge(Database::readInt(row, 8));
        node->setTelephone(Database::readString(row, 9));
        node->setEmail(Database::readString(row, 10));
        // 11-17: pharmacist-specific fields
        node->pharmacistID = Database::readString(row, 0);
        node->department = Database::readString(row, 11);
        node->level = static_cast<PharmacistLevel>(Database::readInt(row, 12));
        node->scheduleInfo = Database::readString(row, 13);
        node->reviewCount = Database::readInt(row, 14);
        node->dispenseCount = Database::readInt(row, 15);
        node->inventoryManageCount = Database::readInt(row, 16);
        node->isOnDuty = Database::readBool(row, 17);
        node->setIsDeleted(false);
        node->setRole(UserRole::PHARMACIST);

        // Head insertion
        node->next = head;
        if (head) head->prev = node;
        head = node;
    }
    mysql_free_result(res);

    // Get max ID counter: pharmacist IDs are "3" + 5 digits
    MYSQL_RES *cntRes = db.query(
        "SELECT COALESCE(MAX(CAST(SUBSTRING(pharmacist_id, 2) AS UNSIGNED)), 0) FROM pharmacists");
    if (cntRes)
    {
        MYSQL_ROW cntRow = mysql_fetch_row(cntRes);
        if (cntRow) count = Database::readInt(cntRow, 0);
        mysql_free_result(cntRes);
    }

    return head;
}

Patient *loadPatientData(int &count)
{
    Database &db = GetDB();
    Patient *head = nullptr;
    count = 0;

    if (!db.isConnected()) return nullptr;

    MYSQL_RES *res = db.query(
        "SELECT patient_id, username, stored_hash, salt, "
        "login_attempts, is_active, create_time, gender, age, "
        "telephone, email, department, address, id_card_number, "
        "emergency_contact_name, emergency_contact_phone, allergy_history, "
        "past_medical_history, marital_status, registration_count, "
        "consultation_count, hospitalization_count, medication_count, "
        "balance_cents, is_hospitalized "
        "FROM patients WHERE is_deleted = 0");
    if (!res) return nullptr;

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res)))
    {
        Patient *node = new Patient();
        // 0-10: base user fields
        node->setUserID(Database::readString(row, 0));
        node->setUsername(Database::readString(row, 1));
        node->setStoredHash(Database::readString(row, 2));
        node->setSalt(Database::readString(row, 3));
        node->setLoginAttempts(Database::readInt(row, 4));
        node->setIsAccountActive(Database::readBool(row, 5));
        node->setCreateTime(Database::readString(row, 6));
        node->setGender(Database::readString(row, 7));
        node->setAge(Database::readInt(row, 8));
        node->setTelephone(Database::readString(row, 9));
        node->setEmail(Database::readString(row, 10));
        // 11-24: patient-specific fields
        node->patientID = Database::readString(row, 0);
        node->department = Database::readString(row, 11);
        node->address = Database::readString(row, 12);
        node->idCardNumber = Database::readString(row, 13);
        node->emergencyContactName = Database::readString(row, 14);
        node->emergencyContactPhone = Database::readString(row, 15);
        node->allergyHistory = Database::readString(row, 16);
        node->pastMedicalHistory = Database::readString(row, 17);
        node->maritalStatus = static_cast<MaritalStatus>(Database::readInt(row, 18));
        node->registrationCount = Database::readInt(row, 19);
        node->consultationCount = Database::readInt(row, 20);
        node->hospitalizationCount = Database::readInt(row, 21);
        node->medicationCount = Database::readInt(row, 22);
        node->balance = Database::readInt(row, 23); // already in cents
        node->isHospitalized = Database::readBool(row, 24);
        node->setIsDeleted(false);
        node->setRole(UserRole::PATIENT);

        // Head insertion
        node->next = head;
        if (head) head->prev = node;
        head = node;
    }
    mysql_free_result(res);

    // Get max ID counter: patient IDs are "4" + 5 digits
    MYSQL_RES *cntRes = db.query(
        "SELECT COALESCE(MAX(CAST(SUBSTRING(patient_id, 2) AS UNSIGNED)), 0) FROM patients");
    if (cntRes)
    {
        MYSQL_ROW cntRow = mysql_fetch_row(cntRes);
        if (cntRow) count = Database::readInt(cntRow, 0);
        mysql_free_result(cntRes);
    }

    return head;
}

// =====================Medical record data loading functions (MySQL)=====================

Registration *loadRegistrations(int &count)
{
    Database &db = GetDB();
    Registration *head = nullptr;
    count = 0;

    if (!db.isConnected()) return nullptr;

    MYSQL_RES *res = db.query(
        "SELECT registration_id, patient_id, department, doctor_id, "
        "register_time, fee_cents, status, note "
        "FROM registrations WHERE is_deleted = 0");
    if (!res) return nullptr;

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res)))
    {
        Registration *node = new Registration();
        // 0=registration_id, 1=patient_id, 2=department, 3=doctor_id,
        // 4=register_time, 5=fee_cents, 6=status, 7=note
        node->registrationID = Database::readString(row, 0);
        node->patientID = Database::readString(row, 1);
        node->department = Database::readString(row, 2);
        node->doctorID = Database::readString(row, 3);
        node->registerTime = Database::readString(row, 4);
        node->fee = Database::readInt(row, 5); // already in cents
        node->status = static_cast<RegistrationStatus>(Database::readInt(row, 6));
        node->note = Database::readString(row, 7);
        node->isDeleted = false;

        // Load related registration IDs from sub-table
        std::string regId = node->registrationID;
        MYSQL_RES *relRes = db.queryPrepared(
            "SELECT related_id FROM related_record_ids "
            "WHERE record_type='registration' AND record_id=?",
            {regId});
        if (relRes)
        {
            MYSQL_ROW relRow;
            while ((relRow = mysql_fetch_row(relRes)))
            {
                node->relatedRegistrationIDs.push_back(Database::readString(relRow, 0));
            }
            mysql_free_result(relRes);
        }

        // Head insertion
        node->next = head;
        if (head) head->prev = node;
        head = node;
    }
    mysql_free_result(res);

    // Get max ID counter: registration IDs are "reg" + 6 digits, SUBSTRING starts at 4
    MYSQL_RES *cntRes = db.query(
        "SELECT COALESCE(MAX(CAST(SUBSTRING(registration_id, 4) AS UNSIGNED)), 0) FROM registrations");
    if (cntRes)
    {
        MYSQL_ROW cntRow = mysql_fetch_row(cntRes);
        if (cntRow) count = Database::readInt(cntRow, 0);
        mysql_free_result(cntRes);
    }

    return head;
}

Consultation *loadConsultations(int &count)
{
    Database &db = GetDB();
    Consultation *head = nullptr;
    count = 0;

    if (!db.isConnected()) return nullptr;

    MYSQL_RES *res = db.query(
        "SELECT consultation_id, registration_id, patient_id, doctor_id, "
        "consultation_time, department, chief_complaint, history_present_illness, "
        "past_medical_history, family_history, preliminary_diagnosis, "
        "is_prescription_reviewed, is_hospitalization_recommended, status, note "
        "FROM consultations WHERE is_deleted = 0");
    if (!res) return nullptr;

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res)))
    {
        Consultation *node = new Consultation();
        // 0=consultation_id, 1=registration_id, 2=patient_id, 3=doctor_id,
        // 4=consultation_time, 5=department, 6=chief_complaint, 7=history_present_illness,
        // 8=past_medical_history, 9=family_history, 10=preliminary_diagnosis,
        // 11=is_prescription_reviewed, 12=is_hospitalization_recommended, 13=status, 14=note
        node->consultationID = Database::readString(row, 0);
        node->registrationID = Database::readString(row, 1);
        node->patientID = Database::readString(row, 2);
        node->doctorID = Database::readString(row, 3);
        node->consultationTime = Database::readString(row, 4);
        node->department = Database::readString(row, 5);
        node->chiefComplaint = Database::readString(row, 6);
        node->historyOfPresentIllness = Database::readString(row, 7);
        node->pastMedicalHistory = Database::readString(row, 8);
        node->familyHistory = Database::readString(row, 9);
        node->preliminaryDiagnosis = Database::readString(row, 10);
        node->isPrescriptionReviewed = Database::readBool(row, 11);
        node->isHospitalizationRecommended = Database::readBool(row, 12);
        node->status = static_cast<ConsultationStatus>(Database::readInt(row, 13));
        node->note = Database::readString(row, 14);
        node->isDeleted = false;

        std::string conId = node->consultationID;

        // Load prescriptions from sub-table
        MYSQL_RES *presRes = db.queryPrepared(
            "SELECT medicine_id, name, dosage, frequency, duration, note "
            "FROM prescription_items WHERE consultation_id=?",
            {conId});
        if (presRes)
        {
            MYSQL_ROW presRow;
            while ((presRow = mysql_fetch_row(presRes)))
            {
                Prescription pres;
                pres.medicineID = Database::readString(presRow, 0);
                pres.name = Database::readString(presRow, 1);
                pres.dosage = Database::readString(presRow, 2);
                pres.frequency = Database::readString(presRow, 3);
                pres.duration = Database::readString(presRow, 4);
                pres.note = Database::readString(presRow, 5);
                node->prescriptions.push_back(pres);
            }
            mysql_free_result(presRes);
        }

        // Load examination items from sub-table
        MYSQL_RES *examItemRes = db.queryPrepared(
            "SELECT item_name FROM examination_items WHERE consultation_id=?",
            {conId});
        if (examItemRes)
        {
            MYSQL_ROW examItemRow;
            while ((examItemRow = mysql_fetch_row(examItemRes)))
            {
                node->examinationlist.push_back(Database::readString(examItemRow, 0));
            }
            mysql_free_result(examItemRes);
        }

        // Load attachments from sub-table
        MYSQL_RES *attRes = db.queryPrepared(
            "SELECT file_path FROM attachment_files "
            "WHERE record_type='consultation' AND record_id=?",
            {conId});
        if (attRes)
        {
            MYSQL_ROW attRow;
            while ((attRow = mysql_fetch_row(attRes)))
            {
                node->attachments.push_back(Database::readString(attRow, 0));
            }
            mysql_free_result(attRes);
        }

        // Load related consultation IDs from sub-table
        MYSQL_RES *relRes = db.queryPrepared(
            "SELECT related_id FROM related_record_ids "
            "WHERE record_type='consultation' AND record_id=?",
            {conId});
        if (relRes)
        {
            MYSQL_ROW relRow;
            while ((relRow = mysql_fetch_row(relRes)))
            {
                node->relatedConsultationIDs.push_back(Database::readString(relRow, 0));
            }
            mysql_free_result(relRes);
        }

        // Head insertion
        node->next = head;
        if (head) head->prev = node;
        head = node;
    }
    mysql_free_result(res);

    // Get max ID counter: consultation IDs are "con" + 6 digits
    MYSQL_RES *cntRes = db.query(
        "SELECT COALESCE(MAX(CAST(SUBSTRING(consultation_id, 4) AS UNSIGNED)), 0) FROM consultations");
    if (cntRes)
    {
        MYSQL_ROW cntRow = mysql_fetch_row(cntRes);
        if (cntRow) count = Database::readInt(cntRow, 0);
        mysql_free_result(cntRes);
    }

    return head;
}

Examination *loadExaminations(int &count)
{
    Database &db = GetDB();
    Examination *head = nullptr;
    count = 0;

    if (!db.isConnected()) return nullptr;

    MYSQL_RES *res = db.query(
        "SELECT examination_id, consultation_id, patient_id, doctor_id, "
        "department, item_name, order_time, report_time, report_summary, "
        "fee_cents, status "
        "FROM examinations WHERE is_deleted = 0");
    if (!res) return nullptr;

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res)))
    {
        Examination *node = new Examination();
        // 0=examination_id, 1=consultation_id, 2=patient_id, 3=doctor_id,
        // 4=department, 5=item_name, 6=order_time, 7=report_time,
        // 8=report_summary, 9=fee_cents, 10=status
        node->examinationID = Database::readString(row, 0);
        node->consultationID = Database::readString(row, 1);
        node->patientID = Database::readString(row, 2);
        node->doctorID = Database::readString(row, 3);
        node->department = Database::readString(row, 4);
        node->itemName = Database::readString(row, 5);
        node->orderTime = Database::readString(row, 6);
        node->reportTime = Database::readString(row, 7);
        node->reportSummary = Database::readString(row, 8);
        node->fee = Database::readInt(row, 9); // already in cents
        node->status = static_cast<ExaminationStatus>(Database::readInt(row, 10));
        node->isDeleted = false;

        std::string examId = node->examinationID;

        // Load vital signs from sub-table
        MYSQL_RES *vsRes = db.queryPrepared(
            "SELECT temperature_c, systolic_bp, diastolic_bp, heart_rate, respiratory_rate, "
            "spo2, height, weight, bmi, pain_score, waist_circumference, "
            "blood_sugar, body_fat, uric_acid, cholesterol "
            "FROM vital_signs WHERE record_type='examination' AND record_id=?",
            {examId});
        if (vsRes)
        {
            MYSQL_ROW vsRow = mysql_fetch_row(vsRes);
            if (vsRow)
            {
                fillVitalSignsFromRow(node->vitalSigns, vsRow);
            }
            mysql_free_result(vsRes);
        }

        // Load attachments from sub-table
        MYSQL_RES *attRes = db.queryPrepared(
            "SELECT file_path FROM attachment_files "
            "WHERE record_type='examination' AND record_id=?",
            {examId});
        if (attRes)
        {
            MYSQL_ROW attRow;
            while ((attRow = mysql_fetch_row(attRes)))
            {
                node->attachments.push_back(Database::readString(attRow, 0));
            }
            mysql_free_result(attRes);
        }

        // Load related examination IDs from sub-table
        MYSQL_RES *relRes = db.queryPrepared(
            "SELECT related_id FROM related_record_ids "
            "WHERE record_type='examination' AND record_id=?",
            {examId});
        if (relRes)
        {
            MYSQL_ROW relRow;
            while ((relRow = mysql_fetch_row(relRes)))
            {
                node->relatedExaminationIDs.push_back(Database::readString(relRow, 0));
            }
            mysql_free_result(relRes);
        }

        // Head insertion
        node->next = head;
        if (head) head->prev = node;
        head = node;
    }
    mysql_free_result(res);

    // Get max ID counter: examination IDs are "exa" + 6 digits
    MYSQL_RES *cntRes = db.query(
        "SELECT COALESCE(MAX(CAST(SUBSTRING(examination_id, 4) AS UNSIGNED)), 0) FROM examinations");
    if (cntRes)
    {
        MYSQL_ROW cntRow = mysql_fetch_row(cntRes);
        if (cntRow) count = Database::readInt(cntRow, 0);
        mysql_free_result(cntRes);
    }

    return head;
}

Hospitalization *loadHospitalizations(int &count)
{
    Database &db = GetDB();
    Hospitalization *head = nullptr;
    count = 0;

    if (!db.isConnected()) return nullptr;

    MYSQL_RES *res = db.query(
        "SELECT hospitalization_id, consultation_id, patient_id, doctor_id, "
        "nurse_id, department, ward_type, bed_number, apply_time, admit_time, "
        "discharge_time, available_admit_time, deposit_cents, total_cost_cents, status "
        "FROM hospitalizations WHERE is_deleted = 0");
    if (!res) return nullptr;

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res)))
    {
        Hospitalization *node = new Hospitalization();
        // 0=hospitalization_id, 1=consultation_id, 2=patient_id, 3=doctor_id,
        // 4=nurse_id, 5=department, 6=ward_type, 7=bed_number,
        // 8=apply_time, 9=admit_time, 10=discharge_time, 11=available_admit_time,
        // 12=deposit_cents, 13=total_cost_cents, 14=status
        node->hospitalizationID = Database::readString(row, 0);
        node->consultationID = Database::readString(row, 1);
        node->patientID = Database::readString(row, 2);
        node->doctorID = Database::readString(row, 3);
        node->nurseID = Database::readString(row, 4);
        node->department = Database::readString(row, 5);
        node->wardType = Database::readString(row, 6);
        node->bedNumber = Database::readString(row, 7);
        node->applyTime = Database::readString(row, 8);
        node->admitTime = Database::readString(row, 9);
        node->dischargeTime = Database::readString(row, 10);
        node->availableAdmitTime = Database::readString(row, 11);
        node->deposit = Database::readInt(row, 12); // already in cents
        node->totalCost = Database::readInt(row, 13); // already in cents
        node->status = static_cast<HospitalizationStatus>(Database::readInt(row, 14));
        node->isDeleted = false;

        std::string hosId = node->hospitalizationID;

        // Load related hospitalization IDs from sub-table
        MYSQL_RES *relRes = db.queryPrepared(
            "SELECT related_id FROM related_record_ids "
            "WHERE record_type='hospitalization' AND record_id=?",
            {hosId});
        if (relRes)
        {
            MYSQL_ROW relRow;
            while ((relRow = mysql_fetch_row(relRes)))
            {
                node->relatedHospitalizationIDs.push_back(Database::readString(relRow, 0));
            }
            mysql_free_result(relRes);
        }

        // Head insertion
        node->next = head;
        if (head) head->prev = node;
        head = node;
    }
    mysql_free_result(res);

    // Get max ID counter: hospitalization IDs are "hos" + 6 digits
    MYSQL_RES *cntRes = db.query(
        "SELECT COALESCE(MAX(CAST(SUBSTRING(hospitalization_id, 4) AS UNSIGNED)), 0) FROM hospitalizations");
    if (cntRes)
    {
        MYSQL_ROW cntRow = mysql_fetch_row(cntRes);
        if (cntRow) count = Database::readInt(cntRow, 0);
        mysql_free_result(cntRes);
    }

    return head;
}

MedicationRecord *loadMedicationRecords(int &count)
{
    Database &db = GetDB();
    MedicationRecord *head = nullptr;
    count = 0;

    if (!db.isConnected()) return nullptr;

    MYSQL_RES *res = db.query(
        "SELECT medication_record_id, consultation_id, doctor_id, pharmacist_id, "
        "patient_id, department, create_time, total_cost_cents, review_status, "
        "status, payment_time, dispense_time, note "
        "FROM medication_records WHERE is_deleted = 0");
    if (!res) return nullptr;

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res)))
    {
        MedicationRecord *node = new MedicationRecord();
        // 0=medication_record_id, 1=consultation_id, 2=doctor_id, 3=pharmacist_id,
        // 4=patient_id, 5=department, 6=create_time, 7=total_cost_cents,
        // 8=review_status, 9=status, 10=payment_time, 11=dispense_time, 12=note
        node->medRecordID = Database::readString(row, 0);
        node->consultationID = Database::readString(row, 1);
        node->doctorID = Database::readString(row, 2);
        node->pharmacistID = Database::readString(row, 3);
        node->patientID = Database::readString(row, 4);
        node->department = Database::readString(row, 5);
        node->createTime = Database::readString(row, 6);
        node->totalCost = Database::readInt(row, 7); // already in cents
        node->reviewStatus = static_cast<MedicationReviewStatus>(Database::readInt(row, 8));
        node->status = static_cast<MedicationStatus>(Database::readInt(row, 9));
        node->paymentTime = Database::readString(row, 10);
        node->dispenseTime = Database::readString(row, 11);
        node->note = Database::readString(row, 12);
        node->isDeleted = false;

        std::string mrdId = node->medRecordID;

        // Load medication lines from sub-table
        MYSQL_RES *lineRes = db.queryPrepared(
            "SELECT medicine_id, medicine_name, quantity, unit_price_cents, note "
            "FROM medication_lines WHERE medication_record_id=?",
            {mrdId});
        if (lineRes)
        {
            MYSQL_ROW lineRow;
            while ((lineRow = mysql_fetch_row(lineRes)))
            {
                MedicationLine medLine;
                medLine.medicineID = Database::readString(lineRow, 0);
                medLine.medicineName = Database::readString(lineRow, 1);
                medLine.quantity = Database::readInt(lineRow, 2);
                medLine.unitPrice = Database::readInt(lineRow, 3); // already in cents
                medLine.note = Database::readString(lineRow, 4);
                node->lines.push_back(medLine);
            }
            mysql_free_result(lineRes);
        }

        // Head insertion
        node->prev = nullptr;
        node->next = head;
        if (head) head->prev = node;
        head = node;
    }
    mysql_free_result(res);

    // Get max ID counter: medication record IDs are "mrd" + 6 digits
    MYSQL_RES *cntRes = db.query(
        "SELECT COALESCE(MAX(CAST(SUBSTRING(medication_record_id, 4) AS UNSIGNED)), 0) FROM medication_records");
    if (cntRes)
    {
        MYSQL_ROW cntRow = mysql_fetch_row(cntRes);
        if (cntRow) count = Database::readInt(cntRow, 0);
        mysql_free_result(cntRes);
    }

    return head;
}

Medicine *loadMedicines(int &count)
{
    Database &db = GetDB();
    Medicine *head = nullptr;
    count = 0;

    if (!db.isConnected()) return nullptr;

    MYSQL_RES *res = db.query(
        "SELECT medicine_id, name, specification, manufacturer, "
        "purchase_price_cents, sale_price_cents, stock, safety_stock, "
        "production_date, expiry_date, department, is_special, "
        "generic_name, status, note "
        "FROM medicines WHERE is_deleted = 0");
    if (!res) return nullptr;

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res)))
    {
        Medicine *node = new Medicine();
        // 0=medicine_id, 1=name, 2=specification, 3=manufacturer,
        // 4=purchase_price_cents, 5=sale_price_cents, 6=stock, 7=safety_stock,
        // 8=production_date, 9=expiry_date, 10=department, 11=is_special,
        // 12=generic_name, 13=status, 14=note
        node->medicineID = Database::readString(row, 0);
        node->name = Database::readString(row, 1);
        node->specification = Database::readString(row, 2);
        node->manufacturer = Database::readString(row, 3);
        node->purchasePrice = Database::readInt(row, 4); // already in cents
        node->salePrice = Database::readInt(row, 5); // already in cents
        node->stock = Database::readInt(row, 6);
        node->safetyStock = Database::readInt(row, 7);
        node->productionDate = Database::readString(row, 8);
        node->expiryDate = Database::readString(row, 9);
        node->department = Database::readString(row, 10);
        node->isSpecial = Database::readBool(row, 11);
        node->genericName = Database::readString(row, 12);
        node->status = static_cast<MedicineStatus>(Database::readInt(row, 13));
        node->note = Database::readString(row, 14);
        node->isDeleted = false;

        std::string medId = node->medicineID;

        // Load aliases from sub-table
        MYSQL_RES *aliasRes = db.queryPrepared(
            "SELECT alias FROM medicine_aliases WHERE medicine_id=?",
            {medId});
        if (aliasRes)
        {
            MYSQL_ROW aliasRow;
            while ((aliasRow = mysql_fetch_row(aliasRes)))
            {
                node->aliases.push_back(Database::readString(aliasRow, 0));
            }
            mysql_free_result(aliasRes);
        }

        // Head insertion
        node->prev = nullptr;
        node->next = head;
        if (head) head->prev = node;
        head = node;
    }
    mysql_free_result(res);

    // Get max ID counter: medicine IDs are "med" + 6 digits
    MYSQL_RES *cntRes = db.query(
        "SELECT COALESCE(MAX(CAST(SUBSTRING(medicine_id, 4) AS UNSIGNED)), 0) FROM medicines");
    if (cntRes)
    {
        MYSQL_ROW cntRow = mysql_fetch_row(cntRes);
        if (cntRow) count = Database::readInt(cntRow, 0);
        mysql_free_result(cntRes);
    }

    return head;
}

bedInfo *loadBedInfos(int &count)
{
    Database &db = GetDB();
    bedInfo *head = nullptr;
    count = 0;

    if (!db.isConnected()) return nullptr;

    MYSQL_RES *res = db.query(
        "SELECT bed_id, status, ward_type, department, area_number, "
        "ward_number, bed_number, note, patient_id, nurse_id, "
        "use_times, days_occupied "
        "FROM bed_info WHERE is_deleted = 0");
    if (!res) return nullptr;

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res)))
    {
        bedInfo *node = new bedInfo();
        // 0=bed_id, 1=status, 2=ward_type, 3=department, 4=area_number,
        // 5=ward_number, 6=bed_number, 7=note, 8=patient_id, 9=nurse_id,
        // 10=use_times, 11=days_occupied
        node->bedID = Database::readString(row, 0);
        node->status = static_cast<bedStatus>(Database::readInt(row, 1));
        node->wardType = Database::readString(row, 2);
        node->department = Database::readString(row, 3);
        node->areaNumber = Database::readInt(row, 4);
        node->wardNumber = Database::readInt(row, 5);
        node->bedNumber = Database::readInt(row, 6);
        node->note = Database::readString(row, 7);
        node->patientID = Database::readString(row, 8);
        node->nurseID = Database::readString(row, 9);
        node->useTimes = Database::readInt(row, 10);
        node->daysOccupied = Database::readInt(row, 11);
        node->isDeleted = false;

        std::string bedId = node->bedID;

        // Load vital signs from sub-table
        MYSQL_RES *vsRes = db.queryPrepared(
            "SELECT temperature_c, systolic_bp, diastolic_bp, heart_rate, respiratory_rate, "
            "spo2, height, weight, bmi, pain_score, waist_circumference, "
            "blood_sugar, body_fat, uric_acid, cholesterol "
            "FROM vital_signs WHERE record_type='bed' AND record_id=?",
            {bedId});
        if (vsRes)
        {
            MYSQL_ROW vsRow = mysql_fetch_row(vsRes);
            if (vsRow)
            {
                fillVitalSignsFromRow(node->vitalSigns, vsRow);
            }
            mysql_free_result(vsRes);
        }

        // Head insertion
        node->next = head;
        if (head) head->prev = node;
        head = node;
    }
    mysql_free_result(res);

    // Bed IDs are auto-generated format, count total rows for counter
    MYSQL_RES *cntRes = db.query(
        "SELECT COUNT(*) FROM bed_info");
    if (cntRes)
    {
        MYSQL_ROW cntRow = mysql_fetch_row(cntRes);
        if (cntRow) count = Database::readInt(cntRow, 0);
        mysql_free_result(cntRes);
    }

    return head;
}

NursingRecord *loadNursingRecords(int &count)
{
    Database &db = GetDB();
    NursingRecord *head = nullptr;
    count = 0;

    if (!db.isConnected()) return nullptr;

    MYSQL_RES *res = db.query(
        "SELECT record_id, patient_id, nurse_id, department, operation_time, "
        "operation_type, detail, vital_signs_snapshot, note "
        "FROM nursing_records WHERE is_deleted = 0");
    if (!res) return nullptr;

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res)))
    {
        NursingRecord *node = new NursingRecord();
        // 0=record_id, 1=patient_id, 2=nurse_id, 3=department, 4=operation_time,
        // 5=operation_type, 6=detail, 7=vital_signs_snapshot, 8=note
        node->recordID = Database::readString(row, 0);
        node->patientID = Database::readString(row, 1);
        node->nurseID = Database::readString(row, 2);
        node->department = Database::readString(row, 3);
        node->operationTime = Database::readString(row, 4);
        node->operationType = Database::readInt(row, 5);
        node->detail = Database::readString(row, 6);
        node->vitalSignsSnapshot = Database::readString(row, 7);
        node->note = Database::readString(row, 8);
        node->isDeleted = false;

        // Head insertion
        node->next = head;
        if (head) head->prev = node;
        head = node;
    }
    mysql_free_result(res);

    // Get max ID counter: nursing record IDs are "nur" + 6 digits
    MYSQL_RES *cntRes = db.query(
        "SELECT COALESCE(MAX(CAST(SUBSTRING(record_id, 4) AS UNSIGNED)), 0) FROM nursing_records");
    if (cntRes)
    {
        MYSQL_ROW cntRow = mysql_fetch_row(cntRes);
        if (cntRow) count = Database::readInt(cntRow, 0);
        mysql_free_result(cntRes);
    }

    return head;
}
