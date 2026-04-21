/**
 * @file Pharmacist.cpp
 * @brief 医疗管理系统中的药剂师类实现
 * @details 该源文件实现了 Pharmacist 类的构造、析构、注册登录、
 *          用药记录管理、药品信息管理及个人信息管理等功能。
 * @author 李阳旭 / 小组成员
 * @date 2026-4-15
 * @version 1.0
 */

#include "../Head/Pharmacist.h"

Pharmacist::Pharmacist()
{
}

Pharmacist::~Pharmacist()
{
}

bool Pharmacist::pharmacistSignUp(int &idCounter)
{
    bool success = signUp(4, idCounter); // 药剂师角色编号为 4
    if (!success)
    {
        std::cout << "药剂师注册失败！" << std::endl;
        return false;
    }

    this->pharmacistID = this->userID;
    this->department = inputDepartmentCheck("请输入所属药房/科室: ");

    std::cout << "请选择药剂师等级:" << std::endl;
    std::cout << "1. 实习药剂师" << std::endl;
    std::cout << "2. 初级药剂师" << std::endl;
    std::cout << "3. 高级药剂师" << std::endl;
    std::cout << "4. 主管药剂师" << std::endl;
    int levelChoice = selectIntCheck(1, 4);
    this->level = static_cast<PharmacistLevel>(levelChoice);

    this->scheduleInfo = inputStringCheck("请输入排班信息: ");

    std::cout << "请输入你现在是否在岗 (1-是, 0-否): " << std::endl;
    int onDutyChoice = selectIntCheck(0, 1);
    this->isOnDuty = (onDutyChoice == 1);

    std::cout << "药剂师注册成功! 您的用户ID是: " << this->userID << std::endl;
    return true;
}

bool Pharmacist::pharmacistSignIn()
{
    if (isAccountActive == false)
    {
        std::cout << "账户已锁定，请联系系统管理员解锁！" << std::endl;
        return false;
    }

    while (loginAttempts < kMaxLoginAttempts)
    {
        std::string pwd = inputStringCheck("请输入密码(输入\"quit\"退出登录): ");

        if (pwd == "quit")
        {
            std::cout << "退出登录。" << std::endl;
            return false;
        }

        bool success = SHA256Verify(pwd, storedHash, kHashIterations);

        if (success)
        {
            isLoggedIn = true;
            std::cout << "药剂师登录成功！" << std::endl;
            return true;
        }
        else
        {
            loginAttempts++;
            std::cout << "密码错误! 请重新输入密码(当前失败次数: " << loginAttempts << ")" << std::endl;

            if (loginAttempts >= kMaxLoginAttempts)
            {
                isAccountActive = false;
                std::cout << "连续登录失败次数过多，账户已锁定，请联系系统管理员解锁！" << std::endl;
            }
        }
    }

    return false;
}

// ==================== Getter ====================

const std::string &Pharmacist::getPharmacistID() const
{
    return pharmacistID;
}

const std::string &Pharmacist::getDepartment() const
{
    return department;
}

PharmacistLevel Pharmacist::getLevel() const
{
    return level;
}

const std::string &Pharmacist::getScheduleInfo() const
{
    return scheduleInfo;
}

int Pharmacist::getReviewCount() const
{
    return reviewCount;
}

int Pharmacist::getDispenseCount() const
{
    return dispenseCount;
}

int Pharmacist::getInventoryManageCount() const
{
    return inventoryManageCount;
}

bool Pharmacist::getIsOnDuty() const
{
    return isOnDuty;
}

// ==================== Setter ====================

void Pharmacist::setPharmacistID(const std::string &id)
{
    pharmacistID = id;
}

void Pharmacist::setDepartment(const std::string &dept)
{
    department = dept;
}

void Pharmacist::setLevel(PharmacistLevel lvl)
{
    level = lvl;
}

void Pharmacist::setScheduleInfo(const std::string &info)
{
    scheduleInfo = info;
}

void Pharmacist::setReviewCount(int count)
{
    reviewCount = count;
}

void Pharmacist::setDispenseCount(int count)
{
    dispenseCount = count;
}

void Pharmacist::setInventoryManageCount(int count)
{
    inventoryManageCount = count;
}

void Pharmacist::setIsOnDuty(bool onDuty)
{
    isOnDuty = onDuty;
}

// ==================== 统计辅助 ====================

void Pharmacist::increaseReviewCount()
{
    ++reviewCount;
}

void Pharmacist::increaseDispenseCount()
{
    ++dispenseCount;
}

void Pharmacist::increaseInventoryManageCount()
{
    ++inventoryManageCount;
}

// ==================== 用药记录管理 ====================
void Pharmacist::printMedicationRecord(MedicationRecord *current)
{
    std::string reviewStatusStr = medicationReviewStatusToString(current->reviewStatus);
    std::string statusStr = medicationStatusToString(current->status);
    std::cout << "用药记录ID: " << current->medRecordID
              << ", 关联看诊记录ID: " << current->consultationID
              << ", 患者ID: " << current->patientID
              << ", 医生ID: " << current->doctorID
              << ", 药师ID: " << (current->pharmacistID.empty() ? "无" : current->pharmacistID)
              << ", 科室: " << current->department
              << ", 审核状态: " << statusStr
              << ", 总费用: " << current->totalCost
              << ", 支付时间: " << (current->paymentTime.empty() ? "未支付" : current->paymentTime)
              << ", 发药时间: " << (current->dispenseTime.empty() ? "未发药" : current->dispenseTime)
              << ", 创建时间: " << current->createTime
              << ", 备注: " << (current->note.empty() ? "无" : current->note)
              << std::endl;

    std::cout << "用药明细: " << std::endl;
    for (const auto &line : current->lines)
    {
        std::cout << "  - 药品ID: " << line.medicineID
                  << ", 药品名称: " << line.medicineName
                  << ", 数量: " << line.quantity
                  << ", 单价: " << line.unitPrice
                  << ", 用法备注: " << (line.note.empty() ? "无" : line.note)
                  << std::endl;
    }
}

