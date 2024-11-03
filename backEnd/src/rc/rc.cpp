//
// Created by 14640 on 2024/11/3.
//

#include <windows.h>

#define IDI_ICON1 101

void load_rc() {
    HINSTANCE hInstance = GetModuleHandle(NULL);
    HICON hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
}
