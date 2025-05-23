//
// Created by 14640 on 2024/11/3.
//

#if defined(_WIN32) || defined(_WIN64)

#include "tray.h"
#include "log.h"
#include "rc.h"
#include <windows.h>

LRESULT CALLBACK Tray::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

    int wmId = 0;

    switch (uMsg) {
    case WM_APP:                                                // 处理托盘图标的点击消息
        if (lParam == WM_RBUTTONUP || lParam == WM_LBUTTONUP) { // 点击
            POINT cursorPos;
            GetCursorPos(&cursorPos);

            // 创建菜单
            HMENU hMenu = CreatePopupMenu();

            int count = 0;
            for (auto& item : getTrayInstance().menuList) {
                AppendMenuW(hMenu, MF_STRING, ++count, item.title.c_str());
            }

            // 显示菜单
            SetForegroundWindow(hwnd); // 确保窗口在最前面
            TrackPopupMenu(hMenu, TPM_RIGHTBUTTON, cursorPos.x, cursorPos.y, 0, hwnd, nullptr);
            DestroyMenu(hMenu); // 销毁菜单
        }
        break;

    case WM_COMMAND:

        wmId = LOWORD(wParam);
        if (wmId > 0 && wmId <= getTrayInstance().menuList.size()) {
            auto it = getTrayInstance().menuList.begin();
            std::advance(it, wmId - 1);
            it->handler();
        }

        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        break;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int Tray::addTray(HINSTANCE const& hInstance, const std::wstring& title) {

    std::thread t([=] {
        // 创建窗口类
        const char CLASS_NAME[] = "Sample Tray Icon";

        WNDCLASS wc      = {};
        wc.lpfnWndProc   = WindowProc;
        wc.hInstance     = hInstance;
        wc.lpszClassName = CLASS_NAME;

        RegisterClass(&wc);

        // 创建窗口
        HWND hwnd = CreateWindowEx(0,
                                   CLASS_NAME,
                                   "Sample Tray Icon",
                                   WS_OVERLAPPEDWINDOW,
                                   CW_USEDEFAULT,
                                   CW_USEDEFAULT,
                                   CW_USEDEFAULT,
                                   CW_USEDEFAULT,
                                   nullptr,
                                   nullptr,
                                   hInstance,
                                   nullptr);

        // 设置托盘图标
        NOTIFYICONDATA nid   = {};
        nid.cbSize           = sizeof(nid);
        nid.hWnd             = hwnd;
        nid.uID              = 1;
        nid.uFlags           = NIF_ICON | NIF_MESSAGE | NIF_TIP;
        nid.uCallbackMessage = WM_APP; // 自定义消息
        nid.hIcon            = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));

        wcstombs_s(
            nullptr, nid.szTip, title.c_str(), sizeof(nid.szTip) - 1); // 复制到szTip，确保不会溢出

        Shell_NotifyIcon(NIM_ADD, &nid);

        // 消息循环
        MSG msg;
        while (GetMessage(&msg, nullptr, 0, 0)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    });
    t.detach();

    return 0;
}

int Tray::regMenu(const std::wstring& menu_name, MenuHandler handler) {
    menuList.emplace_back(TrayInfo{.title = menu_name, .handler = handler});
    return 0;
}

#endif
