#include "../Head/LoadData.h"

// =====================人物数据加载函数=====================
Admin *loadAdminData(int &count)
{
    Admin *adminHead = nullptr;
    std::ifstream inFile(ADMIN_FILE);
    if (!inFile)
    {
        std::cerr << "无法打开管理员数据文件！" << std::endl;
        count = 0;
        return nullptr;
    }

    std::string line;
    count = 0;
    while (std::getline(inFile, line))
    {
        if (line.rfind("count:", 0) == 0)
        {
            std::string countStr = line.substr(6);
            try
            {
                count = std::stoi(countStr);
            }
            catch (...)
            {
                count = 0;
            }
            break;
        }
        if (line.empty())
            continue;

        std::istringstream iss(line);
        Admin *newAdmin = new Admin();
        std::string isActiveStr, totalRevenueStr, totalExpensesStr, netProfitStr, isDeletedStr;
        std::string userID, username, gender, ageStr, telephone, email, storedHash, salt, createTime;

        // 按保存顺序读取
        std::getline(iss, userID, ',');
        std::getline(iss, username, ',');
        std::getline(iss, gender, ',');
        std::getline(iss, ageStr, ',');
        std::getline(iss, telephone, ',');
        std::getline(iss, email, ',');
        std::getline(iss, storedHash, ',');
        std::getline(iss, salt, ',');
        std::getline(iss, isActiveStr, ',');
        std::getline(iss, createTime, ',');
        std::getline(iss, totalRevenueStr, ',');
        std::getline(iss, totalExpensesStr, ',');
        std::getline(iss, netProfitStr, ',');
        std::getline(iss, isDeletedStr, ',');

        newAdmin->setUserID(userID);
        newAdmin->setUsername(username);
        newAdmin->setGender(gender);
        try
        {
            newAdmin->setAge(std::stoi(ageStr));
        }
        catch (...)
        {
            newAdmin->setAge(0);
        }
        newAdmin->setTelephone(telephone);
        newAdmin->setEmail(email);
        newAdmin->setStoredHash(storedHash);
        newAdmin->setSalt(salt);
        newAdmin->setIsAccountActive(isActiveStr == "1");
        newAdmin->setCreateTime(createTime);
        newAdmin->getTotalRevenue() = std::stod(totalRevenueStr);
        newAdmin->getTotalExpenses() = std::stod(totalExpensesStr);
        newAdmin->getNetProfit() = std::stod(netProfitStr);
        newAdmin->setIsDeleted(isDeletedStr == "1");
        newAdmin->setRole(UserRole::ADMIN);

        // 插入链表（头插法，维护双向指针）
        newAdmin->next = adminHead;
        if (adminHead != nullptr)
            adminHead->prev = newAdmin;
        adminHead = newAdmin;
    }
    inFile.close();
    return adminHead;
}

Doctor *loadDoctorData(int &count)
{
    Doctor *doctorHead = nullptr;
    std::ifstream inFile(DOCTOR_FILE);
    if (!inFile)
    {
        std::cerr << "无法打开医生数据文件！" << std::endl;
        count = 0;
        return nullptr;
    }

    std::string line;
    count = 0;
    while (std::getline(inFile, line))
    {
        if (line.rfind("count:", 0) == 0)
        {
            std::string countStr = line.substr(6);
            try
            {
                count = std::stoi(countStr);
            }
            catch (...)
            {
                count = 0;
            }
            break;
        }
        if (line.empty())
            continue;

        std::istringstream iss(line);
        Doctor *newDoctor = new Doctor();
        std::string isActiveStr, isOnDutyStr, isDeletedStr;
        std::string userID, username, gender, ageStr, telephone, email, storedHash, salt, createTime;
        std::string doctorID, department, titleStr, specialty, scheduleInfo, consultationCountStr, examinationCountStr, hospitalizationApplyCountStr;

        // 按保存顺序读取
        std::getline(iss, userID, ',');
        std::getline(iss, username, ',');
        std::getline(iss, gender, ',');
        std::getline(iss, ageStr, ',');
        std::getline(iss, telephone, ',');
        std::getline(iss, email, ',');
        std::getline(iss, storedHash, ',');
        std::getline(iss, salt, ',');
        std::getline(iss, isActiveStr, ',');
        std::getline(iss, createTime, ',');
        std::getline(iss, doctorID, ',');
        std::getline(iss, department, ',');
        std::getline(iss, titleStr, ',');
        std::getline(iss, specialty, ',');
        std::getline(iss, scheduleInfo, ',');
        std::getline(iss, consultationCountStr, ',');
        std::getline(iss, examinationCountStr, ',');
        std::getline(iss, hospitalizationApplyCountStr, ',');
        std::getline(iss, isOnDutyStr, ',');
        std::getline(iss, isDeletedStr);

        // 设置属性
        newDoctor->setUserID(userID);
        newDoctor->setUsername(username);
        newDoctor->setGender(gender);
        try
        {
            newDoctor->setAge(std::stoi(ageStr));
        }
        catch (...)
        {
            newDoctor->setAge(0);
        }
        newDoctor->setTelephone(telephone);
        newDoctor->setEmail(email);
        newDoctor->setStoredHash(storedHash);
        newDoctor->setSalt(salt);
        newDoctor->setIsAccountActive(isActiveStr == "1");
        newDoctor->setCreateTime(createTime);

        newDoctor->doctorID = doctorID;
        newDoctor->department = department;
        try
        {
            newDoctor->title = static_cast<DoctorTitle>(std::stoi(titleStr));
        }
        catch (...)
        {
            newDoctor->title = DoctorTitle::INTERN;
        }
        newDoctor->specialty = specialty;
        newDoctor->scheduleInfo = scheduleInfo;
        try
        {
            newDoctor->consultationCount = std::stoi(consultationCountStr);
        }
        catch (...)
        {
            newDoctor->consultationCount = 0;
        }
        try
        {
            newDoctor->examinationCount = std::stoi(examinationCountStr);
        }
        catch (...)
        {
            newDoctor->examinationCount = 0;
        }
        try
        {
            newDoctor->hospitalizationApplyCount = std::stoi(hospitalizationApplyCountStr);
        }
        catch (...)
        {
            newDoctor->hospitalizationApplyCount = 0;
        }
        newDoctor->isOnDuty = (isOnDutyStr == "1");
        newDoctor->setIsDeleted(isDeletedStr == "1");
        newDoctor->setRole(UserRole::DOCTOR);

        // 插入链表头部
        newDoctor->next = doctorHead;
        if (doctorHead != nullptr) doctorHead->prev = newDoctor;
        doctorHead = newDoctor;
    }
    inFile.close();
    return doctorHead;
}

