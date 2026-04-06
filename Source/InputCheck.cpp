#include "../Head/InputCheck.h"
#include <regex>
#include <algorithm>
#include <cctype>
#include <sstream>

bool isCharacter(char ch) {
    return std::isalpha(ch);
}

bool isDigit(char ch) {
    return std::isdigit(ch);
}

bool isFullCharacter(char str[]) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (!std::isalpha(str[i])) {
            return false;
        }
    }
    return true;
}

bool isFullDigit(char str[]) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (!std::isdigit(str[i])) {
            return false;
        }
    }
    return true;
}

bool isStdInput(char str[]) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (!std::isalnum(str[i])) {
            return false;
        }
    }
    return true;
}

void strToLower(char str[]) {
    for (int i = 0; str[i] != '\0'; i++) {
        str[i] = std::tolower(str[i]);
    }
}

int strToInt(char str[]) {
    int result = 0;
    int sign = 1;
    int i = 0;
    
    // 处理正负号
    if (str[0] == '-') {
        sign = -1;
        i++;
    } else if (str[0] == '+') {
        i++;
    }
    
    // 转换数字
    for (; str[i] != '\0'; i++) {
        if (!std::isdigit(str[i])) {
            break;
        }
        result = result * 10 + (str[i] - '0');
    }
    
    return result * sign;
}

bool isStandardFloat(char str[]) {
    bool hasDecimalPoint = false;
    int len = strlen(str);
    
    if (len == 0) return false;
    
    int start = 0;
    if (str[0] == '+' || str[0] == '-') {
        start = 1;
        if (len <= start) return false;
    }
    
    for (int i = start; str[i] != '\0'; i++) {
        if (str[i] == '.') {
            if (hasDecimalPoint) return false; // 不能有两个小数点
            hasDecimalPoint = true;
        } else if (!std::isdigit(str[i])) {
            return false;
        }
    }
    
    return hasDecimalPoint; // 必须有一个小数点
}

double strToDouble(char str[]) {
    double result = 0.0;
    double fraction = 0.0;
    int sign = 1;
    int i = 0;
    bool afterDecimal = false;
    double divisor = 10.0;
    
    // 处理正负号
    if (str[0] == '-') {
        sign = -1;
        i++;
    } else if (str[0] == '+') {
        i++;
    }
    
    // 转换数字
    for (; str[i] != '\0'; i++) {
        if (str[i] == '.') {
            afterDecimal = true;
        } else if (std::isdigit(str[i])) {
            int digit = str[i] - '0';
            if (!afterDecimal) {
                result = result * 10 + digit;
            } else {
                result += digit / divisor;
                divisor *= 10.0;
            }
        } else {
            break;
        }
    }
    
    return result * sign;
}

double intStrToDouble(char str[]) {
    return static_cast<double>(strToInt(str));
}

bool isStandardText(char str[]) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (!std::isalnum(str[i]) && str[i] != ' ' && str[i] != '_' && str[i] != '-' && str[i] != '.') {
            return false;
        }
    }
    return true;
}

bool isEmailFormat(const std::string& email) {
    const std::regex pattern(R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)");
    return std::regex_match(email, pattern);
}

bool isPhoneNumber(const std::string& phone) {
    // 假设中国手机号格式
    const std::regex pattern(R"(^1[3-9]\d{9}$)");
    return std::regex_match(phone, pattern);
}

bool isPasswordStrength(const std::string& password) {
    if (password.length() < 8) {
        return false; // 密码长度至少8位
    }
    
    bool hasUpper = false, hasLower = false, hasDigit = false, hasSpecial = false;
    
    for (char c : password) {
        if (std::isupper(c)) hasUpper = true;
        else if (std::islower(c)) hasLower = true;
        else if (std::isdigit(c)) hasDigit = true;
        else if (std::ispunct(c)) hasSpecial = true;
    }
    
    // 至少包含大写字母、小写字母、数字和特殊字符中的三种
    int criteria = (hasUpper ? 1 : 0) + (hasLower ? 1 : 0) + 
                   (hasDigit ? 1 : 0) + (hasSpecial ? 1 : 0);
    
    return criteria >= 3;
}

bool isValidLength(const std::string& str, int minLength, int maxLength) {
    int len = str.length();
    return len >= minLength && len <= maxLength;
}

bool containsSpecialChars(const std::string& str) {
    for (char c : str) {
        if (!std::isalnum(c) && !std::isspace(c)) {
            return true;
        }
    }
    return false;
}

