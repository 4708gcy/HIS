
#include "../Head/UI.h"

// 暂停函数，等待用户按键继续
void pause()
{
    std::cout << "按下回车键继续..." << std::flush;
    std::string dummy;
    std::getline(std::cin, dummy);
}

// 去除字符串首尾空格的辅助函数
std::string trim(const std::string &s)
{
    auto start = s.find_first_not_of(" \t\r\n");
    auto end = s.find_last_not_of(" \t\r\n");
    return (start == std::string::npos) ? "" : s.substr(start, end - start + 1);
}

// ============================ 输入校验函数区域 =======================================
// 1. 安全整数输入
int selectIntCheck(const int min, const int max)
{
    int choice;
    std::string line;
    while (true)
    {
        std::cout << "请输入整数: ";
        std::getline(std::cin, line);
        line = trim(line); // 去除首尾空格
        std::stringstream ss(line);
        if (ss >> choice && !(ss >> line) && choice >= min && choice <= max)
        {
            return choice;
        }
        else
        {
            std::cout << "无效输入，请输入 " << min << " - " << max << " 之间的数字!" << std::endl;
        }
    }
}

// 2. 安全 double 输入
double inputFeeCheck(const std::string &prompt)
{
    double fee;
    std::string line;
    while (true)
    {
        std::cout << prompt;
        std::getline(std::cin, line);
        line = trim(line); // 去除首尾空格
        std::stringstream ss(line);
        if (ss >> fee && !(ss >> line) && fee >= 0)
        {
            return fee;
        }
        else
        {
            std::cout << "无效输入，请输入一个非负数!" << std::endl;
        }
    }
}

// 3. 安全字符串输入
std::string inputStringCheck(const std::string &prompt)
{
    std::string input;
    while (true)
    {
        std::cout << prompt;
        std::getline(std::cin, input);
        input = trim(input); // 去除首尾空格
        if (input.empty())
        {
            std::cout << "输入不能为空，请重新输入!" << std::endl;
        }
        else if (input.find(',') != std::string::npos)
        {
            std::cout << "输入不能包含英文逗号(,)！请重新输入!" << std::endl;
        }
        else
        {
            return input;
        }
    }
}

// 4. 安全ID输入
std::string inputIDCheck(const std::string &prompt)
{
    std::string id;
    while (true)
    {
        std::cout << prompt;
        std::getline(std::cin, id);
        id = trim(id); // 去除首尾空格
        // 检查长度、首位、后五位是否全为数字
        if (id.length() == 6 &&
            (id[0] >= '0' && id[0] <= '4') &&
            std::all_of(id.begin() + 1, id.end(), ::isdigit))
        {
            return id;
        }
        else
        {
            std::cout << "无效的ID格式，请输入一个6位数字，首位为0-4，其余为数字！" << std::endl;
        }
    }
}
/*
挂号记录：inputRecordIDCheck("请输入挂号记录ID: ", {"reg"});
看诊记录：inputRecordIDCheck("请输入看诊记录ID: ", {"con"});
检查记录：inputRecordIDCheck("请输入检查记录ID: ", {"exa"});
住院记录：inputRecordIDCheck("请输入住院记录ID: ", {"hos"});
药品记录：inputRecordIDCheck("请输入药品记录ID: ", {"med"});
用药记录：inputRecordIDCheck("请输入用药记录ID: ", {"mrd"});
或者允许多种类型：inputRecordIDCheck("请输入记录ID: ", {"reg", "con", "exa", "hos", "med", "mrd"});
*/
// 通用记录ID校验函数
std::string inputRecordIDCheck(const std::string &prompt, const std::vector<std::string> &prefixes)
{
    std::string id;
    while (true)
    {
        std::cout << prompt;
        std::getline(std::cin, id);
        id = trim(id); // 去除首尾空格

        bool valid = false;
        for (const auto &prefix : prefixes)
        {
            if (id.length() == prefix.length() + 6 &&
                id.substr(0, prefix.length()) == prefix &&
                std::all_of(id.begin() + prefix.length(), id.end(), ::isdigit))
            {
                valid = true;
                break;
            }
        }

        if (valid)
            return id;
        else
            std::cout << "无效的记录ID格式，请输入正确的ID（如reg/6位数字、con/6位数字等）！" << std::endl;
    }
}

// 5. 安全密码输入
std::string inputPwdCheck(const std::string &prompt)
{
    std::string pwd;
    while (true)
    {
        std::cout << prompt << "(密码必须至少8位，包含字母和数字): " << std::endl;
        std::getline(std::cin, pwd);
        pwd = trim(pwd); // 去除首尾空格

        if (pwd.length() < 8)
        {
            std::cout << "密码长度不能少于8位，请重新输入！" << std::endl;
            continue;
        }

        bool hasAlpha = false, hasDigit = false;
        for (char c : pwd)
        {
            if (std::isalpha(static_cast<unsigned char>(c)))
                hasAlpha = true;
            if (std::isdigit(static_cast<unsigned char>(c)))
                hasDigit = true;
        }
        if (!hasAlpha || !hasDigit)
        {
            std::cout << "密码必须包含字母和数字，请重新输入！" << std::endl;
            continue;
        }

        return pwd;
    }
}

// 6. 安全床位号输入
std::string inputBedNumberCheck(const std::string &prompt, std::string department, std::string wardType)
{
    std::string bedNumber, dpt, area, type, ward, num;
    if (department.empty() || wardType.empty())
    {
        std::cout << "部门和病房类型不能为空！" << std::endl;
        return "";
    }

    if (department == "内科")
    {
        dpt = "N";
    }
    else if (department == "外科")
    {
        dpt = "W";
    }
    else if (department == "妇产科")
    {
        dpt = "F";
    }
    else if (department == "急诊科")
    {
        dpt = "J";
    }
    else if (department == "儿科")
    {
        dpt = "E";
    }
    else
    {
        std::cout << "无效的部门！" << std::endl;
        return "";
    }

    if (wardType == "普通病房")
    {
        type = "P";
    }
    else if (wardType == "隔离病房")
    {
        type = "G";
    }
    else if (wardType == "VIP病房")
    {
        type = "V";
    }
    else if (wardType == "ICU病房")
    {
        type = "I";
    }
    else
    {
        std::cout << "无效的病房类型！" << std::endl;
        return "";
    }

    while (true)
    {
        std::cout << prompt << "（区域号2位[00-10]，病房号3位[000-999]，床位号2位[00-10]）" << std::endl;

        std::cout << "请输入区域号（2位数字，00-10）：";
        std::getline(std::cin, area);
        area = trim(area);
        if (area.length() != 2 || !std::isdigit(area[0]) || !std::isdigit(area[1]) ||
            std::stoi(area) < 0 || std::stoi(area) > 10)
        {
            std::cout << "区域号格式错误，请重新输入！" << std::endl;
            continue;
        }

        std::cout << "请输入病房号（3位数字，000-999）：";
        std::getline(std::cin, ward);
        ward = trim(ward);
        if (ward.length() != 3 || !std::isdigit(ward[0]) || !std::isdigit(ward[1]) || !std::isdigit(ward[2]) ||
            std::stoi(ward) < 0 || std::stoi(ward) > 999)
        {
            std::cout << "病房号格式错误，请重新输入！" << std::endl;
            continue;
        }

        std::cout << "请输入床位号（2位数字，00-10）：";
        std::getline(std::cin, num);
        num = trim(num);
        if (num.length() != 2 || !std::isdigit(num[0]) || !std::isdigit(num[1]) ||
            std::stoi(num) < 0 || std::stoi(num) > 10)
        {
            std::cout << "床位号格式错误，请重新输入！" << std::endl;
            continue;
        }

        // 拼接床位号
        std::string bedNumber = dpt + "-" + area + "-" + type + "-" + ward + "-" + num;
        return bedNumber;
    }
}

// 7. 根据输入信息自动生成床位ID
std::string autoGenerateBedID(const std::string &department, const std::string &wardType, int areaNumber, int wardNumber, int bedNumber)
{
    std::string dpt, type;

    if (department == "内科")
    {
        dpt = "N";
    }
    else if (department == "外科")
    {
        dpt = "W";
    }
    else if (department == "妇产科")
    {
        dpt = "F";
    }
    else if (department == "急诊科")
    {
        dpt = "J";
    }
    else if (department == "儿科")
    {
        dpt = "E";
    }
    else
    {
        return "";
    }

    if (wardType == "普通病房")
    {
        type = "P";
    }
    else if (wardType == "隔离病房")
    {
        type = "G";
    }
    else if (wardType == "VIP病房")
    {
        type = "V";
    }
    else if (wardType == "ICU病房")
    {
        type = "I";
    }
    else
    {
        return "";
    }

    // 拼接床位ID
    char buffer[20];
    snprintf(buffer, sizeof(buffer), "%s-%02d-%s-%03d-%02d", dpt.c_str(), areaNumber, type.c_str(), wardNumber, bedNumber);
    return std::string(buffer);
}

// 8. 安全性别输入
std::string inputGenderCheck(const std::string &prompt)
{
    std::string gender;

    while (true)
    {
        std::cout << prompt << "（男/女）" << std::endl;
        std::getline(std::cin, gender);
        gender = trim(gender);

        if (gender == "男" || gender == "女")
        {
            break;
        }
        else
        {
            std::cout << "输入无效，请重新输入！" << std::endl;
        }
    }

    return gender;
}

// 9. 安全电话号码输入
std::string inputTelephoneCheck(const std::string &prompt)
{
    std::string telephone;

    while (true)
    {
        std::cout << prompt << "（11位数字）：";
        std::getline(std::cin, telephone);
        telephone = trim(telephone);

        if (telephone.length() == 11 && std::all_of(telephone.begin(), telephone.end(), ::isdigit))
        {
            break;
        }
        else
        {
            std::cout << "输入无效，请重新输入！" << std::endl;
        }
    }

    return telephone;
}

// 10. 安全邮箱地址输入
std::string inputEmailCheck(const std::string &prompt)
{
    std::string email;

    while (true)
    {
        std::cout << prompt << "（必须包含@和.）：";
        std::getline(std::cin, email);
        email = trim(email);

        if (email.find('@') != std::string::npos && email.find('.') != std::string::npos)
        {
            break;
        }
        else
        {
            std::cout << "输入无效，请重新输入！" << std::endl;
        }
    }

    return email;
}

// 11 . 安全年龄输入
int inputAgeCheck(const std::string &prompt)
{
    int age;
    std::string line;
    while (true)
    {
        std::cout << prompt << "（0-150）: ";
        std::getline(std::cin, line);
        line = trim(line); // 去除首尾空格
        std::stringstream ss(line);
        if (ss >> age && !(ss >> line) && age >= 0 && age <= 150)
        {
            return age;
        }
        else
        {
            std::cout << "无效输入，请输入0-150之间的整数！" << std::endl;
        }
    }
}
// 12. 安全日期输入
std::string inputDateCheck(const std::string &prompt)
{
    std::string date;
    while (true)
    {
        std::cout << prompt << "（格式 YYYY-MM-DD）: ";
        std::getline(std::cin, date);
        date = trim(date);

        if (date.length() == 10 && date[4] == '-' && date[7] == '-' &&
            std::all_of(date.begin(), date.end(), [](char c)
                        { return std::isdigit(c) || c == '-'; }))
        {
            break;
        }
        else
        {
            std::cout << "输入无效，请重新输入！" << std::endl;
        }
    }
    return date;
}

// 13. 正确医疗科室输入
std::string inputDepartmentCheck(const std::string &prompt)
{
    std::string department;
    while (true)
    {
        std::cout << prompt << "（内科/外科/妇产科/急诊科/儿科/#）: ";
        std::getline(std::cin, department);
        department = trim(department);

        if (department == "内科" || department == "外科" || department == "妇产科" || department == "急诊科" || department == "儿科" || department == "#")
        {
            break;
        }
        else
        {
            std::cout << "输入无效，请重新输入！" << std::endl;
        }
    }
    return department;
}

