#include "expappblack.h"

#include "expmanagerbase.h"
#include "expmanagerblack.h"

#include "msgkit/mkservice.h"
#include <configiddefine.h>
#include "configparser/modconfigparser.h"

bool OnEXPMessage(msgObject& objMessage)
{
    //if (CONFIG_MSG_BCAST_CHANGED == objMessage.message)
    //{
    //    if (ST_SYSLOG == objMessage.wParam)
    //    {
    //        // 从配置中读取log等级
    //        run_cmd_t  run_cmd;
    //        memset(&run_cmd, 0, sizeof(run_cmd_t));
    //        int iLevel = configParser_GetConfigInt(kszServerLogLevel);
    //        char strLevel[32] = { 0 };
    //        sprintf(strLevel, "ANY=%d", iLevel);
    //        run_cmd.loglevel_str = strLevel;

    //        run_cmd_log_apply(&run_cmd);
    //    }

    //    return FALSE;
    //}
    //else
    {
        if (objMessage.message != TM_TIMER
                && objMessage.message != EXP_MESSAGE_UPDATE_KEY
                && objMessage.message != EXP_MESSAGE_BG_CHANGE
                && objMessage.message != EXP_MESSAGE_BACKLIGHT
                && objMessage.message != EXP_MESSAGE_COUNT_CHANGE
                && objMessage.message != EXP_MESSAGE_SET_CONTRAST
                && objMessage.message != EXP_MESSAGE_SWITCH_PAGE
                && objMessage.message != CONFIG_MSG_BCAST_CHANGED)
        {
            return false;
        }

        return _EXPManager.OnEXPMessage(objMessage);
    }
}

CEXPAppBlack::CEXPAppBlack()
{
    _EXPManager.Init();
}

CEXPAppBlack::~CEXPAppBlack()
{
    _EXPManager.Uninit();
}

CEXPAppBlack & CEXPAppBlack::GetInstance()
{
    static CEXPAppBlack instance;
    return instance;
}