Nurse *loadNurseData(int &count)
{
    Nurse *nurseHead = nullptr;
    std::ifstream inFile(NURSE_FILE);
    if (!inFile)
    {
        std::cerr << "无法打开护士数据文件！" << std::endl;
        count = 0;
        return nullptr;
    }

    std::string line;
    count = 0;
    while (std::getline(inFile, line))
    {
        if (line.rfind("count:", 0) == 0)
        {
            std::string countStr = line.substr(6);
            try
            {
                count = std::stoi(countStr);
            }
            catch (...)
            {
                count = 0;
            }
            break;
        }
        if (line.empty())
            continue;

        std::istringstream iss(line);
        Nurse *newNurse = new Nurse();
        std::string isActiveStr, levelStr, isOnDutyStr, isDeletedStr;
        std::string userID, username, gender, ageStr, telephone, email, storedHash, salt, createTime;
        std::string nurseID, department, scheduleInfo, patientCareCountStr, bedManageCountStr;

        // 按保存顺序读取
        std::getline(iss, userID, ',');
        std::getline(iss, username, ',');
        std::getline(iss, gender, ',');
        std::getline(iss, ageStr, ',');
        std::getline(iss, telephone, ',');
        std::getline(iss, email, ',');
        std::getline(iss, storedHash, ',');
        std::getline(iss, salt, ',');
        std::getline(iss, isActiveStr, ',');
        std::getline(iss, createTime, ',');
        std::getline(iss, nurseID, ',');
        std::getline(iss, department, ',');
        std::getline(iss, levelStr, ',');
        std::getline(iss, scheduleInfo, ',');
        std::getline(iss, patientCareCountStr, ',');
        std::getline(iss, bedManageCountStr, ',');
        std::getline(iss, isOnDutyStr, ',');
        std::getline(iss, isDeletedStr);

        // 设置属性
        newNurse->setUserID(userID);
        newNurse->setUsername(username);
        newNurse->setGender(gender);
        try
        {
            newNurse->setAge(std::stoi(ageStr));
        }
        catch (...)
        {
            newNurse->setAge(0);
        }
        newNurse->setTelephone(telephone);
        newNurse->setEmail(email);
        newNurse->setStoredHash(storedHash);
        newNurse->setSalt(salt);
        newNurse->setIsAccountActive(isActiveStr == "1");
        newNurse->setCreateTime(createTime);

        newNurse->nurseID = nurseID;
        newNurse->department = department;
        try
        {
            newNurse->level = static_cast<NurseLevel>(std::stoi(levelStr));
        }
        catch (...)
        {
            newNurse->level = NurseLevel::INTERN;
        }
        newNurse->scheduleInfo = scheduleInfo;
        try
        {
            newNurse->patientCareCount = std::stoi(patientCareCountStr);
        }
        catch (...)
        {
            newNurse->patientCareCount = 0;
        }
        try
        {
            newNurse->bedManageCount = std::stoi(bedManageCountStr);
        }
        catch (...)
        {
            newNurse->bedManageCount = 0;
        }
        newNurse->isOnDuty = (isOnDutyStr == "1");
        newNurse->setIsDeleted(isDeletedStr == "1");
        newNurse->setRole(UserRole::NURSE);

        // 插入链表头部
        newNurse->next = nurseHead;
        if (nurseHead != nullptr) nurseHead->prev = newNurse;
        nurseHead = newNurse;
    }
    inFile.close();
    return nurseHead;
}