// 14. 安全身份证号输入
std::string inputIDcardCheck(const std::string &prompt)
{
    std::string idCard;
    while (true)
    {
        std::cout << prompt << "（18位数字或最后一位可以是X）: ";
        std::getline(std::cin, idCard);
        idCard = trim(idCard);

        if ((idCard.length() == 18 && std::all_of(idCard.begin(), idCard.end() - 1, ::isdigit) &&
             (std::isdigit(idCard.back()) || idCard.back() == 'X' || idCard.back() == 'x')))
        {
            break;
        }
        else
        {
            std::cout << "输入无效，请重新输入！" << std::endl;
        }
    }
    return idCard;
}

// 15. 安全整数输入（带提示信息）
int inputIntCheck(const std::string &prompt, int min, int max)
{
    int value;
    std::string line;
    while (true)
    {
        std::cout << prompt;
        std::getline(std::cin, line);
        line = trim(line); // 去除首尾空格
        std::stringstream ss(line);
        if (ss >> value && !(ss >> line) && value >= min && value <= max)
        {
            return value;
        }
        else
        {
            std::cout << "无效输入，请输入一个在 " << min << " 和 " << max << " 之间的整数！" << std::endl;
        }
    }
}

// 16. 安全浮点数输入（带提示信息）
double inputDoubleCheck(const std::string &prompt, double min, double max)
{
    double value;
    std::string line;
    while (true)
    {
        std::cout << prompt;
        std::getline(std::cin, line);
        line = trim(line); // 去除首尾空格
        std::stringstream ss(line);
        if (ss >> value && !(ss >> line) && value >= min && value <= max)
        {
            return value;
        }
        else
        {
            std::cout << "无效输入，请输入一个在 " << min << " 和 " << max << " 之间的数值！" << std::endl;
        }
    }
}

//============================= 菜单显示辅助函数区域 =======================================

// 打印菜单边框（宽度50字符）
static void printMenuBorder()
{
    std::cout << "+";
    for (int i = 0; i < 50; i++)
        std::cout << "-";
    std::cout << "+" << std::endl;
}

// 打印居中标题
static void printMenuTitle(const std::string &title)
{
    int padding = (50 - (int)title.length()) / 2;
    if (padding < 1)
        padding = 1;
    std::cout << "|";
    for (int i = 0; i < padding; i++)
        std::cout << " ";
    std::cout << title;
    int rightPadding = 50 - padding - (int)title.length();
    for (int i = 0; i < rightPadding; i++)
        std::cout << " ";
    std::cout << "|" << std::endl;
}

// 打印菜单分隔线
static void printMenuSeparator()
{
    std::cout << "|";
    for (int i = 0; i < 50; i++)
        std::cout << " ";
    std::cout << "|" << std::endl;
}

//============================= 菜单显示函数区域 =======================================

// 身份选择界面
int identitySelectionMenu()
{
    printMenuBorder();
    printMenuTitle("身份选择");
    printMenuBorder();
    std::cout << "|  1. 管理员                                    |" << std::endl;
    std::cout << "|  2. 医生                                      |" << std::endl;
    std::cout << "|  3. 护士                                      |" << std::endl;
    std::cout << "|  4. 药剂师                                    |" << std::endl;
    std::cout << "|  5. 患者                                      |" << std::endl;
    std::cout << "|  0. 退出系统                                  |" << std::endl;
    printMenuBorder();

    int choice = selectIntCheck(0, 5);
    return choice;
}

// 登录和注册选择界面
int loginRegisterUI()
{
    printMenuBorder();
    printMenuTitle("欢迎使用 HIS 医院管理系统");
    printMenuBorder();
    std::cout << "|  1. 登录                                      |" << std::endl;
    std::cout << "|  2. 注册                                      |" << std::endl;
    std::cout << "|  0. 退出系统                                  |" << std::endl;
    printMenuBorder();

    int choice = selectIntCheck(0, 2);
    return choice;
}

// 开始界面，显示欢迎信息和主菜单
int beginUI()
{
    printMenuBorder();
    printMenuTitle("欢迎使用 HIS 系统");
    printMenuBorder();
    std::cout << "|  1. 管理员                                    |" << std::endl;
    std::cout << "|  2. 医生                                      |" << std::endl;
    std::cout << "|  3. 护士                                      |" << std::endl;
    std::cout << "|  4. 药剂师                                    |" << std::endl;
    std::cout << "|  5. 患者                                      |" << std::endl;
    std::cout << "|  0. 返回上一级                                |" << std::endl;
    printMenuBorder();

    int choice = selectIntCheck(0, 5);
    return choice;
}

// 管理员科室管理菜单(可扩展)
std::string adminDepartmentMenu()
{
    printMenuBorder();
    printMenuTitle("科室选择");
    printMenuBorder();
    std::cout << "|  1. 内科                                      |" << std::endl;
    std::cout << "|  2. 外科                                      |" << std::endl;
    std::cout << "|  3. 妇产科                                    |" << std::endl;
    std::cout << "|  4. 急诊科                                    |" << std::endl;
    std::cout << "|  5. 儿科                                      |" << std::endl;
    std::cout << "|  0. 返回上级菜单                              |" << std::endl;
    printMenuBorder();

    int choice = selectIntCheck(0, 5);

    switch (choice)
    {
    case 0:
        return "0";
    case 1:
        return "内科";
    case 2:
        return "外科";
    case 3:
        return "妇产科";
    case 4:
        return "急诊科";
    case 5:
        return "儿科";
    default:
        return "";
    }
    return "";
}

// 管理员身份主菜单
int adminMenu()
{
    printMenuBorder();
    printMenuTitle("管理员主菜单");
    printMenuBorder();
    std::cout << "|  1. 账户管理                                  |" << std::endl;
    std::cout << "|  2. 医疗记录管理                              |" << std::endl;
    std::cout << "|  3. 药品管理                                  |" << std::endl;
    std::cout << "|  4. 床位管理                                  |" << std::endl;
    std::cout << "|  5. 管理员信息管理                            |" << std::endl;
    std::cout << "|  6. 个人信息管理                              |" << std::endl;
    std::cout << "|  7. 账号封锁管理                              |" << std::endl;
    std::cout << "|  0. 退出登录                                  |" << std::endl;
    printMenuBorder();

    int choice = selectIntCheck(0, 7);
    return choice;
}

// 管理员医疗记录管理菜单
int adminMedicalRecordMenu()
{
    printMenuBorder();
    printMenuTitle("医疗记录管理");
    printMenuBorder();
    std::cout << "|  1. 挂号记录                                  |" << std::endl;
    std::cout << "|  2. 看诊记录                                  |" << std::endl;
    std::cout << "|  3. 检查记录                                  |" << std::endl;
    std::cout << "|  4. 住院记录                                  |" << std::endl;
    std::cout << "|  5. 用药记录                                  |" << std::endl;
    std::cout << "|  0. 返回上级菜单                              |" << std::endl;
    printMenuBorder();

    int choice = selectIntCheck(0, 5);
    return choice;
}

// 管理员挂号记录管理菜单
int adminRegistrationManagementMenu()
{
    printMenuBorder();
    printMenuTitle("挂号记录管理");
    printMenuBorder();
    std::cout << "|  1. 查看挂号记录                              |" << std::endl;
    std::cout << "|  2. 修改挂号状态                              |" << std::endl;
    std::cout << "|  3. 删除挂号记录                              |" << std::endl;
    std::cout << "|  4. 添加挂号记录                              |" << std::endl;
    std::cout << "|  0. 返回上级菜单                              |" << std::endl;
    printMenuBorder();

    int choice = selectIntCheck(0, 4);
    return choice;
}

// 管理员挂号记录查看方式选择菜单
int adminRegistrationViewMenu()
{
    printMenuBorder();
    printMenuTitle("挂号记录查看");
    printMenuBorder();
    std::cout << "|  1. 查看该科室的所有挂号记录                  |" << std::endl;
    std::cout << "|  2. 根据挂号状态查看                          |" << std::endl;
    std::cout << "|  3. 根据患者ID查看                            |" << std::endl;
    std::cout << "|  4. 根据医生ID查看                            |" << std::endl;
    std::cout << "|  5. 根据挂号记录ID查看                        |" << std::endl;
    std::cout << "|  0. 返回上级菜单                              |" << std::endl;
    printMenuBorder();
    int viewChoice = selectIntCheck(0, 5);
    return viewChoice;
}

// 管理员看诊记录管理菜单
int adminConsultationManagementMenu()
{
    printMenuBorder();
    printMenuTitle("看诊记录管理");
    printMenuBorder();
    std::cout << "|  1. 查看看诊记录                              |" << std::endl;
    std::cout << "|  2. 修改看诊状态                              |" << std::endl;
    std::cout << "|  3. 删除看诊记录                              |" << std::endl;
    std::cout << "|  4. 添加看诊记录                              |" << std::endl;
    std::cout << "|  0. 返回上级菜单                              |" << std::endl;
    printMenuBorder();

    int choice = selectIntCheck(0, 4);
    return choice;
}

// 管理员看诊记录查看方式选择菜单
int adminConsultationViewMenu()
{
    printMenuBorder();
    printMenuTitle("看诊记录查看");
    printMenuBorder();
    std::cout << "|  1. 查看该科室的所有看诊记录                  |" << std::endl;
    std::cout << "|  2. 根据患者ID查看                            |" << std::endl;
    std::cout << "|  3. 根据医生ID查看                            |" << std::endl;
    std::cout << "|  4. 根据状态查看                              |" << std::endl;
    std::cout << "|  5. 根据挂号记录ID查看                        |" << std::endl;
    std::cout << "|  6. 根据看诊记录ID查看                        |" << std::endl;
    std::cout << "|  0. 返回上级菜单                              |" << std::endl;
    printMenuBorder();
    int viewChoice = selectIntCheck(0, 6);
    return viewChoice;
}

// 管理员检查记录管理菜单
int adminExaminationManagementMenu()
{
    printMenuBorder();
    printMenuTitle("检查记录管理");
    printMenuBorder();
    std::cout << "|  1. 查看检查记录                              |" << std::endl;
    std::cout << "|  2. 修改检查状态                              |" << std::endl;
    std::cout << "|  3. 删除检查记录                              |" << std::endl;
    std::cout << "|  4. 添加检查记录                              |" << std::endl;
    std::cout << "|  0. 返回上级菜单                              |" << std::endl;
    printMenuBorder();

    int choice = selectIntCheck(0, 4);
    return choice;
}

// 管理员检查记录查看方式选择菜单
int adminExaminationViewMenu()
{
    printMenuBorder();
    printMenuTitle("检查记录查看");
    printMenuBorder();
    std::cout << "|  1. 查看该科室的所有检查记录                  |" << std::endl;
    std::cout << "|  2. 根据患者ID查看                            |" << std::endl;
    std::cout << "|  3. 根据医生ID查看                            |" << std::endl;
    std::cout << "|  4. 根据状态查看                              |" << std::endl;
    std::cout << "|  5. 根据检查记录ID查看                        |" << std::endl;
    std::cout << "|  0. 返回上级菜单                              |" << std::endl;
    printMenuBorder();
    int viewChoice = selectIntCheck(0, 5);
    return viewChoice;
}

