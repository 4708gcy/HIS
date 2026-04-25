#include "../Head/Login.h"

// 管理员登录函数，验证管理员身份并返回登录结果
Admin *adminLogin(Admin *&adminHead)
{
    Admin *current = adminHead;
    std::string id = inputIDCheck("请输入管理员用户ID: ");

    // std::cout << id << std::endl;

    while (current != nullptr)
    {
        // std::cout << current->getUserID() << std::endl;

        if (current->getUserID() == id)
        {
            if (current->adminSignIn()) // 调用管理员登录方法验证密码并设置登录状态
            {
                return current;
            }
            else
            {
                return nullptr; // 登录失败（如密码错误或账户锁定）
            }
        }

        // std::cout << "failed to find user, checking next..." << std::endl;

        current = current->next;
    }
    std::cout << "未找到管理员账号" << std::endl;
    return nullptr;
}

// 医生登录函数，验证医生身份并返回登录结果
Doctor *doctorLogin(Doctor *&doctorHead)
{
    Doctor *currentDoctor = doctorHead;
    std::string id = inputIDCheck("请输入医生用户ID: ");

    while (currentDoctor != nullptr)
    {
        if (currentDoctor->getUserID() == id)
        {
            if (currentDoctor->doctorSignIn()) // 调用医生登录方法验证密码并设置登录状态
            {
                return currentDoctor;
            }
            else
            {
                return nullptr; // 登录失败（如密码错误或账户锁定）
            }
        }
        currentDoctor = currentDoctor->next;
    }

    std::cout << "未找到医生账号" << std::endl;
    return nullptr; // 未找到匹配的医生用户ID
}

// 药剂师登录函数，验证药剂师身份并返回登录结果
Pharmacist *pharmacistLogin(Pharmacist *&pharmacistHead)
{
    Pharmacist *currentPharmacist = pharmacistHead;
    std::string id = inputIDCheck("请输入药剂师用户ID: ");

    while (currentPharmacist != nullptr)
    {
        if (currentPharmacist->getUserID() == id)
        {
            if (currentPharmacist->pharmacistSignIn()) // 调用药剂师登录方法验证密码并设置登录状态
            {
                return currentPharmacist;
            }
            else
            {
                return nullptr; // 登录失败（如密码错误或账户锁定）
            }
        }
        currentPharmacist = currentPharmacist->next;
    }
    
    std::cout << "未找到该用户ID！" << std::endl;
    return nullptr;
}

// 患者登录函数，验证患者身份并返回登录结果
Patient *patientLogin(Patient *&patientHead)
{
    Patient *currentPatient = patientHead;
    std::string id = inputIDCheck("请输入患者用户ID: ");

    while (currentPatient != nullptr)
    {
        if (currentPatient->getUserID() == id)
        {
            if (currentPatient->patientSignIn()) // 调用患者登录方法验证密码并设置登录状态
            {
                return currentPatient;
            }
            else
            {
                return nullptr; // 登录失败（如密码错误或账户锁定）
            }
        }
        currentPatient = currentPatient->next;
    }

    std::cout << "未找到患者账号" << std::endl;
    return nullptr; // 未找到匹配的患者用户ID
}

// 护士登录函数，验证护士身份并返回登录结果
Nurse *nurseLogin(Nurse *&nurseHead)
{
    Nurse *currentNurse = nurseHead;
    std::string id = inputIDCheck("请输入护士用户ID: ");

    while (currentNurse != nullptr)
    {
        if (currentNurse->getUserID() == id)
        {
            if (currentNurse->nurseSignIn()) // 调用护士登录方法验证密码并设置登录状态
            {
                return currentNurse;
            }
            else
            {
                return nullptr; // 登录失败（如密码错误或账户锁定）
            }
        }
        currentNurse = currentNurse->next;
    }

    std::cout << "未找到护士账号" << std::endl;
    return nullptr; // 未找到匹配的护士用户ID
}


// =========================== 管理管理员信息的函数实现 ====================