Pharmacist *loadPharmacistData(int &count)
{
    Pharmacist *pharmacistHead = nullptr;
    std::ifstream inFile(PHARMACIST_FILE);
    if (!inFile)
    {
        std::cerr << "无法打开药剂师数据文件！" << std::endl;
        count = 0;
        return nullptr;
    }

    std::string line;
    count = 0;
    while (std::getline(inFile, line))
    {
        if (line.rfind("count:", 0) == 0)
        {
            std::string countStr = line.substr(6);
            try
            {
                count = std::stoi(countStr);
            }
            catch (...)
            {
                count = 0;
            }
            break;
        }
        if (line.empty())
            continue;

        std::istringstream iss(line);
        Pharmacist *newPharmacist = new Pharmacist();
        std::string isActiveStr, levelStr, isOnDutyStr, isDeletedStr;
        std::string userID, username, gender, ageStr, telephone, email, storedHash, salt, createTime;
        std::string pharmacistID, department, scheduleInfo, reviewCountStr, dispenseCountStr, inventoryManageCountStr;

        // 按保存顺序读取
        std::getline(iss, userID, ',');
        std::getline(iss, username, ',');
        std::getline(iss, gender, ',');
        std::getline(iss, ageStr, ',');
        std::getline(iss, telephone, ',');
        std::getline(iss, email, ',');
        std::getline(iss, storedHash, ',');
        std::getline(iss, salt, ',');
        std::getline(iss, isActiveStr, ',');
        std::getline(iss, createTime, ',');
        std::getline(iss, pharmacistID, ',');
        std::getline(iss, department, ',');
        std::getline(iss, levelStr, ',');
        std::getline(iss, scheduleInfo, ',');
        std::getline(iss, reviewCountStr, ',');
        std::getline(iss, dispenseCountStr, ',');
        std::getline(iss, inventoryManageCountStr, ',');
        std::getline(iss, isOnDutyStr, ',');
        std::getline(iss, isDeletedStr);

        // 设置属性
        newPharmacist->setUserID(userID);
        newPharmacist->setUsername(username);
        newPharmacist->setGender(gender);
        try
        {
            newPharmacist->setAge(std::stoi(ageStr));
        }
        catch (...)
        {
            newPharmacist->setAge(0);
        }
        newPharmacist->setTelephone(telephone);
        newPharmacist->setEmail(email);
        newPharmacist->setStoredHash(storedHash);
        newPharmacist->setSalt(salt);
        newPharmacist->setIsAccountActive(isActiveStr == "1");
        newPharmacist->setCreateTime(createTime);

        newPharmacist->pharmacistID = pharmacistID;
        newPharmacist->department = department;
        try
        {
            newPharmacist->level = static_cast<PharmacistLevel>(std::stoi(levelStr));
        }
        catch (...)
        {
            newPharmacist->level = PharmacistLevel::INTERN;
        }
        newPharmacist->scheduleInfo = scheduleInfo;
        try
        {
            newPharmacist->reviewCount = std::stoi(reviewCountStr);
        }
        catch (...)
        {
            newPharmacist->reviewCount = 0;
        }
        try
        {
            newPharmacist->dispenseCount = std::stoi(dispenseCountStr);
        }
        catch (...)
        {
            newPharmacist->dispenseCount = 0;
        }
        try
        {
            newPharmacist->inventoryManageCount = std::stoi(inventoryManageCountStr);
        }
        catch (...)
        {
            newPharmacist->inventoryManageCount = 0;
        }
        newPharmacist->isOnDuty = (isOnDutyStr == "1");
        newPharmacist->setIsDeleted(isDeletedStr == "1");
        newPharmacist->setRole(UserRole::PHARMACIST);

        // 插入链表头部
        newPharmacist->next = pharmacistHead;
        if (pharmacistHead != nullptr) pharmacistHead->prev = newPharmacist;
        pharmacistHead = newPharmacist;
    }
    inFile.close();
    return pharmacistHead;
}

Patient *loadPatientData(int &count)
{
    Patient *patientHead = nullptr;
    std::ifstream inFile(PATIENT_FILE);
    if (!inFile)
    {
        std::cerr << "无法打开患者数据文件！" << std::endl;
        count = 0;
        return nullptr;
    }

    std::string line;
    count = 0;
    while (std::getline(inFile, line))
    {
        if (line.rfind("count:", 0) == 0)
        {
            std::string countStr = line.substr(6);
            try
            {
                count = std::stoi(countStr);
            }
            catch (...)
            {
                count = 0;
            }
            break;
        }
        if (line.empty())
            continue;

        std::istringstream iss(line);
        Patient *newPatient = new Patient();
        std::string isActiveStr, maritalStatusStr, isHospitalizedStr, isDeletedStr;
        std::string userID, username, gender, ageStr, telephone, email, storedHash, salt, createTime;
        std::string patientID, department, address, idCardNumber, emergencyContactName, emergencyContactPhone, allergyHistory, pastMedicalHistory;
        std::string registrationCountStr, consultationCountStr, hospitalizationCountStr, medicationCountStr, balanceStr;

        // 按保存顺序读取
        std::getline(iss, userID, ',');
        std::getline(iss, username, ',');
        std::getline(iss, gender, ',');
        std::getline(iss, ageStr, ',');
        std::getline(iss, telephone, ',');
        std::getline(iss, email, ',');
        std::getline(iss, storedHash, ',');
        std::getline(iss, salt, ',');
        std::getline(iss, isActiveStr, ',');
        std::getline(iss, createTime, ',');
        std::getline(iss, patientID, ',');
        std::getline(iss, department, ',');
        std::getline(iss, address, ',');
        std::getline(iss, idCardNumber, ',');
        std::getline(iss, emergencyContactName, ',');
        std::getline(iss, emergencyContactPhone, ',');
        std::getline(iss, allergyHistory, ',');
        std::getline(iss, pastMedicalHistory, ',');
        std::getline(iss, maritalStatusStr, ',');
        std::getline(iss, registrationCountStr, ',');
        std::getline(iss, consultationCountStr, ',');
        std::getline(iss, hospitalizationCountStr, ',');
        std::getline(iss, medicationCountStr, ',');
        std::getline(iss, balanceStr, ',');
        std::getline(iss, isHospitalizedStr, ',');
        std::getline(iss, isDeletedStr);

        // 设置属性
        newPatient->setUserID(userID);
        newPatient->setUsername(username);
        newPatient->setGender(gender);
        try
        {
            newPatient->setAge(std::stoi(ageStr));
        }
        catch (...)
        {
            newPatient->setAge(0);
        }
        newPatient->setTelephone(telephone);
        newPatient->setEmail(email);
        newPatient->setStoredHash(storedHash);
        newPatient->setSalt(salt);
        newPatient->setIsAccountActive(isActiveStr == "1");
        newPatient->setCreateTime(createTime);

        newPatient->patientID = patientID;
        newPatient->department = department;
        newPatient->address = address;
        newPatient->idCardNumber = idCardNumber;
        newPatient->emergencyContactName = emergencyContactName;
        newPatient->emergencyContactPhone = emergencyContactPhone;
        newPatient->allergyHistory = allergyHistory;
        newPatient->pastMedicalHistory = pastMedicalHistory;
        try
        {
            newPatient->maritalStatus = static_cast<MaritalStatus>(std::stoi(maritalStatusStr));
        }
        catch (...)
        {
            newPatient->maritalStatus = MaritalStatus::SINGLE;
        }
        try
        {
            newPatient->registrationCount = std::stoi(registrationCountStr);
        }
        catch (...)
        {
            newPatient->registrationCount = 0;
        }
        try
        {
            newPatient->consultationCount = std::stoi(consultationCountStr);
        }
        catch (...)
        {
            newPatient->consultationCount = 0;
        }
        try
        {
            newPatient->hospitalizationCount = std::stoi(hospitalizationCountStr);
        }
        catch (...)
        {
            newPatient->hospitalizationCount = 0;
        }
        try
        {
            newPatient->medicationCount = std::stoi(medicationCountStr);
        }
        catch (...)
        {
            newPatient->medicationCount = 0;
        }
        try
        {
            newPatient->balance = std::stod(balanceStr);
        }
        catch (...)
        {
            newPatient->balance = 0.0;
        }
        newPatient->isHospitalized = (isHospitalizedStr == "1");
        newPatient->setIsDeleted(isDeletedStr == "1");
        newPatient->setRole(UserRole::PATIENT);

        // 插入链表头部
        newPatient->next = patientHead;
        if (patientHead != nullptr) patientHead->prev = newPatient;
        patientHead = newPatient;
    }
    inFile.close();
    return patientHead;
}

