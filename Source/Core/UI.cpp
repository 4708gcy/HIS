
#include "Core/UI.h"
#include "Core/GetTime.h"
#include "Entities/Consultation.h"
#include "Entities/Examination.h"
#include "Entities/MedicationRecord.h"
#include "Entities/Hospitalization.h"
#include "Core/User.h"
#include "Roles/Doctor.h"
#include "Roles/Nurse.h"
#include "Roles/Pharmacist.h"
#include "Roles/Patient.h"
#ifdef _WIN32
#include <windows.h>
#endif
#include <ctime>
#include <cstdio>
#include <fstream>
#include <mutex>
#include <iomanip>
#include <set>

// 暂停函数，等待用户按键继续（支持面包屑导航显示）
void pause(const std::string &breadcrumb)
{
    if (!breadcrumb.empty())
    {
        setConsoleColor(ConsoleColor::CYAN);
        std::cout << "  [" << breadcrumb << "]" << std::endl;
        resetConsoleColor();
    }
    setConsoleColor(ConsoleColor::YELLOW);
    std::cout << "  按下回车键继续..." << std::flush;
    resetConsoleColor();
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

// ======================================== 终端颜色函数 =======================================

void setConsoleColor(ConsoleColor color)
{
#ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    switch (color)
    {
    case ConsoleColor::RED:
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
        break;
    case ConsoleColor::GREEN:
        SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        break;
    case ConsoleColor::YELLOW:
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
        break;
    case ConsoleColor::CYAN:
        SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
        break;
    case ConsoleColor::WHITE:
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
        break;
    default:
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        break;
    }
#else
    // ANSI 颜色码（Linux/Mac 兼容）
    switch (color)
    {
    case ConsoleColor::RED:    std::cout << "\033[91m"; break;
    case ConsoleColor::GREEN:  std::cout << "\033[92m"; break;
    case ConsoleColor::YELLOW: std::cout << "\033[93m"; break;
    case ConsoleColor::CYAN:   std::cout << "\033[96m"; break;
    case ConsoleColor::WHITE:  std::cout << "\033[97m"; break;
    default:                   std::cout << "\033[0m"; break;
    }
#endif
}

void resetConsoleColor()
{
    setConsoleColor(ConsoleColor::DEFAULT);
}

void clearScreen()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

int getDisplayWidth(const std::string &str)
{
    int width = 0;
    for (size_t i = 0; i < str.size();)
    {
        unsigned char c = static_cast<unsigned char>(str[i]);
        if (c < 0x80)
        {
            width += 1;
            i += 1;
        }
        else if (c < 0xE0)
        {
            width += 2;
            i += 2;
        }
        else if (c < 0xF0)
        {
            width += 2;
            i += 3;
        }
        else
        {
            width += 2;
            i += 4;
        }
    }
    return width;
}

void printTitle(const std::string &title)
{
    int titleWidth = getDisplayWidth(title);
    int totalWidth = titleWidth + 4; // 两侧各留2空格
    int borderLen = totalWidth;
    if (borderLen < 30) borderLen = 30;
    totalWidth = borderLen;

    setConsoleColor(ConsoleColor::CYAN);
    std::cout << "\n╔";
    for (int i = 0; i < totalWidth; i++) std::cout << "═";
    std::cout << "╗" << std::endl;

    int leftPad = (totalWidth - titleWidth) / 2;
    int rightPad = totalWidth - leftPad - titleWidth;
    std::cout << "║" << std::string(leftPad, ' ');
    setConsoleColor(ConsoleColor::WHITE);
    std::cout << title;
    setConsoleColor(ConsoleColor::CYAN);
    std::cout << std::string(rightPad, ' ') << "║" << std::endl;

    std::cout << "╚";
    for (int i = 0; i < totalWidth; i++) std::cout << "═";
    std::cout << "╝" << std::endl;
    resetConsoleColor();
}

void printSuccess(const std::string &msg)
{
    setConsoleColor(ConsoleColor::GREEN);
    std::cout << "[成功] " << msg << std::endl;
    resetConsoleColor();
}

void printError(const std::string &msg)
{
    setConsoleColor(ConsoleColor::RED);
    std::cout << "[错误] " << msg << std::endl;
    resetConsoleColor();
}

void printWarning(const std::string &msg)
{
    setConsoleColor(ConsoleColor::YELLOW);
    std::cout << "[警告] " << msg << std::endl;
    resetConsoleColor();
}

void printInfoCard(const std::string &title, const std::vector<std::pair<std::string, std::string>> &fields)
{
    setConsoleColor(ConsoleColor::CYAN);
    std::cout << "╔══════════════════════════════════════════════════╗" << std::endl;
    // Center the title
    int titleWidth = 0;
    for (char c : title) titleWidth += (static_cast<unsigned char>(c) > 127) ? 2 : 1;
    int leftPad = (48 - titleWidth) / 2;
    std::cout << "║" << std::string(leftPad, ' ') << title;
    int rightPad = 48 - leftPad - titleWidth;
    std::cout << std::string(rightPad, ' ') << "║" << std::endl;
    std::cout << "╠══════════════════════════════════════════════════╣" << std::endl;
    resetConsoleColor();

    for (const auto &field : fields)
    {
        std::string label = field.first;
        std::string value = field.second.empty() ? "未填写" : field.second;
        std::string content = label + ": " + value;
        int contentWidth = getDisplayWidth(content);
        int rightPad = 49 - contentWidth; // 50 - 1(space after ║)
        if (rightPad < 0) rightPad = 0;
        setConsoleColor(ConsoleColor::WHITE);
        std::cout << "║ ";
        resetConsoleColor();
        setConsoleColor(ConsoleColor::CYAN);
        std::cout << label;
        resetConsoleColor();
        std::cout << ": ";
        setConsoleColor(ConsoleColor::WHITE);
        std::cout << value;
        std::cout << std::string(rightPad, ' ');
        setConsoleColor(ConsoleColor::CYAN);
        std::cout << "║" << std::endl;
        resetConsoleColor();
    }

    setConsoleColor(ConsoleColor::CYAN);
    std::cout << "╚══════════════════════════════════════════════════╝" << std::endl;
    resetConsoleColor();
}

// ======================================== 操作日志系统 =======================================

LogManager::LogManager()
{
    MyTime &t = MyTime::getInstance();
    std::string today = t.getTime().substr(0, 10);
    // 替换日期中的 '-' 为 '_'
    for (auto &c : today) if (c == '-') c = '_';
    logFilePath = "../Data/OperationLog/his_" + today + ".log";

    // 确保日志目录存在
#ifdef _WIN32
    CreateDirectoryA("../Data/OperationLog", nullptr);
#endif
}

LogManager &LogManager::getInstance()
{
    static LogManager instance;
    return instance;
}

void LogManager::writeLog(const std::string &level, const std::string &msg)
{
    std::lock_guard<std::mutex> lock(logMutex);
    std::ofstream logFile(logFilePath, std::ios::app);
    if (logFile.is_open())
    {
        MyTime &t = MyTime::getInstance();
        logFile << "[" << t.getTime() << "] [" << level << "] " << msg << std::endl;
        logFile.close();
    }
}

void LogManager::info(const std::string &msg) { writeLog("INFO", msg); }
void LogManager::warn(const std::string &msg) { writeLog("WARN", msg); }
void LogManager::error(const std::string &msg) { writeLog("ERROR", msg); }

void LogManager::logOperation(const std::string &userId, const std::string &role,
                              const std::string &operation, const std::string &detail)
{
    std::string msg = "[用户:" + userId + "] [角色:" + role + "] [操作:" + operation + "] [详情:" + detail + "]";
    writeLog("OPERATION", msg);
}

// ============================ 输入校验函数区域 =======================================

std::string padId(int counter, int digits)
{
    std::string s = std::to_string(counter);
    if (static_cast<int>(s.length()) >= digits) return s;
    return std::string(digits - s.length(), '0') + s;
}

bool deptMatch(const std::string &entityDept, const std::string &filter)
{
    return filter == "全院" || entityDept == filter;
}

// 1. 安全整数输入
int selectIntCheck(const int min, const int max)
{
    int choice;
    std::string line;
    while (true)
    {
        std::cout << "请输入整数: ";
        std::getline(std::cin, line);
        if (std::cin.eof() || std::cin.fail())
        {
            return 0; // EOF 或读取失败时返回 0（通常表示返回/取消）
        }
        line = trim(line); // 去除首尾空格
        std::stringstream ss(line);
        if (ss >> choice && !(ss >> line) && choice >= min && choice <= max)
        {
            return choice;
        }
        else
        {
            setConsoleColor(ConsoleColor::RED);
            std::cout << "无效输入，请输入 " << min << " - " << max << " 之间的数字!" << std::endl;
            resetConsoleColor();
        }
    }
}

// 2. 安全 double 输入
int inputFeeCheck(const std::string &prompt)
{
    double fee;
    std::string line;
    while (true)
    {
        std::cout << prompt;
        std::getline(std::cin, line);
        if (std::cin.eof() || std::cin.fail()) return 0;
        line = trim(line);
        std::stringstream ss(line);
        if (ss >> fee && !(ss >> line) && fee >= 0)
        {
            return static_cast<int>(fee * 100 + 0.5); // 元转分，四舍五入
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
        if (std::cin.eof() || std::cin.fail()) return "";
        input = trim(input); // 去除首尾空格
        if (input.empty())
        {
            std::cout << "输入不能为空，请重新输入!" << std::endl;
        }
        else if (input.find(',') != std::string::npos)
        {
            std::cout << "输入不能包含英文逗号(,)！请重新输入!" << std::endl;
        }
        else if (input.find('，') != std::string::npos)
        {
            std::cout << "输入不能包含中文逗号(，)！请重新输入!" << std::endl;
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
        if (std::cin.eof() || std::cin.fail()) return "";
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
        if (std::cin.eof() || std::cin.fail()) return "";
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
        if (std::cin.eof() || std::cin.fail()) return "";
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

// 5b. 隐藏密码输入（用于登录场景，Windows下不回显字符）
std::string inputHiddenPwdCheck(const std::string &prompt)
{
    std::string pwd;
    std::cout << prompt << std::endl;

#ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
    if (GetConsoleScreenBufferInfo(hConsole, &csbiInfo))
    {
        DWORD charsWritten = 0;
        COORD cursorPos = csbiInfo.dwCursorPosition;
        // 覆盖当前行的"请输入密码"提示后的内容
        FillConsoleOutputCharacter(hConsole, ' ', 80, cursorPos, &charsWritten);
        SetConsoleCursorPosition(hConsole, cursorPos);
    }
#endif

    // 使用 Windows API 实现无回显输入
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode = 0;
    GetConsoleMode(hStdin, &mode);
    SetConsoleMode(hStdin, mode & ~ENABLE_LINE_INPUT & ~ENABLE_ECHO_INPUT);

    char c = 0;
    DWORD charsRead = 0;
    while (ReadFile(hStdin, &c, 1, &charsRead, nullptr) && charsRead == 1)
    {
        if (c == '\r' || c == '\n') break;
        if (c == '\b' || c == 127) // 退格
        {
            if (!pwd.empty())
            {
                pwd.pop_back();
                std::cout << "\b \b";
            }
        }
        else if (c >= 32 && c <= 126)
        {
            pwd += c;
            std::cout << '*';
        }
    }
    std::cout << std::endl;
    SetConsoleMode(hStdin, mode); // 恢复控制台模式
    pwd = trim(pwd);
    return pwd;
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
// 12. 安全日期输入（含语义验证）
std::string inputDateCheck(const std::string &prompt)
{
    std::string date;
    while (true)
    {
        std::cout << prompt << "（格式 YYYY-MM-DD）: ";
        std::getline(std::cin, date);
        date = trim(date);

        if (date.length() != 10 || date[4] != '-' || date[7] != '-' ||
            !std::all_of(date.begin(), date.end(), [](char c)
                         { return std::isdigit(c) || c == '-'; }))
        {
            std::cout << "输入无效，请重新输入！" << std::endl;
            continue;
        }

        // 语义验证：检查年月日是否合法
        int year = std::stoi(date.substr(0, 4));
        int month = std::stoi(date.substr(5, 2));
        int day = std::stoi(date.substr(8, 2));

        if (year < 1900 || year > 2100 || month < 1 || month > 12 || day < 1 || day > 31)
        {
            std::cout << "输入无效，请重新输入！" << std::endl;
            continue;
        }

        // 使用 mktime 验证日期有效性（自动处理闰年、月份天数等）
        std::tm timeInfo = {};
        timeInfo.tm_year = year - 1900;
        timeInfo.tm_mon = month - 1;
        timeInfo.tm_mday = day;
        timeInfo.tm_hour = 12; // 避免时区边界问题
        std::mktime(&timeInfo);

        if (timeInfo.tm_mon != month - 1 || timeInfo.tm_mday != day)
        {
            std::cout << "输入无效，请重新输入！" << std::endl;
            continue;
        }

        break;
    }
    return date;
}

// 13. 正确医疗科室输入
std::string inputDepartmentCheck(const std::string &prompt)
{
    std::string department;
    while (true)
    {
        std::cout << prompt << "（输入科室名称，如 内科/外科/妇产科/急诊科/儿科 或自定义科室）: ";
        std::getline(std::cin, department);
        department = trim(department);

        if (!department.empty() && department.find(',') == std::string::npos && department.find('，') == std::string::npos)
        {
            break;
        }
        else
        {
            std::cout << "输入无效（不能为空或包含逗号），请重新输入！" << std::endl;
        }
    }
    return department;
}

// 从角色链表中聚合所有唯一科室名
std::vector<std::string> getDepartmentList(Doctor *docHead, Nurse *nurseHead, Pharmacist *phaHead, Patient *patientHead)
{
    std::set<std::string> deptSet;
    auto collect = [&](auto *head)
    {
        auto *cur = head;
        while (cur)
        {
            if (!cur->isDeleted && !cur->department.empty() && cur->department != "#")
                deptSet.insert(cur->department);
            cur = cur->next;
        }
    };
    collect(docHead);
    collect(nurseHead);
    collect(phaHead);
    collect(patientHead);

    if (deptSet.empty())
        return {"内科", "外科", "妇产科", "急诊科", "儿科"};

    return std::vector<std::string>(deptSet.begin(), deptSet.end());
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

static const int MENU_INNER_WIDTH = 40; // 菜单内部可用宽度（显示列数）

// 打印菜单边框（Unicode 双线）
void printMenuBorder()
{
    setConsoleColor(ConsoleColor::CYAN);
    std::cout << "╔";
    for (int i = 0; i < MENU_INNER_WIDTH; i++)
        std::cout << "═";
    std::cout << "╗" << std::endl;
    resetConsoleColor();
}

// 打印菜单底边框
void printMenuBottom()
{
    setConsoleColor(ConsoleColor::CYAN);
    std::cout << "╚";
    for (int i = 0; i < MENU_INNER_WIDTH; i++)
        std::cout << "═";
    std::cout << "╝" << std::endl;
    resetConsoleColor();
}

// 打印居中标题（CJK 宽度感知）
void printMenuTitle(const std::string &title)
{
    int titleWidth = getDisplayWidth(title);
    int leftPad = (MENU_INNER_WIDTH - titleWidth) / 2;
    if (leftPad < 1) leftPad = 1;
    int rightPad = MENU_INNER_WIDTH - leftPad - titleWidth;
    if (rightPad < 0) rightPad = 0;
    setConsoleColor(ConsoleColor::CYAN);
    std::cout << "╠";
    resetConsoleColor();
    setConsoleColor(ConsoleColor::WHITE);
    std::cout << std::string(leftPad, ' ') << title << std::string(rightPad, ' ');
    setConsoleColor(ConsoleColor::CYAN);
    std::cout << "╣" << std::endl;
    resetConsoleColor();
}

// 打印菜单分隔线
static void printMenuSeparator()
{
    setConsoleColor(ConsoleColor::CYAN);
    std::cout << "║";
    for (int i = 0; i < MENU_INNER_WIDTH; i++)
        std::cout << " ";
    std::cout << "║" << std::endl;
    resetConsoleColor();
}

// 打印格式化菜单项（自动对齐到右边框）
void printMenuItem(int num, const std::string &text)
{
    std::string item = "  " + std::to_string(num) + ". " + text;
    int itemWidth = getDisplayWidth(item);
    int rightPad = MENU_INNER_WIDTH - itemWidth;
    if (rightPad < 0) rightPad = 0;
    setConsoleColor(ConsoleColor::CYAN);
    std::cout << "║";
    resetConsoleColor();
    setConsoleColor(ConsoleColor::WHITE);
    std::cout << item << std::string(rightPad, ' ');
    setConsoleColor(ConsoleColor::CYAN);
    std::cout << "║" << std::endl;
    resetConsoleColor();
}

// 打印无编号的菜单行（用于提示文字等）
void printMenuLine(const std::string &text)
{
    int textWidth = getDisplayWidth(text);
    int rightPad = MENU_INNER_WIDTH - textWidth;
    if (rightPad < 0) rightPad = 0;
    setConsoleColor(ConsoleColor::CYAN);
    std::cout << "║";
    resetConsoleColor();
    setConsoleColor(ConsoleColor::WHITE);
    std::cout << text << std::string(rightPad, ' ');
    setConsoleColor(ConsoleColor::CYAN);
    std::cout << "║" << std::endl;
    resetConsoleColor();
}

//============================= 时间范围工具 =======================================

bool isTimestampInRange(const std::string &timestamp, const std::string &startTime, const std::string &endTime)
{
    if (timestamp == "#" || timestamp.empty()) return false;
    if (startTime.empty() && endTime.empty()) return true;
    if (!startTime.empty() && timestamp < startTime) return false;
    if (!endTime.empty() && timestamp > endTime) return false;
    return true;
}

std::string getCurrentMonthStart()
{
    MyTime &t = MyTime::getInstance();
    std::string now = t.getTime();
    return now.substr(0, 8) + "01 00:00:00";
}

std::string getMonthsAgoStart(int monthsAgo)
{
    MyTime &t = MyTime::getInstance();
    int year = t.getYear();
    int month = t.getMonth();
    month -= monthsAgo;
    while (month <= 0) { month += 12; year--; }
    char buf[20];
    snprintf(buf, sizeof(buf), "%04d-%02d-01 00:00:00", year, month);
    return std::string(buf);
}

int timeRangeMenu()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("选择时间范围");
    printMenuBottom();
    printMenuItem(1, "本月");
    printMenuItem(2, "近三个月");
    printMenuItem(3, "近半年");
    printMenuItem(4, "自定义时间范围");
    printMenuItem(5, "全部时间");
    printMenuItem(0, "返回上级菜单");
    printMenuBottom();
    int choice = selectIntCheck(0, 5);
    return choice;
}

//============================= 菜单显示函数区域 =======================================

// 身份选择界面
int identitySelectionMenu()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("身份选择");
    printMenuBottom();
    printMenuItem(1, "管理员");
    printMenuItem(2, "医生");
    printMenuItem(3, "护士");
    printMenuItem(4, "药剂师");
    printMenuItem(5, "患者");
    printMenuItem(0, "退出系统");
    printMenuBottom();

    int choice = selectIntCheck(0, 5);
    return choice;
}

// 登录和注册选择界面
int loginRegisterUI()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("欢迎使用 HIS 医院管理系统");
    printMenuBottom();
    printMenuItem(1, "登录");
    printMenuItem(2, "注册");
    printMenuItem(0, "退出系统");
    printMenuBottom();

    int choice = selectIntCheck(0, 2);
    return choice;
}

// 开始界面，显示欢迎信息和主菜单
int beginUI()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("欢迎使用 HIS 系统");
    printMenuBottom();
    printMenuItem(1, "管理员");
    printMenuItem(2, "医生");
    printMenuItem(3, "护士");
    printMenuItem(4, "药剂师");
    printMenuItem(5, "患者");
    printMenuItem(0, "返回上级菜单");
    printMenuBottom();

    int choice = selectIntCheck(0, 5);
    return choice;
}

// 管理员科室管理菜单(可扩展)
std::string adminDepartmentMenu()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("科室选择");
    printMenuBottom();
    printMenuItem(1, "全院");
    printMenuItem(2, "内科");
    printMenuItem(3, "外科");
    printMenuItem(4, "妇产科");
    printMenuItem(5, "急诊科");
    printMenuItem(6, "儿科");
    printMenuItem(0, "返回上级菜单");
    printMenuBottom();

    int choice = selectIntCheck(0, 6);

    switch (choice)
    {
    case 0:
        return "0";
    case 1:
        return "全院";
    case 2:
        return "内科";
    case 3:
        return "外科";
    case 4:
        return "妇产科";
    case 5:
        return "急诊科";
    case 6:
        return "儿科";
    default:
        return "";
    }
    return "";
}

// 管理员身份主菜单
int adminMenu()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("管理员主菜单");
    printMenuBottom();
    printMenuItem(1, "账户管理");
    printMenuItem(2, "医疗记录管理");
    printMenuItem(3, "药品管理");
    printMenuItem(4, "床位管理");
    printMenuItem(5, "管理员信息管理");
    printMenuItem(6, "个人信息管理");
    printMenuItem(7, "账号封锁管理");
    printMenuItem(8, "统计报表");
    printMenuItem(9, "AI 智能分析");
    printMenuItem(0, "退出登录");
    printMenuBottom();

    int choice = selectIntCheck(0, 9);
    return choice;
}

// 管理员医疗记录管理菜单
int adminMedicalRecordMenu()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("医疗记录管理");
    printMenuBottom();
    printMenuItem(1, "挂号记录");
    printMenuItem(2, "看诊记录");
    printMenuItem(3, "检查记录");
    printMenuItem(4, "住院记录");
    printMenuItem(5, "用药记录");
    printMenuItem(0, "返回上级菜单");
    printMenuBottom();

    int choice = selectIntCheck(0, 5);
    return choice;
}

