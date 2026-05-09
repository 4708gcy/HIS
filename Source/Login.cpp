#include "../Head/Login.h"
#include "../Head/UI.h"

// 管理员登录函数，验证管理员身份并返回登录结果
Admin *adminLogin(Admin *&adminHead)
{
    Admin *current = adminHead;
    std::string id = inputIDCheck("请输入管理员用户ID: ");

    // 查找未删除的账户
    while (current != nullptr)
    {
        if (current->getUserID() == id && !current->getIsDeleted())
        {
            break;
        }
        current = current->next;
    }

    if (current == nullptr)
    {
        printError("未找到管理员账号");
        return nullptr;
    }

    // SignIn内部已有完整的密码重试和账户锁定逻辑
    if (current->adminSignIn())
    {
        return current;
    }
    return nullptr;
}

// 医生登录函数，验证医生身份并返回登录结果
Doctor *doctorLogin(Doctor *&doctorHead)
{
    Doctor *currentDoctor = doctorHead;
    std::string id = inputIDCheck("请输入医生用户ID: ");

    while (currentDoctor != nullptr)
    {
        if (currentDoctor->getUserID() == id && !currentDoctor->getIsDeleted())
        {
            break;
        }
        currentDoctor = currentDoctor->next;
    }

    if (currentDoctor == nullptr)
    {
        printError("未找到医生账号");
        return nullptr;
    }

    if (currentDoctor->doctorSignIn())
    {
        return currentDoctor;
    }
    return nullptr;
}

// 药剂师登录函数，验证药剂师身份并返回登录结果
Pharmacist *pharmacistLogin(Pharmacist *&pharmacistHead)
{
    Pharmacist *currentPharmacist = pharmacistHead;
    std::string id = inputIDCheck("请输入药剂师用户ID: ");

    while (currentPharmacist != nullptr)
    {
        if (currentPharmacist->getUserID() == id && !currentPharmacist->getIsDeleted())
        {
            break;
        }
        currentPharmacist = currentPharmacist->next;
    }

    if (currentPharmacist == nullptr)
    {
        printError("未找到该用户ID！");
        return nullptr;
    }

    if (currentPharmacist->pharmacistSignIn())
    {
        return currentPharmacist;
    }
    return nullptr;
}

// 患者登录函数，验证患者身份并返回登录结果
Patient *patientLogin(Patient *&patientHead)
{
    Patient *currentPatient = patientHead;
    std::string id = inputIDCheck("请输入患者用户ID: ");

    while (currentPatient != nullptr)
    {
        if (currentPatient->getUserID() == id && !currentPatient->getIsDeleted())
        {
            break;
        }
        currentPatient = currentPatient->next;
    }

    if (currentPatient == nullptr)
    {
        printError("未找到患者账号");
        return nullptr;
    }

    if (currentPatient->patientSignIn())
    {
        return currentPatient;
    }
    return nullptr;
}

// 护士登录函数，验证护士身份并返回登录结果
Nurse *nurseLogin(Nurse *&nurseHead)
{
    Nurse *currentNurse = nurseHead;
    std::string id = inputIDCheck("请输入护士用户ID: ");

    while (currentNurse != nullptr)
    {
        if (currentNurse->getUserID() == id && !currentNurse->getIsDeleted())
        {
            break;
        }
        currentNurse = currentNurse->next;
    }

    if (currentNurse == nullptr)
    {
        printError("未找到护士账号");
        return nullptr;
    }

    if (currentNurse->nurseSignIn())
    {
        return currentNurse;
    }
    return nullptr;
}


// =========================== 管理管理员信息的函数实现 ====================

