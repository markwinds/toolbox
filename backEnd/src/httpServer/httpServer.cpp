//
// Created by 14640 on 2024/11/7.
//

#include "httpServer.h"
#include "json.hpp"
#include "log.h"
#include "staticFile.h"
#include <string>

// 静态文件url前缀
#define STATIC_FILE_PATH "web"

using namespace std;

// 错误码定义和对应的解释
std::map<int, std::string> errorCodeMap = {
    {ERROR_CODE_UNKNOWN, "unknown error"},
    {ERROR_CODE_PARAM_ERR, "param error"},
};

int regStaticFileHandler() {
    // 从程序中载入web静态文件
    initStaticFile();

    // 注册静态文件处理
    drogon::app().registerHandlerViaRegex(
        "/" STATIC_FILE_PATH "/.*",
        [](const drogon::HttpRequestPtr&                         req,
           std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
            vector<char> data;
            string       content_type;

            string path = req->getPath().substr(strlen(STATIC_FILE_PATH) + 2);

            if (0 != staticFileGetFile(path, data, content_type)) {
                logE("not find file: %s", path.c_str());
                auto resp = drogon::HttpResponse::newHttpResponse();
                resp->setStatusCode(drogon::k404NotFound);
                callback(resp);
                return;
            }

            auto resp = drogon::HttpResponse::newHttpResponse();
            resp->setBody(string(data.begin(), data.end()));
            resp->setContentTypeString(content_type);
            callback(resp);
        },
        {drogon::Get});

    // 处理根路径
    drogon::app().registerHandler(
        "/",
        [](const drogon::HttpRequestPtr&                         req,
           std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
            vector<char> data;
            string       path = "index.html";
            string       content_type;

            logD("get static file: %s", path.c_str());

            if (0 != staticFileGetFile(path, data, content_type)) {
                logE("not find file: %s", path.c_str());
                auto resp = drogon::HttpResponse::newHttpResponse();
                resp->setStatusCode(drogon::k404NotFound);
                callback(resp);
                return;
            }

            auto resp = drogon::HttpResponse::newHttpResponse();
            resp->setBody(string(data.begin(), data.end()));
            resp->setContentTypeString(content_type);
            callback(resp);
        },
        {drogon::Get});

    return 0;
}

json createResponse(const int& code, const json& result) {
    json response;
    response["code"]   = code;
    response["result"] = result;
    return response;
}

json createErrResponse(const int& code, const std::string& msg) {
    json response;
    response["code"]    = code;
    response["errStr"]  = errorCodeMap[code];
    response["details"] = msg; // 补充说明错误原因
    return response;
}