// 管理员挂号记录管理菜单
int adminRegistrationManagementMenu()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("挂号记录管理");
    printMenuBottom();
    printMenuItem(1, "查看挂号记录");
    printMenuItem(2, "修改挂号状态");
    printMenuItem(3, "删除挂号记录");
    printMenuItem(4, "添加挂号记录");
    printMenuItem(0, "返回上级菜单");
    printMenuBottom();

    int choice = selectIntCheck(0, 4);
    return choice;
}

// 管理员挂号记录查看方式选择菜单
int adminRegistrationViewMenu()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("挂号记录查看");
    printMenuBottom();
    printMenuItem(1, "查看所有挂号记录");
    printMenuItem(2, "根据挂号状态查看");
    printMenuItem(3, "根据患者ID查看");
    printMenuItem(4, "根据医生ID查看");
    printMenuItem(5, "根据挂号记录ID查看");
    printMenuItem(0, "返回上级菜单");
    printMenuBottom();
    int viewChoice = selectIntCheck(0, 5);
    return viewChoice;
}

// 管理员看诊记录管理菜单
int adminConsultationManagementMenu()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("看诊记录管理");
    printMenuBottom();
    printMenuItem(1, "查看看诊记录");
    printMenuItem(2, "修改看诊状态");
    printMenuItem(3, "删除看诊记录");
    printMenuItem(4, "添加看诊记录");
    printMenuItem(0, "返回上级菜单");
    printMenuBottom();

    int choice = selectIntCheck(0, 4);
    return choice;
}

