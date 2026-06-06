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

void AIQueryClient::setApiKey(const std::string &key)
{
    apiKey = key;
}

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

// 从 HTTP 响应中提取 Content-Length 头部值，返回 -1 表示未找到
static int parseContentLength(const std::string &headers)
{
    const std::string key = "Content-Length:";
    auto pos = headers.find(key);
    if (pos == std::string::npos)
    {
        const std::string keyLower = "content-length:";
        pos = headers.find(keyLower);
    }
    if (pos == std::string::npos)
        return -1;

    pos += std::string("Content-Length:").length();
    while (pos < headers.size() && (headers[pos] == ' ' || headers[pos] == '\t'))
        pos++;

    std::string num;
    while (pos < headers.size() && headers[pos] >= '0' && headers[pos] <= '9')
    {
        num += headers[pos];
        pos++;
    }
    if (num.empty()) return -1;
    return std::stoi(num);
}

std::string AIQueryClient::httpGet(const std::string &path)
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
    req << "GET " << path << " HTTP/1.1\r\n"
        << "Host: " << host << ":" << port << "\r\n"
        << "Connection: close\r\n"
        << "Accept: application/json\r\n";
    if (!apiKey.empty())
        req << "X-API-Key: " << apiKey << "\r\n";
    req << "\r\n";

    std::string reqStr = req.str();
    if (send(sock, reqStr.c_str(), (int)reqStr.size(), 0) < 0)
    {
        CLOSE_SOCKET(sock);
        return "{}";
    }

    std::string response;
    char buf[4096];
    int received;
    while ((received = (int)recv(sock, buf, sizeof(buf), 0)) > 0)
    {
        response.append(buf, received);
    }
    CLOSE_SOCKET(sock);

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

    int timeout = 10000;  // LLM 请求可能需要更长时间
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
        << "Content-Type: application/json\r\n";
    if (!apiKey.empty())
        req << "X-API-Key: " << apiKey << "\r\n";
    req << "Content-Length: " << jsonBody.size() << "\r\n"
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
    while ((received = (int)recv(sock, buf, sizeof(buf), 0)) > 0)
    {
        response.append(buf, received);
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

    pos++;
    while (pos < json.size() && (json[pos] == ' ' || json[pos] == '\t' || json[pos] == '\n'))
        pos++;

    if (pos >= json.size()) return "";

    if (json[pos] == '"')
    {
        pos++;
        auto end = json.find('"', pos);
        if (end == std::string::npos) return "";
        return json.substr(pos, end - pos);
    }

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
            break;
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
    return getDashboard();
}

std::string AIQueryClient::getPredictions()
{
    return getPredictions("auto", "");
}

std::string AIQueryClient::getPredictions(const std::string &strategy, const std::string &department)
{
    std::string body = "{\"months\":6,\"strategy\":\"" + strategy + "\"";
    if (!department.empty())
        body += ",\"department\":\"" + department + "\"";
    body += "}";
    return formatJsonSimple(httpPost("/api/v2/predictions", body));
}

std::string AIQueryClient::getAnomalies()
{
    return getAnomalies(2.0, "auto", "");
}

std::string AIQueryClient::getAnomalies(double threshold, const std::string &strategy, const std::string &department)
{
    std::string body = "{\"threshold\":" + std::to_string(threshold) + ",\"strategy\":\"" + strategy + "\"";
    if (!department.empty())
        body += ",\"department\":\"" + department + "\"";
    body += "}";
    return formatJsonSimple(httpPost("/api/v2/anomalies", body));
}

std::string AIQueryClient::getMedicines()
{
    return getDashboard();
}

std::string AIQueryClient::getDashboard()
{
    return getDashboard("llm");
}

std::string AIQueryClient::getDashboard(const std::string &strategy)
{
    std::string body = "{\"strategy\":\"" + strategy + "\"}";
    return formatJsonSimple(httpPost("/api/v2/dashboard", body));
}

std::string AIQueryClient::getBedOptimization()
{
    return getBedOptimization("auto", "");
}

std::string AIQueryClient::getBedOptimization(const std::string &strategy, const std::string &department)
{
    std::string body = "{\"strategy\":\"" + strategy + "\"";
    if (!department.empty())
        body += ",\"department\":\"" + department + "\"";
    body += "}";
    return formatJsonSimple(httpPost("/api/v2/beds", body));
}

std::string AIQueryClient::ragChat(const std::string &query, int topK)
{
    std::string body = "{\"query\":\"" + query + "\",\"top_k\":" + std::to_string(topK) + "}";
    return formatJsonSimple(httpPost("/api/v2/rag/chat", body));
}

std::string AIQueryClient::downloadChart(const std::string &chartType, const std::string &savePath)
{
    std::string path = "/api/v2/charts/" + chartType;

    if (!initWinsock()) return "";

    struct addrinfo hints{}, *result = nullptr;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    std::string portStr = std::to_string(port);
    if (getaddrinfo(host.c_str(), portStr.c_str(), &hints, &result) != 0)
        return "";

    int sock = (int)socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (sock < 0) { freeaddrinfo(result); return ""; }

    int timeout = 10000;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char *)&timeout, sizeof(timeout));
    setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (const char *)&timeout, sizeof(timeout));

    if (connect(sock, result->ai_addr, (int)result->ai_addrlen) < 0)
    {
        CLOSE_SOCKET(sock);
        freeaddrinfo(result);
        return "";
    }
    freeaddrinfo(result);

    std::ostringstream req;
    req << "GET " << path << " HTTP/1.1\r\n"
        << "Host: " << host << ":" << port << "\r\n"
        << "Connection: close\r\n"
        << "Accept: image/png\r\n";
    if (!apiKey.empty())
        req << "X-API-Key: " << apiKey << "\r\n";
    req << "\r\n";

    std::string reqStr = req.str();
    if (send(sock, reqStr.c_str(), (int)reqStr.size(), 0) < 0)
    {
        CLOSE_SOCKET(sock);
        return "";
    }

    std::string httpResponse;
    char buf[8192];
    int received;
    while ((received = (int)recv(sock, buf, sizeof(buf), 0)) > 0)
    {
        httpResponse.append(buf, received);
    }
    CLOSE_SOCKET(sock);

    auto headerEnd = httpResponse.find("\r\n\r\n");
    if (headerEnd == std::string::npos)
        return "";
    if (httpResponse.size() <= headerEnd + 4)
        return "";

    std::string headers = httpResponse.substr(0, headerEnd);
    if (headers.find("200 OK") == std::string::npos &&
        headers.find("200 ") == std::string::npos)
    {
        std::cerr << "[AIQueryClient] HTTP 错误，状态行: "
                  << headers.substr(0, headers.find('\r')) << std::endl;
        return "";
    }

    const char *bodyStart = httpResponse.data() + headerEnd + 4;
    size_t bodySize = httpResponse.size() - (headerEnd + 4);

    if (bodySize == 0)
        return "";

    int contentLength = parseContentLength(headers);
    if (contentLength > 0 && static_cast<size_t>(contentLength) != bodySize)
    {
        std::cerr << "[AIQueryClient] 警告: Content-Length=" << contentLength
                  << " 但实际体大小=" << bodySize << std::endl;
    }

    std::ofstream out(savePath, std::ios::binary);
    if (!out.is_open())
        return "";
    out.write(bodyStart, bodySize);
    out.close();

    return savePath;
}
