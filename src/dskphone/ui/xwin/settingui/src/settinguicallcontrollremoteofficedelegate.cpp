#include "settinguicallcontrollremoteofficedelegate.h"

#ifdef IF_FEATURE_BROADSOFT_REMOTE_OFFICE
#include "uicallbackfun.h"
#include "settinguimanager.h"
#include "settingui/include/itemnamedefine.h"
#include "settingui/include/pageiddefine.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "broadsoft/xsiactionhelper/include/xsicommon.h"
#include "broadsoft/xsiactionhelper/include/cbasehelper.h"
#include "broadsoft/anywhere/include/anywherecontroller.h"
#include "broadsoft/bwcallcontrol/include/bwcommon.h"
#include "broadsoft/bwcallcontrol/include/modbwcallcontrol.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "messagebox/messageboxbase.h"
#include "messagebox/modmessagebox.h"
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"

namespace
{
#define INDEX_PHONE_NUMBER 1
}

CSettinguiCallControllRemoteOfficeDelegate::CSettinguiCallControllRemoteOfficeDelegate()
{
    etl_RegisterMsgHandle(XSI_MESSAGE_BEGIN, XSI_MESSAGE_END,
                          &CSettinguiCallControllRemoteOfficeDelegate::OnMessage);
}

CSettinguiCallControllRemoteOfficeDelegate::~CSettinguiCallControllRemoteOfficeDelegate()
{
    etl_UnregisterMsgHandle(XSI_MESSAGE_BEGIN, XSI_MESSAGE_END,
                            &CSettinguiCallControllRemoteOfficeDelegate::OnMessage);
}

CSettingUIDelegateBase *
CSettinguiCallControllRemoteOfficeDelegate::CreateCallControllRemoteOfficeDelegate()
{
    CSettinguiCallControllRemoteOfficeDelegate * pDelegate = new
    CSettinguiCallControllRemoteOfficeDelegate;

    return (CSettingUIDelegateBasePtr)pDelegate;
}

LRESULT CSettinguiCallControllRemoteOfficeDelegate::OnMessage(msgObject & msg)
{
    if (NULL == g_pSettingUIManager)
    {
        return FALSE;
    }

    CSettinguiCallControllRemoteOfficeDelegate * pDelegate =
        (CSettinguiCallControllRemoteOfficeDelegate *)g_pSettingUIManager->GetPageDelegate(
            kszPageIDCallControllRemoteOffice);
    if (NULL == pDelegate || pDelegate !=  g_pSettingUIManager->GetTopDelegate())
    {
        return FALSE;
    }

    if (XSI_MESSAGE_REQ_FINISH == msg.message)
    {
        if (XSI_REMOTEOFFICE_GET == msg.wParam)
        {
            MessageBox_RemoveMessageBox(pDelegate, MessageBox_OK, XSI_MSGBOX_ID_LOADING);

            SRemoteOffice stRemoteOffice;
            RO_GetRemoteOfficeInfo(stRemoteOffice, msg.lParam);

            pDelegate->RefreshtRemoteOfficeData(stRemoteOffice.bActive, stRemoteOffice.strPhoneNumber);

            g_SetUICallBackFun.RefreshUI(false, pDelegate->GetPageID());
        }

        else if (XSI_REMOTEOFFICE_PUT == msg.wParam)
        {
            MessageBox_RemoveMessageBox(pDelegate, MessageBox_OK, XSI_MSGBOX_ID_SAVE);
        }
        else
        {
            return FALSE;
        }
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

CSettingUIPageDataBase * CSettinguiCallControllRemoteOfficeDelegate::LoadPageData(
    bool bReLoad /* = false */)
{
    CSettinguiXSIDelegatebase::LoadPageData(bReLoad);

    if (bReLoad && NULL != m_pPageData)
    {
        SRemoteOffice stRemoteOffice;
        RO_GetRemoteOfficeInfo(stRemoteOffice, m_iLineID);
        RefreshtRemoteOfficeData(stRemoteOffice.bActive, stRemoteOffice.strPhoneNumber);

        RO_QueryRemoteOffice(m_iLineID);

        ShowMessageBox(_LANG(TRID_NOTE), _LANG(TRID_LOADING_PLEASE_WAIT), XSI_MSGBOX_ID_LOADING, -1,
                       MESSAGEBOX_Cancel);

        m_eMsgboxID = XSI_MSGBOX_ID_LOADING;
    }

    return m_pPageData;
}

bool CSettinguiCallControllRemoteOfficeDelegate::SavePageData(CSettingUIPageDataBase * pPageData,
        bool bBackToPrevious /* = true */, bool bPopHint /* = true */)
{
    if (NULL == m_pPageData || !pPageData->IsEnable())
    {
        return false;
    }

    bool bActive = (1 == GetComboboxValue(REMOTEOFFICE_ACTIVE));
    yl::string strPhoneNumber = GetValue(REMOTEOFFICE_PHONENUMBER);

    if (!BWCallControl_CheckPhoneNumberValid(strPhoneNumber.c_str())
            && !(!bActive && strPhoneNumber.empty()))
    {
        CSettingUIDataValidInfo validInfo;
        validInfo.SetValidIndex(INDEX_PHONE_NUMBER);
        validInfo.SetValidNote(_LANG(TRID_INVALID_NUMBER));
        g_SetUICallBackFun.DealWithValidData(validInfo);
        return false;
    }

    // 给服务器发送请求，保存结果
    RO_SetRemoteOffice(bActive, strPhoneNumber.c_str(), m_iLineID);

    ShowMessageBox(_LANG(TRID_NOTE), _LANG(TRID_SAVING_PLEASE_WAIT), XSI_MSGBOX_ID_SAVE, -1,
                   MESSAGEBOX_Cancel);

    m_eMsgboxID = XSI_MSGBOX_ID_SAVE;

    return true;
}

void CSettinguiCallControllRemoteOfficeDelegate::RefreshtRemoteOfficeData(bool bActive,
        const yl::string strPhoneNumber)
{
    SetComboboxValue(REMOTEOFFICE_ACTIVE, bActive);
    SetValue(REMOTEOFFICE_PHONENUMBER, strPhoneNumber);
}

void CSettinguiCallControllRemoteOfficeDelegate::OnCancleClick()
{
    RO_CancelRequest();
}

#endif
