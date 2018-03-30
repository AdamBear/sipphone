#pragma once

#include <time.h>
#include <sys/timeb.h>
#include <sys/utime.h>
#include <windows.h>

static _inline void usleep(int a)
{
    Sleep((unsigned long)(a / 1000));
}
