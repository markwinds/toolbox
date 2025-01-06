//
// Created by 14640 on 2024/11/7.
//

#include "httpServer.h"
#include "log.h"
#include "staticFile.h"
#include "json.hpp"
#include <vector>
#include <map>
#include <string>

#define STATIC_FILE_PATH "web"

using namespace httplib;
using namespace std;

httplib::Server svr;

int reg_static_file_handler() {
    string url = "/" + string(STATIC_FILE_PATH) + "/(.*)";

    // 从程序中载入web静态文件
    init_static_file();

    svr.Get(url, [](const httplib::Request &req, httplib::Response &res) {
        vector<char> data;
        string path = req.matches[1];
        string content_type;

        logD("get static file: %s", path.c_str());

        if (0 != static_file_get_file(path, data, content_type)) {
            logE("not find file: %s", path.c_str());
            res.status = 404;
            return;
        }
        res.set_content(data.data(), data.size(), content_type);
    });

    // /返回index.html
    svr.Get("/", [](const httplib::Request &req, httplib::Response &res) {
        vector<char> data;
        string path = "index.html";
        string content_type;

        logD("get static file: %s", path.c_str());

        if (0 != static_file_get_file(path, data, content_type)) {
            logE("not find file: %s", path.c_str());
            res.status = 404;
            return;
        }
        res.set_content(data.data(), data.size(), content_type);
    });

    return 0;
}

json create_response(const std::string &code, const json &result) {
    json response;
    response["code"] = code;
    response["result"] = result;
    return response;
}