// 检查项目选择菜单
std::string ExaminationItemMenu()
{
    printMenuBorder();
    printMenuTitle("检查项目选择");
    printMenuBorder();
    std::cout << "|  1. 体温测量                                  |" << std::endl;
    std::cout << "|  2. 血压测量                                  |" << std::endl;
    std::cout << "|  3. 心率测量                                  |" << std::endl;
    std::cout << "|  4. 呼吸频率测量                              |" << std::endl;
    std::cout << "|  5. 脉搏血氧测量                              |" << std::endl;
    std::cout << "|  6. 身高测量                                  |" << std::endl;
    std::cout << "|  7. 体重测量                                  |" << std::endl;
    std::cout << "|  8. BMI计算                                   |" << std::endl;
    std::cout << "|  9. 疼痛评估                                  |" << std::endl;
    std::cout << "|  10. 腰围测量                                 |" << std::endl;
    std::cout << "|  11. 血糖测量                                 |" << std::endl;
    std::cout << "|  12. 体脂测量                                 |" << std::endl;
    std::cout << "|  13. 尿酸测定                                 |" << std::endl;
    std::cout << "|  14. 血脂测定                                 |" << std::endl;
    std::cout << "|  0. 返回上级菜单                              |" << std::endl;
    printMenuBorder();

    int itemChoice = selectIntCheck(0, 14);

    std::string itemName = "0"; // 默认返回上级菜单

    if (itemChoice == 1)
    {
        itemName = "体温测量";
    }
    else if (itemChoice == 2)
    {
        itemName = "血压测量";
    }
    else if (itemChoice == 3)
    {
        itemName = "心率测量";
    }
    else if (itemChoice == 4)
    {
        itemName = "呼吸频率测量";
    }
    else if (itemChoice == 5)
    {
        itemName = "脉搏血氧测量";
    }
    else if (itemChoice == 6)
    {
        itemName = "身高测量";
    }
    else if (itemChoice == 7)
    {
        itemName = "体重测量";
    }
    else if (itemChoice == 8)
    {
        itemName = "BMI计算";
    }
    else if (itemChoice == 9)
    {
        itemName = "疼痛评估";
    }
    else if (itemChoice == 10)
    {
        itemName = "腰围测量";
    }
    else if (itemChoice == 11)
    {
        itemName = "血糖测量";
    }
    else if (itemChoice == 12)
    {
        itemName = "体脂测量";
    }
    else if (itemChoice == 13)
    {
        itemName = "尿酸测定";
    }
    else if (itemChoice == 14)
    {
        itemName = "血脂测定";
    }

    return itemName; // 返回上级菜单
}

// 管理员住院记录管理菜单
int adminHospitalizationManagementMenu()
{
    printMenuBorder();
    printMenuTitle("住院记录管理");
    printMenuBorder();
    std::cout << "|  1. 查看住院记录                              |" << std::endl;
    std::cout << "|  2. 修改住院记录状态                          |" << std::endl;
    std::cout << "|  3. 删除住院记录                              |" << std::endl;
    std::cout << "|  4. 添加住院记录                              |" << std::endl;
    std::cout << "|  0. 返回上级菜单                              |" << std::endl;
    printMenuBorder();

    int choice = selectIntCheck(0, 4);
    return choice;
}

// 管理员住院记录查看方式选择菜单
int adminHospitalizationViewMenu()
{
    printMenuBorder();
    printMenuTitle("住院记录查看");
    printMenuBorder();
    std::cout << "|  1. 查看该科室的所有住院记录                  |" << std::endl;
    std::cout << "|  2. 根据患者ID查看                            |" << std::endl;
    std::cout << "|  3. 根据医生ID查看                            |" << std::endl;
    std::cout << "|  4. 根据护士ID查看                            |" << std::endl;
    std::cout << "|  5. 根据住院记录ID查看                        |" << std::endl;
    std::cout << "|  6. 根据状态查看                              |" << std::endl;
    std::cout << "|  7. 根据病房类型查看                          |" << std::endl;
    std::cout << "|  0. 返回上级菜单                              |" << std::endl;
    printMenuBorder();
    int viewChoice = selectIntCheck(0, 7);
    return viewChoice;
}

// 管理员住院记录病房类型选择菜单
std::string HospitalizationWardTypeMenu()
{
    printMenuBorder();
    printMenuTitle("病房类型选择");
    printMenuBorder();
    std::cout << "|  1. 普通病房                                  |" << std::endl;
    std::cout << "|  2. 隔离病房                                  |" << std::endl;
    std::cout << "|  3. VIP病房                                   |" << std::endl;
    std::cout << "|  4. ICU病房                                   |" << std::endl;
    std::cout << "|  0. 返回上级菜单                              |" << std::endl;
    printMenuBorder();

    int choice = selectIntCheck(0, 4);
    switch (choice)
    {
    case 0:
        return "0";
    case 1:
        return "普通病房";
    case 2:
        return "隔离病房";
    case 3:
        return "VIP病房";
    case 4:
        return "ICU病房";
    default:
        return "";
    }
}

// 管理员床位管理菜单
int bedManagementMenu()
{
    printMenuBorder();
    printMenuTitle("床位管理");
    printMenuBorder();
    std::cout << "|  1. 查看床位信息                              |" << std::endl;
    std::cout << "|  2. 修改床位状态                              |" << std::endl;
    std::cout << "|  3. 删除床位信息                              |" << std::endl;
    std::cout << "|  4. 添加床位信息                              |" << std::endl;
    std::cout << "|  0. 返回上级菜单                              |" << std::endl;
    printMenuBorder();

    int choice = selectIntCheck(0, 4);
    return choice;
}

// 管理员床位查看方式选择菜单
int bedViewMenu()
{
    printMenuBorder();
    printMenuTitle("床位信息查看");
    printMenuBorder();
    std::cout << "|  1. 查看该科室的所有床位信息                  |" << std::endl;
    std::cout << "|  2. 根据床位状态查看                          |" << std::endl;
    std::cout << "|  3. 根据病房类型查看                          |" << std::endl;
    std::cout << "|  4. 根据床位号查看                            |" << std::endl;
    std::cout << "|  5. 根据患者ID查看                            |" << std::endl;
    std::cout << "|  6. 根据住院记录ID查看                        |" << std::endl;
    std::cout << "|  7. 根据护士ID查看                            |" << std::endl;
    std::cout << "|  0. 返回上级菜单                              |" << std::endl;
    printMenuBorder();

    int viewChoice = selectIntCheck(0, 7);
    return viewChoice;
}

// 管理员用药记录管理菜单
int adminMedicationRecordManagementMenu()
{
    printMenuBorder();
    printMenuTitle("用药记录管理");
    printMenuBorder();
    std::cout << "|  1. 查看用药记录                              |" << std::endl;
    std::cout << "|  2. 修改用药记录状态                          |" << std::endl;
    std::cout << "|  3. 删除用药记录                              |" << std::endl;
    std::cout << "|  4. 添加用药记录                              |" << std::endl;
    std::cout << "|  0. 返回上级菜单                              |" << std::endl;
    printMenuBorder();

    int choice = selectIntCheck(0, 4);
    return choice;
}

// 管理员用药记录查看方式选择菜单
int adminMedicationRecordViewMenu()
{
    printMenuBorder();
    printMenuTitle("用药记录查看");
    printMenuBorder();
    std::cout << "|  1. 查看该科室的所有用药记录                  |" << std::endl;
    std::cout << "|  2. 根据患者ID查看                            |" << std::endl;
    std::cout << "|  3. 根据医生ID查看                            |" << std::endl;
    std::cout << "|  4. 根据药师ID查看                            |" << std::endl;
    std::cout << "|  5. 根据用药记录ID查看                        |" << std::endl;
    std::cout << "|  6. 根据药品ID查看                            |" << std::endl;
    std::cout << "|  7. 根据看诊记录ID查看                        |" << std::endl;
    std::cout << "|  8. 根据审核状态查看                          |" << std::endl;
    std::cout << "|  9. 根据发药状态查看                          |" << std::endl;
    std::cout << "|  0. 返回上级菜单                              |" << std::endl;
    printMenuBorder();

    int viewChoice = selectIntCheck(0, 9);
    return viewChoice;
}

// 用药记录审核状态设置菜单
int MedicationRecordReviewResultMenu()
{
    printMenuBorder();
    printMenuTitle("用药记录审核状态");
    printMenuBorder();
    std::cout << "|  1. 待审核                                    |" << std::endl;
    std::cout << "|  2. 审核通过                                  |" << std::endl;
    std::cout << "|  3. 审核未通过                                |" << std::endl;
    std::cout << "|  4. 医生撤销                                  |" << std::endl;
    std::cout << "|  0. 返回上级菜单                              |" << std::endl;
    printMenuBorder();

    int choice = selectIntCheck(0, 4);
    return choice;
}

// 用药记录状态设置菜单
int MedicationRecordStatusMenu()
{
    printMenuBorder();
    printMenuTitle("用药记录状态");
    printMenuBorder();
    std::cout << "|  1. 未缴费                                    |" << std::endl;
    std::cout << "|  2. 待发药                                    |" << std::endl;
    std::cout << "|  3. 已发药                                    |" << std::endl;
    std::cout << "|  4. 已退药                                    |" << std::endl;
    std::cout << "|  0. 返回上级菜单                              |" << std::endl;
    printMenuBorder();

    int choice = selectIntCheck(0, 4);
    return choice;
}

// 管理员药品管理菜单
int adminMedicineManagementMenu()
{
    printMenuBorder();
    printMenuTitle("药品管理");
    printMenuBorder();
    std::cout << "|  1. 查看药品信息                              |" << std::endl;
    std::cout << "|  2. 修改药品信息                              |" << std::endl;
    std::cout << "|  3. 删除药品信息                              |" << std::endl;
    std::cout << "|  4. 添加药品信息                              |" << std::endl;
    std::cout << "|  0. 返回上级菜单                              |" << std::endl;
    printMenuBorder();

    int choice = selectIntCheck(0, 4);
    return choice;
}

// 管理员药品查看方式选择菜单
int adminMedicineViewMenu()
{
    printMenuBorder();
    printMenuTitle("药品信息查看");
    printMenuBorder();
    std::cout << "|  1. 查看所有药品信息                          |" << std::endl;
    std::cout << "|  2. 根据药品ID查看                            |" << std::endl;
    std::cout << "|  3. 根据药品名称查看                          |" << std::endl;
    std::cout << "|  4. 根据药品状态查看                          |" << std::endl;
    std::cout << "|  5. 根据生产厂家查看                          |" << std::endl;
    std::cout << "|  6. 查看低于安全库存量的药品                  |" << std::endl;
    std::cout << "|  0. 返回上级菜单                              |" << std::endl;
    printMenuBorder();

    int viewChoice = selectIntCheck(0, 6);
    return viewChoice;
}

// 管理员药品状态设置菜单
int MedicineStatusMenu()
{
    printMenuBorder();
    printMenuTitle("药品状态选择");
    printMenuBorder();
    std::cout << "|  1. 正常可用                                  |" << std::endl;
    std::cout << "|  2. 库存低于安全阈值                          |" << std::endl;
    std::cout << "|  3. 已过期                                    |" << std::endl;
    std::cout << "|  4. 已停用                                    |" << std::endl;
    std::cout << "|  0. 返回上级菜单                              |" << std::endl;
    printMenuBorder();

    int choice = selectIntCheck(0, 4);
    return choice;
}

