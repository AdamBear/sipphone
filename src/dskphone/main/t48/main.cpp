
#include "dsklog/dsklog.h"
#include "qt48application.h"
#include "configiddefine.h"
#include "configparser/modconfigparser.h"

#include "mousetest.h"

#ifdef _DEBUGMEM
#include "3rdlib/memory/memory.h"
#endif

#if ENABLE_PROF
#include "prof/prof.h"
#endif

#ifdef IF_ENABLE_TESTABILITY
#include "3rdlib/testability/modtestability.h"
#endif

#include "commonunits/modcommonunits.h"
#include "dskphonedebug/include/dskphonedebug.h"

// 全局初始化完毕的标志
bool g_bAfterInit = true;
#ifndef WIN32
#define MOUSE_TEST      // 控制鼠标测试的宏
#endif

void ParseRunCommand(int argc, char * argv[])
{
    int nArgCount = 0;
    char ** szArgValue = new char * [argc];
    for (int nIndex = 0; nIndex < argc; ++nIndex)
    {
        if (strcmp(argv[nIndex], "-qws") != 0)
        {
            szArgValue[nArgCount++] = argv[nIndex];
        }
    }

    commonUnits_InitLog(nArgCount, szArgValue);
    delete [] szArgValue;
}

int main(int argc, char * argv[])
{
#ifdef IF_FEATURE_TOOLS
    // run debug mode
    if (mkit_exist(mkGetCurrent(), "app_vpPhone")
            || (argc > 1 && strcasecmp(argv[1], "-d") == 0))
    {
        return dskphone_debug_run();
    }
#endif

    comminUnits_DeleteInitInfofile();
    comminUnits_WriteInitInfo("InitTime_LibLoadSucceed = 1");

#if ENABLE_PROF
    dump_maps();
#endif

#ifndef WIN32
    ParseRunCommand(argc, argv);
#endif

    static qT48Application app(argc, argv);

#ifdef MOUSE_TEST
    MouseTestPrivate mouseTest;
    mouseTest.InitMouseTest();
#endif

    app.Init();

#ifdef _DEBUGMEM
    MemoryCheckInit();
#endif

#ifdef IF_ENABLE_TESTABILITY
    MAINWND_INFO("Begin Testability ====");
    Init_Testablity();
#endif

    ////////////////////////////////////////////////////////
    //QT主循环开始执行
    app.exec();
    ////////////////////////////////////////////////////////

    app.UnInit();

    return 0;
}
