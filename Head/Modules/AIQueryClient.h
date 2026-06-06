#ifndef AI_QUERY_CLIENT_H
#define AI_QUERY_CLIENT_H

#include <string>
#include <vector>

/// C++ HTTP 客户端，连接 HIS AI Service v2.0（FastAPI, localhost:5001）
/// 使用 WinSock2 实现，零外部依赖
/// 支持 API Key 认证（X-API-Header）
class AIQueryClient
{
private:
    std::string host;
    int port;
    std::string apiKey;

    // 发送 GET 请求，返回响应体字符串
    std::string httpGet(const std::string &path);

    // 发送 POST 请求（JSON 请求体），返回响应体字符串
    std::string httpPost(const std::string &path, const std::string &jsonBody);

    // 从 JSON 响应中提取指定 key 的字符串值
    static std::string jsonGetStr(const std::string &json, const std::string &key);

    // 简单的 JSON 美化打印（key: value 格式）
    static std::string formatJsonSimple(const std::string &json);

public:
    AIQueryClient(const std::string &host = "127.0.0.1", int port = 5001);

    /// 设置 API Key（用于 X-API-Key 请求头）
    void setApiKey(const std::string &key);

    /// 检查 AI 服务是否在线
    bool isAvailable();

    // ==================== 原有接口（兼容 v1，内部自动适配 v2） ====================

    /// 获取月度统计数据（内部调用 dashboard）
    std::string getMonthlyStats();

    /// 获取需求预测（默认 auto 策略）
    std::string getPredictions();

    /// 获取需求预测（指定策略和科室）
    std::string getPredictions(const std::string &strategy, const std::string &department = "");

    /// 获取 Z-score 异常检测结果（默认 auto 策略）
    std::string getAnomalies();

    /// 获取异常检测（指定阈值、策略和科室）
    std::string getAnomalies(double threshold, const std::string &strategy = "auto", const std::string &department = "");

    /// 获取药品库存分析（内部调用 dashboard）
    std::string getMedicines();

    /// 获取综合仪表盘数据（默认 llm 策略）
    std::string getDashboard();

    /// 获取综合仪表盘（指定策略）
    std::string getDashboard(const std::string &strategy);

    /// 获取床位分配分析与优化建议（默认 auto 策略）
    std::string getBedOptimization();

    /// 获取床位优化（指定策略和科室）
    std::string getBedOptimization(const std::string &strategy, const std::string &department = "");

    // ==================== 新增 v2 接口 ====================

    /// RAG 知识库问答
    /// @param query 用户问题
    /// @param topK 检索片段数（默认 5）
    std::string ragChat(const std::string &query, int topK = 5);

    /// 下载图表 PNG 到本地文件，返回文件路径（空字符串=失败）
    std::string downloadChart(const std::string &chartType, const std::string &savePath);
};

#endif // AI_QUERY_CLIENT_H