// 查看所有管理员信息
void viewAllAdmins(Admin *&adminHead)
{
    Admin *current = adminHead;
    std::cout << "管理员列表：" << std::endl;
    while (current != nullptr)
    {
        std::cout << "用户ID: " << current->getUserID() << ", 姓名: " << current->getUsername()
                  << ", 性别: " << current->getGender() << ", 年龄: " << current->getAge()
                  << ", 电话: " << current->getTelephone() << ", 邮箱: " << current->getEmail()
                  << ", 账户状态: " << (current->getIsAccountActive() ? "激活" : "锁定")
                  << ", 创建时间: " << current->getCreateTime()
                  << std::endl;
        current = current->next;
    }
}
// 根据管理员ID查看管理员信息
void viewAdminByID(Admin *&adminHead)
{
    Admin *current = adminHead;
    std::string targetID = inputIDCheck("请输入要查看的管理员用户ID: ");
    while (current != nullptr)
    {
        if (current->getUserID() == targetID)
        {
            std::cout << "用户ID: " << current->getUserID() << ", 姓名: " << current->getUsername()
                      << ", 性别: " << current->getGender() << ", 年龄: " << current->getAge()
                      << ", 电话: " << current->getTelephone() << ", 邮箱: " << current->getEmail()
                      << ", 账户状态: " << (current->getIsAccountActive() ? "激活" : "锁定")
                      << ", 创建时间: " << current->getCreateTime()
                      << std::endl;
            return;
        }
        current = current->next;
    }
    std::cout << "未找到指定管理员账号" << std::endl;
}
// 根据管理员姓名查看管理员信息(支持模糊查询)
void viewAdminsByName(Admin *&admin)
{
    Admin *current = admin;
    std::string targetName = inputStringCheck("请输入要查看的管理员姓名（支持模糊查询）: ");
    std::cout << "搜索结果：" << std::endl;
    bool found = false;
    while (current != nullptr)
    {
        if (current->getUsername().find(targetName) != std::string::npos) // 模糊匹配
        {
            std::cout << "用户ID: " << current->getUserID() << ", 姓名: " << current->getUsername()
                      << ", 性别: " << current->getGender() << ", 年龄: " << current->getAge()
                      << ", 电话: " << current->getTelephone() << ", 邮箱: " << current->getEmail()
                      << ", 账户状态: " << (current->getIsAccountActive() ? "激活" : "锁定")
                      << ", 创建时间: " << current->getCreateTime()
                      << std::endl;
            found = true;
        }
        current = current->next;
    }
    if (!found)
    {
        std::cout << "未找到匹配的管理员账号" << std::endl;
    }
}
// 根据性别查看管理员信息
void viewAdminsByGender(Admin *&admin)
{
    Admin *current = admin;
    std::string targetGender = inputGenderCheck("请输入要查看的管理员性别");
    std::cout << "搜索结果：" << std::endl;
    bool found = false;
    while (current != nullptr)
    {
        if (current->getGender() == targetGender)
        {
            std::cout << "用户ID: " << current->getUserID() << ", 姓名: " << current->getUsername()
                      << ", 性别: " << current->getGender() << ", 年龄: " << current->getAge()
                      << ", 电话: " << current->getTelephone() << ", 邮箱: " << current->getEmail()
                      << ", 账户状态: " << (current->getIsAccountActive() ? "激活" : "锁定")
                      << ", 创建时间: " << current->getCreateTime()
                      << std::endl;
            found = true;
        }
        current = current->next;
    }
    if (!found)
    {
        std::cout << "未找到匹配的管理员账号" << std::endl;
    }
}
// 根据年龄段查看管理员信息
void viewAdminsByAgeGroup(Admin *&admin)
{
    Admin *current = admin;
    std::cout << "请选择要查看的年龄段区间：" << std::endl;
    int minAge = inputAgeCheck("请输入最小年龄: ");
    int maxAge = inputAgeCheck("请输入最大年龄: ");

    if (minAge > maxAge)
    {
        std::cout << "输入的年龄范围无效，最小年龄应小于或等于最大年龄。" << std::endl;
        return;
    }

    bool found = false;
    std::cout << "正在查找年龄在 " << minAge << " 到 " << maxAge << " 岁之间的管理员信息..." << std::endl;
    while (current != nullptr)
    {
        int age = current->getAge();
        if (age >= minAge && age <= maxAge)
        {
            std::cout << "用户ID: " << current->getUserID() << ", 姓名: " << current->getUsername()
                      << ", 性别: " << current->getGender() << ", 年龄: " << current->getAge()
                      << ", 电话: " << current->getTelephone() << ", 邮箱: " << current->getEmail()
                      << ", 账户状态: " << (current->getIsAccountActive() ? "激活" : "锁定")
                      << ", 创建时间: " << current->getCreateTime()
                      << std::endl;
            found = true;
        }
        current = current->next;
    }
    if (!found)
    {
        std::cout << "未找到匹配的管理员账号" << std::endl;
    }
}
// 根据联系方式查看管理员信息
void viewAdminsByContactInfo(Admin *&admin)
{
    std::cout << "请选择联系方式类型: " << std::endl;
    std::cout << "1. 电话号码" << std::endl;
    std::cout << "2. 电子邮箱" << std::endl;
    std::cout << "0. 返回上一级菜单" << std::endl;
    int contactChoice = selectIntCheck(0, 2);

    if (contactChoice == 0)
    {
        return; // 返回上一级菜单
    }

    if (contactChoice == 1)
    {
        std::string telephone = inputTelephoneCheck("请输入管理员的电话号码: ");
        Admin *current = admin;
        bool found = false;
        std::cout << "正在查找电话号码为 " << telephone << " 的管理员信息..." << std::endl;
        while (current != nullptr)
        {
            if (current->getTelephone() == telephone)
            {
                std::cout << "用户ID: " << current->getUserID() << ", 姓名: " << current->getUsername()
                          << ", 性别: " << current->getGender() << ", 年龄: " << current->getAge()
                          << ", 电话: " << current->getTelephone() << ", 邮箱: " << current->getEmail()
                          << ", 账户状态: " << (current->getIsAccountActive() ? "激活" : "锁定")
                          << ", 创建时间: " << current->getCreateTime()
                          << std::endl;
                found = true;
            }
            current = current->next;
        }
        if (!found)
        {
            std::cout << "未找到电话号码为 " << telephone << " 的管理员信息！" << std::endl;
        }
    }
    else if (contactChoice == 2)
    {
        std::string email = inputEmailCheck("请输入管理员电子邮箱: ");
        Admin *current = admin;
        bool found = false;
        std::cout << "正在查找电子邮箱为 " << email << " 的管理员信息..." << std::endl;
        while (current != nullptr)
        {
            if (current->getEmail() == email)
            {
                std::cout << "用户ID: " << current->getUserID() << ", 姓名: " << current->getUsername()
                          << ", 性别: " << current->getGender() << ", 年龄: " << current->getAge()
                          << ", 电话: " << current->getTelephone() << ", 邮箱: " << current->getEmail()
                          << ", 账户状态: " << (current->getIsAccountActive() ? "激活" : "锁定")
                          << ", 创建时间: " << current->getCreateTime()
                          << std::endl;
                found = true;
            }
            current = current->next;
        }
        if (!found)
        {
            std::cout << "未找到电子邮箱为 " << email << " 的管理员信息！" << std::endl;
        }
    }
}
// 修改管理员名字
void modifyAdminName(Admin *&admin)
{
    std::string targetID = inputIDCheck("请输入要修改信息的管理员用户ID: ");
    Admin *current = admin;
    bool found = false;
    while (current != nullptr)
    {
        if (current->getUserID() == targetID)
        {
            std::cout << "现在的管理员姓名是: " << current->getUsername() << std::endl;
            std::string newName = inputStringCheck("请输入新的管理员姓名: ");
            current->setUsername(newName);
            std::cout << "管理员姓名已更新！" << std::endl;
            found = true;
            break;
        }
        current = current->next;
    }
    if (!found)
    {
        std::cout << "未找到用户ID为 " << targetID << " 的管理员！" << std::endl;
    }
}
// 修改管理员性别
void modifyAdminGender(Admin *&admin)
{
    std::string targetID = inputIDCheck("请输入要修改信息的管理员用户ID: ");
    Admin *current = admin;
    bool found = false;
    while (current != nullptr)
    {
        if (current->getUserID() == targetID)
        {
            std::cout << "现在的管理员性别是: " << current->getGender() << std::endl;
            std::string newGender = inputGenderCheck("请输入新的管理员性别");
            current->setGender(newGender);
            std::cout << "管理员性别已更新！" << std::endl;
            found = true;
            break;
        }
        current = current->next;
    }
    if (!found)
    {
        std::cout << "未找到用户ID为 " << targetID << " 的管理员！" << std::endl;
    }
}
// 修改管理员年龄
void modifyAdminAge(Admin *&admin)
{
    std::string targetID = inputIDCheck("请输入要修改信息的管理员用户ID: ");
    Admin *current = admin;
    bool found = false;
    while (current != nullptr)
    {
        if (current->getUserID() == targetID)
        {
            std::cout << "现在的管理员年龄是: " << current->getAge() << std::endl;
            int newAge = inputAgeCheck("请输入新的管理员年龄: ");
            current->setAge(newAge);
            std::cout << "管理员年龄已更新！" << std::endl;
            found = true;
            break;
        }
        current = current->next;
    }
    if (!found)
    {
        std::cout << "未找到用户ID为 " << targetID << " 的管理员！" << std::endl;
    }
}
// 修改管理员电话号码
void modifyAdminTelephone(Admin *&admin)
{
    std::string targetID = inputIDCheck("请输入要修改信息的管理员用户ID: ");
    Admin *current = admin;
    bool found = false;
    while (current != nullptr)
    {
        if (current->getUserID() == targetID)
        {
            std::cout << "现在的管理员电话号码是: " << current->getTelephone() << std::endl;
            std::string newTelephone = inputTelephoneCheck("请输入新的管理员电话号码");
            current->setTelephone(newTelephone);
            std::cout << "管理员电话号码已更新！" << std::endl;
            found = true;
            break;
        }
        current = current->next;
    }
    if (!found)
    {
        std::cout << "未找到用户ID为 " << targetID << " 的管理员！" << std::endl;
    }
}
// 修改管理员电子邮箱
void modifyAdminEmail(Admin *&admin)
{
    std::string targetID = inputIDCheck("请输入要修改信息的管理员用户ID: ");
    Admin *current = admin;
    bool found = false;
    while (current != nullptr)
    {
        if (current->getUserID() == targetID)
        {
            std::cout << "现在的管理员电子邮箱是: " << current->getEmail() << std::endl;
            std::string newEmail = inputEmailCheck("请输入新的管理员电子邮箱");
            current->setEmail(newEmail);
            std::cout << "管理员电子邮箱已更新！" << std::endl;
            found = true;
            break;
        }
        current = current->next;
    }
    if (!found)
    {
        std::cout << "未找到用户ID为 " << targetID << " 的管理员！" << std::endl;
    }
}
// 删除管理员(逻辑删除)
void deleteAdmin(Admin *&admin)
{
    std::string targetID = inputIDCheck("请输入要删除的管理员用户ID: ");
    Admin *current = admin;
    bool found = false;
    while (current != nullptr)
    {
        if (current->getUserID() == targetID)
        {
            current->setIsDeleted(true);
            std::cout << "管理员账号已删除！" << std::endl;
            found = true;
            break;
        }
        current = current->next;
    }
    if (!found)
    {
        std::cout << "未找到用户ID为 " << targetID << " 的管理员！" << std::endl;
    }
}
// 添加管理员(通过注册流程创建新管理员账号，并插入到管理员链表中)
void addAdmin(Admin *&admin, int &idCounter)
{
    Admin *newAdmin = new Admin();
    if (newAdmin->adminSignUp(idCounter) && newAdmin->getIsAccountActive()) // 调用管理员注册方法创建新管理员账号，并检查是否成功注册且账户激活
    {
        newAdmin->next = admin; // 将新管理员插入到链表头部
        admin = newAdmin;
        std::cout << "新管理员账号已添加！" << std::endl;
    }
    else
    {
        delete newAdmin; // 注册失败或账户未激活，释放内存
        std::cout << "管理员账号添加失败！" << std::endl;
    }
}

