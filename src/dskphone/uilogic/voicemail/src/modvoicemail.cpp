#include "voicemail/include/modvoicemail.h"
#include "voicemailmanager.h"
#include "lamp/ledlamp/include/modledlamp.h"
#include "idlescreen/include/modidlescreen.h"
#include "idle/idle_data_define.h"
#include "lamp/powerled/include/powerledmessagedefine.h"
#include "lamp/powerled/include/modpowerledlogic.h"

bool vmail_Init()
{
    return _VoiceMailManager.Init();
}

int vmail_GetLastVMailAccountId()
{
    return _VoiceMailManager.GetLastVMailAccountID();
}

int vmail_GetReadedVoiceMailNum(int iAccountID /* = -1 */, bool bNotify/* = false*/)
{
    return _VoiceMailManager.GetVoiceMailNumber(iAccountID, true, bNotify);
}

int vmail_GetUnReadVoiceMailNum(int iAccountID /* = -1 */, bool bNotify/* = false*/)
{
    return _VoiceMailManager.GetVoiceMailNumber(iAccountID, false, bNotify);
}

void vmail_ClickInfoByAccount(int iAccount)
{
    _VoiceMailManager.OnConnect(iAccount);
}

bool vmail_GetConfigList(VoiceMailConfigListData & refData)
{
    return _VoiceMailManager.GetVoiceMailConfigList(refData);
}

void vmail_SetConfigList(VoiceMailConfigListData & refData)
{
    _VoiceMailManager.SetVoiceMailConfig(refData);
}

bool vmail_GetInfoList(VoiceMailInfoListData & refData)
{
    return _VoiceMailManager.GetVoiceMailInfoList(refData);
}

#if 0
bool vmail_GetConfig(int iAccountID, VoiceMailConfigData & refData)
{
    CVoiceMailAccount * pAccount = _VoiceMailManager.GetVoiceMailAccountByID(iAccountID);
    if (NULL == pAccount)
    {
        return false;
    }

    return pAccount->GetVoiceMailConfig(refData);
}

void vmail_SetConfig(int iAccountID, VoiceMailConfigData & refData)
{
    CVoiceMailAccount * pAccount = _VoiceMailManager.GetVoiceMailAccountByID(iAccountID);
    if (NULL != pAccount)
    {
        pAccount->SetVoiceMailConfig(refData);
    }
}

bool vmail_GetInfo(int iAccountID, VoiceMailInfoData & refData)
{
    CVoiceMailAccount * pAccount = _VoiceMailManager.GetVoiceMailAccountByID(iAccountID);
    if (NULL == pAccount)
    {
        return false;
    }
    return pAccount->GetVoiceMailInfo(refData);
}
#endif // 0

bool vmail_OnMsgKeyPress()
{
    return _VoiceMailManager.OnMsgKeyPress();
}

void vmail_RemoveNotify(int iAccountID/* = -1*/)
{
    _VoiceMailManager.SetVoiveMailNumber(iAccountID);

    //删除掉全部时，需要清除掉通知与灯
    if (vmail_GetUnReadVoiceMailNum(-1, true) <= 0)
    {
        //删除状态
        idleScreen_DropStatus(PS_STATE_VOICEMAIL);
        ledlamp_PostEvent(LAMP_EVENTTYPE_SMS, false);
    }
}
