/**
 * @file InputCheck.h
 * @brief 输入检查函数声明
 * @details 所有输入环节添加类型、范围、长度、格式校验，拦截越界值、非法字符、空值提交，防止程序崩溃与内存泄漏。
 * @author 郭承宇
 * @date 2026-4-6
 * @version 1.0
 */

#ifndef INPUTCHECK_H
#define INPUTCHECK_H

#include <string>
#include <cctype>
#include <cstring>

//-------------------
//以下为标准输入输出的函数声明
//-------------------

//--------------------
//以下为功能函数
bool isCharacter(char ch);                    // 是否为字符
bool isDigit(char ch);                       // 是否为数字
bool isFullCharacter(char str[]);            // 是否都是字符
bool isFullDigit(char str[]);                // 是否都是数字
// 标准输入意味着该字符串只有字母和数字
bool isStdInput(char str[]);                 // 是否为标准输入输出
// 大写覆写为小写，其余不变
void strToLower(char str[]);                 // 大写覆写为小写其余不变
// 将字符串转换为整数
// 使用本函数前必须先判断字符串合法性！
int strToInt(char str[]);                    // 字符转数字
bool isStandardFloat(char str[]);            // 是否为标准的浮点数输入
double strToDouble(char str[]);              // 字符转浮点数
double intStrToDouble(char str[]);           // 整形字符转浮点数
bool isStandardText(char str[]);             // 是否为标准文本输入
bool isEmailFormat(const std::string& email); // 是否为标准邮箱格式
bool isPhoneNumber(const std::string& phone); // 是否为标准手机号格式
bool isPasswordStrength(const std::string& password); // 密码强度检测
bool isValidLength(const std::string& str, int minLength, int maxLength); // 长度验证
bool containsSpecialChars(const std::string& str); // 包含特殊字符检测
bool isAlphanumeric(const std::string& str);       // 是否只包含字母和数字
bool isNumericString(const std::string& str);      // 是否为纯数字字符串
bool isDateValid(int year, int month, int day);    // 日期有效性验证
bool isTimeValid(int hour, int minute, int second); // 时间有效性验证
std::string sanitizeInput(const std::string& input); // 清理输入，移除危险字符
bool isValidUsername(const std::string& username);   // 用户名格式验证
bool isValidChineseName(const std::string& name);    // 中文姓名格式验证
//--------------------

#endif // INPUTCHECK_H