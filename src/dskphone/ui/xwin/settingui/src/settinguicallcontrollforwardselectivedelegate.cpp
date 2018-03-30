#include "settinguicallcontrollforwardselectivedelegate.h"

#ifdef IF_FEATURE_BROADSOFT_CALL_FORWARD_SELECTIVE

#include "uicallbackfun.h"
#include "settinguimanager.h"
#include "settingui/include/itemnamedefine.h"
#include "settingui/include/pageiddefine.h"
#include "settinguilogic/src/settinguicomboboxitemdata.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "broadsoft/xsiactionhelper/include/xsicommon.h"
#include "broadsoft/xsiactionhelper/include/cbasehelper.h"
#include "broadsoft/bwcallcontrol/include/bwcommon.h"
#include "broadsoft/bwcallcontrol/include/modbwcallcontrol.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "messagebox/messageboxbase.h"
#include "messagebox/modmessagebox.h"
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"

namespace
{
#define INDEX_FORWARD_TO 1
typedef SForwardSelectiveData::VecCriteriaItem VecItem;
}

CSettinguiCallControllForwardSelectiveDelegate::CSettinguiCallControllForwardSelectiveDelegate()
    : m_bRequest(true)
{
    etl_RegisterMsgHandle(XSI_MESSAGE_BEGIN, XSI_MESSAGE_END,
                          &CSettinguiCallControllForwardSelectiveDelegate::OnMessage);
}

CSettinguiCallControllForwardSelectiveDelegate::~CSettinguiCallControllForwardSelectiveDelegate()
{
    etl_UnregisterMsgHandle(XSI_MESSAGE_BEGIN, XSI_MESSAGE_END,
                            &CSettinguiCallControllForwardSelectiveDelegate::OnMessage);
}

CSettingUIDelegateBase *
CSettinguiCallControllForwardSelectiveDelegate::CreateCallControllForwardSelectiveDelegate()
{
    CSettinguiCallControllForwardSelectiveDelegate * pDelegate = new
    CSettinguiCallControllForwardSelectiveDelegate;

    return (CSettingUIDelegateBasePtr)pDelegate;
}