// 管理员看诊记录查看方式选择菜单
int adminConsultationViewMenu()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("看诊记录查看");
    printMenuBottom();
    printMenuItem(1, "查看所有看诊记录");
    printMenuItem(2, "根据患者ID查看");
    printMenuItem(3, "根据医生ID查看");
    printMenuItem(4, "根据状态查看");
    printMenuItem(5, "根据挂号记录ID查看");
    printMenuItem(6, "根据看诊记录ID查看");
    printMenuItem(0, "返回上级菜单");
    printMenuBottom();
    int viewChoice = selectIntCheck(0, 6);
    return viewChoice;
}

// 管理员检查记录管理菜单
int adminExaminationManagementMenu()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("检查记录管理");
    printMenuBottom();
    printMenuItem(1, "查看检查记录");
    printMenuItem(2, "修改检查状态");
    printMenuItem(3, "删除检查记录");
    printMenuItem(4, "添加检查记录");
    printMenuItem(0, "返回上级菜单");
    printMenuBottom();

    int choice = selectIntCheck(0, 4);
    return choice;
}

// 管理员检查记录查看方式选择菜单
int adminExaminationViewMenu()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("检查记录查看");
    printMenuBottom();
    printMenuItem(1, "查看所有检查记录");
    printMenuItem(2, "根据患者ID查看");
    printMenuItem(3, "根据医生ID查看");
    printMenuItem(4, "根据状态查看");
    printMenuItem(5, "根据检查记录ID查看");
    printMenuItem(0, "返回上级菜单");
    printMenuBottom();
    int viewChoice = selectIntCheck(0, 5);
    return viewChoice;
}

// 检查项目选择菜单
std::string ExaminationItemMenu()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("检查项目选择");
    printMenuBottom();
    printMenuItem(1, "体温测量");
    printMenuItem(2, "血压测量");
    printMenuItem(3, "心率测量");
    printMenuItem(4, "呼吸频率测量");
    printMenuItem(5, "脉搏血氧测量");
    printMenuItem(6, "身高测量");
    printMenuItem(7, "体重测量");
    printMenuItem(8, "BMI计算");
    printMenuItem(9, "疼痛评分");
    printMenuItem(10, "腰围测量");
    printMenuItem(11, "血糖测量");
    printMenuItem(12, "体脂测量");
    printMenuItem(13, "尿酸测定");
    printMenuItem(14, "血脂测定");
    printMenuItem(0, "返回上级菜单");
    printMenuBottom();

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
        itemName = "疼痛评分";
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
    clearScreen();
    printMenuBorder();
    printMenuTitle("住院记录管理");
    printMenuBottom();
    printMenuItem(1, "查看住院记录");
    printMenuItem(2, "修改住院记录状态");
    printMenuItem(3, "删除住院记录");
    printMenuItem(4, "添加住院记录");
    printMenuItem(0, "返回上级菜单");
    printMenuBottom();

    int choice = selectIntCheck(0, 4);
    return choice;
}

// 管理员住院记录查看方式选择菜单
int adminHospitalizationViewMenu()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("住院记录查看");
    printMenuBottom();
    printMenuItem(1, "查看所有住院记录");
    printMenuItem(2, "根据患者ID查看");
    printMenuItem(3, "根据医生ID查看");
    printMenuItem(4, "根据护士ID查看");
    printMenuItem(5, "根据住院记录ID查看");
    printMenuItem(6, "根据状态查看");
    printMenuItem(7, "根据病房类型查看");
    printMenuItem(0, "返回上级菜单");
    printMenuBottom();
    int viewChoice = selectIntCheck(0, 7);
    return viewChoice;
}

// 管理员住院记录病房类型选择菜单
std::string HospitalizationWardTypeMenu()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("病房类型选择");
    printMenuBottom();
    printMenuItem(1, "普通病房");
    printMenuItem(2, "隔离病房");
    printMenuItem(3, "VIP病房");
    printMenuItem(4, "ICU病房");
    printMenuItem(0, "返回上级菜单");
    printMenuBottom();

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
    clearScreen();
    printMenuBorder();
    printMenuTitle("床位管理");
    printMenuBottom();
    printMenuItem(1, "查看床位信息");
    printMenuItem(2, "修改床位状态");
    printMenuItem(3, "删除床位信息");
    printMenuItem(4, "添加床位信息");
    printMenuItem(0, "返回上级菜单");
    printMenuBottom();

    int choice = selectIntCheck(0, 4);
    return choice;
}

// 管理员床位查看方式选择菜单
int bedViewMenu()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("床位信息查看");
    printMenuBottom();
    printMenuItem(1, "查看所有床位信息");
    printMenuItem(2, "根据床位状态查看");
    printMenuItem(3, "根据病房类型查看");
    printMenuItem(4, "根据床位号查看");
    printMenuItem(5, "根据患者ID查看");
    printMenuItem(6, "根据住院记录ID查看");
    printMenuItem(7, "根据护士ID查看");
    printMenuItem(0, "返回上级菜单");
    printMenuBottom();

    int viewChoice = selectIntCheck(0, 7);
    return viewChoice;
}

// 管理员用药记录管理菜单
int adminMedicationRecordManagementMenu()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("用药记录管理");
    printMenuBottom();
    printMenuItem(1, "查看用药记录");
    printMenuItem(2, "修改用药记录状态");
    printMenuItem(3, "删除用药记录");
    printMenuItem(4, "添加用药记录");
    printMenuItem(0, "返回上级菜单");
    printMenuBottom();

    int choice = selectIntCheck(0, 4);
    return choice;
}

// 管理员用药记录查看方式选择菜单
int adminMedicationRecordViewMenu()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("用药记录查看");
    printMenuBottom();
    printMenuItem(1, "查看所有用药记录");
    printMenuItem(2, "根据患者ID查看");
    printMenuItem(3, "根据医生ID查看");
    printMenuItem(4, "根据药师ID查看");
    printMenuItem(5, "根据用药记录ID查看");
    printMenuItem(6, "根据药品ID查看");
    printMenuItem(7, "根据看诊记录ID查看");
    printMenuItem(8, "根据审核状态查看");
    printMenuItem(9, "根据发药状态查看");
    printMenuItem(0, "返回上级菜单");
    printMenuBottom();

    int viewChoice = selectIntCheck(0, 9);
    return viewChoice;
}

