#include "misscallidlehandle.h"
#include "uilogiccommon/include/uilogiclanghelper.h"
#include "translateiddefine.h"

#include "account/include/accountmessagedefine.h"
#include "idle/idle_data_define.h"
#include <configiddefine.h>

#include "idlescreen/include/modidlescreen.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "commonapi/stringhelperapi.h"
#include "feature/include/modfeature.h"
#include "configparser/modconfigparser.h"
#include "idleuilogic/include/modidleul.h"
#include "contacts/directory/include/moddirinterface.h"
#include "contacts/directory/include/directorymessagedefine.h"

int CMissCallHandle::m_nMisscallCounts = 0;
///////////////////////////////////////////////////////
CMissCallHandle::CMissCallHandle(int nType /*= PS_STATE_MISSCALL*/)
    : CBaseIdleHandle(nType)
{
    etl_RegisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED,
                          &CMissCallHandle::OnMissCallMessage);
    etl_RegisterMsgHandle(LOCAL_CALLLOG_MESSAGE_MISSED, LOCAL_CALLLOG_MESSAGE_MISSED,
                          &CMissCallHandle::OnMissCallMessage);
    int ret = UpdateNotify(kszDisplayMissedCallPopup, PS_STATE_MISSCALL);
}

CMissCallHandle::~CMissCallHandle()
{
    etl_UnregisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED,
                            &CMissCallHandle::OnMissCallMessage);
    etl_UnregisterMsgHandle(LOCAL_CALLLOG_MESSAGE_MISSED, LOCAL_CALLLOG_MESSAGE_MISSED,
                            &CMissCallHandle::OnMissCallMessage);
}

bool CMissCallHandle::GetPopupBoxInfo(PopupBoxData & popData)
{
    int nCountInfo = Dir_GetContactCount(kszMissNotifId, LOCAL_LOG);
    if (nCountInfo > 0)
    {
        yl::string strCount = commonAPI_FormatString("%d", nCountInfo);
#if IF_SUPPORT_MIDDLE_POPBOX
        popData.m_iType = IDLE_MSGBOX_TYPE_POPUP_MIDDLE;
#else
        popData.m_iType = IDLE_MSGBOX_TYPE_POPUPNORMAL;//messagebox
#endif
        popData.m_strNote = strCount + yl::string(" ") + yl::string(_UILOGIC_LANG_TRAN(
                                TRID_NEW_MISSED_CALLS));
        popData.m_vecSoftkey[0] = ST_EXIT;
        popData.m_vecSoftkey[1] = ST_EMPTY;
        popData.m_vecSoftkey[2] = ST_EMPTY;
        popData.m_vecSoftkey[3] = ST_VIEW;
        return true;
    }
    return false;
}

bool CMissCallHandle::HandleKeyProcess(SoftKey_TYPE eSoftkey, PHONE_KEY_CODE eKeyCode)
{
    unsigned int nNotifyType = GetNeedClearNotifyType();
    bool bRet = false;
    if (ST_VIEW == eSoftkey || ST_OK == eSoftkey
            || PHONE_KEY_OK == eKeyCode)
    {
        if (BsftLog_IsEnable()
                && 1 == configParser_GetConfigInt(kszBWCallLogandDir))
        {
            IdleUL_EnterPageByType(IEPT_BROADSOFT_CALLLOG_MISSED);
        }
        else
        {
            IdleUL_EnterPageByType(IEPT_LOCAL_CALLLOG_MISSED);
        }
        idleScreen_DropStatus(PS_STATE_MISSCALL);
        bRet = true;
    }
    else if (ST_CANCEL == eSoftkey || ST_EXIT == eSoftkey
             || PHONE_KEY_CANCEL == eKeyCode)
    {
        idleScreen_ClearNotifyTypeFlag(m_iType, nNotifyType);
        IdleUL_UpdateStatus();
        bRet = true;
    }
    return bRet;
}

LRESULT CMissCallHandle::OnMissCallMessage(msgObject & msg)
{
    if (LOCAL_CALLLOG_MESSAGE_MISSED == msg.message)
    {
        IDLE_INFO("Miss Call Count Change, [%d] Miss Call(s).", msg.lParam);
        if (msg.lParam > m_nMisscallCounts)
        {
            idleScreen_SetStatus(PS_STATE_MISSCALL, commonAPI_FormatString("%d", msg.lParam));
        }
        else if (msg.lParam < m_nMisscallCounts
                 && msg.lParam > 0) //个数变少，但是个数大于0,不弹框
        {
            //个数变少，不能弹框，清除弹出标示
            //idleScreen_RegisterStatus();
            idleScreen_ClearRegStatusTypeFlag(PS_STATE_MISSCALL, SNT_POPUP);
            idleScreen_SetStatus(PS_STATE_MISSCALL, commonAPI_FormatString("%d", msg.lParam));
        }
        else if (0 == msg.lParam)
        {
            idleScreen_DropStatus(PS_STATE_MISSCALL);
        }
        m_nMisscallCounts = msg.lParam;
        return  TRUE;
    }

    if (CONFIG_MSG_BCAST_CHANGED == msg.message)
    {
        if (ST_VOICEMAIL_POPUP == msg.wParam)
        {
            UpdateNotify(kszDisplayMissedCallPopup, PS_STATE_MISSCALL);
            return TRUE;
        }
    }
    return FALSE;
}

bool CMissCallHandle::GetIconInfo(IdleIconInfo & IconData)
{
    CBaseIdleHandle::GetIconInfo(IconData);
#if IF_SUPPORT_NOTIFY_ICON_FLASH
    IconData.m_bFlash = true;
#endif
    IconData.m_strHint = commonAPI_FormatString("%d", Dir_GetContactCount(kszMissNotifId, LOCAL_LOG));
    return true;
}

#if IF_SUPPORT_TEXT_NOTIFY
bool CMissCallHandle::GetTextNotifyData(ST_TextNotifyData& stNotifyData)
{
    int nCountInfo = Dir_GetContactCount(kszMissNotifId, LOCAL_LOG);
    if (nCountInfo > 0)
    {
        yl::string strCount = commonAPI_FormatString("%d", nCountInfo);
        stNotifyData.m_strNote = strCount + yl::string(" ") + yl::string(_UILOGIC_LANG_TRAN(
                                     TRID_NEW_MISSED_CALLS));
        return true;
    }
    return false;
}

bool CMissCallHandle::GetSoftkey(VEC_SOFTKET_TYPE& m_vecSoftkey)
{
    m_vecSoftkey.clear();
    m_vecSoftkey.push_back(ST_EXIT);
    m_vecSoftkey.push_back(ST_EMPTY);
    m_vecSoftkey.push_back(ST_EMPTY);
    m_vecSoftkey.push_back(ST_VIEW);
    return true;
}
#endif
