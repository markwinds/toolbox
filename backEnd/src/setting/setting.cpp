#include "setting.h"
#include "httpServer.h"
#include <string>

#define BASE_URL std::string(LOCAL_API_BASE_URL) + "/setting"
#define TOOLBOX_VERSION string("v1.0.0")

using namespace std;

int Setting::init() {
    // 注册处理
    reg_http_handler();
    return 0;
}

int Setting::reg_http_handler() {
    drogon::app().registerHandler(
        BASE_URL + "/version",
        [&](const drogon::HttpRequestPtr&                         req,
            std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
            OK_RESP_STR(TOOLBOX_VERSION + " build_" + get_compile_time());
        },
        {drogon::Get});

    return 0;
}

std::string Setting::get_compile_time() {
    // __DATE__ 和 __TIME__ 宏
    std::string date = __DATE__; // 格式: "MMM DD YYYY"
    std::string time = __TIME__; // 格式: "HH:MM:SS"

    // 月份映射表
    std::map<std::string, std::string> month_map = {{"Jan", "01"},
                                                    {"Feb", "02"},
                                                    {"Mar", "03"},
                                                    {"Apr", "04"},
                                                    {"May", "05"},
                                                    {"Jun", "06"},
                                                    {"Jul", "07"},
                                                    {"Aug", "08"},
                                                    {"Sep", "09"},
                                                    {"Oct", "10"},
                                                    {"Nov", "11"},
                                                    {"Dec", "12"}};

    // 解析日期
    std::string month = month_map[date.substr(0, 3)];
    std::string day   = date.substr(4, 2);
    std::string year  = date.substr(7, 4);

    // 移除多余的空格（确保个位数的日期正确）
    if (day[0] == ' ') {
        day = day.substr(1);
    }

    // 格式化为 YYYY-MM-DD HH:MM:SS
    std::ostringstream formatted_date;
    formatted_date << year << "-" << month << "-" << (day.size() == 1 ? "0" : "") << day << "_"
                   << time;

    return formatted_date.str();
}
