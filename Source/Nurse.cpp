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
        std::cout << "护士注册失败！" << std::endl;
        return false;
    }

    this->nurseID = this->userID;
    this->department = inputDepartmentCheck("请输入所属科室: ");

    std::cout << "请选择护士等级:" << std::endl;
    std::cout << "1. 实习护士" << std::endl;
    std::cout << "2. 初级护士" << std::endl;
    std::cout << "3. 高级护士" << std::endl;
    std::cout << "4. 护士长" << std::endl;
    int levelChoice = selectIntCheck(1, 4);
    this->level = static_cast<NurseLevel>(levelChoice);

    this->scheduleInfo = inputStringCheck("请输入排班信息: ");

    std::cout << "请输入你现在是否在岗 (1-是, 0-否): " << std::endl;
    int onDutyChoice = selectIntCheck(0, 1);
    this->isOnDuty = (onDutyChoice == 1);

    std::cout << "护士注册成功! 您的用户ID是: " << this->userID << std::endl;
    return true;
}

bool Nurse::nurseSignIn()
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
            std::cout << "护士登录成功！" << std::endl;
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

bool Nurse::getAllExaminations(Examination *&exaHead)
{
    std::cout << "正在获取本科室的所有检查记录..." << std::endl;

    Examination *current = exaHead;
    bool found = false;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->department == this->department)
        {
            std::cout << "检查ID: " << current->examinationID
                      << ", 看诊ID: " << current->consultationID
                      << ", 患者ID: " << current->patientID
                      << ", 医生ID: " << current->doctorID
                      << ", 科室: " << current->department
                      << ", 开单时间: " << current->orderTime
                      << ", 检查项目: " << current->itemName
                      << ", 检查结果: " << findVitalSignToString(current)
                      << ", 报告摘要: " << current->reportSummary
                      << ", 检查费用: " << current->fee
                      << ", 出报告时间: " << current->reportTime
                      << ", 状态: " << examStatusToString(current->status)
                      << ", 备注: " << current->note;

            std::cout << ", 相关附件: ";
            for (size_t i = 0; i < current->attachments.size(); ++i)
            {
                std::cout << "[" << (i + 1) << "] " << current->attachments[i] << "  ";
            }
            std::cout << std::endl;
            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到本科室的检查记录。" << std::endl;
    }
    return found;
}

bool Nurse::getExaminationsByPatientID(Examination *&exaHead)
{
    std::string patientID = inputRecordIDCheck("请输入要查询的患者ID: ", {"pat"});

    Examination *current = exaHead;
    bool found = false;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->department == this->department && current->patientID == patientID)
        {
            std::cout << "检查ID: " << current->examinationID
                      << ", 看诊ID: " << current->consultationID
                      << ", 患者ID: " << current->patientID
                      << ", 医生ID: " << current->doctorID
                      << ", 科室: " << current->department
                      << ", 开单时间: " << current->orderTime
                      << ", 检查项目: " << current->itemName
                      << ", 检查结果: " << findVitalSignToString(current)
                      << ", 报告摘要: " << current->reportSummary
                      << ", 检查费用: " << current->fee
                      << ", 出报告时间: " << current->reportTime
                      << ", 状态: " << examStatusToString(current->status)
                      << ", 备注: " << current->note
                      << std::endl;
            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "没有找到患者ID为 " << patientID << " 的检查记录。" << std::endl;
    }
    return found;
}

bool Nurse::getExaminationsByConsultationID(Examination *&exaHead)
{
    std::string conID = inputRecordIDCheck("请输入要查询的看诊ID: ", {"con"});

    Examination *current = exaHead;
    bool found = false;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->department == this->department && current->consultationID == conID)
        {
            std::cout << "检查ID: " << current->examinationID
                      << ", 看诊ID: " << current->consultationID
                      << ", 患者ID: " << current->patientID
                      << ", 医生ID: " << current->doctorID
                      << ", 科室: " << current->department
                      << ", 开单时间: " << current->orderTime
                      << ", 检查项目: " << current->itemName
                      << ", 检查结果: " << findVitalSignToString(current)
                      << ", 报告摘要: " << current->reportSummary
                      << ", 检查费用: " << current->fee
                      << ", 出报告时间: " << current->reportTime
                      << ", 状态: " << examStatusToString(current->status)
                      << ", 备注: " << current->note
                      << std::endl;
            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到看诊ID为 " << conID << " 的检查记录。" << std::endl;
    }
    return found;
}

bool Nurse::getExaminationsByID(Examination *&exaHead)
{
    std::string examID = inputRecordIDCheck("请输入要查询的检查ID: ", {"exa"});

    Examination *current = exaHead;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->department == this->department && current->examinationID == examID)
        {
            std::cout << "检查ID: " << current->examinationID
                      << ", 看诊ID: " << current->consultationID
                      << ", 患者ID: " << current->patientID
                      << ", 医生ID: " << current->doctorID
                      << ", 科室: " << current->department
                      << ", 开单时间: " << current->orderTime
                      << ", 检查项目: " << current->itemName
                      << ", 检查结果: " << findVitalSignToString(current)
                      << ", 报告摘要: " << current->reportSummary
                      << ", 检查费用: " << current->fee
                      << ", 出报告时间: " << current->reportTime
                      << ", 状态: " << examStatusToString(current->status)
                      << ", 备注: " << current->note
                      << std::endl;
            return true;
        }
        current = current->next;
    }

    std::cout << "未找到检查ID为 " << examID << " 的检查记录。" << std::endl;
    return false;
}

