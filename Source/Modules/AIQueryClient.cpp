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

std::string AIQueryClient::unescapeJson(const std::string &s)
{
    std::string result;
    for (size_t i = 0; i < s.size(); i++)
    {
        if (s[i] == '\\' && i + 1 < s.size())
        {
            switch (s[i + 1])
            {
            case 'n':  result += '\n'; i++; break;
            case 't':  result += '\t'; i++; break;
            case 'r':  result += '\r'; i++; break;
            case '"':  result += '"';  i++; break;
            case '\\': result += '\\'; i++; break;
            default:   result += s[i]; break;
            }
        }
        else
            result += s[i];
    }
    return result;
}

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
        return unescapeJson(json.substr(pos, end - pos));
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
    std::string resp = httpGet("/health");
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

// ==================== JSON 数组解析辅助 ====================

/// 从 JSON 中提取 key 对应的字符串值，从指定位置开始搜索
static std::string jsonGetStrFrom(const std::string &json, size_t start,
                                   const std::string &key)
{
    std::string searchKey = "\"" + key + "\"";
    auto pos = json.find(searchKey, start);
    if (pos == std::string::npos)
        return "";
    pos = json.find(':', pos + searchKey.size());
    if (pos == std::string::npos)
        return "";
    pos++;
    while (pos < json.size() && (json[pos] == ' ' || json[pos] == '\t' || json[pos] == '\n' || json[pos] == '\r'))
        pos++;
    if (pos >= json.size())
        return "";
    if (json[pos] == '"')
    {
        pos++;
        auto end = json.find('"', pos);
        if (end == std::string::npos)
            return "";
        // 处理转义引号
        while (end > pos && json[end - 1] == '\\')
            end = json.find('"', end + 1);
        return json.substr(pos, end - pos);
    }
    auto end = json.find_first_of(",}]\n\r \t", pos);
    if (end == std::string::npos)
        return json.substr(pos);
    return json.substr(pos, end - pos);
}

/// 从 JSON 数组中逐个提取对象，返回每个对象的起始位置列表
static std::vector<size_t> jsonFindArrayObjects(const std::string &json,
                                                  const std::string &arrayKey)
{
    std::vector<size_t> positions;
    std::string searchKey = "\"" + arrayKey + "\"";
    auto pos = json.find(searchKey);
    if (pos == std::string::npos)
        return positions;

    // 找到数组开始 [
    auto bracketPos = json.find('[', pos + searchKey.size());
    if (bracketPos == std::string::npos)
        return positions;

    // 逐个找 {
    size_t searchFrom = bracketPos + 1;
    while (searchFrom < json.size())
    {
        auto objStart = json.find('{', searchFrom);
        if (objStart == std::string::npos)
            break;
        positions.push_back(objStart);
        // 跳到匹配的 }
        int depth = 1;
        size_t i = objStart + 1;
        while (i < json.size() && depth > 0)
        {
            if (json[i] == '{')
                depth++;
            else if (json[i] == '}')
                depth--;
            i++;
        }
        searchFrom = i;
    }
    return positions;
}

// ==================== 格式化显示接口 ====================