// 获取全部用药记录（仅限本部门且分配给自己的记录）
bool Pharmacist::getAllMedicationRecords(MedicationRecord *&medRecHead)
{
    std::cout << "正在获取全部用药记录..." << std::endl;

    MedicationRecord *current = medRecHead;
    bool found = false;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->department == this->department && current->pharmacistID == this->pharmacistID)
        {
            printMedicationRecord(current);
            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "没有找到任何用药记录。" << std::endl;
    }
    return found;
}
// 根据患者ID查询用药记录（仅限本部门且分配给自己的记录）
bool Pharmacist::getMedicationRecordsByPatientID(MedicationRecord *&medRecHead)
{
    std::string patientID = inputRecordIDCheck("请输入要查询的患者ID: ", {"pat"});

    MedicationRecord *current = medRecHead;
    bool found = false;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->patientID == patientID && current->department == this->department && current->pharmacistID == this->pharmacistID)
        {
            printMedicationRecord(current);
            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到该患者的用药记录。" << std::endl;
    }
    return found;
}
// 根据看诊记录ID查询用药记录（仅限本部门且分配给自己的记录）
bool Pharmacist::getMedicationRecordsByConsultationID(MedicationRecord *&medRecHead)
{
    std::string conID = inputRecordIDCheck("请输入要查询的看诊ID: ", {"con"});

    MedicationRecord *current = medRecHead;
    bool found = false;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->consultationID == conID && current->department == this->department && current->pharmacistID == this->pharmacistID)
        {
            printMedicationRecord(current);
            found = true;
        }
        current = current->next;
    }
    if (!found)
    {
        std::cout << "未找到该看诊对应的用药记录。" << std::endl;
    }
    return found;
}
// 根据用药记录ID查询用药记录（仅限本部门且分配给自己的记录）
bool Pharmacist::getMedicationRecordsByID(MedicationRecord *&medRecHead)
{
    std::string recID = inputRecordIDCheck("请输入要查询的用药记录ID: ", {"med", "mr", "mrec"});

    MedicationRecord *current = medRecHead;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->medRecordID == recID && current->department == this->department && current->pharmacistID == this->pharmacistID)
        {
            printMedicationRecord(current);
            return true;
        }
        current = current->next;
    }

    std::cout << "未找到该用药记录。" << std::endl;
    return false;
}
// 根据用药状态查询用药记录（仅限本部门且分配给自己的记录）
bool Pharmacist::getMedicationRecordsByStatus(MedicationRecord *&medRecHead, int select)
{
    int choice;
    if (select == -1)
    {
        std::cout << "请选择要查询的用药状态：" << std::endl;
        choice = MedicationRecordStatusMenu();
    }
    else
    {
        choice = select;
    }

    if (choice == 0)
    {
        return false;
    }

    MedicationStatus targetStatus = static_cast<MedicationStatus>(choice);

    MedicationRecord *current = medRecHead;
    bool found = false;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->status == targetStatus && current->department == this->department && current->pharmacistID == this->pharmacistID)
        {
            printMedicationRecord(current);
            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到该状态下的用药记录。" << std::endl;
    }
    return found;
}
// 根据审核状态查询用药记录（仅限本部门且分配给自己的记录）
bool Pharmacist::getMedicationRecordsByReviewStatus(MedicationRecord *&medRecHead)
{
    std::cout << "请选择要查询的审核状态：" << std::endl;
    int choice = MedicationRecordReviewResultMenu();
    if (choice == 0)
    {
        return false;
    }

    MedicationReviewStatus targetStatus = static_cast<MedicationReviewStatus>(choice);

    MedicationRecord *current = medRecHead;
    bool found = false;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->reviewStatus == targetStatus && current->department == this->department && current->pharmacistID == this->pharmacistID)
        {
            printMedicationRecord(current);
            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到该审核状态下的用药记录。" << std::endl;
    }
    return found;
}
// 根据创建时间范围查询用药记录（仅限本部门且分配给自己的记录）
bool Pharmacist::getMedicationRecordsByTimeRange(MedicationRecord *&medRecHead)
{
    std::string startDate = inputDateCheck("请输入查询的开始日期: ");
    std::string endDate = inputDateCheck("请输入查询的结束日期: ");

    MedicationRecord *current = medRecHead;
    bool found = false;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->department == this->department && current->pharmacistID == this->pharmacistID)
        {
            std::string recordDate = current->createTime.substr(0, 10);
            if (recordDate >= startDate && recordDate <= endDate)
            {
                printMedicationRecord(current);
                found = true;
            }
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到指定时间范围内的用药记录。" << std::endl;
    }
    return found;
}
// 根据药品名称查询用药记录（仅限本部门且分配给自己的记录）
bool Pharmacist::getMedicationRecordsByMedicineName(MedicationRecord *&medRecHead)
{
    std::string medicineName = inputStringCheck("请输入要查询的药品名称(支持模糊查询): ");

    MedicationRecord *current = medRecHead;
    bool found = false;

    while (current != nullptr)
    {
        if (!current->isDeleted && current->department == this->department && current->pharmacistID == this->pharmacistID)
        {
            for (const auto &line : current->lines)
            {
                if (line.medicineName.find(medicineName) != std::string::npos)
                {
                    printMedicationRecord(current);
                    found = true;
                    break;
                }
            }
        }
        current = current->next;
    }
    if (!found)
    {
        std::cout << "未找到该药品名称的用药记录。" << std::endl;
    }
    return found;
}