// 用药记录审核状态设置菜单
int MedicationRecordReviewResultMenu()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("用药记录审核状态");
    printMenuBottom();
    printMenuItem(1, "待审核");
    printMenuItem(2, "审核未通过");
    printMenuItem(3, "审核通过");
    printMenuItem(4, "医生撤销");
    printMenuItem(0, "返回上级菜单");
    printMenuBottom();

    int choice = selectIntCheck(0, 4);
    return choice;
}

// 用药记录状态设置菜单
int MedicationRecordStatusMenu()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("用药记录状态");
    printMenuBottom();
    printMenuItem(1, "未缴费");
    printMenuItem(2, "待发药");
    printMenuItem(3, "已发药");
    printMenuItem(4, "已退药");
    printMenuItem(0, "返回上级菜单");
    printMenuBottom();

    int choice = selectIntCheck(0, 4);
    return choice;
}

// 管理员药品管理菜单
int adminMedicineManagementMenu()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("药品管理");
    printMenuBottom();
    printMenuItem(1, "查看药品信息");
    printMenuItem(2, "修改药品信息");
    printMenuItem(3, "删除药品信息");
    printMenuItem(4, "添加药品信息");
    printMenuItem(0, "返回上级菜单");
    printMenuBottom();

    int choice = selectIntCheck(0, 4);
    return choice;
}

// 管理员药品查看方式选择菜单
int adminMedicineViewMenu()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("药品信息查看");
    printMenuBottom();
    printMenuItem(1, "查看所有药品信息");
    printMenuItem(2, "根据药品ID查看");
    printMenuItem(3, "根据药品名称查看");
    printMenuItem(4, "根据药品状态查看");
    printMenuItem(5, "根据生产厂家查看");
    printMenuItem(6, "查看低于安全库存量的药品");
    printMenuItem(0, "返回上级菜单");
    printMenuBottom();

    int viewChoice = selectIntCheck(0, 6);
    return viewChoice;
}

// 管理员药品状态设置菜单
int MedicineStatusMenu()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("药品状态选择");
    printMenuBottom();
    printMenuItem(1, "正常可用");
    printMenuItem(2, "库存低于安全阈值");
    printMenuItem(3, "已过期");
    printMenuItem(4, "已停用");
    printMenuItem(0, "返回上级菜单");
    printMenuBottom();

    int choice = selectIntCheck(0, 4);
    return choice;
}

// 修改药品信息菜单
int MedicineModificationMenu()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("修改药品信息");
    printMenuBottom();
    printMenuItem(1, "药品名称");
    printMenuItem(2, "药品规格描述");
    printMenuItem(3, "药品进价");
    printMenuItem(4, "药品售价");
    printMenuItem(5, "药品库存量");
    printMenuItem(6, "药品安全库存阈值");
    printMenuItem(7, "药品状态");
    printMenuItem(8, "药品生产日期");
    printMenuItem(9, "药品有效期限");
    printMenuItem(10, "药品备注");
    printMenuItem(11, "生产厂家");
    printMenuItem(0, "返回上级菜单");
    printMenuBottom();

    int choice = selectIntCheck(0, 11);
    return choice;
}

// 管理员用户管理菜单
int adminUserManagementMenu()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("账户管理");
    printMenuBottom();
    printMenuItem(1, "医生账户管理");
    printMenuItem(2, "护士账户管理");
    printMenuItem(3, "药剂师账户管理");
    printMenuItem(4, "患者账户管理");
    printMenuItem(0, "返回上级菜单");
    printMenuBottom();

    int choice = selectIntCheck(0, 4);
    return choice;
}

// 管理员医生信息管理菜单
int adminDoctorManagementMenu()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("医生账户管理");
    printMenuBottom();
    printMenuItem(1, "查看医生信息");
    printMenuItem(2, "修改医生信息");
    printMenuItem(3, "删除医生账户");
    printMenuItem(4, "添加医生账户");
    printMenuItem(0, "返回上级菜单");
    printMenuBottom();

    int choice = selectIntCheck(0, 4);
    return choice;
}

// 管理员医生信息查看方式选择菜单
int adminDoctorViewMenu()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("医生信息查看");
    printMenuBottom();
    printMenuItem(1, "查看所有医生信息");
    printMenuItem(2, "根据医生ID查看");
    printMenuItem(3, "根据医生姓名查看");
    printMenuItem(4, "根据在岗状态查看");
    printMenuItem(5, "根据职称查看");
    printMenuItem(0, "返回上级菜单");
    printMenuBottom();

    int viewChoice = selectIntCheck(0, 5);
    return viewChoice;
}

// 医生职称选择菜单
int DoctorTitleMenu()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("医生职称选择");
    printMenuBottom();
    printMenuItem(1, "实习医师");
    printMenuItem(2, "住院医师");
    printMenuItem(3, "主治医师");
    printMenuItem(4, "副主任医师");
    printMenuItem(5, "主任医师");
    printMenuItem(0, "返回上级菜单");
    printMenuBottom();

    int choice = selectIntCheck(0, 5);
    return choice;
}

// 管理员医生信息修改菜单
int adminDoctorModificationMenu()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("修改医生信息");
    printMenuBottom();
    printMenuItem(1, "医生姓名");
    printMenuItem(2, "医生性别");
    printMenuItem(3, "医生年龄");
    printMenuItem(4, "医生所属科室");
    printMenuItem(5, "医生职称");
    printMenuItem(6, "医生联系电话");
    printMenuItem(7, "医生邮箱地址");
    printMenuItem(8, "医生在岗状态");
    printMenuItem(9, "医生擅长方向");
    printMenuItem(10, "医生排班信息");
    printMenuItem(11, "累计接诊患者数量");
    printMenuItem(12, "累计完成的检查数量");
    printMenuItem(13, "累计开具住院证的数量");
    printMenuItem(0, "返回上级菜单");
    printMenuBottom();

    int choice = selectIntCheck(0, 13);
    return choice;
}

// 管理员护士信息管理菜单
int adminNurseManagementMenu()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("护士账户管理");
    printMenuBottom();
    printMenuItem(1, "查看护士信息");
    printMenuItem(2, "修改护士信息");
    printMenuItem(3, "删除护士账户");
    printMenuItem(4, "添加护士账户");
    printMenuItem(0, "返回上级菜单");
    printMenuBottom();

    int choice = selectIntCheck(0, 4);
    return choice;
}

// 管理员护士信息查看方式选择菜单
int adminNurseViewMenu()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("护士信息查看");
    printMenuBottom();
    printMenuItem(1, "查看所有护士信息");
    printMenuItem(2, "根据护士ID查看");
    printMenuItem(3, "根据护士姓名查看");
    printMenuItem(4, "根据在岗状态查看");
    printMenuItem(5, "根据职称查看");
    printMenuItem(0, "返回上级菜单");
    printMenuBottom();

    int viewChoice = selectIntCheck(0, 5);
    return viewChoice;
}

// 护士职称选择菜单
int NurseTitleMenu()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("护士职称选择");
    printMenuBottom();
    printMenuItem(1, "实习护士");
    printMenuItem(2, "初级护士");
    printMenuItem(3, "高级护士");
    printMenuItem(4, "护士长");
    printMenuItem(0, "返回上级菜单");
    printMenuBottom();

    int choice = selectIntCheck(0, 4);
    return choice;
}

// 管理员护士信息修改菜单
int adminNurseModificationMenu()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("修改护士信息");
    printMenuBottom();
    printMenuItem(1, "护士姓名");
    printMenuItem(2, "护士性别");
    printMenuItem(3, "护士年龄");
    printMenuItem(4, "护士所属科室");
    printMenuItem(5, "护士职称");
    printMenuItem(6, "护士联系电话");
    printMenuItem(7, "护士邮箱地址");
    printMenuItem(8, "护士在岗状态");
    printMenuItem(9, "护士排班信息");
    printMenuItem(10, "累计护理患者数量");
    printMenuItem(11, "床位管理次数");
    printMenuItem(0, "返回上级菜单");
    printMenuBottom();

    int choice = selectIntCheck(0, 11);
    return choice;
}

// 管理员药剂师信息管理菜单
int adminPharmacistManagementMenu()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("药剂师账户管理");
    printMenuBottom();
    printMenuItem(1, "查看药剂师信息");
    printMenuItem(2, "修改药剂师信息");
    printMenuItem(3, "删除药剂师账户");
    printMenuItem(4, "添加药剂师账户");
    printMenuItem(0, "返回上级菜单");
    printMenuBottom();

    int choice = selectIntCheck(0, 4);
    return choice;
}

// 管理员药剂师信息查看方式选择菜单
int adminPharmacistViewMenu()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("药剂师信息查看");
    printMenuBottom();
    printMenuItem(1, "查看所有药剂师信息");
    printMenuItem(2, "根据药剂师ID查看");
    printMenuItem(3, "根据药剂师姓名查看");
    printMenuItem(4, "根据在岗状态查看");
    printMenuItem(5, "根据职称查看");
    printMenuItem(0, "返回上级菜单");
    printMenuBottom();

    int viewChoice = selectIntCheck(0, 5);
    return viewChoice;
}

// 药剂师职称选择菜单
int PharmacistTitleMenu()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("药剂师职称选择");
    printMenuBottom();
    printMenuItem(1, "实习药剂师");
    printMenuItem(2, "初级药剂师");
    printMenuItem(3, "高级药剂师");
    printMenuItem(4, "主管药剂师");
    printMenuItem(0, "返回上级菜单");
    printMenuBottom();

    int choice = selectIntCheck(0, 4);
    return choice;
}