LRESULT CSettinguiCallControllForwardSelectiveDelegate::OnMessage(msgObject & msg)
{
    if (NULL == g_pSettingUIManager)
    {
        return FALSE;
    }

    CSettinguiCallControllForwardSelectiveDelegate * pDelegate =
        (CSettinguiCallControllForwardSelectiveDelegate *)g_pSettingUIManager->GetPageDelegate(
            kszPageIDCallControllForwardSelective);
    if (NULL == pDelegate || pDelegate !=  g_pSettingUIManager->GetTopDelegate())
    {
        return FALSE;
    }

    if (XSI_MESSAGE_REQ_FINISH == msg.message)
    {
        if (XSI_FORWARD_SELECTIVE_GET == msg.wParam)
        {
            MessageBox_RemoveMessageBox(pDelegate, MessageBox_OK, XSI_MSGBOX_ID_LOADING);

            pDelegate->RefreshtForwardSelectiveData();

            pDelegate->SetRequest(false);

            g_SetUICallBackFun.RefreshUI(false, pDelegate->GetPageID());
        }
        else if (XSI_FORWARD_SELECTIVE_PUT == msg.wParam)
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

CSettingUIPageDataBase * CSettinguiCallControllForwardSelectiveDelegate::LoadPageData(
    bool bReLoad /* = false */)
{
    CSettinguiXSIDelegatebase::LoadPageData(bReLoad);

    if (bReLoad && NULL != m_pPageData)
    {
        RefreshtForwardSelectiveData();

        if (m_bRequest)
        {
            FS_QueryForwardSelective(m_iLineID);
            ShowMessageBox(_LANG(TRID_NOTE), _LANG(TRID_LOADING_PLEASE_WAIT), XSI_MSGBOX_ID_LOADING, -1,
                           MESSAGEBOX_Cancel);
            m_eMsgboxID = XSI_MSGBOX_ID_LOADING;
        }

        m_bRequest = true;
    }

    return m_pPageData;
}

bool CSettinguiCallControllForwardSelectiveDelegate::SavePageData(CSettingUIPageDataBase *
        pPageData, bool bBackToPrevious /* = true */, bool bPopHint /* = true */)
{
    if (NULL == pPageData || !pPageData->IsEnable())
    {
        return false;
    }

    SForwardSelectiveData stFSInfo;
    stFSInfo.bActive = GetComboboxValue(FORWARD_SELECTIVE_SWITCH);
    stFSInfo.strForwardToPhoneNumber = GetValue(FORWARD_SELECTIVE_FWD_TO);
    stFSInfo.bPlayRingReminder = GetComboboxValue(FORWARD_SELECTIVE_PLAY_RING);

    VecItem & vecItem = stFSInfo.vecCriteria;
    int iIndex = 0;
    for (VecItem::iterator it = vecItem.begin(); it != vecItem.end(); ++it)
    {
        yl::string strItemID = commonAPI_FormatString("%d", iIndex++);
        int nValue = GetComboboxValue(strItemID);
        it->bActive = (1 == nValue);
    }

    //判断Number是否合法
    if (stFSInfo.bActive
            && !BWCallControl_CheckPhoneNumberValid(stFSInfo.strForwardToPhoneNumber.c_str()))
    {
        CSettingUIDataValidInfo validInfo;
        validInfo.SetValidIndex(INDEX_FORWARD_TO);
        validInfo.SetValidNote(_LANG(TRID_INVALID_NUMBER));
        g_SetUICallBackFun.DealWithValidData(validInfo);
        return false;
    }

    if (FS_SetForwardSelective(stFSInfo, m_iLineID))
    {
        m_eMsgboxID = XSI_MSGBOX_ID_SAVE;
        ShowMessageBox(_LANG(TRID_NOTE), _LANG(TRID_SAVING_PLEASE_WAIT), XSI_MSGBOX_ID_SAVE, -1,
                       MESSAGEBOX_Cancel);
    }
    else
    {
        ShowMessageBox(_LANG(TRID_WARNING), _LANG(TRID_SAVE_FAILED), XSI_MSGBOX_ID_FAIL, 1500,
                       MESSAGEBOX_Cancel);
    }

    return true;
}

void CSettinguiCallControllForwardSelectiveDelegate::RefreshtForwardSelectiveData()
{
    SForwardSelectiveData stFSInfo;
    if (NULL == m_pPageData || !FS_GetForwardSelectiveInfo(stFSInfo, m_iLineID))
    {
        return;
    }

    SetComboboxValue(FORWARD_SELECTIVE_SWITCH, stFSInfo.bActive);
    SetValue(FORWARD_SELECTIVE_FWD_TO, stFSInfo.strForwardToPhoneNumber);
    SetComboboxValue(FORWARD_SELECTIVE_PLAY_RING, stFSInfo.bPlayRingReminder);

    VecItem & vecItem = stFSInfo.vecCriteria;
    if (vecItem.size() == 0)
    {
        return;
    }

    int iIndex = 0;
    for (VecItem::iterator it = vecItem.begin(); it != vecItem.end(); ++it)
    {
        yl::string strItemID = commonAPI_FormatString("%d", iIndex++);
        yl::string strDisp = it->strName;

        AddCriteriaItem(strItemID, strDisp, it->bActive);
    }
}

void CSettinguiCallControllForwardSelectiveDelegate::SetRequest(bool bRequest)
{
    m_bRequest = bRequest;
}

void CSettinguiCallControllForwardSelectiveDelegate::OnCancleClick()
{

}

void CSettinguiCallControllForwardSelectiveDelegate::AddCriteriaItem(const yl::string & strItemID,
        const yl::string & strDiasplay, bool bOn)
{
    if (NULL == m_pPageData)
    {
        return;
    }

    CSettingUIComboBoxItemDataPtr pComboboxItem = new CSettingUIComboBoxItemData;
    if (NULL != pComboboxItem)
    {
        pComboboxItem->SetItemID(strItemID);
        pComboboxItem->SetDisplayName(strDiasplay);
        pComboboxItem->SetShowIndex(true);
        yl::string strValue = bOn ? "1" : "0";
        pComboboxItem->SetValue(strValue);

        for (int i = 0; i < 2; ++i)
        {
            CSettingUIComboBoxOption * pOption = new CSettingUIComboBoxOption;

            if (NULL == pOption)
            {
                continue;
            }

            pOption->m_strDisplayName = (0 == i) ? _LANG(TRID_OFF) : _LANG(TRID_ON);
            pOption->m_strItemVale = commonAPI_FormatString("%d", i);

            if (!pComboboxItem->AddOptionItem(pOption))
            {
                delete pOption;
            }
        }

        if (!m_pPageData->AddItem(pComboboxItem))
        {
            delete pComboboxItem;
        }
    }
}

#endif
