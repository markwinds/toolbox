//
// Created by 14640 on 2024/11/7.
//

#include "httpServer.h"
#include "log.h"
#include "staticFile.h"
#include <vector>
#include <map>
#include <string>

#define STATIC_FILE_PATH "web"

using namespace httplib;
using namespace std;

httplib::Server svr;

map<string, string> file_content_type_map = {
        {".html", "text/html"},
        {".css", "text/css"},
        {".js", "application/javascript"},
        {".png", "image/png"},
        {".jpg", "image/jpeg"},
        {".jpeg", "image/jpeg"},
        {".gif", "image/gif"},
        {".ico", "image/x-icon"},
};

void get_file_type_by_path(const string &path, string &content_type) {

    // 获取文件后缀名
    string suffix = path.substr(path.find_last_of('.'));

    // 根据后缀名获取文件类型
    if (file_content_type_map.find(suffix) != file_content_type_map.end()) {
        content_type = file_content_type_map[suffix];
    } else {
        content_type = "text/plain";
    }

}

int reg_static_file_handler() {
    string url = "/" + string(STATIC_FILE_PATH) + "/(.*)";

    // 从程序中载入web静态文件
    init_static_file();

    svr.Get(url, [](const httplib::Request &req, httplib::Response &res) {
        vector<char> data;
        string path = req.matches[1];
        string content_type;

        logD("get static file: %s", path.c_str());

        get_file_type_by_path(path, content_type);

        if (0 != static_file_get_file(path, data)) {
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

        get_file_type_by_path(path, content_type);

        if (0 != static_file_get_file(path, data)) {
            logE("not find file: %s", path.c_str());
            res.status = 404;
            return;
        }
        res.set_content(data.data(), data.size(), content_type);
    });

    return 0;
}

