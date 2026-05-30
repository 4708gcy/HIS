/**
 * @file AdminUsers.cpp
 * @brief 管理员人员管理功能实现
 * @details 包含医生、护士、药剂师、患者四类人员管理及个人信息管理
 */

#include "Roles/Admin.h"
#include "Core/UI.h"
#include "Core/User.h"
// ==================================== 医生信息管理 =================================

// 查看所有的医生信息
bool Admin::viewAllDoctors(Doctor *&doc, const std::string &department)
{
    Doctor *current = doc;
    bool found = false;
    std::cout << "正在查找科室 " << department << " 的医生信息..." << std::endl;
    while (current != nullptr)
    {
        if (!current->isDeleted && deptMatch(current->department, department))
        {
            std::string titleStr = doctorTitleToString(current->title);
            std::cout << "医生ID: " << current->getUserID()
                      << ", 姓名: " << current->getUsername()
                      << ",性别: " << current->getGender()
                      << ", 职称: " << titleStr
                      << ", 科室: " << current->department
                      << ", 联系方式: " << current->getTelephone() << " / " << current->getEmail()
                      << ", 擅长领域: " << current->specialty
                      << ", 在岗状态: " << (current->isOnDuty ? "在岗" : "不在岗")
                      << ", 排班信息：" << current->scheduleInfo
                      << ", 累计看诊人数: " << current->consultationCount
                      << ", 累计开具检查次数: " << current->examinationCount
                      << ", 累计开具住院证次数: " << current->hospitalizationApplyCount
                      << std::endl;
            found = true;
            std::cout << std::endl;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "暂无医生信息！" << std::endl;
    }
    return found;
}
// 根据医生ID查看医生信息
bool Admin::viewDoctorByID(Doctor *&doc, const std::string &department)
{
    std::string doctorID = inputIDCheck("请输入要查找的医生ID: ");

    Doctor *current = doc;
    bool found = false;
    std::cout << "正在查找ID: " << doctorID << " 的医生信息..." << std::endl;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->getUserID() == doctorID && deptMatch(current->department, department))
        {
            std::string titleStr = doctorTitleToString(current->title);
            std::cout << "医生ID: " << current->getUserID()
                      << ", 姓名: " << current->getUsername()
                      << ",性别: " << current->getGender()
                      << ", 职称: " << titleStr
                      << ", 科室: " << current->department
                      << ", 联系方式: " << current->getTelephone() << " / " << current->getEmail()
                      << ", 擅长领域: " << current->specialty
                      << ", 在岗状态: " << (current->isOnDuty ? "在岗" : "不在岗")
                      << ", 排班信息：" << current->scheduleInfo
                      << ", 累计看诊人数: " << current->consultationCount
                      << ", 累计开具检查次数: " << current->examinationCount
                      << ", 累计开具住院证次数: " << current->hospitalizationApplyCount
                      << std::endl;
            found = true;
            break;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到ID为 " << doctorID << " 的医生信息！" << std::endl;
    }
    return found;
}
// 根据医生姓名查看医生信息
bool Admin::viewDoctorsByName(Doctor *&doc, const std::string &department)
{
    std::string name = inputStringCheck("请输入医生姓名（支持模糊查询）: ");

    Doctor *current = doc;
    bool found = false;
    std::cout << "正在查找姓名包含 \"" << name << "\" 的医生信息..." << std::endl;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->getUsername().find(name) != std::string::npos && deptMatch(current->department, department))
        {
            std::string titleStr = doctorTitleToString(current->title);
            std::cout << "医生ID: " << current->getUserID()
                      << ", 姓名: " << current->getUsername()
                      << ",性别: " << current->getGender()
                      << ", 职称: " << titleStr
                      << ", 科室: " << current->department
                      << ", 联系方式: " << current->getTelephone() << " / " << current->getEmail()
                      << ", 擅长领域: " << current->specialty
                      << ", 在岗状态: " << (current->isOnDuty ? "在岗" : "不在岗")
                      << ", 排班信息：" << current->scheduleInfo
                      << ", 累计看诊人数: " << current->consultationCount
                      << ", 累计开具检查次数: " << current->examinationCount
                      << ", 累计开具住院证次数: " << current->hospitalizationApplyCount
                      << std::endl;
            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到姓名包含 \"" << name << "\" 的医生信息！" << std::endl;
    }
    return found;
}
// 根据医生职称查看医生信息
bool Admin::viewDoctorByTitle(Doctor *&doc, const std::string &department)
{
    int titleFilter = DoctorTitleMenu(); // 选择医生职称过滤条件

    if (titleFilter == 0)
    {
        return false; // 返回上一级菜单
    }

    bool found = false;

    DoctorTitle filterTitle = static_cast<DoctorTitle>(titleFilter);

    Doctor *current = doc;
    std::cout << "正在查找职称为: " << doctorTitleToString(filterTitle) << " 的医生信息..." << std::endl;
    while (current != nullptr)
    {
        if (!current->isDeleted && deptMatch(current->department, department) && current->title == filterTitle)
        {
            std::string titleStr = doctorTitleToString(current->title);
            std::cout << "医生ID: " << current->getUserID()
                      << ", 姓名: " << current->getUsername()
                      << ",性别: " << current->getGender()
                      << ", 职称: " << titleStr
                      << ", 科室: " << current->department
                      << ", 联系方式: " << current->getTelephone() << " / " << current->getEmail()
                      << ", 擅长领域: " << current->specialty
                      << ", 在岗状态: " << (current->isOnDuty ? "在岗" : "不在岗")
                      << ", 排班信息：" << current->scheduleInfo
                      << ", 累计看诊人数: " << current->consultationCount
                      << ", 累计开具检查次数: " << current->examinationCount
                      << ", 累计开具住院证次数: " << current->hospitalizationApplyCount
                      << std::endl;
            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到职称为 " << doctorTitleToString(filterTitle) << " 的医生信息！" << std::endl;
    }
    return found;
}
// 根据医生在岗状态查看医生信息
bool Admin::viewDoctorsByOnDutyStatus(Doctor *&doc, const std::string &department)
{

    bool found = false;

    clearScreen();
    printMenuBorder();
    printMenuTitle("医生在岗状态筛选");
    printMenuItem(1, "在岗");
    printMenuItem(2, "不在岗");
    printMenuItem(0, "返回上一级菜单");
    printMenuBottom();
    int onDutyFilter = selectIntCheck(0, 2);

    if (onDutyFilter == 0)
    {
        return false; // 返回上一级菜单
    }

    bool filterOnDutyStatus = (onDutyFilter == 1); // 1表示在岗，2表示不在岗

    Doctor *current = doc;
    std::cout << "正在查找在岗状态为: " << (filterOnDutyStatus ? "在岗" : "不在岗") << " 的医生信息..." << std::endl;
    while (current != nullptr)
    {
        if (!current->isDeleted && deptMatch(current->department, department) && current->isOnDuty == filterOnDutyStatus)
        {
            std::string titleStr = doctorTitleToString(current->title);
            std::cout << "医生ID: " << current->getUserID()
                      << ", 姓名: " << current->getUsername()
                      << ",性别: " << current->getGender()
                      << ", 职称: " << titleStr
                      << ", 科室: " << current->department
                      << ", 联系方式: " << current->getTelephone() << " / " << current->getEmail()
                      << ", 擅长领域: " << current->specialty
                      << ", 在岗状态: " << (current->isOnDuty ? "在岗" : "不在岗")
                      << ", 排班信息：" << current->scheduleInfo
                      << ", 累计看诊人数: " << current->consultationCount
                      << ", 累计开具检查次数: " << current->examinationCount
                      << ", 累计开具住院证次数: " << current->hospitalizationApplyCount
                      << std::endl;
            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到在岗状态为 " << (filterOnDutyStatus ? "在岗" : "不在岗") << " 的医生信息！" << std::endl;
    }
    return found;
}
// 修改医生姓名
void Admin::modifyDoctorName(Doctor *&target, const std::string &department)
{
    std::cout << "当前医生姓名: " << target->getUsername() << std::endl;
    std::string newName = inputStringCheck("请输入新的医生姓名: ");
    target->setUsername(newName);
    std::cout << "医生姓名已更新！" << std::endl;
}
// 修改医生性别
void Admin::modifyDoctorGender(Doctor *&target, const std::string &department)
{
    std::cout << "当前医生性别: " << target->getGender() << std::endl;
    std::string newGender = inputGenderCheck("请输入新的医生性别: ");
    target->setGender(newGender);
    std::cout << "医生性别已更新！" << std::endl;
}
// 修改医生的年龄
void Admin::modifyDoctorAge(Doctor *&target, const std::string &department)
{
    std::cout << "当前医生年龄: " << target->getAge() << std::endl;
    int newAge = inputAgeCheck("请输入新的医生年龄: ");
    target->setAge(newAge);
    std::cout << "医生年龄已更新！" << std::endl;
}
// 修改医生所属科室
void Admin::modifyDoctorDepartment(Doctor *&target, const std::string &department)
{
    std::cout << "当前医生所属科室: " << target->department << std::endl;
    std::string newDepartment = adminDepartmentMenu();

    if (newDepartment == "0")
    {
        std::cout << "科室修改已取消！" << std::endl;
        return; // 取消修改，返回上一级菜单
    }
    else if (newDepartment.empty())
    {
        printWarning("输入无效，科室修改已取消！");
        return; // 输入无效，取消修改，返回上一级菜单
    }

    target->department = newDepartment;
    std::cout << "医生所属科室已更新！" << std::endl;
}
// 修改医生职称
void Admin::modifyDoctorTitle(Doctor *&target, const std::string &department)
{
    std::cout << "当前医生职称: " << doctorTitleToString(target->title) << std::endl;
    int newTitle = DoctorTitleMenu(); // 选择新的医生职称

    if (newTitle == 0)
    {
        std::cout << "职称修改已取消！" << std::endl;
        return; // 取消修改，返回上一级菜单
    }

    target->title = static_cast<DoctorTitle>(newTitle);
    std::cout << "医生职称已更新！" << std::endl;
}
// 修改医生电话
void Admin::modifyDoctorTelephone(Doctor *&target, const std::string &department)
{
    std::cout << "当前医生电话: " << target->getTelephone() << std::endl;
    std::string newTelephone = inputTelephoneCheck("请输入新的医生电话: ");
    target->setTelephone(newTelephone);
    std::cout << "医生电话已更新！" << std::endl;
}
// 修改医生邮箱
void Admin::modifyDoctorEmail(Doctor *&target, const std::string &department)
{
    std::cout << "当前医生邮箱: " << target->getEmail() << std::endl;
    std::string newEmail = inputEmailCheck("请输入新的医生邮箱: ");
    target->setEmail(newEmail);
    std::cout << "医生邮箱已更新！" << std::endl;
}
// 修改医生在岗状态
void Admin::modifyDoctorOnDutyStatus(Doctor *&target, const std::string &department)
{
    std::cout << "当前医生在岗状态: " << (target->isOnDuty ? "在岗" : "不在岗") << std::endl;
    clearScreen();
    printMenuBorder();
    printMenuTitle("选择在岗状态");
    printMenuItem(1, "在岗");
    printMenuItem(2, "不在岗");
    printMenuItem(0, "返回上一级菜单");
    printMenuBottom();

    int onDutyChoice = selectIntCheck(0, 2);
    if (onDutyChoice == 0)
    {
        std::cout << "在岗状态修改已取消！" << std::endl;
        return;
    }
    target->isOnDuty = (onDutyChoice == 1); // 1表示在岗，2表示不在岗
    std::cout << "医生在岗状态已更新！" << std::endl;
}
// 修改医生擅长领域
void Admin::modifyDoctorSpecialty(Doctor *&target, const std::string &department)
{
    std::cout << "当前医生擅长领域: " << target->specialty << std::endl;
    std::string newSpecialty = inputStringCheck("请输入新的医生擅长领域: ");
    target->specialty = newSpecialty;
    std::cout << "医生擅长领域已更新！" << std::endl;
}
// 修改医生排班信息
void Admin::modifyDoctorScheduleInfo(Doctor *&target, const std::string &department)
{
    std::cout << "当前医生排班信息: " << target->scheduleInfo << std::endl;
    std::string newScheduleInfo = inputStringCheck("请输入新的医生排班信息: ");
    target->scheduleInfo = newScheduleInfo;
    std::cout << "医生排班信息已更新！" << std::endl;
}
// 修改医生累计看诊人数
void Admin::modifyDoctorConsultationCount(Doctor *&target, const std::string &department)
{
    std::cout << "当前医生累计看诊人数: " << target->consultationCount << std::endl;
    std::cout << "输入新的累计看诊人数 ";
    int newConsultationCount = selectIntCheck(0, INT_MAX);
    target->consultationCount = newConsultationCount;
    std::cout << "医生累计看诊人数已更新！" << std::endl;
}
// 修改医生累计开具检查次数
void Admin::modifyDoctorExaminationCount(Doctor *&target, const std::string &department)
{
    std::cout << "当前医生累计开具检查次数: " << target->examinationCount << std::endl;
    std::cout << "输入新的累计开具检查次数 ";
    int newExaminationCount = selectIntCheck(0, INT_MAX);
    target->examinationCount = newExaminationCount;
    std::cout << "医生累计开具检查次数已更新！" << std::endl;
}
// 修改医生累计开具住院证次数
void Admin::modifyDoctorHospitalizationApplyCount(Doctor *&target, const std::string &department)
{
    std::cout << "当前医生累计开具住院证次数: " << target->hospitalizationApplyCount << std::endl;
    std::cout << "输入新的累计开具住院证次数 ";
    int newHospitalizationApplyCount = selectIntCheck(0, INT_MAX);
    target->hospitalizationApplyCount = newHospitalizationApplyCount;
    std::cout << "医生累计开具住院证次数已更新！" << std::endl;
}
// 删除医生（逻辑删除，设置 isDeleted 标志）
void Admin::deleteDoctor(Doctor *&target, const std::string &department)
{
    target->isDeleted = true; // 逻辑删除
    std::cout << "医生ID: " << target->getUserID() << " 已删除！" << std::endl;
}
// 添加医生（根据输入信息创建新的 Doctor 对象，并插入到链表中）
bool Admin::addDoctor(Doctor *&doc, int &idCounter)
{
    Doctor *newDoc = new Doctor();
    bool success = newDoc->doctorSignUp(idCounter);
    if (!success)
    {
        delete newDoc;
        return false;
    }
    // 插入到链表头部
    newDoc->next = doc;
    if (doc != nullptr)
    {
        doc->prev = newDoc;
    }
    doc = newDoc;
    return true;
}

void Admin::manageDoctors(Doctor *&doc, const std::string &department, int &idCounter)
{
    while (true)
    {
        int choice = adminDoctorManagementMenu();

        if (choice == 1) // 查看医生信息
        {
            while (true)
            {
                int viewChoice = adminDoctorViewMenu();
                if (viewChoice == 0) // 返回上一级菜单
                {
                    break;
                }
                else if (viewChoice == 1) // 查看所有医生信息
                {
                    viewAllDoctors(doc, department);
                    pause("管理员 > 账户管理 > 医生管理");
                }
                else if (viewChoice == 2) // 根据医生ID查看医生信息
                {
                    viewDoctorByID(doc, department);
                    pause("管理员 > 账户管理 > 医生管理");
                }
                else if (viewChoice == 3) // 根据医生姓名查看医生信息
                {
                    viewDoctorsByName(doc, department);
                    pause("管理员 > 账户管理 > 医生管理");
                }
                else if (viewChoice == 4) // 根据医生在岗状态查看医生信息
                {
                    viewDoctorsByOnDutyStatus(doc, department);
                    pause("管理员 > 账户管理 > 医生管理");
                }
                else if (viewChoice == 5) // 根据医生职称查看医生信息
                {
                    viewDoctorByTitle(doc, department);
                    pause("管理员 > 账户管理 > 医生管理");
                }
            }
        }
        else if (choice == 2) // 修改医生信息
        {
            bool ishave = viewAllDoctors(doc, department);

            if (ishave)
            {
                std::string doctorID = inputIDCheck("请输入要进行修改的医生ID: ");

                Doctor *target = doc;
                while (target != nullptr)
                {
                    if (!target->isDeleted && target->getUserID() == doctorID && deptMatch(target->department, department))
                    {
                        break;
                    }
                    target = target->next;
                }

                if (target)
                {
                    while (true)
                    {
                        std::cout << "正在修改的医生信息：" << std::endl;

                        std::string titleStr = doctorTitleToString(target->title);
                        std::cout << "医生ID: " << target->getUserID()
                                  << ", 姓名: " << target->getUsername()
                                  << ",性别: " << target->getGender()
                                  << ", 职称: " << titleStr
                                  << ", 科室: " << target->department
                                  << ", 联系方式: " << target->getTelephone() << " / " << target->getEmail()
                                  << ", 擅长领域: " << target->specialty
                                  << ", 在岗状态: " << (target->isOnDuty ? "在岗" : "不在岗")
                                  << ", 排班信息：" << target->scheduleInfo
                                  << ", 累计看诊人数: " << target->consultationCount
                                  << ", 累计开具检查次数: " << target->examinationCount
                                  << ", 累计开具住院证次数: " << target->hospitalizationApplyCount
                                  << std::endl;

                        int modifyChoice = adminDoctorModificationMenu();

                        if (modifyChoice == 0) // 返回上一级菜单
                        {
                            break;
                        }
                        else if (modifyChoice == 1) // 修改医生姓名
                        {
                            modifyDoctorName(target, department);
                            pause("管理员 > 账户管理 > 医生管理");
                        }
                        else if (modifyChoice == 2) // 修改医生性别
                        {
                            modifyDoctorGender(target, department);
                            pause("管理员 > 账户管理 > 医生管理");
                        }
                        else if (modifyChoice == 3) // 修改医生年龄
                        {
                            modifyDoctorAge(target, department);
                            pause("管理员 > 账户管理 > 医生管理");
                        }
                        else if (modifyChoice == 4) // 修改医生所属科室
                        {
                            modifyDoctorDepartment(target, department);
                            pause("管理员 > 账户管理 > 医生管理");
                        }
                        else if (modifyChoice == 5) // 修改医生职称
                        {
                            modifyDoctorTitle(target, department);
                            pause("管理员 > 账户管理 > 医生管理");
                        }
                        else if (modifyChoice == 6) // 修改医生电话
                        {
                            modifyDoctorTelephone(target, department);
                            pause("管理员 > 账户管理 > 医生管理");
                        }
                        else if (modifyChoice == 7) // 修改医生邮箱
                        {
                            modifyDoctorEmail(target, department);
                            pause("管理员 > 账户管理 > 医生管理");
                        }
                        else if (modifyChoice == 8) // 修改医生在岗状态
                        {
                            modifyDoctorOnDutyStatus(target, department);
                            pause("管理员 > 账户管理 > 医生管理");
                        }
                        else if (modifyChoice == 9) // 修改医生擅长领域
                        {
                            modifyDoctorSpecialty(target, department);
                            pause("管理员 > 账户管理 > 医生管理");
                        }
                        else if (modifyChoice == 10) // 修改医生排班信息
                        {
                            modifyDoctorScheduleInfo(target, department);
                            pause("管理员 > 账户管理 > 医生管理");
                        }
                        else if (modifyChoice == 11) // 修改医生累计看诊人数
                        {
                            modifyDoctorConsultationCount(target, department);
                            pause("管理员 > 账户管理 > 医生管理");
                        }
                        else if (modifyChoice == 12) // 修改医生累计开具检查次数
                        {
                            modifyDoctorExaminationCount(target, department);
                            pause("管理员 > 账户管理 > 医生管理");
                        }
                        else if (modifyChoice == 13) // 修改医生累计开具住院证次数
                        {
                            modifyDoctorHospitalizationApplyCount(target, department);
                            pause("管理员 > 账户管理 > 医生管理");
                        }
                    }
                }
                else
                {
                    std::cout << "未找到ID为 " << doctorID << " 的医生信息！" << std::endl;
                    pause("管理员 > 账户管理 > 医生管理");
                }
            }
        }
        else if (choice == 3) // 删除医生
        {
            bool ishave = viewAllDoctors(doc, department);

            if (ishave)
            {
                std::string doctorID = inputIDCheck("请输入要删除的医生ID: ");

                Doctor *target = doc;
                while (target != nullptr)
                {
                    if (!target->isDeleted && target->getUserID() == doctorID && deptMatch(target->department, department))
                    {
                        break;
                    }
                    target = target->next;
                }

                if (target)
                {
                    std::cout << "正在删除的医生信息如下：" << std::endl;
                    std::string titleStr = doctorTitleToString(target->title);
                    std::cout << "医生ID: " << target->getUserID()
                              << ", 姓名: " << target->getUsername()
                              << ",性别: " << target->getGender()
                              << ", 职称: " << titleStr
                              << ", 科室: " << target->department
                              << ", 联系方式: " << target->getTelephone() << " / " << target->getEmail()
                              << ", 擅长领域: " << target->specialty
                              << ", 在岗状态: " << (target->isOnDuty ? "在岗" : "不在岗")
                              << ", 排班信息：" << target->scheduleInfo
                              << ", 累计看诊人数: " << target->consultationCount
                              << ", 累计开具检查次数: " << target->examinationCount
                              << ", 累计开具住院证次数: " << target->hospitalizationApplyCount
                              << std::endl;

                    clearScreen();
                    printMenuBorder();
                    printMenuTitle("确认删除医生");
                    printMenuItem(1, "确认");
                    printMenuItem(0, "取消");
                    printMenuBottom();

                    int confirmChoice = selectIntCheck(0, 1);
                    if (confirmChoice == 1)
                    {
                        deleteDoctor(target, department);
                        pause("管理员 > 账户管理 > 医生管理");
                    }
                    else
                    {
                        std::cout << "医生删除已取消！" << std::endl;
                        pause("管理员 > 账户管理 > 医生管理");
                    }
                }
                else
                {
                    std::cout << "未找到ID为 " << doctorID << " 的医生信息！" << std::endl;
                    pause("管理员 > 账户管理 > 医生管理");
                }
            }
        }
        else if (choice == 4) // 添加医生
        {
            addDoctor(doc, idCounter);
            pause("管理员 > 账户管理 > 医生管理");
        }
        else if (choice == 0) // 返回上一级菜单
        {
            break;
        }
    }
}

// =================================== 护士信息管理 =================================

// 查看所有的护士信息
bool Admin::viewAllNurses(Nurse *&nurse, const std::string &department)
{
    Nurse *current = nurse;
    bool found = false;
    std::cout << "正在查找科室 " << department << " 的护士信息..." << std::endl;
    while (current != nullptr)
    {
        if (!current->isDeleted && deptMatch(current->department, department))
        {
            std::string level = nurseLevelToString(current->level);
            std::cout << "护士ID: " << current->getUserID()
                      << ", 姓名: " << current->getUsername()
                      << ",性别: " << current->getGender()
                      << ", 职称: " << level
                      << ", 科室: " << current->department
                      << ", 联系方式: " << current->getTelephone() << " / " << current->getEmail()
                      << ", 排班信息：" << current->scheduleInfo
                      << ", 在岗状态: " << (current->isOnDuty ? "在岗" : "不在岗")
                      << ", 累计护理人数: " << current->patientCareCount
                      << ", 床位管理次数: " << current->bedManageCount
                      << std::endl;
            found = true;
            std::cout << std::endl;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "暂无护士信息！" << std::endl;
    }
    return found;
}
// 根据护士ID查看护士信息
bool Admin::viewNurseByID(Nurse *&nurse, const std::string &department)
{
    std::string nurseID = inputIDCheck("请输入要查找的护士ID: ");

    Nurse *current = nurse;
    bool found = false;
    std::cout << "正在查找ID: " << nurseID << " 的护士信息..." << std::endl;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->getUserID() == nurseID && deptMatch(current->department, department))
        {
            std::string level = nurseLevelToString(current->level);
            std::cout << "护士ID: " << current->getUserID()
                      << ", 姓名: " << current->getUsername()
                      << ",性别: " << current->getGender()
                      << ", 职称: " << level
                      << ", 科室: " << current->department
                      << ", 联系方式: " << current->getTelephone() << " / " << current->getEmail()
                      << ", 排班信息：" << current->scheduleInfo
                      << ", 在岗状态: " << (current->isOnDuty ? "在岗" : "不在岗")
                      << ", 累计护理人数: " << current->patientCareCount
                      << ", 床位管理次数: " << current->bedManageCount
                      << std::endl;
            found = true;
            break;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到ID为 " << nurseID << " 的护士信息！" << std::endl;
    }
    return found;
}
// 根据护士姓名查看护士信息
bool Admin::viewNursesByName(Nurse *&nurse, const std::string &department)
{
    std::string name = inputStringCheck("请输入护士姓名（支持模糊查询）: ");

    Nurse *current = nurse;
    bool found = false;
    std::cout << "正在查找姓名包含 \"" << name << "\" 的护士信息..." << std::endl;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->getUsername().find(name) != std::string::npos && deptMatch(current->department, department))
        {
            std::string level = nurseLevelToString(current->level);

            std::cout << "护士ID: " << current->getUserID()
                      << ", 姓名: " << current->getUsername()
                      << ",性别: " << current->getGender()
                      << ", 职称: " << level
                      << ", 科室: " << current->department
                      << ", 联系方式: " << current->getTelephone() << " / " << current->getEmail()
                      << ", 排班信息：" << current->scheduleInfo
                      << ", 在岗状态: " << (current->isOnDuty ? "在岗" : "不在岗")
                      << ", 累计护理人数: " << current->patientCareCount
                      << ", 床位管理次数: " << current->bedManageCount
                      << std::endl;
            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到姓名包含 \"" << name << "\" 的护士信息！" << std::endl;
    }
    return found;
}
// 根据护士在岗状态查看护士信息
bool Admin::viewNursesByOnDutyStatus(Nurse *&nurse, const std::string &department)
{

    bool found = false;

    clearScreen();
    printMenuBorder();
    printMenuTitle("护士在岗状态筛选");
    printMenuItem(1, "在岗");
    printMenuItem(2, "不在岗");
    printMenuItem(0, "返回上一级菜单");
    printMenuBottom();
    int onDutyFilter = selectIntCheck(0, 2);

    if (onDutyFilter == 0)
    {
        return false; // 返回上一级菜单
    }

    bool filterOnDutyStatus = (onDutyFilter == 1); // 1表示在岗，2表示不在岗

    Nurse *current = nurse;
    std::cout << "正在查找在岗状态为: " << (filterOnDutyStatus ? "在岗" : "不在岗") << " 的护士信息..." << std::endl;
    while (current != nullptr)
    {
        if (!current->isDeleted && deptMatch(current->department, department) && current->isOnDuty == filterOnDutyStatus)
        {
            std::string level = nurseLevelToString(current->level);
            std::cout << "护士ID: " << current->getUserID()
                      << ", 姓名: " << current->getUsername()
                      << ",性别: " << current->getGender()
                      << ", 职称: " << level
                      << ", 科室: " << current->department
                      << ", 联系方式: " << current->getTelephone() << " / " << current->getEmail()
                      << ", 排班信息：" << current->scheduleInfo
                      << ", 在岗状态: " << (current->isOnDuty ? "在岗" : "不在岗")
                      << ", 累计护理人数: " << current->patientCareCount
                      << ", 床位管理次数: " << current->bedManageCount
                      << std::endl;
            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到在岗状态为 " << (filterOnDutyStatus ? "在岗" : "不在岗") << " 的护士信息！" << std::endl;
    }
    return found;
}
// 根据护士等级查看护士信息
bool Admin::viewNurseByLevel(Nurse *&nurse, const std::string &department)
{
    int levelFilter = NurseTitleMenu(); // 选择护士职称过滤条件

    if (levelFilter == 0)
    {
        return false; // 返回上一级菜单
    }

    bool found = false;

    NurseLevel filterLevel = static_cast<NurseLevel>(levelFilter);

    Nurse *current = nurse;
    std::cout << "正在查找等级为: " << nurseLevelToString(filterLevel) << " 的护士信息..." << std::endl;
    while (current != nullptr)
    {
        if (!current->isDeleted && deptMatch(current->department, department) && current->level == filterLevel)
        {
            std::cout << "护士ID: " << current->getUserID()
                      << ", 姓名: " << current->getUsername()
                      << ",性别: " << current->getGender()
                      << ", 职称: " << nurseLevelToString(current->level)
                      << ", 科室: " << current->department
                      << ", 联系方式: " << current->getTelephone() << " / " << current->getEmail()
                      << ", 排班信息：" << current->scheduleInfo
                      << ", 在岗状态: " << (current->isOnDuty ? "在岗" : "不在岗")
                      << ", 累计护理人数: " << current->patientCareCount
                      << ", 床位管理次数: " << current->bedManageCount
                      << std::endl;
            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到等级为 " << nurseLevelToString(filterLevel) << " 的护士信息！" << std::endl;
    }
    return found;
}
// 修改护士姓名
void Admin::modifyNurseName(Nurse *&target, const std::string &department)
{
    std::cout << "当前护士姓名: " << target->getUsername() << std::endl;
    std::string newName = inputStringCheck("请输入新的护士姓名: ");
    target->setUsername(newName);
    std::cout << "护士姓名已更新！" << std::endl;
}
// 修改护士性别
void Admin::modifyNurseGender(Nurse *&target, const std::string &department)
{
    std::cout << "当前护士性别: " << target->getGender() << std::endl;
    std::string newGender = inputGenderCheck("请输入新的护士性别: ");
    target->setGender(newGender);
    std::cout << "护士性别已更新！" << std::endl;
}
// 修改护士年龄
void Admin::modifyNurseAge(Nurse *&target, const std::string &department)
{
    std::cout << "当前护士年龄: " << target->getAge() << std::endl;
    int newAge = inputAgeCheck("请输入新的护士年龄: ");
    target->setAge(newAge);
    std::cout << "护士年龄已更新！" << std::endl;
}
// 修改护士所属科室
void Admin::modifyNurseDepartment(Nurse *&target, const std::string &department)
{
    std::cout << "当前护士所属科室: " << target->department << std::endl;
    std::string newDepartment = adminDepartmentMenu();

    if (newDepartment == "0")
    {
        std::cout << "科室修改已取消！" << std::endl;
        return; // 取消修改，返回上一级菜单
    }
    else if (newDepartment.empty())
    {
        printWarning("输入无效，科室修改已取消！");
        return; // 输入无效，取消修改，返回上一级菜单
    }

    target->department = newDepartment;
    std::cout << "护士所属科室已更新！" << std::endl;
}
// 修改护士等级
void Admin::modifyNurseLevel(Nurse *&target, const std::string &department)
{
    std::cout << "当前护士等级: " << nurseLevelToString(target->level) << std::endl;
    int newLevel = NurseTitleMenu(); // 选择新的护士等级

    if (newLevel == 0)
    {
        std::cout << "等级修改已取消！" << std::endl;
        return; // 取消修改，返回上一级菜单
    }

    target->level = static_cast<NurseLevel>(newLevel);
    std::cout << "护士等级已更新！" << std::endl;
}
// 修改护士电话
void Admin::modifyNurseTelephone(Nurse *&target, const std::string &department)
{
    std::cout << "当前护士电话: " << target->getTelephone() << std::endl;
    std::string newTelephone = inputTelephoneCheck("请输入新的护士电话: ");
    target->setTelephone(newTelephone);
    std::cout << "护士电话已更新！" << std::endl;
}
// 修改护士邮箱
void Admin::modifyNurseEmail(Nurse *&target, const std::string &department)
{
    std::cout << "当前护士邮箱: " << target->getEmail() << std::endl;
    std::string newEmail = inputEmailCheck("请输入新的护士邮箱: ");
    target->setEmail(newEmail);
    std::cout << "护士邮箱已更新！" << std::endl;
}
// 修改护士在岗状态
void Admin::modifyNurseOnDutyStatus(Nurse *&target, const std::string &department)
{
    std::cout << "当前护士在岗状态: " << (target->isOnDuty ? "在岗" : "不在岗") << std::endl;
    clearScreen();
    printMenuBorder();
    printMenuTitle("选择在岗状态");
    printMenuItem(1, "在岗");
    printMenuItem(2, "不在岗");
    printMenuItem(0, "返回上一级菜单");
    printMenuBottom();

    int onDutyChoice = selectIntCheck(0, 2);
    if (onDutyChoice == 0)
    {
        std::cout << "在岗状态修改已取消！" << std::endl;
        return;
    }
    target->isOnDuty = (onDutyChoice == 1); // 1表示在岗，2表示不在岗
    std::cout << "护士在岗状态已更新！" << std::endl;
}
// 修改护士排班信息
void Admin::modifyNurseScheduleInfo(Nurse *&target, const std::string &department)
{
    std::cout << "当前护士排班信息: " << target->scheduleInfo << std::endl;
    std::string newScheduleInfo = inputStringCheck("请输入新的护士排班信息: ");
    target->scheduleInfo = newScheduleInfo;
    std::cout << "护士排班信息已更新！" << std::endl;
}
// 修改护士累计护理人数
void Admin::modifyNursePatientCareCount(Nurse *&target, const std::string &department)
{
    std::cout << "当前护士累计护理人数: " << target->patientCareCount << std::endl;
    std::cout << "输入新的累计护理人数 ";
    int newPatientCareCount = selectIntCheck(0, INT_MAX);
    target->patientCareCount = newPatientCareCount;
    std::cout << "护士累计护理人数已更新！" << std::endl;
}
// 修改护士床位管理次数
void Admin::modifyNurseBedManageCount(Nurse *&target, const std::string &department)
{
    std::cout << "当前护士床位管理次数: " << target->bedManageCount << std::endl;
    std::cout << "输入新的床位管理次数 ";
    int newBedManageCount = selectIntCheck(0, INT_MAX);
    target->bedManageCount = newBedManageCount;
    std::cout << "护士床位管理次数已更新！" << std::endl;
}
// 删除护士（逻辑删除，设置 isDeleted 标志）
void Admin::deleteNurse(Nurse *&target, const std::string &department)
{
    target->isDeleted = true; // 逻辑删除
    std::cout << "护士ID: " << target->getUserID() << " 已删除！" << std::endl;
}
// 添加护士（根据输入信息创建新的 Nurse 对象，并插入到链表中）
bool Admin::addNurse(Nurse *&nurse, int &idCounter)
{
    Nurse *newNurse = new Nurse();
    bool success = newNurse->nurseSignUp(idCounter);
    if (!success)
    {
        delete newNurse;
        return false;
    }
    // 插入到链表头部
    newNurse->next = nurse;
    if (nurse != nullptr)
    {
        nurse->prev = newNurse;
    }
    nurse = newNurse;
    return true;
}

void Admin::manageNurses(Nurse *&nurse, const std::string &department, int &idCounter)
{
    while (true)
    {
        int choice = adminNurseManagementMenu();

        if (choice == 1) // 查看护士信息
        {
            while (true)
            {
                int viewChoice = adminNurseViewMenu();
                if (viewChoice == 0) // 返回上一级菜单
                {
                    break;
                }
                else if (viewChoice == 1) // 查看所有护士信息
                {
                    viewAllNurses(nurse, department);
                    pause("管理员 > 账户管理 > 护士管理");
                }
                else if (viewChoice == 2) // 根据护士ID查看护士信息
                {
                    viewNurseByID(nurse, department);
                    pause("管理员 > 账户管理 > 护士管理");
                }
                else if (viewChoice == 3) // 根据护士姓名查看护士信息
                {
                    viewNursesByName(nurse, department);
                    pause("管理员 > 账户管理 > 护士管理");
                }
                else if (viewChoice == 4) // 根据护士在岗状态查看护士信息
                {
                    viewNursesByOnDutyStatus(nurse, department);
                    pause("管理员 > 账户管理 > 护士管理");
                }
                else if (viewChoice == 5) // 根据护士等级查看护士信息
                {
                    viewNurseByLevel(nurse, department);
                    pause("管理员 > 账户管理 > 护士管理");
                }
            }
        }
        else if (choice == 2) // 修改护士信息
        {
            bool ishave = viewAllNurses(nurse, department);

            if (ishave)
            {
                std::string nurseID = inputIDCheck("请输入要修改的护士ID: ");

                Nurse *target = nurse;
                while (target != nullptr)
                {
                    if (!target->isDeleted && target->getUserID() == nurseID && deptMatch(target->department, department))
                    {
                        break;
                    }
                    target = target->next;
                }

                if (target)
                {
                    while (true)
                    {
                        std::cout << "正在修改的护士信息：" << std::endl;
                        std::cout << "护士ID: " << target->getUserID()
                                  << ", 姓名: " << target->getUsername()
                                  << ",性别: " << target->getGender()
                                  << ", 职称: " << nurseLevelToString(target->level)
                                  << ", 科室: " << target->department
                                  << ", 联系方式: " << target->getTelephone() << " / " << target->getEmail()
                                  << ", 排班信息：" << target->scheduleInfo
                                  << ", 在岗状态: " << (target->isOnDuty ? "在岗" : "不在岗")
                                  << ", 累计护理人数: " << target->patientCareCount
                                  << ", 床位管理次数: " << target->bedManageCount
                                  << std::endl;

                        int modifyChoice = adminNurseModificationMenu();

                        if (modifyChoice == 0) // 返回上一级菜单
                        {
                            break;
                        }
                        else if (modifyChoice == 1) // 修改护士姓名
                        {
                            modifyNurseName(target, department);
                            pause("管理员 > 账户管理 > 护士管理");
                        }
                        else if (modifyChoice == 2) // 修改护士性别
                        {
                            modifyNurseGender(target, department);
                            pause("管理员 > 账户管理 > 护士管理");
                        }
                        else if (modifyChoice == 3) // 修改护士年龄
                        {
                            modifyNurseAge(target, department);
                            pause("管理员 > 账户管理 > 护士管理");
                        }
                        else if (modifyChoice == 4) // 修改护士所属科室
                        {
                            modifyNurseDepartment(target, department);
                            pause("管理员 > 账户管理 > 护士管理");
                        }
                        else if (modifyChoice == 5) // 修改护士等级
                        {
                            modifyNurseLevel(target, department);
                            pause("管理员 > 账户管理 > 护士管理");
                        }
                        else if (modifyChoice == 6) // 修改护士电话
                        {
                            modifyNurseTelephone(target, department);
                            pause("管理员 > 账户管理 > 护士管理");
                        }
                        else if (modifyChoice == 7) // 修改护士邮箱
                        {
                            modifyNurseEmail(target, department);
                            pause("管理员 > 账户管理 > 护士管理");
                        }
                        else if (modifyChoice == 8) // 修改护士在岗状态
                        {
                            modifyNurseOnDutyStatus(target, department);
                            pause("管理员 > 账户管理 > 护士管理");
                        }
                        else if (modifyChoice == 9) // 修改护士排班信息
                        {
                            modifyNurseScheduleInfo(target, department);
                            pause("管理员 > 账户管理 > 护士管理");
                        }
                        else if (modifyChoice == 10) // 修改护士累计护理人数
                        {
                            modifyNursePatientCareCount(target, department);
                            pause("管理员 > 账户管理 > 护士管理");
                        }
                        else if (modifyChoice == 11) // 修改护士床位管理次数
                        {
                            modifyNurseBedManageCount(target, department);
                            pause("管理员 > 账户管理 > 护士管理");
                        }
                    }
                }
                else
                {
                    std::cout << "未找到ID为 " << nurseID << " 的护士信息！" << std::endl;
                    pause("管理员 > 账户管理 > 护士管理");
                }
            }
        }
        else if (choice == 3) // 删除护士
        {
            bool ishave = viewAllNurses(nurse, department);

            if (ishave)
            {
                std::string nurseID = inputIDCheck("请输入要删除的护士ID: ");

                Nurse *target = nurse;
                while (target != nullptr)
                {
                    if (!target->isDeleted && target->getUserID() == nurseID && deptMatch(target->department, department))
                    {
                        break;
                    }
                    target = target->next;
                }

                if (target)
                {
                    std::cout << "正在删除的护士信息如下：" << std::endl;
                    std::cout << "护士ID: " << target->getUserID()
                              << ", 姓名: " << target->getUsername()
                              << ",性别: " << target->getGender()
                              << ", 职称: " << nurseLevelToString(target->level)
                              << ", 科室: " << target->department
                              << ", 联系方式: " << target->getTelephone() << " / " << target->getEmail()
                              << ", 排班信息：" << target->scheduleInfo
                              << ", 在岗状态: " << (target->isOnDuty ? "在岗" : "不在岗")
                              << ", 累计护理人数: " << target->patientCareCount
                              << ", 床位管理次数: " << target->bedManageCount
                              << std::endl;

                    clearScreen();
                    printMenuBorder();
                    printMenuTitle("确认删除护士");
                    printMenuItem(1, "确认");
                    printMenuItem(0, "取消");
                    printMenuBottom();

                    int confirmChoice = selectIntCheck(0, 1);
                    if (confirmChoice == 1)
                    {

                        deleteNurse(target, department);
                        pause("管理员 > 账户管理 > 护士管理");
                    }
                    else
                    {
                        std::cout << "护士删除已取消！" << std::endl;
                        pause("管理员 > 账户管理 > 护士管理");
                    }
                }
                else
                {
                    std::cout << "未找到ID为 " << nurseID << " 的护士信息！" << std::endl;
                    pause("管理员 > 账户管理 > 护士管理");
                }
            }
        }
        else if (choice == 4) // 添加护士
        {
            addNurse(nurse, idCounter);
            pause("管理员 > 账户管理 > 护士管理");
        }
        else if (choice == 0) // 返回上一级菜单
        {
            break;
        }
    }
}

// =================================== 药剂师信息管理 =================================

// 查看所有的药剂师信息
bool Admin::viewAllPharmacists(Pharmacist *&pharmacist, const std::string &department)
{
    Pharmacist *current = pharmacist;
    bool found = false;
    std::cout << "正在查找科室 " << department << " 的药剂师信息..." << std::endl;
    while (current != nullptr)
    {
        if (!current->isDeleted && deptMatch(current->department, department))
        {
            std::cout << "药剂师ID: " << current->getUserID()
                      << ", 姓名: " << current->getUsername()
                      << ",性别: " << current->getGender()
                      << ", 科室: " << current->department
                      << ", 职称: " << pharmacistLevelToString(current->level)
                      << ", 排班信息：" << current->scheduleInfo
                      << ", 联系方式: " << current->getTelephone() << " / " << current->getEmail()
                      << ", 在岗状态: " << (current->isOnDuty ? "在岗" : "不在岗")
                      << ", 累计审核用药记录数量: " << current->reviewCount
                      << ", 累计发药数量: " << current->dispenseCount
                      << ", 累计库存管理次数: " << current->inventoryManageCount
                      << std::endl;
            found = true;
            std::cout << std::endl;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "暂无药剂师信息！" << std::endl;
    }
    return found;
}
// 根据药剂师ID查看药剂师信息
bool Admin::viewPharmacistByID(Pharmacist *&pharmacist, const std::string &department)
{
    std::string pharmacistID = inputIDCheck("请输入要查找的药剂师ID: ");

    Pharmacist *current = pharmacist;
    bool found = false;
    std::cout << "正在查找ID: " << pharmacistID << " 的药剂师信息..." << std::endl;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->getUserID() == pharmacistID && deptMatch(current->department, department))
        {
            std::cout << "药剂师ID: " << current->getUserID()
                      << ", 姓名: " << current->getUsername()
                      << ",性别: " << current->getGender()
                      << ", 科室: " << current->department
                      << ", 职称: " << pharmacistLevelToString(current->level)
                      << ", 排班信息：" << current->scheduleInfo
                      << ", 联系方式: " << current->getTelephone() << " / " << current->getEmail()
                      << ", 在岗状态: " << (current->isOnDuty ? "在岗" : "不在岗")
                      << ", 累计审核用药记录数量: " << current->reviewCount
                      << ", 累计发药数量: " << current->dispenseCount
                      << ", 累计库存管理次数: " << current->inventoryManageCount
                      << std::endl;
            found = true;
            break;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到ID为 " << pharmacistID << " 的药剂师信息！" << std::endl;
    }
    return found;
}
// 根据药剂师姓名查看药剂师信息
bool Admin::viewPharmacistsByName(Pharmacist *&pharmacist, const std::string &department)
{
    std::string name = inputStringCheck("请输入药剂师姓名（支持模糊查询）: ");

    Pharmacist *current = pharmacist;
    bool found = false;
    std::cout << "正在查找姓名包含 \"" << name << "\" 的药剂师信息..." << std::endl;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->getUsername().find(name) != std::string::npos && deptMatch(current->department, department))
        {
            std::cout << "药剂师ID: " << current->getUserID()
                      << ", 姓名: " << current->getUsername()
                      << ",性别: " << current->getGender()
                      << ", 科室: " << current->department
                      << ", 职称: " << pharmacistLevelToString(current->level)
                      << ", 排班信息：" << current->scheduleInfo
                      << ", 联系方式: " << current->getTelephone() << " / " << current->getEmail()
                      << ", 在岗状态: " << (current->isOnDuty ? "在岗" : "不在岗")
                      << ", 累计审核用药记录数量: " << current->reviewCount
                      << ", 累计发药数量: " << current->dispenseCount
                      << ", 累计库存管理次数: " << current->inventoryManageCount
                      << std::endl;
            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到姓名包含 \"" << name << "\" 的药剂师信息！" << std::endl;
    }
    return found;
}
// 根据药剂师在岗状态查看药剂师信息
bool Admin::viewPharmacistsByOnDutyStatus(Pharmacist *&pharmacist, const std::string &department)
{

    bool found = false;

    clearScreen();
    printMenuBorder();
    printMenuTitle("药剂师在岗状态筛选");
    printMenuItem(1, "在岗");
    printMenuItem(2, "不在岗");
    printMenuItem(0, "返回上一级菜单");
    printMenuBottom();
    int onDutyFilter = selectIntCheck(0, 2);

    if (onDutyFilter == 0)
    {
        return false; // 返回上一级菜单
    }

    bool filterOnDutyStatus = (onDutyFilter == 1); // 1表示在岗，2表示不在岗

    Pharmacist *current = pharmacist;
    std::cout << "正在查找在岗状态为: " << (filterOnDutyStatus ? "在岗" : "不在岗") << " 的药剂师信息..." << std::endl;
    while (current != nullptr)
    {
        if (!current->isDeleted && deptMatch(current->department, department) && current->isOnDuty == filterOnDutyStatus)
        {
            std::cout << "药剂师ID: " << current->getUserID()
                      << ", 姓名: " << current->getUsername()
                      << ",性别: " << current->getGender()
                      << ", 科室: " << current->department
                      << ", 职称: " << pharmacistLevelToString(current->level)
                      << ", 排班信息：" << current->scheduleInfo
                      << ", 联系方式: " << current->getTelephone() << " / " << current->getEmail()
                      << ", 在岗状态: " << (current->isOnDuty ? "在岗" : "不在岗")
                      << ", 累计审核用药记录数量: " << current->reviewCount
                      << ", 累计发药数量: " << current->dispenseCount
                      << ", 累计库存管理次数: " << current->inventoryManageCount
                      << std::endl;
            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到在岗状态为 " << (filterOnDutyStatus ? "在岗" : "不在岗") << " 的药剂师信息！" << std::endl;
    }
    return found;
}
// 根据药剂师职称查看药剂师信息
bool Admin::viewPharmacistByLevel(Pharmacist *&pharmacist, const std::string &department)
{
    int levelFilter = PharmacistTitleMenu(); // 选择药剂师职称过滤条件

    if (levelFilter == 0)
    {
        return false; // 返回上一级菜单
    }

    bool found = false;

    PharmacistLevel filterLevel = static_cast<PharmacistLevel>(levelFilter);

    Pharmacist *current = pharmacist;
    std::cout << "正在查找职称为: " << pharmacistLevelToString(filterLevel) << " 的药剂师信息..." << std::endl;
    while (current != nullptr)
    {
        if (!current->isDeleted && deptMatch(current->department, department) && current->level == filterLevel)
        {
            std::cout << "药剂师ID: " << current->getUserID()
                      << ", 姓名: " << current->getUsername()
                      << ",性别: " << current->getGender()
                      << ", 科室: " << current->department
                      << ", 职称: " << pharmacistLevelToString(current->level)
                      << ", 排班信息：" << current->scheduleInfo
                      << ", 联系方式: " << current->getTelephone() << " / " << current->getEmail()
                      << ", 在岗状态: " << (current->isOnDuty ? "在岗" : "不在岗")
                      << ", 累计审核用药记录数量: " << current->reviewCount
                      << ", 累计发药数量: " << current->dispenseCount
                      << ", 累计库存管理次数: " << current->inventoryManageCount
                      << std::endl;
            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到职称为 " << pharmacistLevelToString(filterLevel) << " 的药剂师信息！" << std::endl;
    }
    return found;
}
// 修改药剂师姓名
void Admin::modifyPharmacistName(Pharmacist *&target, const std::string &department)
{
    std::cout << "当前药剂师姓名: " << target->getUsername() << std::endl;
    std::string newName = inputStringCheck("请输入新的药剂师姓名: ");
    target->setUsername(newName);
    std::cout << "药剂师姓名已更新！" << std::endl;
}
// 修改药剂师性别
void Admin::modifyPharmacistGender(Pharmacist *&target, const std::string &department)
{
    std::cout << "当前药剂师性别: " << target->getGender() << std::endl;
    std::string newGender = inputGenderCheck("请输入新的药剂师性别: ");
    target->setGender(newGender);
    std::cout << "药剂师性别已更新！" << std::endl;
}
// 修改药剂师年龄
void Admin::modifyPharmacistAge(Pharmacist *&target, const std::string &department)
{
    std::cout << "当前药剂师年龄: " << target->getAge() << std::endl;
    int newAge = inputAgeCheck("请输入新的药剂师年龄: ");
    target->setAge(newAge);
    std::cout << "药剂师年龄已更新！" << std::endl;
}
// 修改药剂师所属科室
void Admin::modifyPharmacistDepartment(Pharmacist *&target, const std::string &department)
{
    std::cout << "当前药剂师所属科室: " << target->department << std::endl;
    std::string newDepartment = adminDepartmentMenu();

    if (newDepartment == "0")
    {
        std::cout << "科室修改已取消！" << std::endl;
        return; // 取消修改，返回上一级菜单
    }
    else if (newDepartment.empty())
    {
        printWarning("输入无效，科室修改已取消！");
        return; // 输入无效，取消修改，返回上一级菜单
    }

    target->department = newDepartment;
    std::cout << "药剂师所属科室已更新！" << std::endl;
}
// 修改药剂师职称
void Admin::modifyPharmacistLevel(Pharmacist *&target, const std::string &department)
{
    std::cout << "当前药剂师职称: " << pharmacistLevelToString(target->level) << std::endl;
    int newLevel = PharmacistTitleMenu(); // 选择新的药剂师职称

    if (newLevel == 0)
    {
        std::cout << "职称修改已取消！" << std::endl;
        return; // 取消修改，返回上一级菜单
    }

    target->level = static_cast<PharmacistLevel>(newLevel);
    std::cout << "药剂师职称已更新！" << std::endl;
}
// 修改药剂师电话
void Admin::modifyPharmacistTelephone(Pharmacist *&target, const std::string &department)
{
    std::cout << "当前药剂师电话: " << target->getTelephone() << std::endl;
    std::string newTelephone = inputTelephoneCheck("请输入新的药剂师电话: ");
    target->setTelephone(newTelephone);
    std::cout << "药剂师电话已更新！" << std::endl;
}
// 修改药剂师邮箱
void Admin::modifyPharmacistEmail(Pharmacist *&target, const std::string &department)
{
    std::cout << "当前药剂师邮箱: " << target->getEmail() << std::endl;
    std::string newEmail = inputEmailCheck("请输入新的药剂师邮箱: ");
    target->setEmail(newEmail);
    std::cout << "药剂师邮箱已更新！" << std::endl;
}
// 修改药剂师在岗状态
void Admin::modifyPharmacistOnDutyStatus(Pharmacist *&target, const std::string &department)
{
    std::cout << "当前药剂师在岗状态: " << (target->isOnDuty ? "在岗" : "不在岗") << std::endl;
    clearScreen();
    printMenuBorder();
    printMenuTitle("选择在岗状态");
    printMenuItem(1, "在岗");
    printMenuItem(2, "不在岗");
    printMenuItem(0, "返回上一级菜单");
    printMenuBottom();

    int onDutyChoice = selectIntCheck(0, 2);
    if (onDutyChoice == 0)
    {
        std::cout << "在岗状态修改已取消！" << std::endl;
        return;
    }
    target->isOnDuty = (onDutyChoice == 1); // 1表示在岗，2表示不在岗
    std::cout << "药剂师在岗状态已更新！" << std::endl;
}
// 修改药剂师排班信息
void Admin::modifyPharmacistScheduleInfo(Pharmacist *&target, const std::string &department)
{
    std::cout << "当前药剂师排班信息: " << target->scheduleInfo << std::endl;
    std::string newScheduleInfo = inputStringCheck("请输入新的药剂师排班信息: ");
    target->scheduleInfo = newScheduleInfo;
    std::cout << "药剂师排班信息已更新！" << std::endl;
}
// 修改药剂师累计审核用药记录数量
void Admin::modifyPharmacistMedicationReviewCount(Pharmacist *&target, const std::string &department)
{
    std::cout << "当前药剂师累计审核用药记录数量: " << target->reviewCount << std::endl;
    std::cout << "输入新的累计审核用药记录数量 ";
    int newReviewCount = selectIntCheck(0, INT_MAX);
    target->reviewCount = newReviewCount;
    std::cout << "药剂师累计审核用药记录数量已更新！" << std::endl;
}
// 修改药剂师累计发药数量
void Admin::modifyPharmacistDispenseCount(Pharmacist *&target, const std::string &department)
{
    std::cout << "当前药剂师累计发药数量: " << target->dispenseCount << std::endl;
    std::cout << "输入新的累计发药数量 ";
    int newDispenseCount = selectIntCheck(0, INT_MAX);
    target->dispenseCount = newDispenseCount;
    std::cout << "药剂师累计发药数量已更新！" << std::endl;
}
// 修改药剂师累计库存管理次数
void Admin::modifyPharmacistInventoryManageCount(Pharmacist *&target, const std::string &department)
{
    std::cout << "当前药剂师累计库存管理次数: " << target->inventoryManageCount << std::endl;
    std::cout << "输入新的累计库存管理次数 ";
    int newInventoryManageCount = selectIntCheck(0, INT_MAX);
    target->inventoryManageCount = newInventoryManageCount;
    std::cout << "药剂师累计库存管理次数已更新！" << std::endl;
}
// 删除药剂师（逻辑删除，设置 isDeleted 标志）
void Admin::deletePharmacist(Pharmacist *&target, const std::string &department)
{
    target->isDeleted = true; // 逻辑删除
    std::cout << "药剂师ID: " << target->getUserID() << " 已删除！" << std::endl;
}
// 添加药剂师（根据输入信息创建新的 Pharmacist 对象，并插入到链表中）
bool Admin::addPharmacist(Pharmacist *&pharmacist, int &idCounter)
{
    Pharmacist *newPharmacist = new Pharmacist();
    bool success = newPharmacist->pharmacistSignUp(idCounter);
    if (!success)
    {
        delete newPharmacist;
        return false;
    }
    // 插入到链表头部
    newPharmacist->next = pharmacist;
    if (pharmacist != nullptr)
    {
        pharmacist->prev = newPharmacist;
    }
    pharmacist = newPharmacist;
    return true;
}

void Admin::managePharmacists(Pharmacist *&pharmacist, const std::string &department, int &idCounter)
{
    while (true)
    {
        int choice = adminPharmacistManagementMenu();

        if (choice == 1) // 查看药剂师信息
        {
            while (true)
            {
                int viewChoice = adminPharmacistViewMenu();
                if (viewChoice == 0) // 返回上一级菜单
                {
                    break;
                }
                else if (viewChoice == 1) // 查看所有药剂师信息
                {
                    viewAllPharmacists(pharmacist, department);
                    pause("管理员 > 账户管理 > 药剂师管理");
                }
                else if (viewChoice == 2) // 根据药剂师ID查看药剂师信息
                {
                    viewPharmacistByID(pharmacist, department);
                    pause("管理员 > 账户管理 > 药剂师管理");
                }
                else if (viewChoice == 3) // 根据药剂师姓名查看药剂师信息
                {
                    viewPharmacistsByName(pharmacist, department);
                    pause("管理员 > 账户管理 > 药剂师管理");
                }
                else if (viewChoice == 4) // 根据药剂师在岗状态查看药剂师信息
                {
                    viewPharmacistsByOnDutyStatus(pharmacist, department);
                    pause("管理员 > 账户管理 > 药剂师管理");
                }
                else if (viewChoice == 5) // 根据药剂师职称查看药剂师信息
                {
                    viewPharmacistByLevel(pharmacist, department);
                    pause("管理员 > 账户管理 > 药剂师管理");
                }
            }
        }
        else if (choice == 2) // 修改药剂师信息
        {
            bool ishave = viewAllPharmacists(pharmacist, department);

            if (ishave)
            {
                std::string pharmacistID = inputIDCheck("请输入要修改的药剂师ID: ");

                Pharmacist *target = pharmacist;
                while (target != nullptr)
                {
                    if (!target->isDeleted && target->getUserID() == pharmacistID && deptMatch(target->department, department))
                    {
                        break;
                    }
                    target = target->next;
                }

                if (target)
                {
                    while (true)
                    {
                        std::cout << "正在修改的药剂师信息：" << std::endl;
                        std::cout << "药剂师ID: " << target->getUserID()
                                  << ", 姓名: " << target->getUsername()
                                  << ",性别: " << target->getGender()
                                  << ", 科室: " << target->department
                                  << ", 职称: " << pharmacistLevelToString(target->level)
                                  << ", 排班信息：" << target->scheduleInfo
                                  << ", 联系方式: " << target->getTelephone() << " / " << target->getEmail()
                                  << ", 在岗状态: " << (target->isOnDuty ? "在岗" : "不在岗")
                                  << ", 累计审核用药记录数量: " << target->reviewCount
                                  << ", 累计发药数量: " << target->dispenseCount
                                  << ", 累计库存管理次数: " << target->inventoryManageCount
                                  << std::endl;

                        int modifyChoice = adminPharmacistModificationMenu();

                        if (modifyChoice == 0) // 返回上一级菜单
                        {
                            break;
                        }
                        else if (modifyChoice == 1) // 修改药剂师姓名
                        {
                            modifyPharmacistName(target, department);
                            pause("管理员 > 账户管理 > 药剂师管理");
                        }
                        else if (modifyChoice == 2) // 修改药剂师性别
                        {
                            modifyPharmacistGender(target, department);
                            pause("管理员 > 账户管理 > 药剂师管理");
                        }
                        else if (modifyChoice == 3) // 修改药剂师年龄
                        {
                            modifyPharmacistAge(target, department);
                            pause("管理员 > 账户管理 > 药剂师管理");
                        }
                        else if (modifyChoice == 4) // 修改药剂师所属科室
                        {
                            modifyPharmacistDepartment(target, department);
                            pause("管理员 > 账户管理 > 药剂师管理");
                        }
                        else if (modifyChoice == 5) // 修改药剂师职称
                        {
                            modifyPharmacistLevel(target, department);
                            pause("管理员 > 账户管理 > 药剂师管理");
                        }
                        else if (modifyChoice == 6) // 修改药剂师电话
                        {
                            modifyPharmacistTelephone(target, department);
                            pause("管理员 > 账户管理 > 药剂师管理");
                        }
                        else if (modifyChoice == 7) // 修改药剂师邮箱
                        {
                            modifyPharmacistEmail(target, department);
                            pause("管理员 > 账户管理 > 药剂师管理");
                        }
                        else if (modifyChoice == 8) // 修改药剂师在岗状态
                        {
                            modifyPharmacistOnDutyStatus(target, department);
                            pause("管理员 > 账户管理 > 药剂师管理");
                        }
                        else if (modifyChoice == 9) // 修改药剂师排班信息
                        {
                            modifyPharmacistScheduleInfo(target, department);
                            pause("管理员 > 账户管理 > 药剂师管理");
                        }
                        else if (modifyChoice == 10) // 修改药剂师累计审核用药记录数量
                        {
                            modifyPharmacistMedicationReviewCount(target, department);
                            pause("管理员 > 账户管理 > 药剂师管理");
                        }
                        else if (modifyChoice == 11) // 修改药剂师累计发药数量
                        {
                            modifyPharmacistDispenseCount(target, department);
                            pause("管理员 > 账户管理 > 药剂师管理");
                        }
                        else if (modifyChoice == 12) // 修改药剂师累计库存管理次数
                        {
                            modifyPharmacistInventoryManageCount(target, department);
                            pause("管理员 > 账户管理 > 药剂师管理");
                        }
                    }
                }
                else
                {
                    std::cout << "未找到ID为 " << pharmacistID << " 的药剂师信息！" << std::endl;
                    pause("管理员 > 账户管理 > 药剂师管理");
                }
            }
        }
        else if (choice == 3) // 删除药剂师
        {
            bool ishave = viewAllPharmacists(pharmacist, department);

            if (ishave)
            {
                std::string pharmacistID = inputIDCheck("请输入要删除的药剂师ID: ");

                Pharmacist *target = pharmacist;
                while (target != nullptr)
                {
                    if (!target->isDeleted && target->getUserID() == pharmacistID && deptMatch(target->department, department))
                    {
                        break;
                    }
                    target = target->next;
                }

                if (target)
                {
                    std::cout << "正在删除的药剂师信息如下：" << std::endl;
                    std::cout << "药剂师ID: " << target->getUserID()
                              << ", 姓名: " << target->getUsername()
                              << ",性别: " << target->getGender()
                              << ", 科室: " << target->department
                              << ", 职称: " << pharmacistLevelToString(target->level)
                              << ", 排班信息：" << target->scheduleInfo
                              << ", 联系方式: " << target->getTelephone() << " / " << target->getEmail()
                              << ", 在岗状态: " << (target->isOnDuty ? "在岗" : "不在岗")
                              << ", 累计审核用药记录数量: " << target->reviewCount
                              << ", 累计发药数量: " << target->dispenseCount
                              << ", 累计库存管理次数: " << target->inventoryManageCount
                              << std::endl;

                    clearScreen();
                    printMenuBorder();
                    printMenuTitle("确认删除药剂师");
                    printMenuItem(1, "确定");
                    printMenuItem(0, "取消");
                    printMenuBottom();

                    int confirmChoice = selectIntCheck(0, 1);

                    if (confirmChoice == 1)
                    {
                        deletePharmacist(target, department);
                        pause("管理员 > 账户管理 > 药剂师管理");
                    }
                    else if (confirmChoice == 0)
                    {
                        std::cout << "药剂师删除已取消！" << std::endl;
                        pause("管理员 > 账户管理 > 药剂师管理");
                    }
                }
                else
                {
                    std::cout << "未找到ID为 " << pharmacistID << " 的药剂师信息！" << std::endl;
                    pause("管理员 > 账户管理 > 药剂师管理");
                }
            }
        }
        else if (choice == 4) // 添加药剂师
        {
            addPharmacist(pharmacist, idCounter);
            pause("管理员 > 账户管理 > 药剂师管理");
        }
        else if (choice == 0) // 返回上一级菜单
        {
            break;
        }
    }
}

// =================================== 患者信息管理 =================================

// 查看所有患者的信息
bool Admin::viewAllPatients(Patient *&patient, const std::string &department)
{
    Patient *current = patient;
    bool found = false;
    std::cout << "正在查找科室 " << department << " 的患者信息..." << std::endl;
    while (current != nullptr)
    {
        if (!current->isDeleted && deptMatch(current->department, department))
        {
            std::cout << "患者ID: " << current->getUserID()
                      << ", 姓名: " << current->getUsername()
                      << ",性别: " << current->getGender()
                      << ", 年龄: " << current->getAge()
                      << ", 科室: " << current->department
                      << ", 身份证号: " << current->idCardNumber
                      << ", 联系方式: " << current->getTelephone() << " / " << current->getEmail()
                      << ", 紧急联系人信息：" << current->emergencyContactName << " / " << current->emergencyContactPhone
                      << ", 过敏史: " << current->allergyHistory
                      << ", 既往病史: " << current->pastMedicalHistory
                      << ", 婚姻状况: " << patientMaritalStatusToString(current->maritalStatus)
                      << ", 住院状态: " << (current->isHospitalized ? "已住院" : "未住院")
                      << ", 挂号次数: " << current->registrationCount
                      << ", 看诊次数: " << current->consultationCount
                      << ", 住院次数: " << current->hospitalizationCount
                      << ", 用药记录数量: " << current->medicationCount
                      << std::endl;
            found = true;
            std::cout << std::endl;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "暂无患者信息！" << std::endl;
    }
    return found;
}
// 根据患者ID查看患者信息
bool Admin::viewPatientByID(Patient *&patient, const std::string &department)
{
    std::string patientID = inputIDCheck("请输入要查找的患者ID: ");

    Patient *current = patient;
    bool found = false;
    std::cout << "正在查找ID: " << patientID << " 的患者信息..." << std::endl;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->getUserID() == patientID && deptMatch(current->department, department))
        {
            std::cout << "患者ID: " << current->getUserID()
                      << ", 姓名: " << current->getUsername()
                      << ",性别: " << current->getGender()
                      << ", 年龄: " << current->getAge()
                      << ", 科室: " << current->department
                      << ", 身份证号: " << current->idCardNumber
                      << ", 联系方式: " << current->getTelephone() << " / " << current->getEmail()
                      << ", 紧急联系人信息：" << current->emergencyContactName << " / " << current->emergencyContactPhone
                      << ", 过敏史: " << current->allergyHistory
                      << ", 既往病史: " << current->pastMedicalHistory
                      << ", 婚姻状况: " << patientMaritalStatusToString(current->maritalStatus)
                      << ", 住院状态: " << (current->isHospitalized ? "已住院" : "未住院")
                      << ", 挂号次数: " << current->registrationCount
                      << ", 看诊次数: " << current->consultationCount
                      << ", 住院次数: " << current->hospitalizationCount
                      << ", 用药记录数量: " << current->medicationCount
                      << std::endl;
            found = true;
            break;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到ID为 " << patientID << " 的患者信息！" << std::endl;
    }
    return found;
}
// 根据患者姓名查看患者信息
bool Admin::viewPatientsByName(Patient *&patient, const std::string &department)
{
    std::string name = inputStringCheck("请输入患者姓名（支持模糊查询）: ");

    Patient *current = patient;
    bool found = false;
    std::cout << "正在查找姓名包含 \"" << name << "\" 的患者信息..." << std::endl;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->getUsername().find(name) != std::string::npos && deptMatch(current->department, department))
        {
            std::cout << "患者ID: " << current->getUserID()
                      << ", 姓名: " << current->getUsername()
                      << ",性别: " << current->getGender()
                      << ", 年龄: " << current->getAge()
                      << ", 科室: " << current->department
                      << ", 身份证号: " << current->idCardNumber
                      << ", 联系方式: " << current->getTelephone() << " / " << current->getEmail()
                      << ", 紧急联系人信息：" << current->emergencyContactName << " / " << current->emergencyContactPhone
                      << ", 过敏史: " << current->allergyHistory
                      << ", 既往病史: " << current->pastMedicalHistory
                      << ", 婚姻状况: " << patientMaritalStatusToString(current->maritalStatus)
                      << ", 住院状态: " << (current->isHospitalized ? "已住院" : "未住院")
                      << ", 挂号次数: " << current->registrationCount
                      << ", 看诊次数: " << current->consultationCount
                      << ", 住院次数: " << current->hospitalizationCount
                      << ", 用药记录数量: " << current->medicationCount
                      << std::endl;
            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到姓名包含 \"" << name << "\" 的患者信息！" << std::endl;
    }
    return found;
}
// 根据患者身份证号查看患者信息
bool Admin::viewPatientsByIDCard(Patient *&patient, const std::string &department)
{
    std::string idCardNumber = inputIDcardCheck("请输入患者身份证号: ");

    Patient *current = patient;
    bool found = false;
    std::cout << "正在查找身份证号: " << idCardNumber << " 的患者信息..." << std::endl;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->idCardNumber == idCardNumber && deptMatch(current->department, department))
        {
            std::cout << "患者ID: " << current->getUserID()
                      << ", 姓名: " << current->getUsername()
                      << ",性别: " << current->getGender()
                      << ", 年龄: " << current->getAge()
                      << ", 科室: " << current->department
                      << ", 身份证号: " << current->idCardNumber
                      << ", 联系方式: " << current->getTelephone() << " / " << current->getEmail()
                      << ", 紧急联系人信息：" << current->emergencyContactName << " / " << current->emergencyContactPhone
                      << ", 过敏史: " << current->allergyHistory
                      << ", 既往病史: " << current->pastMedicalHistory
                      << ", 婚姻状况: " << patientMaritalStatusToString(current->maritalStatus)
                      << ", 住院状态: " << (current->isHospitalized ? "已住院" : "未住院")
                      << ", 挂号次数: " << current->registrationCount
                      << ", 看诊次数: " << current->consultationCount
                      << ", 住院次数: " << current->hospitalizationCount
                      << ", 用药记录数量: " << current->medicationCount
                      << std::endl;
            found = true;
            break;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到身份证号为 " << idCardNumber << " 的患者信息！" << std::endl;
    }
    return found;
}
// 根据患者性别查看患者信息
bool Admin::viewPatientsByGender(Patient *&patient, const std::string &department)
{
    std::string gender = inputGenderCheck("请输入患者性别: ");

    Patient *current = patient;
    bool found = false;
    std::cout << "正在查找性别为 \"" << gender << "\" 的患者信息..." << std::endl;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->getGender() == gender && deptMatch(current->department, department))
        {
            std::cout << "患者ID: " << current->getUserID()
                      << ", 姓名: " << current->getUsername()
                      << ",性别: " << current->getGender()
                      << ", 年龄: " << current->getAge()
                      << ", 科室: " << current->department
                      << ", 身份证号: " << current->idCardNumber
                      << ", 联系方式: " << current->getTelephone() << " / " << current->getEmail()
                      << ", 紧急联系人信息：" << current->emergencyContactName << " / " << current->emergencyContactPhone
                      << ", 过敏史: " << current->allergyHistory
                      << ", 既往病史: " << current->pastMedicalHistory
                      << ", 婚姻状况: " << patientMaritalStatusToString(current->maritalStatus)
                      << ", 住院状态: " << (current->isHospitalized ? "已住院" : "未住院")
                      << ", 挂号次数: " << current->registrationCount
                      << ", 看诊次数: " << current->consultationCount
                      << ", 住院次数: " << current->hospitalizationCount
                      << ", 用药记录数量: " << current->medicationCount
                      << std::endl;
            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到性别为 \"" << gender << "\" 的患者信息！" << std::endl;
    }
    return found;
}
// 根据患者年龄范围查看患者信息
bool Admin::viewPatientsByAgeGroup(Patient *&patient, const std::string &department)
{
    std::cout << "请输入患者年龄范围: " << std::endl;
    int minAge = inputAgeCheck("最小年龄: ");
    int maxAge = inputAgeCheck("最大年龄: ");

    if (minAge > maxAge)
    {
        printError("输入的年龄范围无效！");
        return false;
    }

    Patient *current = patient;
    bool found = false;
    std::cout << "正在查找年龄在 " << minAge << " 到 " << maxAge << " 岁之间的患者信息..." << std::endl;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->getAge() >= minAge && current->getAge() <= maxAge && deptMatch(current->department, department))
        {
            std::cout << "患者ID: " << current->getUserID()
                      << ", 姓名: " << current->getUsername()
                      << ",性别: " << current->getGender()
                      << ", 年龄: " << current->getAge()
                      << ", 科室: " << current->department
                      << ", 身份证号: " << current->idCardNumber
                      << ", 联系方式: " << current->getTelephone() << " / " << current->getEmail()
                      << ", 紧急联系人信息：" << current->emergencyContactName << " / " << current->emergencyContactPhone
                      << ", 过敏史: " << current->allergyHistory
                      << ", 既往病史: " << current->pastMedicalHistory
                      << ", 婚姻状况: " << patientMaritalStatusToString(current->maritalStatus)
                      << ", 住院状态: " << (current->isHospitalized ? "已住院" : "未住院")
                      << ", 挂号次数: " << current->registrationCount
                      << ", 看诊次数: " << current->consultationCount
                      << ", 住院次数: " << current->hospitalizationCount
                      << ", 用药记录数量: " << current->medicationCount
                      << std::endl;
            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到年龄在 " << minAge << " 到 " << maxAge << " 岁之间的患者信息！" << std::endl;
    }
    return found;
}
// 根据患者住院状态查看患者信息
bool Admin::viewPatientsByHospitalizationStatus(Patient *&patient, const std::string &department)
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("选择住院状态");
    printMenuItem(1, "已住院");
    printMenuItem(2, "未住院");
    printMenuItem(0, "返回上一级菜单");
    printMenuBottom();

    int hospitalizationChoice = selectIntCheck(0, 2);
    if (hospitalizationChoice == 0)
    {
        return false; // 返回上一级菜单
    }
    bool filterHospitalized = (hospitalizationChoice == 1); // 1表示已住院，2表示未住院

    Patient *current = patient;
    bool found = false;
    std::cout << "正在查找住院状态为 " << (filterHospitalized ? "已住院" : "未住院") << " 的患者信息..." << std::endl;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->isHospitalized == filterHospitalized && deptMatch(current->department, department))
        {
            std::cout << "患者ID: " << current->getUserID()
                      << ", 姓名: " << current->getUsername()
                      << ",性别: " << current->getGender()
                      << ", 年龄: " << current->getAge()
                      << ", 科室: " << current->department
                      << ", 身份证号: " << current->idCardNumber
                      << ", 联系方式: " << current->getTelephone() << " / " << current->getEmail()
                      << ", 紧急联系人信息：" << current->emergencyContactName << " / " << current->emergencyContactPhone
                      << ", 过敏史: " << current->allergyHistory
                      << ", 既往病史: " << current->pastMedicalHistory
                      << ", 婚姻状况: " << patientMaritalStatusToString(current->maritalStatus)
                      << ", 住院状态: " << (current->isHospitalized ? "已住院" : "未住院")
                      << ", 挂号次数: " << current->registrationCount
                      << ", 看诊次数: " << current->consultationCount
                      << ", 住院次数: " << current->hospitalizationCount
                      << ", 用药记录数量: " << current->medicationCount
                      << std::endl;
            found = true;
        }
        current = current->next;
    }
    if (!found)
    {
        std::cout << "未找到住院状态为 " << (filterHospitalized ? "已住院" : "未住院") << " 的患者信息！" << std::endl;
    }
    return found;
}
// 根据患者婚姻状况查看患者信息
bool Admin::viewPatientsByMaritalStatus(Patient *&patient, const std::string &department)
{

    int maritalStatusChoice = PatientMaritalStatusMenu();

    if (maritalStatusChoice == 0)
    {
        return false; // 返回上一级菜单
    }
    MaritalStatus filterMaritalStatus = static_cast<MaritalStatus>(maritalStatusChoice - 1);

    Patient *current = patient;
    bool found = false;
    std::cout << "正在查找婚姻状况为 " << patientMaritalStatusToString(filterMaritalStatus) << " 的患者信息..." << std::endl;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->maritalStatus == filterMaritalStatus && deptMatch(current->department, department))
        {
            std::cout << "患者ID: " << current->getUserID()
                      << ", 姓名: " << current->getUsername()
                      << ",性别: " << current->getGender()
                      << ", 年龄: " << current->getAge()
                      << ", 科室: " << current->department
                      << ", 身份证号: " << current->idCardNumber
                      << ", 联系方式: " << current->getTelephone() << " / " << current->getEmail()
                      << ", 紧急联系人信息：" << current->emergencyContactName << " / " << current->emergencyContactPhone
                      << ", 过敏史: " << current->allergyHistory
                      << ", 既往病史: " << current->pastMedicalHistory
                      << ", 婚姻状况: " << patientMaritalStatusToString(current->maritalStatus)
                      << ", 住院状态: " << (current->isHospitalized ? "已住院" : "未住院")
                      << ", 挂号次数: " << current->registrationCount
                      << ", 看诊次数: " << current->consultationCount
                      << ", 住院次数: " << current->hospitalizationCount
                      << ", 用药记录数量: " << current->medicationCount
                      << std::endl;
            found = true;
        }
        current = current->next;
    }
    if (!found)
    {
        std::cout << "未找到婚姻状况为 " << patientMaritalStatusToString(filterMaritalStatus) << " 的患者信息！" << std::endl;
    }
    return found;
}
// 根据联系方式查看患者信息
bool Admin::viewPatientsByContactInfo(Patient *&patient, const std::string &department)
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("选择联系方式类型");
    printMenuItem(1, "电话号码");
    printMenuItem(2, "电子邮箱");
    printMenuItem(0, "返回上一级菜单");
    printMenuBottom();
    int contactChoice = selectIntCheck(0, 2);

    if (contactChoice == 0)
    {
        return false; // 返回上一级菜单
    }

    bool found = false;

    if (contactChoice == 1)
    {
        std::string telephone = inputTelephoneCheck("请输入患者电话号码: ");
        Patient *current = patient;

        std::cout << "正在查找电话号码为 " << telephone << " 的患者信息..." << std::endl;
        while (current != nullptr)
        {
            if (!current->isDeleted && current->getTelephone() == telephone && deptMatch(current->department, department))
            {
                std::cout << "患者ID: " << current->getUserID()
                          << ", 姓名: " << current->getUsername()
                          << ",性别: " << current->getGender()
                          << ", 年龄: " << current->getAge()
                          << ", 科室: " << current->department
                          << ", 身份证号: " << current->idCardNumber
                          << ", 联系方式: " << current->getTelephone() << " / " << current->getEmail()
                          << ", 紧急联系人信息：" << current->emergencyContactName << " / " << current->emergencyContactPhone
                          << ", 过敏史: " << current->allergyHistory
                          << ", 既往病史: " << current->pastMedicalHistory
                          << ", 婚姻状况: " << patientMaritalStatusToString(current->maritalStatus)
                          << ", 住院状态: " << (current->isHospitalized ? "已住院" : "未住院")
                          << ", 挂号次数: " << current->registrationCount
                          << ", 看诊次数: " << current->consultationCount
                          << ", 住院次数: " << current->hospitalizationCount
                          << ", 用药记录数量: " << current->medicationCount
                          << std::endl;
                found = true;
            }
            current = current->next;
        }
        if (!found)
        {
            std::cout << "未找到电话号码为 " << telephone << " 的患者信息！" << std::endl;
        }
    }
    else if (contactChoice == 2)
    {
        std::string email = inputEmailCheck("请输入患者电子邮箱: ");
        Patient *current = patient;
        found = false;
        std::cout << "正在查找电子邮箱为 " << email << " 的患者信息..." << std::endl;
        while (current != nullptr)
        {
            if (!current->isDeleted && current->getEmail() == email && deptMatch(current->department, department))
            {
                std::cout << "患者ID: " << current->getUserID()
                          << ", 姓名: " << current->getUsername()
                          << ",性别: " << current->getGender()
                          << ", 年龄: " << current->getAge()
                          << ", 科室: " << current->department
                          << ", 身份证号: " << current->idCardNumber
                          << ", 联系方式: " << current->getTelephone() << " / " << current->getEmail()
                          << ", 紧急联系人信息：" << current->emergencyContactName << " / " << current->emergencyContactPhone
                          << ", 过敏史: " << current->allergyHistory
                          << ", 既往病史: " << current->pastMedicalHistory
                          << ", 婚姻状况: " << patientMaritalStatusToString(current->maritalStatus)
                          << ", 住院状态: " << (current->isHospitalized ? "已住院" : "未住院")
                          << ", 挂号次数: " << current->registrationCount
                          << ", 看诊次数: " << current->consultationCount
                          << ", 住院次数: " << current->hospitalizationCount
                          << ", 用药记录数量: " << current->medicationCount
                          << std::endl;
                found = true;
            }
            current = current->next;
        }
        if (!found)
        {
            std::cout << "未找到电子邮箱为 " << email << " 的患者信息！" << std::endl;
        }
    }
    return found;
}
// 根据患者家庭住址查看患者信息
bool Admin::viewPatientsByAddress(Patient *&patient, const std::string &department)
{
    std::string address = inputStringCheck("请输入患者家庭住址（支持模糊查询）: ");

    Patient *current = patient;
    bool found = false;
    std::cout << "正在查找家庭住址包含 \"" << address << "\" 的患者信息..." << std::endl;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->address.find(address) != std::string::npos && deptMatch(current->department, department))
        {
            std::cout << "患者ID: " << current->getUserID()
                      << ", 姓名: " << current->getUsername()
                      << ",性别: " << current->getGender()
                      << ", 年龄: " << current->getAge()
                      << ", 科室: " << current->department
                      << ", 身份证号: " << current->idCardNumber
                      << ", 联系方式: " << current->getTelephone() << " / " << current->getEmail()
                      << ", 紧急联系人信息：" << current->emergencyContactName << " / " << current->emergencyContactPhone
                      << ", 过敏史: " << current->allergyHistory
                      << ", 既往病史: " << current->pastMedicalHistory
                      << ", 婚姻状况: " << patientMaritalStatusToString(current->maritalStatus)
                      << ", 住院状态: " << (current->isHospitalized ? "已住院" : "未住院")
                      << ", 挂号次数: " << current->registrationCount
                      << ", 看诊次数: " << current->consultationCount
                      << ", 住院次数: " << current->hospitalizationCount
                      << ", 用药记录数量: " << current->medicationCount
                      << std::endl;
            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        std::cout << "未找到家庭住址包含 \"" << address << "\" 的患者信息！" << std::endl;
    }
    return found;
}
// 修改患者姓名
void Admin::modifyPatientName(Patient *&patient, const std::string &department)
{
    std::string patientID = inputIDCheck("请输入要修改姓名的患者ID: ");
    Patient *current = patient;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->getUserID() == patientID && deptMatch(current->department, department))
        {
            std::cout << "当前患者姓名: " << current->getUsername() << std::endl;
            std::string newName = inputStringCheck("请输入新的患者姓名: ");
            current->setUsername(newName);
            std::cout << "患者姓名已更新！" << std::endl;
            return;
        }
        current = current->next;
    }
    std::cout << "未找到指定的患者！" << std::endl;
}
// 修改患者性别
void Admin::modifyPatientGender(Patient *&patient, const std::string &department)
{
    std::string patientID = inputIDCheck("请输入要修改性别的患者ID: ");
    Patient *current = patient;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->getUserID() == patientID && deptMatch(current->department, department))
        {
            std::cout << "当前患者性别: " << current->getGender() << std::endl;
            std::string newGender = inputGenderCheck("请输入新的患者性别: ");
            current->setGender(newGender);
            std::cout << "患者性别已更新！" << std::endl;
            return;
        }
        current = current->next;
    }
    std::cout << "未找到指定的患者！" << std::endl;
}
// 修改患者年龄
void Admin::modifyPatientAge(Patient *&patient, const std::string &department)
{
    std::string patientID = inputIDCheck("请输入要修改年龄的患者ID: ");
    Patient *current = patient;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->getUserID() == patientID && deptMatch(current->department, department))
        {
            std::cout << "当前患者年龄: " << current->getAge() << std::endl;
            int newAge = inputAgeCheck("请输入新的患者年龄: ");
            current->setAge(newAge);
            std::cout << "患者年龄已更新！" << std::endl;
            return;
        }
        current = current->next;
    }
    std::cout << "未找到指定的患者！" << std::endl;
}
// 修改患者所属科室
void Admin::modifyPatientDepartment(Patient *&patient, const std::string &department)
{
    std::string patientID = inputIDCheck("请输入要修改所属科室的患者ID: ");
    Patient *current = patient;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->getUserID() == patientID && deptMatch(current->department, department))
        {
            std::cout << "当前患者所属科室: " << current->department << std::endl;

            std::string newDepartment = adminDepartmentMenu();

            if (newDepartment == "0")
            {
                std::cout << "已取消操作！" << std::endl;
                return;
            }
            else if (newDepartment.empty())
            {
                printError("输入的科室名称无效！");
                return;
            }

            current->department = newDepartment;
            std::cout << "患者所属科室已更新！" << std::endl;
            return;
        }
        current = current->next;
    }
    std::cout << "未找到指定的患者！" << std::endl;
}
// 修改患者身份证号
void Admin::modifyPatientIDCard(Patient *&patient, const std::string &department)
{
    std::string patientID = inputIDCheck("请输入要修改身份证号的患者ID: ");
    Patient *current = patient;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->getUserID() == patientID && deptMatch(current->department, department))
        {
            std::cout << "当前患者身份证号: " << current->idCardNumber << std::endl;
            std::string newIDCardNumber = inputIDcardCheck("请输入新的患者身份证号: ");
            current->idCardNumber = newIDCardNumber;
            std::cout << "患者身份证号已更新！" << std::endl;
            return;
        }
        current = current->next;
    }
    std::cout << "未找到指定的患者！" << std::endl;
}
// 修改患者联系电话
void Admin::modifyPatientTelephone(Patient *&patient, const std::string &department)
{
    std::string patientID = inputIDCheck("请输入要修改联系电话的患者ID: ");
    Patient *current = patient;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->getUserID() == patientID && deptMatch(current->department, department))
        {
            std::cout << "当前患者联系电话: " << current->getTelephone() << std::endl;
            std::string newTelephone = inputTelephoneCheck("请输入新的患者联系电话: ");
            current->setTelephone(newTelephone);
            std::cout << "患者联系电话已更新！" << std::endl;
            return;
        }
        current = current->next;
    }
    std::cout << "未找到指定的患者！" << std::endl;
}
// 修改患者电子邮箱
void Admin::modifyPatientEmail(Patient *&patient, const std::string &department)
{
    std::string patientID = inputIDCheck("请输入要修改电子邮箱的患者ID: ");
    Patient *current = patient;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->getUserID() == patientID && deptMatch(current->department, department))
        {
            std::cout << "当前患者电子邮箱: " << current->getEmail() << std::endl;
            std::string newEmail = inputEmailCheck("请输入新的患者电子邮箱: ");
            current->setEmail(newEmail);
            std::cout << "患者电子邮箱已更新！" << std::endl;
            return;
        }
        current = current->next;
    }
    std::cout << "未找到指定的患者！" << std::endl;
}
// 修改家庭住址
void Admin::modifyPatientAddress(Patient *&patient, const std::string &department)
{
    std::string patientID = inputIDCheck("请输入要修改家庭住址的患者ID: ");
    Patient *current = patient;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->getUserID() == patientID && deptMatch(current->department, department))
        {
            std::cout << "当前患者家庭住址: " << current->address << std::endl;
            std::string newAddress = inputStringCheck("请输入新的患者家庭住址: ");
            current->address = newAddress;
            std::cout << "患者家庭住址已更新！" << std::endl;
            return;
        }
        current = current->next;
    }
    std::cout << "未找到指定的患者！" << std::endl;
}
// 修改患者婚姻状况
void Admin::modifyPatientMaritalStatus(Patient *&patient, const std::string &department)
{
    std::string patientID = inputIDCheck("请输入要修改婚姻状况的患者ID: ");
    Patient *current = patient;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->getUserID() == patientID && deptMatch(current->department, department))
        {
            std::cout << "当前患者婚姻状况: " << patientMaritalStatusToString(current->maritalStatus) << std::endl;

            int maritalStatusChoice = PatientMaritalStatusMenu();

            if (maritalStatusChoice == 0)
            {
                std::cout << "已取消操作！" << std::endl;
                return; // 返回上一级菜单
            }

            MaritalStatus newMaritalStatus = static_cast<MaritalStatus>(maritalStatusChoice - 1);
            current->maritalStatus = newMaritalStatus;
            std::cout << "患者婚姻状况已更新！" << std::endl;
            return;
        }
        current = current->next;
    }
    std::cout << "未找到指定的患者！" << std::endl;
}
// 修改紧急联系人信息
void Admin::modifyPatientEmergencyContact(Patient *&patient, const std::string &department)
{
    std::string patientID = inputIDCheck("请输入要修改紧急联系人信息的患者ID: ");
    Patient *current = patient;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->getUserID() == patientID && deptMatch(current->department, department))
        {
            std::cout << "当前患者紧急联系人姓名: " << current->emergencyContactName << std::endl;
            std::cout << "当前患者紧急联系人电话: " << current->emergencyContactPhone << std::endl;

            std::string newEmergencyContactName = inputStringCheck("请输入新的紧急联系人姓名: ");
            std::string newEmergencyContactPhone = inputTelephoneCheck("请输入新的紧急联系人电话: ");

            current->emergencyContactName = newEmergencyContactName;
            current->emergencyContactPhone = newEmergencyContactPhone;

            std::cout << "患者紧急联系人信息已更新！" << std::endl;
            return;
        }
        current = current->next;
    }
    std::cout << "未找到指定的患者！" << std::endl;
}
// 修改患者累计挂号次数
void Admin::modifyPatientRegistrationCount(Patient *&patient, const std::string &department)
{
    std::string patientID = inputIDCheck("请输入要修改累计挂号次数的患者ID: ");
    Patient *current = patient;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->getUserID() == patientID && deptMatch(current->department, department))
        {
            std::cout << "当前患者累计挂号次数: " << current->registrationCount << std::endl;
            std::cout << "请输入新的累计挂号次数: ";
            int newRegistrationCount = selectIntCheck(0, INT_MAX);
            current->registrationCount = newRegistrationCount;
            std::cout << "患者累计挂号次数已更新！" << std::endl;
            return;
        }
        current = current->next;
    }
    std::cout << "未找到指定的患者！" << std::endl;
}
// 修改患者累计看诊次数
void Admin::modifyPatientConsultationCount(Patient *&patient, const std::string &department)
{
    std::string patientID = inputIDCheck("请输入要修改累计看诊次数的患者ID: ");
    Patient *current = patient;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->getUserID() == patientID && deptMatch(current->department, department))
        {
            std::cout << "当前患者累计看诊次数: " << current->consultationCount << std::endl;
            std::cout << "请输入新的累计看诊次数: ";
            int newConsultationCount = selectIntCheck(0, INT_MAX);
            current->consultationCount = newConsultationCount;
            std::cout << "患者累计看诊次数已更新！" << std::endl;
            return;
        }
        current = current->next;
    }
    std::cout << "未找到指定的患者！" << std::endl;
}
// 修改患者累计住院次数
void Admin::modifyPatientHospitalizationCount(Patient *&patient, const std::string &department)
{
    std::string patientID = inputIDCheck("请输入要修改累计住院次数的患者ID: ");
    Patient *current = patient;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->getUserID() == patientID && deptMatch(current->department, department))
        {
            std::cout << "当前患者累计住院次数: " << current->hospitalizationCount << std::endl;
            std::cout << "请输入新的累计住院次数: ";
            int newHospitalizationCount = selectIntCheck(0, INT_MAX);
            current->hospitalizationCount = newHospitalizationCount;
            std::cout << "患者累计住院次数已更新！" << std::endl;
            return;
        }
        current = current->next;
    }
    std::cout << "未找到指定的患者！" << std::endl;
}
// 修改患者累计用药记录数量
void Admin::modifyPatientMedicationCount(Patient *&patient, const std::string &department)
{
    std::string patientID = inputIDCheck("请输入要修改累计用药记录数量的患者ID: ");
    Patient *current = patient;
    while (current != nullptr)
    {
        if (!current->isDeleted && current->getUserID() == patientID && deptMatch(current->department, department))
        {
            std::cout << "当前患者累计用药记录数量: " << current->medicationCount << std::endl;
            std::cout << "请输入新的累计用药记录数量: ";
            int newMedicationCount = selectIntCheck(0, INT_MAX);
            current->medicationCount = newMedicationCount;
            std::cout << "患者累计用药记录数量已更新！" << std::endl;
            return;
        }
        current = current->next;
    }
    std::cout << "未找到指定的患者！" << std::endl;
}
// 删除患者信息（逻辑删除）
void Admin::deletePatient(Patient *&target, const std::string &department)
{
    target->isDeleted = true;
    std::cout << "患者ID: " << target->getUserID() << " 已删除！" << std::endl;
}
// 添加患者信息
bool Admin::addPatient(Patient *&patient, int &idCounter)
{
    Patient *newPatient = new Patient();
    bool success = newPatient->patientSignUp(idCounter);
    if (!success)
    {
        delete newPatient;
        return false;
    }
    // 插入到链表头部
    newPatient->next = patient;
    if (patient != nullptr)
    {
        patient->prev = newPatient;
    }
    patient = newPatient;
    return true;
}