// =====================医疗记录数据加载函数=====================
Registration *loadRegistrations(int &count)
{
    Registration *regHead = nullptr;
    std::ifstream inFile(REGISTRATION_FILE);
    if (!inFile)
    {
        std::cerr << "无法打开挂号记录文件！" << std::endl;
        count = 0;
        return nullptr;
    }

    std::string line;
    count = 0;
    Registration *lastReg = nullptr;
    while (std::getline(inFile, line))
    {
        if (line.rfind("count:", 0) == 0)
        {
            std::string countStr = line.substr(6);
            try
            {
                count = std::stoi(countStr);
            }
            catch (...)
            {
                count = 0;
            }
            break;
        }
        if (line.empty())
            continue;

        if (line.rfind("RELATED_REGISTRATION_ID:", 0) == 0)
        {
            if (lastReg)
                lastReg->relatedRegistrationIDs.push_back(line.substr(24));
            continue;
        }

        std::istringstream iss(line);
        Registration *newReg = new Registration();
        std::string feeStr, statusStr, isDeletedStr;
        std::getline(iss, newReg->registrationID, ',');
        std::getline(iss, newReg->patientID, ',');
        std::getline(iss, newReg->department, ',');
        std::getline(iss, newReg->doctorID, ',');
        std::getline(iss, newReg->registerTime, ',');
        std::getline(iss, feeStr, ',');
        std::getline(iss, statusStr, ',');
        std::getline(iss, newReg->note, ',');
        std::getline(iss, isDeletedStr);

        try
        {
            newReg->fee = std::stod(feeStr);
        }
        catch (...)
        {
            newReg->fee = 0.0;
        }
        try
        {
            newReg->status = static_cast<RegistrationStatus>(std::stoi(statusStr));
        }
        catch (...)
        {
            newReg->status = RegistrationStatus::BOOKED;
        }
        newReg->isDeleted = (isDeletedStr == "1");

        // 插入链表头部
        newReg->next = regHead;
        if (regHead)
            regHead->prev = newReg;
        regHead = newReg;
        lastReg = newReg;
    }
    inFile.close();
    return regHead;
}

