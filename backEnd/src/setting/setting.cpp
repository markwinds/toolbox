#include "setting.h"
#include "httpServer.h"
#include <curl.h>
#include <string>
#include <fstream>
#include <filesystem>

#define BASE_URL std::string(LOCAL_API_BASE_URL) + "/setting"
#define TOOLBOX_VERSION string("v1.0.0")
#define GITHUB_OWNER "markwinds"
#define GITHUB_REPO "toolbox"
#define HTTP_PROXY "http://127.0.0.1:10809" // 设置代理地址

using namespace std;
namespace fs = std::filesystem;

// 静态成员变量初始化
Config Setting::config_;
const std::string Setting::config_file_path_ = "./config.json";

// 用于存储 curl 响应数据的回调函数
size_t write_callback(void* contents, size_t size, size_t nmemb, string* userp) {
    userp->append((char*)contents, size * nmemb);
    return size * nmemb;
}

int Setting::init() {
    // 尝试从文件加载配置
    load_config_from_file();

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

    drogon::app().registerHandler(
        BASE_URL + "/config",
        [&](const drogon::HttpRequestPtr&                         req,
            std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
            // 返回当前配置
            OK_RESP(get_config());
        },
        {drogon::Get});

    drogon::app().registerHandler(
        BASE_URL + "/saveConfig",
        [&](const drogon::HttpRequestPtr&                         req,
            std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
            auto json = req->getJsonObject();
            if (!json) {
                auto resp = drogon::HttpResponse::newHttpResponse();
                resp->setStatusCode(drogon::k400BadRequest);
                callback(resp);
                return;
            }

            try {
                // 从请求中获取JSON数据
                auto reqJson = nlohmann::json::parse(json->asString());

                // 更新配置
                get_config().from_json(reqJson);

                // 保存到文件
                if (save_config_to_file()) {
                    OK_RESP_STR("配置已保存");
                } else {
                    auto resp = drogon::HttpResponse::newHttpResponse();
                    resp->setStatusCode(drogon::k500InternalServerError);
                    resp->setBody("保存配置失败");
                    callback(resp);
                }
            } catch (const std::exception& e) {
                auto resp = drogon::HttpResponse::newHttpResponse();
                resp->setStatusCode(drogon::k400BadRequest);
                resp->setBody(std::string("无效的配置数据: ") + e.what());
                callback(resp);
            }
        },
        {drogon::Post});

    drogon::app().registerHandler(
        BASE_URL + "/resetConfig",
        [&](const drogon::HttpRequestPtr&                         req,
            std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
            // 重置为默认配置
            reset_to_default();

            // 保存到文件
            if (save_config_to_file()) {
                OK_RESP_STR("配置已重置为默认值");
            } else {
                auto resp = drogon::HttpResponse::newHttpResponse();
                resp->setStatusCode(drogon::k500InternalServerError);
                resp->setBody("重置配置失败");
                callback(resp);
            }
        },
        {drogon::Post});

    drogon::app().registerHandler(
        BASE_URL + "/latestVersion",
        [&](const drogon::HttpRequestPtr&                         req,
            std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
            string url = string("https://api.github.com/repos/") + GITHUB_OWNER + "/" +
                         GITHUB_REPO + "/releases/latest";

            CURL* curl = curl_easy_init();
            if (!curl) {
                auto err_resp = drogon::HttpResponse::newHttpResponse();
                err_resp->setStatusCode(drogon::k500InternalServerError);
                callback(err_resp);
                return;
            }

            string response;
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_PROXY, HTTP_PROXY);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); // 跳过 SSL 验证
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
            curl_easy_setopt(
                curl,
                CURLOPT_USERAGENT,
                R"(Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.124 Safari/537.36)");

            CURLcode res = curl_easy_perform(curl);
            curl_easy_cleanup(curl);

            if (res != CURLE_OK) {
                auto err_resp = drogon::HttpResponse::newHttpResponse();
                err_resp->setStatusCode(drogon::k500InternalServerError);
                callback(err_resp);
                return;
            }

            auto resp = drogon::HttpResponse::newHttpResponse();
            resp->setBody(response);
            callback(resp);
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

Config& Setting::get_config() {
    return config_;
}

bool Setting::save_config_to_file() {
    try {
        // 创建配置文件目录（如果不存在）
        fs::path config_path(config_file_path_);
        fs::path dir_path = config_path.parent_path();
        if (!dir_path.empty() && !fs::exists(dir_path)) {
            fs::create_directories(dir_path);
        }

        // 将配置转换为JSON
        nlohmann::json j;
        config_.to_json(j);

        // 写入文件
        std::ofstream file(config_file_path_);
        if (!file.is_open()) {
            logE("无法打开配置文件进行写入: %s", config_file_path_.c_str());
            return false;
        }

        file << j.dump(4); // 使用缩进以提高可读性
        file.close();

        logI("配置已保存到文件: %s", config_file_path_.c_str());
        return true;
    } catch (const std::exception& e) {
        logE("保存配置到文件时出错: %s", e.what());
        return false;
    }
}

bool Setting::load_config_from_file() {
    try {
        // 检查文件是否存在
        if (!fs::exists(config_file_path_)) {
            logW("配置文件不存在: %s, 将使用默认配置", config_file_path_.c_str());
            return false;
        }

        // 读取文件
        std::ifstream file(config_file_path_);
        if (!file.is_open()) {
            logE("无法打开配置文件: %s", config_file_path_.c_str());
            return false;
        }

        // 解析JSON
        nlohmann::json j;
        file >> j;
        file.close();

        // 更新配置
        config_.from_json(j);

        logI("从文件加载配置成功: %s", config_file_path_.c_str());
        return true;
    } catch (const std::exception& e) {
        logE("从文件加载配置时出错: %s", e.what());
        return false;
    }
}

void Setting::reset_to_default() {
    // 重置为默认值
    config_ = Config();
    logI("配置已重置为默认值");
}
