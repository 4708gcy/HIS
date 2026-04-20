import re

patient_cpp = "D:/vscode编程练习c/C课设-HIS/Source/Patient.cpp"

with open(patient_cpp, 'r', encoding='utf-8') as f:
    content = f.read()

impls = """
// ---------------- 住院信息管理功能实现 ----------------

static void printHospitalizationRecord(Hospitalization *current, User *u)
{
    std::cout << "========================================" << std::endl;
    std::cout << "住院记录ID: " << current->hospitalizationID << std::endl
              << "关联看诊ID: " << current->consultationID << std::endl
              << "申请医生ID: " << current->doctorID << std::endl
              << "负责护士ID: " << current->nurseID << std::endl
              << "所属科室: " << current->department << std::endl
              << "病房类型: " << current->wardType << std::endl
              << "床位号: " << current->bedNumber << std::endl
              << "申请时间: " << current->applyTime << std::endl
              << "可入住时间: " << current->availableAdmitTime << std::endl
              << "实际入院时间: " << current->admitTime << std::endl
              << "出院时间: " << current->dischargeTime << std::endl
              << "押金: " << current->deposit << " 元" << std::endl
              << "总费用: " << current->totalCost << " 元" << std::endl
              << "当前状态: " << u->hosStatusToString(current->status) << std::endl;
    std::cout << "========================================" << std::endl;
}

bool Patient::getAllHospitalizations(Hospitalization *&hosHead)
{
    std::cout << "正在获取您的所有住院信息..." << std::endl;
    Hospitalization *current = hosHead;
    bool found = false;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->patientID == this->patientID)
        {
            printHospitalizationRecord(current, this);
            found = true;
        }
        current = current->next;
    }
    if (!found)
        std::cout << "没找到任何住院记录。" << std::endl;
    return found;
}

bool Patient::getHospitalizationByID(Hospitalization *&hosHead)
{
    std::string hosID = inputRecordIDCheck("请输入要查询的住院记录ID: ", {"hos"});
    Hospitalization *current = hosHead;
    bool found = false;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->patientID == this->patientID && current->hospitalizationID == hosID)
        {
            printHospitalizationRecord(current, this);
            found = true;
            break;
        }
        current = current->next;
    }
    if (!found)
        std::cout << "没找到指定的住院记录。" << std::endl;
    return found;
}

bool Patient::getHospitalizationsByAdmitTimeRange(Hospitalization *&hosHead)
{
    std::string startTime, endTime;
    std::cout << "请输入查询的入院开始时间 (如 2026-04-10): ";
    std::cin >> startTime;
    std::cout << "请输入查询的入院结束时间 (如 2026-04-20): ";
    std::cin >> endTime;
    Hospitalization *current = hosHead;
    bool found = false;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->patientID == this->patientID && current->admitTime >= startTime && current->admitTime <= endTime && current->admitTime != "#")
        {
            printHospitalizationRecord(current, this);
            found = true;
        }
        current = current->next;
    }
    if (!found)
        std::cout << "没找到对应入院时间范围的记录。" << std::endl;
    return found;
}

bool Patient::getHospitalizationsByDischargeTimeRange(Hospitalization *&hosHead)
{
    std::string startTime, endTime;
    std::cout << "请输入查询的出院开始时间 (如 2026-04-10): ";
    std::cin >> startTime;
    std::cout << "请输入查询的出院结束时间 (如 2026-04-20): ";
    std::cin >> endTime;
    Hospitalization *current = hosHead;
    bool found = false;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->patientID == this->patientID && current->dischargeTime >= startTime && current->dischargeTime <= endTime && current->dischargeTime != "#")
        {
            printHospitalizationRecord(current, this);
            found = true;
        }
        current = current->next;
    }
    if (!found)
        std::cout << "没找到对应出院时间范围的记录。" << std::endl;
    return found;
}

bool Patient::getHospitalizationsByStatus(Hospitalization *&hosHead, int select)
{
    int sChoice;
    if (select == -1)
    {
        std::cout << "请选择要查询的住院状态: " << std::endl;
        std::cout << "1. 申请中" << std::endl;
        std::cout << "2. 已缴费" << std::endl;
        std::cout << "3. 已入院" << std::endl;
        std::cout << "4. 已出院" << std::endl;
        std::cout << "5. 已作废" << std::endl;
        sChoice = selectIntCheck(1, 5) - 1;
    }
    else
    {
        sChoice = select;
    }

    Hospitalization *current = hosHead;
    bool found = false;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->patientID == this->patientID && (int)current->status == sChoice)
        {
            printHospitalizationRecord(current, this);
            found = true;
        }
        current = current->next;
    }
    if (!found)
        std::cout << "没有找到对应状态的住院记录。" << std::endl;
    return found;
}

bool Patient::getHospitalizationsByDepartment(Hospitalization *&hosHead)
{
    std::string dept;
    std::cout << "请输入查询的住院科室名称: ";
    std::cin >> dept;
    Hospitalization *current = hosHead;
    bool found = false;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->patientID == this->patientID && current->department.find(dept) != std::string::npos)
        {
            printHospitalizationRecord(current, this);
            found = true;
        }
        current = current->next;
    }
    if (!found)
        std::cout << "没找到对应科室的住院记录。" << std::endl;
    return found;
}

bool Patient::getHospitalizationsByWardType(Hospitalization *&hosHead)
{
    std::string type = HospitalizationWardTypeMenu();
    if(type == "0") return false;
    Hospitalization *current = hosHead;
    bool found = false;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->patientID == this->patientID && current->wardType == type)
        {
            printHospitalizationRecord(current, this);
            found = true;
        }
        current = current->next;
    }
    if (!found)
        std::cout << "没找到对应病房类型的住院记录。" << std::endl;
    return found;
}

bool Patient::getHospitalizationsByBedNumber(Hospitalization *&hosHead)
{
    std::string dept;
    std::cout << "请输入科室名称: ";
    std::cin >> dept;
    std::string wType = HospitalizationWardTypeMenu();
    if(wType == "0") return false;
    std::string bedID = inputBedNumberCheck("请输入要查询的床位号信息: ", dept, wType);
    if(bedID == "") return false;

    Hospitalization *current = hosHead;
    bool found = false;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->patientID == this->patientID && current->bedNumber == bedID)
        {
            printHospitalizationRecord(current, this);
            found = true;
        }
        current = current->next;
    }
    if (!found)
        std::cout << "没找到对应床位号的住院记录。" << std::endl;
    return found;
}

bool Patient::getHospitalizationsByDoctorID(Hospitalization *&hosHead)
{
    std::string docID = inputIDCheck("请输入申请医生的ID: ");
    Hospitalization *current = hosHead;
    bool found = false;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->patientID == this->patientID && current->doctorID == docID)
        {
            printHospitalizationRecord(current, this);
            found = true;
        }
        current = current->next;
    }
    if (!found)
        std::cout << "没找到对应医生的住院记录。" << std::endl;
    return found;
}

bool Patient::getHospitalizationsByConsultationID(Hospitalization *&hosHead)
{
    std::string conID = inputRecordIDCheck("请输入看诊记录ID: ", {"con"});
    Hospitalization *current = hosHead;
    bool found = false;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->patientID == this->patientID && current->consultationID == conID)
        {
            printHospitalizationRecord(current, this);
            found = true;
        }
        current = current->next;
    }
    if (!found)
        std::cout << "没找到对应看诊记录的住院记录。" << std::endl;
    return found;
}

void Patient::applyForDischarge(Hospitalization *&hosHead, bedInfo *&bedHead)
{
    std::string hosID = inputRecordIDCheck("请输入您要申请出院的住院记录ID: ", {"hos"});
    Hospitalization *current = hosHead;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->patientID == this->patientID && current->hospitalizationID == hosID)
            break;
        current = current->next;
    }
    if (!current)
    {
        std::cout << "未找到该住院记录。" << std::endl;
        return;
    }
    if (current->status != HospitalizationStatus::ADMITTED)
    {
        std::cout << "当前记录状态不仅是[已入院]，无法申请出院。" << std::endl;
        return;
    }

    std::cout << "正在为您计算出院费用结算..." << std::endl;
    // 粗略按日期的前十位进行相减，如果不足1天按1天计算
    int days = 1;
    if (current->admitTime.length() >= 10)
    {
        int y1=0,m1=0,d1=0,y2=0,m2=0,d2=0;
        sscanf(current->admitTime.c_str(), "%d-%d-%d", &y1, &m1, &d1);
        std::string nowTime = getCurrentTime();
        sscanf(nowTime.c_str(), "%d-%d-%d", &y2, &m2, &d2);
        int day1 = y1*365 + m1*30 + d1;
        int day2 = y2*365 + m2*30 + d2;
        days = day2 - day1;
        if (days <= 0) days = 1;
    }
    double calculatedCost = calculateHospitalizationFee(current->wardType, days);
    current->totalCost = calculatedCost;

    std::cout << "根据您的入出院天数(" << days << "天)，以及病房类型(" << current->wardType << ")，" << std::endl;
    std::cout << "总费用为 " << calculatedCost << " 元。您已缴纳了押金 " << current->deposit << " 元。" << std::endl;

    double diff = calculatedCost - current->deposit;
    if (diff > 0)
    {
        std::cout << "您还需补交欠费 " << diff << " 元。您的账户余额为 " << this->balance << " 元。" << std::endl;
        if (this->balance < diff)
        {
            std::cout << "余额不足，无法为其办理出院，请先充值!" << std::endl;
            return;
        }
        std::cout << "是否确认扣款并办理出院? (1.确认 / 0.取消) : ";
        int confirm = selectIntCheck(0, 1);
        if(confirm == 1)
        {
            this->balance -= diff;
            std::cout << "扣款成功。新余额：" << this->balance << " 元。" << std::endl;
        }
        else
        {
            std::cout << "出院申请已取消。" << std::endl;
            return;
        }
    }
    else if (diff <= 0)
    {
        double refund = -diff;
        this->balance += refund;
        std::cout << "您的押金已足够支付，系统将为您退还余额 " << refund << " 元。" << std::endl;
        std::cout << "退款成功。新余额：" << this->balance << " 元。" << std::endl;
    }

    current->status = HospitalizationStatus::DISCHARGED;
    current->dischargeTime = getCurrentTime();
    std::cout << "已成功为您办理出院！状态已更改为 [出院]。" << std::endl;

    // 清理床位
    if (current->bedNumber != "#")
    {
        bedInfo *b = bedHead;
        while(b)
        {
            if (b->bedID == current->bedNumber && b->patientID == this->patientID)
            {
                b->status = bedStatus::ClEANING; // 设置为清洁中
                b->patientID = "#";
                std::cout << "相应床位已释放，交由护士进行清洁！" << std::endl;
                break;
            }
            b = b->next;
        }
    }
}

void Patient::payHospitalizationDeposit(Hospitalization *&hosHead)
{
    bool ishave = getHospitalizationsByStatus(hosHead, 0); // 0 对应 APPLIED
    if (!ishave)
    {
        std::cout << "如果想为已入院的记录补交款，可以先联系护士或出院时一并结算哦。" << std::endl;
        return;
    }

    std::string hosID = inputRecordIDCheck("请输入要缴纳押金信息的住院记录ID: ", {"hos"});
    Hospitalization *current = hosHead;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->patientID == this->patientID && current->hospitalizationID == hosID)
            break;
        current = current->next;
    }
    if (!current)
    {
        std::cout << "没有找到对应的记录。" << std::endl;
        return;
    }
    if (current->status != HospitalizationStatus::APPLIED)
    {
        std::cout << "该记录不是 [申请中] 状态，不能补交入账押金。" << std::endl;
        return;
    }

    double requiredDeposit = 1000.0; // 默认收取 1000 元作为基础押金
    if(current->wardType == "ICU病房" || current->wardType == "VIP病房")
    {
        requiredDeposit = 5000.0;
    }
    std::cout << "根据该记录("<< current->wardType <<")，您需要先缴纳床位押金: " << requiredDeposit << " 元。" << std::endl;
    std::cout << "您当前的余额为 " << this->balance << " 元。" << std::endl;

    if (this->balance < requiredDeposit)
    {
        std::cout << "余额不足！请先回到账户中心充值。" << std::endl;
        return;
    }

    std::cout << "是否确认缴纳? (1.确认 / 0.取消) : ";
    int confirm = selectIntCheck(0, 1);
    if(confirm == 1)
    {
        this->balance -= requiredDeposit;
        current->deposit += requiredDeposit;
        current->status = HospitalizationStatus::PAID; // 已缴费可以等护士分床了
        std::cout << "缴纳成功！余额 " << this->balance << " 元。待护士为您排床处理。" << std::endl;
    }
    else
    {
        std::cout << "已取消缴纳。" << std::endl;
    }
}

void Patient::manageHospitalizations(Hospitalization *&hosHead, bedInfo *&bedHead)
{
    while (true)
    {
        int choice = patientHospitalizationManagementMenu();
        if (choice == 0)
        {
            return;
        }
        else if (choice == 1)
        {
            int viewChoice = patientHospitalizationViewMenu();
            switch (viewChoice)
            {
            case 1: getAllHospitalizations(hosHead); break;
            case 2: getHospitalizationByID(hosHead); break;
            case 3: getHospitalizationsByAdmitTimeRange(hosHead); break;
            case 4: getHospitalizationsByDischargeTimeRange(hosHead); break;
            case 5: getHospitalizationsByStatus(hosHead); break;
            case 6: getHospitalizationsByDepartment(hosHead); break;
            case 7: getHospitalizationsByWardType(hosHead); break;
            case 8: getHospitalizationsByBedNumber(hosHead); break;
            case 9: getHospitalizationsByDoctorID(hosHead); break;
            case 10: getHospitalizationsByConsultationID(hosHead); break;
            case 0: break;
            }
        }
        else if (choice == 2)
        {
            applyForDischarge(hosHead, bedHead);
        }
        else if (choice == 3)
        {
            payHospitalizationDeposit(hosHead);
        }
    }
}
"""

if "manageHospitalizations" not in content:
    content += impls

with open(patient_cpp, 'w', encoding='utf-8') as f:
    f.write(content)

print("Patient.cpp updated.")
