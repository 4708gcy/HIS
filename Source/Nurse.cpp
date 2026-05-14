/**
 * @file Nurse.cpp
 * @brief 医疗管理系统中的护士类实现
 * @details 该源文件实现了 Nurse 类的构造、析构、注册登录、
 *          检查记录管理、住院/床位管理及个人信息管理等功能。
 * @author 李阳旭 / 小组成员
 * @date 2026-4-15
 * @version 1.0
 */

#include "../Head/Nurse.h"

Nurse::Nurse()
{
}

Nurse::~Nurse()
{
}

bool Nurse::nurseSignUp(int &idCounter)
{
    bool success = signUp(3, idCounter); // 假定护士角色编号为 3
    if (!success)
    {
        printError("护士注册失败！");
        return false;
    }

    this->nurseID = this->userID;
    this->department = inputDepartmentCheck("请输入所属科室: ");

    clearScreen();
    printMenuBorder();
    printMenuTitle("选择护士等级");
    printMenuBottom();
    printMenuItem(1, "实习护士");
    printMenuItem(2, "初级护士");
    printMenuItem(3, "高级护士");
    printMenuItem(4, "护士长");
    printMenuBottom();
    int levelChoice = selectIntCheck(1, 4);
    this->level = static_cast<NurseLevel>(levelChoice);

    this->scheduleInfo = inputStringCheck("请输入排班信息: ");

    clearScreen();
    printMenuBorder();
    printMenuTitle("在岗状态");
    printMenuBottom();
    printMenuItem(1, "是");
    printMenuItem(0, "否");
    printMenuBottom();
    int onDutyChoice = selectIntCheck(0, 1);
    this->isOnDuty = (onDutyChoice == 1);

    printSuccess("护士注册成功! 您的用户ID是: " + this->userID);
    return true;
}

bool Nurse::nurseSignIn()
{
    if (isAccountActive == false)
    {
        printError("账户已锁定，请联系系统管理员解锁！");
        return false;
    }

    while (loginAttempts < kMaxLoginAttempts)
    {
        std::string pwd = inputHiddenPwdCheck("请输入密码(输入\"quit\"退出登录): ");

        if (pwd == "quit")
        {
            std::cout << "退出登录。" << std::endl;
            return false;
        }

        bool success = SHA256Verify(pwd, storedHash, kHashIterations);

        if (success)
        {
            loginAttempts = 0;
            isLoggedIn = true;
            printSuccess("护士登录成功！");
            return true;
        }
        else
        {
            loginAttempts++;
            printError("密码错误! 请重新输入密码(当前失败次数: " + std::to_string(loginAttempts) + ")");

            if (loginAttempts >= kMaxLoginAttempts)
            {
                isAccountActive = false;
                printError("连续登录失败次数过多，账户已锁定，请联系系统管理员解锁！");
            }
        }
    }

    return false;
}

// ==================== Getter ====================

const std::string &Nurse::getNurseID() const
{
    return nurseID;
}

const std::string &Nurse::getDepartment() const
{
    return department;
}

NurseLevel Nurse::getLevel() const
{
    return level;
}

const std::string &Nurse::getScheduleInfo() const
{
    return scheduleInfo;
}

int Nurse::getPatientCareCount() const
{
    return patientCareCount;
}

int Nurse::getBedManageCount() const
{
    return bedManageCount;
}

bool Nurse::getIsOnDuty() const
{
    return isOnDuty;
}

// ==================== Setter ====================

void Nurse::setNurseID(const std::string &id)
{
    nurseID = id;
}

void Nurse::setDepartment(const std::string &dept)
{
    department = dept;
}

void Nurse::setLevel(NurseLevel lvl)
{
    level = lvl;
}

void Nurse::setScheduleInfo(const std::string &schedule)
{
    scheduleInfo = schedule;
}

void Nurse::setPatientCareCount(int count)
{
    patientCareCount = count;
}

void Nurse::setBedManageCount(int count)
{
    bedManageCount = count;
}

void Nurse::setIsOnDuty(bool onDuty)
{
    isOnDuty = onDuty;
}

// ==================== 统计辅助 ====================

void Nurse::increasePatientCareCount()
{
    ++patientCareCount;
}

void Nurse::increaseBedManageCount()
{
    ++bedManageCount;
}

// ==================== 检查记录管理 ====================

// 辅助函数：打印检查记录详情
void Nurse::printExaminationDetails(Examination *&exa)
{
    if (exa == nullptr)
    {
        printError("检查记录不存在。");
        return;
    }

    std::cout << "检查ID: " << exa->examinationID
              << ", 看诊ID: " << exa->consultationID
              << ", 患者ID: " << exa->patientID
              << ", 医生ID: " << exa->doctorID
              << ", 科室: " << exa->department
              << ", 开单时间: " << exa->orderTime
              << ", 检查项目: " << exa->itemName
              << ", 检查结果: " << findVitalSignToString(exa)
              << ", 报告摘要: " << exa->reportSummary
              << ", 检查费用: " << exa->fee
              << ", 出报告时间: " << exa->reportTime
              << ", 状态: " << examStatusToString(exa->status)
              << ", 备注: " << exa->note;

    std::cout << ", 相关附件: ";
    for (size_t i = 0; i < exa->attachments.size(); ++i)
    {
        std::cout << "[" << (i + 1) << "] " << exa->attachments[i] << "  ";
    }
    std::cout << std::endl;
}

