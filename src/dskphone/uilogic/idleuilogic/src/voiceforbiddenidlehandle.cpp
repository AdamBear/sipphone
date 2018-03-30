#include "voiceforbiddenidlehandle.h"
#include "uilogiccommon/include/uilogiclanghelper.h"
#include "translateiddefine.h"
#include <configiddefine.h>
#include "idle/idle_data_define.h"

#include "configparser/modconfigparser.h"
#include "idlescreen/include/modidlescreen.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "account/include/modaccount.h"
#include "dsklog/log.h"
#include "voice/include/voicecommon.h"

#ifdef IF_FEATURE_GETBALANCE

namespace
{
#define BALANCE_ID_LOADING_NOTE         MSGBOX_ID_HOTDESK_WAITING + 100
}

CVoiceForbiddenHandle::CVoiceForbiddenHandle(int nType /*= PS_STATE_BALANCE_LOADING*/)
    : CBaseIdleHandle(nType)
{
    etl_RegisterMsgHandle(VOICE_CHANNEL_FORBDDEN, VOICE_CHANNEL_FORBDDEN,
                          &CVoiceForbiddenHandle::OnVoiceMessage);
}

CVoiceForbiddenHandle::~CVoiceForbiddenHandle()
{
    etl_UnregisterMsgHandle(VOICE_CHANNEL_FORBDDEN, VOICE_CHANNEL_FORBDDEN,
                            &CVoiceForbiddenHandle::OnVoiceMessage);
}

bool CVoiceForbiddenHandle::GetPopupBoxInfo(PopupBoxData & popData)
{
    popData.m_nTimes = POPUP_BOX_SHOWING_SHORT_TIME;
    popData.m_iType = IDLE_MSGBOX_TYPE_NOTE; //提示
    popData.m_strNote = m_strHint;
    idleScreen_ClearNotifyTypeFlag(m_iType, SNT_POPUP);

    return true;
}

bool CVoiceForbiddenHandle::HandleKeyProcess(SoftKey_TYPE eSoftkey, PHONE_KEY_CODE eKeyCode)
{
    if ((ST_CANCEL == eSoftkey || PHONE_KEY_CANCEL == eKeyCode) &&
            PS_STATE_VOICE_FORBIDDEN == m_iType)
    {
        idleScreen_DropStatus(m_iType);
    }
    return true;
}

LRESULT CVoiceForbiddenHandle::OnVoiceMessage(msgObject & msg)
{
    BOOL bRet = TRUE;
    switch (msg.message)
    {
    case GET_BALANCE_MESSAGE_LOADING:
    {
        yl::string strTips;
        switch (msg.wParam)
        {
        case CHANNEL_HEADSET:
        {
            strTips = TRID_HEADSET_FORBIDDEN;
        }
        break;
        case CHANNEL_HANDSET:
        {
            strTips = TRID_HANDSET_FORBIDDEN;
        }
        break;
        case CHANNEL_HANDFREE:
        {
            strTips = TRID_HANDFREE_FORBIDDEN;
        }
        break;
        default:
            break;
        }
        // 正在下载
        idleScreen_SetStatus(PS_STATE_VOICE_FORBIDDEN, _UILOGIC_LANG_TRAN(strTips.c_str()));
    }
    break;
    default:
    {
        bRet = FALSE;
    }
    break;
    }
    return bRet;
}

#endif
