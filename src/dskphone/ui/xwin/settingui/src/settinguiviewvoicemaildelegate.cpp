#include "settinguiviewvoicemaildelegate.h"
#include "uicallbackfun.h"
#include "settinguimanager.h"
#include "settinguimenudetailitemdata.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "account/include/modaccount.h"
#include "devicelib/phonedevice.h"
#include "setuidatadefine.h"
#include "settingui/include/pageiddefine.h"
#include "settinguilogic/include/modsetuilogic.h"
#include "broadsoft/voicemail/include/modvirtualvoicemail.h"
#include "broadsoft/bwcallcontrol/include/modbwcallcontrol.h"
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"

CSettinguiViewVoiceMailDelegate::CSettinguiViewVoiceMailDelegate()
{
    etl_RegisterMsgHandle(ACCOUNT_STATUS_CHANGED, ACCOUNT_STATUS_CHANGED
                          , &CSettinguiViewVoiceMailDelegate::OnMessage);
}

CSettinguiViewVoiceMailDelegate::~CSettinguiViewVoiceMailDelegate()
{
    etl_UnregisterMsgHandle(ACCOUNT_STATUS_CHANGED, ACCOUNT_STATUS_CHANGED
                            , &CSettinguiViewVoiceMailDelegate::OnMessage);
}

CSettingUIDelegateBase * CSettinguiViewVoiceMailDelegate::CreateViewVoiceMailDelegate()
{
    CSettinguiViewVoiceMailDelegate * pDelegate = new CSettinguiViewVoiceMailDelegate;

    return (CSettingUIDelegateBasePtr)pDelegate;
}

LRESULT CSettinguiViewVoiceMailDelegate::OnMessage(msgObject & msg)
{
    if (ACCOUNT_STATUS_CHANGED == msg.message)
    {
        CSettingUIDelegateBase * pDelegate = g_pSettingUIManager->GetPageDelegate(kszPageIDViewVoiceMail);
        if (NULL != pDelegate)
        {
            if (pDelegate == g_pSettingUIManager->GetTopDelegate())
            {
                g_SetUICallBackFun.RefreshUI(true, kszPageIDViewVoiceMail);
            }
            else
            {
                pDelegate->SetRefreshDataWhenTopDelegate(true);
            }

            return TRUE;
        }
    }

    return FALSE;
}

CSettingUIPageDataBase * CSettinguiViewVoiceMailDelegate::LoadPageData(bool bReLoad /* = false */)
{
    CSettingUIDelegateBase::LoadPageData(bReLoad);

    if (NULL != m_pPageData && bReLoad)
    {
        LoadMailAmountItem(m_pPageData);
    }

    return m_pPageData;
}

bool CSettinguiViewVoiceMailDelegate::OnSoftkeyAction(int iType, bool bLongPress, int nDataIndex)
{
    if (ST_CONNECT == iType)
    {
        OnConnectPress(nDataIndex);
        return true;
    }

    return CSettingUIDelegateBase::OnSoftkeyAction(iType, bLongPress, nDataIndex);
}

bool CSettinguiViewVoiceMailDelegate::OnAction(int eActionType, int iDataIndex)
{
    if (SET_ACTION_TYPE_CLICK == eActionType)
    {
        bool bEnter = false;
#if IF_FEATURE_BROADSOFT_VOICEMAIL
        if (vvMail_IsEnable())
        {
            bEnter = true;
        }
#endif
        if (bEnter)
        {
            OnEnterPress(iDataIndex);
        }
        else
        {
            OnConnectPress(iDataIndex);
        }

        return true;
    }

    return CSettingUIDelegateBase::OnAction(eActionType, iDataIndex);
}

int CSettinguiViewVoiceMailDelegate::GetRealIndex(int nIndex)
{
    CSettingUIItemDataBase * pItemData = GetItemDataByDataIndex(nIndex);
    if (NULL != pItemData)
    {
        return atoi(pItemData->GetItemID().c_str());
    }

    return -1;
}