bool Nurse::getExaminationsByTimeRange(Examination *&exaHead)
{
    std::string startDate = inputDateCheck("请输入查询的开始日期 (格式 YYYY-MM-DD): ");
    std::string endDate = inputDateCheck("请输入查询的结束日期 (格式 YYYY-MM-DD): ");

    Examination *current = exaHead;
    bool found = false;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->department == this->department)
        {
            std::string examDate = current->orderTime.substr(0, 10);
            if (examDate >= startDate && examDate <= endDate)
            {
                std::cout << "检查ID: " << current->examinationID
                          << ", 看诊ID: " << current->consultationID
                          << ", 患者ID: " << current->patientID
                          << ", 医生ID: " << current->doctorID
                          << ", 科室: " << current->department
                          << ", 开单时间: " << current->orderTime
                          << ", 检查项目: " << current->itemName
                          << ", 检查结果: " << findVitalSignToString(current)
                          << ", 报告摘要: " << current->reportSummary
                          << ", 检查费用: " << current->fee
                          << ", 出报告时间: " << current->reportTime
                          << ", 状态: " << examStatusToString(current->status)
                          << ", 备注: " << current->note
                          << std::endl;
                found = true;
            }
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到指定时间范围内的检查记录。" << std::endl;
    }
    return found;
}

bool Nurse::getExaminationsByItemName(Examination *&exaHead)
{
    std::string itemName = ExaminationItemMenu();
    if (itemName == "0")
    {
        std::cout << "已取消查询操作！" << std::endl;
        return false;
    }

    Examination *current = exaHead;
    bool found = false;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->department == this->department && current->itemName == itemName)
        {
            std::cout << "检查ID: " << current->examinationID
                      << ", 看诊ID: " << current->consultationID
                      << ", 患者ID: " << current->patientID
                      << ", 医生ID: " << current->doctorID
                      << ", 科室: " << current->department
                      << ", 开单时间: " << current->orderTime
                      << ", 检查项目: " << current->itemName
                      << ", 检查结果: " << findVitalSignToString(current)
                      << ", 报告摘要: " << current->reportSummary
                      << ", 检查费用: " << current->fee
                      << ", 出报告时间: " << current->reportTime
                      << ", 状态: " << examStatusToString(current->status)
                      << ", 备注: " << current->note
                      << std::endl;
            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到检查项目名称为 '" << itemName << "' 的检查记录。" << std::endl;
    }
    return found;
}

bool Nurse::getExaminationsByStatus(Examination *&exaHead)
{
    std::cout << "请选择要查询的检查状态：" << std::endl;
    std::cout << "1. 已下单" << std::endl;
    std::cout << "2. 已支付" << std::endl;
    std::cout << "3. 检查中" << std::endl;
    std::cout << "4. 检查完成" << std::endl;
    std::cout << "5. 报告已出" << std::endl;
    std::cout << "6. 已作废" << std::endl;
    std::cout << "0. 取消查询" << std::endl;

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
            std::cout << "检查ID: " << current->examinationID
                      << ", 看诊ID: " << current->consultationID
                      << ", 患者ID: " << current->patientID
                      << ", 医生ID: " << current->doctorID
                      << ", 科室: " << current->department
                      << ", 开单时间: " << current->orderTime
                      << ", 检查项目: " << current->itemName
                      << ", 检查结果: " << findVitalSignToString(current)
                      << ", 报告摘要: " << current->reportSummary
                      << ", 检查费用: " << current->fee
                      << ", 出报告时间: " << current->reportTime
                      << ", 状态: " << examStatusToString(current->status)
                      << ", 备注: " << current->note
                      << std::endl;
            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到该状态下的检查记录。" << std::endl;
    }
    return found;
}

void Nurse::setExaminationStatus(Examination *&target)
{
    std::cout << "当前检查状态: " << examStatusToString(target->status) << std::endl;
    std::cout << "请选择新的检查状态：" << std::endl;
    std::cout << "1. 已下单" << std::endl;
    std::cout << "2. 已支付" << std::endl;
    std::cout << "3. 检查中" << std::endl;
    std::cout << "4. 检查完成" << std::endl;
    std::cout << "5. 报告已出" << std::endl;
    std::cout << "6. 已作废" << std::endl;
    std::cout << "0. 取消修改" << std::endl;

    int statusChoice = selectIntCheck(0, 6);
    if (statusChoice == 0)
    {
        std::cout << "已取消修改操作！" << std::endl;
        return;
    }

    target->status = static_cast<ExaminationStatus>(statusChoice);
    std::cout << "检查状态已更新为: " << examStatusToString(target->status) << std::endl;
}