// 管理员药剂师信息修改菜单
int adminPharmacistModificationMenu()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("修改药剂师信息");
    printMenuBottom();
    printMenuItem(1, "药剂师姓名");
    printMenuItem(2, "药剂师性别");
    printMenuItem(3, "药剂师年龄");
    printMenuItem(4, "药剂师所属科室");
    printMenuItem(5, "药剂师职称");
    printMenuItem(6, "药剂师联系电话");
    printMenuItem(7, "药剂师邮箱地址");
    printMenuItem(8, "药剂师在岗状态");
    printMenuItem(9, "药剂师排班信息");
    printMenuItem(10, "累计审核用药记录数量");
    printMenuItem(11, "累计发药记录数量");
    printMenuItem(12, "库存管理次数");
    printMenuItem(0, "返回上级菜单");
    printMenuBottom();

    int choice = selectIntCheck(0, 12);
    return choice;
}

// 管理员患者信息管理菜单
int adminPatientManagementMenu()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("患者账户管理");
    printMenuBottom();
    printMenuItem(1, "查看患者信息");
    printMenuItem(2, "修改患者信息");
    printMenuItem(3, "删除患者账户");
    printMenuItem(4, "添加患者账户");
    printMenuItem(0, "返回上级菜单");
    printMenuBottom();

    int choice = selectIntCheck(0, 4);
    return choice;
}

// 管理员患者信息查看方式选择菜单
int adminPatientViewMenu()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("患者信息查看");
    printMenuBottom();
    printMenuItem(1, "查看所有患者信息");
    printMenuItem(2, "根据患者ID查看");
    printMenuItem(3, "根据患者姓名查看");
    printMenuItem(4, "根据患者身份证号查看");
    printMenuItem(5, "根据患者性别查看");
    printMenuItem(6, "根据患者年龄段查看");
    printMenuItem(7, "根据联系方式查看");
    printMenuItem(8, "根据住院状态查看");
    printMenuItem(9, "根据家庭住址查看");
    printMenuItem(10, "根据婚姻状况查看");
    printMenuItem(0, "返回上级菜单");
    printMenuBottom();

    int viewChoice = selectIntCheck(0, 10);
    return viewChoice;
}

// 患者婚姻状况选择菜单
int PatientMaritalStatusMenu()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("患者婚姻状况");
    printMenuBottom();
    printMenuItem(1, "未婚");
    printMenuItem(2, "已婚");
    printMenuItem(3, "离异");
    printMenuItem(4, "丧偶");
    printMenuItem(0, "返回上级菜单");
    printMenuBottom();

    int choice = selectIntCheck(0, 4);
    return choice;
}

// 管理员患者信息修改菜单
int adminPatientModificationMenu()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("修改患者信息");
    printMenuBottom();
    printMenuItem(1, "患者姓名");
    printMenuItem(2, "患者性别");
    printMenuItem(3, "患者年龄");
    printMenuItem(4, "就诊科室");
    printMenuItem(5, "患者身份证号");
    printMenuItem(6, "患者联系电话");
    printMenuItem(7, "患者邮箱地址");
    printMenuItem(8, "患者家庭住址");
    printMenuItem(9, "患者婚姻状况");
    printMenuItem(10, "紧急联系人信息");
    printMenuItem(11, "累计挂号次数");
    printMenuItem(12, "累计看诊次数");
    printMenuItem(13, "累计住院次数");
    printMenuItem(14, "累计用药次数");
    printMenuItem(0, "返回上级菜单");
    printMenuBottom();

    int choice = selectIntCheck(0, 14);
    return choice;
}

// 管理员信息管理菜单
int adminAdminManagementMenu()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("管理员账户管理");
    printMenuBottom();
    printMenuItem(1, "查看管理员信息");
    printMenuItem(2, "修改管理员信息");
    printMenuItem(3, "删除管理员账户");
    printMenuItem(4, "添加管理员账户");
    printMenuItem(0, "返回上级菜单");
    printMenuBottom();

    int choice = selectIntCheck(0, 4);
    return choice;
}

// 管理员信息查看方式选择菜单
int adminAdminViewMenu()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("管理员信息查看");
    printMenuBottom();
    printMenuItem(1, "查看所有管理员信息");
    printMenuItem(2, "根据管理员ID查看");
    printMenuItem(3, "根据管理员姓名查看");
    printMenuItem(4, "根据性别查看");
    printMenuItem(5, "根据年龄段查看");
    printMenuItem(6, "根据联系方式查看");
    printMenuItem(0, "返回上级菜单");
    printMenuBottom();

    int viewChoice = selectIntCheck(0, 6);
    return viewChoice;
}

// 管理员信息修改菜单
int adminAdminModificationMenu()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("修改管理员信息");
    printMenuBottom();
    printMenuItem(1, "管理员姓名");
    printMenuItem(2, "管理员性别");
    printMenuItem(3, "管理员年龄");
    printMenuItem(4, "管理员联系电话");
    printMenuItem(5, "管理员邮箱地址");
    printMenuItem(0, "返回上级菜单");
    printMenuBottom();

    int choice = selectIntCheck(0, 5);
    return choice;
}

// 管理员个人信息管理菜单
int adminPersonalInfoManagementMenu()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("管理员个人信息管理");
    printMenuBottom();
    printMenuItem(1, "查看个人信息");
    printMenuItem(2, "修改个人信息");
    printMenuItem(0, "返回上级菜单");
    printMenuBottom();

    int choice = selectIntCheck(0, 2);
    return choice;
}

// 管理员个人信息查看菜单
int adminPersonalInfoViewMenu()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("查看个人信息");
    printMenuBottom();
    printMenuItem(1, "ID");
    printMenuItem(2, "姓名");
    printMenuItem(3, "性别");
    printMenuItem(4, "年龄");
    printMenuItem(5, "联系电话");
    printMenuItem(6, "邮箱地址");
    printMenuItem(7, "账号创建时间");
    printMenuItem(0, "返回上级菜单");
    printMenuBottom();

    int viewChoice = selectIntCheck(0, 7);
    return viewChoice;
}

// 管理员个人信息修改菜单
int adminPersonalInfoModificationMenu()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("修改个人信息");
    printMenuBottom();
    printMenuItem(1, "姓名");
    printMenuItem(2, "性别");
    printMenuItem(3, "年龄");
    printMenuItem(4, "联系电话");
    printMenuItem(5, "邮箱地址");
    printMenuItem(6, "账号密码");
    printMenuItem(0, "返回上级菜单");
    printMenuBottom();

    int modificationChoice = selectIntCheck(0, 6);
    return modificationChoice;
}

// 医生管理菜单
int doctorMenu()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("医生功能界面");
    printMenuBottom();
    printMenuItem(1, "管理挂号记录");
    printMenuItem(2, "管理看诊记录");
    printMenuItem(3, "管理检查记录");
    printMenuItem(4, "个人信息管理");
    printMenuItem(0, "退出登录");
    printMenuBottom();

    int choice = selectIntCheck(0, 4);
    return choice;
}

// 医生挂号记录管理菜单
int doctorRegistrationManagementMenu()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("挂号记录管理");
    printMenuBottom();
    printMenuItem(1, "查看挂号记录");
    printMenuItem(2, "修改挂号记录状态");
    printMenuItem(3, "删除挂号记录");
    printMenuItem(4, "添加挂号记录");
    printMenuItem(0, "返回上级菜单");
    printMenuBottom();

    int choice = selectIntCheck(0, 4);
    return choice;
}

// 医生挂号记录查看方式选择菜单
int doctorRegistrationViewMenu()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("挂号记录查看");
    printMenuBottom();
    printMenuItem(1, "查看该医生的所有挂号记录");
    printMenuItem(2, "根据患者ID查看");
    printMenuItem(3, "根据挂号记录ID查看");
    printMenuItem(4, "根据时间范围查看");
    printMenuItem(5, "根据挂号状态查看");
    printMenuItem(0, "返回上级菜单");
    printMenuBottom();

    int viewChoice = selectIntCheck(0, 5);
    return viewChoice;
}

// 医生看诊记录管理菜单
int doctorConsultationManagementMenu()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("看诊记录管理");
    printMenuBottom();
    printMenuItem(1, "查看看诊记录");
    printMenuItem(2, "修改看诊记录");
    printMenuItem(3, "删除看诊记录");
    printMenuItem(4, "添加看诊记录");
    printMenuItem(0, "返回上级菜单");
    printMenuBottom();

    int choice = selectIntCheck(0, 4);
    return choice;
}

// 医生看诊记录查看方式选择菜单
int doctorConsultationViewMenu()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("看诊记录查看");
    printMenuBottom();
    printMenuItem(1, "查看该医生的所有看诊记录");
    printMenuItem(2, "根据患者ID查看");
    printMenuItem(3, "根据看诊记录ID查看");
    printMenuItem(4, "根据时间范围查看");
    printMenuItem(5, "根据看诊状态查看");
    printMenuItem(0, "返回上级菜单");
    printMenuBottom();

    int viewChoice = selectIntCheck(0, 5);
    return viewChoice;
}

// 医生看诊记录修改菜单
int doctorConsultationModificationMenu()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("修改看诊记录");
    printMenuBottom();
    printMenuItem(1, "看诊状态");
    printMenuItem(2, "主诉");
    printMenuItem(3, "现病史");
    printMenuItem(4, "既往史");
    printMenuItem(5, "家族史");
    printMenuItem(6, "初步诊断");
    printMenuItem(7, "添加检查项目");
    printMenuItem(8, "添加处方信息");
    printMenuItem(9, "开具住院证");
    printMenuItem(0, "返回上级菜单");
    printMenuBottom();

    int choice = selectIntCheck(0, 9);
    return choice;
}

// 医生检查记录管理菜单
int doctorExaminationManagementMenu()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("检查记录管理");
    printMenuBottom();
    printMenuItem(1, "查看检查记录");
    printMenuItem(2, "修改检查记录");
    printMenuItem(3, "删除检查记录");
    printMenuItem(4, "添加检查记录");
    printMenuItem(0, "返回上级菜单");
    printMenuBottom();

    int choice = selectIntCheck(0, 4);
    return choice;
}

