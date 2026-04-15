#include "../Head/SaveData.h"

// =====================人物数据保存函数=====================
void saveAdminData(Admin *adminHead, int count)
{
    std::ofstream outFile(ADMIN_FILE);
    if (!outFile)
    {
        std::cerr << "无法打开管理员数据文件进行保存！" << std::endl;
        return;
    }

    Admin *current = adminHead;
    while (current != nullptr)
    {
        outFile << current->getUserID() << ","
                << current->getUsername() << ","
                << current->getGender() << ","
                << current->getAge() << ","
                << current->getTelephone() << ","
                << current->getEmail() << ","
                << current->getStoredHash() << ","
                << current->getSalt() << ","
                << (current->getIsAccountActive() ? "1" : "0") << ","
                << current->getCreateTime() << ","
                << current->getTotalRevenue() << ","
                << current->getTotalExpenses() << ","
                << current->getNetProfit() << ","
                << (current->getIsDeleted() ? "1" : "0") << ","
                << std::endl;

        current = current->next;
    }

    outFile << "count:" << count << std::endl; // 保存记录总数，便于加载时分配内存
    std::cout << "管理员数据保存成功！" << std::endl;

    outFile.close();
}

void saveDoctorData(Doctor *doctorHead, int count)
{
    std::ofstream outFile(DOCTOR_FILE);
    if (!outFile)
    {
        std::cerr << "无法打开医生数据文件进行保存！" << std::endl;
        return;
    }

    Doctor *current = doctorHead;
    while (current != nullptr)
    {
        outFile << current->getUserID() << ","                        // 基类ID
                << current->getUsername() << ","                      // 姓名
                << current->getGender() << ","                        // 性别
                << current->getAge() << ","                           // 年龄
                << current->getTelephone() << ","                     // 电话
                << current->getEmail() << ","                         // 邮箱
                << current->getStoredHash() << ","                    // 密码哈希
                << current->getSalt() << ","                          // 盐值
                << (current->getIsAccountActive() ? "1" : "0") << "," // 激活
                << current->getCreateTime() << ","                    // 创建时间
                << current->doctorID << ","                           // 医生ID
                << current->department << ","                         // 科室
                << static_cast<int>(current->title) << ","            // 职称（枚举转int）
                << current->specialty << ","                          // 擅长方向
                << current->scheduleInfo << ","                       // 排班
                << current->consultationCount << ","                  // 接诊数
                << current->examinationCount << ","                   // 检查数
                << current->hospitalizationApplyCount << ","          // 住院申请数
                << (current->isOnDuty ? "1" : "0") << ","             // 在岗
                << (current->isDeleted ? "1" : "0")                   // 逻辑删除
                << std::endl;

        current = current->next;
    }

    outFile << "count:" << count << std::endl;
    std::cout << "医生数据保存成功！" << std::endl;
    outFile.close();
}

// =====================医疗记录数据保存函数=====================

void saveRegistrations(Registration *regHead, int count)
{
    std::ofstream outFile(REGISTRATION_FILE);
    if (!outFile)
    {
        std::cerr << "无法打开挂号记录文件进行保存！" << std::endl;
        return;
    }

    Registration *current = regHead;
    while (current != nullptr)
    {
        outFile << current->registrationID << ","
                << current->patientID << ","
                << current->department << ","
                << current->doctorID << ","
                << current->registerTime << ","
                << current->fee << ","
                << static_cast<int>(current->status) << "," // 枚举转int
                << (current->note.empty() ? "无备注" : current->note) << ","
                << (current->isDeleted ? "1" : "0")
                << std::endl;

        // 输出相关挂号记录ID列表
        for (const auto &relatedID : current->relatedRegistrationIDs)
        {
            outFile << "RELATED_REGISTRATION_ID:" << relatedID << std::endl;
        }

        current = current->next;
    }

    outFile << "count:" << count << std::endl; // 保存记录总数，便于加载时分配内存

    outFile.close();
}

