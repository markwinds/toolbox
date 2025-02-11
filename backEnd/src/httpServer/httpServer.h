//
// Created by 14640 on 2024/11/7.
//

#pragma once

#include "json.hpp"
#include <drogon/drogon.h>

using json = nlohmann::json;

#define LOCAL_API_BASE_URL "/toolbox"

#define OK_RESP(data)                                                     \
    do {                                                                  \
        auto resp = create_response("200", data);                         \
        callback(drogon::HttpResponse::newHttpJsonResponse(resp.dump())); \
    } while (0)

int reg_static_file_handler();

json create_response(const std::string& code, const json& result);
