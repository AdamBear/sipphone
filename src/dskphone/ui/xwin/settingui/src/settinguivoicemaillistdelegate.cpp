#include "settinguivoicemaillistdelegate.h"

#if IF_FEATURE_BROADSOFT_VOICEMAIL
#include "uicallbackfun.h"
#include "setuidatadefine.h"
#include "settinguimanager.h"
#include "settingui/include/itemnamedefine.h"
#include "settingui/include/pageiddefine.h"
#include "messagebox/selectmessagebox.h"
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
#include "baseui/include/t2xpicpath.h"
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"

namespace
{
typedef YLList<sVoiceMailData> LIST_VOICE_MAIL_DATA;
}

CSettinguiVoiceMailListDelegate::CSettinguiVoiceMailListDelegate()
{
    etl_RegisterMsgHandle(XSI_MESSAGE_BEGIN, XSI_MESSAGE_END,
                          &CSettinguiVoiceMailListDelegate::OnMessage);
}

CSettinguiVoiceMailListDelegate::~CSettinguiVoiceMailListDelegate()
{
    vvMail_ClearListData();
    etl_UnregisterMsgHandle(XSI_MESSAGE_BEGIN, XSI_MESSAGE_END,
                            &CSettinguiVoiceMailListDelegate::OnMessage);
}

CSettingUIDelegateBase * CSettinguiVoiceMailListDelegate::CreateVoiceMailListDelegate()
{
    CSettinguiVoiceMailListDelegate * pDelegate = new CSettinguiVoiceMailListDelegate;

    return (CSettingUIDelegateBasePtr)pDelegate;
}

