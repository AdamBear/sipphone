#if IF_FEATURE_METASWITCH_VOICEMAIL
#include "metaswitch/mtswvoicemail/include/modmtswvoicemail.h"
#include "metaswitch/mtswvoicemail/include/mtsw_voicemailmanager.h"
#include "configiddefine.h"
#include "configparser/modconfigparser.h"

void MTSWVM_Init()
{
    if (configParser_GetConfigInt(kszMTSWVMListEnable) == 1
            && configParser_GetConfigInt(kszMetaSwitchActive) == 1)
    {
        _MTSWVMManager.Init();
    }
}

void MTSWVM_SetDownLoading(bool bDownLoading)
{
    _MTSWVMManager.SetDownLoading(bDownLoading);
}

bool MTSWVM_IsMessagesListEnable()
{
    return _MTSWVMManager.IsMessagesListEnable();
}

void MTSWVM_SetFromIndex(int nIndex)
{
    _MTSWVMManager.SetFromIndex(nIndex);
}

int MTSWVM_GetFromIndex()
{
    return _MTSWVMManager.GetFromIndex();
}

void MTSWVM_SetItemIndex(int nIndex)
{
    _MTSWVMManager.SetItemIndex(nIndex);
}

int MTSWVM_GetItemIndex()
{
    return _MTSWVMManager.GetItemIndex();
}

void MTSWVM_SetPlayLoading(bool bLoading)
{
    _MTSWVMManager.SetPlayLoading(bLoading);
}

bool MTSWVM_MarkReadStatus(ReadStatus eStatus)
{
    return _MTSWVMManager.MarkReadStatus(eStatus);
}

bool MTSWVM_GetMessageDetail(VoiceMsgItemDetailData * pData, int nIndex)
{
    return _MTSWVMManager.GetMessageDetail(pData, nIndex);
}

void MTSWVM_SetPlayStatus(PlayStatus eStatus)
{
    _MTSWVMManager.SetPlayStatus(eStatus);
}

PlayStatus MTSWVM_GetPlayStatus()
{
    return _MTSWVMManager.GetPlayStatus();
}

bool MTSWVM_DownloadVoiceMail()
{
    return _MTSWVMManager.DownloadVoiceMail();
}

bool MTSWVM_Play()
{
    return _MTSWVMManager.Play();
}

bool MTSWVM_Pause()
{
    return _MTSWVMManager.Pause();
}

bool MTSWVM_Resume()
{
    return _MTSWVMManager.Resume();
}

bool MTSWVM_Stop(bool bReset/* = true*/)
{
    return _MTSWVMManager.Stop(bReset);
}

bool MTSWVM_Delete()
{
    return _MTSWVMManager.Delete();
}

bool MTSWVM_DeleteAll()
{
    return _MTSWVMManager.DeleteAll();
}

int MTSWVM_GetTotalCount()
{
    return _MTSWVMManager.GetTotalCount();
}

bool MTSWVM_IsDownLoading()
{
    return _MTSWVMManager.IsDownLoading();
}

int MTSWVM_GetTotalUnRead()
{
    return _MTSWVMManager.GetTotalUnRead();
}

int MTSWVM_GetDuration()
{
    return _MTSWVMManager.GetDuration();
}

yl::string MTSWVM_FormatDuration(int nDuration)
{
    return _MTSWVMManager.FormatDuration(nDuration);
}

ReadStatus MTSWVM_GetMessageStatus(int nIndex/* = -1*/)
{
    return _MTSWVMManager.GetMessageStatus(nIndex);
}

bool MTSWVM_GetMessageList(VoiceMsgSimpListData * pData, int & nTotal)
{
    return _MTSWVMManager.GetMessageList(pData, nTotal);
}

bool MTSWVM_LoadVoiceMailList()
{
    return _MTSWVMManager.LoadVoiceMailList();
}

bool MTSWVM_DownloadNextVoiceMail()
{
    return _MTSWVMManager.DownloadNextVoiceMail();
}

void MTSWVM_CancelDownload()
{
    _MTSWVMManager.CancelDownload();
}

void MTSWVM_Clean()
{
    _MTSWVMManager.MyFinal();
}

#endif