Consultation *loadConsultations(int &count)
{
    Consultation *conHead = nullptr;
    std::ifstream inFile(CONSULTATION_FILE);
    if (!inFile)
    {
        std::cerr << "无法打开看诊记录文件！" << std::endl;
        count = 0;
        return nullptr;
    }

    std::string line;
    count = 0;
    Consultation *lastCon = nullptr;
    while (std::getline(inFile, line))
    {
        if (line.rfind("count:", 0) == 0)
        {
            std::string countStr = line.substr(6);
            try
            {
                count = std::stoi(countStr);
            }
            catch (...)
            {
                count = 0;
            }
            break;
        }
        if (line.empty())
            continue;

        if (line.rfind("PRESCRIPTION:", 0) == 0)
        {
            if (lastCon)
            {
                std::string presStr = line.substr(13);
                std::istringstream iss(presStr);
                Prescription pres;
                std::getline(iss, pres.medicineID, ',');
                std::getline(iss, pres.name, ',');
                std::getline(iss, pres.dosage, ',');
                std::getline(iss, pres.frequency, ',');
                std::getline(iss, pres.duration, ',');
                std::getline(iss, pres.note);
                lastCon->prescriptions.push_back(pres);
            }
            continue;
        }
        if (line.rfind("EXAMINATION_ITEM:", 0) == 0)
        {
            if (lastCon)
                lastCon->examinationlist.push_back(line.substr(17));
            continue;
        }
        if (line.rfind("ATTACHMENT:", 0) == 0)
        {
            if (lastCon)
                lastCon->attachments.push_back(line.substr(11));
            continue;
        }
        if (line.rfind("RELATED_CONSULTATION_ID:", 0) == 0)
        {
            if (lastCon)
                lastCon->relatedConsultationIDs.push_back(line.substr(24));
            continue;
        }

        std::istringstream iss(line);
        Consultation *newCon = new Consultation();
        std::string isPrescriptionReviewedStr, isHospitalizationRecommendedStr, statusStr, isDeletedStr;
        std::getline(iss, newCon->consultationID, ',');
        std::getline(iss, newCon->registrationID, ',');
        std::getline(iss, newCon->patientID, ',');
        std::getline(iss, newCon->doctorID, ',');
        std::getline(iss, newCon->consultationTime, ',');
        std::getline(iss, newCon->department, ',');
        std::getline(iss, newCon->chiefComplaint, ',');
        std::getline(iss, newCon->historyOfPresentIllness, ',');
        std::getline(iss, newCon->pastMedicalHistory, ',');
        std::getline(iss, newCon->familyHistory, ',');
        std::getline(iss, newCon->preliminaryDiagnosis, ',');
        std::getline(iss, isPrescriptionReviewedStr, ',');
        std::getline(iss, isHospitalizationRecommendedStr, ',');
        std::getline(iss, statusStr, ',');
        std::getline(iss, newCon->note, ',');
        std::getline(iss, isDeletedStr);

        newCon->isPrescriptionReviewed = (isPrescriptionReviewedStr == "1");
        newCon->isHospitalizationRecommended = (isHospitalizationRecommendedStr == "1");
        try
        {
            newCon->status = static_cast<ConsultationStatus>(std::stoi(statusStr));
        }
        catch (...)
        {
            newCon->status = ConsultationStatus::PENDING;
        }
        newCon->isDeleted = (isDeletedStr == "1");

        // 插入链表头部
        newCon->next = conHead;
        if (conHead)
            conHead->prev = newCon;
        conHead = newCon;
        lastCon = newCon;
    }
    inFile.close();
    return conHead;
}

Examination *loadExaminations(int &count)
{
    Examination *examHead = nullptr;
    std::ifstream inFile(EXAMINATION_FILE);
    if (!inFile)
    {
        std::cerr << "无法打开检查记录文件！" << std::endl;
        count = 0;
        return nullptr;
    }

    std::string line;
    count = 0;
    Examination *lastExam = nullptr;
    while (std::getline(inFile, line))
    {
        if (line.rfind("count:", 0) == 0)
        {
            std::string countStr = line.substr(6);
            try
            {
                count = std::stoi(countStr);
            }
            catch (...)
            {
                count = 0;
            }
            break;
        }
        if (line.empty())
            continue;

        if (line.rfind("VITAL_SIGNS:", 0) == 0)
        {
            if (lastExam)
            {
                std::string vsStr = line.substr(12);
                std::istringstream iss(vsStr);
                std::string field;
                std::vector<std::string> fields;
                while (std::getline(iss, field, ';'))
                    fields.push_back(field);

                // 按顺序赋值
                try
                {
                    lastExam->vitalSigns.temperatureC = std::stod(fields[0]);
                }
                catch (...)
                {
                    lastExam->vitalSigns.temperatureC = 0.0;
                }
                try
                {
                    lastExam->vitalSigns.systolicBP = std::stod(fields[1]);
                }
                catch (...)
                {
                    lastExam->vitalSigns.systolicBP = 0.0;
                }
                try
                {
                    lastExam->vitalSigns.diastolicBP = std::stod(fields[2]);
                }
                catch (...)
                {
                    lastExam->vitalSigns.diastolicBP = 0.0;
                }
                try
                {
                    lastExam->vitalSigns.heartRate = std::stod(fields[3]);
                }
                catch (...)
                {
                    lastExam->vitalSigns.heartRate = 0.0;
                }
                try
                {
                    lastExam->vitalSigns.respiratoryRate = std::stod(fields[4]);
                }
                catch (...)
                {
                    lastExam->vitalSigns.respiratoryRate = 0.0;
                }
                try
                {
                    lastExam->vitalSigns.spo2 = std::stod(fields[5]);
                }
                catch (...)
                {
                    lastExam->vitalSigns.spo2 = 0.0;
                }
                try
                {
                    lastExam->vitalSigns.height = std::stod(fields[6]);
                }
                catch (...)
                {
                    lastExam->vitalSigns.height = 0.0;
                }
                try
                {
                    lastExam->vitalSigns.weight = std::stod(fields[7]);
                }
                catch (...)
                {
                    lastExam->vitalSigns.weight = 0.0;
                }
                try
                {
                    lastExam->vitalSigns.bmi = std::stod(fields[8]);
                }
                catch (...)
                {
                    lastExam->vitalSigns.bmi = 0.0;
                }
                try
                {
                    lastExam->vitalSigns.painScore = std::stod(fields[9]);
                }
                catch (...)
                {
                    lastExam->vitalSigns.painScore = 0.0;
                }
                try
                {
                    lastExam->vitalSigns.waistCircumference = std::stod(fields[10]);
                }
                catch (...)
                {
                    lastExam->vitalSigns.waistCircumference = 0.0;
                }
                try
                {
                    lastExam->vitalSigns.bloodSugar = std::stod(fields[11]);
                }
                catch (...)
                {
                    lastExam->vitalSigns.bloodSugar = 0.0;
                }
                try
                {
                    lastExam->vitalSigns.bodyFat = std::stod(fields[12]);
                }
                catch (...)
                {
                    lastExam->vitalSigns.bodyFat = 0.0;
                }
                try
                {
                    lastExam->vitalSigns.uricAcid = std::stod(fields[13]);
                }
                catch (...)
                {
                    lastExam->vitalSigns.uricAcid = 0.0;
                }
                try
                {
                    lastExam->vitalSigns.cholesterol = std::stod(fields[14]);
                }
                catch (...)
                {
                    lastExam->vitalSigns.cholesterol = 0.0;
                }
            }
            continue;
        }
        if (line.rfind("ATTACHMENT:", 0) == 0)
        {
            if (lastExam)
                lastExam->attachments.push_back(line.substr(11));
            continue;
        }
        if (line.rfind("RELATED_EXAMINATION_ID:", 0) == 0)
        {
            if (lastExam)
                lastExam->relatedExaminationIDs.push_back(line.substr(23));
            continue;
        }

        // 主体数据
        std::istringstream iss(line);
        Examination *newExam = new Examination();
        std::string feeStr, statusStr, isDeletedStr;
        std::getline(iss, newExam->examinationID, ',');
        std::getline(iss, newExam->consultationID, ',');
        std::getline(iss, newExam->patientID, ',');
        std::getline(iss, newExam->doctorID, ',');
        std::getline(iss, newExam->department, ',');
        std::getline(iss, newExam->itemName, ',');
        std::getline(iss, newExam->orderTime, ',');
        std::getline(iss, newExam->reportTime, ',');
        std::getline(iss, newExam->reportSummary, ',');
        std::getline(iss, feeStr, ',');
        std::getline(iss, statusStr, ',');
        std::getline(iss, isDeletedStr);

        try
        {
            newExam->fee = std::stod(feeStr);
        }
        catch (...)
        {
            newExam->fee = 0.0;
        }
        try
        {
            newExam->status = static_cast<ExaminationStatus>(std::stoi(statusStr));
        }
        catch (...)
        {
            newExam->status = ExaminationStatus::ORDERED;
        }
        newExam->isDeleted = (isDeletedStr == "1");

        // 插入链表头部
        newExam->next = examHead;
        if (examHead)
            examHead->prev = newExam;
        examHead = newExam;
        lastExam = newExam;
    }
    inFile.close();
    return examHead;
}

