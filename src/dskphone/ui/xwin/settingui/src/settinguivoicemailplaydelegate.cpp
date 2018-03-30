#include "settinguivoicemailplaydelegate.h"

#if IF_FEATURE_BROADSOFT_VOICEMAIL
#include "keymap.h"
#include "uicallbackfun.h"
#include "setuidatadefine.h"
#include "settinguimanager.h"
#include "settingui/include/itemnamedefine.h"
#include "settingui/include/pageiddefine.h"
#include "baseui/include/modbaseui.h"
#include "settinguitextmessageitemdata.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "messagebox/messageboxbase.h"
#include "messagebox/modmessagebox.h"
#include "settinguilogic/include/modsetuilogic.h"
#include "account/include/modaccount.h"
#include "contacts/directory/include/moddirinterface.h"
#include "broadsoft/xsiactionhelper/include/xsicommon.h"
#include "broadsoft/xsiactionhelper/include/cbasehelper.h"
#include "broadsoft/voicemail/include/voicemailcommon.h"
#include "broadsoft/voicemail/include/modvirtualvoicemail.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "taskaction/common.h"
#include "xwindow/xSoftkeyBar.h"
#include "voice/include/voicecommon.h"
#include "voice/include/modvoice.h"
#include "idlescreen/include/modidlescreen.h"
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"
#include "voicemailtimeritemdata.h"

namespace
{
#define INDEX_STATUS 2
}

CSettinguiVoiceMailPlayDelegate::CSettinguiVoiceMailPlayDelegate()
    : m_nCurrentIndex(-1)
    , m_nInitIndex(-1)
{
    etl_RegisterMsgHandle(XSI_MESSAGE_BEGIN, XSI_MESSAGE_END,
                          &CSettinguiVoiceMailPlayDelegate::OnMessage);
}

CSettinguiVoiceMailPlayDelegate::~CSettinguiVoiceMailPlayDelegate()
{
    vvMail_Stop();
    if (-1 != m_nInitIndex && !m_sVoiceMailData.m_bRead)
    {
        vvMail_ReqMarkByIndex(m_nInitIndex, true);
    }
    UIManager_LockReturnToIdle(false);
    etl_UnregisterMsgHandle(XSI_MESSAGE_BEGIN, XSI_MESSAGE_END,
                            &CSettinguiVoiceMailPlayDelegate::OnMessage);
}

CSettingUIDelegateBase* CSettinguiVoiceMailPlayDelegate::CreateVoiceMailPlayDelegate()
{
    CSettinguiVoiceMailPlayDelegate* pDelegate = new CSettinguiVoiceMailPlayDelegate;

    return (CSettingUIDelegateBasePtr)pDelegate;
}

LRESULT CSettinguiVoiceMailPlayDelegate::OnMessage(msgObject& msg)
{
    if (NULL == g_pSettingUIManager)
    {
        return FALSE;
    }

    CSettinguiVoiceMailPlayDelegate* pDelegate = (CSettinguiVoiceMailPlayDelegate*)
            g_pSettingUIManager->GetPageDelegate(kszPageIDVoiceMailPlay);
    if (NULL == pDelegate || pDelegate !=  g_pSettingUIManager->GetTopDelegate())
    {
        return FALSE;
    }

    if (XSI_MESSAGE_REQ_FINISH == msg.message)
    {
        pDelegate->OnMessageReqFinish(msg);
    }
    else if (XSI_MESSAGE_REQ_FAIL == msg.message)
    {
        pDelegate->OnMessageReqFail(msg);
    }
    else
    {
        return FALSE;
    }

    return TRUE;
}

void CSettinguiVoiceMailPlayDelegate::OnMessageReqFinish(msgObject& msg)
{
    switch (msg.wParam)
    {
    case XSI_VOICEMESG_DETAIL_GET:
        {
            MessageBox_RemoveMessageBox(this, MessageBox_OK, XSI_MSGBOX_ID_LOAD_DETAIL);
            EnterPlay(m_nCurrentIndex);
        }
        break;
    case XSI_VOICEMESG_DELETE:
        {
            m_nInitIndex = -1;
            MessageBox_RemoveMessageBox(this, MessageBox_OK, XSI_MSGBOX_ID_OPERATE);
            if (NULL != g_pSettingUIManager
                    && g_pSettingUIManager->IsPageDelegateCreate(kszPageIDVoiceMailDetail))
            {
                g_SetUICallBackFun.FallBackToPrevousPage();
            }

            g_SetUICallBackFun.FallBackToPrevousPage();
        }
        break;
    default:
        {
            CSettinguiXSIDelegatebase::OnMessageReqFinish(msg);
        }
        break;
    }
}

