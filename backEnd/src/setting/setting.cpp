#include "setting.h"
#include "httpServer.h"
#include <curl.h>
#include <filesystem>
#include <fstream>
#include <string>

#define BASE_URL std::string(LOCAL_API_BASE_URL) + "/setting"
#define TOOLBOX_VERSION string("v1.0.0")
#define GITHUB_OWNER "markwinds"
#define GITHUB_REPO "toolbox"

using namespace std;
namespace fs = std::filesystem;

#define HTTP_HEADER_BROWSER_STR \
    R"(Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.124 Safari/537.36)"

// 静态成员变量初始化
Config            Setting::config_;
const std::string Setting::configFilePath_ = "./config.json";

// 用于存储 curl 响应数据的回调函数
size_t writeCallback(void* contents, size_t size, size_t nmemb, string* userp) {
    userp->append((char*)contents, size * nmemb);
    return size * nmemb;
}

int Setting::init() {
    // 注册处理
    regHttpHandler();
    return 0;
}

int Setting::regHttpHandler() {
    drogon::app().registerHandler(
        BASE_URL + "/version",
        [&](const drogon::HttpRequestPtr&                         req,
            std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
            OK_RESP_STR(TOOLBOX_VERSION + " build_" + getCompileTime());
        },
        {drogon::Get});

    drogon::app().registerHandler(
        BASE_URL + "/config",
        [&](const drogon::HttpRequestPtr&                         req,
            std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
            // 返回当前配置
            OK_RESP_DATA(getConfig());
        },
        {drogon::Get});

    drogon::app().registerHandler(
        BASE_URL + "/saveConfig",
        [&](const drogon::HttpRequestPtr&                         req,
            std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
            int    ret = ERROR_CODE_UNKNOWN;
            string errMsg;
            json   reqJson;

            PARSE_JSON();

            // 更新配置
            getConfig().fromJson(reqJson);
            if (!saveConfigToFile()) {
                logF("save config filed");
                goto exit;
            }
            OK_RESP();
            return;

        exit:
            ERROR_RESP_MSG(ret, errMsg);
            return;
        },
        {drogon::Post});

    drogon::app().registerHandler(
        BASE_URL + "/resetConfig",
        [&](const drogon::HttpRequestPtr&                         req,
            std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
            int    ret = ERROR_CODE_UNKNOWN;
            string errMsg;

            resetToDefault();

            if (!saveConfigToFile()) {
                logF("save config filed");
                goto exit;
            }
            OK_RESP();
            return;

        exit:
            ERROR_RESP_MSG(ret, errMsg);
            return;
        },
        {drogon::Post});

    drogon::app().registerHandler(
        BASE_URL + "/restart",
        [&](const drogon::HttpRequestPtr&                         req,
            std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
            thread([]() {
                // 休眠 1 秒，等待请求处理完成
                std::this_thread::sleep_for(std::chrono::seconds(1));
                // 重启程序
                restart();
            }).detach();
            OK_RESP();
        },
        {drogon::Get});

    drogon::app().registerHandler(
        BASE_URL + "/exit",
        [&](const drogon::HttpRequestPtr&                         req,
            std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
            thread([]() {
                logF("exit program");
                // 休眠 1 秒，等待请求处理完成
                std::this_thread::sleep_for(std::chrono::seconds(1));
                // 退出程序
                exit(0);
            }).detach();
            OK_RESP();
        },
        {drogon::Get});

    drogon::app().registerHandler(
        BASE_URL + "/latestVersion",
        [&](const drogon::HttpRequestPtr&                         req,
            std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
            string url = string("https://api.github.com/repos/") + GITHUB_OWNER + "/" +
                         GITHUB_REPO + "/releases/latest";
            int      ret = ERROR_CODE_UNKNOWN;
            string   errMsg;
            string   response;
            CURLcode res;

            CURL* curl = curl_easy_init();
            if (!curl) {
                logE("init curl failed");
                goto exit;
            }

            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            if (!config_.proxyUrl.empty()) {
                curl_easy_setopt(curl, CURLOPT_PROXY, config_.proxyUrl.c_str());
            }
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
            curl_easy_setopt(curl, CURLOPT_USERAGENT, HTTP_HEADER_BROWSER_STR);

            res = curl_easy_perform(curl);
            if (res != CURLE_OK) {
                errMsg = curl_easy_strerror(res);
                logE("curl_easy_perform() failed: %s", errMsg.c_str());
                goto exit;
            }
            curl_easy_cleanup(curl);
            curl = nullptr;

            OK_RESP_STR(response);

        exit:
            if (curl) {
                curl_easy_cleanup(curl);
            }
            ERROR_RESP_MSG(ret, errMsg);
        },
        {drogon::Get});

    return 0;
}