bool isAlphanumeric(const std::string& str) {
    for (char c : str) {
        if (!std::isalnum(c)) {
            return false;
        }
    }
    return true;
}

bool isNumericString(const std::string& str) {
    if (str.empty()) return false;
    
    for (char c : str) {
        if (!std::isdigit(c)) {
            return false;
        }
    }
    return true;
}

bool isDateValid(int year, int month, int day) {
    if (year < 1 || month < 1 || month > 12 || day < 1 || day > 31) {
        return false;
    }
    
    // 检查每个月的天数
    int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    
    // 检查闰年
    if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
        daysInMonth[1] = 29; // 闰年2月有29天
    }
    
    return day <= daysInMonth[month - 1];
}

bool isTimeValid(int hour, int minute, int second) {
    return (hour >= 0 && hour <= 23) && 
           (minute >= 0 && minute <= 59) && 
           (second >= 0 && second <= 59);
}

std::string sanitizeInput(const std::string& input) {
    std::string sanitized = input;
    // 移除潜在的危险字符（如SQL注入字符等）
    std::replace(sanitized.begin(), sanitized.end(), '\'', '_');
    std::replace(sanitized.begin(), sanitized.end(), '"', '_');
    std::replace(sanitized.begin(), sanitized.end(), ';', '_');
    std::replace(sanitized.begin(), sanitized.end(), '\\', '_');
    return sanitized;
}

bool isValidUsername(const std::string& username) {
    if (username.length() < 3 || username.length() > 20) {
        return false;
    }
    
    // 用户名只能包含字母、数字和下划线，且不能以数字开头
    if (std::isdigit(username[0])) {
        return false;
    }
    
    for (char c : username) {
        if (!std::isalnum(c) && c != '_') {
            return false;
        }
    }
    
    return true;
}

bool isValidChineseName(const std::string& name) {
    // 简单的中文姓名验证（2-4个汉字）
    if (name.length() < 2 || name.length() > 12) { // 汉字通常占3字节
        return false;
    }
    
    // 检查是否都是中文字符（这是一个简化版本）
    // 在实际应用中可能需要更复杂的中文字符检测
    for (unsigned char c : name) {
        if (c < 0x80) { // ASCII字符
            return false; // 中文姓名不应包含ASCII字符
        }
    }
    
    return true;
}




// 测试代码

// #include <iostream>

// int main()
// {
//     std::cout << "=========================================" << std::endl;
//     std::cout << "      输入检查功能测试" << std::endl;
//     std::cout << "=========================================" << std::endl;

//     // 测试字符检查
//     std::cout << "\n【字符检查测试】" << std::endl;
//     std::cout << "isCharacter('A'): " << isCharacter('A') << " (应为1)" << std::endl;
//     std::cout << "isCharacter('5'): " << isCharacter('5') << " (应为0)" << std::endl;
//     std::cout << "isCharacter('@'): " << isCharacter('@') << " (应为0)" << std::endl;

//     // 测试数字检查
//     std::cout << "\n【数字检查测试】" << std::endl;
//     std::cout << "isDigit('5'): " << isDigit('5') << " (应为1)" << std::endl;
//     std::cout << "isDigit('A'): " << isDigit('A') << " (应为0)" << std::endl;

//     // 测试字符串检查
//     char testStr1[] = "HelloWorld";
//     char testStr2[] = "123456";
//     char testStr3[] = "Hello123";
//     char testStr4[] = "Hello123!";

//     std::cout << "\n【字符串检查测试】" << std::endl;
//     std::cout << "isFullCharacter(\"HelloWorld\"): " << isFullCharacter(testStr1) << " (应为1)" << std::endl;
//     std::cout << "isFullDigit(\"123456\"): " << isFullDigit(testStr2) << " (应为1)" << std::endl;
//     std::cout << "isStdInput(\"Hello123\"): " << isStdInput(testStr3) << " (应为1)" << std::endl;
//     std::cout << "isStdInput(\"Hello123!\"): " << isStdInput(testStr4) << " (应为0)" << std::endl;

//     // 测试字符串转数字
//     char numStr1[] = "12345";
//     char numStr2[] = "-6789";
//     char numStr3[] = "+456";

//     std::cout << "\n【字符串转数字测试】" << std::endl;
//     std::cout << "strToInt(\"12345\"): " << strToInt(numStr1) << " (应为12345)" << std::endl;
//     std::cout << "strToInt(\"-6789\"): " << strToInt(numStr2) << " (应为-6789)" << std::endl;
//     std::cout << "strToInt(\"+456\"): " << strToInt(numStr3) << " (应为456)" << std::endl;

