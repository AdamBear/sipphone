#include "mtswvoicemailplaydelegate.h"

#if IF_FEATURE_METASWITCH_VOICEMAIL

#include "settingui/include/pageiddefine.h"
#include "settingui/include/itemnamedefine.h"
#include "settingui/include/modsettingui.h"
#include "settinguimanager.h"
#include "setuidatadefine.h"
#include "uicallbackfun.h"
#include "voicemailtimeritemdata.h"

#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "messagebox/modmessagebox.h"

#include "account/include/modaccount.h"
#include "broadsoft/callcenter/include/modcallcenter.h"
#include "metaswitch/mtswcontrol/include/modmtsw.h"
#include "metaswitch/mtswvoicemail/include/modmtswvoicemail.h"
#include "metaswitch/mtswcontrol/include/mtsw_messagedefine.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "baselibs/include/taskaction/common.h"

#include "settinguilogic/include/common.h"
#include "settinguilogic/src/settinguicomboboxitemdata.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "settinguilogic/include/modsetuilogic.h"
#include "dsklog/log.h"


namespace
{
#define MSG_ID_LOAD_VOICEMAIL             1230
#define DELAY_EXIT_TIME                   50
}

CMtswVoiceMailPlayDelegate::CMtswVoiceMailPlayDelegate()
{
    UIManager_LockReturnToIdle(true);
    etl_RegisterMsgHandle(MTSW_MESSAGE_BEGIN, MTSW_MESSAGE_END,
                          &CMtswVoiceMailPlayDelegate::OnMTSWVoiceMailPlayMsg);
}

CMtswVoiceMailPlayDelegate::~CMtswVoiceMailPlayDelegate()
{
    etl_UnregisterMsgHandle(MTSW_MESSAGE_BEGIN, MTSW_MESSAGE_END,
                            &CMtswVoiceMailPlayDelegate::OnMTSWVoiceMailPlayMsg);
    MTSWVM_Stop();
    MTSWVM_SetPlayLoading(false);
    UIManager_LockReturnToIdle(false);

    if (m_timerExit.IsTimerRuning())
    {
        m_timerExit.KillTimer();
    }
}

CSettingUIDelegateBase* CMtswVoiceMailPlayDelegate::CreateMtswVoiceMailPlayDelegate()
{
    CMtswVoiceMailPlayDelegate* pDelegate = new CMtswVoiceMailPlayDelegate;
    return (CSettingUIDelegateBasePtr)pDelegate;
}

LRESULT CMtswVoiceMailPlayDelegate::OnMTSWVoiceMailPlayMsg(msgObject& msg)
{
    CSettingUIDelegateBase* pDelegate = g_pSettingUIManager->GetTopDelegate();
    if (NULL == pDelegate || kszPageMTSWVoiceMailPlay != pDelegate->GetPageID())
    {
        return FALSE;
    }

    CMtswVoiceMailPlayDelegate* pVoiceMailPlayDelegate = static_cast<CMtswVoiceMailPlayDelegate*>(pDelegate);
    if (NULL == pVoiceMailPlayDelegate)
    {
        return FALSE;
    }

    switch (msg.message)
    {
    case MTSW_MESSAGE_DOWNLOADED:
        {
            MTSWVM_SetPlayLoading(false);
            //关闭弹框
            MessageBox_RemoveAllMessageBox(MessageBox_Close);
            pVoiceMailPlayDelegate->StartVoiceMail();
        }
        break;
    case MTSW_MESSAGE_DOWNLOADFAILED:
        {
            //关闭弹框
            MessageBox_RemoveAllMessageBox();
            if (NS_TA::TA_FILE_EXCEED_MAX_SIZE == msg.wParam)
            {
                g_SetUICallBackFun.PopCommonMessageBox(kszPageMTSWVoiceMailDetail, _LANG(TRID_NOTE), _LANG(TRID_DOWNLOADFILE_OUT_OF_MEMORY),
                                                       SETTING_MESSAGEBOX_NOTE, "", "", SETTING_NOTE_MSGBOX_SHOW_SHORT_TIME);
            }
            else
            {
                g_SetUICallBackFun.PopCommonMessageBox(kszPageMTSWVoiceMailDetail, _LANG(TRID_NOTE), _LANG(TRID_LOAD_FAILED),
                                                       SETTING_MESSAGEBOX_NOTE, "", "", SETTING_NOTE_MSGBOX_SHOW_SHORT_TIME);
            }

            pVoiceMailPlayDelegate->ProcessStop();
        }
        break;
    default:
        break;
    }

    return TRUE;
}

