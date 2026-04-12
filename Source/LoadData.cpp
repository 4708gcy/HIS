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
        std::string isActiveStr, isDeletedStr, totalRevenueStr, totalExpensesStr, netProfitStr;
        std::string userID, username, storedHash, salt, createTime;

        std::getline(iss, userID, ',');
        std::getline(iss, username, ',');
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
        newAdmin->setStoredHash(storedHash);
        newAdmin->setSalt(salt);
        newAdmin->setIsAccountActive(isActiveStr == "1");
        newAdmin->setCreateTime(createTime);
        newAdmin->getTotalRevenue() = std::stod(totalRevenueStr);
        newAdmin->getTotalExpenses() = std::stod(totalExpensesStr);
        newAdmin->getNetProfit() = std::stod(netProfitStr);
        newAdmin->setIsDeleted(isDeletedStr == "1");
        newAdmin->setRole(UserRole::ADMIN);

        // 插入链表
        newAdmin->next = adminHead;
        adminHead = newAdmin;
    }
    inFile.close();
    return adminHead;
}

Doctor *loadDoctorData(int &count)
{
    return nullptr;
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
        std::string isPrecriptionReviewedStr, isHospitalizationRecommendedStr, statusStr, isDeletedStr;
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
        std::getline(iss, isPrecriptionReviewedStr, ',');
        std::getline(iss, isHospitalizationRecommendedStr, ',');
        std::getline(iss, statusStr, ',');
        std::getline(iss, newCon->note, ',');
        std::getline(iss, isDeletedStr);

        newCon->isPrecriptionReviewed = (isPrecriptionReviewedStr == "1");
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
                if (fields.size() > 0 && !fields[0].empty())
                    lastExam->vitalSigns.temperatureC = std::stod(fields[0]);
                if (fields.size() > 1 && !fields[1].empty())
                    lastExam->vitalSigns.systolicBP = std::stoi(fields[1]);
                if (fields.size() > 2 && !fields[2].empty())
                    lastExam->vitalSigns.diastolicBP = std::stoi(fields[2]);
                if (fields.size() > 3 && !fields[3].empty())
                    lastExam->vitalSigns.heartRate = std::stoi(fields[3]);
                if (fields.size() > 4 && !fields[4].empty())
                    lastExam->vitalSigns.respiratoryRate = std::stod(fields[4]);
                if (fields.size() > 5 && !fields[5].empty())
                    lastExam->vitalSigns.spo2 = std::stoi(fields[5]);
                if (fields.size() > 6 && !fields[6].empty())
                    lastExam->vitalSigns.height = std::stod(fields[6]);
                if (fields.size() > 7 && !fields[7].empty())
                    lastExam->vitalSigns.weight = std::stod(fields[7]);
                if (fields.size() > 8 && !fields[8].empty())
                    lastExam->vitalSigns.bmi = std::stod(fields[8]);
                if (fields.size() > 9 && !fields[9].empty())
                    lastExam->vitalSigns.painScore = std::stoi(fields[9]);
                if (fields.size() > 10 && !fields[10].empty())
                    lastExam->vitalSigns.waistCircumference = std::stod(fields[10]);
                if (fields.size() > 11 && !fields[11].empty())
                    lastExam->vitalSigns.bloodSugar = std::stod(fields[11]);
                if (fields.size() > 12 && !fields[12].empty())
                    lastExam->vitalSigns.bodyFat = std::stod(fields[12]);
                if (fields.size() > 13 && !fields[13].empty())
                    lastExam->vitalSigns.uricAcid = std::stod(fields[13]);
                if (fields.size() > 14 && !fields[14].empty())
                    lastExam->vitalSigns.cholesterol = std::stod(fields[14]);
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
        bedInfo *newBed = new bedInfo();
        std::string statusStr, areaStr, wardStr, bedNumStr, patientIDStr, nurseIDStr;

        std::getline(iss, newBed->bedID, ',');
        std::getline(iss, statusStr, ',');
        std::getline(iss, newBed->wardType, ',');
        std::getline(iss, newBed->department, ',');
        std::getline(iss, areaStr, ',');
        std::getline(iss, wardStr, ',');
        std::getline(iss, bedNumStr, ',');
        std::getline(iss, newBed->note, ',');
        std::getline(iss, patientIDStr, ',');
        std::getline(iss, nurseIDStr);

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
        try
        {
            newBed->patientID = std::stoi(patientIDStr);
        }
        catch (...)
        {
            newBed->patientID = 0;
        }
        try
        {
            newBed->nurseID = std::stoi(nurseIDStr);
        }
        catch (...)
        {
            newBed->nurseID = 0;
        }

        // 在 while 读取每个床位主数据后，继续读取下一行
        if (std::getline(inFile, line) && line.rfind("VITAL_SIGNS:", 0) == 0)
        {
            std::string vsStr = line.substr(12);
            std::istringstream iss(vsStr);
            std::string field;
            std::vector<std::string> fields;
            while (std::getline(iss, field, ';'))
                fields.push_back(field);

            if (fields.size() > 0 && !fields[0].empty())
                newBed->vitalSigns.temperatureC = std::stod(fields[0]);
            if (fields.size() > 1 && !fields[1].empty())
                newBed->vitalSigns.systolicBP = std::stoi(fields[1]);
            if (fields.size() > 2 && !fields[2].empty())
                newBed->vitalSigns.diastolicBP = std::stoi(fields[2]);
            if (fields.size() > 3 && !fields[3].empty())
                newBed->vitalSigns.heartRate = std::stoi(fields[3]);
            if (fields.size() > 4 && !fields[4].empty())
                newBed->vitalSigns.respiratoryRate = std::stod(fields[4]);
            if (fields.size() > 5 && !fields[5].empty())
                newBed->vitalSigns.spo2 = std::stoi(fields[5]);
            if (fields.size() > 6 && !fields[6].empty())
                newBed->vitalSigns.height = std::stod(fields[6]);
            if (fields.size() > 7 && !fields[7].empty())
                newBed->vitalSigns.weight = std::stod(fields[7]);
            if (fields.size() > 8 && !fields[8].empty())
                newBed->vitalSigns.bmi = std::stod(fields[8]);
            if (fields.size() > 9 && !fields[9].empty())
                newBed->vitalSigns.painScore = std::stoi(fields[9]);
            if (fields.size() > 10 && !fields[10].empty())
                newBed->vitalSigns.waistCircumference = std::stod(fields[10]);
            if (fields.size() > 11 && !fields[11].empty())
                newBed->vitalSigns.bloodSugar = std::stod(fields[11]);
            if (fields.size() > 12 && !fields[12].empty())
                newBed->vitalSigns.bodyFat = std::stod(fields[12]);
            if (fields.size() > 13 && !fields[13].empty())
                newBed->vitalSigns.uricAcid = std::stod(fields[13]);
            if (fields.size() > 14 && !fields[14].empty())
                newBed->vitalSigns.cholesterol = std::stod(fields[14]);
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

    return nullptr;
}

Medicine *loadMedicines(int &count)
{

    return nullptr;
}