void Nurse::setExaminationReport(Examination *&target)
{
    std::cout << "当前检查项目: " << target->itemName << std::endl;
    std::cout << "当前检查结果: " << findVitalSignToString(target) << std::endl;

    setVitalSigns(target->vitalSigns, target->itemName);

    if (target->status == ExaminationStatus::PAID)
    {
        target->status = ExaminationStatus::IN_PROGRESS;
    }

    std::cout << "检查结果已更新！" << std::endl;
}

void Nurse::setExaminationReportSummary(Examination *&target)
{
    std::cout << "当前检查报告摘要: " << target->reportSummary << std::endl;
    std::string newSummary = inputStringCheck("请输入新的检查报告摘要: ");
    target->reportSummary = newSummary;
    std::cout << "检查报告摘要已更新！" << std::endl;
}

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

        std::cout << "请选择操作:" << std::endl;
        std::cout << "1. 添加附件" << std::endl;
        std::cout << "2. 删除附件" << std::endl;
        std::cout << "0. 返回上级菜单" << std::endl;

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
                std::cout << "该附件已存在，不能重复添加！" << std::endl;
            }
            else
            {
                target->attachments.push_back(newAttach);
                std::cout << "附件已添加！" << std::endl;
            }
        }
        else if (op == 2)
        {
            if (target->attachments.empty())
            {
                std::cout << "当前没有附件可删除！" << std::endl;
                continue;
            }
            int idx = selectIntCheck(1, static_cast<int>(target->attachments.size()));
            target->attachments.erase(target->attachments.begin() + idx - 1);
            std::cout << "附件已删除！" << std::endl;
        }
    }
}

void Nurse::setExaminationNote(Examination *&target)
{
    std::cout << "当前备注信息: " << target->note << std::endl;
    target->note = inputStringCheck("请输入新的备注信息: ");
    std::cout << "备注信息已更新！" << std::endl;
}

void Nurse::deleteExamination(Examination *&target)
{
    target->isDeleted = true;
    std::cout << "检查记录已删除！" << std::endl;
}

void Nurse::manageExaminations(Examination *&exaHead)
{
    while (true)
    {
        std::cout << "\n========== 护士-检查记录管理 ==========" << std::endl;
        std::cout << "1. 查看检查记录" << std::endl;
        std::cout << "2. 修改检查记录" << std::endl;
        std::cout << "3. 删除检查记录" << std::endl;
        std::cout << "0. 返回上一级" << std::endl;

        int choice = selectIntCheck(0, 3);

        if (choice == 0)
        {
            break;
        }
        else if (choice == 1)
        {
            while (true)
            {
                std::cout << "\n------ 查看检查记录 ------" << std::endl;
                std::cout << "1. 查看本科室全部检查记录" << std::endl;
                std::cout << "2. 按患者ID查询" << std::endl;
                std::cout << "3. 按看诊ID查询" << std::endl;
                std::cout << "4. 按检查ID查询" << std::endl;
                std::cout << "5. 按时间范围查询" << std::endl;
                std::cout << "6. 按检查项目查询" << std::endl;
                std::cout << "7. 按状态查询" << std::endl;
                std::cout << "0. 返回上一级" << std::endl;

                int viewChoice = selectIntCheck(0, 7);

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

                pause();
            }
        }
        else if (choice == 2)
        {
            bool ishave = getAllExaminations(exaHead);
            if (!ishave)
            {
                pause();
                continue;
            }

            std::string examID = inputRecordIDCheck("请输入要修改的检查ID: ", {"exa"});
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
                std::cout << "未找到指定检查记录！" << std::endl;
                pause();
                continue;
            }

            while (true)
            {
                std::cout << "\n------ 修改检查记录 ------" << std::endl;
                std::cout << "1. 修改检查状态" << std::endl;
                std::cout << "2. 录入/修改检查结果" << std::endl;
                std::cout << "3. 修改报告摘要" << std::endl;
                std::cout << "4. 管理附件" << std::endl;
                std::cout << "5. 修改备注" << std::endl;
                std::cout << "0. 返回上一级" << std::endl;

                int modifyChoice = selectIntCheck(0, 5);

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

                pause();
            }
        }
        else if (choice == 3)
        {
            bool ishave = getAllExaminations(exaHead);
            if (!ishave)
            {
                pause();
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
                std::cout << "未找到指定检查记录！" << std::endl;
                pause();
                continue;
            }

            std::cout << "确认删除该检查记录吗？\n1. 确认\n0. 取消" << std::endl;
            int confirmChoice = selectIntCheck(0, 1);
            if (confirmChoice == 1)
            {
                deleteExamination(target);
            }
            else
            {
                std::cout << "已取消删除操作！" << std::endl;
            }
            pause();
        }
    }
}

// ==================== 住院记录管理 ====================

