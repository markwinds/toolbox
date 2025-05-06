#include "httpServer.h"
#include "log.h"
#include "setting.h"
#include <drogon/drogon.h>
#include <map>
#include <string>

#if defined(_WIN32) || defined(_WIN64)
#include "rc.h"
#include "tray.h"
#endif

std::map<std::string, std::string> argMap;

// 解析命令行参数，格式为key1=val1,key2=val2,key3,key4
// 其中key3,key4这样没有值的参数会被设置为"true"
void parseArgs(const std::string& args) {
    if (args.empty()) {
        return;
    }

    size_t start = 0;
    size_t end   = 0;

    while (end != std::string::npos) {
        // 查找下一个逗号分隔符
        end = args.find(',', start);

        // 提取当前参数
        std::string param =
            args.substr(start, (end == std::string::npos) ? std::string::npos : end - start);

        // 查找等号
        size_t equalPos = param.find('=');
        if (equalPos != std::string::npos) {
            // 有等号，解析为key=value
            std::string key   = param.substr(0, equalPos);
            std::string value = param.substr(equalPos + 1);
            argMap[key]       = value;
        } else {
            // 没有等号，将值设为"true"
            argMap[param] = "true";
        }

        // 移动到下一个参数
        start = (end == std::string::npos) ? end : end + 1;
    }
}

// 浏览器打开一个tab页
void openBrowser(const std::string& url) {
#if defined(_WIN32) || defined(_WIN64)
    std::string command = "start " + url;
#elif defined(__APPLE__)
    std::string command = "open " + url;
#else
    std::string command = "xdg-open " + url;
#endif

    int result = system(command.c_str());
    if (result != 0) {
        logE("打开浏览器失败，错误码: %d", result);
    }
}

void doMain() {
    // 设置初始日志级别为INFO，确保启动信息能够被记录
    setLogLevel(LOG_LEVEL_INFO);

    logW("=================program start==================");

    // 尝试从文件加载配置
    Setting::loadConfigFromFile();
    auto config = Setting::getConfig();

    // 确保日志级别已应用（loadConfigFromFile中已经应用，这里是双重保障）
    setLogLevel(config.logLevel);
    logI("Current log level: %d", static_cast<int>(getLogLevel()));

    // 配置drogon
    drogon::app().addListener(config.remoteAccess ? "0.0.0.0" : "127.0.0.1", config.port);

    // web静态文件处理
    regStaticFileHandler();
    logD("regStaticFileHandler success");

    // 注册业务处理
    Setting::init();

    if (argMap.find("dev") == argMap.end()) {
        // 非dev模式下打开浏览器 dev模式下因为有web前端调试界面就不用再打开界面了
        // 起一个线程
        std::thread([]() {
            std::this_thread::sleep_for(std::chrono::microseconds(200));
            // 浏览器打开一个tab页
            openBrowser("http://127.0.0.1:" + std::to_string(Setting::getConfig().port));
        }).detach();
    }

    // 启动drogon
    drogon::app().run();
}

#if defined(_WIN32) || defined(_WIN64)
// Windows GUI程序入口
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR lpCmdLine, int nCmdShow) {
    // 为程序添加托盘图标
    getTrayInstance().addTray(hInstance, L"toolbox");
    getTrayInstance().regMenu(L"good", []() { logI("good"); });
    getTrayInstance().regMenu(L"退出", []() {
        logW("tray exit program");
        exit(0);
    });

    std::string cmdLine(lpCmdLine);
    parseArgs(cmdLine);

    doMain();
    return 0;
}
#endif

#if !defined(_WIN32) && !defined(_WIN64)
// 标准程序入口（非Windows平台）
int main(int argc, char* argv[]) {
    // 解析命令行参数
    if (argc > 1) {
        std::string args;
        for (int i = 1; i < argc; ++i) {
            if (i > 1) {
                args += ",";
            }
            args += argv[i];
        }
        parseArgs(args);
    }

    doMain();
    return 0;
}
#endif