// 查询本科室的所有检查记录
bool Nurse::getAllExaminations(Examination *&exaHead)
{
    std::cout << "正在获取本科室的所有检查记录..." << std::endl;

    Examination *current = exaHead;
    bool found = false;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->department == this->department)
        {
            printExaminationDetails(current);
            std::cout << std::endl;
            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        printError("未找到本科室的检查记录。");
    }
    return found;
}
// 根据患者ID查询检查记录
bool Nurse::getExaminationsByPatientID(Examination *&exaHead)
{
    std::string patientID = inputIDCheck("请输入要查询的患者ID: ");

    Examination *current = exaHead;
    bool found = false;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->department == this->department && current->patientID == patientID)
        {
            printExaminationDetails(current);
            std::cout << std::endl;
            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        printError("没有找到患者ID为 " + patientID + " 的检查记录。");
    }
    return found;
}
// 根据看诊ID查询检查记录
bool Nurse::getExaminationsByConsultationID(Examination *&exaHead)
{
    std::string conID = inputRecordIDCheck("请输入要查询的看诊ID: ", {"con"});

    Examination *current = exaHead;
    bool found = false;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->department == this->department && current->consultationID == conID)
        {
            printExaminationDetails(current);
            std::cout << std::endl;
            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        printError("未找到看诊ID为 " + conID + " 的检查记录。");
    }
    return found;
}
// 根据检查ID查询检查记录
bool Nurse::getExaminationsByID(Examination *&exaHead)
{
    std::string examID = inputRecordIDCheck("请输入要查询的检查ID: ", {"exa"});

    Examination *current = exaHead;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->department == this->department && current->examinationID == examID)
        {
            printExaminationDetails(current);
            return true;
        }
        current = current->next;
    }

    printError("未找到检查ID为 " + examID + " 的检查记录。");
    return false;
}
// 根据检查时间范围查询检查记录
bool Nurse::getExaminationsByTimeRange(Examination *&exaHead)
{
    std::string startDate = inputDateCheck("请输入查询的开始日期: ");
    std::string endDate = inputDateCheck("请输入查询的结束日期: ");

    Examination *current = exaHead;
    bool found = false;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->department == this->department)
        {
            std::string examDate = current->orderTime.substr(0, 10);
            if (examDate >= startDate && examDate <= endDate)
            {
                printExaminationDetails(current);
                std::cout << std::endl;
                found = true;
            }
        }
        current = current->next;
    }

    if (!found)
    {
        printError("未找到指定时间范围内的检查记录。");
    }
    return found;
}
// 根据检查项目名称查询检查记录
bool Nurse::getExaminationsByItemName(Examination *&exaHead)
{
    std::string itemName = ExaminationItemMenu();
    if (itemName == "0")
    {
        printWarning("已取消查询操作！");
        return false;
    }

    Examination *current = exaHead;
    bool found = false;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->department == this->department && current->itemName == itemName)
        {
            printExaminationDetails(current);
            std::cout << std::endl;
            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        printError("未找到检查项目名称为 '" + itemName + "' 的检查记录。");
    }
    return found;
}
// 根据检查状态查询检查记录
bool Nurse::getExaminationsByStatus(Examination *&exaHead)
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("查询检查状态");
    printMenuBottom();
    printMenuItem(1, "已下单");
    printMenuItem(2, "已支付");
    printMenuItem(3, "检查中");
    printMenuItem(4, "检查完成");
    printMenuItem(5, "报告已出");
    printMenuItem(6, "已作废");
    printMenuItem(0, "取消查询");
    printMenuBottom();

    int statusChoice = selectIntCheck(0, 6);
    if (statusChoice == 0)
    {
        return false;
    }

    ExaminationStatus targetStatus = static_cast<ExaminationStatus>(statusChoice);

    Examination *current = exaHead;
    bool found = false;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->department == this->department && current->status == targetStatus)
        {
            printExaminationDetails(current);
            std::cout << std::endl;
            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        printError("未找到该状态下的检查记录。");
    }
    return found;
}

// 护士修改检查记录状态
void Nurse::setExaminationStatus(Examination *&target)
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("修改检查状态");
    printMenuLine("当前状态: " + examStatusToString(target->status));
    printMenuBottom();
    printMenuItem(1, "已下单");
    printMenuItem(2, "已支付");
    printMenuItem(3, "检查中");
    printMenuItem(4, "检查完成");
    printMenuItem(5, "报告已出");
    printMenuItem(6, "已作废");
    printMenuItem(0, "取消修改");
    printMenuBottom();

    int statusChoice = selectIntCheck(0, 6);
    if (statusChoice == 0)
    {
        printWarning("已取消修改操作！");
        return;
    }

    target->status = static_cast<ExaminationStatus>(statusChoice);
    printSuccess("检查状态已更新为: " + std::string(examStatusToString(target->status)));
}
// 护士录入/更新检查结果（体征信息）
void Nurse::setExaminationReport(Examination *&target)
{
    if (target->status == ExaminationStatus::ORDERED)
    {
        printError("检查尚未支付，无法录入检查结果！");
        return;
    }

    std::cout << "检查项目: " << target->itemName << std::endl;
    std::cout << "之前的检查结果: " << findVitalSignToString(target) << std::endl;

    std::cout << "正在进行检查..." << std::endl;

    target->status = ExaminationStatus::IN_PROGRESS; // 更新状态为检查中

    std::cout << "检查完成，正在录入检查结果..." << std::endl;

    setVitalSigns(target->vitalSigns, target->itemName);

    target->status = ExaminationStatus::COMPLETED; // 更新状态为检查完成

    target->reportTime = MyTime::getInstance().getTime();

    printSuccess("检查结果已更新！");
}
// 护士录入/更新检查报告摘要
void Nurse::setExaminationReportSummary(Examination *&target)
{
    if (target->status == ExaminationStatus::ORDERED)
    {
        printError("检查尚未支付，无法录入检查报告摘要！");
        return;
    }

    if (target->status == ExaminationStatus::IN_PROGRESS)
    {
        printError("检查尚未完成，无法录入检查报告摘要！");
        return;
    }

    std::cout << "当前检查报告摘要: " << target->reportSummary << std::endl;
    std::string newSummary = inputStringCheck("请输入新的检查报告摘要: ");
    target->reportSummary = newSummary;
    printSuccess("检查报告摘要已更新！");
}
// 护士管理检查记录相关附件
void Nurse::setExaminationAttachments(Examination *&target)
{

    while (true)
    {
        std::cout << "当前相关附件: ";
        if (target->attachments.empty())
        {
            std::cout << "无";
        }
        else
        {
            for (size_t i = 0; i < target->attachments.size(); ++i)
            {
                std::cout << "[" << (i + 1) << "] " << target->attachments[i] << "  ";
            }
        }
        std::cout << std::endl;

        clearScreen();
        printMenuBorder();
        printMenuTitle("附件管理");
        printMenuBottom();
        printMenuItem(1, "添加附件");
        printMenuItem(2, "删除附件");
        printMenuItem(0, "返回上级菜单");
        printMenuBottom();

        int op = selectIntCheck(0, 2);
        if (op == 0)
        {
            break;
        }
        else if (op == 1)
        {
            std::string newAttach = inputStringCheck("请输入要添加的附件名称/路径: ");
            bool exists = false;
            for (const auto &a : target->attachments)
            {
                if (a == newAttach)
                {
                    exists = true;
                    break;
                }
            }
            if (exists)
            {
                printWarning("该附件已存在，不能重复添加！");
            }
            else
            {
                target->attachments.push_back(newAttach);
                printSuccess("附件已添加！");
            }
        }
        else if (op == 2)
        {
            if (target->attachments.empty())
            {
                printWarning("当前没有附件可删除！");
                continue;
            }

            std::cout << "请输入要删除的附件编号: ";

            int idx = selectIntCheck(1, static_cast<int>(target->attachments.size()));
            target->attachments.erase(target->attachments.begin() + idx - 1);
            printSuccess("附件已删除！");
        }
    }
}
// 护士修改检查记录备注信息
void Nurse::setExaminationNote(Examination *&target)
{
    std::cout << "当前备注信息: " << target->note << std::endl;
    target->note = inputStringCheck("请输入新的备注信息: ");
    printSuccess("备注信息已更新！");
}
// 护士删除检查记录（逻辑删除）
void Nurse::deleteExamination(Examination *&target)
{
    target->isDeleted = true;
    printSuccess("检查记录已删除！");
}