// 管理员账户管理
void manageAdmins(Admin *&admin, int &idCounter)
{
    while (true)
    {
        int choice = adminAdminManagementMenu();
        if (choice == 0)
        {
            break; // 返回上一级菜单
        }
        else if (choice == 1)
        {
            while (true)
            {
                int viewChoice = adminAdminViewMenu();
                if (viewChoice == 0)
                {
                    break; // 返回管理员管理菜单
                }
                else if (viewChoice == 1)
                {
                    viewAllAdmins(admin);
                    pause();
                }
                else if (viewChoice == 2)
                {
                    viewAdminByID(admin);
                    pause();
                }
                else if (viewChoice == 3)
                {
                    viewAdminsByName(admin);
                    pause();
                }
                else if (viewChoice == 4)
                {
                    viewAdminsByGender(admin);
                    pause();
                }
                else if (viewChoice == 5)
                {
                    viewAdminsByAgeGroup(admin);
                    pause();
                }
                else if (viewChoice == 6)
                {
                    viewAdminsByContactInfo(admin);
                    pause();
                }
            }
        }
        else if (choice == 2)
        {
            while (true)
            {
                int modifyChoice = adminAdminModificationMenu();
                if (modifyChoice == 0)
                {
                    break; // 返回管理员管理菜单
                }
                else if (modifyChoice == 1)
                {
                    modifyAdminName(admin);
                    pause();
                }
                else if (modifyChoice == 2)
                {
                    modifyAdminGender(admin);
                    pause();
                }
                else if (modifyChoice == 3)
                {
                    modifyAdminAge(admin);
                    pause();
                }
                else if (modifyChoice == 4)
                {
                    modifyAdminTelephone(admin);
                    pause();
                }
                else if (modifyChoice == 5)
                {
                    modifyAdminEmail(admin);
                    pause();
                }
            }
        }
        else if (choice == 3)
        {
            deleteAdmin(admin);
            pause();
        }
        else if (choice == 4)
        {
            addAdmin(admin, idCounter);
            pause();
        }
    }
}

