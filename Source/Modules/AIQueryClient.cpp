#include "Modules/AIQueryClient.h"

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <netdb.h>
#endif

#include <sstream>
#include <iostream>
#include <fstream>
#include <cstring>

// ==================== 构造函数 ====================

AIQueryClient::AIQueryClient(const std::string &host, int port)
    : host(host), port(port) {}

// ==================== 低级 HTTP 通信 ====================

static bool initWinsock()
{
#ifdef _WIN32
    static bool initialized = false;
    if (!initialized)
    {
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
            return false;
        initialized = true;
    }
#endif
    return true;
}

#ifdef _WIN32
    #define CLOSE_SOCKET(s) closesocket(s)
#else
    #define CLOSE_SOCKET(s) close(s)
#endif

std::string AIQueryClient::httpGet(const std::string &path)
{
    if (!initWinsock()) return "{}";

    // 解析主机名
    struct addrinfo hints{}, *result = nullptr;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    std::string portStr = std::to_string(port);
    if (getaddrinfo(host.c_str(), portStr.c_str(), &hints, &result) != 0)
        return "{}";

    int sock = (int)socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (sock < 0) { freeaddrinfo(result); return "{}"; }

    // 设置超时 5 秒
    int timeout = 5000;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char *)&timeout, sizeof(timeout));
    setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (const char *)&timeout, sizeof(timeout));

    // 连接
    if (connect(sock, result->ai_addr, (int)result->ai_addrlen) < 0)
    {
        CLOSE_SOCKET(sock);
        freeaddrinfo(result);
        return "{}";
    }
    freeaddrinfo(result);

    // 构造 HTTP 请求
    std::ostringstream req;
    req << "GET " << path << " HTTP/1.1\r\n"
        << "Host: " << host << ":" << port << "\r\n"
        << "Connection: close\r\n"
        << "Accept: application/json\r\n"
        << "\r\n";

    std::string reqStr = req.str();
    if (send(sock, reqStr.c_str(), (int)reqStr.size(), 0) < 0)
    {
        CLOSE_SOCKET(sock);
        return "{}";
    }

    // 接收响应
    std::string response;
    char buf[4096];
    int received;
    while ((received = (int)recv(sock, buf, sizeof(buf) - 1, 0)) > 0)
    {
        buf[received] = '\0';
        response += buf;
    }
    CLOSE_SOCKET(sock);

    // 分离头部和体
    auto headerEnd = response.find("\r\n\r\n");
    if (headerEnd == std::string::npos)
        return "{}";

    return response.substr(headerEnd + 4);
}

std::string AIQueryClient::httpPost(const std::string &path, const std::string &jsonBody)
{
    if (!initWinsock()) return "{}";

    struct addrinfo hints{}, *result = nullptr;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    std::string portStr = std::to_string(port);
    if (getaddrinfo(host.c_str(), portStr.c_str(), &hints, &result) != 0)
        return "{}";

    int sock = (int)socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (sock < 0) { freeaddrinfo(result); return "{}"; }

    int timeout = 5000;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char *)&timeout, sizeof(timeout));
    setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (const char *)&timeout, sizeof(timeout));

    if (connect(sock, result->ai_addr, (int)result->ai_addrlen) < 0)
    {
        CLOSE_SOCKET(sock);
        freeaddrinfo(result);
        return "{}";
    }
    freeaddrinfo(result);

    std::ostringstream req;
    req << "POST " << path << " HTTP/1.1\r\n"
        << "Host: " << host << ":" << port << "\r\n"
        << "Content-Type: application/json\r\n"
        << "Content-Length: " << jsonBody.size() << "\r\n"
        << "Connection: close\r\n"
        << "\r\n"
        << jsonBody;

    std::string reqStr = req.str();
    if (send(sock, reqStr.c_str(), (int)reqStr.size(), 0) < 0)
    {
        CLOSE_SOCKET(sock);
        return "{}";
    }

    std::string response;
    char buf[4096];
    int received;
    while ((received = (int)recv(sock, buf, sizeof(buf) - 1, 0)) > 0)
    {
        buf[received] = '\0';
        response += buf;
    }
    CLOSE_SOCKET(sock);

    auto headerEnd = response.find("\r\n\r\n");
    if (headerEnd == std::string::npos)
        return "{}";

    return response.substr(headerEnd + 4);
}

