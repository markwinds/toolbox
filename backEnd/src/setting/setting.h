#pragma once
#include "log.h"
#include <fstream>
#include <json.hpp>
#include <string>

struct Config {
    LogLevel     logLevel     = LOG_LEVEL_WARN;
    std::string  dataDir      = "./data"; // 数据目录
    bool         remoteAccess = false;    // 是否允许远程访问
    unsigned int port         = 80;       // 监听端口
    std::string  proxyUrl     = "";       // 代理地址

    // 将配置转换为JSON
    void toJson(nlohmann::json& j) const {
        j = nlohmann::json{{"logLevel", static_cast<int>(logLevel)},
                           {"dataDir", dataDir},
                           {"remoteAccess", remoteAccess},
                           {"port", port},
                           {"proxyUrl", proxyUrl}};
    }

    // 从JSON加载配置
    void fromJson(const nlohmann::json& j) {
        if (j.contains("logLevel"))
            logLevel = static_cast<LogLevel>(j["logLevel"].get<int>());
        if (j.contains("dataDir"))
            dataDir = j["dataDir"].get<std::string>();
        if (j.contains("remoteAccess"))
            remoteAccess = j["remoteAccess"].get<bool>();
        if (j.contains("port"))
            port = j["port"].get<unsigned int>();
        if (j.contains("proxyUrl"))
            proxyUrl = j["proxyUrl"].get<std::string>();
    }
};

class Setting {

  public:
    static int init();
    static int regHttpHandler();

    // 获取当前配置
    static Config& getConfig();

    // 保存配置到文件
    static bool saveConfigToFile();

    // 从文件加载配置
    static bool loadConfigFromFile();

    // 重置为默认配置
    static void resetToDefault();

    // 重启程序
    static void restart();

  private:
    static std::string       getCompileTime();
    static Config            config_;
    static const std::string configFilePath_;
};
