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

bool Pharmacist::getAllMedicationRecords(MedicationRecord *&medRecHead)
{
    std::cout << "正在获取全部用药记录..." << std::endl;

    MedicationRecord *current = medRecHead;
    bool found = false;
    while (current != nullptr)
    {
        if (!current->isDeleted)
        {
            std::cout << "用药记录ID: " << current->medRecordID
                      << ", 看诊ID: " << current->consultationID
                      << ", 医生ID: " << current->doctorID
                      << ", 药剂师ID: " << current->pharmacistID
                      << ", 患者ID: " << current->patientID
                      << ", 科室: " << current->department
                      << ", 创建时间: " << current->createTime
                      << ", 总费用: " << current->totalCost
                      << ", 审核状态: " << medicationReviewStatusToString(current->reviewStatus)
                      << ", 用药状态: " << medicationStatusToString(current->status)
                      << ", 支付时间: " << current->paymentTime
                      << ", 发药时间: " << current->dispenseTime
                      << ", 备注: " << current->note
                      << std::endl;

            if (!current->lines.empty())
            {
                std::cout << "用药明细:" << std::endl;
                for (const auto &line : current->lines)
                {
                    std::cout << "  - 药品ID: " << line.medicineID
                              << ", 药品名称: " << line.medicineName
                              << ", 数量: " << line.quantity
                              << ", 单价: " << line.unitPrice
                              << ", 备注: " << line.note
                              << std::endl;
                }
            }
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

bool Pharmacist::getMedicationRecordsByPatientID(MedicationRecord *&medRecHead)
{
    std::string patientID = inputRecordIDCheck("请输入要查询的患者ID: ", {"pat"});

    MedicationRecord *current = medRecHead;
    bool found = false;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->patientID == patientID)
        {
            std::cout << "用药记录ID: " << current->medRecordID
                      << ", 患者ID: " << current->patientID
                      << ", 看诊ID: " << current->consultationID
                      << ", 审核状态: " << medicationReviewStatusToString(current->reviewStatus)
                      << ", 用药状态: " << medicationStatusToString(current->status)
                      << std::endl;
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

bool Pharmacist::getMedicationRecordsByConsultationID(MedicationRecord *&medRecHead)
{
    std::string conID = inputRecordIDCheck("请输入要查询的看诊ID: ", {"con"});

    MedicationRecord *current = medRecHead;
    bool found = false;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->consultationID == conID)
        {
            std::cout << "用药记录ID: " << current->medRecordID
                      << ", 看诊ID: " << current->consultationID
                      << ", 患者ID: " << current->patientID
                      << ", 审核状态: " << medicationReviewStatusToString(current->reviewStatus)
                      << ", 用药状态: " << medicationStatusToString(current->status)
                      << std::endl;
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

bool Pharmacist::getMedicationRecordsByID(MedicationRecord *&medRecHead)
{
    std::string recID = inputRecordIDCheck("请输入要查询的用药记录ID: ", {"med", "mr", "mrec"});

    MedicationRecord *current = medRecHead;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->medRecordID == recID)
        {
            std::cout << "用药记录ID: " << current->medRecordID
                      << ", 看诊ID: " << current->consultationID
                      << ", 医生ID: " << current->doctorID
                      << ", 药剂师ID: " << current->pharmacistID
                      << ", 患者ID: " << current->patientID
                      << ", 科室: " << current->department
                      << ", 创建时间: " << current->createTime
                      << ", 总费用: " << current->totalCost
                      << ", 审核状态: " << medicationReviewStatusToString(current->reviewStatus)
                      << ", 用药状态: " << medicationStatusToString(current->status)
                      << ", 支付时间: " << current->paymentTime
                      << ", 发药时间: " << current->dispenseTime
                      << ", 备注: " << current->note
                      << std::endl;

            if (!current->lines.empty())
            {
                std::cout << "用药明细:" << std::endl;
                for (const auto &line : current->lines)
                {
                    std::cout << "  - 药品ID: " << line.medicineID
                              << ", 药品名称: " << line.medicineName
                              << ", 数量: " << line.quantity
                              << ", 单价: " << line.unitPrice
                              << ", 备注: " << line.note
                              << std::endl;
                }
            }
            return true;
        }
        current = current->next;
    }

    std::cout << "未找到该用药记录。" << std::endl;
    return false;
}

bool Pharmacist::getMedicationRecordsByStatus(MedicationRecord *&medRecHead)
{
    std::cout << "请选择要查询的用药状态：" << std::endl;
    int choice = MedicationRecordStatusMenu();
    if (choice == 0)
    {
        return false;
    }

    MedicationStatus targetStatus = static_cast<MedicationStatus>(choice);

    MedicationRecord *current = medRecHead;
    bool found = false;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->status == targetStatus)
        {
            std::cout << "用药记录ID: " << current->medRecordID
                      << ", 患者ID: " << current->patientID
                      << ", 用药状态: " << medicationStatusToString(current->status)
                      << ", 审核状态: " << medicationReviewStatusToString(current->reviewStatus)
                      << std::endl;
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
        if (!current->isDeleted && current->reviewStatus == targetStatus)
        {
            std::cout << "用药记录ID: " << current->medRecordID
                      << ", 患者ID: " << current->patientID
                      << ", 用药状态: " << medicationStatusToString(current->status)
                      << ", 审核状态: " << medicationReviewStatusToString(current->reviewStatus)
                      << std::endl;
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

bool Pharmacist::getMedicationRecordsByTimeRange(MedicationRecord *&medRecHead)
{
    std::string startDate = inputDateCheck("请输入查询的开始日期 (格式 YYYY-MM-DD): ");
    std::string endDate = inputDateCheck("请输入查询的结束日期 (格式 YYYY-MM-DD): ");

    MedicationRecord *current = medRecHead;
    bool found = false;
    while (current != nullptr)
    {
        if (!current->isDeleted)
        {
            std::string recordDate = current->createTime.substr(0, 10);
            if (recordDate >= startDate && recordDate <= endDate)
            {
                std::cout << "用药记录ID: " << current->medRecordID
                          << ", 患者ID: " << current->patientID
                          << ", 创建时间: " << current->createTime
                          << ", 用药状态: " << medicationStatusToString(current->status)
                          << ", 审核状态: " << medicationReviewStatusToString(current->reviewStatus)
                          << std::endl;
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

void Pharmacist::setMedicationRecordReviewStatus(MedicationRecord *&target)
{
    std::cout << "当前审核状态: " << medicationReviewStatusToString(target->reviewStatus) << std::endl;
    int choice = MedicationRecordReviewResultMenu();
    if (choice == 0)
    {
        std::cout << "已取消修改！" << std::endl;
        return;
    }

    target->reviewStatus = static_cast<MedicationReviewStatus>(choice);
    increaseReviewCount();
    std::cout << "审核状态已更新！" << std::endl;
}

void Pharmacist::setMedicationRecordNote(MedicationRecord *&target)
{
    std::cout << "当前备注: " << target->note << std::endl;
    target->note = inputStringCheck("请输入新的备注信息: ");
    std::cout << "备注已更新！" << std::endl;
}

void Pharmacist::assignPharmacistToMedicationRecord(MedicationRecord *&target)
{
    target->pharmacistID = this->pharmacistID;
    std::cout << "已将当前药剂师分配到该用药记录。" << std::endl;
}

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

void Pharmacist::deleteMedicationRecord(MedicationRecord *&target)
{
    target->isDeleted = true;
    std::cout << "用药记录已逻辑删除！" << std::endl;
}

void Pharmacist::manageMedicationRecords(MedicationRecord *&medRecHead, Medicine *&medHead)
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
                    getMedicationRecordsByPatientID(medRecHead);
                }
                else if (viewChoice == 3)
                {
                    getMedicationRecordsByConsultationID(medRecHead);
                }
                else if (viewChoice == 4)
                {
                    getMedicationRecordsByID(medRecHead);
                }
                else if (viewChoice == 5)
                {
                    getMedicationRecordsByStatus(medRecHead);
                }
                else if (viewChoice == 6)
                {
                    getMedicationRecordsByReviewStatus(medRecHead);
                }
                else if (viewChoice == 7)
                {
                    getMedicationRecordsByTimeRange(medRecHead);
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

            std::string recID = inputRecordIDCheck("请输入要修改的用药记录ID: ", {"med", "mr", "mrec"});
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
                    setMedicationRecordReviewStatus(target);
                }
                else if (modifyChoice == 3)
                {
                    assignPharmacistToMedicationRecord(target);
                }
                else if (modifyChoice == 4)
                {
                    dispenseMedicine(target, medHead);
                }
                else if (modifyChoice == 5)
                {
                    setMedicationRecordNote(target);
                }
                pause();
            }
        }
        else if (choice == 3)
        {
            bool ishave = getAllMedicationRecords(medRecHead);
            if (!ishave)
            {
                pause();
                continue;
            }

            std::string recID = inputRecordIDCheck("请输入要删除的用药记录ID: ", {"med", "mr", "mrec"});
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
    }
}

// ==================== 药品信息管理 ====================

bool Pharmacist::getAllMedicines(Medicine *&medHead)
{
    std::cout << "正在获取全部药品信息..." << std::endl;

    Medicine *current = medHead;
    bool found = false;
    while (current != nullptr)
    {
        if (!current->isDeleted)
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

bool Pharmacist::getMedicinesByID(Medicine *&medHead)
{
    std::string medID = inputRecordIDCheck("请输入要查询的药品ID: ", {"med"});

    Medicine *current = medHead;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->medicineID == medID)
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

bool Pharmacist::getMedicinesByName(Medicine *&medHead)
{
    std::string medName = inputStringCheck("请输入要查询的药品名称: ");

    Medicine *current = medHead;
    bool found = false;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->name == medName)
        {
            std::cout << "药品ID: " << current->medicineID
                      << ", 名称: " << current->name
                      << ", 规格: " << current->specification
                      << ", 库存: " << current->stock
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

bool Pharmacist::getMedicinesByStatus(Medicine *&medHead)
{
    int choice = MedicineStatusMenu();
    if (choice == 0)
    {
        return false;
    }

    MedicineStatus targetStatus = static_cast<MedicineStatus>(choice);

    Medicine *current = medHead;
    bool found = false;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->status == targetStatus)
        {
            std::cout << "药品ID: " << current->medicineID
                      << ", 名称: " << current->name
                      << ", 库存: " << current->stock
                      << ", 状态: " << medicineStatusToString(current->status)
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

void Pharmacist::setMedicineName(Medicine *&target)
{
    std::cout << "当前药品名称: " << target->name << std::endl;
    target->name = inputStringCheck("请输入新的药品名称: ");
    std::cout << "药品名称已更新！" << std::endl;
}

void Pharmacist::setMedicineSpecification(Medicine *&target)
{
    std::cout << "当前药品规格: " << target->specification << std::endl;
    target->specification = inputStringCheck("请输入新的药品规格: ");
    std::cout << "药品规格已更新！" << std::endl;
}

void Pharmacist::setMedicineManufacturer(Medicine *&target)
{
    std::cout << "当前生产厂家: " << target->manufacturer << std::endl;
    target->manufacturer = inputStringCheck("请输入新的生产厂家: ");
    std::cout << "生产厂家已更新！" << std::endl;
}

void Pharmacist::setMedicinePurchasePrice(Medicine *&target)
{
    std::cout << "当前进价: " << target->purchasePrice << std::endl;
    target->purchasePrice = inputFeeCheck("请输入新的进价: ");
    std::cout << "进价已更新！" << std::endl;
}

void Pharmacist::setMedicineSalePrice(Medicine *&target)
{
    std::cout << "当前售价: " << target->salePrice << std::endl;
    target->salePrice = inputFeeCheck("请输入新的售价: ");
    std::cout << "售价已更新！" << std::endl;
}

void Pharmacist::setMedicineSafetyStock(Medicine *&target)
{
    std::cout << "当前安全库存: " << target->safetyStock << std::endl;
    target->safetyStock = inputIntCheck("请输入新的安全库存: ", 0, 1000000);
    std::cout << "安全库存已更新！" << std::endl;
}

void Pharmacist::setMedicineProductionDate(Medicine *&target)
{
    std::cout << "当前生产日期: " << target->productionDate << std::endl;
    target->productionDate = inputDateCheck("请输入新的生产日期 (YYYY-MM-DD): ");
    std::cout << "生产日期已更新！" << std::endl;
}

void Pharmacist::setMedicineExpiryDate(Medicine *&target)
{
    std::cout << "当前有效期: " << target->expiryDate << std::endl;
    target->expiryDate = inputDateCheck("请输入新的有效期 (YYYY-MM-DD): ");
    std::cout << "有效期已更新！" << std::endl;
}

void Pharmacist::setMedicineDepartment(Medicine *&target)
{
    std::cout << "当前科室标签: " << target->department << std::endl;
    target->department = inputDepartmentCheck("请输入新的科室标签: ");
    std::cout << "科室标签已更新！" << std::endl;
}

void Pharmacist::setMedicineSpecialFlag(Medicine *&target)
{
    std::cout << "当前是否专科药: " << (target->isSpecial ? "是" : "否") << std::endl;
    std::cout << "请选择新的设置:\n1. 是\n2. 否" << std::endl;
    int choice = selectIntCheck(1, 2);
    target->isSpecial = (choice == 1);
    std::cout << "专科药标记已更新！" << std::endl;
}

void Pharmacist::setMedicineNote(Medicine *&target)
{
    std::cout << "当前备注: " << target->note << std::endl;
    target->note = inputStringCheck("请输入新的备注信息: ");
    std::cout << "药品备注已更新！" << std::endl;
}

void Pharmacist::deleteMedicine(Medicine *&target)
{
    target->isDeleted = true;
    std::cout << "药品信息已逻辑删除！" << std::endl;
}

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
    std::cout << "库存增加成功！当前库存: " << target->stock << std::endl;
}

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
    std::cout << "库存减少成功！当前库存: " << target->stock << std::endl;
}

void Pharmacist::manageMedicines(Medicine *&medHead)
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
                else if (modifyChoice == 1)
                {
                    setMedicineStatus(target);
                }
                else if (modifyChoice == 2)
                {
                    setMedicineName(target);
                }
                else if (modifyChoice == 3)
                {
                    setMedicineSpecification(target);
                }
                else if (modifyChoice == 4)
                {
                    setMedicineManufacturer(target);
                }
                else if (modifyChoice == 5)
                {
                    setMedicinePurchasePrice(target);
                }
                else if (modifyChoice == 6)
                {
                    setMedicineSalePrice(target);
                }
                else if (modifyChoice == 7)
                {
                    addMedicineStock(target);
                }
                else if (modifyChoice == 8)
                {
                    reduceMedicineStock(target);
                }
                else if (modifyChoice == 9)
                {
                    setMedicineSafetyStock(target);
                }
                else if (modifyChoice == 10)
                {
                    setMedicineProductionDate(target);
                }
                else if (modifyChoice == 11)
                {
                    setMedicineExpiryDate(target);
                }
                else if (modifyChoice == 12)
                {
                    setMedicineDepartment(target);
                }
                else if (modifyChoice == 13)
                {
                    setMedicineSpecialFlag(target);
                }
                else if (modifyChoice == 14)
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
                    std::cout << "等级: " << pharmacistLevelToString(this->level) << std::endl;
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
                    std::cout << "当前等级: " << pharmacistLevelToString(this->level) << std::endl;
                    int levelChoice = PharmacistTitleMenu();
                    if (levelChoice == 0)
                    {
                        std::cout << "已取消修改操作！" << std::endl;
                        continue;
                    }
                    this->level = static_cast<PharmacistLevel>(levelChoice);
                    std::cout << "等级已更新！" << std::endl;
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
                    std::cout << "1. 在岗" << std::endl;
                    std::cout << "2. 不在岗" << std::endl;
                    int dutyChoice = selectIntCheck(1, 2);
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
                    std::string oldpwd = inputStringCheck("请输入当前密码以验证身份: ");
                    if (SHA256Verify(oldpwd, this->storedHash, this->kHashIterations))
                    {
                        std::string newpwd1 = inputStringCheck("请输入新的密码: ");
                        std::string newpwd2 = inputStringCheck("请再次输入新的密码以确认: ");

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