void saveConsultations(Consultation *conHead, int count)
{
    std::ofstream outFile(CONSULTATION_FILE);
    if (!outFile)
    {
        std::cerr << "无法打开看诊记录文件进行保存！" << std::endl;
        return;
    }

    Consultation *current = conHead;
    while (current != nullptr)
    {
        outFile << current->consultationID << ","
                << current->registrationID << ","
                << current->patientID << ","
                << current->doctorID << ","
                << current->consultationTime << ","
                << current->department << ","
                << current->chiefComplaint << ","
                << current->historyOfPresentIllness << ","
                << current->pastMedicalHistory << ","
                << current->familyHistory << ","
                << current->preliminaryDiagnosis << ","
                // examinationlist、prescriptions、attachments、relatedConsultationIDs 建议用分号拼接
                << (current->isPrecriptionReviewed ? "1" : "0") << ","
                << (current->isHospitalizationRecommended ? "1" : "0") << ","
                << static_cast<int>(current->status) << ","
                << (current->note.empty() ? "无备注" : current->note) << ","
                << (current->isDeleted ? "1" : "0") << std::endl;

        // 保存处方列表
        for (const auto &pres : current->prescriptions)
        {
            outFile << "PRESCRIPTION:" << pres.medicineID << ","
                    << pres.name << ","
                    << pres.dosage << ","
                    << pres.frequency << ","
                    << pres.duration << ","
                    << (pres.note.empty() ? "无备注" : pres.note) << std::endl;
        }

        // 保存检查项目列表
        for (const auto &exam : current->examinationlist)
        {
            outFile << "EXAMINATION_ITEM:" << exam << std::endl;
        }

        // 保存附件列表
        for (const auto &attach : current->attachments)
        {
            outFile << "ATTACHMENT:" << attach << std::endl;
        }

        // 保存相关看诊记录ID列表
        for (const auto &relatedID : current->relatedConsultationIDs)
        {
            outFile << "RELATED_CONSULTATION_ID:" << relatedID << std::endl;
        }

        current = current->next;
    }

    outFile << "count:" << count << std::endl; // 保存记录总数，便于加载时分配内存

    outFile.close();
}

void saveExaminations(Examination *examHead, int count)
{
    std::ofstream outFile(EXAMINATION_FILE);
    if (!outFile)
    {
        std::cerr << "无法打开检查记录文件进行保存！" << std::endl;
        return;
    }

    Examination *current = examHead;
    while (current != nullptr)
    {
        outFile << current->examinationID << ","
                << current->consultationID << ","
                << current->patientID << ","
                << current->doctorID << ","
                << current->department << ","
                << current->itemName << ","
                << current->orderTime << ","
                << current->reportTime << ","
                << current->reportSummary << ","
                << current->fee << ","
                << static_cast<int>(current->status) << ","
                << (current->isDeleted ? "1" : "0") << std::endl;

        // 保存生命体征
        const VitalSigns &vs = current->vitalSigns;
        outFile << "VITAL_SIGNS:"
                << std::to_string(vs.temperatureC) << ";"
                << std::to_string(vs.systolicBP) << ";"
                << std::to_string(vs.diastolicBP) << ";"
                << std::to_string(vs.heartRate) << ";"
                << std::to_string(vs.respiratoryRate) << ";"
                << std::to_string(vs.spo2) << ";"
                << std::to_string(vs.height) << ";"
                << std::to_string(vs.weight) << ";"
                << std::to_string(vs.bmi) << ";"
                << std::to_string(vs.painScore) << ";"
                << std::to_string(vs.waistCircumference) << ";"
                << std::to_string(vs.bloodSugar) << ";"
                << std::to_string(vs.bodyFat) << ";"
                << std::to_string(vs.uricAcid) << ";"
                << std::to_string(vs.cholesterol)
                << std::endl;

        // 保存附件
        for (const auto &attach : current->attachments)
            outFile << "ATTACHMENT:" << attach << std::endl;

        // 保存相关检查记录ID
        for (const auto &relatedID : current->relatedExaminationIDs)
            outFile << "RELATED_EXAMINATION_ID:" << relatedID << std::endl;

        current = current->next;
    }

    outFile << "count:" << count << std::endl;
    outFile.close();
}

void saveHospitalizations(Hospitalization *hosHead, int count)
{
    std::ofstream outFile(HOSPITALIZATION_FILE);
    if (!outFile)
    {
        std::cerr << "无法打开住院记录文件进行保存！" << std::endl;
        return;
    }

    Hospitalization *current = hosHead;
    while (current != nullptr)
    {
        outFile << current->hospitalizationID << ","
                << current->consultationID << ","
                << current->patientID << ","
                << current->doctorID << ","
                << current->nurseID << ","
                << current->department << ","
                << current->wardType << ","
                << current->bedNumber << ","
                << current->applyTime << ","
                << current->admitTime << ","
                << current->dischargeTime << ","
                << current->availableAdmitTime << ","
                << current->deposit << ","
                << current->totalCost << ","
                << static_cast<int>(current->status) << ","
                << (current->isDeleted ? "1" : "0") << std::endl;

        // 保存相关住院记录ID列表
        for (const auto &relatedID : current->relatedHospitalizationIDs)
            outFile << "RELATED_HOSPITALIZATION_ID:" << relatedID << std::endl;

        current = current->next;
    }

    outFile << "count:" << count << std::endl; // 保存记录总数，便于加载时分配内存
    outFile.close();
}

