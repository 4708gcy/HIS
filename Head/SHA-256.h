/**
 * @file SHA-256.h
 * @brief SHA-256 + 盐 + 迭代 哈希算法的函数声明
 * @details 该头文件声明了一个函数，用于对输入的字符串进行 SHA-256 哈希处理，结合盐值和迭代次数以增强安全性
 * @author 郭承宇
 * @date 2026-4-6
 * @version 1.0
 */

#ifndef SHA256_H
#define SHA256_H

#include <string>
#include <vector>
#include <iomanip>
#include <sstream>
#include <cstdint>
#include <random>

// SHA-256 核心算法实现

class SHA256
{

protected:
    // SHA-256 的初始哈希值 (前8个质数 2,3,5,7,11,13,17,19 的平方根的小数部分)
    static const uint32_t INITIAL_HASH[8];

    // 64个常量 K (前64个质数 2..311 的立方根的小数部分)
    static const uint32_t K[64];

    // 辅助函数：右旋转
    static uint32_t ROTR(uint32_t x, uint32_t n)
    {
        return (x >> n) | (x << (32 - n));
    }

    // 辅助函数：逻辑函数
    static uint32_t CH(uint32_t x, uint32_t y, uint32_t z) { return (x & y) ^ (~x & z); }
    static uint32_t MAJ(uint32_t x, uint32_t y, uint32_t z) { return (x & y) ^ (x & z) ^ (y & z); }
    static uint32_t SIG0(uint32_t x) { return ROTR(x, 2) ^ ROTR(x, 13) ^ ROTR(x, 22); }
    static uint32_t SIG1(uint32_t x) { return ROTR(x, 6) ^ ROTR(x, 11) ^ ROTR(x, 25); }
    static uint32_t EP0(uint32_t x) { return ROTR(x, 7) ^ ROTR(x, 18) ^ (x >> 3); }
    static uint32_t EP1(uint32_t x) { return ROTR(x, 17) ^ ROTR(x, 19) ^ (x >> 10); }

public:
    // 对输入的字符串进行 SHA-256 哈希处理，结合盐值和迭代次数以增强安全性
    static std::string hash(const std::string &input);
};

std::string generateSalt(unsigned int len = 16);

std::string SHA256Encrypt(const std::string &password, const std::string &salt, int iterations);

bool SHA256Verify(const std::string &inputPassword, const std::string &storedHash, int iterations);

#endif // SHA256_H