bool Nurse::getAllHospitalizations(Hospitalization *&hosHead)
{
    std::cout << "正在获取本科室住院记录..." << std::endl;
    Hospitalization *current = hosHead;
    bool found = false;

    while (current != nullptr)
    {
        if (!current->isDeleted && current->department == this->department)
        {
            std::cout << "住院ID: " << current->hospitalizationID
                      << ", 看诊ID: " << current->consultationID
                      << ", 患者ID: " << current->patientID
                      << ", 医生ID: " << current->doctorID
                      << ", 护士ID: " << current->nurseID
                      << ", 科室: " << current->department
                      << ", 病房类型: " << current->wardType
                      << ", 床位号: " << current->bedNumber
                      << ", 申请时间: " << current->applyTime
                      << ", 可入院时间: " << current->availableAdmitTime
                      << ", 入院时间: " << current->admitTime
                      << ", 出院时间: " << current->dischargeTime
                      << ", 押金: " << current->deposit
                      << ", 总费用: " << current->totalCost
                      << ", 状态: " << hosStatusToString(current->status)
                      << std::endl;
            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到本科室住院记录。" << std::endl;
    }
    return found;
}

bool Nurse::getHospitalizationsByPatientID(Hospitalization *&hosHead)
{
    std::string patientID = inputRecordIDCheck("请输入要查询的患者ID: ", {"pat"});
    Hospitalization *current = hosHead;
    bool found = false;

    while (current != nullptr)
    {
        if (!current->isDeleted && current->department == this->department && current->patientID == patientID)
        {
            std::cout << "住院ID: " << current->hospitalizationID
                      << ", 患者ID: " << current->patientID
                      << ", 病房类型: " << current->wardType
                      << ", 床位号: " << current->bedNumber
                      << ", 状态: " << hosStatusToString(current->status)
                      << std::endl;
            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到该患者住院记录。" << std::endl;
    }
    return found;
}

bool Nurse::getHospitalizationsByConsultationID(Hospitalization *&hosHead)
{
    std::string conID = inputRecordIDCheck("请输入要查询的看诊ID: ", {"con"});
    Hospitalization *current = hosHead;
    bool found = false;

    while (current != nullptr)
    {
        if (!current->isDeleted && current->department == this->department && current->consultationID == conID)
        {
            std::cout << "住院ID: " << current->hospitalizationID
                      << ", 看诊ID: " << current->consultationID
                      << ", 患者ID: " << current->patientID
                      << ", 状态: " << hosStatusToString(current->status)
                      << std::endl;
            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到该看诊关联的住院记录。" << std::endl;
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
            std::cout << "住院ID: " << current->hospitalizationID
                      << ", 患者ID: " << current->patientID
                      << ", 护士ID: " << current->nurseID
                      << ", 病房类型: " << current->wardType
                      << ", 床位号: " << current->bedNumber
                      << ", 状态: " << hosStatusToString(current->status)
                      << std::endl;
            return true;
        }
        current = current->next;
    }

    std::cout << "未找到该住院记录。" << std::endl;
    return false;
}

bool Nurse::getHospitalizationsByStatus(Hospitalization *&hosHead)
{
    std::cout << "请选择住院状态：" << std::endl;
    std::cout << "1. 申请中" << std::endl;
    std::cout << "2. 已缴费待分床" << std::endl;
    std::cout << "3. 已入院" << std::endl;
    std::cout << "4. 已出院" << std::endl;
    std::cout << "5. 已作废" << std::endl;
    std::cout << "0. 取消" << std::endl;

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
            std::cout << "住院ID: " << current->hospitalizationID
                      << ", 患者ID: " << current->patientID
                      << ", 病房类型: " << current->wardType
                      << ", 床位号: " << current->bedNumber
                      << ", 状态: " << hosStatusToString(current->status)
                      << std::endl;
            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到对应状态住院记录。" << std::endl;
    }
    return found;
}

bool Nurse::getHospitalizationsByTimeRange(Hospitalization *&hosHead)
{
    std::string startDate = inputDateCheck("请输入查询的开始日期 (格式 YYYY-MM-DD): ");
    std::string endDate = inputDateCheck("请输入查询的结束日期 (格式 YYYY-MM-DD): ");

    Hospitalization *current = hosHead;
    bool found = false;

    while (current != nullptr)
    {
        if (!current->isDeleted && current->department == this->department)
        {
            std::string applyDate = current->applyTime.substr(0, 10);
            if (applyDate >= startDate && applyDate <= endDate)
            {
                std::cout << "住院ID: " << current->hospitalizationID
                          << ", 患者ID: " << current->patientID
                          << ", 申请时间: " << current->applyTime
                          << ", 状态: " << hosStatusToString(current->status)
                          << std::endl;
                found = true;
            }
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到指定时间范围内住院记录。" << std::endl;
    }
    return found;
}

void Nurse::setHospitalizationStatus(Hospitalization *&target)
{
    std::cout << "当前状态: " << hosStatusToString(target->status) << std::endl;
    std::cout << "请选择新的住院状态：" << std::endl;
    std::cout << "1. 申请中" << std::endl;
    std::cout << "2. 已缴费待分床" << std::endl;
    std::cout << "3. 已入院" << std::endl;
    std::cout << "4. 已出院" << std::endl;
    std::cout << "5. 已作废" << std::endl;
    std::cout << "0. 取消" << std::endl;

    int choice = selectIntCheck(0, 5);
    if (choice == 0)
    {
        std::cout << "已取消修改！" << std::endl;
        return;
    }

    target->status = static_cast<HospitalizationStatus>(choice);
    std::cout << "住院状态已更新！" << std::endl;
}

void Nurse::assignNurseToHospitalization(Hospitalization *&target)
{
    target->nurseID = this->nurseID;
    std::cout << "已将当前护士分配到该住院记录。" << std::endl;
}

void Nurse::assignBed(Hospitalization *&target, bedInfo *&bedHead)
{
    if (target->status != HospitalizationStatus::PAID)
    {
        std::cout << "当前住院记录未处于“已缴费待分床”状态，无法分床！" << std::endl;
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
            std::cout << "床位ID: " << current->bedID
                      << ", 区域号: " << current->areaNumber
                      << ", 病房号: " << current->wardNumber
                      << ", 床号: " << current->bedNumber
                      << ", 备注: " << current->note
                      << std::endl;
            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到可用床位！" << std::endl;
        return;
    }

    std::string bedID = inputStringCheck("请输入要分配的床位ID: ");

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

            std::cout << "分配床位成功！" << std::endl;
            return;
        }
        current = current->next;
    }

    std::cout << "指定床位不可用，分配失败！" << std::endl;
}

