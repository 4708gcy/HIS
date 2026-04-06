/**
 * @file GetTime.h
 * @brief 获取当前时间的函数声明
 * @details 该头文件声明了一个函数，用于获取当前时间的字符串表示形式，格式为 "YYYY-MM-DD hh:mm:ss"
 * @author 郭承宇
 * @date 2026-4-6
 * @version 1.0
*/

#ifndef GETTIME_H
#define GETTIME_H

#include <ctime>
#include <string>

class MyTime{
    public:
        // 获取 MyTime 类的单例实例，确保全局只有一个 MyTime 对象存在
        static MyTime &getInstance();

        // 返回当前时间的字符串表示形式，格式为 "YYYY-MM-DD hh:mm:ss"
        std::string getTime();

        // 获取当前年份
        int getYear();

        // 获取当前月份
        int getMonth();

        // 获取当前日期
        int getDay();

        // 获取当前小时
        int getHour();

        // 获取当前分钟
        int getMinute();

        // 获取当前秒钟
        int getSecond();

    private:
        // 私有构造函数，禁止外部创建 MyTime 对象
        MyTime() = default;

        // 私有拷贝构造函数和赋值运算符，禁止复制 MyTime 对象
        MyTime(const MyTime&) = delete;
        MyTime& operator=(const MyTime&) = delete;
};

#endif // GETTIME_H

