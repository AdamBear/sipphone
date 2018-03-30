#include "textmessageidlehandle.h"
#include "uilogiccommon/include/uilogiclanghelper.h"
#include "translateiddefine.h"

#include "idle/idle_data_define.h"
#include <configiddefine.h>
#include "service_config.h"

#include "idlescreen/include/modidlescreen.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "idleuilogic/include/modidleul.h"
#include "settinguilogic/include/modsetuilogic.h"

CTextMessageHandle::CTextMessageHandle(int nType /*= PS_STATE_TEXTMESSAGE*/)
    : CBaseIdleHandle(nType)
{
    etl_RegisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED,
                          &CTextMessageHandle::OnTextMessageMessage);
    UpdateNotify(kszDisplayTextMessagePopup, PS_STATE_TEXTMESSAGE);
}

CTextMessageHandle::~CTextMessageHandle()
{
    etl_UnregisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED,
                            &CTextMessageHandle::OnTextMessageMessage);
}

bool CTextMessageHandle::GetPopupBoxInfo(PopupBoxData & popData)
{
    // softkey
    IdleStatusItem StatusItems = idleScreen_GetTopStatus(SNT_POPUP);
    yl::string  strTarget = StatusItems.m_strHint;
    if (!strTarget.empty())
    {
#if IF_SUPPORT_MIDDLE_POPBOX
        popData.m_iType = IDLE_MSGBOX_TYPE_POPUP_MIDDLE;
#else
        popData.m_iType = IDLE_MSGBOX_TYPE_POPUPNORMAL;//messagebox
#endif
        popData.m_strNote = strTarget + " " + yl::string(_UILOGIC_LANG_TRAN(TRID_UNREAD_TEXT_MESSAGE));
        popData.m_vecSoftkey[0] = ST_EXIT;
        popData.m_vecSoftkey[1] = ST_EMPTY;
        popData.m_vecSoftkey[2] = ST_EMPTY;
        popData.m_vecSoftkey[3] = ST_VIEW;
        return true;
    }
    return false;
}

bool CTextMessageHandle::HandleKeyProcess(SoftKey_TYPE eSoftkey, PHONE_KEY_CODE eKeyCode)
{
    bool bRet = false;
    unsigned int nNotifyType = GetNeedClearNotifyType();
    idleScreen_ClearNotifyTypeFlag(m_iType, nNotifyType);

    if (ST_OK == eSoftkey || ST_VIEW == eSoftkey
            || PHONE_KEY_OK == eKeyCode)
    {
        int nUnReadMsg = SettingUILogic_GetTotalUnReadMessage();
        if (1 == nUnReadMsg)
        {
            IdleUL_EnterPageByType(IEPT_TEXTMESSAGE_VIEW_INMSG);
        }
        else
        {
            IdleUL_EnterPageByType(IEPT_TEXTMESSAGE_INBOX);
        }
        bRet = true;
    }
    else if (ST_CANCEL == eSoftkey || ST_EXIT == eSoftkey
             || PHONE_KEY_CANCEL == eKeyCode)
    {
        IdleUL_UpdateStatus();
        bRet = true;
    }

    return bRet;
}

LRESULT CTextMessageHandle::OnTextMessageMessage(msgObject & msg)
{
    if (CONFIG_MSG_BCAST_CHANGED == msg.message)
    {
        if (ST_VOICEMAIL_POPUP == msg.wParam)
        {
            UpdateNotify(kszDisplayTextMessagePopup, PS_STATE_TEXTMESSAGE);
            return TRUE;
        }
    }
    return FALSE;
}

bool CTextMessageHandle::GetIconInfo(IdleIconInfo & IconData)
{
    CBaseIdleHandle::GetIconInfo(IconData);
    IconData.m_strHint = commonAPI_FormatString("%d", SettingUILogic_GetTotalUnReadMessage());
#if IF_SUPPORT_NOTIFY_ICON_FLASH
    IconData.m_bFlash = true;
#endif
    return true;
}

#if IF_SUPPORT_TEXT_NOTIFY
bool CTextMessageHandle::GetTextNotifyData(ST_TextNotifyData& stNotifyData)
{
    IdleStatusItem StatusItems = idleScreen_GetTopStatus(SNT_TEXT_NOTIFY);
    yl::string  strUnReadNum = StatusItems.m_strHint;
    if (!strUnReadNum.empty())
    {
        stNotifyData.m_strNote = strUnReadNum + " " + yl::string(_UILOGIC_LANG_TRAN(TRID_UNREAD_TEXT_MESSAGE));
        return true;
    }
    return false;
}

bool CTextMessageHandle::GetSoftkey(VEC_SOFTKET_TYPE& m_vecSoftkey)
{
    m_vecSoftkey.clear();
    m_vecSoftkey.push_back(ST_EXIT);
    m_vecSoftkey.push_back(ST_EMPTY);
    m_vecSoftkey.push_back(ST_EMPTY);
    m_vecSoftkey.push_back(ST_VIEW);
    return true;
}
#endif