void Admin::managePatients(Patient *&patient, const std::string &department, int &idCounter)
{
    while (true)
    {
        int choice = adminPatientManagementMenu();
        if (choice == 0)
        {
            break; // 返回上一级菜单
        }
        else if (choice == 1)
        {
            while (true)
            {
                int viewChoice = adminPatientViewMenu();

                if (viewChoice == 0)
                {
                    break; // 返回患者信息查看菜单
                }
                else if (viewChoice == 1)
                {
                    viewAllPatients(patient, department);
                    pause("管理员 > 账户管理 > 患者管理");
                }
                else if (viewChoice == 2)
                {
                    viewPatientByID(patient, department);
                    pause("管理员 > 账户管理 > 患者管理");
                }
                else if (viewChoice == 3)
                {
                    viewPatientsByName(patient, department);
                    pause("管理员 > 账户管理 > 患者管理");
                }
                else if (viewChoice == 4)
                {
                    viewPatientsByIDCard(patient, department);
                    pause("管理员 > 账户管理 > 患者管理");
                }
                else if (viewChoice == 5)
                {
                    viewPatientsByGender(patient, department);
                    pause("管理员 > 账户管理 > 患者管理");
                }
                else if (viewChoice == 6)
                {
                    viewPatientsByAgeGroup(patient, department);
                    pause("管理员 > 账户管理 > 患者管理");
                }
                else if (viewChoice == 7)
                {
                    viewPatientsByContactInfo(patient, department);
                    pause("管理员 > 账户管理 > 患者管理");
                }
                else if (viewChoice == 8)
                {
                    viewPatientsByHospitalizationStatus(patient, department);
                    pause("管理员 > 账户管理 > 患者管理");
                }
                else if (viewChoice == 9)
                {
                    viewPatientsByAddress(patient, department);
                    pause("管理员 > 账户管理 > 患者管理");
                }
                else if (viewChoice == 10)
                {
                    viewPatientsByMaritalStatus(patient, department);
                    pause("管理员 > 账户管理 > 患者管理");
                }
            }
        }
        else if (choice == 2)
        {
            bool hasPatients = viewAllPatients(patient, department);

            if (hasPatients)
            {
                std::string patientID = inputIDCheck("请输入要修改信息的患者ID: ");
                Patient *target = patient;
                while (target != nullptr)
                {
                    if (!target->isDeleted && target->getUserID() == patientID && deptMatch(target->department, department))
                    {
                        break; // 找到目标患者，跳出循环
                    }
                    target = target->next;
                }

                if (target)
                {
                    while (true)
                    {
                        std::cout << "正在进行修改的患者信息: " << std::endl;
                        std::cout << "患者ID: " << target->getUserID()
                                  << ", 姓名: " << target->getUsername()
                                  << ",性别: " << target->getGender()
                                  << ", 年龄: " << target->getAge()
                                  << ", 科室: " << target->department
                                  << ", 身份证号: " << target->idCardNumber
                                  << ", 联系方式: " << target->getTelephone() << " / " << target->getEmail()
                                  << ", 紧急联系人信息：" << target->emergencyContactName << " / " << target->emergencyContactPhone
                                  << ", 过敏史: " << target->allergyHistory
                                  << ", 既往病史: " << target->pastMedicalHistory
                                  << ", 婚姻状况: " << patientMaritalStatusToString(target->maritalStatus)
                                  << ", 住院状态: " << (target->isHospitalized ? "已住院" : "未住院")
                                  << ", 挂号次数: " << target->registrationCount
                                  << ", 看诊次数: " << target->consultationCount
                                  << ", 住院次数: " << target->hospitalizationCount
                                  << ", 用药记录数量: " << target->medicationCount
                                  << std::endl;

                        int modifyChoice = adminPatientModificationMenu();

                        if (modifyChoice == 0)
                        {
                            break; // 返回患者信息修改菜单
                        }
                        else if (modifyChoice == 1)
                        {
                            modifyPatientName(target, department);
                            pause("管理员 > 账户管理 > 患者管理");
                        }
                        else if (modifyChoice == 2)
                        {
                            modifyPatientGender(target, department);
                            pause("管理员 > 账户管理 > 患者管理");
                        }
                        else if (modifyChoice == 3)
                        {
                            modifyPatientAge(target, department);
                            pause("管理员 > 账户管理 > 患者管理");
                        }
                        else if (modifyChoice == 4)
                        {
                            modifyPatientDepartment(target, department);
                            pause("管理员 > 账户管理 > 患者管理");
                        }
                        else if (modifyChoice == 5)
                        {
                            modifyPatientIDCard(target, department);
                            pause("管理员 > 账户管理 > 患者管理");
                        }
                        else if (modifyChoice == 6)
                        {
                            modifyPatientTelephone(target, department);
                            pause("管理员 > 账户管理 > 患者管理");
                        }
                        else if (modifyChoice == 7)
                        {
                            modifyPatientEmail(target, department);
                            pause("管理员 > 账户管理 > 患者管理");
                        }
                        else if (modifyChoice == 8)
                        {
                            modifyPatientAddress(target, department);
                            pause("管理员 > 账户管理 > 患者管理");
                        }
                        else if (modifyChoice == 9)
                        {
                            modifyPatientMaritalStatus(target, department);
                            pause("管理员 > 账户管理 > 患者管理");
                        }
                        else if (modifyChoice == 10)
                        {
                            modifyPatientEmergencyContact(target, department);
                            pause("管理员 > 账户管理 > 患者管理");
                        }
                        else if (modifyChoice == 11)
                        {
                            modifyPatientRegistrationCount(target, department);
                            pause("管理员 > 账户管理 > 患者管理");
                        }
                        else if (modifyChoice == 12)
                        {
                            modifyPatientConsultationCount(target, department);
                            pause("管理员 > 账户管理 > 患者管理");
                        }
                        else if (modifyChoice == 13)
                        {
                            modifyPatientHospitalizationCount(target, department);
                            pause("管理员 > 账户管理 > 患者管理");
                        }
                        else if (modifyChoice == 14)
                        {
                            modifyPatientMedicationCount(target, department);
                            pause("管理员 > 账户管理 > 患者管理");
                        }
                    }
                }
                else
                {
                    std::cout << "未找到ID为 " << patientID << " 的患者！" << std::endl;
                    pause("管理员 > 账户管理 > 患者管理");
                }
            }
        }
        else if (choice == 3)
        {
            bool hasPatients = viewAllPatients(patient, department);

            if (hasPatients)
            {
                std::string patientID = inputIDCheck("请输入要删除的患者ID: ");
                Patient *target = patient;
                while (target != nullptr)
                {
                    if (!target->isDeleted && target->getUserID() == patientID && deptMatch(target->department, department))
                    {
                        break; // 找到目标患者，跳出循环
                    }
                    target = target->next;
                }

                if (target)
                {
                    std::cout << "正在删除的患者信息: " << std::endl;
                    std::cout << "患者ID: " << target->getUserID()
                              << ", 姓名: " << target->getUsername()
                              << ",性别: " << target->getGender()
                              << ", 年龄: " << target->getAge()
                              << ", 科室: " << target->department
                              << ", 身份证号: " << target->idCardNumber
                              << ", 联系方式: " << target->getTelephone() << " / " << target->getEmail()
                              << ", 紧急联系人信息：" << target->emergencyContactName << " / " << target->emergencyContactPhone
                              << ", 过敏史: " << target->allergyHistory
                              << ", 既往病史: " << target->pastMedicalHistory
                              << ", 婚姻状况: " << patientMaritalStatusToString(target->maritalStatus)
                              << ", 住院状态: " << (target->isHospitalized ? "已住院" : "未住院")
                              << ", 挂号次数: " << target->registrationCount
                              << ", 看诊次数: " << target->consultationCount
                              << ", 住院次数: " << target->hospitalizationCount
                              << ", 用药记录数量: " << target->medicationCount
                              << std::endl;

                    clearScreen();
                    printMenuBorder();
                    printMenuTitle("确认删除患者信息");
                    printMenuItem(1, "确定");
                    printMenuItem(0, "取消");
                    printMenuBottom();
                    int confirmChoice = selectIntCheck(0, 1);
                    if (confirmChoice == 1)
                    {
                        deletePatient(target, department);
                        pause("管理员 > 账户管理 > 患者管理");
                    }
                    else
                    {
                        std::cout << "已取消删除操作！" << std::endl;
                        pause("管理员 > 账户管理 > 患者管理");
                    }
                }
                else
                {
                    std::cout << "未找到ID为 " << patientID << " 的患者！" << std::endl;
                    pause("管理员 > 账户管理 > 患者管理");
                }
            }
        }
        else if (choice == 4)
        {
            addPatient(patient, idCounter);
            pause("管理员 > 账户管理 > 患者管理");
        }
    }
}

