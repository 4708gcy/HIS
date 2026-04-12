
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
        std::cout << "请输入你的选择: ";
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
        if (!input.empty())
        {
            return input;
        }
        else
        {
            std::cout << "输入不能为空，请重新输入!" << std::endl;
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
    std::string bedNumber, dpt, area,type, ward, num;
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
//============================= 菜单显示函数区域 =======================================

// 登录和注册选择界面
int loginRegisterUI()
{
    std::cout << "欢迎来到HIS系统!" << std::endl;
    std::cout << "请选择操作:" << std::endl;
    std::cout << "1. 登录" << std::endl;
    std::cout << "2. 注册" << std::endl;
    std::cout << "0. 退出系统" << std::endl;

    int choice = selectIntCheck(0, 2);
    return choice;
}

// 开始界面，显示欢迎信息和主菜单
int beginUI()
{
    std::cout << "欢迎使用HIS系统!" << std::endl;
    std::cout << "请选择你的身份" << std::endl;
    std::cout << "1. 管理员" << std::endl;
    std::cout << "2. 医生" << std::endl;
    std::cout << "3. 护士" << std::endl;
    std::cout << "4. 药剂师" << std::endl;
    std::cout << "5. 患者" << std::endl;
    std::cout << "0. 返回上一级" << std::endl;

    int choice = selectIntCheck(0, 5);
    return choice;
}

// 管理员科室管理菜单(可扩展)
std::string adminDepartmentMenu()
{
    std::cout << "科室选择界面" << std::endl;
    std::cout << "请选择你要进入的科室:" << std::endl;
    std::cout << "1. 内科" << std::endl;
    std::cout << "2. 外科" << std::endl;
    std::cout << "3. 妇产科" << std::endl;
    std::cout << "4. 急诊科" << std::endl;
    std::cout << "5. 儿科" << std::endl;
    std::cout << "0. 返回上级菜单" << std::endl;

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
    std::cout << "欢迎进入管理员界面!" << std::endl;
    std::cout << "请选择操作:" << std::endl;
    std::cout << "1. 账户管理" << std::endl;
    std::cout << "2. 医疗记录管理" << std::endl;
    std::cout << "3. 药品管理" << std::endl;
    std::cout << "0. 退出登录" << std::endl;

    int choice = selectIntCheck(0, 3);
    return choice;
}

// 管理员医疗记录管理菜单
int adminMedicalRecordMenu()
{
    std::cout << "医疗记录管理界面" << std::endl;
    std::cout << "请选择你要管理的医疗记录类型:" << std::endl;
    std::cout << "1. 挂号记录" << std::endl;
    std::cout << "2. 看诊记录" << std::endl;
    std::cout << "3. 检查记录" << std::endl;
    std::cout << "4. 住院记录" << std::endl;
    std::cout << "5. 用药记录" << std::endl;
    std::cout << "0. 返回上级菜单" << std::endl;

    int choice = selectIntCheck(0, 5);
    return choice;
}

// 管理员挂号记录管理菜单
int adminRegistrationManagementMenu()
{
    std::cout << "挂号记录管理界面" << std::endl;
    std::cout << "请选择你要进行的操作:" << std::endl;
    std::cout << "1. 查看挂号记录" << std::endl;
    std::cout << "2. 修改挂号状态" << std::endl;
    std::cout << "3. 删除挂号记录" << std::endl;
    std::cout << "4. 添加挂号记录" << std::endl;
    std::cout << "0. 返回上级菜单" << std::endl;

    int choice = selectIntCheck(0, 4);
    return choice;
}

// 管理员挂号记录查看方式选择菜单
int adminRegistrationViewMenu()
{
    std::cout << "请选择你要查看的方式:" << std::endl;
    std::cout << "1. 查看该科室的所有挂号记录" << std::endl;
    std::cout << "2. 根据挂号状态查看" << std::endl;
    std::cout << "3. 根据患者ID查看" << std::endl;
    std::cout << "4. 根据医生ID查看" << std::endl;
    std::cout << "5. 根据挂号记录ID查看" << std::endl;
    std::cout << "0. 返回上级菜单" << std::endl;
    int viewChoice = selectIntCheck(0, 5);
    return viewChoice;
}

// 管理员看诊记录管理菜单
int adminConsultationManagementMenu()
{
    std::cout << "看诊记录管理界面" << std::endl;
    std::cout << "请选择你要进行的操作:" << std::endl;
    std::cout << "1. 查看看诊记录" << std::endl;
    std::cout << "2. 修改看诊状态" << std::endl;
    std::cout << "3. 删除看诊记录" << std::endl;
    std::cout << "4. 添加看诊记录" << std::endl;
    std::cout << "0. 返回上级菜单" << std::endl;

    int choice = selectIntCheck(0, 4);
    return choice;
}

// 管理员看诊记录查看方式选择菜单
int adminConsultationViewMenu()
{
    std::cout << "请选择你要查看的方式:" << std::endl;
    std::cout << "1. 查看该科室的所有看诊记录" << std::endl;
    std::cout << "2. 根据患者ID查看" << std::endl;
    std::cout << "3. 根据医生ID查看" << std::endl;
    std::cout << "4. 根据状态查看" << std::endl;
    std::cout << "5. 根据挂号记录ID查看" << std::endl;
    std::cout << "6. 根据看诊记录ID查看" << std::endl;
    std::cout << "0. 返回上级菜单" << std::endl;
    int viewChoice = selectIntCheck(0, 6);
    return viewChoice;
}

// 管理员检查记录管理菜单
int adminExaminationManagementMenu()
{
    std::cout << "检查记录管理界面" << std::endl;
    std::cout << "请选择你要进行的操作:" << std::endl;
    std::cout << "1. 查看检查记录" << std::endl;
    std::cout << "2. 修改检查状态" << std::endl;
    std::cout << "3. 删除检查记录" << std::endl;
    std::cout << "4. 添加检查记录" << std::endl;
    std::cout << "0. 返回上级菜单" << std::endl;

    int choice = selectIntCheck(0, 4);
    return choice;
}

// 管理员检查记录查看方式选择菜单
int adminExaminationViewMenu()
{
    std::cout << "请选择你要查看的方式:" << std::endl;
    std::cout << "1. 查看该科室的所有检查记录" << std::endl;
    std::cout << "2. 根据患者ID查看" << std::endl;
    std::cout << "3. 根据医生ID查看" << std::endl;
    std::cout << "4. 根据状态查看" << std::endl;
    std::cout << "5. 根据检查记录ID查看" << std::endl;
    std::cout << "0. 返回上级菜单" << std::endl;
    int viewChoice = selectIntCheck(0, 5);
    return viewChoice;
}

// 检查项目选择菜单
std::string ExaminationItemMenu()
{
    std::cout << "请输入检查项目名称: " << std::endl;
    std::cout << "1. 体温测量" << std::endl;
    std::cout << "2. 血压测量" << std::endl;
    std::cout << "3. 心率测量" << std::endl;
    std::cout << "4. 呼吸频率测量" << std::endl;
    std::cout << "5. 脉搏血氧测量" << std::endl;
    std::cout << "6. 身高测量" << std::endl;
    std::cout << "7. 体重测量" << std::endl;
    std::cout << "8. BMI计算" << std::endl;
    std::cout << "9. 疼痛评估" << std::endl;
    std::cout << "10. 腰围测量" << std::endl;
    std::cout << "11. 血糖测量" << std::endl;
    std::cout << "12. 体脂测量" << std::endl;
    std::cout << "13. 尿酸测定" << std::endl;
    std::cout << "14. 血脂测定" << std::endl;
    std::cout << "0. 返回上级菜单" << std::endl;

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
    std::cout << "住院记录管理界面" << std::endl;
    std::cout << "请选择你要进行的操作:" << std::endl;
    std::cout << "1. 查看住院记录" << std::endl;
    std::cout << "2. 修改住院记录状态" << std::endl;
    std::cout << "3. 删除住院记录" << std::endl;
    std::cout << "4. 添加住院记录" << std::endl;
    std::cout << "0. 返回上级菜单" << std::endl;

    int choice = selectIntCheck(0, 4);
    return choice;
}

// 管理员住院记录查看方式选择菜单
int adminHospitalizationViewMenu()
{
    std::cout << "请选择你要查看的方式:" << std::endl;
    std::cout << "1. 查看该科室的所有住院记录" << std::endl;
    std::cout << "2. 根据患者ID查看" << std::endl;
    std::cout << "3. 根据医生ID查看" << std::endl;
    std::cout << "4. 根据护士ID查看" << std::endl;
    std::cout << "5.根据住院记录ID查看" << std::endl;
    std::cout << "6. 根据状态查看" << std::endl;
    std::cout << "7. 根据病房类型查看" << std::endl;
    std::cout << "0. 返回上级菜单" << std::endl;
    int viewChoice = selectIntCheck(0, 7);
    return viewChoice;
}

// 管理员住院记录病房类型选择菜单
std::string HospitalizationWardTypeMenu()
{
    std::cout << "请选择病房类型:" << std::endl;
    std::cout << "1. 普通病房" << std::endl;
    std::cout << "2. 隔离病房" << std::endl;
    std::cout << "3. VIP病房" << std::endl;
    std::cout << "4. ICU病房" << std::endl;
    std::cout << "0. 返回上级菜单" << std::endl;

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