// 账号激活/封锁管理函数，允许管理员激活或封锁其他用户的账户
void AccountManagement(Admin *&adminHead, Doctor *&doctorHead, Nurse *&nurseHead, Pharmacist *&pharmacistHead, Patient *&patientHead)
{
    int identityChoice = identitySelectionMenu(); // 选择要管理的用户身份

    UserRole role = static_cast<UserRole>(identityChoice); // 将选择转换为 UserRole 枚举类型

    if (role == UserRole::ADMIN)
    {
        AccountManageGeneric<Admin>(adminHead, "管理员", "请输入要管理的管理员用户ID: ");
    }
    else if (role == UserRole::DOCTOR)
    {
        AccountManageGeneric<Doctor>(doctorHead, "医生", "请输入要管理的医生用户ID: ");
    }
    else if (role == UserRole::NURSE)
    {
        AccountManageGeneric<Nurse>(nurseHead, "护士", "请输入要管理的护士用户ID: ");
    }
    else if (role == UserRole::PHARMACIST)
    {
        AccountManageGeneric<Pharmacist>(pharmacistHead, "药剂师", "请输入要管理的药剂师用户ID: ");
    }
    else if (role == UserRole::PATIENT)
    {
        AccountManageGeneric<Patient>(patientHead, "患者", "请输入要管理的患者用户ID: ");
    }
}