// 修改用药记录的用药状态
void Pharmacist::setMedicationRecordStatus(MedicationRecord *&target)
{
    std::cout << "当前用药状态: " << medicationStatusToString(target->status) << std::endl;
    int choice = MedicationRecordStatusMenu();
    if (choice == 0)
    {
        std::cout << "已取消修改！" << std::endl;
        return;
    }

    target->status = static_cast<MedicationStatus>(choice);
    std::cout << "用药状态已更新！" << std::endl;
}
// 修改用药记录的审核状态
void Pharmacist::setMedicationRecordReviewStatus(MedicationRecord *&target, Consultation *&conHead)
{
    Consultation *con = conHead;
    while (con)
    {
        if (con->consultationID == target->consultationID)
        {
            break;
        }
        con = con->next;
    }

    if (con == nullptr)
    {
        std::cout << "未找到关联的看诊记录，无法修改审核状态！" << std::endl;
        return;
    }

    if (con->isPrecriptionReviewed)
    {
        std::cout << "该用药记录对应的看诊记录中的处方已审核，不能修改审核状态！" << std::endl;
        return;
    }

    std::cout << "当前审核状态: " << medicationReviewStatusToString(target->reviewStatus) << std::endl;
    int choice = MedicationRecordReviewResultMenu();
    if (choice == 0)
    {
        std::cout << "已取消修改！" << std::endl;
        return;
    }

    target->reviewStatus = static_cast<MedicationReviewStatus>(choice);

    con->isPrecriptionReviewed = true; // 同步更新看诊记录中的处方审核状态

    increaseReviewCount();
    std::cout << "审核状态已更新！" << std::endl;
}
// 增加用药记录中的药品数量（仅限审核通过且未发药的记录）
void Pharmacist::setMedicationRecordMedicineIncrease(MedicationRecord *&target, Medicine *&medHead)
{
    if (target->reviewStatus != MedicationReviewStatus::APPROVED)
    {
        std::cout << "该用药记录尚未审核通过，不能修改药品信息！" << std::endl;
        return;
    }

    if (target->status == MedicationStatus::DISPENSED)
    {
        std::cout << "该用药记录已发药，不能修改药品信息！" << std::endl;
        return;
    }

    Medicine *currentMed = medHead;
    std::cout << "正在查询可用的药品..." << std::endl;
    while (currentMed != nullptr)
    {
        if (!currentMed->isDeleted && currentMed->status != MedicineStatus::EXPIRED && currentMed->status != MedicineStatus::DISCONTINUED)
        {
            std::cout << "药品ID: " << currentMed->medicineID
                      << ", 药品名称: " << currentMed->name
                      << ", 库存: " << currentMed->stock
                      << ", 销售价格: " << currentMed->salePrice
                      << std::endl;
        }
        currentMed = currentMed->next;
    }

    std::string medID = inputRecordIDCheck("请输入要增加的药品ID: ", {"med"});
    currentMed = medHead;

    while (currentMed != nullptr)
    {
        if (!currentMed->isDeleted && currentMed->medicineID == medID && currentMed->status != MedicineStatus::EXPIRED && currentMed->status != MedicineStatus::DISCONTINUED)
        {
            int quantity = inputIntCheck("请输入要增加的数量: ", 1, 1000);
            target->lines.push_back({medID, currentMed->name, quantity, currentMed->salePrice, ""});
            target->totalCost += quantity * currentMed->salePrice;
            std::cout << "已成功增加药品到用药记录！" << std::endl;
            return;
        }
        currentMed = currentMed->next;
    }

    std::cout << "未找到该药品，无法增加！" << std::endl;
}
// 减少用药记录中的药品数量（仅限审核通过且未发药的记录）
void Pharmacist::setMedicationRecordMedicineDecrease(MedicationRecord *&target, Medicine *&medHead)
{
    if (target->reviewStatus != MedicationReviewStatus::APPROVED)
    {
        std::cout << "该用药记录尚未审核通过，不能修改药品信息！" << std::endl;
        return;
    }

    if (target->status == MedicationStatus::DISPENSED)
    {
        std::cout << "该用药记录已发药，不能修改药品信息！" << std::endl;
        return;
    }

    bool hasMedicine = false;
    std::cout << "正在查询当前用药记录中的药品..." << std::endl;
    for (const auto &line : target->lines)
    {
        std::cout << "药品ID: " << line.medicineID
                  << ", 药品名称: " << line.medicineName
                  << ", 数量: " << line.quantity
                  << ", 单价: " << line.unitPrice
                  << std::endl;
        hasMedicine = true;
    }

    if (!hasMedicine)
    {
        std::cout << "该用药记录没有任何药品，无法减少！" << std::endl;
        return;
    }

    std::string medID = inputRecordIDCheck("请输入要减少的药品ID: ", {"med"});
    for (auto it = target->lines.begin(); it != target->lines.end(); ++it)
    {
        if (it->medicineID == medID)
        {
            int quantity = inputIntCheck("请输入要减少的数量: ", 1, it->quantity);
            it->quantity -= quantity;
            target->totalCost -= quantity * it->unitPrice;

            if (it->quantity == 0)
            {
                target->lines.erase(it);
            }

            std::cout << "已成功减少药品数量！" << std::endl;
            return;
        }
    }

    std::cout << "未找到该药品，无法减少！" << std::endl;
}
// 修改用药记录的备注信息（仅限审核通过且未发药的记录）
void Pharmacist::setMedicationRecordNote(MedicationRecord *&target)
{
    if (target->reviewStatus != MedicationReviewStatus::APPROVED)
    {
        std::cout << "该用药记录尚未审核通过，不能修改备注信息！" << std::endl;
        return;
    }
    if (target->status == MedicationStatus::DISPENSED)
    {
        std::cout << "该用药记录已发药，不能修改备注信息！" << std::endl;
        return;
    }

    std::cout << "当前备注: " << target->note << std::endl;
    target->note = inputStringCheck("请输入新的备注信息: ");
    std::cout << "备注已更新！" << std::endl;
}
// 发药（仅限审核通过且已缴费的记录）
bool Pharmacist::dispenseMedicine(MedicationRecord *&target, Medicine *&medHead)
{
    if (target->reviewStatus != MedicationReviewStatus::APPROVED)
    {
        std::cout << "该用药记录尚未审核通过，不能发药！" << std::endl;
        return false;
    }

    if (target->status != MedicationStatus::PAID)
    {
        std::cout << "该用药记录尚未缴费，不能发药！" << std::endl;
        return false;
    }

    for (auto &line : target->lines)
    {
        Medicine *currentMed = medHead;
        bool found = false;

        while (currentMed != nullptr)
        {
            if (!currentMed->isDeleted && currentMed->medicineID == line.medicineID)
            {
                found = true;

                if (currentMed->status == MedicineStatus::EXPIRED ||
                    currentMed->status == MedicineStatus::DISCONTINUED)
                {
                    std::cout << "药品 [" << currentMed->name << "] 当前状态不可发放！" << std::endl;
                    return false;
                }

                if (currentMed->stock < line.quantity)
                {
                    std::cout << "药品 [" << currentMed->name << "] 库存不足，无法发药！" << std::endl;
                    return false;
                }

                break;
            }
            currentMed = currentMed->next;
        }

        if (!found)
        {
            std::cout << "未找到药品ID为 " << line.medicineID << " 的药品，无法发药！" << std::endl;
            return false;
        }
    }

    for (auto &line : target->lines)
    {
        Medicine *currentMed = medHead;
        while (currentMed != nullptr)
        {
            if (!currentMed->isDeleted && currentMed->medicineID == line.medicineID)
            {
                currentMed->stock -= line.quantity;

                if (currentMed->stock < currentMed->safetyStock)
                {
                    currentMed->status = MedicineStatus::LOW_STOCK;
                }
                else if (currentMed->status != MedicineStatus::EXPIRED &&
                         currentMed->status != MedicineStatus::DISCONTINUED)
                {
                    currentMed->status = MedicineStatus::NORMAL;
                }

                break;
            }
            currentMed = currentMed->next;
        }
    }

    target->pharmacistID = this->pharmacistID;
    target->dispenseTime = MyTime::getInstance().getTime();
    target->status = MedicationStatus::DISPENSED;

    increaseDispenseCount();

    std::cout << "发药成功！" << std::endl;
    return true;
}
// 逻辑删除用药记录
void Pharmacist::deleteMedicationRecord(MedicationRecord *&target)
{
    target->isDeleted = true;
    std::cout << "用药记录已逻辑删除！" << std::endl;
}
// 创建用药记录（仅限关联的看诊记录已开具处方的情况）
void Pharmacist::addMedicationRecord(MedicationRecord *&medRecHead, Consultation *conHead, Medicine *&medHead, int &idCounter)
{
    Consultation *con = conHead;

    std::cout << "正在查找未审核的看诊记录..." << std::endl;

    bool hasUnreviewed = false;

    while (con)
    {
        if (!con->isDeleted && con->department == this->department && !con->prescriptions.empty() && !con->isPrecriptionReviewed)
        {
            std::cout << "看诊ID: " << con->consultationID
                      << ", 挂号ID: " << con->registrationID
                      << ", 医生ID: " << con->doctorID
                      << ", 科室: " << con->department
                      << ", 时间: " << con->consultationTime
                      << ", 状态: " << conStatusToString(con->status)
                      << ", 主诉: " << con->chiefComplaint
                      << ", 现病史: " << con->historyOfPresentIllness
                      << ", 既往史: " << con->pastMedicalHistory
                      << ", 家族史: " << con->familyHistory
                      << ", 初步诊断: " << con->preliminaryDiagnosis
                      << ", 检查项目数: " << con->examinationlist.size()
                      << ", 处方数: " << con->prescriptions.size()
                      << ", 处方审核状态: " << (con->isPrecriptionReviewed ? "已审核" : "未审核")
                      << ", 住院建议: " << (con->isHospitalizationRecommended ? "是" : "否")
                      << ", 备注: " << con->note
                      << std::endl;
            if (con->examinationlist.size() > 0)
            {
                std::cout << "检查项目列表:" << std::endl;
                for (const auto &exam : con->examinationlist)
                {
                    std::cout << "  - " << exam << std::endl;
                }
            }
            if (con->prescriptions.size() > 0)
            {
                std::cout << "处方列表:" << std::endl;
                for (const auto &pres : con->prescriptions)
                {
                    std::cout << "  - 药品ID: " << pres.medicineID
                              << ", 药品名称: " << pres.name
                              << ", 药品数量: " << pres.quantity
                              << ", 用量: " << pres.dosage
                              << ", 频次: " << pres.frequency
                              << ", 疗程: " << pres.duration
                              << ", 备注: " << pres.note
                              << std::endl;
                }
            }

            hasUnreviewed = true;
        }
        con = con->next;
    }

    if (!hasUnreviewed)
    {
        std::cout << "没有找到任何未审核的看诊记录，无法创建用药记录！" << std::endl;
        return;
    }

    std::string conID = inputRecordIDCheck("请输入关联的看诊ID: ", {"con"});

    while (con != nullptr)
    {
        if (!con->isDeleted && con->consultationID == conID && con->department == this->department && !con->prescriptions.empty() && !con->isPrecriptionReviewed)
        {
            break;
        }
        con = con->next;
    }

    if (con == nullptr)
    {
        std::cout << "未找到指定的看诊记录！" << std::endl;
        return;
    }

    if (con->prescriptions.empty())
    {
        std::cout << "该看诊记录尚未开具处方，无法添加用药记录！" << std::endl;
        return;
    }

    MedicationRecord *newRecord = new MedicationRecord;
    newRecord->medRecordID = "mrd" + std::to_string(++idCounter).insert(0, 6 - std::to_string(idCounter).length(), '0');
    newRecord->consultationID = con->consultationID;
    newRecord->doctorID = con->doctorID;
    newRecord->pharmacistID = this->pharmacistID;
    newRecord->patientID = con->patientID;
    newRecord->department = con->department;
    newRecord->createTime = MyTime::getInstance().getTime();
    newRecord->status = MedicationStatus::UNPAID;
    newRecord->reviewStatus = MedicationReviewStatus::PENDING_REVIEW;

    double total = 0.0;
    for (const auto &pres : con->prescriptions)
    {
        Medicine *currentMed = medHead;
        bool found = false;
        double price = 0.0;
        std::string medName = pres.name;

        while (currentMed != nullptr)
        {
            if (!currentMed->isDeleted && currentMed->medicineID == pres.medicineID)
            {
                price = currentMed->salePrice;
                medName = currentMed->name;
                found = true;
                break;
            }
            currentMed = currentMed->next;
        }

        MedicationLine line;
        line.medicineID = pres.medicineID;
        line.medicineName = medName;
        line.quantity = pres.quantity;
        line.unitPrice = price;
        line.note = pres.dosage + " " + pres.frequency + " " + pres.duration + " " + pres.note;
        newRecord->lines.push_back(line);

        total += line.quantity * line.unitPrice;
    }

    newRecord->totalCost = total;

    // 插入链表尾部
    if (medRecHead == nullptr)
    {
        medRecHead = newRecord;
    }
    else
    {
        MedicationRecord *temp = medRecHead;
        while (temp->next != nullptr)
        {
            temp = temp->next;
        }
        temp->next = newRecord;
        newRecord->prev = temp;
    }

    std::cout << "用药记录创建成功！记录ID: " << newRecord->medRecordID << std::endl;
}

