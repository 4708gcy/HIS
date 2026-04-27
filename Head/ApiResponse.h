#ifndef API_RESPONSE_H
#define API_RESPONSE_H

#include "json.hpp"
#include <string>

using json = nlohmann::json;

// 统一 API 响应格式构建器
namespace ApiResponse
{
    inline json success(const std::string &message = "success", const json &data = nullptr)
    {
        json res;
        res["code"] = 200;
        res["message"] = message;
        if (!data.is_null())
            res["data"] = data;
        else
            res["data"] = json::object();
        return res;
    }

    inline json error(int code, const std::string &message)
    {
        json res;
        res["code"] = code;
        res["message"] = message;
        res["data"] = json::object();
        return res;
    }

    inline json badRequest(const std::string &message = "请求参数错误")
    {
        return error(400, message);
    }

    inline json unauthorized(const std::string &message = "未登录或登录已过期")
    {
        return error(401, message);
    }

    inline json forbidden(const std::string &message = "无权限执行此操作")
    {
        return error(403, message);
    }

    inline json notFound(const std::string &message = "资源不存在")
    {
        return error(404, message);
    }

    inline json serverError(const std::string &message = "服务器内部错误")
    {
        return error(500, message);
    }
}

#endif