void saveBedInfos(bedInfo *bedHead, int count)
{
    std::ofstream outFile(BED_FILE);
    if (!outFile)
    {
        std::cerr << "无法打开床位信息文件进行保存！" << std::endl;
        return;
    }

    bedInfo *current = bedHead;
    while (current != nullptr)
    {
        outFile << current->bedID << ","
                << static_cast<int>(current->status) << ","
                << current->wardType << ","
                << current->department << ","
                << current->areaNumber << ","
                << current->wardNumber << ","
                << current->bedNumber << ","
                << current->note << ","
                << current->patientID << ","
                << current->nurseID << ","
                << (current->isDeleted ? "1" : "0") << ","
                << current->useTimes << ","
                << current->daysOccupied
                << std::endl;

        // 保存体征信息
        const VitalSigns &vs = current->vitalSigns;
        outFile << "VITAL_SIGNS:"
                << std::to_string(vs.temperatureC) << ";"
                << std::to_string(vs.systolicBP) << ";"
                << std::to_string(vs.diastolicBP) << ";"
                << std::to_string(vs.heartRate) << ";"
                << std::to_string(vs.respiratoryRate) << ";"
                << std::to_string(vs.spo2) << ";"
                << std::to_string(vs.height) << ";"
                << std::to_string(vs.weight) << ";"
                << std::to_string(vs.bmi) << ";"
                << std::to_string(vs.painScore) << ";"
                << std::to_string(vs.waistCircumference) << ";"
                << std::to_string(vs.bloodSugar) << ";"
                << std::to_string(vs.bodyFat) << ";"
                << std::to_string(vs.uricAcid) << ";"
                << std::to_string(vs.cholesterol)
                << std::endl;

        current = current->next;
    }
    outFile << "count:" << count << std::endl;
    outFile.close();
}

void saveMedicationRecords(MedicationRecord *medRecHead, int count)
{
    std::ofstream outFile(MEDICATION_RECORD_FILE);
    if (!outFile)
    {
        std::cerr << "无法打开用药记录文件进行保存！" << std::endl;
        return;
    }

    MedicationRecord *current = medRecHead;
    while (current != nullptr)
    {
        outFile << current->medRecordID << ","
                << current->consultationID << ","
                << current->doctorID << ","
                << current->pharmacistID << ","
                << current->patientID << ","
                << current->department << ","
                << current->createTime << ","
                << current->totalCost << ","
                << static_cast<int>(current->reviewStatus) << ","
                << static_cast<int>(current->status) << ","
                << current->paymentTime << ","
                << current->dispenseTime << ","
                << (current->note.empty() ? "无备注" : current->note) << ","
                << (current->isDeleted ? "1" : "0")
                << std::endl; // 注意：最后一项后不加逗号

        // 保存用药明细
        for (const auto &line : current->lines)
        {
            outFile << "MEDICATION_LINE:"
                    << line.medicineID << ","
                    << line.medicineName << ","
                    << line.quantity << ","
                    << line.unitPrice << ","
                    << (line.note.empty() ? "无备注" : line.note)
                    << std::endl;
        }
        current = current->next;
    }

    outFile << "count:" << count << std::endl;
    outFile.close();
}

void saveMedicines(Medicine *medHead, int count)
{
    std::ofstream outFile(MEDICINE_FILE);
    if (!outFile)
    {
        std::cerr << "无法打开药品信息文件进行保存！" << std::endl;
        return;
    }

    Medicine *current = medHead;
    while (current != nullptr)
    {
        outFile << current->medicineID << ","
                << current->name << ","
                << current->specification << ","
                << current->manufacturer << ","
                << current->purchasePrice << ","
                << current->salePrice << ","
                << current->stock << ","
                << current->safetyStock << ","
                << current->productionDate << ","
                << current->expiryDate << ","
                << current->department << ","
                << (current->isSpecial ? "1" : "0") << ","
                << (current->isDeleted ? "1" : "0") << ","
                << (current->note.empty() ? "无备注" : current->note) << ","
                << static_cast<int>(current->status)
                << std::endl;
        current = current->next;
    }

    outFile << "count:" << count << std::endl;
    outFile.close();
}