void CSettinguiVoiceMailPlayDelegate::OnMessageReqFail(msgObject& msg)
{
    switch (msg.wParam)
    {
    case XSI_VOICEMESG_DETAIL_GET:
        {
            MessageBox_RemoveMessageBox(this, MessageBox_OK, XSI_MSGBOX_ID_LOAD_DETAIL);
            g_SetUICallBackFun.RefreshUI(false);
            SXsiExtenInfo* pErrInfo = (SXsiExtenInfo*)msg.GetExtraData();
            if (pErrInfo != NULL)
            {
                switch (pErrInfo->m_nError)
                {
                case NS_TA::TA_FILE_EXCEED_MAX_SIZE:
                    {
                        ShowMessageBox(_LANG(TRID_NOTE), _LANG(TRID_VOICEMAIL_FILE_TOO_LARGE), -1,
                                       SETTING_NOTE_MSGBOX_SHOW_NORMAL_TIME, MESSAGEBOX_NOTE);
                    }
                    break;
                case NS_TA::TA_FILE_OPERATION_NOT_SUPPORTED:
                    {
                        ShowMessageBox(_LANG(TRID_NOTE), _LANG(TRID_VOICEMAIL_NOT_SUPPORT_VIDEO_MESSAGE), -1,
                                       SETTING_NOTE_MSGBOX_SHOW_NORMAL_TIME,
                                       MESSAGEBOX_NOTE);
                    }
                    break;
                default:
                    {
                        ShowMessageBox(_LANG(TRID_NOTE), _LANG(TRID_FAILED_PLEASE_RETRY), XSI_MSGBOX_ID_FAIL, 1000,
                                       MESSAGEBOX_NOTE);
                    }
                    break;
                }
            }

            m_nCurrentIndex = m_nInitIndex;
            return;
        }
    case XSI_VOICEMESG_MARK_READ_PUT:
    case XSI_VOICEMESG_MARK_UNREAD_PUT:
    case XSI_VOICEMESG_MARKALL_READ_PUT:
    case XSI_VOICEMESG_MARKALL_UNREAD_PUT:
    case XSI_VOICEMESG_DELETE:
        {
            CMessageBoxBase* pTopMsg = MessageBox_GetTopMessageBox();
            if (NULL != pTopMsg && XSI_MSGBOX_ID_OPERATE == pTopMsg->GetID())
            {
                MessageBox_RemoveMessageBox(this, MessageBox_OK, XSI_MSGBOX_ID_OPERATE);
                g_SetUICallBackFun.RefreshUI(false);
                ShowMessageBox(_LANG(TRID_NOTE), _LANG(TRID_FAILED_PLEASE_RETRY), XSI_MSGBOX_ID_FAIL, 1000,
                               MESSAGEBOX_NOTE);
            }
            return;
        }
        break;
    }

    CSettinguiXSIDelegatebase::OnMessageReqFail(msg);
}

void CSettinguiVoiceMailPlayDelegate::MessageBoxCallBack(CMessageBoxBase* pMessageBox)
{
    if (NULL == pMessageBox)
    {
        return;
    }

    int nID = pMessageBox->GetID();
    int nResult = pMessageBox->GetResult();

    if (XSI_MSGBOX_ID_DELETE == nID)
    {
        if (MessageBox_OK == nResult)
        {
            if (PLAYSTATUS_Play == vvMail_GetPlayStatus())
            {
                ProcessPause();
            }

            bool bResult = vvMail_ReqDeleteMessageByIndex(m_nInitIndex);
            ShowOperateMsgBox(bResult);
            return;
        }
    }
    else if (XSI_MSGBOX_ID_LOAD_DETAIL == nID)
    {
        if (MessageBox_Cancel == nResult)
        {
            vvMail_CancelAllRequest();
            m_nCurrentIndex = m_nInitIndex;
            return;
        }
    }

    CSettinguiXSIDelegatebase::MessageBoxCallBack(pMessageBox);
}

void CSettinguiVoiceMailPlayDelegate::InitExtraData(void* pData)
{
    UIManager_LockReturnToIdle(true);

    m_iLineID = vvMail_GetAccountId();
    if (NULL != pData)
    {
        int* pIndex = (int*)pData;
        if (NULL != pIndex)
        {
            m_nCurrentIndex = *pIndex;
            m_nInitIndex = m_nCurrentIndex;
        }
    }
    vvMail_Play();
}

