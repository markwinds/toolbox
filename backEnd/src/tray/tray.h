#pragma once

#include "singleton.h"
#include <map>
#include <string>
#include <windows.h>

using MenuHandler = void (*)();

class Tray {

  public:
    int addTray(const HINSTANCE& hInstance, const std::wstring& title);

    int regMenu(const std::wstring& menu_name, MenuHandler handler);

  private:
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

  private:
    std::map<std::wstring, MenuHandler> menuMap;
};

inline Tray& getTrayInstance() {
    return Singleton<Tray>::get_instance();
}