void Nurse::transferBed(Hospitalization *&target, bedInfo *&bedHead)
{
    if (target->status != HospitalizationStatus::ADMITTED)
    {
        std::cout << "当前患者未入院，无法转床！" << std::endl;
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
        std::cout << "未找到原床位信息，无法转床！" << std::endl;
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
            std::cout << "床位ID: " << current->bedID
                      << ", 区域号: " << current->areaNumber
                      << ", 病房号: " << current->wardNumber
                      << ", 床号: " << current->bedNumber
                      << std::endl;
            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "没有可转入的新床位！" << std::endl;
        return;
    }

    std::string newBedID = inputStringCheck("请输入新床位ID: ");

    current = bedHead;
    while (current != nullptr)
    {
        if (!current->isDeleted &&
            current->bedID == newBedID &&
            current->department == target->department &&
            current->wardType == target->wardType &&
            current->status == bedStatus::AVAILABLE)
        {
            oldBed->status = bedStatus::ClEANING;
            oldBed->patientID = "#";
            oldBed->nurseID = "#";

            current->status = bedStatus::OCCUPIED;
            current->patientID = target->patientID;
            current->nurseID = this->nurseID;
            current->useTimes++;

            target->bedNumber = current->bedID;

            increaseBedManageCount();

            std::cout << "转床成功！" << std::endl;
            return;
        }
        current = current->next;
    }

    std::cout << "新床位无效，转床失败！" << std::endl;
}

void Nurse::dischargePatient(Hospitalization *&target, bedInfo *&bedHead)
{
    if (target->status != HospitalizationStatus::ADMITTED)
    {
        std::cout << "当前住院记录未处于已入院状态，无法办理出院！" << std::endl;
        return;
    }

    bedInfo *current = bedHead;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->bedID == target->bedNumber)
        {
            current->status = bedStatus::ClEANING;
            current->patientID = "#";
            current->nurseID = "#";
            break;
        }
        current = current->next;
    }

    target->status = HospitalizationStatus::DISCHARGED;
    target->dischargeTime = MyTime::getInstance().getTime();

    increaseBedManageCount();

    std::cout << "患者已办理出院，床位已释放为清洁中状态！" << std::endl;
}

void Nurse::deleteHospitalization(Hospitalization *&target)
{
    target->isDeleted = true;
    std::cout << "住院记录已逻辑删除！" << std::endl;
}

void Nurse::manageHospitalizations(Hospitalization *&hosHead, bedInfo *&bedHead)
{
    while (true)
    {
        std::cout << "\n========== 护士-住院记录管理 ==========" << std::endl;
        std::cout << "1. 查看住院记录" << std::endl;
        std::cout << "2. 修改住院记录" << std::endl;
        std::cout << "3. 删除住院记录" << std::endl;
        std::cout << "0. 返回上一级" << std::endl;

        int choice = selectIntCheck(0, 3);

        if (choice == 0)
        {
            break;
        }
        else if (choice == 1)
        {
            while (true)
            {
                std::cout << "\n------ 查看住院记录 ------" << std::endl;
                std::cout << "1. 查看本科室全部住院记录" << std::endl;
                std::cout << "2. 按患者ID查询" << std::endl;
                std::cout << "3. 按看诊ID查询" << std::endl;
                std::cout << "4. 按住院ID查询" << std::endl;
                std::cout << "5. 按状态查询" << std::endl;
                std::cout << "6. 按时间范围查询" << std::endl;
                std::cout << "0. 返回上一级" << std::endl;

                int viewChoice = selectIntCheck(0, 6);

                if (viewChoice == 0)
                    break;
                else if (viewChoice == 1)
                    getAllHospitalizations(hosHead);
                else if (viewChoice == 2)
                    getHospitalizationsByPatientID(hosHead);
                else if (viewChoice == 3)
                    getHospitalizationsByConsultationID(hosHead);
                else if (viewChoice == 4)
                    getHospitalizationsByID(hosHead);
                else if (viewChoice == 5)
                    getHospitalizationsByStatus(hosHead);
                else if (viewChoice == 6)
                    getHospitalizationsByTimeRange(hosHead);

                pause();
            }
        }
        else if (choice == 2)
        {
            bool ishave = getAllHospitalizations(hosHead);
            if (!ishave)
            {
                pause();
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
                std::cout << "未找到指定住院记录！" << std::endl;
                pause();
                continue;
            }

            while (true)
            {
                std::cout << "\n------ 修改住院记录 ------" << std::endl;
                std::cout << "1. 修改住院状态" << std::endl;
                std::cout << "2. 分配当前护士" << std::endl;
                std::cout << "3. 分配床位" << std::endl;
                std::cout << "4. 调整床位" << std::endl;
                std::cout << "5. 办理出院" << std::endl;
                std::cout << "0. 返回上一级" << std::endl;

                int modifyChoice = selectIntCheck(0, 5);

                if (modifyChoice == 0)
                    break;
                else if (modifyChoice == 1)
                    setHospitalizationStatus(target);
                else if (modifyChoice == 2)
                    assignNurseToHospitalization(target);
                else if (modifyChoice == 3)
                    assignBed(target, bedHead);
                else if (modifyChoice == 4)
                    transferBed(target, bedHead);
                else if (modifyChoice == 5)
                    dischargePatient(target, bedHead);

                pause();
            }
        }
        else if (choice == 3)
        {
            bool ishave = getAllHospitalizations(hosHead);
            if (!ishave)
            {
                pause();
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
                std::cout << "未找到指定住院记录！" << std::endl;
                pause();
                continue;
            }

            std::cout << "确认删除该住院记录吗？\n1. 确认\n0. 取消" << std::endl;
            int confirmChoice = selectIntCheck(0, 1);
            if (confirmChoice == 1)
            {
                deleteHospitalization(target);
            }
            else
            {
                std::cout << "已取消删除操作！" << std::endl;
            }
            pause();
        }
    }
}

