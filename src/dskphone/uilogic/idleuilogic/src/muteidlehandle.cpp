#include "muteidlehandle.h"

#include "idle/idle_data_define.h"
#include "idlescreen/include/modidlescreen.h"
#include "voice/include/modvoice.h"
#include <configiddefine.h>
#include "idle/idle_data_define.h"
#include "configparser/modconfigparser.h"
#include "etlmsghandler/modetlmsghandler.h"

CMuteHandle::CMuteHandle(int nType /*= PS_STATE_RING_MUTE*/)
    : CBaseIdleHandle(nType)
{
    if (0 == voice_GetVolume(VT_RING))
    {
        idleScreen_SetStatus(PS_STATE_RING_MUTE, "");
    }
    etl_RegisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED,
                          &CMuteHandle::OnReceiveMsg);
}

CMuteHandle::~CMuteHandle()
{
    etl_UnregisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED,
                            &CMuteHandle::OnReceiveMsg);
}

LRESULT CMuteHandle::OnReceiveMsg(msgObject & objMessage)
{
    bool bRet = FALSE;
    switch (objMessage.message)
    {
    case CONFIG_MSG_BCAST_CHANGED:
        {
            if (ST_FORCEVOICE_SWITCH == objMessage.wParam)
            {
                if (0 == voice_GetVolume(VT_RING))
                {
                    idleScreen_SetStatus(PS_STATE_RING_MUTE);
                }
                else
                {
                    idleScreen_DropStatus(PS_STATE_RING_MUTE);
                }
                bRet = TRUE;
            }
        }
        break;
    default:
        break;
    }
    return bRet;
}
