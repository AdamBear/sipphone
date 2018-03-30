#include "settinguicallforwardlineselectdelegate.h"
#include "uicallbackfun.h"
#include "setuidatadefine.h"
#include "settinguimenudetailitemdata.h"
#include "settingui/include/pageiddefine.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "account/include/modaccount.h"
#include <configiddefine.h>
#include "configparser/modconfigparser.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"

namespace
{
const yl::string kszEmpty = "Empty";
}

CSettinguiCallForwardLineSelectDelegate::CSettinguiCallForwardLineSelectDelegate()
{

}

CSettinguiCallForwardLineSelectDelegate::~CSettinguiCallForwardLineSelectDelegate()
{

}

CSettingUIDelegateBase *
CSettinguiCallForwardLineSelectDelegate::CreateCallForwardLineSelectDelegate()
{
    CSettinguiCallForwardLineSelectDelegate * pDelegate = new CSettinguiCallForwardLineSelectDelegate;

    return (CSettingUIDelegateBasePtr)pDelegate;
}

bool CSettinguiCallForwardLineSelectDelegate::OnAction(int eActionType, int iDataIndex)
{
    if (SET_ACTION_TYPE_CLICK == eActionType)
    {
        CSettingUIItemDataBasePtr pItemData = m_pPageData->GetItemByDataIndex(iDataIndex);
        if (NULL != pItemData)
        {
            int nExtraData = atoi(pItemData->GetItemID().c_str());
            PageOpenData opPageData(kszPageIDCallForward);
            opPageData.pExtraData = (void *)&nExtraData;
            g_SetUICallBackFun.EnterPage(opPageData);

            SetRefreshDataWhenTopDelegate(true);
            return true;
        }
    }

    return CSettingUIDelegateBase::OnAction(eActionType, iDataIndex);
}

CSettingUIPageDataBase * CSettinguiCallForwardLineSelectDelegate::LoadPageData(
    bool bReLoad /* = false */)
{
    CSettingUIDelegateBase::LoadPageData(bReLoad);

    if (!bReLoad || NULL == m_pPageData)
    {
        return m_pPageData;
    }

    AddAccountListItem(m_pPageData);

    int nItemCount = m_pPageData->GetTotalItemNumber();
    if (nItemCount <= 0)
    {
        AddBaseItemData(m_pPageData, _LANG(TRID_NONE), "", kszEmpty, true, false);
    }

    return m_pPageData;
}

void CSettinguiCallForwardLineSelectDelegate::GetPagePrivateSoftkey(CArraySoftKeyBarData &
        vecSoftKey)
{
    if (SOFTKEY_NUM != vecSoftKey.size())
    {
        return;
    }

    if (NULL == m_pPageData || IsItemShow(kszEmpty))
    {
        vecSoftKey[3].m_iType = ST_EMPTY;
        vecSoftKey[3].m_strSoftkeyTitle = "";
    }
    else
    {
        vecSoftKey[3].m_iType = ST_ENTER;
        vecSoftKey[3].m_strSoftkeyTitle = _LANG(TRID_ENTER);
    }
}

bool CSettinguiCallForwardLineSelectDelegate::IsForwardOn(int nAccountID)
{
    if (1 == configParser_GetCfgItemIntValue(kszAccountAlwaysFWDStatus, nAccountID)
            || 1 == configParser_GetCfgItemIntValue(kszAccountBusyFWDStatus, nAccountID)
            || 1 == configParser_GetCfgItemIntValue(kszAccountNoASWFWDStatus, nAccountID))
    {
        return true;
    }

    return false;
}

void CSettinguiCallForwardLineSelectDelegate::AddAccountListItem(CSettingUIPageDataBase * pPageData)
{
    if (NULL == pPageData)
    {
        return;
    }

    int nAccountCount = acc_AccountNum();
    for (int i = 0; i < nAccountCount; ++i)
    {
        if (!acc_IsAccountAvailable(i))
        {
            continue;
        }

        yl::string strShowText = acc_GetAccountShowText(i);
        CSettingUIDetailMenuItemDataPtr pItem = new CSettingUIDetailMenuItemData;
        if (NULL != pItem)
        {
            yl::string strShowText = acc_GetAccountShowText(i);
            pItem->SetShowIndex(true);
            pItem->SetShowColon(false);
            pItem->SetDisplayName(strShowText);
            yl::string strID = commonAPI_FormatString("%d", i);
            pItem->SetItemID(strID);

            bool bOn = IsForwardOn(i);
            yl::string strValue = bOn ? _LANG(TRID_ON) : _LANG(TRID_OFF);
            pItem->SetValue(strValue);

            if (!m_pPageData->AddItem(pItem))
            {
                delete pItem;
            }
        }
    }
}