// 修改药品信息菜单
int MedicineModificationMenu()
{
    printMenuBorder();
    printMenuTitle("修改药品信息");
    printMenuBorder();
    std::cout << "|  1. 药品名称                                  |" << std::endl;
    std::cout << "|  2. 药品规格描述                              |" << std::endl;
    std::cout << "|  3. 药品进价                                  |" << std::endl;
    std::cout << "|  4. 药品售价                                  |" << std::endl;
    std::cout << "|  5. 药品库存量                                |" << std::endl;
    std::cout << "|  6. 药品安全库存阈值                          |" << std::endl;
    std::cout << "|  7. 药品状态                                  |" << std::endl;
    std::cout << "|  8. 药品生产日期                              |" << std::endl;
    std::cout << "|  9. 药品有效期限                              |" << std::endl;
    std::cout << "|  10. 药品备注                                 |" << std::endl;
    std::cout << "|  11. 生产厂家                                 |" << std::endl;
    std::cout << "|  0. 返回上级菜单                              |" << std::endl;
    printMenuBorder();

    int choice = selectIntCheck(0, 11);
    return choice;
}

// 管理员用户管理菜单
int adminUserManagementMenu()
{
    printMenuBorder();
    printMenuTitle("账户管理");
    printMenuBorder();
    std::cout << "|  1. 医生账户管理                              |" << std::endl;
    std::cout << "|  2. 护士账户管理                              |" << std::endl;
    std::cout << "|  3. 药剂师账户管理                            |" << std::endl;
    std::cout << "|  4. 患者账户管理                              |" << std::endl;
    std::cout << "|  0. 返回上级菜单                              |" << std::endl;
    printMenuBorder();

    int choice = selectIntCheck(0, 4);
    return choice;
}

// 管理员医生信息管理菜单
int adminDoctorManagementMenu()
{
    printMenuBorder();
    printMenuTitle("医生账户管理");
    printMenuBorder();
    std::cout << "|  1. 查看医生信息                              |" << std::endl;
    std::cout << "|  2. 修改医生信息                              |" << std::endl;
    std::cout << "|  3. 删除医生账户                              |" << std::endl;
    std::cout << "|  4. 添加医生账户                              |" << std::endl;
    std::cout << "|  0. 返回上级菜单                              |" << std::endl;
    printMenuBorder();

    int choice = selectIntCheck(0, 4);
    return choice;
}

// 管理员医生信息查看方式选择菜单
int adminDoctorViewMenu()
{
    printMenuBorder();
    printMenuTitle("医生信息查看");
    printMenuBorder();
    std::cout << "|  1. 查看所有医生信息                          |" << std::endl;
    std::cout << "|  2. 根据医生ID查看                            |" << std::endl;
    std::cout << "|  3. 根据医生姓名查看                          |" << std::endl;
    std::cout << "|  4. 根据在岗状态查看                          |" << std::endl;
    std::cout << "|  5. 根据职称查看                              |" << std::endl;
    std::cout << "|  0. 返回上级菜单                              |" << std::endl;
    printMenuBorder();

    int viewChoice = selectIntCheck(0, 5);
    return viewChoice;
}

// 医生职称选择菜单
int DoctorTitleMenu()
{
    printMenuBorder();
    printMenuTitle("医生职称选择");
    printMenuBorder();
    std::cout << "|  1. 实习医师                                  |" << std::endl;
    std::cout << "|  2. 住院医师                                  |" << std::endl;
    std::cout << "|  3. 主治医师                                  |" << std::endl;
    std::cout << "|  4. 副主任医师                                |" << std::endl;
    std::cout << "|  5. 主任医师                                  |" << std::endl;
    std::cout << "|  0. 返回上级菜单                              |" << std::endl;
    printMenuBorder();

    int choice = selectIntCheck(0, 5);
    return choice;
}

// 管理员医生信息修改菜单
int adminDoctorModificationMenu()
{
    printMenuBorder();
    printMenuTitle("修改医生信息");
    printMenuBorder();
    std::cout << "|  1. 医生姓名                                  |" << std::endl;
    std::cout << "|  2. 医生性别                                  |" << std::endl;
    std::cout << "|  3. 医生年龄                                  |" << std::endl;
    std::cout << "|  4. 医生所属科室                              |" << std::endl;
    std::cout << "|  5. 医生职称                                  |" << std::endl;
    std::cout << "|  6. 医生联系电话                              |" << std::endl;
    std::cout << "|  7. 医生邮箱地址                              |" << std::endl;
    std::cout << "|  8. 医生在岗状态                              |" << std::endl;
    std::cout << "|  9. 医生擅长方向                              |" << std::endl;
    std::cout << "|  10. 医生排班信息                             |" << std::endl;
    std::cout << "|  11. 累计接诊患者数量                         |" << std::endl;
    std::cout << "|  12. 累计完成的检查数量                       |" << std::endl;
    std::cout << "|  13. 累计开具住院证的数量                     |" << std::endl;
    std::cout << "|  0. 返回上级菜单                              |" << std::endl;
    printMenuBorder();

    int choice = selectIntCheck(0, 13);
    return choice;
}

// 管理员护士信息管理菜单
int adminNurseManagementMenu()
{
    printMenuBorder();
    printMenuTitle("护士账户管理");
    printMenuBorder();
    std::cout << "|  1. 查看护士信息                              |" << std::endl;
    std::cout << "|  2. 修改护士信息                              |" << std::endl;
    std::cout << "|  3. 删除护士账户                              |" << std::endl;
    std::cout << "|  4. 添加护士账户                              |" << std::endl;
    std::cout << "|  0. 返回上级菜单                              |" << std::endl;
    printMenuBorder();

    int choice = selectIntCheck(0, 4);
    return choice;
}

// 管理员护士信息查看方式选择菜单
int adminNurseViewMenu()
{
    printMenuBorder();
    printMenuTitle("护士信息查看");
    printMenuBorder();
    std::cout << "|  1. 查看所有护士信息                          |" << std::endl;
    std::cout << "|  2. 根据护士ID查看                            |" << std::endl;
    std::cout << "|  3. 根据护士姓名查看                          |" << std::endl;
    std::cout << "|  4. 根据在岗状态查看                          |" << std::endl;
    std::cout << "|  5. 根据职称查看                              |" << std::endl;
    std::cout << "|  0. 返回上级菜单                              |" << std::endl;
    printMenuBorder();

    int viewChoice = selectIntCheck(0, 5);
    return viewChoice;
}

// 护士职称选择菜单
int NurseTitleMenu()
{
    printMenuBorder();
    printMenuTitle("护士职称选择");
    printMenuBorder();
    std::cout << "|  1. 实习护士                                  |" << std::endl;
    std::cout << "|  2. 初级护士                                  |" << std::endl;
    std::cout << "|  3. 高级护士                                  |" << std::endl;
    std::cout << "|  4. 护士长                                    |" << std::endl;
    std::cout << "|  0. 返回上级菜单                              |" << std::endl;
    printMenuBorder();

    int choice = selectIntCheck(0, 4);
    return choice;
}

// 管理员护士信息修改菜单
int adminNurseModificationMenu()
{
    printMenuBorder();
    printMenuTitle("修改护士信息");
    printMenuBorder();
    std::cout << "|  1. 护士姓名                                  |" << std::endl;
    std::cout << "|  2. 护士性别                                  |" << std::endl;
    std::cout << "|  3. 护士年龄                                  |" << std::endl;
    std::cout << "|  4. 护士所属科室                              |" << std::endl;
    std::cout << "|  5. 护士职称                                  |" << std::endl;
    std::cout << "|  6. 护士联系电话                              |" << std::endl;
    std::cout << "|  7. 护士邮箱地址                              |" << std::endl;
    std::cout << "|  8. 护士在岗状态                              |" << std::endl;
    std::cout << "|  9. 护士排班信息                              |" << std::endl;
    std::cout << "|  10. 累计护理患者数量                         |" << std::endl;
    std::cout << "|  11. 床位管理次数                             |" << std::endl;
    std::cout << "|  0. 返回上级菜单                              |" << std::endl;
    printMenuBorder();

    int choice = selectIntCheck(0, 11);
    return choice;
}

// 管理员药剂师信息管理菜单
int adminPharmacistManagementMenu()
{
    printMenuBorder();
    printMenuTitle("药剂师账户管理");
    printMenuBorder();
    std::cout << "|  1. 查看药剂师信息                            |" << std::endl;
    std::cout << "|  2. 修改药剂师信息                            |" << std::endl;
    std::cout << "|  3. 删除药剂师账户                            |" << std::endl;
    std::cout << "|  4. 添加药剂师账户                            |" << std::endl;
    std::cout << "|  0. 返回上级菜单                              |" << std::endl;
    printMenuBorder();

    int choice = selectIntCheck(0, 4);
    return choice;
}

// 管理员药剂师信息查看方式选择菜单
int adminPharmacistViewMenu()
{
    printMenuBorder();
    printMenuTitle("药剂师信息查看");
    printMenuBorder();
    std::cout << "|  1. 查看所有药剂师信息                        |" << std::endl;
    std::cout << "|  2. 根据药剂师ID查看                          |" << std::endl;
    std::cout << "|  3. 根据药剂师姓名查看                        |" << std::endl;
    std::cout << "|  4. 根据在岗状态查看                          |" << std::endl;
    std::cout << "|  5. 根据职称查看                              |" << std::endl;
    std::cout << "|  0. 返回上级菜单                              |" << std::endl;
    printMenuBorder();

    int viewChoice = selectIntCheck(0, 5);
    return viewChoice;
}

// 药剂师职称选择菜单
int PharmacistTitleMenu()
{
    printMenuBorder();
    printMenuTitle("药剂师职称选择");
    printMenuBorder();
    std::cout << "|  1. 实习药剂师                                |" << std::endl;
    std::cout << "|  2. 初级药剂师                                |" << std::endl;
    std::cout << "|  3. 高级药剂师                                |" << std::endl;
    std::cout << "|  4. 主管药剂师                                |" << std::endl;
    std::cout << "|  0. 返回上级菜单                              |" << std::endl;
    printMenuBorder();

    int choice = selectIntCheck(0, 4);
    return choice;
}

// 管理员药剂师信息修改菜单
int adminPharmacistModificationMenu()
{
    printMenuBorder();
    printMenuTitle("修改药剂师信息");
    printMenuBorder();
    std::cout << "|  1. 药剂师姓名                                |" << std::endl;
    std::cout << "|  2. 药剂师性别                                |" << std::endl;
    std::cout << "|  3. 药剂师年龄                                |" << std::endl;
    std::cout << "|  4. 药剂师所属科室                            |" << std::endl;
    std::cout << "|  5. 药剂师职称                                |" << std::endl;
    std::cout << "|  6. 药剂师联系电话                            |" << std::endl;
    std::cout << "|  7. 药剂师邮箱地址                            |" << std::endl;
    std::cout << "|  8. 药剂师在岗状态                            |" << std::endl;
    std::cout << "|  9. 药剂师排班信息                            |" << std::endl;
    std::cout << "|  10. 累计审核用药记录数量                     |" << std::endl;
    std::cout << "|  11. 累计发药记录数量                         |" << std::endl;
    std::cout << "|  12. 库存管理次数                             |" << std::endl;
    std::cout << "|  0. 返回上级菜单                              |" << std::endl;
    printMenuBorder();

    int choice = selectIntCheck(0, 12);
    return choice;
}

// 管理员患者信息管理菜单
int adminPatientManagementMenu()
{
    printMenuBorder();
    printMenuTitle("患者账户管理");
    printMenuBorder();
    std::cout << "|  1. 查看患者信息                              |" << std::endl;
    std::cout << "|  2. 修改患者信息                              |" << std::endl;
    std::cout << "|  3. 删除患者账户                              |" << std::endl;
    std::cout << "|  4. 添加患者账户                              |" << std::endl;
    std::cout << "|  0. 返回上级菜单                              |" << std::endl;
    printMenuBorder();

    int choice = selectIntCheck(0, 4);
    return choice;
}