std::string AIQueryClient::displayPredictions()
{
    std::string raw = httpPost("/api/v2/predictions", "{\"months\":6,\"strategy\":\"auto\"}");

    // 提取总体信息
    std::string strategy = jsonGetStr(raw, "strategy");
    std::string time = jsonGetStr(raw, "processing_time");

    // 解析预测数组
    auto objs = jsonFindArrayObjects(raw, "predictions");

    if (objs.empty())
    {
        // 可能是错误响应
        std::string errMsg = jsonGetStr(raw, "error_msg");
        if (!errMsg.empty())
            return "❌ 请求失败: " + errMsg;
        return "❌ 未获取到预测数据";
    }

    std::ostringstream out;
    out << "\n╔══════════════════════════════════════════════╗\n";
    out << "║            AI 需求预测报告                  ║\n";
    out << "╚══════════════════════════════════════════════╝\n";
    out << "  策略: " << (strategy == "auto" ? "自动" : strategy);
    out << "  |  处理耗时: " << time << "s\n";
    out << "──────────────────────────────────────────────\n";

    for (size_t idx = 0; idx < objs.size(); idx++)
    {
        size_t pos = objs[idx];
        // 找到这个对象的结束位置
        size_t endPos = pos + 1;
        int depth = 1;
        while (endPos < raw.size() && depth > 0)
        {
            if (raw[endPos] == '{') depth++;
            else if (raw[endPos] == '}') depth--;
            endPos++;
        }

        std::string dept = jsonGetStrFrom(raw, pos, "department");
        std::string predicted = jsonGetStrFrom(raw, pos, "predicted_next_month");
        std::string growth = jsonGetStrFrom(raw, pos, "growth_rate");
        std::string confidence = jsonGetStrFrom(raw, pos, "confidence");

        out << "\n  【" << dept << "】\n";
        out << "    预测下月入院: " << predicted << " 人\n";
        out << "    增长率: " << growth << "%\n";

        // 只对第一个科室显示 AI 总结（所有科室的总结相同）
        if (idx == 0)
        {
            std::string interpretation = jsonGetStrFrom(raw, pos, "interpretation");
            if (!interpretation.empty())
            {
                // 解码 \\n 为真正的换行
                std::string decoded;
                for (size_t i = 0; i < interpretation.size(); i++)
                {
                    if (i + 1 < interpretation.size() && interpretation[i] == '\\' && interpretation[i + 1] == 'n')
                    {
                        decoded += '\n';
                        i++;
                    }
                    else
                    {
                        decoded += interpretation[i];
                    }
                }
                // 去掉 Holt-Winters 公式化前缀，只留 AI 分析
                auto aiPos = decoded.find("【AI 分析】");
                if (aiPos != std::string::npos)
                {
                    static const std::string MARKER = "【AI 分析】";
                    std::string aiText = decoded.substr(aiPos + MARKER.size());
                    // 首尾去空白
                    while (!aiText.empty() && (aiText.front() == ' ' || aiText.front() == '\n' || aiText.front() == '\r'))
                        aiText.erase(0, 1);
                    while (!aiText.empty() && (aiText.back() == ' ' || aiText.back() == '\n' || aiText.back() == '\r'))
                        aiText.pop_back();
                    if (!aiText.empty())
                        out << "\n  ◆ AI 分析: " << aiText << "\n";
                }
            }
        }
    }

    out << "──────────────────────────────────────────────\n";
    return out.str();
}

