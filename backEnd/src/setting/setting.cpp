#include "setting.h"
#include "httpServer.h"
#include "util.h"
#include <curl.h>
#include <filesystem>
#include <fstream>
#include <string>

#ifndef TOOLBOX_VERSION
#define TOOLBOX_VERSION "v1.0.0"
#endif

#define BASE_URL std::string(LOCAL_API_BASE_URL) + "/setting"
#define GITHUB_OWNER "markwinds"
#define GITHUB_REPO "toolbox"

#define DOWNLOAD_PROGRAM_FILENAME "toolbox.zip"
#define L_DOWNLOAD_PROGRAM_FILENAME L"toolbox.zip"

#define OLD_PROGRAM_FILENAME ".toolbox.exe"
#define L_OLD_PROGRAM_FILENAME L".toolbox.exe"

using namespace std;
namespace fs = std::filesystem;

// 静态成员变量初始化
Config            Setting::config_;
const std::string Setting::configFilePath_ = "./config.json";

int Setting::init() {
    // 注册处理
    regHttpHandler();

    // 删除可能存在的老文件
    remove(OLD_PROGRAM_FILENAME);
    return 0;
}

std::string Setting::getVersionInfo() {
    return string(TOOLBOX_VERSION) + " build_" + getCompileTime();
}

