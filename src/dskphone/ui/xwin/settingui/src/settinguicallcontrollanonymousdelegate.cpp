#include "settinguicallcontrollanonymousdelegate.h"

#ifdef IF_FEATURE_BROADSOFT_CLID

#include "uicallbackfun.h"
#include "settinguimanager.h"
#include "settingui/include/itemnamedefine.h"
#include "settingui/include/pageiddefine.h"
#include "settinguilogic/src/settinguicomboboxitemdata.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "account/include/modaccount.h"
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

CSettinguiCallControllAnonymousDelegate::CSettinguiCallControllAnonymousDelegate()
{
    etl_RegisterMsgHandle(XSI_MESSAGE_BEGIN, XSI_MESSAGE_END,
                          &CSettinguiCallControllAnonymousDelegate::OnMessage);
}

CSettinguiCallControllAnonymousDelegate::~CSettinguiCallControllAnonymousDelegate()
{
    etl_UnregisterMsgHandle(XSI_MESSAGE_BEGIN, XSI_MESSAGE_END,
                            &CSettinguiCallControllAnonymousDelegate::OnMessage);
}

CSettingUIDelegateBase *
CSettinguiCallControllAnonymousDelegate::CreateCallControllAnonymousDelegate()
{
    CSettinguiCallControllAnonymousDelegate * pDelegate = new CSettinguiCallControllAnonymousDelegate;

    return (CSettingUIDelegateBasePtr)pDelegate;
}

LRESULT CSettinguiCallControllAnonymousDelegate::OnMessage(msgObject & msg)
{
    if (NULL == g_pSettingUIManager)
    {
        return FALSE;
    }

    CSettingUIDelegateBase * pDelegate = g_pSettingUIManager->GetPageDelegate(
            kszPageIDCallControllHideNumber);
    if (NULL == pDelegate)
    {
        pDelegate = g_pSettingUIManager->GetPageDelegate(kszPageIDCallControllAnonymoRrejection);
    }

    CSettinguiCallControllAnonymousDelegate * pAnymousDelegate =
        static_cast<CSettinguiCallControllAnonymousDelegate *>(pDelegate);
    if (NULL == pAnymousDelegate || pAnymousDelegate !=  g_pSettingUIManager->GetTopDelegate())
    {
        return FALSE;
    }

    if (XSI_MESSAGE_REQ_FINISH == msg.message)
    {
        if ((XSI_ANONYOUSCALL_GET_STATE == msg.wParam
                && kszPageIDCallControllHideNumber == pDelegate->GetPageID())
                || (XSI_ANONYMOUNSCALL_REJECT_GET_STATE == msg.wParam
                    && kszPageIDCallControllAnonymoRrejection == pDelegate->GetPageID()))
        {
            MessageBox_RemoveMessageBox(pAnymousDelegate, MessageBox_OK, XSI_MSGBOX_ID_LOADING);

            pAnymousDelegate->RefreshtAnonymousData();

            g_SetUICallBackFun.RefreshUI(false, pDelegate->GetPageID());
        }
        else if (XSI_ANONYMOUNSCALL_REJECT_PUT_STATE == msg.wParam
                 || XSI_ANONYOUSCALL_PUT_STATE == msg.wParam)
        {
            MessageBox_RemoveMessageBox(pAnymousDelegate, MessageBox_OK, XSI_MSGBOX_ID_SAVE);
        }
        else
        {
            return FALSE;
        }
    }
    else if (XSI_MESSAGE_REQ_FAIL == msg.message)
    {
        pAnymousDelegate->OnMessageReqFail(msg);
    }
    else
    {
        return FALSE;
    }

    return TRUE;
}

CSettingUIPageDataBase * CSettinguiCallControllAnonymousDelegate::LoadPageData(
    bool bReLoad /* = false */)
{
    CSettinguiXSIDelegatebase::LoadPageData(bReLoad);

    if (bReLoad && NULL != m_pPageData && IsHasAccountRegister())
    {
        LoadAnymousItem();

        RefreshtAnonymousData();

        if (kszPageIDCallControllHideNumber == GetPageID())
        {
            CLID_QueryAnonymousCallState(m_iLineID);
        }
        else
        {
            CLID_QueryAnonymousCallRejectionState(m_iLineID);
        }

        ShowMessageBox(_LANG(TRID_NOTE), _LANG(TRID_LOADING_PLEASE_WAIT), XSI_MSGBOX_ID_LOADING, -1,
                       MESSAGEBOX_Cancel);

        m_eMsgboxID = XSI_MSGBOX_ID_LOADING;
    }

    return m_pPageData;
}

bool CSettinguiCallControllAnonymousDelegate::SavePageData(CSettingUIPageDataBase * pPageData,
        bool bBackToPrevious /* = true */, bool bPopHint /* = true */)
{
    if (NULL == pPageData || !pPageData->IsEnable())
    {
        return false;
    }

    bool bActive = (1 == GetComboboxValue(BROADSOFT_ANONYMOUS_LINE)) ? true : false;

    if (kszPageIDCallControllHideNumber == GetPageID())
    {
        CLID_SetAnonymousCallState(bActive, m_iLineID);
    }
    else
    {
        CLID_SetAnonymousCallRejectonState(bActive, m_iLineID);
    }

    ShowMessageBox(_LANG(TRID_NOTE), _LANG(TRID_SAVING_PLEASE_WAIT), XSI_MSGBOX_ID_SAVE, -1,
                   MESSAGEBOX_Cancel);

    m_eMsgboxID = XSI_MSGBOX_ID_SAVE;

    return true;
}

void CSettinguiCallControllAnonymousDelegate::RefreshtAnonymousData()
{
    bool bActive = false;

    if (kszPageIDCallControllHideNumber == GetPageID())
    {
        bActive = CLID_GetAnonymousCallState(m_iLineID);
    }
    else
    {
        bActive = CLID_GetAnonymousCallRejectionState(m_iLineID);
    }

    SetComboboxValue(BROADSOFT_ANONYMOUS_LINE, bActive);
}

void CSettinguiCallControllAnonymousDelegate::OnCancleClick()
{
    CLID_CancelRequest();
}

void CSettinguiCallControllAnonymousDelegate::LoadAnymousItem()
{
    if (NULL == m_pPageData || !acc_IsAccountAvailable(m_iLineID))
    {
        return;
    }

    CSettingUIComboBoxItemDataPtr pItemData = new CSettingUIComboBoxItemData;
    if (NULL != pItemData)
    {
        yl::string strDisplay = commonAPI_FormatString("%s", acc_GetAccountShowText(m_iLineID).c_str());
        pItemData->SetItemID(BROADSOFT_ANONYMOUS_LINE);
        pItemData->SetDisplayName(strDisplay);
        pItemData->SetShowIndex(true);

        for (int i = 0; i < 2; ++i)
        {
            CSettingUIComboBoxOption * pOption = new CSettingUIComboBoxOption;
            if (NULL == pOption)
            {
                continue;
            }

            pOption->m_strDisplayName = (0 == i) ? _LANG(TRID_OFF) : _LANG(TRID_ON);
            pOption->m_strItemVale = commonAPI_FormatString("%d", i);

            if (!pItemData->AddOptionItem(pOption))
            {
                delete pOption;
            }
        }

        if (!m_pPageData->AddItem(pItemData))
        {
            delete pItemData;
        }
    }
}

#endif
