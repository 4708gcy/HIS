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
                << (vs.temperatureC ? std::to_string(*vs.temperatureC) : "") << ";"
                << (vs.systolicBP ? std::to_string(*vs.systolicBP) : "") << ";"
                << (vs.diastolicBP ? std::to_string(*vs.diastolicBP) : "") << ";"
                << (vs.heartRate ? std::to_string(*vs.heartRate) : "") << ";"
                << (vs.respiratoryRate ? std::to_string(*vs.respiratoryRate) : "") << ";"
                << (vs.spo2 ? std::to_string(*vs.spo2) : "") << ";"
                << (vs.height ? std::to_string(*vs.height) : "") << ";"
                << (vs.weight ? std::to_string(*vs.weight) : "") << ";"
                << (vs.bmi ? std::to_string(*vs.bmi) : "") << ";"
                << (vs.painScore ? std::to_string(*vs.painScore) : "") << ";"
                << (vs.waistCircumference ? std::to_string(*vs.waistCircumference) : "") << ";"
                << (vs.bloodSugar ? std::to_string(*vs.bloodSugar) : "") << ";"
                << (vs.bodyFat ? std::to_string(*vs.bodyFat) : "") << ";"
                << (vs.uricAcid ? std::to_string(*vs.uricAcid) : "") << ";"
                << (vs.cholesterol ? std::to_string(*vs.cholesterol) : "")
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

}