void Nurse::manageExaminations(Examination *&exaHead)
{
    while (true)
    {
        int choice = nurseExaminationManagementMenu();

        if (choice == 0)
        {
            break;
        }
        else if (choice == 1)
        {
            while (true)
            {
                int viewChoice = nurseExaminationViewMenu();

                if (viewChoice == 0)
                    break;
                else if (viewChoice == 1)
                    getAllExaminations(exaHead);
                else if (viewChoice == 2)
                    getExaminationsByPatientID(exaHead);
                else if (viewChoice == 3)
                    getExaminationsByConsultationID(exaHead);
                else if (viewChoice == 4)
                    getExaminationsByID(exaHead);
                else if (viewChoice == 5)
                    getExaminationsByTimeRange(exaHead);
                else if (viewChoice == 6)
                    getExaminationsByItemName(exaHead);
                else if (viewChoice == 7)
                    getExaminationsByStatus(exaHead);

                pause("护士 > 检查管理");
            }
        }
        else if (choice == 2)
        {
            bool ishave = getAllExaminations(exaHead);
            if (!ishave)
            {
                pause("护士 > 检查管理");
                continue;
            }

            std::string examID = inputRecordIDCheck("请输入要修改的检查ID: ", {"exa"});
            Examination *target = exaHead;
            while (target)
            {
                if (!target->isDeleted && target->status != ExaminationStatus::VOIDED && target->department == this->department && target->examinationID == examID)
                {
                    break;
                }
                target = target->next;
            }

            if (!target)
            {
                printError("未找到指定检查记录！");
                pause("护士 > 检查管理");
                continue;
            }

            while (true)
            {
                std::cout << "正在修改的检查记录详情：" << std::endl;
                printExaminationDetails(target);

                int modifyChoice = nurseExaminationModificationMenu();

                if (modifyChoice == 0)
                    break;
                else if (modifyChoice == 1)
                    setExaminationStatus(target);
                else if (modifyChoice == 2)
                    setExaminationReport(target);
                else if (modifyChoice == 3)
                    setExaminationReportSummary(target);
                else if (modifyChoice == 4)
                    setExaminationAttachments(target);
                else if (modifyChoice == 5)
                    setExaminationNote(target);

                pause("护士 > 检查管理");
            }
        }
        else if (choice == 3)
        {
            bool ishave = getAllExaminations(exaHead);
            if (!ishave)
            {
                pause("护士 > 检查管理");
                continue;
            }

            std::string examID = inputRecordIDCheck("请输入要删除的检查ID: ", {"exa"});
            Examination *target = exaHead;
            while (target)
            {
                if (!target->isDeleted && target->department == this->department && target->examinationID == examID)
                {
                    break;
                }
                target = target->next;
            }

            if (!target)
            {
                printError("未找到指定检查记录！");
                pause("护士 > 检查管理");
                continue;
            }

            std::cout << "正在删除的检查记录详情：" << std::endl;
            printExaminationDetails(target);

            std::cout << "确认删除该检查记录吗？\n1. 确认\n0. 取消" << std::endl;
            int confirmChoice = selectIntCheck(0, 1);
            if (confirmChoice == 1)
            {
                deleteExamination(target);
            }
            else
            {
                printWarning("已取消删除操作！");
            }
            pause("护士 > 检查管理");
        }
    }
}

// ==================== 住院记录管理 ====================

// 辅助函数：打印住院记录详情
void Nurse::printHospitalizationDetails(Hospitalization *&hos)
{
    if (hos == nullptr)
    {
        printError("住院记录不存在。");
        return;
    }

    std::cout << "住院ID: " << hos->hospitalizationID
              << ", 看诊ID: " << hos->consultationID
              << ", 患者ID: " << hos->patientID
              << ", 医生ID: " << hos->doctorID
              << ", 护士ID: " << hos->nurseID
              << ", 科室: " << hos->department
              << ", 病房类型: " << hos->wardType
              << ", 床位号: " << hos->bedNumber
              << ", 申请时间: " << hos->applyTime
              << ", 允许入院时间: " << hos->availableAdmitTime
              << ", 实际入院时间: " << hos->admitTime
              << ", 出院时间: " << hos->dischargeTime
              << ", 押金: " << hos->deposit
              << ", 总费用: " << hos->totalCost
              << ", 状态: " << hosStatusToString(hos->status)
              << std::endl;
}
// 辅助函数：打印床位信息
void Nurse::printBedDetails(bedInfo *&bed)
{
    if (bed == nullptr)
    {
        printError("床位信息不存在。");
        return;
    }

    std::cout << "床位ID: " << bed->bedID
              << ", 科室: " << bed->department
              << ", 病房类型: " << bed->wardType
              << ", 区域号: " << bed->areaNumber
              << ", 病房号: " << bed->wardNumber
              << ", 床位号: " << bed->bedNumber
              << ", 患者ID:" << bed->patientID
              << ", 看护护士ID: " << bed->nurseID
              << ", 床位使用次数: " << bed->useTimes
              << ", 占用天数: " << bed->daysOccupied
              << ", 当前状态: " << bedStatusToString(bed->status)
              << std::endl;
}

