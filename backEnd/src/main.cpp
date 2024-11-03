
#include "httplib.h"

#if defined(_WIN32) || defined(_WIN64)
#include "rc.h"
#endif

void do_main()
{

    // HTTP
    httplib::Server svr;

    svr.Get("/hi",
            [](const httplib::Request &, httplib::Response &res)
            { res.set_content("Hello World!", "text/plain"); });

    svr.listen("0.0.0.0", 8080);
}

#if defined(_WIN32) || defined(_WIN64)
// Windows GUI程序入口
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    // 为exe程序添加图标
    load_rc();

    do_main();

    return 0;
}
#endif
