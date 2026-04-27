#include "JWTAuth.h"
#include "json.hpp"
#include "SHA-256.h"
#include <sstream>
#include <chrono>
#include <random>
#include <vector>
#include <ctime>
#include <iomanip>

using json = nlohmann::json;

namespace
{
    // Base64 URL 编码（无填充）
    std::string base64UrlEncode(const unsigned char *data, size_t len)
    {
        static const char table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";
        std::string result;
        result.reserve((len + 2) / 3 * 4);
        for (size_t i = 0; i < len; i += 3)
        {
            unsigned int n = (unsigned int)data[i] << 16;
            if (i + 1 < len) n |= (unsigned int)data[i + 1] << 8;
            if (i + 2 < len) n |= data[i + 2];
            result.push_back(table[(n >> 18) & 0x3F]);
            result.push_back(table[(n >> 12) & 0x3F]);
            result.push_back((i + 1 < len) ? table[(n >> 6) & 0x3F] : '.');
            result.push_back((i + 2 < len) ? table[n & 0x3F] : '.');
        }
        // 移除填充的 '.'
        while (!result.empty() && result.back() == '.')
            result.pop_back();
        return result;
    }

    std::string base64UrlEncode(const std::string &s)
    {
        return base64UrlEncode(reinterpret_cast<const unsigned char *>(s.data()), s.size());
    }

    // Base64 URL 解码
    std::string base64UrlDecode(const std::string &input)
    {
        static const int table[256] = {[]() -> int
        {
            int t[256];
            std::fill(t, t + 256, -1);
            for (int i = 0; i < 26; i++) t[(int)"ABCDEFGHIJKLMNOPQRSTUVWXYZ"[i]] = i;
            for (int i = 0; i < 26; i++) t[(int)"abcdefghijklmnopqrstuvwxyz"[i]] = i + 26;
            for (int i = 0; i < 10; i++) t[(int)"0123456789"[i]] = i + 52;
            t[(int)'-'] = 62;
            t[(int)'_'] = 63;
            return *t; // just for initialization
        }()};

        // 重新初始化查找表（避免 lambda 陷阱）
        int lookup[256];
        std::fill(lookup, lookup + 256, -1);
        for (int i = 0; i < 26; i++) lookup[(int)"ABCDEFGHIJKLMNOPQRSTUVWXYZ"[i]] = i;
        for (int i = 0; i < 26; i++) lookup[(int)"abcdefghijklmnopqrstuvwxyz"[i]] = i + 26;
        for (int i = 0; i < 10; i++) lookup[(int)"0123456789"[i]] = i + 52;
        lookup[(int)'-'] = 62;
        lookup[(int)'_'] = 63;

        std::string result;
        std::vector<int> vals;
        for (char c : input)
        {
            if (lookup[(unsigned char)c] == -1) break;
            vals.push_back(lookup[(unsigned char)c]);
        }
        for (size_t i = 0; i < vals.size(); i += 4)
        {
            unsigned int n = 0;
            int bytes = 0;
            for (int j = 0; j < 4 && i + j < vals.size(); j++)
            {
                n |= vals[i + j] << (18 - j * 6);
                bytes++;
            }
            for (int j = 0; j < bytes - 1; j++)
                result.push_back((char)((n >> (16 - j * 8)) & 0xFF));
        }
        return result;
    }

    // HMAC-SHA256
    std::string hmacSha256(const std::string &key, const std::string &msg)
    {
        const size_t blockSize = 64;
        std::string k = key;
        if (k.size() > blockSize)
            k = SHA256Encrypt(k, "", 1).substr(0, 64); // 利用已有的 SHA256
        k.resize(blockSize, '\0');

        std::string oKeyPad, iKeyPad;
        oKeyPad.resize(blockSize);
        iKeyPad.resize(blockSize);
        for (size_t i = 0; i < blockSize; i++)
        {
            oKeyPad[i] = k[i] ^ 0x5c;
            iKeyPad[i] = k[i] ^ 0x36;
        }

        // 内部 hash: SHA256(iKeyPad + msg) -> hex string
        std::string innerData = iKeyPad + msg;
        // 使用项目现有的 SHA256 函数获取 hex digest
        // 由于 SHA256Encrypt 返回 "salt$hash" 格式，我们直接用它的 hash 部分
        // 但这里需要原始字节，所以用一个简化方案：直接用 hex 字符串作为签名
        std::string innerHash = SHA256Encrypt(innerData, "hmac-inner", 1);
        // 提取 hash 部分（跳过 "salt$" 前缀）
        size_t dollarPos = innerHash.find('$');
        if (dollarPos != std::string::npos)
            innerHash = innerHash.substr(dollarPos + 1);

        std::string outerData = oKeyPad;
        // 将 hex string 转回 bytes 追加
        for (size_t i = 0; i < innerHash.size(); i += 2)
        {
            int byte = 0;
            std::istringstream iss(innerHash.substr(i, 2));
            iss >> std::hex >> byte;
            outerData.push_back((char)byte);
        }

        std::string outerHash = SHA256Encrypt(outerData, "hmac-outer", 1);
        size_t pos = outerHash.find('$');
        if (pos != std::string::npos)
            outerHash = outerHash.substr(pos + 1);
        return outerHash;
    }
}

