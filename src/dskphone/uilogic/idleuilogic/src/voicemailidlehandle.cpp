#include "voicemailidlehandle.h"
#include "uilogiccommon/include/uilogiclanghelper.h"
#include "translateiddefine.h"

#include "idle/idle_data_define.h"
#include "account/include/accountmessagedefine.h"
#include <configiddefine.h>
#include "lamp/powerled/include/powerledmessagedefine.h"

#include "idlescreen/include/modidlescreen.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "commonapi/stringhelperapi.h"
#include "configparser/modconfigparser.h"
#include "account/include/modaccount.h"
#include "lamp/ledlamp/include/modledlamp.h"
#include "voice/include/modvoice.h"
#include "lamp/backlight/include/modbacklight.h"
#include "idleuilogic/include/modidleul.h"
#include "settinguilogic/include/modsetuilogic.h"

///////////////////////////////////////////////////////
CVoiceMailHandle::CVoiceMailHandle(int nType /*= PS_STATE_VOICEMAIL*/)
    : CBaseIdleHandle(nType)
{
    etl_RegisterMsgHandle(ACCOUNT_VOICE_MAIL_COUNT_CHANGED, ACCOUNT_VOICE_MAIL_COUNT_CHANGED,
                          &CVoiceMailHandle::OnVoiceMailMessage);
    etl_RegisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED,
                          &CVoiceMailHandle::OnVoiceMailMessage);
    UpdateNotify(kszDisplayVoiceMailPopup, PS_STATE_VOICEMAIL);
}

CVoiceMailHandle::~CVoiceMailHandle()
{
    etl_UnregisterMsgHandle(ACCOUNT_VOICE_MAIL_COUNT_CHANGED, ACCOUNT_VOICE_MAIL_COUNT_CHANGED,
                            &CVoiceMailHandle::OnVoiceMailMessage);
    etl_UnregisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED,
                            &CVoiceMailHandle::OnVoiceMailMessage);
}

bool CVoiceMailHandle::GetPopupBoxInfo(PopupBoxData & popData)
{
    //Voice Mail 弹窗提醒
    m_VoiceMailData.ReSet();
    m_VoiceMailData.m_nCountInfo = 0;
    m_VoiceMailData.m_nUnReadCount = acc_GetUnReadVoiceMailNum();
    if (m_VoiceMailData.m_nUnReadCount <= 0)  //当前没有未读的voicemail，则不弹出弹出框
    {
        return false;
    }
    yl::string strCount = commonAPI_FormatString("%d", m_VoiceMailData.m_nUnReadCount);
#if IF_SUPPORT_MIDDLE_POPBOX
    popData.m_iType = IDLE_MSGBOX_TYPE_POPUP_MIDDLE;
#else
    popData.m_iType = IDLE_MSGBOX_TYPE_POPUPNORMAL;//messagebox
#endif
    popData.m_strNote = strCount + " " + yl::string(_UILOGIC_LANG_TRAN(TRID_NEW_VOICE_MAIL));
    popData.m_vecSoftkey[0] = ST_EXIT;
    popData.m_vecSoftkey[1] = ST_EMPTY;
    popData.m_vecSoftkey[2] = ST_VIEW ;
    popData.m_vecSoftkey[3] = ST_CONNECT;
    return true;
}

bool CVoiceMailHandle::HandleKeyProcess(SoftKey_TYPE eSoftkey, PHONE_KEY_CODE eKeyCode)
{
    unsigned int nNotifyType = GetNeedClearNotifyType();
    bool bRet = false;
    if (ST_CONNECT == eSoftkey
            || PHONE_KEY_OK == eKeyCode || ST_OK == eSoftkey)
    {
        SettingUILogic_ConnectVoiceMail(acc_GetLastVMailAccountId());
        bRet = true;
    }
    else if (ST_VIEW == eSoftkey)
    {
        idleScreen_ClearNotifyTypeFlag(m_iType, nNotifyType);
#if IF_FEATURE_METASWITCH_VOICEMAIL
        if (!IdleUL_EnterMTSWVoiceMail())
        {
            IdleUL_EnterPageByType(IEPT_VIEW_VOICE_MAIL);
        }
#else
        IdleUL_EnterPageByType(IEPT_VIEW_VOICE_MAIL);
#endif
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

LRESULT CVoiceMailHandle::OnVoiceMailMessage(msgObject & msg)
{
    if (CONFIG_MSG_BCAST_CHANGED == msg.message)
    {
        if (ST_VOICEMAIL_POPUP == msg.wParam)
        {
            UpdateNotify(kszDisplayVoiceMailPopup, PS_STATE_VOICEMAIL);
            return TRUE;
        }
    }
    else if (ACCOUNT_VOICE_MAIL_COUNT_CHANGED == msg.message)
    {
        if (0 != acc_GetUnReadVoiceMailNum())
        {
            //新消息，直接增加状态
            if (1 == msg.wParam)
            {
                idleScreen_SetStatus(PS_STATE_VOICEMAIL, commonAPI_FormatString("%d", acc_GetUnReadVoiceMailNum()));
                ledlamp_PostEvent(LAMP_EVENTTYPE_SMS, true);
                if (0 != configParser_GetConfigInt(kszEnableVoiceMailTone))
                {
                    voice_PlayTone(TONE_VOICEMAIL, 300);
                }
                // 点亮背光灯
                backlight_RegisterEvent(BACK_LIGHT_DEFAULT, true);
            }
            else
            {
                idleScreen_ChangeHint(PS_STATE_VOICEMAIL, commonAPI_FormatString("%d",
                                      acc_GetUnReadVoiceMailNum()));
            }
        }
        else
        {
            //没有未读消息，删除状态
            idleScreen_DropStatus(PS_STATE_VOICEMAIL);
            ledlamp_PostEvent(LAMP_EVENTTYPE_SMS, false);
        }
        etl_NotifyApp(false, POWER_LED_VOICE_MAIL, 0, 0);
        return TRUE;
    }
    return FALSE;
}

bool CVoiceMailHandle::GetIconInfo(IdleIconInfo & IconData)
{
    CBaseIdleHandle::GetIconInfo(IconData);
    IconData.m_strHint = commonAPI_FormatString("%d", acc_GetUnReadVoiceMailNum());
#if IF_SUPPORT_NOTIFY_ICON_FLASH
    IconData.m_bFlash = true;
#endif
    return true;
}

#if IF_SUPPORT_TEXT_NOTIFY
bool CVoiceMailHandle::GetTextNotifyData(ST_TextNotifyData& stNotifyData)
{
    //Voice Mail 弹窗提醒
    m_VoiceMailData.ReSet();
    m_VoiceMailData.m_nCountInfo = 0;
    m_VoiceMailData.m_nUnReadCount = acc_GetUnReadVoiceMailNum();

    if (m_VoiceMailData.m_nUnReadCount > 0)  //当前有可读的voicemail，才显示提示
    {
        yl::string strCount = commonAPI_FormatString("%d", m_VoiceMailData.m_nUnReadCount);
        stNotifyData.m_strNote = strCount + " " + yl::string(_UILOGIC_LANG_TRAN(TRID_NEW_VOICE_MAIL));
        return true;
    }

    return false;
}

bool CVoiceMailHandle::GetSoftkey(VEC_SOFTKET_TYPE& m_vecSoftkey)
{
    m_vecSoftkey.clear();
    m_vecSoftkey.push_back(ST_EXIT);
    m_vecSoftkey.push_back(ST_EMPTY);
    m_vecSoftkey.push_back(ST_VIEW);
    m_vecSoftkey.push_back(ST_CONNECT);
    return true;
}
#endif