Hospitalization *loadHospitalizations(int &count)
{
    Hospitalization *hosHead = nullptr;
    std::ifstream inFile(HOSPITALIZATION_FILE);
    if (!inFile)
    {
        std::cerr << "无法打开住院记录文件！" << std::endl;
        count = 0;
        return nullptr;
    }

    std::string line;
    count = 0;
    Hospitalization *lastHos = nullptr;
    while (std::getline(inFile, line))
    {
        if (line.rfind("count:", 0) == 0)
        {
            std::string countStr = line.substr(6);
            try
            {
                count = std::stoi(countStr);
            }
            catch (...)
            {
                count = 0;
            }
            break;
        }
        if (line.empty())
            continue;

        if (line.rfind("RELATED_HOSPITALIZATION_ID:", 0) == 0)
        {
            if (lastHos)
                lastHos->relatedHospitalizationIDs.push_back(line.substr(27));
            continue;
        }

        std::istringstream iss(line);
        Hospitalization *newHos = new Hospitalization();
        std::string depositStr, totalCostStr, statusStr, isDeletedStr;

        std::getline(iss, newHos->hospitalizationID, ',');
        std::getline(iss, newHos->consultationID, ',');
        std::getline(iss, newHos->patientID, ',');
        std::getline(iss, newHos->doctorID, ',');
        std::getline(iss, newHos->nurseID, ',');
        std::getline(iss, newHos->department, ',');
        std::getline(iss, newHos->wardType, ',');
        std::getline(iss, newHos->bedNumber, ',');
        std::getline(iss, newHos->applyTime, ',');
        std::getline(iss, newHos->admitTime, ',');
        std::getline(iss, newHos->dischargeTime, ',');
        std::getline(iss, newHos->availableAdmitTime, ',');
        std::getline(iss, depositStr, ',');
        std::getline(iss, totalCostStr, ',');
        std::getline(iss, statusStr, ',');
        std::getline(iss, isDeletedStr);

        try
        {
            newHos->deposit = std::stod(depositStr);
        }
        catch (...)
        {
            newHos->deposit = 0.0;
        }
        try
        {
            newHos->totalCost = std::stod(totalCostStr);
        }
        catch (...)
        {
            newHos->totalCost = 0.0;
        }
        try
        {
            newHos->status = static_cast<HospitalizationStatus>(std::stoi(statusStr));
        }
        catch (...)
        {
            newHos->status = HospitalizationStatus::APPLIED;
        }
        newHos->isDeleted = (isDeletedStr == "1");

        // 插入链表头部
        newHos->next = hosHead;
        if (hosHead)
            hosHead->prev = newHos;
        hosHead = newHos;
        lastHos = newHos;
    }
    inFile.close();
    return hosHead;
}