// 查询本科室的所有住院记录
bool Nurse::getAllHospitalizations(Hospitalization *&hosHead)
{
    std::cout << "正在获取本科室住院记录..." << std::endl;
    Hospitalization *current = hosHead;
    bool found = false;

    while (current != nullptr)
    {
        if (!current->isDeleted && current->department == this->department)
        {
            printHospitalizationDetails(current);
            std::cout << std::endl;
            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        printError("未找到本科室住院记录。");
    }
    return found;
}
// 根据患者ID查询住院记录
bool Nurse::getHospitalizationsByPatientID(Hospitalization *&hosHead)
{
    std::string patientID = inputIDCheck("请输入要查询的患者ID: ");
    Hospitalization *current = hosHead;
    bool found = false;

    while (current != nullptr)
    {
        if (!current->isDeleted && current->department == this->department && current->patientID == patientID)
        {
            printHospitalizationDetails(current);
            std::cout << std::endl;
            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        printError("未找到该患者住院记录。");
    }
    return found;
}
// 根据看诊ID查询住院记录
bool Nurse::getHospitalizationsByConsultationID(Hospitalization *&hosHead)
{
    std::string conID = inputRecordIDCheck("请输入要查询的看诊ID: ", {"con"});
    Hospitalization *current = hosHead;
    bool found = false;

    while (current != nullptr)
    {
        if (!current->isDeleted && current->department == this->department && current->consultationID == conID)
        {
            printHospitalizationDetails(current);
            std::cout << std::endl;
            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        printError("未找到该看诊关联的住院记录。");
    }
    return found;
}
bool Nurse::getHospitalizationsByID(Hospitalization *&hosHead)
{
    std::string hosID = inputRecordIDCheck("请输入要查询的住院ID: ", {"hos"});
    Hospitalization *current = hosHead;

    while (current != nullptr)
    {
        if (!current->isDeleted && current->department == this->department && current->hospitalizationID == hosID)
        {
            printHospitalizationDetails(current);
            return true;
        }
        current = current->next;
    }

    printError("未找到该住院记录。");
    return false;
}
// 根据住院状态查询住院记录
bool Nurse::getHospitalizationsByStatus(Hospitalization *&hosHead)
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("查询住院状态");
    printMenuBottom();
    printMenuItem(1, "申请中");
    printMenuItem(2, "已缴费待分床");
    printMenuItem(3, "已入院");
    printMenuItem(4, "已出院");
    printMenuItem(5, "已作废");
    printMenuItem(0, "取消");
    printMenuBottom();

    int choice = selectIntCheck(0, 5);
    if (choice == 0)
    {
        return false;
    }

    HospitalizationStatus targetStatus = static_cast<HospitalizationStatus>(choice);
    Hospitalization *current = hosHead;
    bool found = false;

    while (current != nullptr)
    {
        if (!current->isDeleted && current->department == this->department && current->status == targetStatus)
        {
            printHospitalizationDetails(current);
            std::cout << std::endl;
            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        printError("未找到对应状态住院记录。");
    }
    return found;
}
// 根据入院时间范围查询住院记录
bool Nurse::getHospitalizationsByAdmitTimeRange(Hospitalization *&hosHead)
{
    std::string startDate = inputDateCheck("请输入要查询的入院开始日期: ");
    std::string endDate = inputDateCheck("请输入要查询的入院结束日期: ");

    Hospitalization *current = hosHead;
    bool found = false;

    while (current != nullptr)
    {
        if (!current->isDeleted && current->department == this->department)
        {
            if (current->admitTime == "#" || current->admitTime.length() < 10) { current = current->next; continue; }
            std::string admitDate = current->admitTime.substr(0, 10);
            if (admitDate >= startDate && admitDate <= endDate)
            {
                printHospitalizationDetails(current);
                std::cout << std::endl;
                found = true;
            }
        }
        current = current->next;
    }

    if (!found)
    {
        printError("未找到指定时间范围内住院记录。");
    }
    return found;
}
// 根据出院时间范围查询住院记录
bool Nurse::getHospitalizationsByDischargeTimeRange(Hospitalization *&hosHead)
{
    std::string startDate = inputDateCheck("请输入要查询的出院开始日期: ");
    std::string endDate = inputDateCheck("请输入要查询的出院结束日期: ");

    Hospitalization *current = hosHead;
    bool found = false;

    while (current != nullptr)
    {
        if (!current->isDeleted && current->department == this->department)
        {
            if (current->dischargeTime == "#" || current->dischargeTime.length() < 10) { current = current->next; continue; }
            std::string dischargeDate = current->dischargeTime.substr(0, 10);
            if (dischargeDate >= startDate && dischargeDate <= endDate)
            {
                printHospitalizationDetails(current);
                std::cout << std::endl;
                found = true;
            }
        }
        current = current->next;
    }

    if (!found)
    {
        printError("未找到指定时间范围内住院记录。");
    }
    return found;
}
// 根据病房类型查询住院记录
bool Nurse::getHospitalizationsByWardType(Hospitalization *&hosHead)
{
    std::string wardType = HospitalizationWardTypeMenu();

    if (wardType == "0")
    {
        printWarning("已取消查询操作！");
        return false;
    }

    Hospitalization *current = hosHead;
    bool found = false;

    while (current != nullptr)
    {
        if (!current->isDeleted && current->department == this->department && current->wardType == wardType)
        {
            printHospitalizationDetails(current);
            std::cout << std::endl;
            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        printError("未找到该病房类型的住院记录。");
    }
    return found;
}
// 根据医生ID查询住院记录
bool Nurse::getHospitalizationsByDoctorID(Hospitalization *&hosHead)
{
    std::string doctorID = inputIDCheck("请输入要查询的医生ID: ");
    Hospitalization *current = hosHead;
    bool found = false;

    while (current != nullptr)
    {
        if (!current->isDeleted && current->department == this->department && current->doctorID == doctorID)
        {
            printHospitalizationDetails(current);
            std::cout << std::endl;
            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        printError("未找到该医生关联的住院记录。");
    }
    return found;
}
// 根据床位号查询住院记录
bool Nurse::getHospitalizationsByBedNumber(Hospitalization *&hosHead)
{
    std::string wardtype = HospitalizationWardTypeMenu();
    if (wardtype == "0")
    {
        printWarning("已取消查询操作！");
        return false;
    }

    std::string bedNumber = inputBedNumberCheck("请填写要查询的床位信息: ", this->department, wardtype);
    Hospitalization *current = hosHead;
    bool found = false;

    while (current != nullptr)
    {
        if (!current->isDeleted && current->department == this->department && current->bedNumber == bedNumber)
        {
            printHospitalizationDetails(current);
            std::cout << std::endl;
            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        printError("未找到该床位号的住院记录。");
    }
    return found;
}

// 护士修改住院记录状态
void Nurse::setHospitalizationStatus(Hospitalization *&target)
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("修改住院状态");
    printMenuLine("当前状态: " + hosStatusToString(target->status));
    printMenuBottom();
    printMenuItem(1, "申请中");
    printMenuItem(2, "已缴费待分床");
    printMenuItem(3, "已入院");
    printMenuItem(4, "已出院");
    printMenuItem(5, "已作废");
    printMenuItem(0, "取消");
    printMenuBottom();

    int choice = selectIntCheck(0, 5);
    if (choice == 0)
    {
        printWarning("已取消修改！");
        return;
    }

    target->status = static_cast<HospitalizationStatus>(choice);
    printSuccess("住院状态已更新！");
}
// 护士分配自己负责的住院记录
void Nurse::assignNurseToHospitalization(Hospitalization *&target)
{
    target->nurseID = this->nurseID;
    printSuccess("已将当前护士分配到该住院记录。");
}
// 护士分配床位给住院患者
void Nurse::assignBed(Hospitalization *&target, bedInfo *&bedHead)
{
    if (target->status != HospitalizationStatus::PAID)
    {
        printError("当前住院记录未处于\"已缴费待分床\"状态，无法分床！");
        return;
    }

    std::cout << "正在查找可用床位..." << std::endl;
    bedInfo *current = bedHead;
    bool found = false;
    while (current != nullptr)
    {
        if (!current->isDeleted &&
            current->department == target->department &&
            current->wardType == target->wardType &&
            current->status == bedStatus::AVAILABLE)
        {
            printBedDetails(current);
            std::cout << std::endl;
            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        printError("未找到可用床位！");
        return;
    }

    std::string bedID = inputBedNumberCheck("请填写要分配的床位信息: ", this->department, target->wardType);

    current = bedHead;
    while (current != nullptr)
    {
        if (!current->isDeleted &&
            current->bedID == bedID &&
            current->department == target->department &&
            current->wardType == target->wardType &&
            current->status == bedStatus::AVAILABLE)
        {
            current->status = bedStatus::OCCUPIED;
            current->patientID = target->patientID;
            current->nurseID = this->nurseID;
            current->useTimes++;

            target->nurseID = this->nurseID;
            target->bedNumber = current->bedID;
            target->availableAdmitTime = MyTime::getInstance().getTime();
            target->status = HospitalizationStatus::ADMITTED;
            target->admitTime = target->availableAdmitTime;

            increaseBedManageCount();
            increasePatientCareCount();

            printSuccess("分配床位成功！");
            return;
        }
        current = current->next;
    }

    printError("指定床位不可用，分配失败！");
}
// 护士转床操作
void Nurse::transferBed(Hospitalization *&target, bedInfo *&bedHead)
{
    if (target->status != HospitalizationStatus::ADMITTED)
    {
        printError("当前患者未入院，无法转床！");
        return;
    }

    bedInfo *oldBed = nullptr;
    bedInfo *current = bedHead;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->bedID == target->bedNumber)
        {
            oldBed = current;
            break;
        }
        current = current->next;
    }

    if (oldBed == nullptr)
    {
        printError("未找到原床位信息，无法转床！");
        return;
    }

    std::cout << "当前床位: " << oldBed->bedID << std::endl;
    std::cout << "正在查找新的可用床位..." << std::endl;

    current = bedHead;
    bool found = false;
    while (current != nullptr)
    {
        if (!current->isDeleted &&
            current->department == target->department &&
            current->wardType == target->wardType &&
            current->status == bedStatus::AVAILABLE)
        {
            printBedDetails(current);
            std::cout << std::endl;
            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        printError("没有可转入的新床位！");
        return;
    }

    std::string newBedID = inputBedNumberCheck("请填写要转入的新床位信息: ", this->department, target->wardType);

    current = bedHead;
    while (current != nullptr)
    {
        if (!current->isDeleted &&
            current->bedID == newBedID &&
            current->department == target->department &&
            current->wardType == target->wardType &&
            current->status == bedStatus::AVAILABLE)
        {
            oldBed->status = bedStatus::CLEANING;
            oldBed->patientID = "#";
            oldBed->nurseID = "#";

            current->status = bedStatus::OCCUPIED;
            current->patientID = target->patientID;
            current->nurseID = this->nurseID;
            current->useTimes++;

            target->bedNumber = current->bedID;

            increaseBedManageCount();

            printSuccess("转床成功！");
            return;
        }
        current = current->next;
    }

    printError("新床位无效，转床失败！");
}
// 护士办理患者出院
void Nurse::dischargePatient(Hospitalization *&target, bedInfo *&bedHead)
{
    if (target->status != HospitalizationStatus::ADMITTED)
    {
        printError("当前住院记录未处于已入院状态，无法办理出院！");
        return;
    }

    bedInfo *current = bedHead;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->bedID == target->bedNumber)
        {
            current->status = bedStatus::CLEANING;
            current->patientID = "#";
            current->nurseID = "#";
            break;
        }
        current = current->next;
    }

    target->status = HospitalizationStatus::DISCHARGED;
    target->dischargeTime = MyTime::getInstance().getTime();

    increaseBedManageCount();

    printSuccess("患者已办理出院，床位已释放为清洁中状态！");
}
// 修改病房类型
void Nurse::setHospitalizationWardType(Hospitalization *&target)
{
    std::cout << "当前病房类型: " << target->wardType << std::endl;

    std::string newWardType = HospitalizationWardTypeMenu();
    if (newWardType == "0")
    {
        printWarning("已取消修改操作！");
        return;
    }

    target->wardType = newWardType;
    printSuccess("病房类型已更新！");
}
// 护士逻辑删除住院记录
void Nurse::deleteHospitalization(Hospitalization *&target)
{
    target->isDeleted = true;
    printSuccess("住院记录已逻辑删除！");
}
// 根据开具住院证的看诊信息创建住院记录
void Nurse::createHospitalization(Hospitalization *&hosHead, Consultation *&conHead, int &idCounter)
{

    std::cout << "正在查询当前科室中开具住院证的看诊记录..." << std::endl;
    Consultation *current = conHead;
    bool found = false;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->department == this->department && current->isHospitalizationRecommended)
        {
            std::cout << "看诊ID: " << current->consultationID
                      << ", 挂号ID: " << current->registrationID
                      << ", 患者ID: " << current->patientID
                      << ", 看诊时间: " << current->consultationTime
                      << ", 科室: " << current->department
                      << ", 主诉: " << current->chiefComplaint
                      << ", 现病史: " << current->historyOfPresentIllness
                      << ", 既往史: " << current->pastMedicalHistory
                      << ", 家族史: " << current->familyHistory
                      << ", 初步诊断: " << current->preliminaryDiagnosis
                      << ", 检查项目数: " << current->examinationlist.size()
                      << ", 处方数: " << current->prescriptions.size()
                      << ", 处方审核状态: " << (current->isPrescriptionReviewed ? "已审核" : "未审核")
                      << ", 住院建议: " << (current->isHospitalizationRecommended ? "是" : "否")
                      << ", 备注: " << current->note
                      << std::endl;
            std::cout << std::endl;
            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        printError("未找到开具住院证的看诊记录！无法创建住院记录");
        return;
    }

    std::string conID = inputRecordIDCheck("请输入要创建住院记录的看诊ID: ", {"con"});
    current = conHead;

    while (current)
    {
        if (!current->isDeleted && current->department == this->department && current->isHospitalizationRecommended && current->consultationID == conID)
        {
            break;
        }
        current = current->next;
    }

    if (!current)
    {
        printError("未找到指定看诊记录！无法创建住院记录");
        return;
    }

    Hospitalization *newHos = new Hospitalization();
    newHos->doctorID = current->doctorID;
    newHos->nurseID = this->nurseID;
    newHos->patientID = current->patientID;
    newHos->consultationID = current->consultationID;
    newHos->department = current->department;
    newHos->applyTime = MyTime::getInstance().getTime();
    newHos->hospitalizationID = "hos" + padId(idCounter, 6);
    idCounter++;
    newHos->availableAdmitTime = "#";
    newHos->admitTime = "#";

    std::string wardChoice = HospitalizationWardTypeMenu();
    if (wardChoice == "0") { delete newHos; return; }
    newHos->wardType = wardChoice;

    // 将新住院记录插入链表头部
    newHos->next = hosHead;
    if (hosHead)
    {
        hosHead->prev = newHos;
    }
    hosHead = newHos;
    printSuccess("住院记录创建成功！住院ID: " + newHos->hospitalizationID);
}
// 护士修改住院记录押金
void Nurse::setHospitalizationDeposit(Hospitalization *&target)
{
    std::cout << "当前押金: " << target->deposit << std::endl;
    double newDeposit = inputFeeCheck("请输入新的押金金额: ");
    target->deposit = newDeposit;
    printSuccess("押金已更新！当前押金: " + std::to_string(target->deposit));
}

// 护士管理住院记录主函数
void Nurse::manageHospitalizations(Hospitalization *&hosHead, Consultation *&conHead, bedInfo *&bedHead, int &idCounter)
{
    while (true)
    {

        int choice = nurseHospitalizationManagementMenu();

        if (choice == 0)
        {
            break;
        }
        else if (choice == 1)
        {
            while (true)
            {
                int viewChoice = nurseHospitalizationViewMenu();

                if (viewChoice == 0)
                    break;
                else if (viewChoice == 1)
                {
                    getAllHospitalizations(hosHead);
                }
                else if (viewChoice == 2)
                {
                    getHospitalizationsByID(hosHead);
                }
                else if (viewChoice == 3)
                {
                    getHospitalizationsByPatientID(hosHead);
                }
                else if (viewChoice == 4)
                {
                    getHospitalizationsByAdmitTimeRange(hosHead);
                }
                else if (viewChoice == 5)
                {
                    getHospitalizationsByDischargeTimeRange(hosHead);
                }
                else if (viewChoice == 6)
                {
                    getHospitalizationsByStatus(hosHead);
                }
                else if (viewChoice == 7)
                {
                    getHospitalizationsByWardType(hosHead);
                }
                else if (viewChoice == 8)
                {
                    getHospitalizationsByBedNumber(hosHead);
                }
                else if (viewChoice == 9)
                {
                    getHospitalizationsByDoctorID(hosHead);
                }
                else if (viewChoice == 10)
                {
                    getHospitalizationsByConsultationID(hosHead);
                }

                pause("护士 > 住院管理");
            }
        }
        else if (choice == 2)
        {
            bool ishave = getAllHospitalizations(hosHead);
            if (!ishave)
            {
                pause("护士 > 住院管理");
                continue;
            }

            std::string hosID = inputRecordIDCheck("请输入要修改的住院ID: ", {"hos"});
            Hospitalization *target = hosHead;
            while (target)
            {
                if (!target->isDeleted && target->department == this->department && target->hospitalizationID == hosID)
                {
                    break;
                }
                target = target->next;
            }

            if (!target)
            {
                printError("未找到指定住院记录！");
                pause("护士 > 住院管理");
                continue;
            }

            while (true)
            {
                std::cout << "正在修改的住院记录详情：" << std::endl;
                printHospitalizationDetails(target);

                int modifyChoice = nurseHospitalizationModificationMenu();

                if (modifyChoice == 0)
                    break;
                else if (modifyChoice == 1)
                    setHospitalizationStatus(target);
                else if (modifyChoice == 2)
                    setHospitalizationWardType(target);
                else if (modifyChoice == 3)
                    assignNurseToHospitalization(target);
                else if (modifyChoice == 4)
                    assignBed(target, bedHead);
                else if (modifyChoice == 5)
                    transferBed(target, bedHead);
                else if (modifyChoice == 6)
                    dischargePatient(target, bedHead);
                else if (modifyChoice == 7)
                    setHospitalizationDeposit(target);

                pause("护士 > 住院管理");
            }
        }
        else if (choice == 3)
        {
            bool ishave = getAllHospitalizations(hosHead);
            if (!ishave)
            {
                pause("护士 > 住院管理");
                continue;
            }

            std::string hosID = inputRecordIDCheck("请输入要删除的住院ID: ", {"hos"});
            Hospitalization *target = hosHead;
            while (target)
            {
                if (!target->isDeleted && target->department == this->department && target->hospitalizationID == hosID)
                {
                    break;
                }
                target = target->next;
            }

            if (!target)
            {
                printError("未找到指定住院记录！");
                pause("护士 > 住院管理");
                continue;
            }

            std::cout << "正在删除的住院记录详情：" << std::endl;
            printHospitalizationDetails(target);

            std::cout << "确认删除该住院记录吗？\n1. 确认\n0. 取消" << std::endl;
            int confirmChoice = selectIntCheck(0, 1);
            if (confirmChoice == 1)
            {
                deleteHospitalization(target);
            }
            else
            {
                printWarning("已取消删除操作！");
            }
            pause("护士 > 住院管理");
        }
        else if (choice == 4)
        {
            createHospitalization(hosHead, conHead, idCounter);
            pause("护士 > 住院管理");
        }
    }
}

// ==================== 床位信息管理 ====================

// 获取本科室的所有床位信息
bool Nurse::getAllBeds(bedInfo *&bedHead)
{
    bedInfo *current = bedHead;
    bool found = false;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->department == this->department)
        {
            printBedDetails(current);
            std::cout << std::endl;
            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        printError("未找到本科室床位信息。");
    }
    return found;
}
// 根据床位状态查询床位信息
bool Nurse::getBedsByStatus(bedInfo *&bedHead)
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("查询床位状态");
    printMenuBottom();
    printMenuItem(1, "已占用");
    printMenuItem(2, "清洁中");
    printMenuItem(3, "可分配");
    printMenuItem(4, "不可用");
    printMenuItem(0, "取消");
    printMenuBottom();

    int choice = selectIntCheck(0, 4);
    if (choice == 0)
        return false;

    bedStatus targetStatus = static_cast<bedStatus>(choice);

    bedInfo *current = bedHead;
    bool found = false;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->department == this->department && current->status == targetStatus)
        {
            printBedDetails(current);
            std::cout << std::endl;
            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        printError("未找到该状态床位。");
    }
    return found;
}
// 根据患者ID查询床位信息
bool Nurse::getBedsByPatientID(bedInfo *&bedHead)
{
    std::string patientID = inputIDCheck("请输入患者ID: ");
    bedInfo *current = bedHead;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->department == this->department && current->patientID == patientID)
        {
            printBedDetails(current);
            return true;
        }
        current = current->next;
    }
    printError("未找到该患者对应床位。");
    return false;
}
// 根据护士ID查询床位信息
bool Nurse::getBedsByNurseID(bedInfo *&bedHead)
{
    std::string nurseID = inputIDCheck("请输入护士ID: ");
    bedInfo *current = bedHead;
    bool found = false;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->department == this->department && current->nurseID == nurseID)
        {
            printBedDetails(current);
            std::cout << std::endl;
            found = true;
        }
        current = current->next;
    }
    if (!found)
    {
        printError("未找到该护士负责的床位。");
    }
    return found;
}
// 根据科室查询床位信息
bool Nurse::getBedsByDepartment(bedInfo *&bedHead)
{
    std::string dept = inputDepartmentCheck("请输入科室名称: ");

    if(dept == "#"){
        printWarning("已取消查询操作！");
        return false;
    }

    bedInfo *current = bedHead;
    bool found = false;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->department == dept)
        {
            printBedDetails(current);
            std::cout << std::endl;
            found = true;
        }
        current = current->next;
    }
    if (!found)
    {
        printError("未找到该科室床位。");
    }
    return found;
}
// 根据床位ID查询床位信息
bool Nurse::getBedByID(bedInfo *&bedHead)
{
    std::string dept = inputDepartmentCheck("请输入科室名称: ");

    if(dept == "#"){
        printWarning("已取消查询操作！");
        return false;
    }

    std::string wardtype = HospitalizationWardTypeMenu();
    if(wardtype == "0"){
        printWarning("已取消查询操作！");
        return false;
    }

    std::string bedID = inputBedNumberCheck("请填写要查找的床位信息: ", dept, wardtype);
    bedInfo *current = bedHead;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->bedID == bedID)
        {
            printBedDetails(current);
            return true;
        }
        current = current->next;
    }
    printError("未找到该床位。");
    return false;
}
// 根据病房类型查询床位信息
bool Nurse::getBedsByWardType(bedInfo *&bedHead)
{
    std::string wardType = HospitalizationWardTypeMenu();

    if (wardType == "0")
    {
        printWarning("已取消查询操作！");
        return false;
    }

    bedInfo *current = bedHead;
    bool found = false;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->department == this->department && current->wardType == wardType)
        {
            printBedDetails(current);
            std::cout << std::endl;
            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        printError("未找到该病房类型的床位。");
    }
    return found;
}