// ==================== 床位信息管理 ====================

bool Nurse::getAllBeds(bedInfo *&bedHead)
{
    bedInfo *current = bedHead;
    bool found = false;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->department == this->department)
        {
            std::cout << "床位ID: " << current->bedID
                      << ", 状态: " << bedStatusToString(current->status)
                      << ", 病房类型: " << current->wardType
                      << ", 科室: " << current->department
                      << ", 区域号: " << current->areaNumber
                      << ", 病房号: " << current->wardNumber
                      << ", 床号: " << current->bedNumber
                      << ", 患者ID: " << current->patientID
                      << ", 护士ID: " << current->nurseID
                      << ", 使用次数: " << current->useTimes
                      << ", 占用天数: " << current->daysOccupied
                      << ", 备注: " << current->note
                      << std::endl;
            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到本科室床位信息。" << std::endl;
    }
    return found;
}

bool Nurse::getBedsByStatus(bedInfo *&bedHead)
{
    std::cout << "请选择床位状态：" << std::endl;
    std::cout << "1. 已占用" << std::endl;
    std::cout << "2. 清洁中" << std::endl;
    std::cout << "3. 可分配" << std::endl;
    std::cout << "4. 不可用" << std::endl;
    std::cout << "0. 取消" << std::endl;

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
            std::cout << "床位ID: " << current->bedID
                      << ", 状态: " << bedStatusToString(current->status)
                      << ", 患者ID: " << current->patientID
                      << ", 护士ID: " << current->nurseID
                      << std::endl;
            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到该状态床位。" << std::endl;
    }
    return found;
}

bool Nurse::getBedsByPatientID(bedInfo *&bedHead)
{
    std::string patientID = inputRecordIDCheck("请输入患者ID: ", {"pat"});
    bedInfo *current = bedHead;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->department == this->department && current->patientID == patientID)
        {
            std::cout << "床位ID: " << current->bedID << ", 状态: " << bedStatusToString(current->status) << std::endl;
            return true;
        }
        current = current->next;
    }
    std::cout << "未找到该患者对应床位。" << std::endl;
    return false;
}

bool Nurse::getBedsByNurseID(bedInfo *&bedHead)
{
    std::string nurseID = inputIDCheck("请输入护士ID: ");
    bedInfo *current = bedHead;
    bool found = false;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->department == this->department && current->nurseID == nurseID)
        {
            std::cout << "床位ID: " << current->bedID << ", 患者ID: " << current->patientID << std::endl;
            found = true;
        }
        current = current->next;
    }
    if (!found)
    {
        std::cout << "未找到该护士负责的床位。" << std::endl;
    }
    return found;
}

bool Nurse::getBedsByDepartment(bedInfo *&bedHead)
{
    std::string dept = inputDepartmentCheck("请输入科室名称: ");
    bedInfo *current = bedHead;
    bool found = false;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->department == dept)
        {
            std::cout << "床位ID: " << current->bedID
                      << ", 状态: " << bedStatusToString(current->status)
                      << std::endl;
            found = true;
        }
        current = current->next;
    }
    if (!found)
    {
        std::cout << "未找到该科室床位。" << std::endl;
    }
    return found;
}

bool Nurse::getBedByID(bedInfo *&bedHead)
{
    std::string bedID = inputStringCheck("请输入床位ID: ");
    bedInfo *current = bedHead;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->bedID == bedID)
        {
            std::cout << "床位ID: " << current->bedID
                      << ", 状态: " << bedStatusToString(current->status)
                      << ", 病房类型: " << current->wardType
                      << ", 患者ID: " << current->patientID
                      << std::endl;
            return true;
        }
        current = current->next;
    }
    std::cout << "未找到该床位。" << std::endl;
    return false;
}