void CMtswVoiceMailPlayDelegate::GetPagePrivateSoftkey(CArraySoftKeyBarData& vecSoftKey)
{
    if (vecSoftKey.size() < MIN_SETTING_SOFTKEY_NUM)
    {
        return;
    }

    if (STATUS_PLAY == MTSWVM_GetPlayStatus())
    {
        vecSoftKey[0].m_iType = ST_VOICEMAIL_PAUSE;
        vecSoftKey[0].m_strSoftkeyTitle = _LANG(TRID_PAUSE);
    }
    else
    {
        vecSoftKey[0].m_iType = ST_VOICEMAIL_PLAY;
        vecSoftKey[0].m_strSoftkeyTitle = _LANG(TRID_PLAY);
    }

    vecSoftKey[1].m_iType = ST_SEND;
    vecSoftKey[1].m_strSoftkeyTitle = _LANG(TRID_SEND);
    if (MTSWVM_GetTotalUnRead() > 0)
    {
        vecSoftKey[2].m_iType = ST_VOICEMAIL_NEXT;
        vecSoftKey[2].m_strSoftkeyTitle = _LANG(TRID_NEXT);
    }
    else
    {
        vecSoftKey[2].m_iType = ST_EMPTY;
        vecSoftKey[2].m_strSoftkeyTitle = "";
    }

    vecSoftKey[3].m_iType = ST_VOICEMAIL_STOP;
    vecSoftKey[3].m_strSoftkeyTitle = _LANG(TRID_STOP);
}

bool CMtswVoiceMailPlayDelegate::OnSoftkeyAction(int iType, bool bLongPress, int nDataIndex)
{
    switch (iType)
    {
    case ST_SEND:
        {
            ProcessCallBack();
            return true;
        }
        break;
    case ST_VOICEMAIL_PLAY:
        {
            ProcessPlay();
            return true;
        }
        break;
    case ST_VOICEMAIL_PAUSE:
        {
            ProcessPause();
            return true;
        }
        break;
    case ST_VOICEMAIL_NEXT:
        {
            ProcessNext();
            return true;
        }
        break;
    case ST_VOICEMAIL_STOP:
        {
            ProcessStop();
            return true;
        }
        break;
    default:
        break;
    }

    return CSettingUIDelegateBase::OnSoftkeyAction(iType, bLongPress, nDataIndex);
}

void CMtswVoiceMailPlayDelegate::InitExtraData(void* pData)
{
    LoadVoiceMailPlayInfo();
}

bool CMtswVoiceMailPlayDelegate::ProcessKeyEvent(int nKeyCode, bool bPress)
{
    if (bPress && voice_IsChannelKey(nKeyCode) &&
            (STATUS_PLAY == MTSWVM_GetPlayStatus()
             || STATUS_MTSW_PAUSE == MTSWVM_GetPlayStatus()))
    {
        switch (nKeyCode)
        {
        case PHONE_KEY_HANDSET_ON_HOOK:
            {
                voice_SwitchChannel(CHANNEL_RING);
                return true;
            }
            break;
        case PHONE_KEY_HEADSET_CTRL:
            {
                if (!voice_GetHeadsetStatus())
                {
                    voice_SwitchChannel(CHANNEL_RING);
                }
                return true;
            }
            break;
        default:
            break;
        }
    }
    return CSettinguiDataDelegateBase::ProcessKeyEvent(nKeyCode, bPress);
}

bool CMtswVoiceMailPlayDelegate::OnKeyEvent(int nKeyCode, bool bPress, int nDataIndex)
{
    if (bPress)
    {
        switch (nKeyCode)
        {
        case PHONE_KEY_LEFT:
        case PHONE_KEY_RIGHT:
            {
                SwitchMessage(PHONE_KEY_RIGHT == nKeyCode);
            }
            break;
        case PHONE_KEY_UP:
        case PHONE_KEY_DOWN:
            {
                return true;
            }
            break;
        case PHONE_KEY_VOLUME_DECREASE:
        case PHONE_KEY_VOLUME_INCREASE:
            {
                int iVolume = voice_GetVolume();
                iVolume = (PHONE_KEY_VOLUME_INCREASE == nKeyCode) ? iVolume + 1 : iVolume - 1;
                voice_SetVolume(iVolume, VT_AUTO, TRUE);
                return false;
            }
            break;
        default:
            break;
        }
    }
    return CSettinguiDataDelegateBase::OnKeyEvent(nKeyCode, bPress, nDataIndex);
}

bool CMtswVoiceMailPlayDelegate::IsShowVolumeBar(int& iVolumeType, int& iMin, int& iMax, yl::string& strPix)
{
    iMin = 0;
    iMax = 15;
    return true;
}

void CMtswVoiceMailPlayDelegate::OnTimeOut()
{
    m_timerExit.SingleShot(DELAY_EXIT_TIME, TIMER_METHOD(this, CMtswVoiceMailPlayDelegate::OnDelayExit));
}

void CMtswVoiceMailPlayDelegate::StartVoiceMail()
{
    // 设置数据
    g_SetUICallBackFun.RefreshUI(false);
    MTSWVM_Play();
    MTSWVM_MarkReadStatus(STATUS_READ);
}

void CMtswVoiceMailPlayDelegate::ProcessPlay()
{
    MTSWVM_Play();
    SetVoiceMailPlayByItemID(MESW_VOICE_MAIL_PLAY_STATUS, VMPS_RESUME);
    g_SetUICallBackFun.RefreshSoftkey();
}

