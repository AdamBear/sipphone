#pragma once

#include "winimp.h"
#include <path_define.h>

#define BUFFER_SIZE         (MAX_PATH * (0x01 << 1))
#define BLOCK_BUFFER_SIZE   (MAX_PATH * (0x01 << 3))

struct LibraryLoadGuard
{
    LibraryLoadGuard(LPCSTR name) : hModule(0)
    {
        if (name)
        {
            hModule = ::LoadLibrary(name);
        }
    }

    FARPROC GetInstance(LPCSTR name)
    {
        return name ? ::GetProcAddress(hModule, name) : NULL;
    }

    ~LibraryLoadGuard()
    {
        if (hModule)
        {
            ::FreeLibrary(hModule);
        }
    }

private:
    HMODULE hModule;
};

#define TRACE_FUCTION \
    { do{ \
        printf("[%s]:[%d]\n", __FUNCTION__, __LINE__ );\
    }while(0); return 0 ; }

#define WIN32_INFO(fmt,...)  do{ SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE); fprintf(stdout, fmt "\n", ##__VA_ARGS__); } while (0)
#define WIN32_WARR(fmt,...)  do{ SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_BLUE | FOREGROUND_RED); fprintf(stdout, fmt "\n", ##__VA_ARGS__); } while (0)
#define WIN32_ERR(fmt,...)   WIN32_WARR(fmt, ##__VA_ARGS__)
#define WIN32_DEBUG(fmt,...) WIN32_INFO(fmt, ##__VA_ARGS__)