void Pharmacist::manageMedicationRecords(MedicationRecord *&medRecHead, Medicine *&medHead, Consultation *conHead, int &idCounter)
{
    while (true)
    {
        int choice = pharmacistMedicationRecordManagementMenu();
        if (choice == 0)
        {
            break;
        }
        else if (choice == 1)
        {
            while (true)
            {
                int viewChoice = pharmacistMedicationRecordViewMenu();
                if (viewChoice == 0)
                {
                    break;
                }
                else if (viewChoice == 1)
                {
                    getAllMedicationRecords(medRecHead);
                }
                else if (viewChoice == 2)
                {
                    getMedicationRecordsByID(medRecHead);
                }
                else if (viewChoice == 3)
                {
                    getMedicationRecordsByConsultationID(medRecHead);
                }
                else if (viewChoice == 4)
                {
                    getMedicationRecordsByPatientID(medRecHead);
                }
                else if (viewChoice == 5)
                {
                    getMedicationRecordsByReviewStatus(medRecHead);
                }
                else if (viewChoice == 6)
                {
                    getMedicationRecordsByStatus(medRecHead);
                }
                else if (viewChoice == 7)
                {
                    getMedicationRecordsByTimeRange(medRecHead);
                }
                else if (viewChoice == 8)
                {
                    getMedicationRecordsByMedicineName(medRecHead);
                }
                pause();
            }
        }
        else if (choice == 2)
        {
            bool ishave = getAllMedicationRecords(medRecHead);
            if (!ishave)
            {
                pause();
                continue;
            }

            std::string recID = inputRecordIDCheck("请输入要修改的用药记录ID: ", {"mrd"});
            MedicationRecord *target = medRecHead;
            while (target)
            {
                if (!target->isDeleted && target->medRecordID == recID)
                {
                    break;
                }
                target = target->next;
            }

            if (!target)
            {
                std::cout << "未找到指定用药记录！" << std::endl;
                pause();
                continue;
            }

            while (true)
            {
                int modifyChoice = pharmacistMedicationRecordModificationMenu();
                if (modifyChoice == 0)
                {
                    break;
                }
                else if (modifyChoice == 1)
                {
                    setMedicationRecordStatus(target);
                }
                else if (modifyChoice == 2)
                {
                    setMedicationRecordReviewStatus(target, conHead);
                }
                else if (modifyChoice == 3)
                {
                    std::cout << "请选择你的操作:" << std::endl;
                    std::cout << "1. 增加药品" << std::endl;
                    std::cout << "2. 减少药品" << std::endl;
                    std::cout << "0. 取消" << std::endl;

                    int medModifyChoice = selectIntCheck(0, 2);

                    if (medModifyChoice == 1)
                    {
                        setMedicationRecordMedicineIncrease(target, medHead);
                    }
                    else if (medModifyChoice == 2)
                    {
                        setMedicationRecordMedicineDecrease(target, medHead);
                    }
                }
                else if (modifyChoice == 4)
                {
                    setMedicationRecordNote(target);
                }
                pause();
            }
        }
        else if (choice == 3) // 发药操作
        {
            bool ishave = getMedicationRecordsByStatus(medRecHead, 2);
            if (!ishave)
            {
                pause();
                continue;
            }

            std::string recID = inputRecordIDCheck("请输入要发药的用药记录ID: ", {"mrd"});
            MedicationRecord *target = medRecHead;
            while (target)
            {
                if (!target->isDeleted && target->medRecordID == recID && target->status == MedicationStatus::PAID)
                {
                    break;
                }
                target = target->next;
            }

            if (!target)
            {
                std::cout << "未找到指定用药记录！" << std::endl;
                pause();
                continue;
            }

            dispenseMedicine(target, medHead);
            pause();
        }
        else if (choice == 4) // 删除操作
        {
            bool ishave = getAllMedicationRecords(medRecHead);
            if (!ishave)
            {
                pause();
                continue;
            }

            std::string recID = inputRecordIDCheck("请输入要删除的用药记录ID: ", {"mrd"});
            MedicationRecord *target = medRecHead;
            while (target)
            {
                if (!target->isDeleted && target->medRecordID == recID)
                {
                    break;
                }
                target = target->next;
            }

            if (!target)
            {
                std::cout << "未找到指定用药记录！" << std::endl;
                pause();
                continue;
            }

            std::cout << "确认删除该用药记录吗？\n1. 确认\n0. 取消" << std::endl;
            int confirmChoice = selectIntCheck(0, 1);
            if (confirmChoice == 1)
            {
                deleteMedicationRecord(target);
            }
            else
            {
                std::cout << "已取消删除操作！" << std::endl;
            }
            pause();
        }
        else if (choice == 5) // 创建用药记录
        {
            addMedicationRecord(medRecHead, conHead, medHead, idCounter);
            pause();
        }
    }
}