// 医生检查记录查看方式选择菜单
int doctorExaminationViewMenu()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("检查记录查看");
    printMenuBottom();
    printMenuItem(1, "查看该医生的所有检查记录");
    printMenuItem(2, "根据患者ID查看");
    printMenuItem(3, "根据看诊记录ID查看");
    printMenuItem(4, "根据检查记录ID查看");
    printMenuItem(5, "根据时间范围查看");
    printMenuItem(6, "根据检查项目查看");
    printMenuItem(7, "根据检查记录状态查看");
    printMenuItem(0, "返回上级菜单");
    printMenuBottom();

    int viewChoice = selectIntCheck(0, 7);
    return viewChoice;
}

// 医生检查记录修改菜单
int doctorExaminationModificationMenu()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("修改检查记录");
    printMenuBottom();
    printMenuItem(1, "检查记录状态");
    printMenuItem(2, "检查项目");
    printMenuItem(3, "检查结果报告");
    printMenuItem(4, "检查结果报告摘要");
    printMenuItem(5, "修改检查报告附件文件");
    printMenuItem(6, "添加检查记录备注信息");
    printMenuItem(0, "返回上级菜单");
    printMenuBottom();

    int choice = selectIntCheck(0, 6);
    return choice;
}

// 医生个人信息管理菜单
int doctorPersonalInfoManagementMenu()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("个人信息管理");
    printMenuBottom();
    printMenuItem(1, "查看个人信息");
    printMenuItem(2, "修改个人信息");
    printMenuItem(0, "返回上级菜单");
    printMenuBottom();

    int choice = selectIntCheck(0, 2);
    return choice;
}

// 医生个人信息查看选择菜单
int doctorPersonalInfoViewMenu()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("查看个人信息");
    printMenuBottom();
    printMenuItem(1, "ID");
    printMenuItem(2, "姓名");
    printMenuItem(3, "性别");
    printMenuItem(4, "年龄");
    printMenuItem(5, "所属科室");
    printMenuItem(6, "职称");
    printMenuItem(7, "联系电话");
    printMenuItem(8, "邮箱地址");
    printMenuItem(9, "在岗状态");
    printMenuItem(10, "擅长方向");
    printMenuItem(11, "排班信息");
    printMenuItem(12, "累计接诊患者数量");
    printMenuItem(13, "累计完成的检查数量");
    printMenuItem(14, "累计开具住院证的数量");
    printMenuItem(15, "账户创建时间");
    printMenuItem(0, "返回上级菜单");
    printMenuBottom();

    int viewChoice = selectIntCheck(0, 15);
    return viewChoice;
}

// 医生个人信息修改菜单
int doctorPersonalInfoModificationMenu()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("修改个人信息");
    printMenuBottom();
    printMenuItem(1, "姓名");
    printMenuItem(2, "性别");
    printMenuItem(3, "年龄");
    printMenuItem(4, "所属科室");
    printMenuItem(5, "职称");
    printMenuItem(6, "联系电话");
    printMenuItem(7, "邮箱地址");
    printMenuItem(8, "在岗状态");
    printMenuItem(9, "擅长方向");
    printMenuItem(10, "排班信息");
    printMenuItem(11, "账号密码");
    printMenuItem(0, "返回上级菜单");
    printMenuBottom();

    int choice = selectIntCheck(0, 11);
    return choice;
}

// 患者管理菜单
int patientMenu()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("患者功能界面");
    printMenuBottom();
    printMenuItem(1, "挂号信息管理");
    printMenuItem(2, "看诊信息管理");
    printMenuItem(3, "检查信息管理");
    printMenuItem(4, "用药信息管理");
    printMenuItem(5, "住院信息管理");
    printMenuItem(6, "个人信息管理");
    printMenuItem(0, "退出登录");
    printMenuBottom();

    int choice = selectIntCheck(0, 6);
    return choice;
}

// 患者挂号信息管理菜单
int patientRegistrationManagementMenu()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("挂号信息管理");
    printMenuBottom();
    printMenuItem(1, "查看挂号信息");
    printMenuItem(2, "添加挂号信息");
    printMenuItem(3, "撤回挂号申请");
    printMenuItem(4, "缴纳挂号费用");
    printMenuItem(0, "返回上级菜单");
    printMenuBottom();

    int choice = selectIntCheck(0, 4);
    return choice;
}

// 患者挂号信息查看方式选择菜单
int patientRegistrationViewMenu()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("挂号信息查看");
    printMenuBottom();
    printMenuItem(1, "查看所有挂号信息");
    printMenuItem(2, "根据挂号记录ID查看");
    printMenuItem(3, "根据医生ID查看");
    printMenuItem(4, "根据挂号状态查看");
    printMenuItem(5, "根据挂号时间范围查看");
    printMenuItem(6, "根据挂号科室查看");
    printMenuItem(0, "返回上级菜单");
    printMenuBottom();

    int viewChoice = selectIntCheck(0, 6);
    return viewChoice;
}

// 患者看诊信息管理菜单
int patientConsultationManagementMenu()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("看诊信息管理");
    printMenuBottom();
    printMenuItem(1, "查看看诊信息");
    printMenuItem(0, "返回上级菜单");
    printMenuBottom();

    int choice = selectIntCheck(0, 1);
    return choice;
}

// 患者看诊信息查看方式选择菜单
int patientConsultationViewMenu()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("看诊信息查看");
    printMenuBottom();
    printMenuItem(1, "查看所有看诊信息");
    printMenuItem(2, "根据看诊记录ID查看");
    printMenuItem(3, "根据医生ID查看");
    printMenuItem(4, "根据看诊状态查看");
    printMenuItem(5, "根据看诊时间范围查看");
    printMenuItem(6, "根据主诉内容关键词查看");
    printMenuItem(7, "根据看诊科室查看");
    printMenuItem(0, "返回上级菜单");
    printMenuBottom();

    int viewChoice = selectIntCheck(0, 7);
    return viewChoice;
}

// 患者检查信息管理菜单
int patientExaminationManagementMenu()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("检查信息管理");
    printMenuBottom();
    printMenuItem(1, "查看检查信息");
    printMenuItem(2, "缴纳检查费用");
    printMenuItem(0, "返回上级菜单");
    printMenuBottom();

    int choice = selectIntCheck(0, 2);
    return choice;
}

// 患者检查信息查看方式选择菜单
int patientExaminationViewMenu()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("检查信息查看");
    printMenuBottom();
    printMenuItem(1, "查看所有检查信息");
    printMenuItem(2, "根据检查记录ID查看");
    printMenuItem(3, "根据看诊记录ID查看");
    printMenuItem(4, "根据检查项目查看");
    printMenuItem(5, "根据检查状态查看");
    printMenuItem(6, "根据检查时间范围查看");
    printMenuItem(7, "根据检查科室查看");
    printMenuItem(0, "返回上级菜单");
    printMenuBottom();

    int viewChoice = selectIntCheck(0, 7);
    return viewChoice;
}

// 患者用药信息管理菜单
int patientMedicationManagementMenu()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("用药信息管理");
    printMenuBottom();
    printMenuItem(1, "查看用药信息");
    printMenuItem(2, "缴纳药品费用");
    printMenuItem(0, "返回上级菜单");
    printMenuBottom();

    int choice = selectIntCheck(0, 2);
    return choice;
}

// 患者用药信息查看方式选择菜单
int patientMedicationViewMenu()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("用药信息查看");
    printMenuBottom();
    printMenuItem(1, "查看所有用药信息");
    printMenuItem(2, "根据用药记录ID查看");
    printMenuItem(3, "根据看诊记录ID查看");
    printMenuItem(4, "根据药品名称查看");
    printMenuItem(5, "根据用药状态查看");
    printMenuItem(6, "根据用药时间范围查看");
    printMenuItem(7, "根据用药科室查看");
    printMenuItem(0, "返回上级菜单");
    printMenuBottom();

    int viewChoice = selectIntCheck(0, 7);
    return viewChoice;
}

// 患者住院信息管理菜单
int patientHospitalizationManagementMenu()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("住院信息管理");
    printMenuBottom();
    printMenuItem(1, "查看住院信息");
    printMenuItem(2, "申请出院");
    printMenuItem(3, "缴纳住院费用");
    printMenuItem(0, "返回上级菜单");
    printMenuBottom();

    int choice = selectIntCheck(0, 3);
    return choice;
}

// 患者住院信息查看方式选择菜单
int patientHospitalizationViewMenu()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("住院信息查看");
    printMenuBottom();
    printMenuItem(1, "查看所有住院信息");
    printMenuItem(2, "根据住院记录ID查看");
    printMenuItem(3, "根据入院时间范围查看");
    printMenuItem(4, "根据出院时间范围查看");
    printMenuItem(5, "根据住院状态查看");
    printMenuItem(6, "根据住院科室查看");
    printMenuItem(7, "根据病房类型查看");
    printMenuItem(8, "根据床位号查看");
    printMenuItem(9, "根据主治医生ID查看");
    printMenuItem(10, "根据看诊记录ID查看");
    printMenuItem(0, "返回上级菜单");
    printMenuBottom();

    int viewChoice = selectIntCheck(0, 10);
    return viewChoice;
}

// 患者个人信息管理菜单
int patientPersonalInfoManagementMenu()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("个人信息管理");
    printMenuBottom();
    printMenuItem(1, "查看个人信息");
    printMenuItem(2, "修改个人信息");
    printMenuItem(3, "充值账户余额");
    printMenuItem(0, "返回上级菜单");
    printMenuBottom();

    int choice = selectIntCheck(0, 3);
    return choice;
}

