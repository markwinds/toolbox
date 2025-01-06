
#include "httpServer.h"
#include "httplib.h"
#include "log.h"
#include "setting.h"

#if defined(_WIN32) || defined(_WIN64)

#include "rc.h"
#include "tray.h"

#endif

void do_main() {
    logW("=================program start==================");

    // web静态文件处理
    reg_static_file_handler();
    logD("reg_static_file_handler success");

    Setting::get_instance().init();

    svr.listen("0.0.0.0", 80);
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