//     // 测试浮点数检查
//     char floatStr1[] = "123.45";
//     char floatStr2[] = "123.45.67";
//     char floatStr3[] = "-123.45";
//     char floatStr4[] = "abc";

//     std::cout << "\n【浮点数检查测试】" << std::endl;
//     std::cout << "isStandardFloat(\"123.45\"): " << isStandardFloat(floatStr1) << " (应为1)" << std::endl;
//     std::cout << "isStandardFloat(\"123.45.67\"): " << isStandardFloat(floatStr2) << " (应为0)" << std::endl;
//     std::cout << "isStandardFloat(\"-123.45\"): " << isStandardFloat(floatStr3) << " (应为0，因为不是标准浮点数格式)" << std::endl;
//     std::cout << "isStandardFloat(\"abc\"): " << isStandardFloat(floatStr4) << " (应为0)" << std::endl;

//     // 测试浮点数转换
//     std::cout << "\n【浮点数转换测试】" << std::endl;
//     std::cout << "strToDouble(\"123.45\"): " << strToDouble(floatStr1) << std::endl;
//     std::cout << "intStrToDouble(\"12345\"): " << intStrToDouble(numStr1) << std::endl;

//     // 测试标准文本检查
//     char textStr1[] = "Hello World";
//     char textStr2[] = "Hello@World";
//     char textStr3[] = "User_Name-123";

//     std::cout << "\n【标准文本检查测试】" << std::endl;
//     std::cout << "isStandardText(\"Hello World\"): " << isStandardText(textStr1) << " (应为1)" << std::endl;
//     std::cout << "isStandardText(\"Hello@World\"): " << isStandardText(textStr2) << " (应为0)" << std::endl;
//     std::cout << "isStandardText(\"User_Name-123\"): " << isStandardText(textStr3) << " (应为1)" << std::endl;

//     // 测试邮箱格式
//     std::cout << "\n【邮箱格式检查测试】" << std::endl;
//     std::cout << "isEmailFormat(\"test@example.com\"): " << isEmailFormat("test@example.com") << " (应为1)" << std::endl;
//     std::cout << "isEmailFormat(\"invalid-email\"): " << isEmailFormat("invalid-email") << " (应为0)" << std::endl;
//     std::cout << "isEmailFormat(\"user@domain.co.uk\"): " << isEmailFormat("user@domain.co.uk") << " (应为1)" << std::endl;

//     // 测试手机号格式
//     std::cout << "\n【手机号格式检查测试】" << std::endl;
//     std::cout << "isPhoneNumber(\"13812345678\"): " << isPhoneNumber("13812345678") << " (应为1)" << std::endl;
//     std::cout << "isPhoneNumber(\"12345678901\"): " << isPhoneNumber("12345678901") << " (应为0)" << std::endl;
//     std::cout << "isPhoneNumber(\"15912345678\"): " << isPhoneNumber("15912345678") << " (应为1)" << std::endl;

//     // 测试密码强度
//     std::cout << "\n【密码强度检查测试】" << std::endl;
//     std::cout << "isPasswordStrength(\"weak\"): " << isPasswordStrength("weak") << " (应为0)" << std::endl;
//     std::cout << "isPasswordStrength(\"StrongPass123!\"): " << isPasswordStrength("StrongPass123!") << " (应为1)" << std::endl;
//     std::cout << "isPasswordStrength(\"simple123\"): " << isPasswordStrength("simple123") << " (应为0)" << std::endl;
//     std::cout << "isPasswordStrength(\"MyP@ssw0rd\"): " << isPasswordStrength("MyP@ssw0rd") << " (应为1)" << std::endl;

//     // 测试用户名格式
//     std::cout << "\n【用户名格式检查测试】" << std::endl;
//     std::cout << "isValidUsername(\"user123\"): " << isValidUsername("user123") << " (应为1)" << std::endl;
//     std::cout << "isValidUsername(\"123user\"): " << isValidUsername("123user") << " (应为0)" << std::endl;
//     std::cout << "isValidUsername(\"user_name\"): " << isValidUsername("user_name") << " (应为1)" << std::endl;
//     std::cout << "isValidUsername(\"user@name\"): " << isValidUsername("user@name") << " (应为0)" << std::endl;
//     std::cout << "isValidUsername(\"ab\"): " << isValidUsername("ab") << " (应为0，太短)" << std::endl;

