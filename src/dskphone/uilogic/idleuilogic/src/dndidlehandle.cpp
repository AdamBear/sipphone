#include "dndidlehandle.h"
#include "idle/idle_data_define.h"
#include "idlescreen/include/modidlescreen.h"
#include "idleuilogic/include/modidleul.h"
#include "talk/fwd_dnd/include/modfwddnd.h"
#include "talk/fwd_dnd/include/fwddndmsgdefine.h"
#include "account/include/modaccount.h"
#include "etlmsghandler/modetlmsghandler.h"

//////////////////CDNDHandle//////////////////
CDNDHandle::CDNDHandle(int nType /*= PS_STATE_DND*/)
    : CBaseIdleHandle(nType)
{
    etl_RegisterMsgHandle(DND_MESSAGE_STATUS_CHANGED, DND_MESSAGE_STATUS_CHANGED,
                          &CDNDHandle::OnDNDMessage);
    etl_RegisterMsgHandle(ACCOUNT_DEFAULT_CHANGED, ACCOUNT_DEFAULT_CHANGED,
                          &CDNDHandle::OnDNDMessage);
}

CDNDHandle::~CDNDHandle()
{
    etl_UnregisterMsgHandle(DND_MESSAGE_STATUS_CHANGED, DND_MESSAGE_STATUS_CHANGED,
                            &CDNDHandle::OnDNDMessage);
    etl_UnregisterMsgHandle(ACCOUNT_DEFAULT_CHANGED, ACCOUNT_DEFAULT_CHANGED,
                            &CDNDHandle::OnDNDMessage);
}

LRESULT CDNDHandle::OnDNDMessage(msgObject& msg)
{
    switch (msg.message)
    {
    case DND_MESSAGE_STATUS_CHANGED:
        {
            if (FWDDND_ACCOUNT_GLOBAL == msg.wParam
                    || acc_GetDefaultAccount() == msg.wParam)
            {
                bool bEnable = *((bool*)msg.GetExtraData());
                if (bEnable)
                {
                    idleScreen_SetStatus(PS_STATE_DND);
                }
                else
                {
                    idleScreen_DropStatus(PS_STATE_DND);
                }
            }
        }
        break;
    case ACCOUNT_DEFAULT_CHANGED:
        {
            // 默认账号变化，需要重新设置FWD&DND状态
            if (FWDDND_MODE_ACCOUNT == dnd_GetMode())
            {
                if (dnd_IsEnable(acc_GetDefaultAccount()))
                {
                    idleScreen_SetStatus(PS_STATE_DND);
                }
                else
                {
                    idleScreen_DropStatus(PS_STATE_DND);
                }
            }
        }
        break;
    default:
        break;
    }

    return TRUE;
}