// ========================== 管理员个人信息管理 ========================

// 管理员个人信息管理
void Admin::AdminPersonalInfo()
{
    while (true)
    {
        int choice = adminPersonalInfoManagementMenu();
        if (choice == 0)
        {
            break; // 返回上一级菜单
        }
        else if (choice == 1)
        {
            printInfoCard("管理员个人信息", {
                {"用户ID", userID},
                {"姓名", username},
                {"性别", gender},
                {"年龄", std::to_string(age)},
                {"联系电话", telephone},
                {"电子邮箱", email},
                {"账号创建时间", createTime},
            });
            pause("管理员 > 个人信息管理");
        }
        else if (choice == 2)
        {
            while (true)
            {
                int modifyChoice = adminPersonalInfoModificationMenu();
                if (modifyChoice == 0)
                {
                    break; // 返回个人信息修改菜单
                }
                else if (modifyChoice == 1)
                {
                    std::cout << "当前姓名: " << this->username << std::endl;
                    std::string newName = inputStringCheck("请输入新的姓名: ");
                    this->username = newName;
                    std::cout << "姓名已更新！" << std::endl;
                }
                else if (modifyChoice == 2)
                {
                    std::cout << "当前性别: " << this->gender << std::endl;
                    std::string newGender = inputGenderCheck("请输入新的性别: ");
                    this->gender = newGender;
                    std::cout << "性别已更新！" << std::endl;
                }
                else if (modifyChoice == 3)
                {
                    std::cout << "当前年龄: " << this->age << std::endl;
                    int newAge = inputAgeCheck("请输入新的年龄: ");
                    this->age = newAge;
                    std::cout << "年龄已更新！" << std::endl;
                }
                else if (modifyChoice == 4)
                {
                    std::cout << "当前联系电话: " << this->telephone << std::endl;
                    std::string newTelephone = inputTelephoneCheck("请输入新的联系电话: ");
                    this->telephone = newTelephone;
                    std::cout << "联系电话已更新！" << std::endl;
                }
                else if (modifyChoice == 5)
                {
                    std::cout << "当前电子邮箱: " << this->email << std::endl;
                    std::string newEmail = inputEmailCheck("请输入新的电子邮箱: ");
                    this->email = newEmail;
                    std::cout << "电子邮箱已更新！" << std::endl;
                }
                else if (modifyChoice == 6)
                {
                    std::string oldpwd = inputPwdCheck("请输入当前密码以验证身份: ");

                    if (verifyPasswordCompat(oldpwd, this->storedHash))
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
                pause("管理员 > 个人信息管理");
            }
        }
    }
}
