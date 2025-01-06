//
// Created by 14640 on 2024/11/7.
//

#pragma once

#include "httplib.h"
#include "json.hpp"

using json = nlohmann::json;

#define LOCAL_API_BASE_URL "/toolbox"

#define OK_RESP(data) do { \
    auto resp = create_response("200", data);\
    res.set_content(resp.dump(), "application/json");\
} while(0)

extern httplib::Server svr;

int reg_static_file_handler();

json create_response(const std::string &code, const json &result);