// 管理员患者信息查看方式选择菜单
int adminPatientViewMenu()
{
    printMenuBorder();
    printMenuTitle("患者信息查看");
    printMenuBorder();
    std::cout << "|  1. 查看所有患者信息                          |" << std::endl;
    std::cout << "|  2. 根据患者ID查看                            |" << std::endl;
    std::cout << "|  3. 根据患者姓名查看                          |" << std::endl;
    std::cout << "|  4. 根据患者身份证号查看                      |" << std::endl;
    std::cout << "|  5. 根据患者性别查看                          |" << std::endl;
    std::cout << "|  6. 根据患者年龄段查看                        |" << std::endl;
    std::cout << "|  7. 根据联系方式查看                          |" << std::endl;
    std::cout << "|  8. 根据住院状态查看                          |" << std::endl;
    std::cout << "|  9. 根据家庭住址查看                          |" << std::endl;
    std::cout << "|  10. 根据婚姻状况查看                         |" << std::endl;
    std::cout << "|  0. 返回上级菜单                              |" << std::endl;
    printMenuBorder();

    int viewChoice = selectIntCheck(0, 10);
    return viewChoice;
}

// 患者婚姻状况选择菜单
int PatientMaritalStatusMenu()
{
    printMenuBorder();
    printMenuTitle("患者婚姻状况");
    printMenuBorder();
    std::cout << "|  1. 未婚                                      |" << std::endl;
    std::cout << "|  2. 已婚                                      |" << std::endl;
    std::cout << "|  3. 离异                                      |" << std::endl;
    std::cout << "|  4. 丧偶                                      |" << std::endl;
    std::cout << "|  0. 返回上级菜单                              |" << std::endl;
    printMenuBorder();

    int choice = selectIntCheck(0, 4);
    return choice;
}

// 管理员患者信息修改菜单
int adminPatientModificationMenu()
{
    printMenuBorder();
    printMenuTitle("修改患者信息");
    printMenuBorder();
    std::cout << "|  1. 患者姓名                                  |" << std::endl;
    std::cout << "|  2. 患者性别                                  |" << std::endl;
    std::cout << "|  3. 患者年龄                                  |" << std::endl;
    std::cout << "|  4. 就诊科室                                  |" << std::endl;
    std::cout << "|  5. 患者身份证号                              |" << std::endl;
    std::cout << "|  6. 患者联系电话                              |" << std::endl;
    std::cout << "|  7. 患者邮箱地址                              |" << std::endl;
    std::cout << "|  8. 患者家庭住址                              |" << std::endl;
    std::cout << "|  9. 患者婚姻状况                              |" << std::endl;
    std::cout << "|  10. 紧急联系人信息                           |" << std::endl;
    std::cout << "|  11. 累计挂号次数                             |" << std::endl;
    std::cout << "|  12. 累计看诊次数                             |" << std::endl;
    std::cout << "|  13. 累计住院次数                             |" << std::endl;
    std::cout << "|  14. 累计用药次数                             |" << std::endl;
    std::cout << "|  0. 返回上级菜单                              |" << std::endl;
    printMenuBorder();

    int choice = selectIntCheck(0, 14);
    return choice;
}

// 管理员信息管理菜单
int adminAdminManagementMenu()
{
    printMenuBorder();
    printMenuTitle("管理员账户管理");
    printMenuBorder();
    std::cout << "|  1. 查看管理员信息                            |" << std::endl;
    std::cout << "|  2. 修改管理员信息                            |" << std::endl;
    std::cout << "|  3. 删除管理员账户                            |" << std::endl;
    std::cout << "|  4. 添加管理员账户                            |" << std::endl;
    std::cout << "|  0. 返回上级菜单                              |" << std::endl;
    printMenuBorder();

    int choice = selectIntCheck(0, 4);
    return choice;
}

// 管理员信息查看方式选择菜单
int adminAdminViewMenu()
{
    printMenuBorder();
    printMenuTitle("管理员信息查看");
    printMenuBorder();
    std::cout << "|  1. 查看所有管理员信息                        |" << std::endl;
    std::cout << "|  2. 根据管理员ID查看                          |" << std::endl;
    std::cout << "|  3. 根据管理员姓名查看                        |" << std::endl;
    std::cout << "|  4. 根据性别查看                              |" << std::endl;
    std::cout << "|  5. 根据年龄段查看                            |" << std::endl;
    std::cout << "|  6. 根据联系方式查看                          |" << std::endl;
    std::cout << "|  0. 返回上级菜单                              |" << std::endl;
    printMenuBorder();

    int viewChoice = selectIntCheck(0, 6);
    return viewChoice;
}

// 管理员信息修改菜单
int adminAdminModificationMenu()
{
    printMenuBorder();
    printMenuTitle("修改管理员信息");
    printMenuBorder();
    std::cout << "|  1. 管理员姓名                                |" << std::endl;
    std::cout << "|  2. 管理员性别                                |" << std::endl;
    std::cout << "|  3. 管理员年龄                                |" << std::endl;
    std::cout << "|  4. 管理员联系电话                            |" << std::endl;
    std::cout << "|  5. 管理员邮箱地址                            |" << std::endl;
    std::cout << "|  0. 返回上级菜单                              |" << std::endl;
    printMenuBorder();

    int choice = selectIntCheck(0, 5);
    return choice;
}

// 管理员个人信息管理菜单
int adminPersonalInfoManagementMenu()
{
    printMenuBorder();
    printMenuTitle("管理员个人信息管理");
    printMenuBorder();
    std::cout << "|  1. 查看个人信息                              |" << std::endl;
    std::cout << "|  2. 修改个人信息                              |" << std::endl;
    std::cout << "|  0. 返回上级菜单                              |" << std::endl;
    printMenuBorder();

    int choice = selectIntCheck(0, 2);
    return choice;
}

// 管理员个人信息查看菜单
int adminPersonalInfoViewMenu()
{
    printMenuBorder();
    printMenuTitle("查看个人信息");
    printMenuBorder();
    std::cout << "|  1. ID                                       |" << std::endl;
    std::cout << "|  2. 姓名                                      |" << std::endl;
    std::cout << "|  3. 性别                                      |" << std::endl;
    std::cout << "|  4. 年龄                                      |" << std::endl;
    std::cout << "|  5. 联系电话                                  |" << std::endl;
    std::cout << "|  6. 邮箱地址                                  |" << std::endl;
    std::cout << "|  7. 账号创建时间                              |" << std::endl;
    std::cout << "|  0. 返回上级菜单                              |" << std::endl;
    printMenuBorder();

    int viewChoice = selectIntCheck(0, 7);
    return viewChoice;
}

// 管理员个人信息修改菜单
int adminPersonalInfoModificationMenu()
{
    printMenuBorder();
    printMenuTitle("修改个人信息");
    printMenuBorder();
    std::cout << "|  1. 姓名                                      |" << std::endl;
    std::cout << "|  2. 性别                                      |" << std::endl;
    std::cout << "|  3. 年龄                                      |" << std::endl;
    std::cout << "|  4. 联系电话                                  |" << std::endl;
    std::cout << "|  5. 邮箱地址                                  |" << std::endl;
    std::cout << "|  6. 账号密码                                  |" << std::endl;
    std::cout << "|  0. 返回上级菜单                              |" << std::endl;
    printMenuBorder();

    int modificationChoice = selectIntCheck(0, 6);
    return modificationChoice;
}

// 医生管理菜单
int doctorMenu()
{
    printMenuBorder();
    printMenuTitle("医生功能界面");
    printMenuBorder();
    std::cout << "|  1. 管理挂号记录                              |" << std::endl;
    std::cout << "|  2. 管理看诊记录                              |" << std::endl;
    std::cout << "|  3. 管理检查记录                              |" << std::endl;
    std::cout << "|  4. 个人信息管理                              |" << std::endl;
    std::cout << "|  0. 退出登录                                  |" << std::endl;
    printMenuBorder();

    int choice = selectIntCheck(0, 4);
    return choice;
}

// 医生挂号记录管理菜单
int doctorRegistrationManagementMenu()
{
    printMenuBorder();
    printMenuTitle("挂号记录管理");
    printMenuBorder();
    std::cout << "|  1. 查看挂号记录                              |" << std::endl;
    std::cout << "|  2. 修改挂号记录状态                          |" << std::endl;
    std::cout << "|  3. 删除挂号记录                              |" << std::endl;
    std::cout << "|  4. 添加挂号记录                              |" << std::endl;
    std::cout << "|  0. 返回上级菜单                              |" << std::endl;
    printMenuBorder();

    int choice = selectIntCheck(0, 4);
    return choice;
}

// 医生挂号记录查看方式选择菜单
int doctorRegistrationViewMenu()
{
    printMenuBorder();
    printMenuTitle("挂号记录查看");
    printMenuBorder();
    std::cout << "|  1. 查看该医生的所有挂号记录                  |" << std::endl;
    std::cout << "|  2. 根据患者ID查看                            |" << std::endl;
    std::cout << "|  3. 根据挂号记录ID查看                        |" << std::endl;
    std::cout << "|  4. 根据时间范围查看                          |" << std::endl;
    std::cout << "|  5. 根据挂号状态查看                          |" << std::endl;
    std::cout << "|  0. 返回上级菜单                              |" << std::endl;
    printMenuBorder();

    int viewChoice = selectIntCheck(0, 5);
    return viewChoice;
}

// 医生看诊记录管理菜单
int doctorConsultationManagementMenu()
{
    printMenuBorder();
    printMenuTitle("看诊记录管理");
    printMenuBorder();
    std::cout << "|  1. 查看看诊记录                              |" << std::endl;
    std::cout << "|  2. 修改看诊记录                              |" << std::endl;
    std::cout << "|  3. 删除看诊记录                              |" << std::endl;
    std::cout << "|  4. 添加看诊记录                              |" << std::endl;
    std::cout << "|  0. 返回上级菜单                              |" << std::endl;
    printMenuBorder();

    int choice = selectIntCheck(0, 4);
    return choice;
}

// 医生看诊记录查看方式选择菜单
int doctorConsultationViewMenu()
{
    printMenuBorder();
    printMenuTitle("看诊记录查看");
    printMenuBorder();
    std::cout << "|  1. 查看该医生的所有看诊记录                  |" << std::endl;
    std::cout << "|  2. 根据患者ID查看                            |" << std::endl;
    std::cout << "|  3. 根据看诊记录ID查看                        |" << std::endl;
    std::cout << "|  4. 根据时间范围查看                          |" << std::endl;
    std::cout << "|  5. 根据看诊状态查看                          |" << std::endl;
    std::cout << "|  0. 返回上级菜单                              |" << std::endl;
    printMenuBorder();

    int viewChoice = selectIntCheck(0, 5);
    return viewChoice;
}

// 医生看诊记录修改菜单
int doctorConsultationModificationMenu()
{
    printMenuBorder();
    printMenuTitle("修改看诊记录");
    printMenuBorder();
    std::cout << "|  1. 看诊状态                                  |" << std::endl;
    std::cout << "|  2. 主诉                                      |" << std::endl;
    std::cout << "|  3. 现病史                                    |" << std::endl;
    std::cout << "|  4. 既往史                                    |" << std::endl;
    std::cout << "|  5. 家族史                                    |" << std::endl;
    std::cout << "|  6. 初步诊断                                  |" << std::endl;
    std::cout << "|  7. 添加检查项目                              |" << std::endl;
    std::cout << "|  8. 添加处方信息                              |" << std::endl;
    std::cout << "|  9. 开具住院证                                |" << std::endl;
    std::cout << "|  0. 返回上级菜单                              |" << std::endl;
    printMenuBorder();

    int choice = selectIntCheck(0, 9);
    return choice;
}