CSettingUIPageDataBase* CSettinguiVoiceMailPlayDelegate::LoadPageData(bool bReLoad /* = false*/)
{
    CSettinguiXSIDelegatebase::LoadPageData(bReLoad);

    if (bReLoad && NULL != m_pPageData)
    {
        UpdateTitle();

        vvMail_GetDetailByIndex(m_sVoiceMailData, m_nCurrentIndex);

        yl::string strFrom = "";
        if (!m_sVoiceMailData.m_bPrivacy)
        {
            strFrom = m_sVoiceMailData.m_strName;
            if (m_sVoiceMailData.m_strNumber != m_sVoiceMailData.m_strName)
            {
                strFrom += "-";
                strFrom += m_sVoiceMailData.m_strNumber;
            }
        }
        else
        {
            strFrom = _LANG(TRID_ANONYMOUS);
        }
        SetValue(VOICEMAIL_FROM, strFrom);

        yl::string strTime = m_sVoiceMailData.m_strTime.substr(0, 10);
        time_t tTime = atoi(strTime.c_str());
        yl::string strTimeValue = SettingUILogic_Time2String(tTime, true);
        SetValue(VOICEMAIL_TIME, strTimeValue);

        yl::string strDuration;
        SettingUILogic_Duration2String(m_sVoiceMailData.m_nDuration, strDuration);
        int nTotalTime = GetTotalTime();
        LoadVoiceMailPlayBoxItem(m_pPageData, strDuration, nTotalTime, false, true,
                                 VMPS_PLAY, VOICEMAIL_TIMEPIECE);
    }

    return m_pPageData;
}

void CSettinguiVoiceMailPlayDelegate::GetPagePrivateSoftkey(CArraySoftKeyBarData& vecSoftKey)
{
    vecSoftKey.clear();

    ePlayStatus playStatus = vvMail_GetPlayStatus();

    if (!m_sVoiceMailData.m_bPrivacy)
    {
        AddSoftkey(vecSoftKey, ST_VOICEMAIL_CALLBACK, _LANG(TRID_VOICEMAIL_OPTION_CALL_BACK));
    }

    AddSoftkey(vecSoftKey, ST_DELETE, _LANG(TRID_DELETE));

    if (playStatus == PLAYSTATUS_Stop)
    {
        AddSoftkey(vecSoftKey, ST_VOICEMAIL_PLAY, _LANG(TRID_PLAY));
    }
    else if (playStatus == PLAYSTATUS_Pause)
    {
        AddSoftkey(vecSoftKey, ST_VOICEMAIL_RESUME, _LANG(TRID_RESUME));
    }
    else
    {
        AddSoftkey(vecSoftKey, ST_VOICEMAIL_PAUSE, _LANG(TRID_PAUSE));
    }

    if (m_nCurrentIndex != 0)
    {
        AddSoftkey(vecSoftKey, ST_VOICEMAIL_PREVIOUS, _LANG(TRID_PREVIOUS));
    }

    if (m_nCurrentIndex < vvMail_GetTotalCount() - 1)
    {
        AddSoftkey(vecSoftKey, ST_VOICEMAIL_NEXT, _LANG(TRID_NEXT));
    }

    AddSoftkey(vecSoftKey, ST_BACK, _LANG(TRID_BACK));
}

bool CSettinguiVoiceMailPlayDelegate::OnAction(int eActionType, int iDataIndex)
{
    if (SET_ACTION_TYPE_FOCUS == eActionType)
    {
        resetSoftkeyPageIndex();
        g_SetUICallBackFun.RefreshSoftkey();
        return true;
    }

    return CSettinguiXSIDelegatebase::OnAction(eActionType, iDataIndex);
}