// 护士修改床位状态
void Nurse::setBedStatus(bedInfo *&target)
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("修改床位状态");
    printMenuLine("当前状态: " + bedStatusToString(target->status));
    printMenuBottom();
    printMenuItem(1, "已占用");
    printMenuItem(2, "清洁中");
    printMenuItem(3, "可分配");
    printMenuItem(4, "不可用");
    printMenuItem(0, "取消");
    printMenuBottom();

    int choice = selectIntCheck(0, 4);

    if (choice == 0)
    {
        printWarning("已取消修改！");
        return;
    }

    target->status = static_cast<bedStatus>(choice);
    printSuccess("床位状态已更新！");
}
// 护士修改床位备注
void Nurse::setBedNote(bedInfo *&target)
{
    std::cout << "当前备注: " << target->note << std::endl;
    target->note = inputStringCheck("请输入新的备注信息: ");
    printSuccess("床位备注已更新！");
}
// 护士修改床位关联患者生命体征
void Nurse::setBedVitalSigns(bedInfo *&target)
{
    std::string itemName = ExaminationItemMenu();

    if (itemName == "0")    {
        printWarning("已取消修改操作！");
        return;
    }

    std::cout << "开始录入床位关联患者的生命体征..." << std::endl;

    setVitalSigns(target->vitalSigns, itemName);

    printSuccess("生命体征已更新！");
}
// 护士逻辑删除床位信息
void Nurse::deleteBed(bedInfo *&target)
{
    target->isDeleted = true;
    printSuccess("床位信息已逻辑删除！");
}
// 护士创建床位信息
bool Nurse::createBed(bedInfo *&bedHead, int &idCounter)
{
    bedInfo *newBed = new bedInfo();

    newBed->department = inputDepartmentCheck("请输入床位所属科室，输入 \" # \" 取消创建: ");

    if(newBed->department == "#"){
        printWarning("已取消创建操作！");
        delete newBed;
        return false;
    }

    newBed->wardType = HospitalizationWardTypeMenu();

    if(newBed->wardType == "0"){
        printWarning("已取消创建操作！");
        delete newBed;
        return false;
    }

    newBed->areaNumber = inputIntCheck("请输入新添加病房所在的区域号(0-10)：", 0, 10); // 输入区域号并检查格式，假设区域号在0-10之间

    newBed->wardNumber = inputIntCheck("请输入新添加病房的病房号(0-100)：", 0, 100); // 输入病房号并检查格式，假设病房号在0-100之间

    newBed->bedNumber = inputIntCheck("请输入新添加病房的床位号(0-10)：", 0, 10); // 输入床位号并检查格式，假设床位号在0-10之间

    newBed->bedID = autoGenerateBedID(newBed->department, newBed->wardType, newBed->areaNumber, newBed->wardNumber, newBed->bedNumber); // 自动生成床位ID

    bedInfo *current = bedHead;
    // 检查是否有重复的床位ID
    while (current != nullptr)
    {
        if (!current->isDeleted && current->bedID == newBed->bedID && current->department == newBed->department)
        {


            printError("床位ID " + newBed->bedID + " 已存在！无法添加床位信息。");
            delete newBed; // 释放内存
            return false;
        }
        current = current->next;
    }

    newBed->note = inputStringCheck("请输入备注信息: ");

    newBed->next = bedHead;
    if (bedHead)
        bedHead->prev = newBed;
    bedHead = newBed;

    printSuccess("床位创建成功！床位ID: " + newBed->bedID);
    return true;
}

