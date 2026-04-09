#include "../Head/SaveData.h"


// =====================人物数据保存函数=====================
void saveAdminData(Admin *adminHead)
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
                << (current->getIsAccountActive() ? "true" : "false") << ","
                << current->getCreateTime()
                << std::endl;

        current = current->next;
    }

    std::cout << "管理员数据保存成功！" << std::endl;

    outFile.close();
}


// =====================医疗记录数据保存函数=====================

void saveRegistrations(Registration *regHead)
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
                << current->note << ","
                << (current->isDeleted ? "1" : "0")
                << std::endl;
        current = current->next;
    }

    outFile.close();
}

void saveConsultations(Consultation *conHead)
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
                << current->chiefComplaint << ","
                << current->historyOfPresentIllness << ","
                << current->pastMedicalHistory << ","
                << current->familyHistory << ","
                << current->preliminaryDiagnosis << ","
                // examinationlist、prescriptions、attachments、relatedConsultationIDs 建议用分号拼接
                << static_cast<int>(current->status) << ","
                << current->note
                << std::endl;
        current = current->next;
    }

    outFile.close();
}

void saveExaminations(Examination *examHead)
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
                << current->itemName << ","
                << current->orderTime << ","
                << current->reportTime << ","
                << current->reportSummary << ","
                // vitalSigns 建议用分号拼接
                << current->fee << ","
                << (current->isPaid ? "1" : "0") << ","
                << static_cast<int>(current->status) << ","
                << current->note
                << std::endl;
        current = current->next;
    }

    outFile.close();
}

void saveHospitalizations(Hospitalization *hosHead)
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
                << current->patientID << ","
                << current->doctorID << ","
                << current->nurseID << ","
                << current->department << ","
                << current->wardType << ","
                << current->bedNo << ","
                << current->applyTime << ","
                << current->admitTime << ","
                << current->dischargeTime << ","
                << current->availableAdmitTime << ","
                << current->deposit << ","
                << current->totalCost << ","
                << (current->isDepositPaid ? "1" : "0") << ","
                << static_cast<int>(current->status)
                << std::endl;
        current = current->next;
    }

    outFile.close();
}