#include "settinguivoicemaildetaildelegate.h"

#if IF_FEATURE_BROADSOFT_VOICEMAIL
#include "uicallbackfun.h"
#include "setuidatadefine.h"
#include "settinguimanager.h"
#include "settingui/include/itemnamedefine.h"
#include "settingui/include/pageiddefine.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "messagebox/messageboxbase.h"
#include "messagebox/modmessagebox.h"
#include "settinguilogic/include/modsetuilogic.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "contacts/directory/include/moddirinterface.h"
#include "broadsoft/xsiactionhelper/include/xsicommon.h"
#include "broadsoft/xsiactionhelper/include/cbasehelper.h"
#include "broadsoft/voicemail/include/voicemailcommon.h"
#include "broadsoft/voicemail/include/modvirtualvoicemail.h"
#include "taskaction/common.h"
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"

CSettinguiVoiceMailDetailDelegate::CSettinguiVoiceMailDetailDelegate()
    : m_nVoiceMailIndex(-1)
{
    etl_RegisterMsgHandle(XSI_MESSAGE_BEGIN, XSI_MESSAGE_END,
                          &CSettinguiVoiceMailDetailDelegate::OnMessage);
}

CSettinguiVoiceMailDetailDelegate::~CSettinguiVoiceMailDetailDelegate()
{
    etl_UnregisterMsgHandle(XSI_MESSAGE_BEGIN, XSI_MESSAGE_END,
                            &CSettinguiVoiceMailDetailDelegate::OnMessage);
}

CSettingUIDelegateBase * CSettinguiVoiceMailDetailDelegate::CreateVoiceMailDetailDelegate()
{
    CSettinguiVoiceMailDetailDelegate * pDelegate = new CSettinguiVoiceMailDetailDelegate;

    return (CSettingUIDelegateBasePtr)pDelegate;
}

