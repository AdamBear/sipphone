#ifndef __MOD_MTSW_VOICEMAIL_H_
#define __MOD_MTSW_VOICEMAIL_H_

#if IF_FEATURE_METASWITCH_VOICEMAIL
#include "metaswitch/mtswvoicemail/include/mtswvoicemaildefine.h"

void MTSWVM_Init();

bool MTSWVM_IsMessagesListEnable();

void MTSWVM_SetDownLoading(bool bDownLoading);

void MTSWVM_SetFromIndex(int nIndex);

int MTSWVM_GetFromIndex();

void MTSWVM_SetItemIndex(int nIndex);
int MTSWVM_GetItemIndex();
bool MTSWVM_MarkReadStatus(ReadStatus eStatus);
void MTSWVM_SetPlayLoading(bool bLoading);
bool MTSWVM_GetMessageDetail(VoiceMsgItemDetailData * pData, int nIndex);
void MTSWVM_SetPlayStatus(PlayStatus eStatus);
PlayStatus MTSWVM_GetPlayStatus();
bool MTSWVM_DownloadVoiceMail();
bool MTSWVM_Play();
bool MTSWVM_Pause();
bool MTSWVM_Resume();
bool MTSWVM_Stop(bool bReset = true);
bool MTSWVM_Delete();
bool MTSWVM_DeleteAll();
int MTSWVM_GetTotalCount();
bool MTSWVM_IsDownLoading();
int MTSWVM_GetTotalUnRead();
int MTSWVM_GetDuration();
yl::string MTSWVM_FormatDuration(int nDuration);

ReadStatus MTSWVM_GetMessageStatus(int nIndex = -1);

bool MTSWVM_GetMessageList(VoiceMsgSimpListData * pData, int & nTotal);

bool MTSWVM_LoadVoiceMailList();

bool MTSWVM_DownloadNextVoiceMail();

void MTSWVM_CancelDownload();

void MTSWVM_Clean();
#endif

#endif