std::string Setting::getCompileTime() {
    // __DATE__ 和 __TIME__ 宏
    std::string date = __DATE__; // 格式: "MMM DD YYYY"
    std::string time = __TIME__; // 格式: "HH:MM:SS"

    // 月份映射表
    std::map<std::string, std::string> monthMap = {{"Jan", "01"},
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
    std::string month = monthMap[date.substr(0, 3)];
    std::string day   = date.substr(4, 2);
    std::string year  = date.substr(7, 4);

    // 移除多余的空格（确保个位数的日期正确）
    if (day[0] == ' ') {
        day = day.substr(1);
    }

    // 格式化为 YYYY-MM-DD HH:MM:SS
    std::ostringstream formattedDate;
    formattedDate << year << "-" << month << "-" << (day.size() == 1 ? "0" : "") << day << "_"
                  << time;

    return formattedDate.str();
}

Config& Setting::getConfig() {
    return config_;
}

bool Setting::saveConfigToFile() {
    try {
        // 创建配置文件目录（如果不存在）
        fs::path configPath(configFilePath_);
        fs::path dirPath = configPath.parent_path();
        if (!dirPath.empty() && !fs::exists(dirPath)) {
            fs::create_directories(dirPath);
        }

        // 将配置转换为JSON
        nlohmann::json j;
        config_.toJson(j);

        // 写入文件
        std::ofstream file(configFilePath_);
        if (!file.is_open()) {
            logE("can not open file: %s", configFilePath_.c_str());
            return false;
        }

        file << j.dump(4); // 使用缩进以提高可读性
        file.close();

        logI("save config file ok: %s", configFilePath_.c_str());
        return true;
    } catch (const std::exception& e) {
        logE("save config file err: %s", e.what());
        return false;
    }
}

bool Setting::loadConfigFromFile() {
    try {
        // 检查文件是否存在
        if (!fs::exists(configFilePath_)) {
            logW("config file[%s] not exit, user default config", configFilePath_.c_str());
            return false;
        }

        // 读取文件
        std::ifstream file(configFilePath_);
        if (!file.is_open()) {
            logE("can not open config file: %s", configFilePath_.c_str());
            return false;
        }

        // 解析JSON
        nlohmann::json j;
        file >> j;
        file.close();

        // 更新配置
        config_.fromJson(j);

        logI("load config file ok: %s", configFilePath_.c_str());
        return true;
    } catch (const std::exception& e) {
        logE("load config file err: %s", e.what());
        return false;
    }
}

void Setting::resetToDefault() {
    // 重置为默认值
    config_ = Config();
    logW("reset config ok");
}

void Setting::restart() {
    logW("restart server");

#ifdef WIN32
    // 获取当前程序的路径
    char szFilePath[MAX_PATH];
    GetModuleFileName(NULL, szFilePath, MAX_PATH);

    // 启动新进程
    WinExec(szFilePath, SW_SHOWNORMAL);
    // 终止当前进程
    ExitProcess(0);
#else

#endif
}