// 查看所有管理员信息
void viewAllAdmins(Admin *&adminHead)
{
    Admin *current = adminHead;
    std::vector<std::string> lines;
    lines.push_back("管理员列表：");
    while (current != nullptr)
    {
        if (!current->getIsDeleted())
        {
            lines.push_back("用户ID: " + current->getUserID() + ", 姓名: " + current->getUsername()
                      + ", 性别: " + current->getGender() + ", 年龄: " + std::to_string(current->getAge())
                      + ", 电话: " + current->getTelephone() + ", 邮箱: " + current->getEmail()
                      + ", 账户状态: " + (current->getIsAccountActive() ? "激活" : "锁定")
                      + ", 创建时间: " + current->getCreateTime());
            lines.push_back("");
        }
        current = current->next;
    }
    if (lines.size() <= 1)
    {
        printError("当前没有管理员账号");
    }
    printWithPagination(lines, 10);
}
// 根据管理员ID查看管理员信息
void viewAdminByID(Admin *&adminHead)
{
    Admin *current = adminHead;
    std::string targetID = inputIDCheck("请输入要查看的管理员用户ID: ");
    while (current != nullptr)
    {
        if (current->getUserID() == targetID && !current->getIsDeleted())
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
    printError("未找到指定管理员账号");
}
// 根据管理员姓名查看管理员信息(支持模糊查询)
void viewAdminsByName(Admin *&admin)
{
    Admin *current = admin;
    std::string targetName = inputStringCheck("请输入要查看的管理员姓名（支持模糊查询）: ");
    std::vector<std::string> lines;
    lines.push_back("搜索结果：");
    bool found = false;
    while (current != nullptr)
    {
        if (current->getUsername().find(targetName) != std::string::npos && !current->getIsDeleted()) // 模糊匹配
        {
            lines.push_back("用户ID: " + current->getUserID() + ", 姓名: " + current->getUsername()
                      + ", 性别: " + current->getGender() + ", 年龄: " + std::to_string(current->getAge())
                      + ", 电话: " + current->getTelephone() + ", 邮箱: " + current->getEmail()
                      + ", 账户状态: " + (current->getIsAccountActive() ? "激活" : "锁定")
                      + ", 创建时间: " + current->getCreateTime());
            lines.push_back("");
            found = true;
        }
        current = current->next;
    }
    if (!found)
    {
        printError("未找到匹配的管理员账号");
    }
    else
    {
        printWithPagination(lines, 10);
    }
}
// 根据性别查看管理员信息
void viewAdminsByGender(Admin *&admin)
{
    Admin *current = admin;
    std::string targetGender = inputGenderCheck("请输入要查看的管理员性别");
    std::vector<std::string> lines;
    lines.push_back("搜索结果：");
    bool found = false;
    while (current != nullptr)
    {
        if (current->getGender() == targetGender && !current->getIsDeleted())
        {
            lines.push_back("用户ID: " + current->getUserID() + ", 姓名: " + current->getUsername()
                      + ", 性别: " + current->getGender() + ", 年龄: " + std::to_string(current->getAge())
                      + ", 电话: " + current->getTelephone() + ", 邮箱: " + current->getEmail()
                      + ", 账户状态: " + (current->getIsAccountActive() ? "激活" : "锁定")
                      + ", 创建时间: " + current->getCreateTime());
            lines.push_back("");
            found = true;
        }
        current = current->next;
    }
    if (!found)
    {
        printError("未找到匹配的管理员账号");
    }
    else
    {
        printWithPagination(lines, 10);
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
        printError("输入的年龄范围无效，最小年龄应小于或等于最大年龄。");
        return;
    }

    std::vector<std::string> lines;
    lines.push_back("正在查找年龄在 " + std::to_string(minAge) + " 到 " + std::to_string(maxAge) + " 岁之间的管理员信息...");
    bool found = false;
    while (current != nullptr)
    {
        int age = current->getAge();
        if (age >= minAge && age <= maxAge && !current->getIsDeleted())
        {
            lines.push_back("用户ID: " + current->getUserID() + ", 姓名: " + current->getUsername()
                      + ", 性别: " + current->getGender() + ", 年龄: " + std::to_string(current->getAge())
                      + ", 电话: " + current->getTelephone() + ", 邮箱: " + current->getEmail()
                      + ", 账户状态: " + (current->getIsAccountActive() ? "激活" : "锁定")
                      + ", 创建时间: " + current->getCreateTime());
            lines.push_back("");
            found = true;
        }
        current = current->next;
    }
    if (!found)
    {
        printError("未找到匹配的管理员账号");
    }
    else
    {
        printWithPagination(lines, 10);
    }
}
// 根据联系方式查看管理员信息
void viewAdminsByContactInfo(Admin *&admin)
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("联系方式类型");
    printMenuBottom();
    printMenuItem(1, "电话号码");
    printMenuItem(2, "电子邮箱");
    printMenuItem(0, "返回上一级菜单");
    printMenuBottom();
    int contactChoice = selectIntCheck(0, 2);

    if (contactChoice == 0)
    {
        return; // 返回上一级菜单
    }

    std::vector<std::string> lines;
    if (contactChoice == 1)
    {
        std::string telephone = inputTelephoneCheck("请输入管理员的电话号码: ");
        Admin *current = admin;
        bool found = false;
        lines.push_back("正在查找电话号码为 " + telephone + " 的管理员信息...");
        while (current != nullptr)
        {
            if (current->getTelephone() == telephone && !current->getIsDeleted())
            {
                lines.push_back("用户ID: " + current->getUserID() + ", 姓名: " + current->getUsername()
                          + ", 性别: " + current->getGender() + ", 年龄: " + std::to_string(current->getAge())
                          + ", 电话: " + current->getTelephone() + ", 邮箱: " + current->getEmail()
                          + ", 账户状态: " + (current->getIsAccountActive() ? "激活" : "锁定")
                          + ", 创建时间: " + current->getCreateTime());
                lines.push_back("");
                found = true;
            }
            current = current->next;
        }
        if (!found)
        {
            printError("未找到电话号码为 " + telephone + " 的管理员信息！");
        }
    }
    else if (contactChoice == 2)
    {
        std::string email = inputEmailCheck("请输入管理员电子邮箱: ");
        Admin *current = admin;
        bool found = false;
        lines.push_back("正在查找电子邮箱为 " + email + " 的管理员信息...");
        while (current != nullptr)
        {
            if (current->getEmail() == email && !current->getIsDeleted())
            {
                lines.push_back("用户ID: " + current->getUserID() + ", 姓名: " + current->getUsername()
                          + ", 性别: " + current->getGender() + ", 年龄: " + std::to_string(current->getAge())
                          + ", 电话: " + current->getTelephone() + ", 邮箱: " + current->getEmail()
                          + ", 账户状态: " + (current->getIsAccountActive() ? "激活" : "锁定")
                          + ", 创建时间: " + current->getCreateTime());
                lines.push_back("");
                found = true;
            }
            current = current->next;
        }
        if (!found)
        {
            printError("未找到电子邮箱为 " + email + " 的管理员信息！");
        }
    }
    printWithPagination(lines, 10);
}
// 修改管理员名字
void modifyAdminName(Admin *&admin)
{
    std::string targetID = inputIDCheck("请输入要修改信息的管理员用户ID: ");
    Admin *current = admin;
    bool found = false;
    while (current != nullptr)
    {
        if (current->getUserID() == targetID && !current->getIsDeleted())
        {
            std::cout << "现在的管理员姓名是: " << current->getUsername() << std::endl;
            std::string newName = inputStringCheck("请输入新的管理员姓名: ");
            current->setUsername(newName);
            printSuccess("管理员姓名已更新！");
            found = true;
            break;
        }
        current = current->next;
    }
    if (!found)
    {
        printError("未找到用户ID为 " + targetID + " 的管理员！");
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
        if (current->getUserID() == targetID && !current->getIsDeleted())
        {
            std::cout << "现在的管理员性别是: " << current->getGender() << std::endl;
            std::string newGender = inputGenderCheck("请输入新的管理员性别");
            current->setGender(newGender);
            printSuccess("管理员性别已更新！");
            found = true;
            break;
        }
        current = current->next;
    }
    if (!found)
    {
        printError("未找到用户ID为 " + targetID + " 的管理员！");
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
        if (current->getUserID() == targetID && !current->getIsDeleted())
        {
            std::cout << "现在的管理员年龄是: " << current->getAge() << std::endl;
            int newAge = inputAgeCheck("请输入新的管理员年龄: ");
            current->setAge(newAge);
            printSuccess("管理员年龄已更新！");
            found = true;
            break;
        }
        current = current->next;
    }
    if (!found)
    {
        printError("未找到用户ID为 " + targetID + " 的管理员！");
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
        if (current->getUserID() == targetID && !current->getIsDeleted())
        {
            std::cout << "现在的管理员电话号码是: " << current->getTelephone() << std::endl;
            std::string newTelephone = inputTelephoneCheck("请输入新的管理员电话号码");
            current->setTelephone(newTelephone);
            printSuccess("管理员电话号码已更新！");
            found = true;
            break;
        }
        current = current->next;
    }
    if (!found)
    {
        printError("未找到用户ID为 " + targetID + " 的管理员！");
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
        if (current->getUserID() == targetID && !current->getIsDeleted())
        {
            std::cout << "现在的管理员电子邮箱是: " << current->getEmail() << std::endl;
            std::string newEmail = inputEmailCheck("请输入新的管理员电子邮箱");
            current->setEmail(newEmail);
            printSuccess("管理员电子邮箱已更新！");
            found = true;
            break;
        }
        current = current->next;
    }
    if (!found)
    {
        printError("未找到用户ID为 " + targetID + " 的管理员！");
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
        if (current->getUserID() == targetID && !current->getIsDeleted())
        {
            current->setIsDeleted(true);
            printSuccess("管理员账号已删除！");
            found = true;
            break;
        }
        current = current->next;
    }
    if (!found)
    {
        printError("未找到用户ID为 " + targetID + " 的有效管理员！");
    }
}
// 添加管理员(通过注册流程创建新管理员账号，并插入到管理员链表中)
void addAdmin(Admin *&admin, int &idCounter)
{
    Admin *newAdmin = new Admin();
    if (newAdmin->adminSignUp(idCounter) && newAdmin->getIsAccountActive()) // 调用管理员注册方法创建新管理员账号，并检查是否成功注册且账户激活
    {
        newAdmin->next = admin; // 将新管理员插入到链表头部
        if (admin != nullptr) admin->prev = newAdmin;
        admin = newAdmin;
        printSuccess("新管理员账号已添加！");
    }
    else
    {
        delete newAdmin; // 注册失败或账户未激活，释放内存
        printError("管理员账号添加失败！");
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
            int viewChoice = adminAdminViewMenu();
            if (viewChoice == 1)
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
            // viewChoice == 0: 返回上级菜单，直接 fall through
        }
        else if (choice == 2)
        {
            int modifyChoice = adminAdminModificationMenu();
            if (modifyChoice == 1)
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
            // modifyChoice == 0: 返回上级菜单，直接 fall through
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