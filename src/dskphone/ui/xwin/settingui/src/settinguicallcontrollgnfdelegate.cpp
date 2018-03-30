#include "settinguicallcontrollgnfdelegate.h"

#ifdef IF_FEATURE_BROADSOFT_GROUP_NIGHT_FORWARD
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

CSettinguiCallControllGNFDelegate::CSettinguiCallControllGNFDelegate()
{
    etl_RegisterMsgHandle(XSI_MESSAGE_BEGIN, XSI_MESSAGE_END,
                          &CSettinguiCallControllGNFDelegate::OnMessage);
}

CSettinguiCallControllGNFDelegate::~CSettinguiCallControllGNFDelegate()
{
    etl_UnregisterMsgHandle(XSI_MESSAGE_BEGIN, XSI_MESSAGE_END,
                            &CSettinguiCallControllGNFDelegate::OnMessage);
}

CSettingUIDelegateBase * CSettinguiCallControllGNFDelegate::CreateCallControllGNFDelegate()
{
    CSettinguiCallControllGNFDelegate * pDelegate = new CSettinguiCallControllGNFDelegate;

    return (CSettingUIDelegateBasePtr)pDelegate;
}

LRESULT CSettinguiCallControllGNFDelegate::OnMessage(msgObject & msg)
{
    if (NULL == g_pSettingUIManager)
    {
        return FALSE;
    }

    CSettinguiCallControllGNFDelegate * pDelegate = (CSettinguiCallControllGNFDelegate *)
            g_pSettingUIManager->GetPageDelegate(kszPageIDCallControllGNF);
    if (NULL == pDelegate || pDelegate !=  g_pSettingUIManager->GetTopDelegate())
    {
        return FALSE;
    }

    if (XSI_MESSAGE_REQ_FINISH == msg.message && XSI_GROUPNIGHT_FORWARD_GET == msg.wParam)
    {
        MessageBox_RemoveMessageBox(pDelegate, MessageBox_OK, XSI_MSGBOX_ID_LOADING);

        pDelegate->RefreshtGNFData();

        g_SetUICallBackFun.RefreshUI(false, pDelegate->GetPageID());
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

CSettingUIPageDataBase * CSettinguiCallControllGNFDelegate::LoadPageData(bool bReLoad /* = false */)
{
    CSettinguiXSIDelegatebase::LoadPageData(bReLoad);

    if (bReLoad && NULL != m_pPageData)
    {
        RefreshtGNFData();

        GNF_QueryGnf(m_iLineID);

        ShowMessageBox(_LANG(TRID_NOTE), _LANG(TRID_LOADING_PLEASE_WAIT), XSI_MSGBOX_ID_LOADING, -1,
                       MESSAGEBOX_Cancel);

        m_eMsgboxID = XSI_MSGBOX_ID_LOADING;
    }

    return m_pPageData;
}

bool CSettinguiCallControllGNFDelegate::PageDataFilter(CSettingUIPageDataBase * pPagedata,
        bool bReload /* = false */)
{
    bool bResult = CSettinguiXSIDelegatebase::PageDataFilter(pPagedata, bReload);

    SetGNFItemShow(GNF_HOLIDAYNAME);
    SetGNFItemShow(GNF_BUSSINESSNAME);
    return bResult;
}

bool CSettinguiCallControllGNFDelegate::SavePageData(CSettingUIPageDataBase * pPageData,
        bool bBackToPrevious /* = true */, bool bPopHint /* = true */)
{
    return true;
}

void CSettinguiCallControllGNFDelegate::RefreshtGNFData()
{
    if (NULL != m_pPageData)
    {
        SGroupNightForward stGNFInfo;
        GNF_GetGnfInfo(stGNFInfo, m_iLineID);
        SetValue(GNF_USERNIGHTFORWARDINGMODE, stGNFInfo.strUserNightFwdMode);
        SetValue(GNF_HOLIDAYNAME, stGNFInfo.strHolidayScheduleName);
        SetValue(GNF_BUSSINESSNAME, stGNFInfo.strBusinessHoursName);
    }
}

void CSettinguiCallControllGNFDelegate::OnCancleClick()
{
    GNF_CancelRequest();
}

void CSettinguiCallControllGNFDelegate::SetGNFItemShow(const yl::string & strItemID)
{
    if (NULL == m_pPageData)
    {
        return;
    }

    if (GetValue(strItemID).empty())
    {
        SetItemShow(m_pPageData, strItemID, false);
    }
    else
    {
        SetItemShow(m_pPageData, strItemID, true);
    }
}

#endif