// ==================== 药品信息管理 ====================

// # 表示通用药品，其他具体科室名称表示专科药品

// 获取全部药品信息（仅显示本部门的专科药和通用药）
bool Pharmacist::getAllMedicines(Medicine *&medHead)
{
    std::cout << "正在获取全部药品信息..." << std::endl;

    Medicine *current = medHead;
    bool found = false;
    while (current != nullptr)
    {
        if (!current->isDeleted && (current->department == this->department || current->department == "#"))
        {
            std::cout << "药品ID: " << current->medicineID
                      << ", 名称: " << current->name
                      << ", 规格: " << current->specification
                      << ", 厂家: " << current->manufacturer
                      << ", 进价: " << current->purchasePrice
                      << ", 售价: " << current->salePrice
                      << ", 库存: " << current->stock
                      << ", 安全库存: " << current->safetyStock
                      << ", 生产日期: " << current->productionDate
                      << ", 有效期: " << current->expiryDate
                      << ", 科室: " << current->department
                      << ", 是否专科药: " << (current->isSpecial ? "是" : "否")
                      << ", 状态: " << medicineStatusToString(current->status)
                      << ", 备注: " << current->note
                      << std::endl;
            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "没有找到任何药品信息。" << std::endl;
    }
    return found;
}
// 根据药品ID查询药品信息（仅限本部门的专科药和通用药）
bool Pharmacist::getMedicinesByID(Medicine *&medHead)
{
    std::string medID = inputRecordIDCheck("请输入要查询的药品ID: ", {"med"});

    Medicine *current = medHead;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->medicineID == medID && (current->department == this->department || current->department == "#"))
        {
            std::cout << "药品ID: " << current->medicineID
                      << ", 名称: " << current->name
                      << ", 规格: " << current->specification
                      << ", 厂家: " << current->manufacturer
                      << ", 进价: " << current->purchasePrice
                      << ", 售价: " << current->salePrice
                      << ", 库存: " << current->stock
                      << ", 安全库存: " << current->safetyStock
                      << ", 生产日期: " << current->productionDate
                      << ", 有效期: " << current->expiryDate
                      << ", 科室: " << current->department
                      << ", 是否专科药: " << (current->isSpecial ? "是" : "否")
                      << ", 状态: " << medicineStatusToString(current->status)
                      << ", 备注: " << current->note
                      << std::endl;
            return true;
        }
        current = current->next;
    }

    std::cout << "未找到该药品信息。" << std::endl;
    return false;
}
// 根据药品名称查询药品信息（支持模糊查询，仅限本部门的专科药和通用药）
bool Pharmacist::getMedicinesByName(Medicine *&medHead)
{
    std::string medName = inputStringCheck("请输入要查询的药品名称(支持模糊查询): ");

    Medicine *current = medHead;
    bool found = false;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->name.find(medName) != std::string::npos && (current->department == this->department || current->department == "#"))
        {
            std::cout << "药品ID: " << current->medicineID
                      << ", 名称: " << current->name
                      << ", 规格: " << current->specification
                      << ", 厂家: " << current->manufacturer
                      << ", 进价: " << current->purchasePrice
                      << ", 售价: " << current->salePrice
                      << ", 库存: " << current->stock
                      << ", 安全库存: " << current->safetyStock
                      << ", 生产日期: " << current->productionDate
                      << ", 有效期: " << current->expiryDate
                      << ", 科室: " << current->department
                      << ", 是否专科药: " << (current->isSpecial ? "是" : "否")
                      << ", 状态: " << medicineStatusToString(current->status)
                      << ", 备注: " << current->note
                      << std::endl;
            found = true;
        }
        current = current->next;
    }
    if (!found)
    {
        std::cout << "未找到该名称对应的药品。" << std::endl;
    }
    return found;
}
// 根据药品状态查询药品信息（仅限本部门的专科药和通用药）
bool Pharmacist::getMedicinesByStatus(Medicine *&medHead, int select)
{
    int choice;
    if (select == -1)
    {
        int choice = MedicineStatusMenu();
    }
    else
    {
        choice = select;
    }

    if (choice == 0)
    {
        return false;
    }

    MedicineStatus targetStatus = static_cast<MedicineStatus>(choice);

    Medicine *current = medHead;
    bool found = false;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->status == targetStatus && (current->department == this->department || current->department == "#"))
        {
            std::cout << "药品ID: " << current->medicineID
                      << ", 名称: " << current->name
                      << ", 规格: " << current->specification
                      << ", 厂家: " << current->manufacturer
                      << ", 进价: " << current->purchasePrice
                      << ", 售价: " << current->salePrice
                      << ", 库存: " << current->stock
                      << ", 安全库存: " << current->safetyStock
                      << ", 生产日期: " << current->productionDate
                      << ", 有效期: " << current->expiryDate
                      << ", 科室: " << current->department
                      << ", 是否专科药: " << (current->isSpecial ? "是" : "否")
                      << ", 状态: " << medicineStatusToString(current->status)
                      << ", 备注: " << current->note
                      << std::endl;
            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到该状态下的药品。" << std::endl;
    }
    return found;
}
// 根据科室标签查询药品信息（仅限本部门的专科药和通用药）
bool Pharmacist::getMedicinesByDepartment(Medicine *&medHead)
{
    std::string dept = inputDepartmentCheck("请输入要查询的科室标签: ");

    Medicine *current = medHead;
    bool found = false;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->department == dept && (current->department == this->department || current->department == "#"))
        {
            std::cout << "药品ID: " << current->medicineID
                      << ", 名称: " << current->name
                      << ", 规格: " << current->specification
                      << ", 厂家: " << current->manufacturer
                      << ", 进价: " << current->purchasePrice
                      << ", 售价: " << current->salePrice
                      << ", 库存: " << current->stock
                      << ", 安全库存: " << current->safetyStock
                      << ", 生产日期: " << current->productionDate
                      << ", 有效期: " << current->expiryDate
                      << ", 科室: " << current->department
                      << ", 是否专科药: " << (current->isSpecial ? "是" : "否")
                      << ", 状态: " << medicineStatusToString(current->status)
                      << ", 备注: " << current->note
                      << std::endl;
            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到该科室标签的药品。" << std::endl;
    }
    return found;
}