void CMtswVoiceMailPlayDelegate::ProcessPause()
{
    SETTINGUI_INFO("CMtswVoiceMailPlayDelegate::ProcessPause()");
    MTSWVM_Pause();
    SetVoiceMailPlayByItemID(MESW_VOICE_MAIL_PLAY_STATUS, VMPS_PAUSE);
    g_SetUICallBackFun.RefreshSoftkey();
}

void CMtswVoiceMailPlayDelegate::ProcessStop()
{
    MTSWVM_Stop();
    SetVoiceMailPlayByItemID(MESW_VOICE_MAIL_PLAY_STATUS, VMPS_STOP);
    g_SetUICallBackFun.FallBackToPrevousPage();
}

void CMtswVoiceMailPlayDelegate::ProcessNext()
{
    ProcessPause();
    MTSWVM_SetPlayLoading(true);
    MTSWVM_DownloadNextVoiceMail();
    g_SetUICallBackFun.PopCommonMessageBox(m_strPageID, _LANG(TRID_NOTE), _LANG(TRID_LOADING_PLEASE_WAIT),
                                           SETTING_MESSAGEBOX_Cancel, "", "", -1,
                                           MSG_ID_LOAD_VOICEMAIL);
}

void CMtswVoiceMailPlayDelegate::OnDelayExit(xThreadTimer* timer, LPVOID pExtraData)
{
    ProcessStop();
}

void CMtswVoiceMailPlayDelegate::ProcessCallBack()
{
    VoiceMsgItemDetailData VMIDD;
    int iIndex = MTSWVM_GetItemIndex();
    if (MTSWVM_GetMessageDetail(&VMIDD, iIndex))
    {
        talklogic_CallOut(VMIDD.m_strNumber, VMIDD.m_strName);
    }
}

bool CMtswVoiceMailPlayDelegate::SwitchMessage(bool bNext)
{
    g_SetUICallBackFun.PopCommonMessageBox(m_strPageID, _LANG(TRID_NOTE), _LANG(TRID_LOADING_PLEASE_WAIT),
                                           SETTING_MESSAGEBOX_Cancel, "", "", -1,
                                           MSG_ID_LOAD_VOICEMAIL);

    int nTotalCount = MTSWVM_GetTotalCount();
    int nCurIndex = MTSWVM_GetItemIndex();
    int nSwitchIndex;

    if (!SettingUI_GetSwitchIndex(nTotalCount, nCurIndex, bNext, nSwitchIndex))
    {
        return false;
    }

    //停止播放
    ProcessPause();
    //开始下载
    MTSWVM_SetItemIndex(nSwitchIndex);
    MTSWVM_SetPlayLoading(true);
    MTSWVM_DownloadVoiceMail();
    return true;
}

void CMtswVoiceMailPlayDelegate::LoadVoiceMailPlayInfo()
{
    if (NULL == m_pPageData)
    {
        return;
    }

    VoiceMsgItemDetailData VMIDD;
    int iIndex = MTSWVM_GetItemIndex();
    bool bRet = MTSWVM_GetMessageDetail(&VMIDD, iIndex);

    if (!bRet)
    {
        return;
    }

    AddBaseItemData(m_pPageData, _LANG(TRID_FROM),
                    VMIDD.m_strName, MESW_VOICE_MAIL_FROM, false);

    AddBaseItemData(m_pPageData, _LANG(TRID_NUMBER),
                    VMIDD.m_strNumber, MESW_VOICE_MAIL_NUMBER, false);

    AddBaseItemData(m_pPageData, _LANG(TRID_TIME),
                    SettingUILogic_Time2NewFormatString(VMIDD.m_tTime), MESW_VOICE_MAIL_TIME,
                    false);

    int nMaxTicks = MTSWVM_GetDuration();
    LoadVoiceMailPlayBoxItem(m_pPageData, VMIDD.m_strDuration, nMaxTicks, true, false,
                             VMPS_PLAY, MESW_VOICE_MAIL_PLAY_STATUS);

    StartVoiceMail();
}

bool CMtswVoiceMailPlayDelegate::ProcessMsgCallBack(int nResult, int nID,
        void* pExtraData /*= NULL*/)
{
    switch (nID)
    {
    case MSG_ID_LOAD_VOICEMAIL:
        {
            if (MessageBox_Cancel == nResult)
            {
                g_SetUICallBackFun.FallBackToPrevousPage();
            }
            else if (MessageBox_Close == nResult)
            {
                MTSWVM_SetPlayLoading(false);
            }
        }
        break;
    default:
        break;
    }

    return CSettinguiDataDelegateBase::ProcessMsgCallBack(nResult, nID, pExtraData);
}

CSettingUIPageDataBase* CMtswVoiceMailPlayDelegate::LoadPageData(bool bReLoad /*= false*/)
{
    CSettingUIPageDataBase* pPageData = CSettinguiDataDelegateBase::LoadPageData(bReLoad);

    if (NULL == pPageData)
    {
        return NULL;
    }

    if (bReLoad)
    {
        LoadVoiceMailPlayInfo();
    }

    return pPageData;
}

#endif