LRESULT CSettinguiVoiceMailDetailDelegate::OnMessage(msgObject & msg)
{
    if (NULL == g_pSettingUIManager)
    {
        return FALSE;
    }

    CSettinguiVoiceMailDetailDelegate * pDelegate = (CSettinguiVoiceMailDetailDelegate *)
            g_pSettingUIManager->GetPageDelegate(kszPageIDVoiceMailDetail);
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

void CSettinguiVoiceMailDetailDelegate::OnMessageReqFinish(msgObject & msg)
{
    if (XSI_VOICEMESG_DETAIL_GET == msg.wParam)
    {
        MessageBox_RemoveMessageBox(this, MessageBox_OK, XSI_MSGBOX_ID_LOAD_DETAIL);
        EnterPlay();
        return;
    }

    CSettinguiXSIDelegatebase::OnMessageReqFinish(msg);
}

void CSettinguiVoiceMailDetailDelegate::OnMessageReqFail(msgObject & msg)
{
    switch (msg.wParam)
    {
    case XSI_VOICEMESG_DETAIL_GET:
        {
            MessageBox_RemoveMessageBox(this, MessageBox_Close, XSI_MSGBOX_ID_LOAD_DETAIL);
            g_SetUICallBackFun.RefreshUI(false);
            SXsiExtenInfo * pErrInfo = (SXsiExtenInfo *)msg.GetExtraData();
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
            return;
        }
        break;
    default:
        break;
    }

    CSettinguiXSIDelegatebase::OnMessageReqFail(msg);
}

void CSettinguiVoiceMailDetailDelegate::MessageBoxCallBack(CMessageBoxBase * pMessageBox)
{
    if (NULL == pMessageBox)
    {
        return;
    }

    int nID = pMessageBox->GetID();
    int nResult = pMessageBox->GetResult();

    if (XSI_MSGBOX_ID_LOAD_DETAIL == nID)
    {
        if (MessageBox_Cancel == nResult)
        {
            vvMail_CancelAllRequest();
            return;
        }
    }

    CSettinguiXSIDelegatebase::MessageBoxCallBack(pMessageBox);
}

void CSettinguiVoiceMailDetailDelegate::InitExtraData(void * pData)
{
    m_iLineID = vvMail_GetAccountId();
    if (NULL != pData)
    {
        int * pIndex = (int *)pData;
        if (NULL != pIndex)
        {
            m_nVoiceMailIndex = *pIndex;
        }
    }
}

CSettingUIPageDataBase * CSettinguiVoiceMailDetailDelegate::LoadPageData(bool bReLoad /* = false */)
{
    CSettinguiXSIDelegatebase::LoadPageData(bReLoad);

    if (bReLoad && NULL != m_pPageData)
    {
        sVoiceMailData stVoiceMailData;
        vvMail_GetDetailByIndex(stVoiceMailData, m_nVoiceMailIndex);

        yl::string strName = stVoiceMailData.m_bPrivacy ? _LANG(TRID_ANONYMOUS) : stVoiceMailData.m_strName;
        m_pPageData->SetPageTitle(strName);

        SetValue(VOICEMAIL_NAME, strName);

        yl::string strNumber = stVoiceMailData.m_bPrivacy ? _LANG(TRID_ANONYMOUS) :
                               stVoiceMailData.m_strNumber;
        SetValue(VOICEMAIL_NUMBER, strNumber);

        yl::string strTime = stVoiceMailData.m_strTime.substr(0, 10);
        time_t tTime = atoi(strTime.c_str());
        yl::string strTimeValue = SettingUILogic_Time2String(tTime, true);
        SetValue(VOICEMAIL_TIME, strTimeValue);

        yl::string strLine = commonAPI_FormatString("%s%d", _LANG(TRID_LINE).c_str(), m_iLineID + 1);
        SetValue(VOICEMAIL_LINE, strLine);

        yl::string strDuration = "";
        SettingUILogic_Duration2String(stVoiceMailData.m_nDuration, strDuration);
        SetValue(VOICEMAIL_DURATION, strDuration);

        yl::string strStatus = stVoiceMailData.m_bRead ? _LANG(TRID_VOICEMAIL_READ) : _LANG(
                                   TRID_VOICEMAIL_UNREAD);
        SetValue(VOICEMAIL_STATUS, strStatus);

        yl::string strType = (MEDIA_video == stVoiceMailData.m_eMediaType) ? _LANG(
                                 TRID_VOICEMAIL_TYPE_VIDEO) : _LANG(TRID_VOICEMAIL_TYPE_VOICE);
        SetValue(VOICEMAIL_TYPE, strType);
    }

    return m_pPageData;
}

void CSettinguiVoiceMailDetailDelegate::GetPagePrivateSoftkey(CArraySoftKeyBarData & vecSoftKey)
{
    if (SOFTKEY_NUM != vecSoftKey.size())
    {
        return;
    }

    vecSoftKey[3].m_iType = ST_VOICEMAIL_PLAY;
    vecSoftKey[3].m_strSoftkeyTitle = _LANG(TRID_PLAY);
}

bool CSettinguiVoiceMailDetailDelegate::OnSoftkeyAction(int iType, bool bLongPress, int nDataIndex)
{
    if (ST_VOICEMAIL_PLAY == iType)
    {
        bool bResult = vvMail_ReqPlayContentByIndex(m_nVoiceMailIndex);
        ShowDownloadMsgBox(bResult);
        return true;
    }

    return CSettinguiXSIDelegatebase::OnSoftkeyAction(iType, bLongPress, nDataIndex);
}

void CSettinguiVoiceMailDetailDelegate::EnterPlay()
{
    PageOpenData opPageData(kszPageIDVoiceMailPlay);
    opPageData.pExtraData = (void *)&m_nVoiceMailIndex;
    g_SetUICallBackFun.EnterPage(opPageData);
}

void CSettinguiVoiceMailDetailDelegate::ShowDownloadMsgBox(bool bSuccessed)
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

#endif //IF_FEATURE_BROADSOFT_VOICEMAIL
