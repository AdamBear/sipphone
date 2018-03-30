#include "expapp.h"

#include <ETLLib/ETLLib.hpp>

#include <configiddefine.h>
#include "configparser/modconfigparser.h"

#ifdef _DEBUGMEM
#include "3rdlib/memory/memory.h"
#endif

#include <dsklog/dsklog.h>
#include "dsklog/log.h"

bool g_bAfterInit = true;

void ParseRunCommand(int argc, char * argv[])
{
#ifdef _USE_SKIA
//  // 从配置中读取log等级
//  int iLevel = configParser_GetConfigInt(kszServerLogLevel);
//
//  /*set log type*/
//  LOG_INIT(LOG_CONFIG_NAME, DBG_LOG_TYPE_LOGCAT, 0, "");
//
//  /*set log level*/
//  LOG_SET(DBG_LOG_ALL, iLevel);
#elif defined _XWIN
#else
    run_cmd_t  run_cmd;
    memset(&run_cmd, 0, sizeof(run_cmd_t));
    int nArgCount = 0;
    char ** szArgValue = new char * [argc];
    for (int nIndex = 0; nIndex < argc; ++nIndex)
    {
        if (strcmp(argv[nIndex], "-qws") != 0)
        {
            szArgValue[nArgCount++] = argv[nIndex];
        }
    }

    //  run_cmd_parse(&run_cmd, argc, argv);
    run_cmd_parse(&run_cmd, nArgCount, szArgValue);
    delete [] szArgValue;
    run_cmd_log_apply(&run_cmd);

    if (!(run_cmd.is_manual_loglevel))
    {
        // 从配置中读取log等级
        int iLevel = configParser_GetConfigInt(kszServerLogLevel);
        char strLevel[32] = {0};
        sprintf(strLevel, "ANY =%d", iLevel);
        run_cmd.loglevel_str = strLevel;//strLevel.c_str();
        run_cmd_log_apply(&run_cmd);
    }
#endif
}

int main(int argc, char * argv[])
{
    ParseRunCommand(argc, argv);

#ifdef _DEBUGMEM
    MemoryCheckInit();
#endif

#ifdef _USE_SKIA
    CEXPApp objExpApp;
    int retCode = objExpApp.Run();
    return retCode;
#elif defined _XWIN
#else

    CEXPApp objExpApp(argc, argv);
    objExpApp.Init();
    int iResult = objExpApp.exec();
    objExpApp.Uninit();
    return iResult;
#endif

}
