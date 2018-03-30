#include "settinguicallcontrollmobilitydelegate.h"

#ifdef IF_FEATURE_BROADSOFT_MOBILITY
#include "uicallbackfun.h"
#include "settinguimanager.h"
#include "settingui/include/itemnamedefine.h"
#include "settingui/include/pageiddefine.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "broadsoft/xsiactionhelper/include/xsicommon.h"
#include "broadsoft/xsiactionhelper/include/cbasehelper.h"
#include "broadsoft/anywhere/include/anywherecontroller.h"
#include "broadsoft/bwcallcontrol/include/modbwcallcontrol.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "messagebox/messageboxbase.h"
#include "messagebox/modmessagebox.h"
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"

namespace
{
#define INDEX_PHONE_NUM 1
}

CSettinguiCallControllMobilityDelegate::CSettinguiCallControllMobilityDelegate()
{
    etl_RegisterMsgHandle(XSI_MESSAGE_BEGIN, XSI_MESSAGE_END,
                          &CSettinguiCallControllMobilityDelegate::OnMessage);
}

CSettinguiCallControllMobilityDelegate::~CSettinguiCallControllMobilityDelegate()
{
    etl_UnregisterMsgHandle(XSI_MESSAGE_BEGIN, XSI_MESSAGE_END,
                            &CSettinguiCallControllMobilityDelegate::OnMessage);
}

CSettingUIDelegateBase *
CSettinguiCallControllMobilityDelegate::CreateCallControllMobilityDelegate()
{
    CSettinguiCallControllMobilityDelegate * pDelegate = new CSettinguiCallControllMobilityDelegate;

    return (CSettingUIDelegateBasePtr)pDelegate;
}

LRESULT CSettinguiCallControllMobilityDelegate::OnMessage(msgObject & msg)
{
    if (NULL == g_pSettingUIManager)
    {
        return FALSE;
    }

    CSettinguiCallControllMobilityDelegate * pDelegate = (CSettinguiCallControllMobilityDelegate *)
            g_pSettingUIManager->GetPageDelegate(kszPageIDCallControllMobility);
    if (NULL == pDelegate || pDelegate !=  g_pSettingUIManager->GetTopDelegate())
    {
        return FALSE;
    }

    if (XSI_MESSAGE_REQ_FINISH == msg.message)
    {
        if (XSI_MOBILITY_GET == msg.wParam)
        {
            MessageBox_RemoveMessageBox(pDelegate, MessageBox_OK, XSI_MSGBOX_ID_LOADING);

            SMobility mobilityInfo;
            MBLT_GetMobilityInfo(mobilityInfo, pDelegate->GetPageLineID());
            pDelegate->RefreshtMobilityData(mobilityInfo);

            g_SetUICallBackFun.RefreshUI(false, pDelegate->GetPageID());
        }

        else if (XSI_MOBILITY_PUT == msg.wParam)
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

CSettingUIPageDataBase * CSettinguiCallControllMobilityDelegate::LoadPageData(
    bool bReLoad /* = false */)
{
    CSettinguiXSIDelegatebase::LoadPageData(bReLoad);

    if (bReLoad && NULL != m_pPageData)
    {
        SMobility mobilityInfo;
        MBLT_GetMobilityInfo(mobilityInfo, m_iLineID);
        RefreshtMobilityData(mobilityInfo);

        MBLT_QueryMobility(m_iLineID);

        ShowMessageBox(_LANG(TRID_NOTE), _LANG(TRID_LOADING_PLEASE_WAIT), XSI_MSGBOX_ID_LOADING, -1,
                       MESSAGEBOX_Cancel);

        m_eMsgboxID = XSI_MSGBOX_ID_LOADING;
    }

    return m_pPageData;
}

bool CSettinguiCallControllMobilityDelegate::SavePageData(CSettingUIPageDataBase * pPageData,
        bool bBackToPrevious /* = true */, bool bPopHint /* = true */)
{
    if (NULL == pPageData || !pPageData->IsEnable())
    {
        return false;
    }

    SMobility mobilityInfo;
    GetMobilityData(mobilityInfo);

    //判断Number是否合法
    // http://10.2.1.199/Bug.php?BugID=97359 不判断开关，只要number不合法就弹提示，不进行保存
    if (/*mobilityInfo.bActive && */!BWCallControl_CheckPhoneNumberValid(
                                        mobilityInfo.strMobilePhoneNumber.c_str()))
    {
        CSettingUIDataValidInfo validInfo;
        validInfo.SetValidIndex(INDEX_PHONE_NUM);
        validInfo.SetValidNote(_LANG(TRID_INVALID_NUMBER));
        g_SetUICallBackFun.DealWithValidData(validInfo);
        return false;
    }

    MBLT_SetMobility(mobilityInfo, m_iLineID);

    ShowMessageBox(_LANG(TRID_NOTE), _LANG(TRID_SAVING_PLEASE_WAIT), XSI_MSGBOX_ID_SAVE, -1,
                   MESSAGEBOX_Cancel);

    m_eMsgboxID = XSI_MSGBOX_ID_SAVE;

    return true;
}

void CSettinguiCallControllMobilityDelegate::GetMobilityData(SMobility & mobilityInfo)
{
    if (NULL == m_pPageData)
    {
        return;
    }

    mobilityInfo.bActive = GetComboboxValue(MOBILITY_ACTIVE);
    mobilityInfo.nPhoneToRing = GetComboboxValue(MOBILITY_PHONETORING);
    mobilityInfo.strMobilePhoneNumber = GetValue(MOBILITY_MOBILEPHONENUMBER);
    mobilityInfo.bAlertClickToDialCalls = GetComboboxValue(MOBILITY_ALERTCLICKTODIALCALLS);
    mobilityInfo.bAlertGroupPagingCalls = GetComboboxValue(MOBILITY_ALERTGROUPPAGINGCALLS);
    mobilityInfo.bEnableDiversionInhibitor = GetComboboxValue(MOBILITY_ENABLEDIVERSIONINHIBITOR);
    mobilityInfo.bRequireAnswerConfirmation = GetComboboxValue(MOBILITY_REQUIREANSWERCONFIRMATION);
    mobilityInfo.bBroadworksCallControl = GetComboboxValue(MOBILITY_BROADWORKSCALLCONTROL);
}

void CSettinguiCallControllMobilityDelegate::RefreshtMobilityData(const SMobility & mobilityInfo)
{
    if (NULL == m_pPageData)
    {
        return;
    }

    SetComboboxValue(MOBILITY_ACTIVE, mobilityInfo.bActive);
    SetComboboxValue(MOBILITY_PHONETORING, mobilityInfo.nPhoneToRing);
    SetValue(MOBILITY_MOBILEPHONENUMBER, mobilityInfo.strMobilePhoneNumber);
    SetComboboxValue(MOBILITY_ALERTCLICKTODIALCALLS, mobilityInfo.bAlertClickToDialCalls);
    SetComboboxValue(MOBILITY_ALERTGROUPPAGINGCALLS, mobilityInfo.bAlertGroupPagingCalls);
    SetComboboxValue(MOBILITY_ENABLEDIVERSIONINHIBITOR, mobilityInfo.bEnableDiversionInhibitor);
    SetComboboxValue(MOBILITY_REQUIREANSWERCONFIRMATION, mobilityInfo.bRequireAnswerConfirmation);
    SetComboboxValue(MOBILITY_BROADWORKSCALLCONTROL, mobilityInfo.bBroadworksCallControl);
}

void CSettinguiCallControllMobilityDelegate::OnCancleClick()
{
    MBLT_CancelRequest();
}

#endif