// 医生检查记录管理菜单
int doctorExaminationManagementMenu()
{
    printMenuBorder();
    printMenuTitle("检查记录管理");
    printMenuBorder();
    std::cout << "|  1. 查看检查记录                              |" << std::endl;
    std::cout << "|  2. 修改检查记录                              |" << std::endl;
    std::cout << "|  3. 删除检查记录                              |" << std::endl;
    std::cout << "|  4. 添加检查记录                              |" << std::endl;
    std::cout << "|  0. 返回上级菜单                              |" << std::endl;
    printMenuBorder();

    int choice = selectIntCheck(0, 4);
    return choice;
}

// 医生检查记录查看方式选择菜单
int doctorExaminationViewMenu()
{
    printMenuBorder();
    printMenuTitle("检查记录查看");
    printMenuBorder();
    std::cout << "|  1. 查看该医生的所有检查记录                  |" << std::endl;
    std::cout << "|  2. 根据患者ID查看                            |" << std::endl;
    std::cout << "|  3. 根据看诊记录ID查看                        |" << std::endl;
    std::cout << "|  4. 根据检查记录ID查看                        |" << std::endl;
    std::cout << "|  5. 根据时间范围查看                          |" << std::endl;
    std::cout << "|  6. 根据检查项目查看                          |" << std::endl;
    std::cout << "|  7. 根据检查记录状态查看                      |" << std::endl;
    std::cout << "|  0. 返回上级菜单                              |" << std::endl;
    printMenuBorder();

    int viewChoice = selectIntCheck(0, 7);
    return viewChoice;
}

// 医生检查记录修改菜单
int doctorExaminationModificationMenu()
{
    printMenuBorder();
    printMenuTitle("修改检查记录");
    printMenuBorder();
    std::cout << "|  1. 检查记录状态                              |" << std::endl;
    std::cout << "|  2. 检查项目                                  |" << std::endl;
    std::cout << "|  3. 检查结果报告                              |" << std::endl;
    std::cout << "|  4. 检查结果报告摘要                          |" << std::endl;
    std::cout << "|  5. 修改检查报告附件文件                      |" << std::endl;
    std::cout << "|  6. 添加检查记录备注信息                      |" << std::endl;
    std::cout << "|  0. 返回上级菜单                              |" << std::endl;
    printMenuBorder();

    int choice = selectIntCheck(0, 6);
    return choice;
}

// 医生个人信息管理菜单
int doctorPersonalInfoManagementMenu()
{
    printMenuBorder();
    printMenuTitle("个人信息管理");
    printMenuBorder();
    std::cout << "|  1. 查看个人信息                              |" << std::endl;
    std::cout << "|  2. 修改个人信息                              |" << std::endl;
    std::cout << "|  0. 返回上级菜单                              |" << std::endl;
    printMenuBorder();

    int choice = selectIntCheck(0, 2);
    return choice;
}

// 医生个人信息查看选择菜单
int doctorPersonalInfoViewMenu()
{
    printMenuBorder();
    printMenuTitle("查看个人信息");
    printMenuBorder();
    std::cout << "|  1. ID                                       |" << std::endl;
    std::cout << "|  2. 姓名                                      |" << std::endl;
    std::cout << "|  3. 性别                                      |" << std::endl;
    std::cout << "|  4. 年龄                                      |" << std::endl;
    std::cout << "|  5. 所属科室                                  |" << std::endl;
    std::cout << "|  6. 职称                                      |" << std::endl;
    std::cout << "|  7. 联系电话                                  |" << std::endl;
    std::cout << "|  8. 邮箱地址                                  |" << std::endl;
    std::cout << "|  9. 在岗状态                                  |" << std::endl;
    std::cout << "|  10. 擅长方向                                 |" << std::endl;
    std::cout << "|  11. 排班信息                                 |" << std::endl;
    std::cout << "|  12. 累计接诊患者数量                         |" << std::endl;
    std::cout << "|  13. 累计完成的检查数量                       |" << std::endl;
    std::cout << "|  14. 累计开具住院证的数量                     |" << std::endl;
    std::cout << "|  15. 账户创建时间                             |" << std::endl;
    std::cout << "|  0. 返回上级菜单                              |" << std::endl;
    printMenuBorder();

    int viewChoice = selectIntCheck(0, 15);
    return viewChoice;
}

// 医生个人信息修改菜单
int doctorPersonalInfoModificationMenu()
{
    printMenuBorder();
    printMenuTitle("修改个人信息");
    printMenuBorder();
    std::cout << "|  1. 姓名                                      |" << std::endl;
    std::cout << "|  2. 性别                                      |" << std::endl;
    std::cout << "|  3. 年龄                                      |" << std::endl;
    std::cout << "|  4. 所属科室                                  |" << std::endl;
    std::cout << "|  5. 职称                                      |" << std::endl;
    std::cout << "|  6. 联系电话                                  |" << std::endl;
    std::cout << "|  7. 邮箱地址                                  |" << std::endl;
    std::cout << "|  8. 在岗状态                                  |" << std::endl;
    std::cout << "|  9. 擅长方向                                  |" << std::endl;
    std::cout << "|  10. 排班信息                                 |" << std::endl;
    std::cout << "|  11. 账号密码                                 |" << std::endl;
    std::cout << "|  0. 返回上级菜单                              |" << std::endl;
    printMenuBorder();

    int choice = selectIntCheck(0, 11);
    return choice;
}

// 患者管理菜单
int patientMenu()
{
    printMenuBorder();
    printMenuTitle("患者功能界面");
    printMenuBorder();
    std::cout << "|  1. 挂号信息管理                              |" << std::endl;
    std::cout << "|  2. 看诊信息管理                              |" << std::endl;
    std::cout << "|  3. 检查信息管理                              |" << std::endl;
    std::cout << "|  4. 用药信息管理                              |" << std::endl;
    std::cout << "|  5. 住院信息管理                              |" << std::endl;
    std::cout << "|  6. 个人信息管理                              |" << std::endl;
    std::cout << "|  0. 退出登录                                  |" << std::endl;
    printMenuBorder();

    int choice = selectIntCheck(0, 6);
    return choice;
}

// 患者挂号信息管理菜单
int patientRegistrationManagementMenu()
{
    printMenuBorder();
    printMenuTitle("挂号信息管理");
    printMenuBorder();
    std::cout << "|  1. 查看挂号信息                              |" << std::endl;
    std::cout << "|  2. 添加挂号信息                              |" << std::endl;
    std::cout << "|  3. 撤回挂号申请                              |" << std::endl;
    std::cout << "|  4. 缴纳挂号费用                              |" << std::endl;
    std::cout << "|  0. 返回上级菜单                              |" << std::endl;
    printMenuBorder();

    int choice = selectIntCheck(0, 4);
    return choice;
}

// 患者挂号信息查看方式选择菜单
int patientRegistrationViewMenu()
{
    printMenuBorder();
    printMenuTitle("挂号信息查看");
    printMenuBorder();
    std::cout << "|  1. 查看所有挂号信息                          |" << std::endl;
    std::cout << "|  2. 根据挂号记录ID查看                        |" << std::endl;
    std::cout << "|  3. 根据医生ID查看                            |" << std::endl;
    std::cout << "|  4. 根据挂号状态查看                          |" << std::endl;
    std::cout << "|  5. 根据挂号时间范围查看                      |" << std::endl;
    std::cout << "|  6. 根据挂号科室查看                          |" << std::endl;
    std::cout << "|  0. 返回上级菜单                              |" << std::endl;
    printMenuBorder();

    int viewChoice = selectIntCheck(0, 6);
    return viewChoice;
}

// 患者看诊信息管理菜单
int patientConsultationManagementMenu()
{
    printMenuBorder();
    printMenuTitle("看诊信息管理");
    printMenuBorder();
    std::cout << "|  1. 查看看诊信息                              |" << std::endl;
    std::cout << "|  0. 返回上级菜单                              |" << std::endl;
    printMenuBorder();

    int choice = selectIntCheck(0, 1);
    return choice;
}

// 患者看诊信息查看方式选择菜单
int patientConsultationViewMenu()
{
    printMenuBorder();
    printMenuTitle("看诊信息查看");
    printMenuBorder();
    std::cout << "|  1. 查看所有看诊信息                          |" << std::endl;
    std::cout << "|  2. 根据看诊记录ID查看                        |" << std::endl;
    std::cout << "|  3. 根据医生ID查看                            |" << std::endl;
    std::cout << "|  4. 根据看诊状态查看                          |" << std::endl;
    std::cout << "|  5. 根据看诊时间范围查看                      |" << std::endl;
    std::cout << "|  6. 根据主诉内容关键词查看                    |" << std::endl;
    std::cout << "|  7. 根据看诊科室查看                          |" << std::endl;
    std::cout << "|  0. 返回上级菜单                              |" << std::endl;
    printMenuBorder();

    int viewChoice = selectIntCheck(0, 7);
    return viewChoice;
}

// 患者检查信息管理菜单
int patientExaminationManagementMenu()
{
    printMenuBorder();
    printMenuTitle("检查信息管理");
    printMenuBorder();
    std::cout << "|  1. 查看检查信息                              |" << std::endl;
    std::cout << "|  2. 缴纳检查费用                              |" << std::endl;
    std::cout << "|  0. 返回上级菜单                              |" << std::endl;
    printMenuBorder();

    int choice = selectIntCheck(0, 2);
    return choice;
}

// 患者检查信息查看方式选择菜单
int patientExaminationViewMenu()
{
    printMenuBorder();
    printMenuTitle("检查信息查看");
    printMenuBorder();
    std::cout << "|  1. 查看所有检查信息                          |" << std::endl;
    std::cout << "|  2. 根据检查记录ID查看                        |" << std::endl;
    std::cout << "|  3. 根据看诊记录ID查看                        |" << std::endl;
    std::cout << "|  4. 根据检查项目查看                          |" << std::endl;
    std::cout << "|  5. 根据检查状态查看                          |" << std::endl;
    std::cout << "|  6. 根据检查时间范围查看                      |" << std::endl;
    std::cout << "|  7. 根据检查科室查看                          |" << std::endl;
    std::cout << "|  0. 返回上级菜单                              |" << std::endl;
    printMenuBorder();

    int viewChoice = selectIntCheck(0, 7);
    return viewChoice;
}

// 患者用药信息管理菜单
int patientMedicationManagementMenu()
{
    printMenuBorder();
    printMenuTitle("用药信息管理");
    printMenuBorder();
    std::cout << "|  1. 查看用药信息                              |" << std::endl;
    std::cout << "|  2. 缴纳药品费用                              |" << std::endl;
    std::cout << "|  0. 返回上级菜单                              |" << std::endl;
    printMenuBorder();

    int choice = selectIntCheck(0, 2);
    return choice;
}

// 患者用药信息查看方式选择菜单
int patientMedicationViewMenu()
{
    printMenuBorder();
    printMenuTitle("用药信息查看");
    printMenuBorder();
    std::cout << "|  1. 查看所有用药信息                          |" << std::endl;
    std::cout << "|  2. 根据用药记录ID查看                        |" << std::endl;
    std::cout << "|  3. 根据看诊记录ID查看                        |" << std::endl;
    std::cout << "|  4. 根据药品名称查看                          |" << std::endl;
    std::cout << "|  5. 根据用药状态查看                          |" << std::endl;
    std::cout << "|  6. 根据用药时间范围查看                      |" << std::endl;
    std::cout << "|  7. 根据用药科室查看                          |" << std::endl;
    std::cout << "|  0. 返回上级菜单                              |" << std::endl;
    printMenuBorder();

    int viewChoice = selectIntCheck(0, 7);
    return viewChoice;
}