std::string AIQueryClient::displayAnomalies()
{
    std::string raw = httpPost("/api/v2/anomalies",
        "{\"months\":12,\"threshold\":2.0,\"strategy\":\"auto\"}");

    std::string threshold = jsonGetStr(raw, "threshold");
    std::string count = jsonGetStr(raw, "anomaly_count");
    std::string time = jsonGetStr(raw, "processing_time");

    std::ostringstream out;
    out << "\n╔══════════════════════════════════════════════╗\n";
    out << "║            AI 异常检测报告                  ║\n";
    out << "╚══════════════════════════════════════════════╝\n";
    out << "  Z-score 阈值: " << threshold;
    out << "  |  检出异常: " << count << " 个";
    out << "  |  耗时: " << time << "s\n";
    out << "──────────────────────────────────────────────\n";

    auto objs = jsonFindArrayObjects(raw, "anomalies");
    if (objs.empty())
    {
        if (count == "0")
            out << "\n  ✓ 未检测到异常数据，各项指标正常。\n";
        else
        {
            std::string errMsg = jsonGetStr(raw, "error_msg");
            out << "\n  " << (errMsg.empty() ? "未获取到异常数据" : "❌ " + errMsg) << "\n";
        }
        out << "──────────────────────────────────────────────\n";
        return out.str();
    }

    for (size_t idx = 0; idx < objs.size(); idx++)
    {
        size_t pos = objs[idx];
        size_t endPos = pos + 1;
        int depth = 1;
        while (endPos < raw.size() && depth > 0)
        {
            if (raw[endPos] == '{') depth++;
            else if (raw[endPos] == '}') depth--;
            endPos++;
        }

        std::string dept = jsonGetStrFrom(raw, pos, "department");
        std::string month = jsonGetStrFrom(raw, pos, "month");
        std::string value = jsonGetStrFrom(raw, pos, "value");
        std::string zscore = jsonGetStrFrom(raw, pos, "z_score");
        std::string direction = jsonGetStrFrom(raw, pos, "direction");
        std::string mean = jsonGetStrFrom(raw, pos, "mean");
        std::string stddev = jsonGetStrFrom(raw, pos, "stddev");
        std::string interpretation = jsonGetStrFrom(raw, pos, "interpretation");

        out << "\n  【" << dept << " - " << month << "】";
        out << (direction == "high" ? " ⬆ 显著偏高" : " ⬇ 显著偏低") << "\n";
        out << "    实际值: " << value << " 人";
        out << "  |  均值: " << mean;
        out << "  |  标准差: " << stddev << "\n";
        out << "    Z-score: " << zscore << "\n";

        if (!interpretation.empty())
        {
            // 解码 \\n
            std::string decoded;
            for (size_t i = 0; i < interpretation.size(); i++)
            {
                if (i + 1 < interpretation.size() && interpretation[i] == '\\' && interpretation[i + 1] == 'n')
                {
                    decoded += '\n';
                    i++;
                }
                else
                {
                    decoded += interpretation[i];
                }
            }
            out << "    💬 " << decoded << "\n";
        }
    }

    out << "──────────────────────────────────────────────\n";
    return out.str();
}

std::string AIQueryClient::displayBedOptimization()
{
    std::string raw = httpPost("/api/v2/beds", "{\"strategy\":\"auto\"}");

    std::string strategy = jsonGetStr(raw, "strategy");
    std::string time = jsonGetStr(raw, "processing_time");

    auto objs = jsonFindArrayObjects(raw, "departments");
    if (objs.empty())
    {
        std::string errMsg = jsonGetStr(raw, "error_msg");
        return errMsg.empty() ? "❌ 未获取到床位数据" : "❌ " + errMsg;
    }

    std::ostringstream out;
    out << "\n╔══════════════════════════════════════════════╗\n";
    out << "║          AI 床位优化建议报告                ║\n";
    out << "╚══════════════════════════════════════════════╝\n";
    out << "  策略: " << (strategy == "auto" ? "自动" : strategy);
    out << "  |  处理耗时: " << time << "s\n";
    out << "──────────────────────────────────────────────\n";

    for (size_t idx = 0; idx < objs.size(); idx++)
    {
        size_t pos = objs[idx];
        size_t endPos = pos + 1;
        int depth = 1;
        while (endPos < raw.size() && depth > 0)
        {
            if (raw[endPos] == '{') depth++;
            else if (raw[endPos] == '}') depth--;
            endPos++;
        }

        std::string dept = jsonGetStrFrom(raw, pos, "department");
        std::string total = jsonGetStrFrom(raw, pos, "total_beds");
        std::string occupied = jsonGetStrFrom(raw, pos, "occupied");
        std::string available = jsonGetStrFrom(raw, pos, "available");
        std::string utilization = jsonGetStrFrom(raw, pos, "utilization_rate");

        out << "\n  【" << dept << "】\n";
        out << "    总床位: " << total;
        out << "  已占: " << occupied;
        out << "  空闲: " << available;
        out << "  利用率: " << utilization << "%\n";

        std::string report = jsonGetStrFrom(raw, pos, "report");
        if (!report.empty())
        {
            // 解码 \\n
            std::string decoded;
            for (size_t i = 0; i < report.size(); i++)
            {
                if (i + 1 < report.size() && report[i] == '\\' && report[i + 1] == 'n')
                {
                    decoded += '\n';
                    i++;
                }
                else
                {
                    decoded += report[i];
                }
            }
            out << "    💬 " << decoded << "\n";
        }
    }

    out << "──────────────────────────────────────────────\n";
    return out.str();
}