// 修改药品状态
void Pharmacist::setMedicineStatus(Medicine *&target)
{
    std::cout << "当前药品状态: " << medicineStatusToString(target->status) << std::endl;
    int choice = MedicineStatusMenu();
    if (choice == 0)
    {
        std::cout << "已取消修改！" << std::endl;
        return;
    }

    target->status = static_cast<MedicineStatus>(choice);
    std::cout << "药品状态已更新！" << std::endl;
}
// 修改药品名称
void Pharmacist::setMedicineName(Medicine *&target)
{
    std::cout << "当前药品名称: " << target->name << std::endl;
    target->name = inputStringCheck("请输入新的药品名称: ");
    std::cout << "药品名称已更新！" << std::endl;
}
// 修改药品规格
void Pharmacist::setMedicineSpecification(Medicine *&target)
{
    std::cout << "当前药品规格: " << target->specification << std::endl;
    target->specification = inputStringCheck("请输入新的药品规格: ");
    std::cout << "药品规格已更新！" << std::endl;
}
// 修改药品生产厂家
void Pharmacist::setMedicineManufacturer(Medicine *&target)
{
    std::cout << "当前生产厂家: " << target->manufacturer << std::endl;
    target->manufacturer = inputStringCheck("请输入新的生产厂家: ");
    std::cout << "生产厂家已更新！" << std::endl;
}
// 修改药品进价
void Pharmacist::setMedicinePurchasePrice(Medicine *&target)
{
    std::cout << "当前进价: " << target->purchasePrice << std::endl;
    target->purchasePrice = inputFeeCheck("请输入新的进价: ");
    std::cout << "进价已更新！" << std::endl;
}
// 修改药品售价
void Pharmacist::setMedicineSalePrice(Medicine *&target)
{
    std::cout << "当前售价: " << target->salePrice << std::endl;
    target->salePrice = inputFeeCheck("请输入新的售价: ");
    std::cout << "售价已更新！" << std::endl;
}
// 修改药品安全库存
void Pharmacist::setMedicineSafetyStock(Medicine *&target)
{
    std::cout << "当前安全库存: " << target->safetyStock << std::endl;
    target->safetyStock = inputIntCheck("请输入新的安全库存: ", 0, 1000000);
    std::cout << "安全库存已更新！" << std::endl;
}
// 修改药品生产日期
void Pharmacist::setMedicineProductionDate(Medicine *&target)
{
    std::cout << "当前生产日期: " << target->productionDate << std::endl;
    target->productionDate = inputDateCheck("请输入新的生产日期: ");
    std::cout << "生产日期已更新！" << std::endl;
}
// 修改药品有效期
void Pharmacist::setMedicineExpiryDate(Medicine *&target)
{
    std::cout << "当前有效期: " << target->expiryDate << std::endl;
    target->expiryDate = inputDateCheck("请输入新的有效期: ");
    std::cout << "有效期已更新！" << std::endl;
}
// 修改药品科室标签
void Pharmacist::setMedicineDepartment(Medicine *&target)
{
    std::cout << "当前科室标签: " << target->department << std::endl;
    target->department = inputDepartmentCheck("请输入新的科室标签: ");
    target->isSpecial = (target->department != "#");
    std::cout << "科室标签已更新！" << std::endl;
}
// 修改药品备注信息
void Pharmacist::setMedicineNote(Medicine *&target)
{
    std::cout << "当前备注: " << target->note << std::endl;
    target->note = inputStringCheck("请输入新的备注信息: ");
    std::cout << "药品备注已更新！" << std::endl;
}
// 逻辑删除药品信息
void Pharmacist::deleteMedicine(Medicine *&target)
{
    target->isDeleted = true;
    std::cout << "药品信息已逻辑删除！" << std::endl;
}
// 增加药品库存
void Pharmacist::addMedicineStock(Medicine *&target)
{
    std::cout << "当前库存: " << target->stock << std::endl;
    int amount = inputIntCheck("请输入增加数量: ", 1, 1000000);
    target->stock += amount;

    if (target->status != MedicineStatus::EXPIRED &&
        target->status != MedicineStatus::DISCONTINUED)
    {
        if (target->stock < target->safetyStock)
            target->status = MedicineStatus::LOW_STOCK;
        else
            target->status = MedicineStatus::NORMAL;
    }

    increaseInventoryManageCount();
    if (target->stock > target->safetyStock && target->status == MedicineStatus::LOW_STOCK)
    {
        target->status = MedicineStatus::NORMAL;
        std::cout << "药品库存已恢复至安全阈值以上，状态更新为NORMAL！" << std::endl;
    }
    std::cout << "库存增加成功！当前库存: " << target->stock << std::endl;
}
// 减少药品库存
void Pharmacist::reduceMedicineStock(Medicine *&target)
{
    std::cout << "当前库存: " << target->stock << std::endl;
    int amount = inputIntCheck("请输入减少数量: ", 1, target->stock);
    target->stock -= amount;

    if (target->status != MedicineStatus::EXPIRED &&
        target->status != MedicineStatus::DISCONTINUED)
    {
        if (target->stock < target->safetyStock)
            target->status = MedicineStatus::LOW_STOCK;
        else
            target->status = MedicineStatus::NORMAL;
    }

    increaseInventoryManageCount();
    if (target->stock <= target->safetyStock && target->status == MedicineStatus::NORMAL)
    {
        target->status = MedicineStatus::LOW_STOCK;
        std::cout << "警告：药品库存已低至安全阈值及以下，状态更新为LOW_STOCK！" << std::endl;
    }
    std::cout << "库存减少成功！当前库存: " << target->stock << std::endl;
}
// 添加新药品信息
void Pharmacist::addNewMedicine(Medicine *&medHead, int &idCounter)
{
    Medicine *newMed = new Medicine;
    newMed->medicineID = "med" + std::to_string(++idCounter).insert(0, 6 - std::to_string(idCounter).length(), '0');
    newMed->name = inputStringCheck("请输入药品名称: ");
    newMed->specification = inputStringCheck("请输入药品规格: ");
    newMed->manufacturer = inputStringCheck("请输入生产厂家: ");
    newMed->purchasePrice = inputFeeCheck("请输入进价: ");
    newMed->salePrice = inputFeeCheck("请输入售价: ");
    newMed->stock = inputIntCheck("请输入初始库存: ", 0, 1000000);
    newMed->safetyStock = inputIntCheck("请输入安全库存阈值: ", 0, 1000000);
    newMed->productionDate = inputDateCheck("请输入生产日期: ");
    newMed->expiryDate = inputDateCheck("请输入有效期: ");

    newMed->department = inputDepartmentCheck("请输入科室标签: ");
    newMed->isSpecial = (newMed->department != "#");
    newMed->status = newMed->stock < newMed->safetyStock ? MedicineStatus::LOW_STOCK : MedicineStatus::NORMAL;
    newMed->note = inputStringCheck("请输入备注信息: ");

    // 插入链表尾部
    if (medHead == nullptr)
    {
        medHead = newMed;
    }
    else
    {
        Medicine *temp = medHead;
        while (temp->next != nullptr)
        {
            temp = temp->next;
        }
        temp->next = newMed;
        newMed->prev = temp;
    }

    std::cout << "新药品信息添加成功！药品ID: " << newMed->medicineID << std::endl;
}

