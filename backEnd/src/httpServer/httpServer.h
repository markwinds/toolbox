#pragma once

#include "json.hpp"
#include "log.h"
#include <drogon/drogon.h>

using json = nlohmann::json;

#define LOCAL_API_BASE_URL "/toolbox"

#define OK_RESP(data)                                            \
    do {                                                         \
        json j;                                                  \
        data.to_json(j);                                         \
        auto resp     = create_response(200, j);                 \
        auto httpResp = drogon::HttpResponse::newHttpResponse(); \
        httpResp->setBody(resp.dump());                          \
        httpResp->addHeader("Content-Type", "application/json"); \
        callback(httpResp);                                      \
    } while (0)

#define OK_RESP_STR(data)                                        \
    do {                                                         \
        auto resp     = create_response(200, data);              \
        auto httpResp = drogon::HttpResponse::newHttpResponse(); \
        httpResp->setBody(resp.dump());                          \
        httpResp->addHeader("Content-Type", "application/json"); \
        callback(httpResp);                                      \
    } while (0)

int reg_static_file_handler();

json create_response(const int& code, const json& result);