bedInfo *loadBedInfos(int &count)
{
    bedInfo *bedHead = nullptr;
    std::ifstream inFile(BED_FILE);
    if (!inFile)
    {
        std::cerr << "无法打开床位信息文件！" << std::endl;
        count = 0;
        return nullptr;
    }

    std::string line;
    count = 0;
    bedInfo *lastBed = nullptr;
    while (std::getline(inFile, line))
    {
        if (line.rfind("count:", 0) == 0)
        {
            try
            {
                count = std::stoi(line.substr(6));
            }
            catch (...)
            {
                count = 0;
            }
            break;
        }
        if (line.empty())
            continue;

        std::istringstream iss(line);
        bedInfo *newBed = new bedInfo();
        std::string statusStr, areaStr, wardStr, bedNumStr, isDeletedStr, useTimesStr, daysOccupiedStr;

        std::getline(iss, newBed->bedID, ',');
        std::getline(iss, statusStr, ',');
        std::getline(iss, newBed->wardType, ',');
        std::getline(iss, newBed->department, ',');
        std::getline(iss, areaStr, ',');
        std::getline(iss, wardStr, ',');
        std::getline(iss, bedNumStr, ',');
        std::getline(iss, newBed->note, ',');
        std::getline(iss, newBed->patientID, ',');
        std::getline(iss, newBed->nurseID, ',');
        std::getline(iss, isDeletedStr, ',');
        std::getline(iss, useTimesStr, ',');
        std::getline(iss, daysOccupiedStr);

        try
        {
            newBed->status = static_cast<bedStatus>(std::stoi(statusStr));
        }
        catch (...)
        {
            newBed->status = bedStatus::AVAILABLE;
        }
        try
        {
            newBed->areaNumber = std::stoi(areaStr);
        }
        catch (...)
        {
            newBed->areaNumber = 0;
        }
        try
        {
            newBed->wardNumber = std::stoi(wardStr);
        }
        catch (...)
        {
            newBed->wardNumber = 0;
        }
        try
        {
            newBed->bedNumber = std::stoi(bedNumStr);
        }
        catch (...)
        {
            newBed->bedNumber = 0;
        }
        newBed->isDeleted = (isDeletedStr == "1");
        try
        {
            newBed->useTimes = std::stoi(useTimesStr);
        }
        catch (...)
        {
            newBed->useTimes = 0;
        }
        try
        {
            newBed->daysOccupied = std::stoi(daysOccupiedStr);
        }
        catch (...)
        {
            newBed->daysOccupied = 0;
        }

        // 读取体征信息
        if (std::getline(inFile, line) && line.rfind("VITAL_SIGNS:", 0) == 0)
        {
            std::string vsStr = line.substr(12);
            std::istringstream vsStream(vsStr);
            std::string field;
            std::vector<std::string> fields;
            while (std::getline(vsStream, field, ';'))
                fields.push_back(field);

            try
            {
                newBed->vitalSigns.temperatureC = std::stod(fields[0]);
            }
            catch (...)
            {
                newBed->vitalSigns.temperatureC = 0.0;
            }
            try
            {
                newBed->vitalSigns.systolicBP = std::stod(fields[1]);
            }
            catch (...)
            {
                newBed->vitalSigns.systolicBP = 0.0;
            }
            try
            {
                newBed->vitalSigns.diastolicBP = std::stod(fields[2]);
            }
            catch (...)
            {
                newBed->vitalSigns.diastolicBP = 0.0;
            }
            try
            {
                newBed->vitalSigns.heartRate = std::stod(fields[3]);
            }
            catch (...)
            {
                newBed->vitalSigns.heartRate = 0.0;
            }
            try
            {
                newBed->vitalSigns.respiratoryRate = std::stod(fields[4]);
            }
            catch (...)
            {
                newBed->vitalSigns.respiratoryRate = 0.0;
            }
            try
            {
                newBed->vitalSigns.spo2 = std::stod(fields[5]);
            }
            catch (...)
            {
                newBed->vitalSigns.spo2 = 0.0;
            }
            try
            {
                newBed->vitalSigns.height = std::stod(fields[6]);
            }
            catch (...)
            {
                newBed->vitalSigns.height = 0.0;
            }
            try
            {
                newBed->vitalSigns.weight = std::stod(fields[7]);
            }
            catch (...)
            {
                newBed->vitalSigns.weight = 0.0;
            }
            try
            {
                newBed->vitalSigns.bmi = std::stod(fields[8]);
            }
            catch (...)
            {
                newBed->vitalSigns.bmi = 0.0;
            }
            try
            {
                newBed->vitalSigns.painScore = std::stod(fields[9]);
            }
            catch (...)
            {
                newBed->vitalSigns.painScore = 0.0;
            }
            try
            {
                newBed->vitalSigns.waistCircumference = std::stod(fields[10]);
            }
            catch (...)
            {
                newBed->vitalSigns.waistCircumference = 0.0;
            }
            try
            {
                newBed->vitalSigns.bloodSugar = std::stod(fields[11]);
            }
            catch (...)
            {
                newBed->vitalSigns.bloodSugar = 0.0;
            }
            try
            {
                newBed->vitalSigns.bodyFat = std::stod(fields[12]);
            }
            catch (...)
            {
                newBed->vitalSigns.bodyFat = 0.0;
            }
            try
            {
                newBed->vitalSigns.uricAcid = std::stod(fields[13]);
            }
            catch (...)
            {
                newBed->vitalSigns.uricAcid = 0.0;
            }
            try
            {
                newBed->vitalSigns.cholesterol = std::stod(fields[14]);
            }
            catch (...)
            {
                newBed->vitalSigns.cholesterol = 0.0;
            }
        }

        // 链表头插
        newBed->next = bedHead;
        if (bedHead)
            bedHead->prev = newBed;
        bedHead = newBed;
        lastBed = newBed;
    }

    inFile.close();
    return bedHead;
}

