#include "settinguicallcontrollcallwaitingdelegate.h"

#ifdef IF_FEATURE_BROADSOFT_CALL_WAITING
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

CSettinguiCallControllCallWaitingDelegate::CSettinguiCallControllCallWaitingDelegate()
{
    etl_RegisterMsgHandle(XSI_MESSAGE_BEGIN, XSI_MESSAGE_END,
                          &CSettinguiCallControllCallWaitingDelegate::OnMessage);
}

CSettinguiCallControllCallWaitingDelegate::~CSettinguiCallControllCallWaitingDelegate()
{
    etl_UnregisterMsgHandle(XSI_MESSAGE_BEGIN, XSI_MESSAGE_END,
                            &CSettinguiCallControllCallWaitingDelegate::OnMessage);
}

CSettingUIDelegateBase *
CSettinguiCallControllCallWaitingDelegate::CreateCallControllCallWaitingDelegate()
{
    CSettinguiCallControllCallWaitingDelegate * pDelegate = new
    CSettinguiCallControllCallWaitingDelegate;

    return (CSettingUIDelegateBasePtr)pDelegate;
}

LRESULT CSettinguiCallControllCallWaitingDelegate::OnMessage(msgObject & msg)
{
    if (NULL == g_pSettingUIManager)
    {
        return FALSE;
    }

    CSettinguiCallControllCallWaitingDelegate * pDelegate = (CSettinguiCallControllCallWaitingDelegate
            *)g_pSettingUIManager->GetPageDelegate(kszPageIDCallControllCallWaiting);
    if (NULL == pDelegate || pDelegate !=  g_pSettingUIManager->GetTopDelegate())
    {
        return FALSE;
    }

    if (XSI_MESSAGE_REQ_FINISH == msg.message)
    {
        if (XSI_CALL_WAITING_GET == msg.wParam)
        {
            MessageBox_RemoveMessageBox(pDelegate, MessageBox_OK, XSI_MSGBOX_ID_LOADING);

            pDelegate->RefreshtCallWaitingData();

            g_SetUICallBackFun.RefreshUI(false, pDelegate->GetPageID());
        }

        else if (XSI_CALL_WAITING_PUT == msg.wParam)
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

CSettingUIPageDataBase * CSettinguiCallControllCallWaitingDelegate::LoadPageData(
    bool bReLoad /* = false */)
{
    CSettinguiXSIDelegatebase::LoadPageData(bReLoad);

    if (bReLoad && NULL != m_pPageData)
    {
        RefreshtCallWaitingData();

        CW_QueryCallWaiting(m_iLineID);

        ShowMessageBox(_LANG(TRID_NOTE), _LANG(TRID_LOADING_PLEASE_WAIT), XSI_MSGBOX_ID_LOADING, -1,
                       MESSAGEBOX_Cancel);

        m_eMsgboxID = XSI_MSGBOX_ID_LOADING;
    }

    return m_pPageData;
}

bool CSettinguiCallControllCallWaitingDelegate::SavePageData(CSettingUIPageDataBase * pPageData,
        bool bBackToPrevious /* = true */, bool bPopHint /* = true */)
{
    if (NULL == pPageData || !pPageData->IsEnable())
    {
        return false;
    }

    bool bEnable = GetComboboxValue(CALLCONTROL_CALLWAITING_ENABLE);

    // 给服务器发送请求，保存结果
    CW_SetCallWaitingState(bEnable, m_iLineID);

    ShowMessageBox(_LANG(TRID_NOTE), _LANG(TRID_SAVING_PLEASE_WAIT), XSI_MSGBOX_ID_SAVE, -1,
                   MESSAGEBOX_Cancel);

    m_eMsgboxID = XSI_MSGBOX_ID_SAVE;

    return true;
}

void CSettinguiCallControllCallWaitingDelegate::RefreshtCallWaitingData()
{
    bool bEnable = CW_GetCallWaitingState(m_iLineID);
    SetComboboxValue(CALLCONTROL_CALLWAITING_ENABLE, bEnable);
}

void CSettinguiCallControllCallWaitingDelegate::OnCancleClick()
{
    CW_CancelRequest();
}

#endif