// 患者住院信息管理菜单
int patientHospitalizationManagementMenu()
{
    printMenuBorder();
    printMenuTitle("住院信息管理");
    printMenuBorder();
    std::cout << "|  1. 查看住院信息                              |" << std::endl;
    std::cout << "|  2. 申请出院                                  |" << std::endl;
    std::cout << "|  3. 缴纳住院费用                              |" << std::endl;
    std::cout << "|  0. 返回上级菜单                              |" << std::endl;
    printMenuBorder();

    int choice = selectIntCheck(0, 3);
    return choice;
}

// 患者住院信息查看方式选择菜单
int patientHospitalizationViewMenu()
{
    printMenuBorder();
    printMenuTitle("住院信息查看");
    printMenuBorder();
    std::cout << "|  1. 查看所有住院信息                          |" << std::endl;
    std::cout << "|  2. 根据住院记录ID查看                        |" << std::endl;
    std::cout << "|  3. 根据入院时间范围查看                      |" << std::endl;
    std::cout << "|  4. 根据出院时间范围查看                      |" << std::endl;
    std::cout << "|  5. 根据住院状态查看                          |" << std::endl;
    std::cout << "|  6. 根据住院科室查看                          |" << std::endl;
    std::cout << "|  7. 根据病房类型查看                          |" << std::endl;
    std::cout << "|  8. 根据床位号查看                            |" << std::endl;
    std::cout << "|  9. 根据主治医生ID查看                        |" << std::endl;
    std::cout << "|  10. 根据看诊记录ID查看                       |" << std::endl;
    std::cout << "|  0. 返回上级菜单                              |" << std::endl;
    printMenuBorder();

    int viewChoice = selectIntCheck(0, 10);
    return viewChoice;
}

// 患者个人信息管理菜单
int patientPersonalInfoManagementMenu()
{
    printMenuBorder();
    printMenuTitle("个人信息管理");
    printMenuBorder();
    std::cout << "|  1. 查看个人信息                              |" << std::endl;
    std::cout << "|  2. 修改个人信息                              |" << std::endl;
    std::cout << "|  3. 充值账户余额                              |" << std::endl;
    std::cout << "|  0. 返回上级菜单                              |" << std::endl;
    printMenuBorder();

    int choice = selectIntCheck(0, 3);
    return choice;
}

// 患者个人信息查看选择菜单
int patientPersonalInfoViewMenu()
{
    printMenuBorder();
    printMenuTitle("查看个人信息");
    printMenuBorder();
    std::cout << "|  1. ID                                       |" << std::endl;
    std::cout << "|  2. 姓名                                      |" << std::endl;
    std::cout << "|  3. 性别                                      |" << std::endl;
    std::cout << "|  4. 年龄                                      |" << std::endl;
    std::cout << "|  5. 身份证号                                  |" << std::endl;
    std::cout << "|  6. 联系电话                                  |" << std::endl;
    std::cout << "|  7. 邮箱地址                                  |" << std::endl;
    std::cout << "|  8. 家庭住址                                  |" << std::endl;
    std::cout << "|  9. 紧急联系人姓名                            |" << std::endl;
    std::cout << "|  10. 紧急联系人电话                           |" << std::endl;
    std::cout << "|  11. 过敏史                                   |" << std::endl;
    std::cout << "|  12. 既往病史                                 |" << std::endl;
    std::cout << "|  13. 婚姻状况                                 |" << std::endl;
    std::cout << "|  14. 账户余额                                 |" << std::endl;
    std::cout << "|  15. 是否住院中                               |" << std::endl;
    std::cout << "|  16. 累计挂号次数                             |" << std::endl;
    std::cout << "|  17. 累计就诊次数                             |" << std::endl;
    std::cout << "|  18. 累计住院次数                             |" << std::endl;
    std::cout << "|  19. 累计用药次数                             |" << std::endl;
    std::cout << "|  20. 账户创建时间                             |" << std::endl;
    std::cout << "|  0. 返回上级菜单                              |" << std::endl;
    printMenuBorder();

    int viewChoice = selectIntCheck(0, 20);
    return viewChoice;
}

// 患者个人信息修改菜单
int patientPersonalInfoModificationMenu()
{
    printMenuBorder();
    printMenuTitle("修改个人信息");
    printMenuBorder();
    std::cout << "|  1. 姓名                                      |" << std::endl;
    std::cout << "|  2. 性别                                      |" << std::endl;
    std::cout << "|  3. 年龄                                      |" << std::endl;
    std::cout << "|  4. 身份证号                                  |" << std::endl;
    std::cout << "|  5. 联系电话                                  |" << std::endl;
    std::cout << "|  6. 邮箱地址                                  |" << std::endl;
    std::cout << "|  7. 家庭住址                                  |" << std::endl;
    std::cout << "|  8. 紧急联系人姓名                            |" << std::endl;
    std::cout << "|  9. 紧急联系人电话                            |" << std::endl;
    std::cout << "|  10. 过敏史                                   |" << std::endl;
    std::cout << "|  11. 既往病史                                 |" << std::endl;
    std::cout << "|  12. 婚姻状况                                 |" << std::endl;
    std::cout << "|  13. 账号密码                                 |" << std::endl;
    std::cout << "|  0. 返回上级菜单                              |" << std::endl;
    printMenuBorder();

    int choice = selectIntCheck(0, 13);
    return choice;
}

// 药剂师管理菜单
int pharmacistMenu()
{
    printMenuBorder();
    printMenuTitle("药剂师功能界面");
    printMenuBorder();
    std::cout << "|  1. 管理用药信息                              |" << std::endl;
    std::cout << "|  2. 管理药品信息                              |" << std::endl;
    std::cout << "|  3. 个人信息管理                              |" << std::endl;
    std::cout << "|  0. 退出登录                                  |" << std::endl;
    printMenuBorder();

    int choice = selectIntCheck(0, 3);
    return choice;
}

// 药剂师用药信息管理菜单
int pharmacistMedicationRecordManagementMenu()
{
    printMenuBorder();
    printMenuTitle("用药信息管理");
    printMenuBorder();
    std::cout << "|  1. 查看用药信息                              |" << std::endl;
    std::cout << "|  2. 修改用药信息                              |" << std::endl;
    std::cout << "|  3. 发药                                      |" << std::endl;
    std::cout << "|  4. 删除用药记录                              |" << std::endl;
    std::cout << "|  5. 添加用药记录                              |" << std::endl;
    std::cout << "|  0. 返回上级菜单                              |" << std::endl;
    printMenuBorder();

    int choice = selectIntCheck(0, 5);
    return choice;
}

// 药剂师用药信息查看方式选择菜单
int pharmacistMedicationRecordViewMenu()
{
    printMenuBorder();
    printMenuTitle("用药信息查看");
    printMenuBorder();
    std::cout << "|  1. 查看所有用药信息                          |" << std::endl;
    std::cout << "|  2. 根据用药记录ID查看                        |" << std::endl;
    std::cout << "|  3. 根据看诊记录ID查看                        |" << std::endl;
    std::cout << "|  4. 根据患者ID查看                            |" << std::endl;
    std::cout << "|  5. 根据审核状态查看                          |" << std::endl;
    std::cout << "|  6. 根据用药状态查看                          |" << std::endl;
    std::cout << "|  7. 根据用药时间范围查看                      |" << std::endl;
    std::cout << "|  8. 根据药品名称查看                          |" << std::endl;
    std::cout << "|  0. 返回上级菜单                              |" << std::endl;
    printMenuBorder();

    int viewChoice = selectIntCheck(0, 8);
    return viewChoice;
}

// 药剂师用药信息修改菜单
int pharmacistMedicationRecordModificationMenu()
{
    printMenuBorder();
    printMenuTitle("修改用药信息");
    printMenuBorder();
    std::cout << "|  1. 用药状态                                  |" << std::endl;
    std::cout << "|  2. 审核状态                                  |" << std::endl;
    std::cout << "|  3. 管理药品明细                              |" << std::endl;
    std::cout << "|  4. 修改用药记录备注信息                      |" << std::endl;
    std::cout << "|  0. 返回上级菜单                              |" << std::endl;
    printMenuBorder();

    int choice = selectIntCheck(0, 4);
    return choice;
}

// 药剂师药品信息管理菜单
int pharmacistMedicineManagementMenu()
{
    printMenuBorder();
    printMenuTitle("药品信息管理");
    printMenuBorder();
    std::cout << "|  1. 查看药品信息                              |" << std::endl;
    std::cout << "|  2. 修改药品信息                              |" << std::endl;
    std::cout << "|  3. 删除药品信息                              |" << std::endl;
    std::cout << "|  4. 添加药品信息                              |" << std::endl;
    std::cout << "|  5. 管理药品库存                              |" << std::endl;
    std::cout << "|  0. 返回上级菜单                              |" << std::endl;
    printMenuBorder();

    int choice = selectIntCheck(0, 5);
    return choice;
}

// 药剂师药品信息查看方式选择菜单
int pharmacistMedicineViewMenu()
{
    printMenuBorder();
    printMenuTitle("药品信息查看");
    printMenuBorder();
    std::cout << "|  1. 查看所有药品信息                          |" << std::endl;
    std::cout << "|  2. 根据药品ID查看                            |" << std::endl;
    std::cout << "|  3. 根据药品名称查看                          |" << std::endl;
    std::cout << "|  4. 根据科室查看                              |" << std::endl;
    std::cout << "|  5. 根据药品状态查看                          |" << std::endl;
    std::cout << "|  0. 返回上级菜单                              |" << std::endl;
    printMenuBorder();

    int viewChoice = selectIntCheck(0, 5);
    return viewChoice;
}

// 药剂师药品信息修改菜单
int pharmacistMedicineModificationMenu()
{
    printMenuBorder();
    printMenuTitle("修改药品信息");
    printMenuBorder();
    std::cout << "|  1. 药品名称                                  |" << std::endl;
    std::cout << "|  2. 药品规格                                  |" << std::endl;
    std::cout << "|  3. 药品生产厂家                              |" << std::endl;
    std::cout << "|  4. 药品进价                                  |" << std::endl;
    std::cout << "|  5. 药品售价                                  |" << std::endl;
    std::cout << "|  6. 安全库存阈值                              |" << std::endl;
    std::cout << "|  7. 生产日期                                  |" << std::endl;
    std::cout << "|  8. 有效期                                    |" << std::endl;
    std::cout << "|  9. 科室                                      |" << std::endl;
    std::cout << "|  10. 药品状态                                 |" << std::endl;
    std::cout << "|  11. 修改药品备注信息                         |" << std::endl;
    std::cout << "|  0. 返回上级菜单                              |" << std::endl;
    printMenuBorder();

    int choice = selectIntCheck(0, 11);
    return choice;
}

// 药剂师个人信息管理菜单
int pharmacistPersonalInfoManagementMenu()
{
    printMenuBorder();
    printMenuTitle("个人信息管理");
    printMenuBorder();
    std::cout << "|  1. 查看个人信息                              |" << std::endl;
    std::cout << "|  2. 修改个人信息                              |" << std::endl;
    std::cout << "|  0. 返回上级菜单                              |" << std::endl;
    printMenuBorder();

    int choice = selectIntCheck(0, 2);
    return choice;
}

// 药剂师个人信息查看选择菜单
int pharmacistPersonalInfoViewMenu()
{
    printMenuBorder();
    printMenuTitle("查看个人信息");
    printMenuBorder();
    std::cout << "|  1. ID                                       |" << std::endl;
    std::cout << "|  2. 姓名                                      |" << std::endl;
    std::cout << "|  3. 性别                                      |" << std::endl;
    std::cout << "|  4. 年龄                                      |" << std::endl;
    std::cout << "|  5. 所属科室                                  |" << std::endl;
    std::cout << "|  6. 职称                                      |" << std::endl;
    std::cout << "|  7. 联系电话                                  |" << std::endl;
    std::cout << "|  8. 邮箱地址                                  |" << std::endl;
    std::cout << "|  9. 在岗状态                                  |" << std::endl;
    std::cout << "|  10. 排班信息                                 |" << std::endl;
    std::cout << "|  11. 累计审核用药记录数量                     |" << std::endl;
    std::cout << "|  12. 累计发药数量                             |" << std::endl;
    std::cout << "|  13. 累计管理药品库存次数                     |" << std::endl;
    std::cout << "|  14. 账户创建时间                             |" << std::endl;
    std::cout << "|  0. 返回上级菜单                              |" << std::endl;
    printMenuBorder();

    int viewChoice = selectIntCheck(0, 14);
    return viewChoice;
}