// 患者个人信息查看选择菜单
int patientPersonalInfoViewMenu()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("查看个人信息");
    printMenuBottom();
    printMenuItem(1, "ID");
    printMenuItem(2, "姓名");
    printMenuItem(3, "性别");
    printMenuItem(4, "年龄");
    printMenuItem(5, "身份证号");
    printMenuItem(6, "联系电话");
    printMenuItem(7, "邮箱地址");
    printMenuItem(8, "家庭住址");
    printMenuItem(9, "紧急联系人姓名");
    printMenuItem(10, "紧急联系人电话");
    printMenuItem(11, "过敏史");
    printMenuItem(12, "既往病史");
    printMenuItem(13, "婚姻状况");
    printMenuItem(14, "账户余额");
    printMenuItem(15, "是否住院中");
    printMenuItem(16, "累计挂号次数");
    printMenuItem(17, "累计就诊次数");
    printMenuItem(18, "累计住院次数");
    printMenuItem(19, "累计用药次数");
    printMenuItem(20, "账户创建时间");
    printMenuItem(0, "返回上级菜单");
    printMenuBottom();

    int viewChoice = selectIntCheck(0, 20);
    return viewChoice;
}

// 患者个人信息修改菜单
int patientPersonalInfoModificationMenu()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("修改个人信息");
    printMenuBottom();
    printMenuItem(1, "姓名");
    printMenuItem(2, "性别");
    printMenuItem(3, "年龄");
    printMenuItem(4, "身份证号");
    printMenuItem(5, "联系电话");
    printMenuItem(6, "邮箱地址");
    printMenuItem(7, "家庭住址");
    printMenuItem(8, "紧急联系人姓名");
    printMenuItem(9, "紧急联系人电话");
    printMenuItem(10, "过敏史");
    printMenuItem(11, "既往病史");
    printMenuItem(12, "婚姻状况");
    printMenuItem(13, "账号密码");
    printMenuItem(0, "返回上级菜单");
    printMenuBottom();

    int choice = selectIntCheck(0, 13);
    return choice;
}

// 药剂师管理菜单
int pharmacistMenu()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("药剂师功能界面");
    printMenuBottom();
    printMenuItem(1, "处方审核");
    printMenuItem(2, "管理用药信息");
    printMenuItem(3, "管理药品信息");
    printMenuItem(4, "个人信息管理");
    printMenuItem(0, "退出登录");
    printMenuBottom();

    int choice = selectIntCheck(0, 4);
    return choice;
}

// 药剂师用药信息管理菜单
int pharmacistMedicationRecordManagementMenu()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("用药信息管理");
    printMenuBottom();
    printMenuItem(1, "查看用药信息");
    printMenuItem(2, "修改用药信息");
    printMenuItem(3, "发药");
    printMenuItem(4, "删除用药记录");
    printMenuItem(5, "添加用药记录");
    printMenuItem(0, "返回上级菜单");
    printMenuBottom();

    int choice = selectIntCheck(0, 5);
    return choice;
}

// 药剂师用药信息查看方式选择菜单
int pharmacistMedicationRecordViewMenu()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("用药信息查看");
    printMenuBottom();
    printMenuItem(1, "查看所有用药信息");
    printMenuItem(2, "根据用药记录ID查看");
    printMenuItem(3, "根据看诊记录ID查看");
    printMenuItem(4, "根据患者ID查看");
    printMenuItem(5, "根据审核状态查看");
    printMenuItem(6, "根据用药状态查看");
    printMenuItem(7, "根据用药时间范围查看");
    printMenuItem(8, "根据药品名称查看");
    printMenuItem(0, "返回上级菜单");
    printMenuBottom();

    int viewChoice = selectIntCheck(0, 8);
    return viewChoice;
}

// 药剂师用药信息修改菜单
int pharmacistMedicationRecordModificationMenu()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("修改用药信息");
    printMenuBottom();
    printMenuItem(1, "用药状态");
    printMenuItem(2, "审核状态");
    printMenuItem(3, "管理药品明细");
    printMenuItem(4, "修改用药记录备注信息");
    printMenuItem(0, "返回上级菜单");
    printMenuBottom();

    int choice = selectIntCheck(0, 4);
    return choice;
}

// 药剂师药品信息管理菜单
int pharmacistMedicineManagementMenu()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("药品信息管理");
    printMenuBottom();
    printMenuItem(1, "查看药品信息");
    printMenuItem(2, "修改药品信息");
    printMenuItem(3, "删除药品信息");
    printMenuItem(4, "添加药品信息");
    printMenuItem(5, "管理药品库存");
    printMenuItem(0, "返回上级菜单");
    printMenuBottom();

    int choice = selectIntCheck(0, 5);
    return choice;
}

// 药剂师药品信息查看方式选择菜单
int pharmacistMedicineViewMenu()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("药品信息查看");
    printMenuBottom();
    printMenuItem(1, "查看所有药品信息");
    printMenuItem(2, "根据药品ID查看");
    printMenuItem(3, "根据药品名称查看");
    printMenuItem(4, "根据科室查看");
    printMenuItem(5, "根据药品状态查看");
    printMenuItem(0, "返回上级菜单");
    printMenuBottom();

    int viewChoice = selectIntCheck(0, 5);
    return viewChoice;
}

// 药剂师药品信息修改菜单
int pharmacistMedicineModificationMenu()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("修改药品信息");
    printMenuBottom();
    printMenuItem(1, "药品名称");
    printMenuItem(2, "药品规格");
    printMenuItem(3, "药品生产厂家");
    printMenuItem(4, "药品进价");
    printMenuItem(5, "药品售价");
    printMenuItem(6, "安全库存阈值");
    printMenuItem(7, "生产日期");
    printMenuItem(8, "有效期");
    printMenuItem(9, "科室");
    printMenuItem(10, "药品状态");
    printMenuItem(11, "修改药品备注信息");
    printMenuItem(0, "返回上级菜单");
    printMenuBottom();

    int choice = selectIntCheck(0, 11);
    return choice;
}

// 药剂师个人信息管理菜单
int pharmacistPersonalInfoManagementMenu()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("个人信息管理");
    printMenuBottom();
    printMenuItem(1, "查看个人信息");
    printMenuItem(2, "修改个人信息");
    printMenuItem(0, "返回上级菜单");
    printMenuBottom();

    int choice = selectIntCheck(0, 2);
    return choice;
}

// 药剂师个人信息查看选择菜单
int pharmacistPersonalInfoViewMenu()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("查看个人信息");
    printMenuBottom();
    printMenuItem(1, "ID");
    printMenuItem(2, "姓名");
    printMenuItem(3, "性别");
    printMenuItem(4, "年龄");
    printMenuItem(5, "所属科室");
    printMenuItem(6, "职称");
    printMenuItem(7, "联系电话");
    printMenuItem(8, "邮箱地址");
    printMenuItem(9, "在岗状态");
    printMenuItem(10, "排班信息");
    printMenuItem(11, "累计审核用药记录数量");
    printMenuItem(12, "累计发药数量");
    printMenuItem(13, "累计管理药品库存次数");
    printMenuItem(14, "账户创建时间");
    printMenuItem(0, "返回上级菜单");
    printMenuBottom();

    int viewChoice = selectIntCheck(0, 14);
    return viewChoice;
}

// 药剂师个人信息修改菜单
int pharmacistPersonalInfoModificationMenu()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("修改个人信息");
    printMenuBottom();
    printMenuItem(1, "姓名");
    printMenuItem(2, "性别");
    printMenuItem(3, "年龄");
    printMenuItem(4, "所属科室");
    printMenuItem(5, "职称");
    printMenuItem(6, "联系电话");
    printMenuItem(7, "邮箱地址");
    printMenuItem(8, "在岗状态");
    printMenuItem(9, "排班信息");
    printMenuItem(10, "账号密码");
    printMenuItem(0, "返回上级菜单");
    printMenuBottom();

    int choice = selectIntCheck(0, 10);
    return choice;
}

// 护士管理菜单
int nurseMenu()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("护士功能界面");
    printMenuBottom();
    printMenuItem(1, "管理住院信息");
    printMenuItem(2, "管理检查信息");
    printMenuItem(3, "管理床位信息");
    printMenuItem(4, "护理记录管理");
    printMenuItem(5, "个人信息管理");
    printMenuItem(0, "退出登录");
    printMenuBottom();

    int choice = selectIntCheck(0, 5);
    return choice;
}

// 护士检查信息管理菜单
int nurseExaminationManagementMenu()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("检查信息管理");
    printMenuBottom();
    printMenuItem(1, "查看检查信息");
    printMenuItem(2, "修改检查信息");
    printMenuItem(3, "删除检查信息");
    printMenuItem(0, "返回上级菜单");
    printMenuBottom();

    int choice = selectIntCheck(0, 3);
    return choice;
}

// 护士检查信息查看方式选择菜单
int nurseExaminationViewMenu()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("检查记录查看");
    printMenuBottom();
    printMenuItem(1, "查看本科室全部检查记录");
    printMenuItem(2, "按患者ID查询");
    printMenuItem(3, "按看诊ID查询");
    printMenuItem(4, "按检查ID查询");
    printMenuItem(5, "按时间范围查询");
    printMenuItem(6, "按检查项目查询");
    printMenuItem(7, "按状态查询");
    printMenuItem(0, "返回上级菜单");
    printMenuBottom();

    int viewChoice = selectIntCheck(0, 7);
    return viewChoice;
}

// 护士检查信息修改菜单
int nurseExaminationModificationMenu()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("修改检查记录");
    printMenuBottom();
    printMenuItem(1, "修改检查状态");
    printMenuItem(2, "录入/修改检查结果");
    printMenuItem(3, "修改报告摘要");
    printMenuItem(4, "管理附件");
    printMenuItem(5, "修改备注");
    printMenuItem(0, "返回上级菜单");
    printMenuBottom();

    int choice = selectIntCheck(0, 5);
    return choice;
}


// 护士住院信息管理菜单
int nurseHospitalizationManagementMenu()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("住院信息管理");
    printMenuBottom();
    printMenuItem(1, "查看住院信息");
    printMenuItem(2, "修改住院信息");
    printMenuItem(3, "删除住院信息");
    printMenuItem(4, "添加住院信息");
    printMenuItem(0, "返回上级菜单");
    printMenuBottom();

    int choice = selectIntCheck(0, 4);
    return choice;
}