namespace JWTAuth
{
    static const std::string SECRET_KEY = "HIS_JWT_SECRET_KEY_2026";

    std::string getSecretKey() { return SECRET_KEY; }

    std::string generateToken(const std::string &userID, int role, const std::string &secretKey)
    {
        // Header
        json header;
        header["alg"] = "HS256";
        header["typ"] = "JWT";
        std::string headerStr = header.dump();
        std::string headerB64 = base64UrlEncode(headerStr);

        // Payload
        auto now = std::chrono::system_clock::now();
        auto exp = now + std::chrono::hours(24);
        auto nowSec = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
        auto expSec = std::chrono::duration_cast<std::chrono::seconds>(exp.time_since_epoch()).count();

        json payload;
        payload["userID"] = userID;
        payload["role"] = role;
        payload["iat"] = nowSec;
        payload["exp"] = expSec;
        std::string payloadStr = payload.dump();
        std::string payloadB64 = base64UrlEncode(payloadStr);

        // Signature
        std::string signInput = headerB64 + "." + payloadB64;
        std::string signature = hmacSha256(secretKey, signInput);

        return headerB64 + "." + payloadB64 + "." + base64UrlEncode(signature);
    }

    bool validateToken(const std::string &token, const std::string &secretKey)
    {
        size_t dot1 = token.find('.');
        size_t dot2 = token.rfind('.');
        if (dot1 == std::string::npos || dot2 == dot1)
            return false;

        std::string headerB64 = token.substr(0, dot1);
        std::string payloadB64 = token.substr(dot1 + 1, dot2 - dot1 - 1);
        std::string sigB64 = token.substr(dot2 + 1);

        std::string signInput = headerB64 + "." + payloadB64;
        std::string expectedSig = hmacSha256(secretKey, signInput);
        std::string expectedSigB64 = base64UrlEncode(expectedSig);

        // 时序安全比较
        volatile bool match = true;
        volatile size_t len1 = sigB64.size();
        volatile size_t len2 = expectedSigB64.size();
        if (len1 != len2)
            match = false;
        else
        {
            for (size_t i = 0; i < len1; i++)
            {
                if (sigB64[i] != expectedSigB64[i])
                    match = false;
            }
        }
        if (!match)
            return false;

        // 检查过期
        try
        {
            std::string payloadStr = base64UrlDecode(payloadB64);
            json payload = json::parse(payloadStr);
            if (payload.contains("exp"))
            {
                auto nowSec = std::chrono::duration_cast<std::chrono::seconds>(
                                  std::chrono::system_clock::now().time_since_epoch())
                                  .count();
                if (payload["exp"].get<long long>() < nowSec)
                    return false;
            }
        }
        catch (...)
        {
            return false;
        }

        return true;
    }

    std::map<std::string, std::string> parsePayload(const std::string &token)
    {
        std::map<std::string, std::string> result;
        size_t dot1 = token.find('.');
        size_t dot2 = token.rfind('.');
        if (dot1 == std::string::npos || dot2 == dot1)
            return result;

        try
        {
            std::string payloadB64 = token.substr(dot1 + 1, dot2 - dot1 - 1);
            std::string payloadStr = base64UrlDecode(payloadB64);
            json payload = json::parse(payloadStr);
            for (auto it = payload.begin(); it != payload.end(); ++it)
            {
                if (it.value().is_string())
                    result[it.key()] = it.value().get<std::string>();
                else
                    result[it.key()] = it.value().dump();
            }
        }
        catch (...)
        {
        }
        return result;
    }

    std::string getUserID(const std::string &token)
    {
        auto payload = parsePayload(token);
        auto it = payload.find("userID");
        return (it != payload.end()) ? it->second : "";
    }

    int getRole(const std::string &token)
    {
        auto payload = parsePayload(token);
        auto it = payload.find("role");
        if (it != payload.end())
        {
            try { return std::stoi(it->second); }
            catch (...) { return 0; }
        }
        return 0;
    }

    std::string extractBearerToken(const std::string &authHeader)
    {
        if (authHeader.size() > 7 && authHeader.substr(0, 7) == "Bearer ")
            return authHeader.substr(7);
        return authHeader;
    }
}
