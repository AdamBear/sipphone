#include "settinguicallcontrollcfnrdelegate.h"

#ifdef IF_FEATURE_BROADSOFT_CALL_FORWARD_NOT_REACHABLE

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

CSettinguiCallControllCFNRDelegate::CSettinguiCallControllCFNRDelegate()
{
    etl_RegisterMsgHandle(XSI_MESSAGE_BEGIN, XSI_MESSAGE_END,
                          &CSettinguiCallControllCFNRDelegate::OnMessage);
}

CSettinguiCallControllCFNRDelegate::~CSettinguiCallControllCFNRDelegate()
{
    etl_UnregisterMsgHandle(XSI_MESSAGE_BEGIN, XSI_MESSAGE_END,
                            &CSettinguiCallControllCFNRDelegate::OnMessage);
}

CSettingUIDelegateBase * CSettinguiCallControllCFNRDelegate::CreateCallControllCFNRDelegate()
{
    CSettinguiCallControllCFNRDelegate * pDelegate = new CSettinguiCallControllCFNRDelegate;

    return (CSettingUIDelegateBasePtr)pDelegate;
}

LRESULT CSettinguiCallControllCFNRDelegate::OnMessage(msgObject & msg)
{
    if (NULL == g_pSettingUIManager)
    {
        return FALSE;
    }

    CSettinguiCallControllCFNRDelegate * pDelegate = (CSettinguiCallControllCFNRDelegate *)
            g_pSettingUIManager->GetPageDelegate(kszPageIDCallControllCFNR);
    if (NULL == pDelegate || pDelegate !=  g_pSettingUIManager->GetTopDelegate())
    {
        return FALSE;
    }

    if (XSI_MESSAGE_REQ_FINISH == msg.message)
    {
        if (XSI_CALLFORWARD_NOTREACHABLE_GET == msg.wParam)
        {
            MessageBox_RemoveMessageBox(pDelegate, MessageBox_OK, XSI_MSGBOX_ID_LOADING);

            SCallForwardNotReachable stCFNR;
            CFNR_GetForwardNotReachableInfo(stCFNR, msg.lParam);

            pDelegate->RefreshtCFNRData(stCFNR.bActive, stCFNR.strPhoneNumber);

            g_SetUICallBackFun.RefreshUI(false, pDelegate->GetPageID());
        }

        else if (XSI_CALLFORWARD_NOTREACHABLE_PUT == msg.wParam)
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

CSettingUIPageDataBase * CSettinguiCallControllCFNRDelegate::LoadPageData(
    bool bReLoad /* = false */)
{
    CSettinguiXSIDelegatebase::LoadPageData(bReLoad);

    if (bReLoad && NULL != m_pPageData)
    {
        SCallForwardNotReachable stCFNR;
        CFNR_GetForwardNotReachableInfo(stCFNR, m_iLineID);
        RefreshtCFNRData(stCFNR.bActive, stCFNR.strPhoneNumber);

        CFNR_QueryForwardNotReachable(m_iLineID);

        ShowMessageBox(_LANG(TRID_NOTE), _LANG(TRID_LOADING_PLEASE_WAIT), XSI_MSGBOX_ID_LOADING, -1,
                       MESSAGEBOX_Cancel);

        m_eMsgboxID = XSI_MSGBOX_ID_LOADING;
    }

    return m_pPageData;
}

bool CSettinguiCallControllCFNRDelegate::SavePageData(CSettingUIPageDataBase * pPageData,
        bool bBackToPrevious /* = true */, bool bPopHint /* = true */)
{
    if (NULL == pPageData || !pPageData->IsEnable())
    {
        return false;
    }

    bool bActive = GetComboboxValue(FORWARDNOTREACHABLE_ACTIVE);
    yl::string strPhoneNumber = GetValue(FORWARDNOTREACHABLE_PHONENUMBER);

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
    CFNR_SetForwardNotReachable(bActive, strPhoneNumber.c_str(), m_iLineID);

    ShowMessageBox(_LANG(TRID_NOTE), _LANG(TRID_SAVING_PLEASE_WAIT), XSI_MSGBOX_ID_SAVE, -1,
                   MESSAGEBOX_Cancel);

    m_eMsgboxID = XSI_MSGBOX_ID_SAVE;

    return true;
}

void CSettinguiCallControllCFNRDelegate::RefreshtCFNRData(bool bActive,
        const yl::string strPhoneNumber)
{
    SetComboboxValue(FORWARDNOTREACHABLE_ACTIVE, bActive);
    SetValue(FORWARDNOTREACHABLE_PHONENUMBER, strPhoneNumber);
}

void CSettinguiCallControllCFNRDelegate::OnCancleClick()
{
    CFNR_CancelRequest();
}

#endif