// 护士住院信息查看方式选择菜单
int nurseHospitalizationViewMenu()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("住院信息查看");
    printMenuBottom();
    printMenuItem(1, "查看所有住院信息");
    printMenuItem(2, "根据住院记录ID查看");
    printMenuItem(3, "根据患者ID查看");
    printMenuItem(4, "根据入院时间范围查看");
    printMenuItem(5, "根据出院时间范围查看");
    printMenuItem(6, "根据住院状态查看");
    printMenuItem(7, "根据病房类型查看");
    printMenuItem(8, "根据床位号查看");
    printMenuItem(9, "根据主治医生ID查看");
    printMenuItem(10, "根据看诊记录ID查看");
    printMenuItem(0, "返回上级菜单");
    printMenuBottom();

    int viewChoice = selectIntCheck(0, 10);
    return viewChoice;
}

// 护士住院信息修改菜单
int nurseHospitalizationModificationMenu()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("修改住院信息");
    printMenuBottom();
    printMenuItem(1, "住院状态");
    printMenuItem(2, "病房类型");
    printMenuItem(3, "分配护士");
    printMenuItem(4, "分配床位");
    printMenuItem(5, "调整床位");
    printMenuItem(6, "办理出院");
    printMenuItem(7, "设置押金金额");
    printMenuItem(0, "返回上级菜单");
    printMenuBottom();

    int choice = selectIntCheck(0, 7);
    return choice;
}

// 护士床位信息管理菜单
int nurseBedManagementMenu()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("床位信息管理");
    printMenuBottom();
    printMenuItem(1, "查看床位信息");
    printMenuItem(2, "修改床位信息");
    printMenuItem(3, "删除床位信息");
    printMenuItem(4, "新增床位信息");
    printMenuItem(0, "返回上级菜单");
    printMenuBottom();

    int choice = selectIntCheck(0, 4);
    return choice;
}

// 护士床位信息查看方式选择菜单
int nurseBedViewMenu()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("床位信息查看");
    printMenuBottom();
    printMenuItem(1, "查看本科室全部床位");
    printMenuItem(2, "按床位状态查询");
    printMenuItem(3, "按患者ID查询");
    printMenuItem(4, "按护士ID查询");
    printMenuItem(5, "按科室查询");
    printMenuItem(6, "按床位ID查询");
    printMenuItem(7, "按病房类型查询");
    printMenuItem(0, "返回上级菜单");
    printMenuBottom();

    int viewChoice = selectIntCheck(0, 7);
    return viewChoice;
}

// 护士床位信息修改菜单
int nurseBedModificationMenu()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("修改床位信息");
    printMenuBottom();
    printMenuItem(1, "修改床位状态");
    printMenuItem(2, "修改备注");
    printMenuItem(3, "更新生命体征");
    printMenuItem(0, "返回上级菜单");
    printMenuBottom();

    int choice = selectIntCheck(0, 3);
    return choice;
}

// 护士个人信息管理菜单
int nursePersonalInfoManagementMenu()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("个人信息管理");
    printMenuBottom();
    printMenuItem(1, "查看个人信息");
    printMenuItem(2, "修改个人信息");
    printMenuItem(0, "返回上级菜单");
    printMenuBottom();

    int choice = selectIntCheck(0, 2);
    return choice;
}

// 护士个人信息查看选择菜单
int nursePersonalInfoViewMenu()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("查看个人信息");
    printMenuBottom();
    printMenuItem(1, "护士ID");
    printMenuItem(2, "姓名");
    printMenuItem(3, "性别");
    printMenuItem(4, "年龄");
    printMenuItem(5, "科室");
    printMenuItem(6, "护士等级");
    printMenuItem(7, "联系电话");
    printMenuItem(8, "邮箱");
    printMenuItem(9, "在岗状态");
    printMenuItem(10, "排班信息");
    printMenuItem(11, "累计护理人数");
    printMenuItem(12, "累计床位管理次数");
    printMenuItem(13, "账户创建时间");
    printMenuItem(0, "返回上级菜单");
    printMenuBottom();

    int viewChoice = selectIntCheck(0, 13);
    return viewChoice;
}

// 护士个人信息修改菜单
int nursePersonalInfoModificationMenu()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("修改个人信息");
    printMenuBottom();
    printMenuItem(1, "修改姓名");
    printMenuItem(2, "修改性别");
    printMenuItem(3, "修改年龄");
    printMenuItem(4, "修改科室");
    printMenuItem(5, "修改护士等级");
    printMenuItem(6, "修改联系电话");
    printMenuItem(7, "修改邮箱");
    printMenuItem(8, "修改在岗状态");
    printMenuItem(9, "修改排班信息");
    printMenuItem(10, "修改密码");
    printMenuItem(0, "返回上级菜单");
    printMenuBottom();

    int choice = selectIntCheck(0, 10);
    return choice;
}

int adminReportMenu()
{
    clearScreen();
    printMenuBorder();
    printMenuTitle("统计报表");
    printMenuBottom();
    printMenuItem(1, "科室统计总览");
    printMenuItem(2, "医生工作量统计");
    printMenuItem(3, "患者就诊统计");
    printMenuItem(4, "床位使用率统计");
    printMenuItem(5, "药品库存统计");
    printMenuItem(0, "返回上级菜单");
    printMenuBottom();

    int choice = selectIntCheck(0, 5);
    return choice;
}

// ======================================== 共享记录显示函数 =======================================

// 打印看诊记录卡片（含检查项目列表和处方列表）
// 格式与 Patient.cpp 中 getAllConsultations 等函数的输出一致
void printConsultationCard(const Consultation *con)
{
    User u; // 用于调用状态转换函数（这些函数不依赖实例状态）

    std::cout << "看诊ID: " << con->consultationID
              << ", 挂号ID: " << con->registrationID
              << ", 医生ID: " << con->doctorID
              << ", 科室: " << con->department
              << ", 时间: " << con->consultationTime
              << ", 状态: " << u.conStatusToString(con->status)
              << ", 主诉: " << con->chiefComplaint
              << ", 现病史: " << con->historyOfPresentIllness
              << ", 既往史: " << con->pastMedicalHistory
              << ", 家族史: " << con->familyHistory
              << ", 初步诊断: " << con->preliminaryDiagnosis
              << ", 检查项目数: " << con->examinationlist.size()
              << ", 处方数: " << con->prescriptions.size()
              << ", 处方审核状态: " << (con->isPrescriptionReviewed ? "已审核" : "未审核")
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
}

// 打印检查记录卡片（含附件信息）
// 格式与 Doctor.cpp / Nurse.cpp 中检查记录的输出一致
void printExaminationCard(const Examination *exa)
{
    User u; // 用于调用状态转换函数

    std::cout << "检查ID: " << exa->examinationID
              << ", 看诊ID: " << exa->consultationID
              << ", 患者ID: " << exa->patientID
              << ", 医生ID: " << exa->doctorID
              << ", 科室: " << exa->department
              << ", 开单时间: " << exa->orderTime
              << ", 检查项目: " << exa->itemName
              << ", 检查结果: " << u.findVitalSignToString(const_cast<Examination *>(exa))
              << ", 报告摘要: " << exa->reportSummary
              << ", 检查费用: " << std::fixed << std::setprecision(2) << (exa->fee / 100.0)
              << ", 出报告时间: " << exa->reportTime
              << ", 状态: " << u.examStatusToString(exa->status)
              << ", 备注: " << exa->note;

    std::cout << ", 相关附件: ";
    for (size_t i = 0; i < exa->attachments.size(); ++i)
    {
        std::cout << "[" << (i + 1) << "] " << exa->attachments[i] << "  ";
    }
    std::cout << std::endl;
}

// 打印用药记录卡片（含药品明细，单行紧凑格式）
// 格式与 Pharmacist.cpp / Admin.cpp 中的用药记录输出一致
void printMedicationRecordCard(const MedicationRecord *rec)
{
    User u; // 用于调用状态转换函数

    std::string reviewStatusStr = u.medicationReviewStatusToString(rec->reviewStatus);
    std::string statusStr = u.medicationStatusToString(rec->status);
    std::cout << "用药记录ID: " << rec->medRecordID
              << ", 关联看诊记录ID: " << rec->consultationID
              << ", 患者ID: " << rec->patientID
              << ", 医生ID: " << rec->doctorID
              << ", 药师ID: " << (rec->pharmacistID.empty() ? "无" : rec->pharmacistID)
              << ", 科室: " << rec->department
              << ", 审核状态: " << reviewStatusStr
              << ", 总费用: " << std::fixed << std::setprecision(2) << (rec->totalCost / 100.0)
              << ", 支付时间: " << (rec->paymentTime.empty() ? "未支付" : rec->paymentTime)
              << ", 发药时间: " << (rec->dispenseTime.empty() ? "未发药" : rec->dispenseTime)
              << ", 创建时间: " << rec->createTime
              << ", 备注: " << (rec->note.empty() ? "无" : rec->note)
              << std::endl;

    std::cout << "用药明细: " << std::endl;
    for (const auto &line : rec->lines)
    {
        std::cout << "  - 药品ID: " << line.medicineID
                  << ", 药品名称: " << line.medicineName
                  << ", 数量: " << line.quantity
                  << ", 单价: " << std::fixed << std::setprecision(2) << (line.unitPrice / 100.0)
                  << ", 用法备注: " << (line.note.empty() ? "无" : line.note)
                  << std::endl;
    }
}

// 打印住院记录卡片（含状态字符串）
// 格式与 Nurse.cpp 中 printHospitalizationDetails 的输出一致
void printHospitalizationCard(const Hospitalization *hos)
{
    User u; // 用于调用状态转换函数

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
              << ", 押金: " << std::fixed << std::setprecision(2) << (hos->deposit / 100.0)
              << ", 总费用: " << std::fixed << std::setprecision(2) << (hos->totalCost / 100.0)
              << ", 状态: " << u.hosStatusToString(hos->status)
              << std::endl;
}