void Nurse::setBedStatus(bedInfo *&target)
{
    std::cout << "当前床位状态: " << bedStatusToString(target->status) << std::endl;
    std::cout << "请选择新的床位状态：" << std::endl;
    std::cout << "1. 已占用" << std::endl;
    std::cout << "2. 清洁中" << std::endl;
    std::cout << "3. 可分配" << std::endl;
    std::cout << "4. 不可用" << std::endl;

    int choice = selectIntCheck(1, 4);
    target->status = static_cast<bedStatus>(choice);
    std::cout << "床位状态已更新！" << std::endl;
}

void Nurse::setBedNote(bedInfo *&target)
{
    std::cout << "当前备注: " << target->note << std::endl;
    target->note = inputStringCheck("请输入新的备注信息: ");
    std::cout << "床位备注已更新！" << std::endl;
}

void Nurse::setBedVitalSigns(bedInfo *&target)
{
    std::cout << "开始录入床位关联患者生命体征..." << std::endl;
    setVitalSigns(target->vitalSigns, "综合体征录入");
    std::cout << "生命体征已更新！" << std::endl;
}

void Nurse::deleteBed(bedInfo *&target)
{
    target->isDeleted = true;
    std::cout << "床位信息已逻辑删除！" << std::endl;
}

bool Nurse::createBed(bedInfo *&bedHead, int &idCounter)
{
    bedInfo *newBed = new bedInfo();

    newBed->department = inputDepartmentCheck("请输入床位所属科室: ");
    newBed->wardType = inputStringCheck("请输入病房类型(普通病房/隔离病房/VIP病房/ICU): ");

    std::cout << "请输入区域号: " << std::endl;
    newBed->areaNumber = selectIntCheck(1, 999);

    std::cout << "请输入病房号: " << std::endl;
    newBed->wardNumber = selectIntCheck(1, 999);

    std::cout << "请输入床号: " << std::endl;
    newBed->bedNumber = selectIntCheck(1, 99);

    newBed->note = inputStringCheck("请输入备注信息: ");

    newBed->bedID = "bed" + std::to_string(idCounter++).insert(0, 6 - std::to_string(idCounter).length(), '0');

    newBed->next = bedHead;
    if (bedHead)
        bedHead->prev = newBed;
    bedHead = newBed;

    std::cout << "床位创建成功！床位ID: " << newBed->bedID << std::endl;
    return true;
}

void Nurse::manageBeds(bedInfo *&bedHead, int &idCounter)
{
    while (true)
    {
        std::cout << "\n========== 护士-床位信息管理 ==========" << std::endl;
        std::cout << "1. 查看床位信息" << std::endl;
        std::cout << "2. 修改床位信息" << std::endl;
        std::cout << "3. 删除床位信息" << std::endl;
        std::cout << "4. 新增床位信息" << std::endl;
        std::cout << "0. 返回上一级" << std::endl;

        int choice = selectIntCheck(0, 4);

        if (choice == 0)
        {
            break;
        }
        else if (choice == 1)
        {
            while (true)
            {
                std::cout << "\n------ 查看床位信息 ------" << std::endl;
                std::cout << "1. 查看本科室全部床位" << std::endl;
                std::cout << "2. 按床位状态查询" << std::endl;
                std::cout << "3. 按患者ID查询" << std::endl;
                std::cout << "4. 按护士ID查询" << std::endl;
                std::cout << "5. 按科室查询" << std::endl;
                std::cout << "6. 按床位ID查询" << std::endl;
                std::cout << "0. 返回上一级" << std::endl;

                int viewChoice = selectIntCheck(0, 6);

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

                pause();
            }
        }
        else if (choice == 2)
        {
            bool ishave = getAllBeds(bedHead);
            if (!ishave)
            {
                pause();
                continue;
            }

            std::string bedID = inputStringCheck("请输入要修改的床位ID: ");
            bedInfo *target = bedHead;
            while (target)
            {
                if (!target->isDeleted && target->bedID == bedID)
                {
                    break;
                }
                target = target->next;
            }

            if (!target)
            {
                std::cout << "未找到指定床位！" << std::endl;
                pause();
                continue;
            }

            while (true)
            {
                std::cout << "\n------ 修改床位信息 ------" << std::endl;
                std::cout << "1. 修改床位状态" << std::endl;
                std::cout << "2. 修改备注" << std::endl;
                std::cout << "3. 更新生命体征" << std::endl;
                std::cout << "0. 返回上一级" << std::endl;

                int modifyChoice = selectIntCheck(0, 3);

                if (modifyChoice == 0)
                    break;
                else if (modifyChoice == 1)
                    setBedStatus(target);
                else if (modifyChoice == 2)
                    setBedNote(target);
                else if (modifyChoice == 3)
                    setBedVitalSigns(target);

                pause();
            }
        }
        else if (choice == 3)
        {
            bool ishave = getAllBeds(bedHead);
            if (!ishave)
            {
                pause();
                continue;
            }

            std::string bedID = inputStringCheck("请输入要删除的床位ID: ");
            bedInfo *target = bedHead;
            while (target)
            {
                if (!target->isDeleted && target->bedID == bedID)
                {
                    break;
                }
                target = target->next;
            }

            if (!target)
            {
                std::cout << "未找到指定床位！" << std::endl;
                pause();
                continue;
            }

            std::cout << "确认删除该床位吗？\n1. 确认\n0. 取消" << std::endl;
            int confirmChoice = selectIntCheck(0, 1);
            if (confirmChoice == 1)
            {
                deleteBed(target);
            }
            else
            {
                std::cout << "已取消删除操作！" << std::endl;
            }
            pause();
        }
        else if (choice == 4)
        {
            createBed(bedHead, idCounter);
            pause();
        }
    }
}

