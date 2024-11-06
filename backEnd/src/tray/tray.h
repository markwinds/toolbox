//
// Created by 14640 on 2024/11/3.
//

#pragma once

#include "singleton.h"
#include <windows.h>
#include <map>
#include <string>

using MenuHandler = void (*)();

class Tray {

public:
    int add_tray(const HINSTANCE &hInstance, const std::wstring &title);

    int reg_menu(const std::wstring &menu_name, MenuHandler handler);

private:
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
    std::map<std::wstring, MenuHandler> menu_map;

};

inline Tray &get_tray_instance() {
    return Singleton<Tray>::getInstance();
}