// ==================== JSON 辅助 ====================

std::string AIQueryClient::jsonGetStr(const std::string &json, const std::string &key)
{
    std::string searchKey = "\"" + key + "\"";
    auto pos = json.find(searchKey);
    if (pos == std::string::npos) return "";

    pos = json.find(':', pos + searchKey.size());
    if (pos == std::string::npos) return "";

    // 跳过冒号和空白
    pos++;
    while (pos < json.size() && (json[pos] == ' ' || json[pos] == '\t' || json[pos] == '\n'))
        pos++;

    if (pos >= json.size()) return "";

    // 字符串值
    if (json[pos] == '"')
    {
        pos++;
        auto end = json.find('"', pos);
        if (end == std::string::npos) return "";
        return json.substr(pos, end - pos);
    }

    // 数字或布尔值
    auto end = json.find_first_of(",}\n\r \t", pos);
    if (end == std::string::npos) return json.substr(pos);
    return json.substr(pos, end - pos);
}

std::string AIQueryClient::formatJsonSimple(const std::string &json)
{
    std::string result;
    result.reserve(json.size());

    int indent = 0;
    bool inString = false;

    for (size_t i = 0; i < json.size(); i++)
    {
        char c = json[i];

        if (c == '"' && (i == 0 || json[i - 1] != '\\'))
            inString = !inString;

        if (inString)
        {
            result += c;
            continue;
        }

        switch (c)
        {
        case '{': case '[':
            result += c;
            result += '\n';
            indent += 2;
            result += std::string(indent, ' ');
            break;
        case '}': case ']':
            result += '\n';
            indent -= 2;
            if (indent < 0) indent = 0;
            result += std::string(indent, ' ');
            result += c;
            break;
        case ',':
            result += c;
            result += '\n';
            result += std::string(indent, ' ');
            break;
        case ':':
            result += ": ";
            break;
        case ' ':
        case '\t':
        case '\n':
        case '\r':
            break; // 跳过空白
        default:
            result += c;
            break;
        }
    }
    return result;
}

// ==================== 高级 API ====================

bool AIQueryClient::isAvailable()
{
    std::string resp = httpGet("/api/health");
    return resp.find("\"ok\"") != std::string::npos ||
           resp.find("\"status\":\"ok\"") != std::string::npos ||
           resp.find("\"status\": \"ok\"") != std::string::npos;
}

std::string AIQueryClient::getMonthlyStats()
{
    return formatJsonSimple(httpGet("/api/stats/monthly"));
}

std::string AIQueryClient::getPredictions()
{
    return formatJsonSimple(httpGet("/api/predictions"));
}

std::string AIQueryClient::getAnomalies()
{
    return formatJsonSimple(httpGet("/api/anomalies"));
}

std::string AIQueryClient::getMedicines()
{
    return formatJsonSimple(httpGet("/api/medicines"));
}

std::string AIQueryClient::getDashboard()
{
    return formatJsonSimple(httpGet("/api/dashboard"));
}

std::string AIQueryClient::getBedOptimization()
{
    return formatJsonSimple(httpGet("/api/bed-optimization"));
}

std::string AIQueryClient::downloadChart(const std::string &chartType, const std::string &savePath)
{
    std::string path = "/api/charts/" + chartType;
    std::string pngData = httpGet(path);

    if (pngData.empty() || pngData == "{}" || pngData.find("error") != std::string::npos)
        return "";

    // 写入 PNG 文件
    std::ofstream out(savePath, std::ios::binary);
    if (!out.is_open())
        return "";
    out.write(pngData.data(), pngData.size());
    out.close();
    return savePath;
}
