#include "httpServer.h"
#include "log.h"
#include "setting.h"
#include <drogon/drogon.h>

#if defined(_WIN32) || defined(_WIN64)
#include "rc.h"
#include "tray.h"
#endif

// 浏览器打开一个tab页
void open_browser(const std::string& url) {
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

void do_main() {
    logW("=================program start==================");

    // 配置drogon
    drogon::app().addListener("0.0.0.0", 80);

    // web静态文件处理
    reg_static_file_handler();
    logD("reg_static_file_handler success");

    Setting::get_instance().init();

    // 起一个线程
    std::thread([]() {
        std::this_thread::sleep_for(std::chrono::microseconds(200));
        // 浏览器打开一个tab页
        open_browser("http://127.0.0.1:80");
    }).detach();

    // 启动drogon
    drogon::app().run();
}

#if defined(_WIN32) || defined(_WIN64)
// Windows GUI程序入口
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    // 为程序添加托盘图标
    get_tray_instance().add_tray(hInstance, L"toolbox");
    get_tray_instance().reg_menu(L"good", []() { logI("good"); });
    get_tray_instance().reg_menu(L"退出", []() {
        logW("tray exit program");
        exit(0);
    });

    do_main();
    return 0;
}
#endif