LRESULT CSettinguiVoiceMailListDelegate::OnMessage(msgObject & msg)
{
    if (NULL == g_pSettingUIManager)
    {
        return FALSE;
    }

    CSettinguiVoiceMailListDelegate * pDelegate = (CSettinguiVoiceMailListDelegate *)
            g_pSettingUIManager->GetPageDelegate(kszPageIDVoiceMailList);
    if (NULL == pDelegate || pDelegate !=  g_pSettingUIManager->GetTopDelegate())
    {
        if (NULL == pDelegate)
        {
            return FALSE;
        }

        if (XSI_VOICEMESG_MARK_READ_PUT == msg.wParam
                || XSI_VOICEMESG_MARK_UNREAD_PUT == msg.wParam
                || XSI_VOICEMESG_MARKALL_READ_PUT == msg.wParam
                || XSI_VOICEMESG_MARKALL_UNREAD_PUT == msg.wParam)
        {
            pDelegate->SetRefreshDataWhenTopDelegate(true);
        }
        if (XSI_VOICEMESG_DELETE == msg.wParam && XSI_MESSAGE_REQ_FINISH == msg.message)
        {
            int iCurrentIndex = pDelegate->GetFocusIndex();
            if (iCurrentIndex != 0)
            {
                pDelegate->SetFocusIndex(iCurrentIndex - 1);
            }
        }

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

void CSettinguiVoiceMailListDelegate::OnMessageReqFinish(msgObject & msg)
{
    switch (msg.wParam)
    {
    case XSI_VOICEMESG_LIST_GET:
        {
            MessageBox_RemoveMessageBox(this, MessageBox_OK, XSI_MSGBOX_ID_LOADING);
            UpdateUIData();
        }
        break;
    case XSI_VOICEMESG_DETAIL_GET:
        {
            MessageBox_RemoveMessageBox(this, MessageBox_OK, XSI_MSGBOX_ID_LOAD_DETAIL);
            EnterPlay(GetFocusItemID());
        }
        break;
    case XSI_VOICEMESG_MARK_READ_PUT:
    case XSI_VOICEMESG_MARK_UNREAD_PUT:
    case XSI_VOICEMESG_MARKALL_READ_PUT:
    case XSI_VOICEMESG_MARKALL_UNREAD_PUT:
        {
            MessageBox_RemoveMessageBox(this, MessageBox_OK, XSI_MSGBOX_ID_OPERATE);
            UpdateUIData();
        }
        break;
    case XSI_VOICEMESG_DELETE:
        {
            MessageBox_RemoveMessageBox(this, MessageBox_OK, XSI_MSGBOX_ID_OPERATE);
            int iCurrentIndex = GetFocusIndex();
            if (iCurrentIndex != 0)
            {
                SetFocusIndex(iCurrentIndex - 1);
            }
            UpdateUIData();
        }
        break;
    default:
        break;
    }
}

void CSettinguiVoiceMailListDelegate::OnMessageReqFail(msgObject & msg)
{
    switch (msg.wParam)
    {
    case XSI_VOICEMESG_DETAIL_GET:
        {
            MessageBox_RemoveMessageBox(this, MessageBox_OK, XSI_MSGBOX_ID_LOAD_DETAIL);
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
    case XSI_VOICEMESG_MARK_READ_PUT:
    case XSI_VOICEMESG_MARK_UNREAD_PUT:
    case XSI_VOICEMESG_MARKALL_READ_PUT:
    case XSI_VOICEMESG_MARKALL_UNREAD_PUT:
    case XSI_VOICEMESG_DELETE:
        {
            CMessageBoxBase * pTopMsg = MessageBox_GetTopMessageBox();
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
    default:
        break;
    }

    CSettinguiXSIDelegatebase::OnMessageReqFail(msg);
}

void CSettinguiVoiceMailListDelegate::MessageBoxCallBack(CMessageBoxBase * pMessageBox)
{
    if (NULL == pMessageBox)
    {
        return;
    }

    if (MESSAGEBOX_SELECTBOX == pMessageBox->GetType()
            && MessageBox_OK == pMessageBox->GetResult())
    {
        CSelectMessageBox * pOptionMsgBox = (CSelectMessageBox *)pMessageBox;
        int nAction = pOptionMsgBox->GetFcousAction();
        OnOptionBoxCallBack(nAction);
        return;
    }

    int nID = pMessageBox->GetID();
    int nResult = pMessageBox->GetResult();

    if (XSI_MSGBOX_ID_DELETE == nID)
    {
        if (MessageBox_OK == nResult)
        {
            bool bResult = vvMail_ReqDeleteMessageByIndex(GetFocusItemID());
            ShowOperateMsgBox(bResult);
            return;
        }
    }
    else if (XSI_MSGBOX_ID_LOADING == nID)
    {
        if (MessageBox_Cancel == nResult)
        {
            vvMail_CancelAllRequest();

            g_SetUICallBackFun.FallBackToPrevousPage();
            return;
        }
    }
    else if (XSI_MSGBOX_ID_LOAD_DETAIL == nID)
    {
        if (MessageBox_Cancel == nResult)
        {
            vvMail_CancelAllRequest();
            return;
        }
    }

    CSettinguiXSIDelegatebase::MessageBoxCallBack(pMessageBox);
}

void CSettinguiVoiceMailListDelegate::InitExtraData(void * pData)
{
    CSettinguiXSIDelegatebase::InitExtraData(pData);

    if (0 == vvMail_GetTotalCount())
    {
        vvMail_SetAccountId(m_iLineID);
        m_tmStarRequest.SingleShot(10, TIMER_METHOD(this, CSettinguiVoiceMailListDelegate::OnStarRequest));
    }
}

CSettingUIPageDataBase * CSettinguiVoiceMailListDelegate::LoadPageData(bool bReLoad /* = false */)
{
    int nLastFocusIndex = -1;
    if (bReLoad && NULL != m_pPageData)
    {
        nLastFocusIndex = m_pPageData->GetFocusIndex();
    }

    CSettinguiXSIDelegatebase::LoadPageData(bReLoad);

    if (bReLoad && NULL != m_pPageData)
    {
        if (-1 != m_iLineID)
        {
            yl::string strTitle = acc_GetAccountShowText(m_iLineID);
            m_pPageData->SetPageTitle(strTitle);
        }

        LoadVoiceListItemData(m_pPageData);

        if (-1 != nLastFocusIndex && nLastFocusIndex < m_pPageData->GetTotalItemNumber())
        {
            m_pPageData->SetFocusIndex(nLastFocusIndex);
        }
    }

    return m_pPageData;
}

void CSettinguiVoiceMailListDelegate::GetPagePrivateSoftkey(CArraySoftKeyBarData & vecSoftKey)
{
    if (SOFTKEY_NUM != vecSoftKey.size())
    {
        return;
    }

    if (vvMail_GetTotalCount() <= 0)
    {
        vecSoftKey[1].m_iType = ST_EMPTY;
        vecSoftKey[1].m_strSoftkeyTitle = "";
        vecSoftKey[2].m_iType = ST_EMPTY;
        vecSoftKey[2].m_strSoftkeyTitle = "";
        vecSoftKey[3].m_iType = ST_EMPTY;
        vecSoftKey[3].m_strSoftkeyTitle = "";
    }
    else
    {
        vecSoftKey[1].m_iType = ST_DELETE;
        vecSoftKey[1].m_strSoftkeyTitle = _LANG(TRID_DELETE);
        vecSoftKey[2].m_iType = ST_OPTION;
        vecSoftKey[2].m_strSoftkeyTitle = _LANG(TRID_OPTION);
        vecSoftKey[3].m_iType = ST_VOICEMAIL_PLAY;
        vecSoftKey[3].m_strSoftkeyTitle = _LANG(TRID_PLAY);
    }
}

bool CSettinguiVoiceMailListDelegate::OnSoftkeyAction(int iType, bool bLongPress, int nDataIndex)
{
    if (ST_OPTION == iType)
    {
        OnOptionClick();
    }
    else if (ST_DELETE == iType)
    {
        ShowMessageBox(_LANG(TRID_NOTE), _LANG(TRID_VOICEMAIL_DELETE_VOICE_MAIL), XSI_MSGBOX_ID_DELETE, -1,
                       MESSAGEBOX_OkCancel);
    }
    else if (ST_VOICEMAIL_PLAY == iType)
    {
        bool bResult = vvMail_ReqPlayContentByIndex(GetFocusItemID());
        ShowDownloadMsgBox(bResult);
    }
    else
    {
        return CSettinguiXSIDelegatebase::OnSoftkeyAction(iType, bLongPress, nDataIndex);
    }
    return true;
}

bool CSettinguiVoiceMailListDelegate::OnAction(int eActionType, int iDataIndex)
{
    if (NULL == m_pPageData)
    {
        return false;
    }

    if (SET_ACTION_TYPE_CLICK == eActionType && m_pPageData->GetTotalItemNumber() > 0)
    {
        bool bResult = vvMail_ReqPlayContentByIndex(GetFocusItemID());
        ShowDownloadMsgBox(bResult);
        return true;
    }

    if (SET_ACTION_TYPE_FOCUS == eActionType)
    {
        g_SetUICallBackFun.RefreshDlgTitle(m_pPageData->GetPageTitle());
    }

    return CSettinguiXSIDelegatebase::OnAction(eActionType, iDataIndex);
}

void CSettinguiVoiceMailListDelegate::OnCancleClick()
{
    vvMail_CancelAllRequest();
}

void CSettinguiVoiceMailListDelegate::UpdateUIData()
{
    LoadVoiceListItemData(m_pPageData);

    g_SetUICallBackFun.RefreshUI(false);
}

void CSettinguiVoiceMailListDelegate::OnStarRequest(xThreadTimer* timer, LPVOID pExtraData)
{
    m_tmStarRequest.KillTimer();

    vvMail_ReqVoicemessageList();

    m_eMsgboxID = XSI_MSGBOX_ID_LOADING;

    ShowMessageBox(_LANG(TRID_NOTE), _LANG(TRID_VOICEMAIL_DOWNLOADING), XSI_MSGBOX_ID_LOADING, -1,
                   MESSAGEBOX_Cancel);
}

void CSettinguiVoiceMailListDelegate::OnOptionBoxCallBack(int nAction)
{
    int nID = GetFocusItemID();
    if (OPTION_CALL_BACK == nAction)
    {
        return ProcessCallBack(nID);
    }
    else if (OPTION_MARK_HEARD == nAction)
    {
        bool bResult = vvMail_ReqMarkByIndex(nID, true);
        ShowOperateMsgBox(bResult);
    }
    else if (OPTION_MARK_NEW == nAction)
    {
        bool bResult = vvMail_ReqMarkByIndex(nID, false);
        ShowOperateMsgBox(bResult);
    }
    else if (OPTION_MARK_ALL_NEW == nAction)
    {
        bool bResult = vvMail_ReqMarkAllUnRead();
        ShowOperateMsgBox(bResult);
    }
    else if (OPTION_MARK_ALL_HEARD == nAction)
    {
        bool bResult = vvMail_ReqMarkAllRead();
        ShowOperateMsgBox(bResult);
    }
    else if (OPTION_DETAIL == nAction)
    {
        EnterDetail(nID);
    }
    else if (OPTION_MOVE_TO_CONTACT == nAction)
    {
        AddToContacts(nID);
    }
    else if (OPTION_ADD_TO_PERSONAL == nAction)
    {
        AddToContacts(nID);
    }
}

void CSettinguiVoiceMailListDelegate::ProcessCallBack(int nID)
{
    sVoiceMailData stVoiceMailData;
    if (vvMail_GetDetailByIndex(stVoiceMailData, nID))
    {
        yl::string strNumber = stVoiceMailData.m_strNumber;
        yl::string strName = stVoiceMailData.m_strName;
        talklogic_CallOut(strNumber.c_str(), strName.c_str(), m_iLineID);
    }
}

bool CSettinguiVoiceMailListDelegate::EnterDetail(int nID)
{
    PageOpenData opPageData(kszPageIDVoiceMailDetail);
    opPageData.pExtraData = (void *)&nID;
    g_SetUICallBackFun.EnterPage(opPageData);
    SetRefreshDataWhenTopDelegate(true);
    return true;
}

bool CSettinguiVoiceMailListDelegate::EnterPlay(int nID)
{
    PageOpenData opPageData(kszPageIDVoiceMailPlay);
    opPageData.pExtraData = (void *)&nID;
    g_SetUICallBackFun.EnterPage(opPageData);

    SetRefreshDataWhenTopDelegate(true);
    return true;
}

bool CSettinguiVoiceMailListDelegate::AddToContacts(int nID)
{
    PageOpenData opPageData(kszPageIDVoiceMailAddToPersonal);
    opPageData.pExtraData = (void *)&nID;
    g_SetUICallBackFun.EnterPage(opPageData);
    return true;
}

void CSettinguiVoiceMailListDelegate::OnOptionClick()
{
    VEC_OPTION_ITEM_DATA vecOption;
    ST_OPTION_ITEM_DATA stOption;

    sVoiceMailData refData;
    vvMail_GetDetailByIndex(refData, GetFocusItemID());

    AddOption(vecOption, OPTION_DETAIL, _LANG(TRID_DETAIL));

    if (!refData.m_bPrivacy)
    {
        AddOption(vecOption, OPTION_CALL_BACK, _LANG(TRID_VOICEMAIL_OPTION_CALL_BACK));

        SmartPtr pDetail = Dir_GetDetailById(kszPersonalId, BSFT_DIR);

        if (!pDetail.IsEmpty())
        {
            AddOption(vecOption, OPTION_ADD_TO_PERSONAL, _LANG(TRID_ADD_TO_PERSONAL));
        }
    }

    if (refData.m_bRead)
    {
        AddOption(vecOption, OPTION_MARK_NEW, _LANG(TRID_VOICEMAIL_OPTION_MARK_NEW));
    }
    else
    {
        AddOption(vecOption, OPTION_MARK_HEARD, _LANG(TRID_VOICEMAIL_OPTION_MARK_HEARD));
    }

    AddOption(vecOption, OPTION_MARK_ALL_NEW, _LANG(TRID_VOICEMAIL_OPTION_MARK_ALL_NEW));

    AddOption(vecOption, OPTION_MARK_ALL_HEARD, _LANG(TRID_VOICEMAIL_OPTION_MARK_ALL_HEARD));

    BaseUI_ShowOptionMenu(this, vecOption);
}

int CSettinguiVoiceMailListDelegate::GetFocusItemID()
{
    if (NULL != m_pPageData)
    {
        CSettingUIItemDataBasePtr pItemData  = m_pPageData->GetItemByDataIndex(
                m_pPageData->GetFocusIndex());
        if (NULL != pItemData)
        {
            return atoi(pItemData->GetItemID().c_str());
        }
    }

    return -1;
}

void CSettinguiVoiceMailListDelegate::ShowOperateMsgBox(bool bSuccessed)
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

void CSettinguiVoiceMailListDelegate::ShowDownloadMsgBox(bool bSuccessed)
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

void CSettinguiVoiceMailListDelegate::AddOption(VEC_OPTION_ITEM_DATA & vecOption, int nAction,
        const yl::string & strDisp)
{
    ST_OPTION_ITEM_DATA stOption;
    stOption.m_nAction = nAction;
    stOption.m_strTitle = strDisp;
    vecOption.push_back(stOption);
}

void CSettinguiVoiceMailListDelegate::LoadVoiceListItemData(CSettingUIPageDataBase * pPageData)
{
    //清除页面数据
    pPageData->ClearItemData();

    VoiceMailListData voiceMailListData;
    if (!vvMail_GetVoiceMailListData(&voiceMailListData) || voiceMailListData.m_nTotalCount <= 0)
    {
        return;
    }

    LIST_VOICE_MAIL_DATA::iterator it = voiceMailListData.m_listData.begin();
    LIST_VOICE_MAIL_DATA::iterator it_end = voiceMailListData.m_listData.end();

    for (int iIndex = 0; it != it_end; ++iIndex, ++it)
    {
        sVoiceMailData & voiceMailData = *it;
        yl::string strName = "";
        if (voiceMailData.m_bUrgent)
        {
            strName.append("!");
        }

        if (voiceMailData.m_bConfidential)
        {
            strName.append("*");
        }

        if (!voiceMailData.m_bPrivacy)
        {
            strName.append(voiceMailData.m_strName.c_str());
        }
        else
        {
            strName.append(_LANG(TRID_ANONYMOUS));
        }

        yl::string strTime = voiceMailData.m_strTime.substr(0, 10);
        time_t tTime = atoi(strTime.c_str());
        strTime = SettingUILogic_Time2NewFormatString(tTime, false);

        yl::string strItemID = commonAPI_FormatString("%d", iIndex);
        AddVoiceMailItem(pPageData, strItemID, strName, strTime, voiceMailData.m_bRead);
    }
}

void CSettinguiVoiceMailListDelegate::AddVoiceMailItem(CSettingUIPageDataBase * pPageData,
        const yl::string & strItemID, const yl::string & strDisp, const yl::string & strValue, bool bRead)
{
    if (NULL != pPageData)
    {
        CSettingUITextMessageItemDataPtr pItemData = new CSettingUITextMessageItemData;
        if (NULL != pItemData)
        {
            pItemData->SetItemID(strItemID);
            pItemData->SetDisplayName(strDisp);
            pItemData->SetDateString(strValue);
            pItemData->SetShowColon(false);
            yl::string strIconPath = bRead ? PIC_VOICE_MAIL_READ : PIC_VOICE_MAIL_UNREAD;
            pItemData->SetStatusIcon(strIconPath);

            if (!pPageData->AddItem(pItemData))
            {
                delete pItemData;
            }
        }
    }
}

#endif //IF_FEATURE_BROADSOFT_VOICEMAIL