//     // 测试长度验证
//     std::cout << "\n【长度验证测试】" << std::endl;
//     std::cout << "isValidLength(\"hello\", 3, 10): " << isValidLength("hello", 3, 10) << " (应为1)" << std::endl;
//     std::cout << "isValidLength(\"hi\", 3, 10): " << isValidLength("hi", 3, 10) << " (应为0)" << std::endl;
//     std::cout << "isValidLength(\"verylongstring\", 3, 10): " << isValidLength("verylongstring", 3, 10) << " (应为0)" << std::endl;

//     // 测试特殊字符检测
//     std::cout << "\n【特殊字符检测测试】" << std::endl;
//     std::cout << "containsSpecialChars(\"hello\"): " << containsSpecialChars("hello") << " (应为0)" << std::endl;
//     std::cout << "containsSpecialChars(\"hello@world\"): " << containsSpecialChars("hello@world") << " (应为1)" << std::endl;
//     std::cout << "containsSpecialChars(\"hello world\"): " << containsSpecialChars("hello world") << " (应为0，空格不算特殊字符)" << std::endl;

//     // 测试字母数字检查
//     std::cout << "\n【字母数字检查测试】" << std::endl;
//     std::cout << "isAlphanumeric(\"Hello123\"): " << isAlphanumeric("Hello123") << " (应为1)" << std::endl;
//     std::cout << "isAlphanumeric(\"Hello@123\"): " << isAlphanumeric("Hello@123") << " (应为0)" << std::endl;

//     // 测试纯数字字符串检查
//     std::cout << "\n【纯数字字符串检查测试】" << std::endl;
//     std::cout << "isNumericString(\"12345\"): " << isNumericString("12345") << " (应为1)" << std::endl;
//     std::cout << "isNumericString(\"123a45\"): " << isNumericString("123a45") << " (应为0)" << std::endl;
//     std::cout << "isNumericString(\"\"): " << isNumericString("") << " (应为0)" << std::endl;

//     // 测试字符串大小写转换
//     char lowerStr[] = "HeLLo WoRLd";
//     std::cout << "\n【字符串大小写转换测试】" << std::endl;
//     std::cout << "原字符串: " << lowerStr << std::endl;
//     strToLower(lowerStr);
//     std::cout << "转换后: " << lowerStr << std::endl;

//     // 测试日期验证
//     std::cout << "\n【日期验证测试】" << std::endl;
//     std::cout << "isDateValid(2023, 12, 25): " << isDateValid(2023, 12, 25) << " (应为1)" << std::endl;
//     std::cout << "isDateValid(2023, 2, 30): " << isDateValid(2023, 2, 30) << " (应为0)" << std::endl;
//     std::cout << "isDateValid(2024, 2, 29): " << isDateValid(2024, 2, 29) << " (应为1，闰年)" << std::endl;
//     std::cout << "isDateValid(2023, 2, 29): " << isDateValid(2023, 2, 29) << " (应为0，非闰年)" << std::endl;

//     // 测试时间验证
//     std::cout << "\n【时间验证测试】" << std::endl;
//     std::cout << "isTimeValid(14, 30, 45): " << isTimeValid(14, 30, 45) << " (应为1)" << std::endl;
//     std::cout << "isTimeValid(25, 30, 45): " << isTimeValid(25, 30, 45) << " (应为0)" << std::endl;
//     std::cout << "isTimeValid(12, 60, 45): " << isTimeValid(12, 60, 45) << " (应为0)" << std::endl;
//     std::cout << "isTimeValid(12, 30, 60): " << isTimeValid(12, 30, 60) << " (应为0)" << std::endl;

//     // 测试输入清理功能
//     std::cout << "\n【输入清理功能测试】" << std::endl;
//     std::string dangerousInput = "John's \"Dangerous\" Input;";
//     std::string safeInput = sanitizeInput(dangerousInput);
//     std::cout << "原始输入: " << dangerousInput << std::endl;
//     std::cout << "清理后: " << safeInput << std::endl;

//     // 测试中文姓名验证（简单测试）
//     std::cout << "\n【中文姓名验证测试】" << std::endl;
//     std::cout << "isValidChineseName(\"张三\"): " << isValidChineseName("张三") << " (应为1)" << std::endl;
//     std::cout << "isValidChineseName(\"John\"): " << isValidChineseName("John") << " (应为0)" << std::endl;

//     std::cout << "\n=========================================" << std::endl;
//     std::cout << "          输入检查测试完成！" << std::endl;
//     std::cout << "=========================================" << std::endl;

//     return 0;
// }