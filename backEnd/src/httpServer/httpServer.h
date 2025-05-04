#pragma once

#include "json.hpp"
#include "log.h"
#include <drogon/drogon.h>

using json = nlohmann::json;

#define LOCAL_API_BASE_URL "/toolbox"

#define OK_RESP() OK_RESP_STR("")

// 成功响应 入参为结构体
#define OK_RESP_DATA(data)                                       \
    do {                                                         \
        json j;                                                  \
        (data).toJson(j);                                        \
        auto resp     = createResponse(200, j);                  \
        auto httpResp = drogon::HttpResponse::newHttpResponse(); \
        httpResp->setBody(resp.dump());                          \
        httpResp->addHeader("Content-Type", "application/json"); \
        callback(httpResp);                                      \
    } while (0)

// 成功响应 入参为字符串
#define OK_RESP_STR(data)                                        \
    do {                                                         \
        auto resp     = createResponse(200, data);               \
        auto httpResp = drogon::HttpResponse::newHttpResponse(); \
        httpResp->setBody(resp.dump());                          \
        httpResp->addHeader("Content-Type", "application/json"); \
        callback(httpResp);                                      \
    } while (0)

// 失败响应 并不携带额外错误信息
#define ERROR_RESP(code) ERROR_RESP_MSG(code, "")

// 失败响应 并携带错误信息
#define ERROR_RESP_MSG(code, msg)                                \
    do {                                                         \
        auto resp     = createErrResponse(code, msg);            \
        auto httpResp = drogon::HttpResponse::newHttpResponse(); \
        httpResp->setBody(resp.dump());                          \
        httpResp->addHeader("Content-Type", "application/json"); \
        callback(httpResp);                                      \
    } while (0)

#define PARSE_JSON(res, orgStr)                           \
    do {                                                  \
        try {                                             \
            (res) = nlohmann::json::parse(orgStr);        \
        } catch (const std::exception& e) {               \
            ret    = ERROR_CODE_PARAM_ERR;                \
            errMsg = e.what();                            \
            logE("json parse error: %s", errMsg.c_str()); \
            goto exit;                                    \
        }                                                 \
    } while (0)

/**
 * @brief 注册静态文件处理器
 * @return
 */
int regStaticFileHandler();

/**
 * @brief 创建成功响应
 * @param code
 * @param result
 * @return
 */
json createResponse(const int& code, const json& result);

/**
 * @brief 创建错误响应
 * @param code
 * @param msg
 * @return
 */
json createErrResponse(const int& code, const std::string& msg);

// 错误码定义
// 通用错误码
#define ERROR_CODE_UNKNOWN (1000)
#define ERROR_CODE_PARAM_ERR (1001)

// 存放错误码的map
extern std::map<int, std::string> errorCodeMap;
