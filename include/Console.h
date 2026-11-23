#pragma once
#ifdef _WIN32
#include <windows.h>
inline void setConsoleColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}
#else
inline void setConsoleColor(int) {}
#endif
