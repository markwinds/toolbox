#pragma once

#include "singleton.h"
#include <map>
#include <string>
#include <windows.h>

using MenuHandler = void (*)();

class Tray {

  public:
    int add_tray(const HINSTANCE& hInstance, const std::wstring& title);

    int reg_menu(const std::wstring& menu_name, MenuHandler handler);

  private:
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

  private:
    std::map<std::wstring, MenuHandler> menu_map;
};

inline Tray& get_tray_instance() {
    return Singleton<Tray>::get_instance();
}