void Nurse::manageBeds(bedInfo *&bedHead, int &idCounter)
{
    while (true)
    {
        int choice = nurseBedManagementMenu();

        if (choice == 0)
        {
            break;
        }
        else if (choice == 1)
        {
            while (true)
            {
                int viewChoice = nurseBedViewMenu();

                if (viewChoice == 0)
                    break;
                else if (viewChoice == 1)
                    getAllBeds(bedHead);
                else if (viewChoice == 2)
                    getBedsByStatus(bedHead);
                else if (viewChoice == 3)
                    getBedsByPatientID(bedHead);
                else if (viewChoice == 4)
                    getBedsByNurseID(bedHead);
                else if (viewChoice == 5)
                    getBedsByDepartment(bedHead);
                else if (viewChoice == 6)
                    getBedByID(bedHead);
                else if (viewChoice == 7)
                    getBedsByWardType(bedHead);

                pause("护士 > 床位管理");
            }
        }
        else if (choice == 2)
        {
            bool ishave = getAllBeds(bedHead);
            if (!ishave)
            {
                pause("护士 > 床位管理");
                continue;
            }

            std::string bedID = inputStringCheck("请输入要修改的床位ID: ");
            bedInfo *target = bedHead;
            while (target)
            {
                if (!target->isDeleted && target->bedID == bedID && target->department == this->department)
                {
                    break;
                }
                target = target->next;
            }

            if (!target)
            {
                printError("未找到指定床位！");
                pause("护士 > 床位管理");
                continue;
            }

            while (true)
            {
                int modifyChoice = nurseBedModificationMenu();

                if (modifyChoice == 0)
                    break;
                else if (modifyChoice == 1)
                    setBedStatus(target);
                else if (modifyChoice == 2)
                    setBedNote(target);
                else if (modifyChoice == 3)
                    setBedVitalSigns(target);

                pause("护士 > 床位管理");
            }
        }
        else if (choice == 3)
        {
            bool ishave = getAllBeds(bedHead);
            if (!ishave)
            {
                pause("护士 > 床位管理");
                continue;
            }

            std::string bedID = inputStringCheck("请输入要删除的床位ID: ");
            bedInfo *target = bedHead;
            while (target)
            {
                if (!target->isDeleted && target->bedID == bedID && target->department == this->department)
                {
                    break;
                }
                target = target->next;
            }

            if (!target)
            {
                printError("未找到指定床位！");
                pause("护士 > 床位管理");
                continue;
            }

            if(target->status == bedStatus::OCCUPIED){
                printError("该床位当前处于占用状态，无法删除！");
                pause("护士 > 床位管理");
                continue;
            }

            std::cout << "正在删除的床位详情：" << std::endl;
            printBedDetails(target);

            std::cout << "确认删除该床位吗？\n1. 确认\n0. 取消" << std::endl;
            int confirmChoice = selectIntCheck(0, 1);
            if (confirmChoice == 1)
            {
                deleteBed(target);
            }
            else
            {
                printWarning("已取消删除操作！");
            }
            pause("护士 > 床位管理");
        }
        else if (choice == 4)
        {
            createBed(bedHead, idCounter);
            pause("护士 > 床位管理");
        }
    }
}

