#pragma once
#include "log.h"
#include <fstream>
#include <json.hpp>
#include <string>

struct Config {
    LogLevel     log_level     = LOG_LEVEL_DEBUG;
    std::string  data_dir      = "./data"; // 数据目录
    bool         remote_access = false;    // 是否允许远程访问
    unsigned int port          = 6666;     // 监听端口
    std::string  proxy_url     = "";       // 代理地址

    // 将配置转换为JSON
    void to_json(nlohmann::json& j) const {
        j = nlohmann::json{{"log_level", static_cast<int>(log_level)},
                           {"data_dir", data_dir},
                           {"remote_access", remote_access},
                           {"port", port},
                           {"proxy_url", proxy_url}};
    }

    // 从JSON加载配置
    void from_json(const nlohmann::json& j) {
        if (j.contains("log_level"))
            log_level = static_cast<LogLevel>(j["log_level"].get<int>());
        if (j.contains("data_dir"))
            data_dir = j["data_dir"].get<std::string>();
        if (j.contains("remote_access"))
            remote_access = j["remote_access"].get<bool>();
        if (j.contains("port"))
            port = j["port"].get<unsigned int>();
        if (j.contains("proxy_url"))
            proxy_url = j["proxy_url"].get<std::string>();
    }
};

class Setting {

  public:
    static int init();
    static int reg_http_handler();

    // 获取当前配置
    static Config& get_config();

    // 保存配置到文件
    static bool save_config_to_file();

    // 从文件加载配置
    static bool load_config_from_file();

    // 重置为默认配置
    static void reset_to_default();

  private:
    static std::string       get_compile_time();
    static Config            config_;
    static const std::string config_file_path_;
};
