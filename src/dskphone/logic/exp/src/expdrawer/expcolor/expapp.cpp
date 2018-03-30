#include "expapp.h"
#include "expmanager.h"
#include "backtrace.h"

#include "msgkit/mkservice.h"
#include <configiddefine.h>
#include "configparser/modconfigparser.h"

LRESULT OnEXPMsg(msgObject & objMessage)
{
    if (CONFIG_MSG_BCAST_CHANGED == objMessage.message)
    {
        if (ST_SYSLOG == objMessage.wParam)
        {
#ifndef _XWIN
            // 从配置中读取log等级
            run_cmd_t  run_cmd;
            memset(&run_cmd, 0, sizeof(run_cmd_t));
            int iLevel = configParser_GetConfigInt(kszServerLogLevel);
//          std::cout << iLevel << "!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
            char strLevel[32] = {0};
            sprintf(strLevel, "ANY=%d", iLevel);
            run_cmd.loglevel_str = strLevel;

            run_cmd_log_apply(&run_cmd);
#endif // !_XWIN
        }

        return FALSE;
    }
    else
    {
        return _EXPManager.OnEXPMessage(objMessage);
    }
}

#if (defined _USE_SKIA) || (defined _XWIN)
CEXPApp::CEXPApp()
    : CMKThread(VP_EXP_COLOR_THREAD_NAME, NULL)
{
#if MK_MSG_USER_DEFINECONFIG
    SetMsgDepth(128);
#endif
}
#else
CEXPApp::CEXPApp(int & argc, char ** argv)
    : QDSKApplication(argc, argv)
{

}
#endif

CEXPApp::~CEXPApp(void)
{
}

#ifdef _XWIN
CEXPApp & CEXPApp::GetInstance()
{
    static CEXPApp instance;
    return instance;
}
#endif

#if (defined _USE_SKIA) || (defined _XWIN)
BOOL CEXPApp::AfterThreadLaunched()
#else
void CEXPApp::Init()
#endif
{
    mkservice_init(NULL, 0);

#ifndef WIN32
    catch_sig();
#endif

    _EXPManager.Init();
#if (defined _USE_SKIA) || (defined _XWIN)
    return true;
#endif
}

#if (defined _USE_SKIA) || (defined _XWIN)
void CEXPApp::BeforeThreadShutdown()
#else
void CEXPApp::Uninit()
#endif
{
    _EXPManager.Uninit();
}