bool CSettinguiVoiceMailPlayDelegate::OnSoftkeyAction(int iType, bool bLongPress, int nDataIndex)
{
    switch (iType)
    {
    case ST_DELETE:
        {
            ShowMessageBox(_LANG(TRID_NOTE), _LANG(TRID_VOICEMAIL_DELETE_VOICE_MAIL), XSI_MSGBOX_ID_DELETE, -1,
                           MESSAGEBOX_OkCancel);
        }
        break;
    case ST_VOICEMAIL_PLAY:
        {
            ProcessPlay();
        }
        break;
    case ST_VOICEMAIL_PAUSE:
        {
            ProcessPause();
        }
        break;
    case ST_VOICEMAIL_RESUME:
        {
            ProcessResume();
        }
        break;
    case ST_VOICEMAIL_STOP:
        {
            ProcessStop();
        }
        break;
    case ST_VOICEMAIL_PREVIOUS:
    case ST_VOICEMAIL_NEXT:
        {
            if (ST_VOICEMAIL_PREVIOUS == iType)
            {
                m_nCurrentIndex--;
            }
            else
            {
                m_nCurrentIndex++;
            }

            if (PLAYSTATUS_Stop != vvMail_GetPlayStatus())
            {
                ProcessStop();
            }

            bool bResult = vvMail_ReqPlayContentByIndex(m_nCurrentIndex);
            ShowDownloadMsgBox(bResult);
        }
        break;
    case ST_VOICEMAIL_CALLBACK:
        {
            ProcessCallBack();
        }
        break;
    default:
        {
            return CSettinguiXSIDelegatebase::OnSoftkeyAction(iType, bLongPress, nDataIndex);
        }
    }

    return true;
}

bool CSettinguiVoiceMailPlayDelegate::ProcessKeyEvent(int nKeyCode, bool bPress)
{
    if (!bPress)
    {
        return false;
    }

    if (PHONE_KEY_HANDFREE == nKeyCode || PHONE_KEY_HANDSET_ON_HOOK == nKeyCode)
    {
        return true;
    }

    if (PHONE_KEY_1 == nKeyCode || PHONE_KEY_2 == nKeyCode || PHONE_KEY_3 == nKeyCode
            || PHONE_KEY_0 == nKeyCode || PHONE_KEY_POUND == nKeyCode || PHONE_KEY_STAR == nKeyCode)
    {
        int nOldFocus = GetFocusIndex();
        int nNewFocus = INDEX_STATUS;
        if (PHONE_KEY_1 == nKeyCode || PHONE_KEY_2 == nKeyCode || PHONE_KEY_3 == nKeyCode)
        {
            nNewFocus = nKeyCode - PHONE_KEY_1;
        }

        resetSoftkeyPageIndex();
        if (nOldFocus == nNewFocus)
        {
            g_SetUICallBackFun.RefreshSoftkey();
        }
        else
        {
            SetFocusIndex(nNewFocus);
            g_SetUICallBackFun.RefreshUI(false);
        }

        return true;
    }

    if ((PHONE_KEY_VOLUME_DECREASE == nKeyCode || PHONE_KEY_VOLUME_INCREASE == nKeyCode)
            && voice_IsVolumeCanChange())
    {
        int nVolume = voice_GetVolume();
        nVolume = (PHONE_KEY_VOLUME_INCREASE == nKeyCode) ? nVolume + 1 : nVolume - 1;
        voice_SetVolume(nVolume, VT_AUTO, TRUE);

        if (0 == voice_GetVolume())
        {
            // 音量为0，就显示静音状态
            idleScreen_SetStatus(PS_STATE_RING_MUTE);
        }
        else
        {
            // 音量改变，就撤销静音状态
            idleScreen_DropStatus(PS_STATE_RING_MUTE);
        }
    }

    return CSettinguiXSIDelegatebase::ProcessKeyEvent(nKeyCode, bPress);
}

bool CSettinguiVoiceMailPlayDelegate::OnKeyEvent(int nKeyCode, bool bPress, int nDataIndex)
{
    if (!bPress)
    {
        return false;
    }

    if (PHONE_KEY_OK == nKeyCode)
    {
        ePlayStatus playStatus = vvMail_GetPlayStatus();

        if (playStatus == PLAYSTATUS_Stop)
        {
            return ProcessPlay();
        }
        else if (playStatus == PLAYSTATUS_Pause)
        {
            return ProcessResume();
        }
        else if (playStatus == PLAYSTATUS_Play)
        {
            return ProcessPause();
        }
    }

    return CSettinguiXSIDelegatebase::OnKeyEvent(nKeyCode, bPress, nDataIndex);
}

void CSettinguiVoiceMailPlayDelegate::OnTimeOut()
{
    if (!m_sVoiceMailData.m_bRead)
    {
        vvMail_ReqMarkByIndex(m_nCurrentIndex, true);
    }
    ProcessStop();
    g_SetUICallBackFun.RefreshUI(false);
}

bool CSettinguiVoiceMailPlayDelegate::ProcessCallBack()
{
    yl::string strNumber = m_sVoiceMailData.m_strNumber;
    yl::string strName = m_sVoiceMailData.m_strName;
    if (!m_sVoiceMailData.m_bRead)
    {
        vvMail_ReqMarkByIndex(m_nInitIndex, true);
    }

    return talklogic_CallOut(strNumber.c_str(), strName.c_str(), m_iLineID);
}

