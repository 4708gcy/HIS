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
        // 检查是否是count行
        if (line.rfind("count:", 0) == 0)
        {
            // 以count:开头
            std::string countStr = line.substr(6);
            try
            {
                count = std::stoi(countStr);
            }
            catch (...)
            {
                count = 0;
            }
            break; // 读到count就结束
        }

        std::istringstream iss(line);
        Admin *newAdmin = new Admin();

        std::string userID, username, storedHash, salt, isActiveStr, createTimeStr;
        std::getline(iss, userID, ',');
        std::getline(iss, username, ',');
        std::getline(iss, storedHash, ',');
        std::getline(iss, salt, ',');
        std::getline(iss, isActiveStr, ',');
        std::getline(iss, createTimeStr);

        newAdmin->setUserID(userID);
        newAdmin->setUsername(username);
        newAdmin->setStoredHash(storedHash);
        newAdmin->setSalt(salt);
        newAdmin->setIsAccountActive(isActiveStr == "1" || isActiveStr == "true");
        newAdmin->setCreateTime(createTimeStr);
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
    while (std::getline(inFile, line))
    {
        // 检查是否是count行
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

        std::istringstream iss(line);
        Registration *newReg = new Registration();

        std::string feeStr, statusStr, isDeletedStr;
        // 依次读取各字段
        std::getline(iss, newReg->registrationID, ',');
        std::getline(iss, newReg->patientID, ',');
        std::getline(iss, newReg->department, ',');
        std::getline(iss, newReg->doctorID, ',');
        std::getline(iss, newReg->registerTime, ',');
        std::getline(iss, feeStr, ',');
        std::getline(iss, statusStr, ',');
        std::getline(iss, newReg->note, ',');
        std::getline(iss, isDeletedStr);

        // 字符串转数值
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
            int statusInt = std::stoi(statusStr);
            newReg->status = static_cast<RegistrationStatus>(statusInt);
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
    }

    inFile.close();
    return regHead;
}

Consultation *loadConsultations(int &count)
{

    return nullptr;
}

Examination *loadExaminations(int &count)
{

    return nullptr;
}

Hospitalization *loadHospitalizations(int &count)
{

    return nullptr;
}

MedicationRecord *loadMedicationRecords(int &count)
{

    return nullptr;
}

Medicine *loadMedicines(int &count)
{

    return nullptr;
}