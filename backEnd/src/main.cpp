
#include "httplib.h"
#include "log.h"

#if defined(_WIN32) || defined(_WIN64)

#include "rc.h"
#include "tray.h"

#endif

void do_main() {

    // HTTP
    httplib::Server svr;

    svr.Get("/hi",
            [](const httplib::Request &, httplib::Response &res) { res.set_content("Hello World!", "text/plain"); });

    logW("=================program start==================");

    svr.listen("0.0.0.0", 8080);
}


#if defined(_WIN32) || defined(_WIN64)

// Windows GUI程序入口
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {

    // 为程序添加托盘图标
    get_tray_instance().add_tray(hInstance, L"toolbox");
    get_tray_instance().reg_menu(L"good", []() { logI("good"); });
    get_tray_instance().reg_menu(L"ok", []() { logI("ok"); });

    do_main();

    return 0;
}

#endif