// ==================== 个人信息管理 ====================

void Nurse::managePersonalInfo()
{
    while (true)
    {
        std::cout << "\n========== 护士-个人信息管理 ==========" << std::endl;
        std::cout << "1. 查看个人信息" << std::endl;
        std::cout << "2. 修改个人信息" << std::endl;
        std::cout << "0. 返回上一级" << std::endl;

        int choice = selectIntCheck(0, 2);

        if (choice == 0)
        {
            break;
        }
        else if (choice == 1)
        {
            while (true)
            {
                std::cout << "\n------ 查看个人信息 ------" << std::endl;
                std::cout << "1. 护士ID" << std::endl;
                std::cout << "2. 姓名" << std::endl;
                std::cout << "3. 性别" << std::endl;
                std::cout << "4. 年龄" << std::endl;
                std::cout << "5. 科室" << std::endl;
                std::cout << "6. 护士等级" << std::endl;
                std::cout << "7. 联系电话" << std::endl;
                std::cout << "8. 邮箱" << std::endl;
                std::cout << "9. 在岗状态" << std::endl;
                std::cout << "10. 排班信息" << std::endl;
                std::cout << "11. 累计护理人数" << std::endl;
                std::cout << "12. 累计床位管理次数" << std::endl;
                std::cout << "13. 账户创建时间" << std::endl;
                std::cout << "0. 返回上一级" << std::endl;

                int viewChoice = selectIntCheck(0, 13);

                if (viewChoice == 0)
                    break;
                else if (viewChoice == 1)
                    std::cout << "护士ID: " << this->nurseID << std::endl;
                else if (viewChoice == 2)
                    std::cout << "姓名: " << this->username << std::endl;
                else if (viewChoice == 3)
                    std::cout << "性别: " << this->gender << std::endl;
                else if (viewChoice == 4)
                    std::cout << "年龄: " << this->age << std::endl;
                else if (viewChoice == 5)
                    std::cout << "科室: " << this->department << std::endl;
                else if (viewChoice == 6)
                    std::cout << "护士等级: " << nurseLevelToString(this->level) << std::endl;
                else if (viewChoice == 7)
                    std::cout << "联系电话: " << this->telephone << std::endl;
                else if (viewChoice == 8)
                    std::cout << "邮箱: " << this->email << std::endl;
                else if (viewChoice == 9)
                    std::cout << "在岗状态: " << (this->isOnDuty ? "在岗" : "不在岗") << std::endl;
                else if (viewChoice == 10)
                    std::cout << "排班信息: " << this->scheduleInfo << std::endl;
                else if (viewChoice == 11)
                    std::cout << "累计护理人数: " << this->patientCareCount << std::endl;
                else if (viewChoice == 12)
                    std::cout << "累计床位管理次数: " << this->bedManageCount << std::endl;
                else if (viewChoice == 13)
                    std::cout << "账户创建时间: " << this->createTime << std::endl;

                pause();
            }
        }
        else if (choice == 2)
        {
            while (true)
            {
                std::cout << "\n------ 修改个人信息 ------" << std::endl;
                std::cout << "1. 修改姓名" << std::endl;
                std::cout << "2. 修改性别" << std::endl;
                std::cout << "3. 修改年龄" << std::endl;
                std::cout << "4. 修改科室" << std::endl;
                std::cout << "5. 修改护士等级" << std::endl;
                std::cout << "6. 修改联系电话" << std::endl;
                std::cout << "7. 修改邮箱" << std::endl;
                std::cout << "8. 修改在岗状态" << std::endl;
                std::cout << "9. 修改排班信息" << std::endl;
                std::cout << "10. 修改密码" << std::endl;
                std::cout << "0. 返回上一级" << std::endl;

                int modifyChoice = selectIntCheck(0, 10);

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
                    std::cout << "当前等级: " << nurseLevelToString(this->level) << std::endl;
                    std::cout << "请选择新的护士等级:" << std::endl;
                    std::cout << "1. 实习护士" << std::endl;
                    std::cout << "2. 初级护士" << std::endl;
                    std::cout << "3. 高级护士" << std::endl;
                    std::cout << "4. 护士长" << std::endl;
                    int levelChoice = selectIntCheck(1, 4);
                    this->level = static_cast<NurseLevel>(levelChoice);
                    std::cout << "护士等级已更新！" << std::endl;
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