void Pharmacist::manageMedicines(Medicine *&medHead, int &idCounter)
{
    while (true)
    {
        int choice = pharmacistMedicineManagementMenu();
        if (choice == 0)
        {
            break;
        }
        else if (choice == 1)
        {
            while (true)
            {
                int viewChoice = pharmacistMedicineViewMenu();
                if (viewChoice == 0)
                {
                    break;
                }
                else if (viewChoice == 1)
                {
                    getAllMedicines(medHead);
                }
                else if (viewChoice == 2)
                {
                    getMedicinesByID(medHead);
                }
                else if (viewChoice == 3)
                {
                    getMedicinesByName(medHead);
                }
                else if (viewChoice == 4)
                {
                    getMedicinesByDepartment(medHead);
                }else if (viewChoice == 5)
                {
                    getMedicinesByStatus(medHead);
                }
                pause();
            }
        }
        else if (choice == 2)
        {
            bool ishave = getAllMedicines(medHead);
            if (!ishave)
            {
                pause();
                continue;
            }

            std::string medID = inputRecordIDCheck("请输入要修改的药品ID: ", {"med"});
            Medicine *target = medHead;
            while (target)
            {
                if (!target->isDeleted && target->medicineID == medID)
                {
                    break;
                }
                target = target->next;
            }

            if (!target)
            {
                std::cout << "未找到指定药品！" << std::endl;
                pause();
                continue;
            }

            while (true)
            {
                int modifyChoice = pharmacistMedicineModificationMenu();
                if (modifyChoice == 0)
                {
                    break;
                }
                else if (modifyChoice == 1) // 修改药品名称
                {
                    setMedicineName(target);
                }
                else if (modifyChoice == 2) // 修改药品规格
                {
                    setMedicineSpecification(target);
                }
                else if (modifyChoice == 3) // 修改药品生产厂家
                {
                    setMedicineManufacturer(target);
                }
                else if (modifyChoice == 4) // 修改药品进价
                {
                    setMedicinePurchasePrice(target);
                }
                else if (modifyChoice == 5) // 修改药品售价
                {
                    setMedicineSalePrice(target);
                }
                else if (modifyChoice == 6) // 修改安全库存阈值
                {
                    setMedicineSafetyStock(target);
                }
                else if (modifyChoice == 7) // 修改生产日期
                {
                    setMedicineProductionDate(target);
                }
                else if (modifyChoice == 8) // 修改有效期
                {
                    setMedicineExpiryDate(target);
                }
                else if (modifyChoice == 9) // 修改科室标签
                {
                    setMedicineDepartment(target);
                }
                else if (modifyChoice == 10) // 修改药品状态
                {
                    setMedicineStatus(target);
                }
                else if (modifyChoice == 11) // 修改药品备注信息
                {
                    setMedicineNote(target);
                }
                
                pause();
            }
        }
        else if (choice == 3)
        {
            bool ishave = getAllMedicines(medHead);
            if (!ishave)
            {
                pause();
                continue;
            }

            std::string medID = inputRecordIDCheck("请输入要删除的药品ID: ", {"med"});
            Medicine *target = medHead;
            while (target)
            {
                if (!target->isDeleted && target->medicineID == medID)
                {
                    break;
                }
                target = target->next;
            }

            if (!target)
            {
                std::cout << "未找到指定药品！" << std::endl;
                pause();
                continue;
            }

            std::cout << "当前要删除的药品信息如下:" << std::endl;
            std::cout << "药品ID: " << target->medicineID
                      << ", 名称: " << target->name
                      << ", 规格: " << target->specification
                      << ", 厂家: " << target->manufacturer
                      << ", 进价: " << target->purchasePrice
                      << ", 售价: " << target->salePrice
                      << ", 库存: " << target->stock
                      << ", 安全库存: " << target->safetyStock
                      << ", 生产日期: " << target->productionDate
                      << ", 有效期: " << target->expiryDate
                      << ", 科室: " << target->department
                      << ", 是否专科药: " << (target->isSpecial ? "是" : "否")
                      << ", 状态: " << medicineStatusToString(target->status)
                      << ", 备注: " << target->note
                      << std::endl;

            std::cout << "确认删除该药品吗？\n1. 确认\n0. 取消" << std::endl;
            int confirmChoice = selectIntCheck(0, 1);
            if (confirmChoice == 1)
            {
                deleteMedicine(target);
            }
            else
            {
                std::cout << "已取消删除操作！" << std::endl;
            }
            pause();
        }else if(choice == 4) // 添加新药品信息
        {
            addNewMedicine(medHead, idCounter);
            pause();
        }else if(choice == 5) // 管理药品库存
        {
            bool ishave = getAllMedicines(medHead);
            if (!ishave)
            {
                pause();
                continue;
            }

            std::string medID = inputRecordIDCheck("请输入要管理库存的药品ID: ", {"med"});
            Medicine *target = medHead;
            while (target)
            {
                if (!target->isDeleted && target->medicineID == medID)
                {
                    break;
                }
                target = target->next;
            }

            if (!target)
            {
                std::cout << "未找到指定药品！" << std::endl;
                pause();
                continue;
            }

            while(true){
                std::cout << "当前药品库存:" << std::endl;
                std::cout << "药品ID: " << target->medicineID
                          << ", 名称: " << target->name
                          << ", 库存: " << target->stock
                          << ", 安全库存: " << target->safetyStock
                          << ", 状态: " << medicineStatusToString(target->status)
                          << std::endl;

                std::cout << "请选择库存管理操作:\n1. 增加库存\n2. 减少库存\n0. 取消" << std::endl;
                int stockChoice = selectIntCheck(0, 2);
                if(stockChoice == 0){
                    std::cout << "已取消库存管理操作！" << std::endl;
                    break;
                }else if(stockChoice == 1){
                    addMedicineStock(target);
                }else if(stockChoice == 2){
                    reduceMedicineStock(target);
                }
                pause();
            }
        }
    }
}

