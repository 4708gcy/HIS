
#include "../Head/UI.h"

// 暂停函数，等待用户按键继续
void pause()
{
    std::cout << "按下回车键继续..." << std::flush;
    std::string dummy;
    std::getline(std::cin, dummy);
}

// ============================ 输入校验函数区域 =======================================
int selectIntCheck(const int min, const int max)
{
    int choice;
    while (true)
    {
        std::cout << "请输入你的选择: ";
        if (std::cin >> choice && choice >= min && choice <= max)
        {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return choice;
        }
        else
        {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "无效输入，请输入 " << min << " - " << max << " 之间的数字!" << std::endl;
        }
    }
}

double inputFeeCheck()
{
    double fee;
    while (true)
    {
        std::cout << "请输入费用: ";
        if (std::cin >> fee && fee >= 0)
        {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return fee;
        }
        else
        {
            std::cin.clear();                                                   // 清除错误状态
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // 丢弃无效输入
            std::cout << "无效输入，请输入一个非负数!" << std::endl;
        }
    }
}

std::string inputStringCheck(const std::string &prompt)
{
    std::string input;
    while (true)
    {
        std::cout << prompt;
        std::cin >> input;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

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

std::string inputIDCheck(const std::string &prompt)
{
    std::string id;
    while (true)
    {
        std::cout << prompt;
        std::cin >> id;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
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
        std::cin >> id;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

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

std::string inputPwdCheck(const std::string &prompt)
{
    std::string pwd;
    while (true)
    {
        std::cout << prompt << "(密码必须至少8位，包含字母和数字): " << std::endl;
        std::cin >> pwd;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (pwd.length() < 8)
        {
            std::cout << "密码长度不能少于8位，请重新输入！" << std::endl;
            continue;
        }

        // 可选：要求必须包含字母和数字
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

        // 通过所有检查
        return pwd;
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
    std::cout << "0. 返回上级菜单" << std::endl;
    int viewChoice = selectIntCheck(0, 4);
    return viewChoice;
}