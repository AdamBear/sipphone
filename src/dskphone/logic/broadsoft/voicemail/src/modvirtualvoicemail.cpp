#include "virtualvoicemailprcocesser.h"
#include "modvirtualvoicemail.h"
#include "configparser/modconfigparser.h"
#include <configiddefine.h>

//进入调用前先设置账号
void vvMail_SetAccountId(int nLine)
{
    g_pVoiceMailProcesser->SetLineID(nLine);
}

//获取当前Voicemail账号
int vvMail_GetAccountId()
{
    return g_pVoiceMailProcesser->GetLineID();
}

bool vvMail_ReqVoicemessageList()
{
    return g_pVoiceMailProcesser->ReqVoicemessageList();
}

bool vvMail_ReqPlayContentByIndex(int index)
{
    return g_pVoiceMailProcesser->ReqPlayContentByIndex(index);
}

bool vvMail_ReqMarkAllRead()
{
    return g_pVoiceMailProcesser->ReqMarkAllRead();
}

bool vvMail_ReqMarkAllUnRead()
{
    return g_pVoiceMailProcesser->ReqMarkAllUnRead();
}

bool vvMail_ReqMarkByIndex(int index, bool bRead)
{
    return g_pVoiceMailProcesser->ReqMarkByIndex(index, bRead);
}

bool vvMail_ReqDeleteMessageByIndex(int index)
{
    return g_pVoiceMailProcesser->ReqDeleteMessageByIndex(index);
}

//退出界面最好调用该接口清除数据，减小内存压力
void vvMail_ClearListData()
{
    return g_pVoiceMailProcesser->ClearListData();
}

bool vvMail_GetVoiceMailListData(VoiceMailListData * pData)
{
    return g_pVoiceMailProcesser->GetVoiceMailListData(pData);
}

bool vvMail_GetDetailByIndex(sVoiceMailData & refData, int index)
{
    return g_pVoiceMailProcesser->GetDetailByIndex(refData, index);
}

bool vvMail_IsEnable()
{
    if (configParser_GetConfigInt(kszBroadsoftActive) == 1
            && configParser_GetConfigInt(kszXSISwitch) == 1
            && configParser_GetConfigInt(kszBroadsoftVisualVoiceMail) == 1)
    {
        return true;
    }

    return false;
}

void vvMail_CancelAllRequest()
{
    g_pVoiceMailProcesser->CancelAllRequest();
}

bool vvMail_Play()
{
    return g_pVoiceMailProcesser->Play();
}

bool vvMail_Pause()
{
    return g_pVoiceMailProcesser->Pause();
}

bool vvMail_Resume()
{
    return g_pVoiceMailProcesser->Resume();
}

bool vvMail_Stop()
{
    return g_pVoiceMailProcesser->Stop();
}

bool vvMail_Seek(unsigned int locate)
{
    return g_pVoiceMailProcesser->Seek(locate);
}

ePlayStatus vvMail_GetPlayStatus()
{
    return g_pVoiceMailProcesser->GetPlayStatus();
}

int vvMail_GetTotalCount()
{
    return g_pVoiceMailProcesser->GetTotalCount();
}