bool CSettinguiVoiceMailPlayDelegate::ProcessPlay()
{
    vvMail_Play();
    UpdateTitle();
    SetVoiceMailPlayByItemID(VOICEMAIL_TIMEPIECE, VMPS_PLAY);
    return TRUE;
}

bool CSettinguiVoiceMailPlayDelegate::ProcessPause()
{
    vvMail_Pause();
    UpdateTitle();
    SetVoiceMailPlayByItemID(VOICEMAIL_TIMEPIECE, VMPS_PAUSE);
    return TRUE;
}

bool CSettinguiVoiceMailPlayDelegate::ProcessResume()
{
    vvMail_Resume();
    UpdateTitle();
    SetVoiceMailPlayByItemID(VOICEMAIL_TIMEPIECE, VMPS_RESUME);
    return TRUE;
}

bool CSettinguiVoiceMailPlayDelegate::ProcessStop()
{
    vvMail_Stop();
    UpdateTitle();
    SetVoiceMailPlayByItemID(VOICEMAIL_TIMEPIECE, VMPS_STOP);
    return TRUE;
}

int CSettinguiVoiceMailPlayDelegate::GetTotalTime()
{
    if (0 == (m_sVoiceMailData.m_nDuration % 1000))

    {
        return (m_sVoiceMailData.m_nDuration / 1000);

    }
    return (m_sVoiceMailData.m_nDuration / 1000 + 1);
}

bool CSettinguiVoiceMailPlayDelegate::UpdateTitle()
{
    if (NULL == m_pPageData)
    {
        return false;
    }

    yl::string strTitle = "";
    ePlayStatus eStatus = vvMail_GetPlayStatus();
    if (PLAYSTATUS_Pause == eStatus)
    {
        strTitle = _LANG(TRID_VOICEMAIL_PAUSE);
    }
    else if (PLAYSTATUS_Stop == eStatus)
    {
        strTitle = _LANG(TRID_VOICEMAIL_FINISHED);
    }
    else
    {
        strTitle = _LANG(TRID_VOICEMAIL_PLAYING);
    }

    m_pPageData->SetPageTitle(strTitle);

    return true;
}

void CSettinguiVoiceMailPlayDelegate::ShowDownloadMsgBox(bool bSuccessed)
{
    if (bSuccessed)
    {
        ShowMessageBox(_LANG(TRID_NOTE), _LANG(TRID_LOADING_PLEASE_WAIT), XSI_MSGBOX_ID_LOAD_DETAIL, -1,
                       MESSAGEBOX_Cancel);
    }
    else
    {
        ShowMessageBox(_LANG(TRID_NOTE), _LANG(TRID_FAILED_PLEASE_RETRY), XSI_MSGBOX_ID_OPERATE, 1000,
                       MESSAGEBOX_NOTE);
    }
}

void CSettinguiVoiceMailPlayDelegate::ShowOperateMsgBox(bool bSuccessed)
{
    if (bSuccessed)
    {
        ShowMessageBox(_LANG(TRID_NOTE), _LANG(TRID_UPDATING_PLEASE_WAIT), XSI_MSGBOX_ID_OPERATE, -1,
                       MESSAGEBOX_NOTE);
    }
    else
    {
        ShowMessageBox(_LANG(TRID_NOTE), _LANG(TRID_FAILED_PLEASE_RETRY), XSI_MSGBOX_ID_OPERATE, 1000,
                       MESSAGEBOX_NOTE);
    }
}

void CSettinguiVoiceMailPlayDelegate::EnterPlay(int nID)
{
    g_SetUICallBackFun.FallBackToPrevousPage();

    PageOpenData opPageData(kszPageIDVoiceMailPlay);
    opPageData.pExtraData = (void*)&nID;
    g_SetUICallBackFun.EnterPage(opPageData);
}

void CSettinguiVoiceMailPlayDelegate::AddSoftkey(CArraySoftKeyBarData& vecSoftKey, int nType,
        const yl::string& strDisp)
{
    CSoftKeyBarData objSoftkeyData;

    objSoftkeyData.m_iType = nType;
    objSoftkeyData.m_strSoftkeyTitle = strDisp;
    vecSoftKey.push_back(objSoftkeyData);
}

#endif //IF_FEATURE_BROADSOFT_VOICEMAIL
