#ifndef JWT_AUTH_H
#define JWT_AUTH_H

#include <string>
#include <map>

// JWT token 生成与验证
namespace JWTAuth
{
    // 生成 JWT token
    std::string generateToken(const std::string &userID, int role, const std::string &secretKey);

    // 验证 JWT token，返回 true 表示有效
    bool validateToken(const std::string &token, const std::string &secretKey);

    // 从 token 中解析 payload（不验证签名）
    std::map<std::string, std::string> parsePayload(const std::string &token);

    // 从 token 中获取 userID
    std::string getUserID(const std::string &token);

    // 从 token 中获取 role
    int getRole(const std::string &token);

    // 从 HTTP Authorization header 中提取 token
    std::string extractBearerToken(const std::string &authHeader);

    // 获取全局密钥
    std::string getSecretKey();
}

#endif
