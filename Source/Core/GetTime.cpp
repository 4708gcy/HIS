#include "Core/GetTime.h"
#include <iomanip>
#include <sstream>

// 获取 MyTime 类的单例实例
MyTime& MyTime::getInstance() {
    static MyTime instance;  // C++11 保证静态局部变量的线程安全初始化
    return instance;
}

static struct tm getLocalTime() {
    time_t now = std::time(nullptr);
    struct tm local_tm;
#ifdef _WIN32
    localtime_s(&local_tm, &now);
#else
    localtime_r(&now, &local_tm);
#endif
    return local_tm;
}

// 返回当前时间的字符串表示形式，格式为 "YYYY-MM-DD hh:mm:ss"
std::string MyTime::getTime()
{
    struct tm local_tm = getLocalTime();

    char buffer[20]; // "YYYY-MM-DD hh:mm:ss" + '\0' = 20 characters
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &local_tm);

    return std::string(buffer);
}

// 获取当前年份
int MyTime::getYear()
{
    struct tm local_tm = getLocalTime();
    return local_tm.tm_year + 1900; // tm_year 是从1900年开始计算的
}

// 获取当前月份
int MyTime::getMonth()
{
    struct tm local_tm = getLocalTime();
    return local_tm.tm_mon + 1; // tm_mon 的范围是0-11，所以需要+1
}

// 获取当前日期
int MyTime::getDay()
{
    struct tm local_tm = getLocalTime();
    return local_tm.tm_mday; // 日期范围是1-31
}

// 获取当前小时
int MyTime::getHour()
{
    struct tm local_tm = getLocalTime();
    return local_tm.tm_hour; // 小时范围是0-23
}

// 获取当前分钟
int MyTime::getMinute()
{
    struct tm local_tm = getLocalTime();
    return local_tm.tm_min; // 分钟范围是0-59
}

// 获取当前秒钟
int MyTime::getSecond()
{
    struct tm local_tm = getLocalTime();
    return local_tm.tm_sec; // 秒钟范围是0-59
}
