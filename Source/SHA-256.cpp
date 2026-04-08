#include "../Head/SHA-256.h"

// 定义静态常量成员
const uint32_t SHA256::INITIAL_HASH[8] = {
    0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
    0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};

const uint32_t SHA256::K[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};

std::string SHA256::hash(const std::string& input) {
    // 1. 数据准备：将字符串转为字节向量
    std::vector<uint8_t> data(input.begin(), input.end());
    
    // 2. 预处理 (Padding)
    size_t original_bit_len = data.size() * 8;
    
    // 添加位 '1' (0x80)
    data.push_back(0x80);
    
    // 添加 '0' 直到长度满足 (len % 64 == 56) 即 448 bit
    // 注意：这里以字节为单位，56 bytes = 448 bits
    while (data.size() % 64 != 56) {
        data.push_back(0x00);
    }
    
    // 添加原始长度 (64位大端序)
    // 因为 SHA-256 处理的是 512-bit (64字节) 的块
    for (int i = 7; i >= 0; --i) {
        data.push_back((original_bit_len >> (8 * i)) & 0xFF);
    }

    // 3. 初始化哈希值 H
    uint32_t h[8];
    std::copy(std::begin(INITIAL_HASH), std::end(INITIAL_HASH), h);

    // 4. 处理每一个 512-bit (64字节) 的块
    for (size_t i = 0; i < data.size(); i += 64) {
        uint32_t w[64]; // 消息调度数组

        // 4.1 准备消息调度数组 W
        // 前 16 个字直接从数据块获取 (注意转为大端序)
        for (int j = 0; j < 16; ++j) {
            w[j] = (data[i + j*4] << 24) | (data[i + j*4 + 1] << 16) | 
                   (data[i + j*4 + 2] << 8) | (data[i + j*4 + 3]);
        }
        // 后 48 个字通过公式扩展
        for (int j = 16; j < 64; ++j) {
            w[j] = EP1(w[j - 2]) + w[j - 7] + EP0(w[j - 15]) + w[j - 16];
        }

        // 4.2 初始化工作变量
        uint32_t a = h[0], b = h[1], c = h[2], d = h[3];
        uint32_t e = h[4], f = h[5], g = h[6], h_var = h[7];

        // 4.3 64轮主循环 (核心混淆过程)
        for (int j = 0; j < 64; ++j) {
            uint32_t t1 = h_var + SIG1(e) + CH(e, f, g) + K[j] + w[j];
            uint32_t t2 = SIG0(a) + MAJ(a, b, c);
            
            h_var = g;
            g = f;
            f = e;
            e = d + t1;
            d = c;
            c = b;
            b = a;
            a = t1 + t2;
        }

        // 4.4 将结果累加到 H 中
        h[0] += a; h[1] += b; h[2] += c; h[3] += d;
        h[4] += e; h[5] += f; h[6] += g; h[7] += h_var;
    }

    // 5. 输出结果 (转为 Hex 字符串)
    std::stringstream ss;
    for (int i = 0; i < 8; ++i) {
        ss << std::hex << std::setfill('0') << std::setw(8) << h[i];
    }
    return ss.str();
}

std::string SHA256Encrypt(const std::string& password, const std::string& salt, int iterations) {
    std::string currentHash = password + salt;
    
    // 核心思想：通过多次哈希，人为增加计算时间，抵抗暴力破解
    // 如果黑客想破解，他也必须循环这么多次，成本增加 iterations 倍
    for (int i = 0; i < iterations; ++i) {
        currentHash = SHA256::hash(currentHash);
    }
    
    return salt + "$" + currentHash;
}

bool SHA256Verify(const std::string& inputPassword, const std::string& storedHash, int iterations) {
    // 从存储的哈希值中提取盐值
    size_t pos = storedHash.find('$');
    if (pos == std::string::npos) {
        return false; // 格式不正确
    }
    
    // substr(起始位置，截取长度)
    std::string salt = storedHash.substr(0, pos);
    
    // 重新计算哈希值
    std::string calculatedHash = SHA256Encrypt(inputPassword, salt, iterations);
    
    // 比较计算出的哈希值与存储的哈希值
    return calculatedHash == storedHash;
}

std::string generateSalt(unsigned int len)
{
    static const char charset[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    thread_local static std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<std::size_t> dist(0, sizeof(charset) - 2);

    std::string salt;
    salt.reserve(len);
    for (unsigned int i = 0; i < len; ++i)
    {
        salt += charset[dist(rng)];
    }
    return salt;
}

// 测试代码

// #include<iostream>

// int main(){

//     std::cout << "\n=== 密码加密与验证功能测试 ===" << std::endl;

//     // 4. 测试密码加密 (课设场景)
//     std::string myPassword = "password123";
//     std::string mySalt = "RandomSalt_123"; // 实际项目中这里应该用 rand() 生成随机串
//     int myIterations = 1000;               // 迭代 1000 次

//     std::string storedHash = SHA256Encrypt(myPassword, mySalt, myIterations);

//     std::cout << "原始密码: " << myPassword << std::endl;
//     std::cout << "盐值: " << mySalt << std::endl;
//     std::cout << "迭代次数: " << myIterations << std::endl;
//     std::cout << "存储到数据库的哈希: " << storedHash << std::endl;
//     std::cout << "----------------------------------------" << std::endl;

//     // 5. 模拟登录验证
//     std::cout << "\n--- 登录验证模拟 ---" << std::endl;

//     // 正确密码测试
//     std::string correctPassword = "password123";
//     bool isVerified = SHA256Verify(correctPassword, storedHash, myIterations);
//     std::cout << "输入密码: " << correctPassword << std::endl;
//     std::cout << "验证结果: " << (isVerified ? "成功" : "失败") << std::endl;

//     // 错误密码测试
//     std::string wrongPassword = "wrongpassword";
//     bool isWrongVerified = SHA256Verify(wrongPassword, storedHash, myIterations);
//     std::cout << "输入密码: " << wrongPassword << std::endl;
//     std::cout << "验证结果: " << (isWrongVerified ? "成功" : "失败") << std::endl;
//     std::cout << "----------------------------------------" << std::endl;

//     // 6. 测试不同密码产生不同的哈希值
//     std::cout << "\n=== 不同密码哈希值唯一性测试 ===" << std::endl;
//     std::string password1 = "mypassword";
//     std::string password2 = "mypassword";
//     std::string salt1 = "salt1";
//     std::string salt2 = "salt2";

//     std::string hash1 = SHA256Encrypt(password1, salt1, 1000);
//     std::string hash2 = SHA256Encrypt(password2, salt2, 1000);
//     std::string hash3 = SHA256Encrypt(password1, salt1, 2000); // 相同密码和盐，不同迭代次数

//     std::cout << "密码相同，盐不同: " << (hash1 == hash2 ? "相同" : "不同") << std::endl;
//     std::cout << "密码相同，盐相同，迭代次数不同: " << (hash1 == hash3 ? "相同" : "不同") << std::endl;

//     // 相同密码、盐、迭代次数应该产生相同的哈希
//     std::string hash4 = SHA256Encrypt(password1, salt1, 1000);
//     std::cout << "完全相同参数: " << (hash1 == hash4 ? "相同" : "不同") << std::endl;

//     std::cout << "\n=== 测试完成 ===" << std::endl;

//     return 0;
// }