// ==================== 个人信息管理 ====================

void Nurse::managePersonalInfo()
{
    while (true)
    {

        int choice = nursePersonalInfoManagementMenu();

        if (choice == 0)
        {
            break;
        }
        else if (choice == 1)
        {
            printInfoCard("护士个人信息", {
                {"护士ID", nurseID}, {"姓名", username}, {"性别", gender}, {"年龄", std::to_string(age)}, {"科室", department}, {"护士等级", nurseLevelToString(level)}, {"联系电话", telephone}, {"邮箱", email}, {"在岗状态", isOnDuty ? "在岗" : "不在岗"}, {"排班信息", scheduleInfo}, {"累计护理人数", std::to_string(patientCareCount)}, {"累计床位管理次数", std::to_string(bedManageCount)}, {"账户创建时间", createTime}
            });
            pause("护士 > 个人信息管理");
        }
        else if (choice == 2)
        {
            while (true)
            {
                int modifyChoice = nursePersonalInfoModificationMenu();

                if (modifyChoice == 0)
                {
                    break;
                }
                else if (modifyChoice == 1)
                {
                    std::cout << "当前姓名: " << this->username << std::endl;
                    this->username = inputStringCheck("请输入新的姓名: ");
                    printSuccess("姓名已更新！");
                }
                else if (modifyChoice == 2)
                {
                    std::cout << "当前性别: " << this->gender << std::endl;
                    this->gender = inputGenderCheck("请输入新的性别: ");
                    printSuccess("性别已更新！");
                }
                else if (modifyChoice == 3)
                {
                    std::cout << "当前年龄: " << this->age << std::endl;
                    this->age = inputAgeCheck("请输入新的年龄: ");
                    printSuccess("年龄已更新！");
                }
                else if (modifyChoice == 4)
                {
                    std::cout << "当前科室: " << this->department << std::endl;
                    this->department = inputDepartmentCheck("请输入新的科室: ");
                    printSuccess("科室已更新！");
                }
                else if (modifyChoice == 5)
                {
                    clearScreen();
                    printMenuBorder();
                    printMenuTitle("修改护士等级");
                    printMenuLine("当前等级: " + nurseLevelToString(this->level));
                    printMenuBottom();
                    printMenuItem(1, "实习护士");
                    printMenuItem(2, "初级护士");
                    printMenuItem(3, "高级护士");
                    printMenuItem(4, "护士长");
                    printMenuItem(0, "取消");
                    printMenuBottom();

                    int levelChoice = selectIntCheck(0, 4);

                    if (levelChoice == 0)
                    {
                        printWarning("已取消修改操作！");
                        pause("护士 > 个人信息管理");
                        continue;
                    }

                    this->level = static_cast<NurseLevel>(levelChoice);
                    printSuccess("护士等级已更新！");
                }
                else if (modifyChoice == 6)
                {
                    std::cout << "当前联系电话: " << this->telephone << std::endl;
                    this->telephone = inputTelephoneCheck("请输入新的联系电话: ");
                    printSuccess("联系电话已更新！");
                }
                else if (modifyChoice == 7)
                {
                    std::cout << "当前邮箱: " << this->email << std::endl;
                    this->email = inputEmailCheck("请输入新的邮箱: ");
                    printSuccess("邮箱已更新！");
                }
                else if (modifyChoice == 8)
                {
                    clearScreen();
                    printMenuBorder();
                    printMenuTitle("修改在岗状态");
                    printMenuLine("当前状态: " + std::string(this->isOnDuty ? "在岗" : "不在岗"));
                    printMenuBottom();
                    printMenuItem(1, "在岗");
                    printMenuItem(2, "不在岗");
                    printMenuItem(0, "取消");
                    printMenuBottom();

                    int dutyChoice = selectIntCheck(0, 2);

                    if (dutyChoice == 0)
                    {
                        printWarning("已取消修改操作！");
                        pause("护士 > 个人信息管理");
                        continue;
                    }

                    this->isOnDuty = (dutyChoice == 1);
                    printSuccess("在岗状态已更新！");
                }
                else if (modifyChoice == 9)
                {
                    std::cout << "当前排班信息: " << this->scheduleInfo << std::endl;
                    this->scheduleInfo = inputStringCheck("请输入新的排班信息: ");
                    printSuccess("排班信息已更新！");
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
                            printSuccess("密码更新成功！");
                        }
                        else
                        {
                            printError("两次输入的新密码不一致，密码更新失败！");
                        }
                    }
                    else
                    {
                        printError("密码验证失败，无法修改密码！");
                    }
                }

                pause("护士 > 个人信息管理");
            }
        }
    }
}