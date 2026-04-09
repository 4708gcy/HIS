#include "../Head/LoadData.h"


// =====================人物数据加载函数=====================
Admin *loadAdminData()
{
    Admin *adminHead = nullptr;
    std::ifstream inFile(ADMIN_FILE);
    if (!inFile)
    {
        std::cerr << "无法打开管理员数据文件！" << std::endl;
        return nullptr;
    }

    std::string line;
    while (std::getline(inFile, line))
    {
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

// =====================医疗记录数据加载函数=====================
Registration *loadRegistrations(){

    return nullptr;
}

Consultation *loadConsultations(){

    return nullptr;
}

Examination *loadExaminations(){

    return nullptr;
}

Hospitalization *loadHospitalizations(){

    return nullptr;
}

MedicationRecord *loadMedicationRecords(){

    return nullptr;
}

Medicine *loadMedicines(){

    return nullptr;
}