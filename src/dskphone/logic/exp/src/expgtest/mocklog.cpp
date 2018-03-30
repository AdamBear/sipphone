#include <stdio.h>
#include <stdarg.h>

extern "C"
{
    void log_log(int module, int log_level, const char * fmt, ...)
    {
        va_list args;
        char msg[1024] = { 0 };
        va_start(args, fmt);
        vsprintf(msg, fmt, args);
        va_end(args);
        /*printf("#LOG# mod[%d] level[%d] %s", module, log_level, msg);*/
        printf("-------------------------%s", msg);
    }
}