void CSettinguiViewVoiceMailDelegate::LoadMailAmountItem(CSettingUIPageDataBase * pPageData)
{
    if (NULL == pPageData)
    {
        return;
    }

    int nTotalAccount = acc_AccountNum();
    for (int i = 0; i < nTotalAccount; ++i)
    {
        if (LS_REGISTERED != acc_GetState(i))
        {
            continue;
        }

        int iNewMsgNum = 0;
        int iOldMsgNum = 0;
        iOldMsgNum = acc_GetReadedVoiceMailNum(i);
        iNewMsgNum = acc_GetUnReadVoiceMailNum(i);

        yl::string strValue = commonAPI_FormatString("%d %s %d %s %s",
                              iNewMsgNum, _LANG(TRID_NEW).c_str(), iOldMsgNum, _LANG(TRID_OLD).c_str(), _LANG(TRID_MAIL).c_str());

        yl::string strName = acc_GetAccountShowText(i);
        if (strName.empty())
        {
            strName = _LANG(TRID_ACCOUNT_EMPTY);
        }

        CSettingUIDetailMenuItemDataPtr pItemData = new CSettingUIDetailMenuItemData;
        if (NULL == pItemData)
        {
            return;
        }
        pItemData->SetDisplayName(strName);
        pItemData->SetValue(strValue);
        pItemData->SetShowIndex(true);
        pItemData->SetShowColon(false);
        yl::string strID = commonAPI_FormatString("%d", i);
        pItemData->SetItemID(strID);
        if (!pPageData->AddItem(pItemData))
        {
            delete pItemData;
        }
    }
}

void CSettinguiViewVoiceMailDelegate::GetPagePrivateSoftkey(CArraySoftKeyBarData & vecSoftKey)
{
    if (NULL != m_pPageData && m_pPageData->GetTotalItemNumber() > 0)
    {
        bool bEnter = false;
#if IF_FEATURE_BROADSOFT_VOICEMAIL
        if (vvMail_IsEnable())
        {
            bEnter = true;
        }
#endif

        if (bEnter)
        {
            vecSoftKey[2].m_iType = ST_CONNECT;
            vecSoftKey[2].m_strSoftkeyTitle = _LANG(TRID_CONNECT);
            vecSoftKey[3].m_iType = ST_ENTER;
            vecSoftKey[3].m_strSoftkeyTitle = _LANG(TRID_ENTER);
        }
        else
        {
            vecSoftKey[2].m_iType = ST_EMPTY;
            vecSoftKey[2].m_strSoftkeyTitle = "";
            vecSoftKey[3].m_iType = ST_CONNECT;
            vecSoftKey[3].m_strSoftkeyTitle = _LANG(TRID_CONNECT);
        }
    }
}

void CSettinguiViewVoiceMailDelegate::OnConnectPress(int nDataIndex)
{
    int nRealIndex = GetRealIndex(nDataIndex);
    SettingUILogic_ConnectVoiceMail(nRealIndex);
}

void CSettinguiViewVoiceMailDelegate::OnEnterPress(int nDataIndex)
{
#if IF_FEATURE_BROADSOFT_VOICEMAIL
    // 如果账号对应的XSI不可用返回
    int nRealID = GetRealIndex(nDataIndex);
    if (!BWCallControl_IsAccountXSIEnable(nRealID))
    {
        g_SetUICallBackFun.PopCommonMessageBox(m_strPageID, _LANG(TRID_WARNING),
                                               _LANG(TRID_XSI_INFORMATION_IS_INCOMPLETE)
                                               , SETTING_MESSAGEBOX_NOTE, "", "", 1500);
        return;
    }

    PageOpenData opPageData(kszPageIDVoiceMailList);
    opPageData.pExtraData = (void *)&nRealID;
    g_SetUICallBackFun.EnterPage(opPageData);
#endif
}

