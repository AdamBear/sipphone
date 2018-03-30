#include "modtestability.h"

#include <dlfcn.h>
#include <cstdio>

// #include <string>

#include "service_config.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "dsklog/log.h"

typedef yl::string String;

void LoadTestability();
LRESULT OnTestabilityChanged(msgObject& msg);
String GetCmdResult(const String& cmd);


void Init_Testablity()
{
    LoadTestability();
    SingleMsgReg(CONFIG_MSG_BCAST_CHANGED, OnTestabilityChanged);
}

void LoadTestability()
{
    if (GetCmdResult("/boot/bin/userenv -g " userenvGuiTestability) == userenvGuiTestability "=1")
    {
        void* lib = dlopen("libtestability.so", RTLD_LAZY);
        if (lib)
        {
            typedef void(*SOMAINTYPE)(void);
            SOMAINTYPE somain = (SOMAINTYPE)dlsym(lib, "somain");
            if (somain)
            {
                somain();
            }
            else
            {
                APP_INFO("Load Testablity somain failed %s.", strerror(errno));
            }
        }
        else
        {
            APP_INFO("Load Testablity libraries failed %s.", strerror(errno));
        }
    }
}


LRESULT OnTestabilityChanged(msgObject& msg)
{
    if (msg.wParam == ST_TESTABILITY_CHANGED)
    {
        LoadTestability();
        return TRUE;
    }

    return FALSE;
}

String GetCmdResult(const String& cmd)
{
    FILE* fd = popen(cmd.c_str(), "r");
    String output;
    int ret = -1;
    if (fd)
    {
        char buffer[256] = { 0 };
        int readsize = 0;
        while (readsize = fread(buffer, 1, 256, fd))
        {
            output.append(buffer, readsize);
        }

        ret = pclose(fd);
    }

    String::size_type lastChar = output.find_last_not_of("\r\n");
    if (lastChar != String::npos)
    {
        return output.substr(0, lastChar + 1);
    }

    return output;
}
