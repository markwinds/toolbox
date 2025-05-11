#pragma once

#include "singleton.h"
#include <map>
#include <string>
#include <vector>
#include <windows.h>

using MenuHandler = void (*)();

struct TrayInfo {
    std::wstring title;
    MenuHandler  handler;
};

class Tray {

  public:
    int addTray(const HINSTANCE& hInstance, const std::wstring& title);

    int regMenu(const std::wstring& menu_name, MenuHandler handler);

  private:
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

  private:
    std::vector<TrayInfo> menuList;
};

inline Tray& getTrayInstance() {
    return Singleton<Tray>::get_instance();
}