int Setting::regHttpHandler() {
    drogon::app().registerHandler(
        BASE_URL + "/version",
        [&](const drogon::HttpRequestPtr&                         req,
            std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
            OK_RESP_STR(getVersionInfo());
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

            PARSE_JSON(reqJson, req->getBody());

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
                restartProgram();
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
            json     latestRespJson;
            string   latestVersionStr;

            CURL* curl = curl_easy_init();
            if (!curl) {
                logE("init curl failed");
                goto exit;
            }

            configCurl(curl);
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

            res = curl_easy_perform(curl);
            if (res != CURLE_OK) {
                errMsg = curl_easy_strerror(res);
                logE("curl_easy_perform() failed: %s", errMsg.c_str());
                goto exit;
            }
            curl_easy_cleanup(curl);
            curl = nullptr;

            //            logD("resp:\n%s", response.c_str());
            PARSE_JSON(latestRespJson, response);
            latestVersionStr = latestRespJson["name"];
            OK_RESP_STR(latestVersionStr);

        exit:
            if (curl) {
                curl_easy_cleanup(curl);
            }
            ERROR_RESP_MSG(ret, errMsg);
        },
        {drogon::Get});

    drogon::app().registerHandler(
        BASE_URL + "/update",
        [&](const drogon::HttpRequestPtr&                         req,
            std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
            int    ret = ERROR_CODE_UNKNOWN;
            string errMsg;
            string url = string("https://api.github.com/repos/") + GITHUB_OWNER + "/" +
                         GITHUB_REPO + "/releases/latest";
            string downloadUrl = string("https://github.com/") + GITHUB_OWNER + "/" + GITHUB_REPO +
                                 "/releases/download/";
            string   response;
            CURLcode res;
            json     latestRespJson;
            string   latestVersionStr;
            string   tmpDownloadFilename = DOWNLOAD_PROGRAM_FILENAME;
            FILE*    fd                  = nullptr;

            CURL* curl = curl_easy_init();
            if (!curl) {
                logE("init curl failed");
                goto exit;
            }

            configCurl(curl);
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

            res = curl_easy_perform(curl);
            if (res != CURLE_OK) {
                errMsg = curl_easy_strerror(res);
                logE("curl_easy_perform() failed: %s", errMsg.c_str());
                goto exit;
            }
            PARSE_JSON(latestRespJson, response);
            latestVersionStr = latestRespJson["name"];

            if (compareVersions(latestVersionStr, TOOLBOX_VERSION) <= 0) {
                errMsg = "not need update";
                logW("not need update, latestVersion:%s nowVersion:%s",
                     latestVersionStr.c_str(),
                     TOOLBOX_VERSION);
                goto exit;
            }
            downloadUrl += latestVersionStr;
            downloadUrl += "/";
            downloadUrl += tmpDownloadFilename;
            logI("download from url:%s", downloadUrl.c_str());

            curl_easy_setopt(curl, CURLOPT_URL, downloadUrl.c_str());

            remove(tmpDownloadFilename.c_str());
            fd = fopen(tmpDownloadFilename.c_str(), "ab");
            if (fd == nullptr) {
                errMsg = "can not open file:" + tmpDownloadFilename;
                logE("%s", errMsg.c_str());
                goto exit;
            }
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &fd);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlRespFile);

            res = curl_easy_perform(curl);
            if (res != CURLE_OK) {
                errMsg = curl_easy_strerror(res);
                logE("curl_easy_perform() failed: %s", errMsg.c_str());
                goto exit;
            }

            curl_easy_cleanup(curl);
            curl = nullptr;
            fclose(fd);
            fd = nullptr;

            // 解压替换程序
            if (updateProgramFile() != 0) {
                errMsg = "update program file failed";
                logE("%s", errMsg.c_str());
                goto exit;
            }
            remove(DOWNLOAD_PROGRAM_FILENAME);

            restartProgram();

            OK_RESP();
            return;

        exit:
            if (curl) {
                curl_easy_cleanup(curl);
            }
            if (fd) {
                fclose(fd);
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

int Setting::compareVersions(const std::string& version1, const std::string& version2) {
    // 移除版本号中的 'v' 前缀
    std::string v1 = version1[0] == 'v' ? version1.substr(1) : version1;
    std::string v2 = version2[0] == 'v' ? version2.substr(1) : version2;

    // 将版本号按照 '.' 分割
    std::vector<int>  parts1;
    std::vector<int>  parts2;
    std::stringstream ss1(v1);
    std::stringstream ss2(v2);
    std::string       item;

    // 解析第一个版本号
    while (std::getline(ss1, item, '.')) {
        parts1.push_back(std::stoi(item));
    }

    // 解析第二个版本号
    while (std::getline(ss2, item, '.')) {
        parts2.push_back(std::stoi(item));
    }

    // 补齐长度（用0填充）
    size_t maxSize = parts1.size() > parts2.size() ? parts1.size() : parts2.size();
    while (parts1.size() < maxSize)
        parts1.push_back(0);
    while (parts2.size() < maxSize)
        parts2.push_back(0);

    // 逐位比较
    for (size_t i = 0; i < maxSize; i++) {
        if (parts1[i] > parts2[i])
            return 1;
        if (parts1[i] < parts2[i])
            return -1;
    }

    return 0;
}

void Setting::resetToDefault() {
    // 重置为默认值
    config_ = Config();
    logW("reset config ok");
}

bool Setting::renameExecutable(const std::wstring& oldPath, const std::wstring& newName) {
    // 使用 std::filesystem 解析路径
    fs::path exePath(oldPath);
    fs::path parentDir = exePath.parent_path();
    fs::path newPath   = parentDir / newName;

    try {
        // 尝试重命名文件
        fs::rename(oldPath, newPath);
        logW("Successfully renamed %ls to %ls", oldPath.c_str(), newPath.c_str());
        return true;
    } catch (const fs::filesystem_error& e) {
        logE("rename %ls to %ls failed", oldPath.c_str(), newPath.c_str());
        return false;
    }
}

int Setting::updateProgramFile() {
    // 修改当前程序文件名
    auto filePath = getExecutablePath();

    if (filePath.empty()) {
        logE("get executable path failed");
        return -1;
    }
    if (!renameExecutable(filePath, L_OLD_PROGRAM_FILENAME)) {
        logE("rename file failed");
        return -1;
    }

    // 解压zip包
    if (decompressZipData(L_DOWNLOAD_PROGRAM_FILENAME) != 0) {
        logE("decompress zip file failed");
        return -1;
    }

    return 0;
}