MedicationRecord *loadMedicationRecords(int &count)
{
    MedicationRecord *head = nullptr;
    MedicationRecord *last = nullptr;
    std::ifstream inFile(MEDICATION_RECORD_FILE);
    if (!inFile)
    {
        std::cerr << "无法打开用药记录文件！" << std::endl;
        count = 0;
        return nullptr;
    }

    std::string line;
    count = 0;
    MedicationRecord *current = nullptr;
    while (std::getline(inFile, line))
    {
        if (line.rfind("count:", 0) == 0)
        {
            try
            {
                count = std::stoi(line.substr(6));
            }
            catch (...)
            {
                count = 0;
            }
            break;
        }
        if (line.empty())
            continue;

        if (line.rfind("MEDICATION_LINE:", 0) == 0)
        {
            if (current)
            {
                std::string detail = line.substr(16);
                std::istringstream iss(detail);
                MedicationLine medLine;
                std::string quantityStr, unitPriceStr;
                std::getline(iss, medLine.medicineID, ',');
                std::getline(iss, medLine.medicineName, ',');
                std::getline(iss, quantityStr, ',');
                std::getline(iss, unitPriceStr, ',');
                std::getline(iss, medLine.note);

                try
                {
                    medLine.quantity = std::stoi(quantityStr);
                }
                catch (...)
                {
                    medLine.quantity = 0;
                }
                try
                {
                    medLine.unitPrice = std::stod(unitPriceStr);
                }
                catch (...)
                {
                    medLine.unitPrice = 0.0;
                }

                current->lines.push_back(medLine);
            }
            continue;
        }

        // 主记录
        std::istringstream iss(line);
        MedicationRecord *newRec = new MedicationRecord();
        std::string totalCostStr, reviewStatusStr, statusStr, isDeletedStr;
        std::getline(iss, newRec->medRecordID, ',');
        std::getline(iss, newRec->consultationID, ',');
        std::getline(iss, newRec->doctorID, ',');
        std::getline(iss, newRec->pharmacistID, ',');
        std::getline(iss, newRec->patientID, ',');
        std::getline(iss, newRec->department, ',');
        std::getline(iss, newRec->createTime, ',');
        std::getline(iss, totalCostStr, ',');
        std::getline(iss, reviewStatusStr, ',');
        std::getline(iss, statusStr, ',');
        std::getline(iss, newRec->paymentTime, ',');
        std::getline(iss, newRec->dispenseTime, ',');
        std::getline(iss, newRec->note, ',');
        std::getline(iss, isDeletedStr);

        try
        {
            newRec->totalCost = std::stod(totalCostStr);
        }
        catch (...)
        {
            newRec->totalCost = 0.0;
        }
        try
        {
            newRec->reviewStatus = static_cast<MedicationReviewStatus>(std::stoi(reviewStatusStr));
        }
        catch (...)
        {
            newRec->reviewStatus = MedicationReviewStatus::PENDING_REVIEW;
        }
        try
        {
            newRec->status = static_cast<MedicationStatus>(std::stoi(statusStr));
        }
        catch (...)
        {
            newRec->status = MedicationStatus::UNPAID;
        }
        newRec->isDeleted = (isDeletedStr == "1");

        // 链表插入
        newRec->prev = nullptr;
        newRec->next = head;
        if (head)
            head->prev = newRec;
        head = newRec;
        current = newRec;
    }
    inFile.close();
    return head;
}

Medicine *loadMedicines(int &count)
{
    Medicine *medHead = nullptr;
    std::ifstream inFile(MEDICINE_FILE);
    if (!inFile)
    {
        std::cerr << "无法打开药品信息文件！" << std::endl;
        count = 0;
        return nullptr;
    }

    std::string line;
    count = 0;
    while (std::getline(inFile, line))
    {
        if (line.rfind("count:", 0) == 0)
        {
            try
            {
                count = std::stoi(line.substr(6));
            }
            catch (...)
            {
                count = 0;
            }
            break;
        }
        if (line.empty())
            continue;

        std::istringstream iss(line);
        Medicine *newMed = new Medicine();
        std::string purchasePriceStr, salePriceStr, stockStr, safetyStockStr, isSpecialStr, isDeletedStr, noteStr, statusStr;

        std::getline(iss, newMed->medicineID, ',');
        std::getline(iss, newMed->name, ',');
        std::getline(iss, newMed->specification, ',');
        std::getline(iss, newMed->manufacturer, ',');
        std::getline(iss, purchasePriceStr, ',');
        std::getline(iss, salePriceStr, ',');
        std::getline(iss, stockStr, ',');
        std::getline(iss, safetyStockStr, ',');
        std::getline(iss, newMed->productionDate, ',');
        std::getline(iss, newMed->expiryDate, ',');
        std::getline(iss, newMed->department, ',');
        std::getline(iss, isSpecialStr, ',');
        std::getline(iss, isDeletedStr, ',');
        std::getline(iss, noteStr, ',');
        std::getline(iss, statusStr);

        try
        {
            newMed->purchasePrice = std::stod(purchasePriceStr);
        }
        catch (...)
        {
            newMed->purchasePrice = 0.0;
        }
        try
        {
            newMed->salePrice = std::stod(salePriceStr);
        }
        catch (...)
        {
            newMed->salePrice = 0.0;
        }
        try
        {
            newMed->stock = std::stoi(stockStr);
        }
        catch (...)
        {
            newMed->stock = 0;
        }
        try
        {
            newMed->safetyStock = std::stoi(safetyStockStr);
        }
        catch (...)
        {
            newMed->safetyStock = 0;
        }
        newMed->isSpecial = (isSpecialStr == "1");
        newMed->isDeleted = (isDeletedStr == "1");
        newMed->note = (noteStr == "无备注" ? "" : noteStr);
        try
        {
            newMed->status = static_cast<MedicineStatus>(std::stoi(statusStr));
        }
        catch (...)
        {
            newMed->status = MedicineStatus::NORMAL;
        }

        // 链表头插
        newMed->prev = nullptr;
        newMed->next = medHead;
        if (medHead)
            medHead->prev = newMed;
        medHead = newMed;
    }
    inFile.close();
    return medHead;
}