// ==================== 个人信息管理 ====================

void Pharmacist::managePersonalInfo()
{
    while (true)
    {
        int choice = pharmacistPersonalInfoManagementMenu();
        if (choice == 0)
        {
            break;
        }
        else if (choice == 1)
        {
            while (true)
            {
                int viewChoice = pharmacistPersonalInfoViewMenu();
                if (viewChoice == 0)
                {
                    break;
                }
                else if (viewChoice == 1)
                {
                    std::cout << "药剂师ID: " << this->pharmacistID << std::endl;
                }
                else if (viewChoice == 2)
                {
                    std::cout << "姓名: " << this->username << std::endl;
                }
                else if (viewChoice == 3)
                {
                    std::cout << "性别: " << this->gender << std::endl;
                }
                else if (viewChoice == 4)
                {
                    std::cout << "年龄: " << this->age << std::endl;
                }
                else if (viewChoice == 5)
                {
                    std::cout << "科室: " << this->department << std::endl;
                }
                else if (viewChoice == 6)
                {
                    std::cout << "职称: " << pharmacistLevelToString(this->level) << std::endl;
                }
                else if (viewChoice == 7)
                {
                    std::cout << "联系电话: " << this->telephone << std::endl;
                }
                else if (viewChoice == 8)
                {
                    std::cout << "邮箱: " << this->email << std::endl;
                }
                else if (viewChoice == 9)
                {
                    std::cout << "在岗状态: " << (this->isOnDuty ? "在岗" : "不在岗") << std::endl;
                }
                else if (viewChoice == 10)
                {
                    std::cout << "排班信息: " << this->scheduleInfo << std::endl;
                }
                else if (viewChoice == 11)
                {
                    std::cout << "累计审核次数: " << this->reviewCount << std::endl;
                }
                else if (viewChoice == 12)
                {
                    std::cout << "累计发药次数: " << this->dispenseCount << std::endl;
                }
                else if (viewChoice == 13)
                {
                    std::cout << "累计库存管理次数: " << this->inventoryManageCount << std::endl;
                }
                else if (viewChoice == 14)
                {
                    std::cout << "账户创建时间: " << this->createTime << std::endl;
                }
                pause();
            }
        }
        else if (choice == 2)
        {
            while (true)
            {
                int modifyChoice = pharmacistPersonalInfoModificationMenu();

                if (modifyChoice == 0)
                {
                    break;
                }
                else if (modifyChoice == 1)
                {
                    std::cout << "当前姓名: " << this->username << std::endl;
                    this->username = inputStringCheck("请输入新的姓名: ");
                    std::cout << "姓名已更新！" << std::endl;
                }
                else if (modifyChoice == 2)
                {
                    std::cout << "当前性别: " << this->gender << std::endl;
                    this->gender = inputGenderCheck("请输入新的性别: ");
                    std::cout << "性别已更新！" << std::endl;
                }
                else if (modifyChoice == 3)
                {
                    std::cout << "当前年龄: " << this->age << std::endl;
                    this->age = inputAgeCheck("请输入新的年龄: ");
                    std::cout << "年龄已更新！" << std::endl;
                }
                else if (modifyChoice == 4)
                {
                    std::cout << "当前科室: " << this->department << std::endl;
                    this->department = inputDepartmentCheck("请输入新的科室: ");
                    std::cout << "科室已更新！" << std::endl;
                }
                else if (modifyChoice == 5)
                {
                    std::cout << "当前职称: " << pharmacistLevelToString(this->level) << std::endl;
                    int levelChoice = PharmacistTitleMenu();
                    if (levelChoice == 0)
                    {
                        std::cout << "已取消修改操作！" << std::endl;
                        continue;
                    }
                    this->level = static_cast<PharmacistLevel>(levelChoice);
                    std::cout << "职称已更新！" << std::endl;
                }
                else if (modifyChoice == 6)
                {
                    std::cout << "当前联系电话: " << this->telephone << std::endl;
                    this->telephone = inputTelephoneCheck("请输入新的联系电话: ");
                    std::cout << "联系电话已更新！" << std::endl;
                }
                else if (modifyChoice == 7)
                {
                    std::cout << "当前邮箱: " << this->email << std::endl;
                    this->email = inputEmailCheck("请输入新的邮箱: ");
                    std::cout << "邮箱已更新！" << std::endl;
                }
                else if (modifyChoice == 8)
                {
                    std::cout << "当前在岗状态: " << (this->isOnDuty ? "在岗" : "不在岗") << std::endl;
                    std::cout << "请选择新的在岗状态: " << std::endl;
                    std::cout << "1. 在岗" << std::endl;
                    std::cout << "2. 不在岗" << std::endl;
                    std::cout << "0. 取消" << std::endl;

                    int dutyChoice = selectIntCheck(0, 2);

                    if(dutyChoice == 0)
                    {
                        std::cout << "已取消修改操作！" << std::endl;
                        continue;
                    }

                    this->isOnDuty = (dutyChoice == 1);
                    std::cout << "在岗状态已更新！" << std::endl;
                }
                else if (modifyChoice == 9)
                {
                    std::cout << "当前排班信息: " << this->scheduleInfo << std::endl;
                    this->scheduleInfo = inputStringCheck("请输入新的排班信息: ");
                    std::cout << "排班信息已更新！" << std::endl;
                }
                else if (modifyChoice == 10)
                {
                    std::string oldpwd = inputPwdCheck("请输入当前密码以验证身份: ");
                    if (SHA256Verify(oldpwd, this->storedHash, this->kHashIterations))
                    {
                        std::string newpwd1 = inputPwdCheck("请输入新的密码: ");
                        std::string newpwd2 = inputPwdCheck("请再次输入新的密码以确认: ");

                        if (newpwd1 == newpwd2)
                        {
                            std::string newSalt = generateSalt();
                            std::string newHash = SHA256Encrypt(newpwd1, newSalt, this->kHashIterations);
                            this->salt = newSalt;
                            this->storedHash = newHash;
                            std::cout << "密码更新成功！" << std::endl;
                        }
                        else
                        {
                            std::cout << "两次输入的新密码不一致，密码更新失败！" << std::endl;
                        }
                    }
                    else
                    {
                        std::cout << "密码验证失败，无法修改密码！" << std::endl;
                    }
                }

                pause();
            }
        }
    }
}