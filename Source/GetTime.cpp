#include "../Head/GetTime.h"
#include <iomanip>
#include <sstream>

// 获取 MyTime 类的单例实例
MyTime& MyTime::getInstance() {
    static MyTime instance;  // C++11 保证静态局部变量的线程安全初始化
    return instance;
}

// 返回当前时间的字符串表示形式，格式为 "YYYY-MM-DD hh:mm:ss"
std::string MyTime::getTime()
{
    time_t now = std::time(nullptr);
    struct tm local_tm = *std::localtime(&now);

    char buffer[20]; // "YYYY-MM-DD hh:mm:ss" + '\0' = 20 characters
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &local_tm);

    return std::string(buffer);
}

// 获取当前年份
int MyTime::getYear()
{
    time_t now = std::time(nullptr);
    struct tm local_tm = *std::localtime(&now);
    return local_tm.tm_year + 1900; // tm_year 是从1900年开始计算的
}

// 获取当前月份
int MyTime::getMonth()
{
    time_t now = std::time(nullptr);
    struct tm local_tm = *std::localtime(&now);
    return local_tm.tm_mon + 1; // tm_mon 的范围是0-11，所以需要+1
}

// 获取当前日期
int MyTime::getDay()
{
    time_t now = std::time(nullptr);
    struct tm local_tm = *std::localtime(&now);
    return local_tm.tm_mday; // 日期范围是1-31
}

// 获取当前小时
int MyTime::getHour()
{
    time_t now = std::time(nullptr);
    struct tm local_tm = *std::localtime(&now);
    return local_tm.tm_hour; // 小时范围是0-23
}

// 获取当前分钟
int MyTime::getMinute()
{
    time_t now = std::time(nullptr);
    struct tm local_tm = *std::localtime(&now);
    return local_tm.tm_min; // 分钟范围是0-59
}

// 获取当前秒钟
int MyTime::getSecond()
{
    time_t now = std::time(nullptr);
    struct tm local_tm = *std::localtime(&now);
    return local_tm.tm_sec; // 秒钟范围是0-59
}

// 测试代码

// #include <iostream>
// int main()
// {
//     MyTime &timeInstance = MyTime::getInstance();

//     std::cout << "完整时间: " << timeInstance.getTime() << std::endl;
//     std::cout << "年: " << timeInstance.getYear() << std::endl;
//     std::cout << "月: " << timeInstance.getMonth() << std::endl;
//     std::cout << "日: " << timeInstance.getDay() << std::endl;
//     std::cout << "时: " << timeInstance.getHour() << std::endl;
//     std::cout << "分: " << timeInstance.getMinute() << std::endl;
//     std::cout << "秒: " << timeInstance.getSecond() << std::endl;

//     return 0;
// }