std::string AIQueryClient::displayDashboard()
{
    std::string raw = httpPost("/api/v2/dashboard", "{\"strategy\":\"llm\"}");
    if (raw.empty())
        return "❌ 未获取到仪表盘数据";

    std::ostringstream out;
    out << "\n╔══════════════════════════════════════════════╗\n";
    out << "║            AI 综合运营仪表盘                ║\n";
    out << "╚══════════════════════════════════════════════╝\n";

    // 提取关键摘要信息
    std::string execSummary = jsonGetStr(raw, "executive_summary");
    std::string statsCount = jsonGetStr(raw, "stats_count");
    std::string procTime = jsonGetStr(raw, "processing_time");

    out << "  数据源: " << (statsCount.empty() ? "—" : statsCount) << " 条";
    out << "  |  处理耗时: " << (procTime.empty() ? "—" : procTime) << "s\n";
    out << "──────────────────────────────────────────────\n";

    if (!execSummary.empty())
    {
        // 解码 \\n
        std::string decoded;
        for (size_t i = 0; i < execSummary.size(); i++)
        {
            if (i + 1 < execSummary.size() && execSummary[i] == '\\' && execSummary[i + 1] == 'n')
            {
                decoded += '\n';
                i++;
            }
            else
            {
                decoded += execSummary[i];
            }
        }
        out << "\n  📊 AI 运营总结:\n";
        out << "  " << decoded << "\n";
    }
    else
    {
        // 如果没有 executive_summary，显示原始格式化 JSON
        out << "\n" << raw << "\n";
    }

    out << "\n──────────────────────────────────────────────\n";
    return out.str();
}

std::string AIQueryClient::displayMedicines()
{
    // 药品库存数据在 dashboard 中
    return displayDashboard();
}

std::string AIQueryClient::displayRagChat(const std::string &query)
{
    // 转义 query 中的特殊字符
    std::string escaped;
    for (char c : query)
    {
        if (c == '"') escaped += "\\\"";
        else if (c == '\\') escaped += "\\\\";
        else escaped += c;
    }
    std::string raw = httpPost("/api/v2/rag/chat",
        "{\"query\":\"" + escaped + "\",\"top_k\":5}");

    std::string answer = jsonGetStr(raw, "answer");
    if (answer.empty())
        return "❌ 未获取到回答。当前知识库为空，请先上传文档。";

    std::ostringstream out;
    out << "\n╔══════════════════════════════════════════════╗\n";
    out << "║            AI 知识库问答                    ║\n";
    out << "╚══════════════════════════════════════════════╝\n";
    out << "  问题: " << query << "\n";
    out << "──────────────────────────────────────────────\n";
    out << "\n  ◆ 回答:\n";

    // UTF-8 安全换行（每行最多 48 个显示宽度，不在多字节字符中间截断）
    std::string line;
    int charCount = 0;
    for (size_t i = 0; i < answer.size(); )
    {
        unsigned char c = (unsigned char)answer[i];
        int bytes = 1;
        if (c >= 0xFC)       bytes = 6;
        else if (c >= 0xF8)  bytes = 5;
        else if (c >= 0xF0)  bytes = 4;
        else if (c >= 0xE0)  bytes = 3;
        else if (c >= 0xC0)  bytes = 2;

        int width = (bytes > 1) ? 2 : 1;  // CJK=2宽度, ASCII=1宽度

        if (charCount + width > 48)
        {
            out << "    " << line << "\n";
            line.clear();
            charCount = 0;
        }
        line.append(answer, i, bytes);
        charCount += width;
        i += bytes;
    }
    if (!line.empty())
        out << "    " << line << "\n";

    out << "\n──────────────────────────────────────────────\n";
    return out.str();
}