// 药剂师个人信息修改菜单
int pharmacistPersonalInfoModificationMenu()
{
    printMenuBorder();
    printMenuTitle("修改个人信息");
    printMenuBorder();
    std::cout << "|  1. 姓名                                      |" << std::endl;
    std::cout << "|  2. 性别                                      |" << std::endl;
    std::cout << "|  3. 年龄                                      |" << std::endl;
    std::cout << "|  4. 所属科室                                  |" << std::endl;
    std::cout << "|  5. 职称                                      |" << std::endl;
    std::cout << "|  6. 联系电话                                  |" << std::endl;
    std::cout << "|  7. 邮箱地址                                  |" << std::endl;
    std::cout << "|  8. 在岗状态                                  |" << std::endl;
    std::cout << "|  9. 排班信息                                  |" << std::endl;
    std::cout << "|  10. 账号密码                                 |" << std::endl;
    std::cout << "|  0. 返回上级菜单                              |" << std::endl;
    printMenuBorder();

    int choice = selectIntCheck(0, 10);
    return choice;
}

// 护士管理菜单
int nurseMenu()
{
    printMenuBorder();
    printMenuTitle("护士功能界面");
    printMenuBorder();
    std::cout << "|  1. 管理住院信息                              |" << std::endl;
    std::cout << "|  2. 管理检查信息                              |" << std::endl;
    std::cout << "|  3. 管理床位信息                              |" << std::endl;
    std::cout << "|  4. 个人信息管理                              |" << std::endl;
    std::cout << "|  0. 退出登录                                  |" << std::endl;
    printMenuBorder();

    int choice = selectIntCheck(0, 4);
    return choice;
}

// 护士检查信息管理菜单
int nurseExaminationManagementMenu()
{
    printMenuBorder();
    printMenuTitle("检查信息管理");
    printMenuBorder();
    std::cout << "|  1. 查看检查信息                              |" << std::endl;
    std::cout << "|  2. 修改检查信息                              |" << std::endl;
    std::cout << "|  3. 删除检查信息                              |" << std::endl;
    std::cout << "|  0. 返回上级菜单                              |" << std::endl;
    printMenuBorder();

    int choice = selectIntCheck(0, 3);
    return choice;
}

// 护士检查信息查看方式选择菜单
int nurseExaminationViewMenu()
{
    printMenuBorder();
    printMenuTitle("检查记录查看");
    printMenuBorder();
    std::cout << "|  1. 查看本科室全部检查记录                    |" << std::endl;
    std::cout << "|  2. 按患者ID查询                              |" << std::endl;
    std::cout << "|  3. 按看诊ID查询                              |" << std::endl;
    std::cout << "|  4. 按检查ID查询                              |" << std::endl;
    std::cout << "|  5. 按时间范围查询                            |" << std::endl;
    std::cout << "|  6. 按检查项目查询                            |" << std::endl;
    std::cout << "|  7. 按状态查询                                |" << std::endl;
    std::cout << "|  0. 返回上一级                                |" << std::endl;
    printMenuBorder();

    int viewChoice = selectIntCheck(0, 7);
    return viewChoice;
}

// 护士检查信息修改菜单
int nurseExaminationModificationMenu()
{
    printMenuBorder();
    printMenuTitle("修改检查记录");
    printMenuBorder();
    std::cout << "|  1. 修改检查状态                              |" << std::endl;
    std::cout << "|  2. 录入/修改检查结果                         |" << std::endl;
    std::cout << "|  3. 修改报告摘要                              |" << std::endl;
    std::cout << "|  4. 管理附件                                  |" << std::endl;
    std::cout << "|  5. 修改备注                                  |" << std::endl;
    std::cout << "|  0. 返回上一级                                |" << std::endl;
    printMenuBorder();

    int choice = selectIntCheck(0, 5);
    return choice;
}


// 护士住院信息管理菜单
int nurseHospitalizationManagementMenu()
{
    printMenuBorder();
    printMenuTitle("住院信息管理");
    printMenuBorder();
    std::cout << "|  1. 查看住院信息                              |" << std::endl;
    std::cout << "|  2. 修改住院信息                              |" << std::endl;
    std::cout << "|  3. 删除住院信息                              |" << std::endl;
    std::cout << "|  4. 添加住院信息                              |" << std::endl;
    std::cout << "|  0. 返回上级菜单                              |" << std::endl;
    printMenuBorder();

    int choice = selectIntCheck(0, 4);
    return choice;
}

// 护士住院信息查看方式选择菜单
int nurseHospitalizationViewMenu()
{
    printMenuBorder();
    printMenuTitle("住院信息查看");
    printMenuBorder();
    std::cout << "|  1. 查看所有住院信息                          |" << std::endl;
    std::cout << "|  2. 根据住院记录ID查看                        |" << std::endl;
    std::cout << "|  3. 根据患者ID查看                            |" << std::endl;
    std::cout << "|  4. 根据入院时间范围查看                      |" << std::endl;
    std::cout << "|  5. 根据出院时间范围查看                      |" << std::endl;
    std::cout << "|  6. 根据住院状态查看                          |" << std::endl;
    std::cout << "|  7. 根据病房类型查看                          |" << std::endl;
    std::cout << "|  8. 根据床位号查看                            |" << std::endl;
    std::cout << "|  9. 根据主治医生ID查看                        |" << std::endl;
    std::cout << "|  10. 根据看诊记录ID查看                       |" << std::endl;
    std::cout << "|  0. 返回上级菜单                              |" << std::endl;
    printMenuBorder();

    int viewChoice = selectIntCheck(0, 10);
    return viewChoice;
}

// 护士住院信息修改菜单
int nurseHospitalizationModificationMenu()
{
    printMenuBorder();
    printMenuTitle("修改住院信息");
    printMenuBorder();
    std::cout << "|  1. 住院状态                                  |" << std::endl;
    std::cout << "|  2. 病房类型                                  |" << std::endl;
    std::cout << "|  3. 分配护士                                  |" << std::endl;
    std::cout << "|  4. 分配床位                                  |" << std::endl;
    std::cout << "|  5. 调整床位                                  |" << std::endl;
    std::cout << "|  6. 办理出院                                  |" << std::endl;
    std::cout << "|  7. 设置押金金额                              |" << std::endl;
    std::cout << "|  0. 返回上级菜单                              |" << std::endl;
    printMenuBorder();

    int choice = selectIntCheck(0, 7);
    return choice;
}

// 护士床位信息管理菜单
int nurseBedManagementMenu()
{
    printMenuBorder();
    printMenuTitle("床位信息管理");
    printMenuBorder();
    std::cout << "|  1. 查看床位信息                              |" << std::endl;
    std::cout << "|  2. 修改床位信息                              |" << std::endl;
    std::cout << "|  3. 删除床位信息                              |" << std::endl;
    std::cout << "|  4. 新增床位信息                              |" << std::endl;
    std::cout << "|  0. 返回上一级                                |" << std::endl;
    printMenuBorder();

    int choice = selectIntCheck(0, 4);
    return choice;
}

// 护士床位信息查看方式选择菜单
int nurseBedViewMenu()
{
    printMenuBorder();
    printMenuTitle("床位信息查看");
    printMenuBorder();
    std::cout << "|  1. 查看本科室全部床位                        |" << std::endl;
    std::cout << "|  2. 按床位状态查询                            |" << std::endl;
    std::cout << "|  3. 按患者ID查询                              |" << std::endl;
    std::cout << "|  4. 按护士ID查询                              |" << std::endl;
    std::cout << "|  5. 按科室查询                                |" << std::endl;
    std::cout << "|  6. 按床位ID查询                              |" << std::endl;
    std::cout << "|  7. 按病房类型查询                            |" << std::endl;
    std::cout << "|  0. 返回上一级                                |" << std::endl;
    printMenuBorder();

    int viewChoice = selectIntCheck(0, 7);
    return viewChoice;
}

// 护士床位信息修改菜单
int nurseBedModificationMenu()
{
    printMenuBorder();
    printMenuTitle("修改床位信息");
    printMenuBorder();
    std::cout << "|  1. 修改床位状态                              |" << std::endl;
    std::cout << "|  2. 修改备注                                  |" << std::endl;
    std::cout << "|  3. 更新生命体征                              |" << std::endl;
    std::cout << "|  0. 返回上一级                                |" << std::endl;
    printMenuBorder();

    int choice = selectIntCheck(0, 3);
    return choice;
}

// 护士个人信息管理菜单
int nursePersonalInfoManagementMenu()
{
    printMenuBorder();
    printMenuTitle("个人信息管理");
    printMenuBorder();
    std::cout << "|  1. 查看个人信息                              |" << std::endl;
    std::cout << "|  2. 修改个人信息                              |" << std::endl;
    std::cout << "|  0. 返回上一级                                |" << std::endl;
    printMenuBorder();

    int choice = selectIntCheck(0, 2);
    return choice;
}

// 护士个人信息查看选择菜单
int nursePersonalInfoViewMenu()
{
    printMenuBorder();
    printMenuTitle("查看个人信息");
    printMenuBorder();
    std::cout << "|  1. 护士ID                                    |" << std::endl;
    std::cout << "|  2. 姓名                                      |" << std::endl;
    std::cout << "|  3. 性别                                      |" << std::endl;
    std::cout << "|  4. 年龄                                      |" << std::endl;
    std::cout << "|  5. 科室                                      |" << std::endl;
    std::cout << "|  6. 护士等级                                  |" << std::endl;
    std::cout << "|  7. 联系电话                                  |" << std::endl;
    std::cout << "|  8. 邮箱                                      |" << std::endl;
    std::cout << "|  9. 在岗状态                                  |" << std::endl;
    std::cout << "|  10. 排班信息                                 |" << std::endl;
    std::cout << "|  11. 累计护理人数                             |" << std::endl;
    std::cout << "|  12. 累计床位管理次数                         |" << std::endl;
    std::cout << "|  13. 账户创建时间                             |" << std::endl;
    std::cout << "|  0. 返回上一级                                |" << std::endl;
    printMenuBorder();

    int viewChoice = selectIntCheck(0, 13);
    return viewChoice;
}

// 护士个人信息修改菜单
int nursePersonalInfoModificationMenu()
{
    printMenuBorder();
    printMenuTitle("修改个人信息");
    printMenuBorder();
    std::cout << "|  1. 修改姓名                                  |" << std::endl;
    std::cout << "|  2. 修改性别                                  |" << std::endl;
    std::cout << "|  3. 修改年龄                                  |" << std::endl;
    std::cout << "|  4. 修改科室                                  |" << std::endl;
    std::cout << "|  5. 修改护士等级                              |" << std::endl;
    std::cout << "|  6. 修改联系电话                              |" << std::endl;
    std::cout << "|  7. 修改邮箱                                  |" << std::endl;
    std::cout << "|  8. 修改在岗状态                              |" << std::endl;
    std::cout << "|  9. 修改排班信息                              |" << std::endl;
    std::cout << "|  10. 修改密码                                 |" << std::endl;
    std::cout << "|  0. 返回上一级                                |" << std::endl;
    printMenuBorder();

    int choice = selectIntCheck(0, 10);
    return choice;
}