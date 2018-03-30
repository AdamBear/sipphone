#include "settinguicallforwarddelegate.h"
#include <settinguilogic/include/settingdefine.h>
#include "uicallbackfun.h"
#include "settinguilogic/src/datafactory.h"
#include "settinguilogic/src/dataloader.h"
#include "settinguilogic/include/pagedatafactory.h"
#include "settinguilogic/src/settinguibaseitemdata.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "settingui/include/itemnamedefine.h"
#include "settingui/include/pageiddefine.h"
#include "settinguilogic/include/common.h"
#include "settinguilogic/src/callforwardcontroller.h"
#include "common/common_data_define.h"
#include "configparser/modconfigparser.h"
#include <configiddefine.h>
#include "netmsgtypedefine.h"
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"
#include "settinguimenudetailitemdata.h"

namespace
{
#define SOFTKEY_COUNT 4
const char* const kszOpenPage = "openpage(%s)";
}

CSettingUICallForwardDelegate::CSettingUICallForwardDelegate()
{
}

CSettingUICallForwardDelegate::~CSettingUICallForwardDelegate()
{
}

CSettingUIDelegateBase* CSettingUICallForwardDelegate::CreateCallForwardDelegate()
{
    CSettingUICallForwardDelegate* pCallForward = new CSettingUICallForwardDelegate;

    return (CSettingUIDelegateBasePtr)pCallForward;
}

void CSettingUICallForwardDelegate::OnEnterPage(const yl::string& strPage, int iDataIndex)
{
    PageOpenData opPageData(strPage);
    opPageData.pExtraData = (void*)(&m_iLineID);
    g_SetUICallBackFun.EnterPage(opPageData);
}

CSettingUIPageDataBase* CSettingUICallForwardDelegate::LoadPageData(bool bReLoad /*= false*/)
{
    CSettingUIAccountDelegateBase::LoadPageData(bReLoad);

    if (bReLoad)
    {
        LoadCallForwardData(m_pPageData);
    }

    if (1 == configParser_GetConfigInt(kszForwardMode))
    {
        LoadPrivateData(m_pPageData);
    }

    return m_pPageData;
}

bool CSettingUICallForwardDelegate::SavePageData(CSettingUIPageDataBase* pPageData,
        bool bBackToPrevious /*= true*/, bool bPopHint /*= true*/)
{
    return true;
}

bool CSettingUICallForwardDelegate::OnAction(int eActionType, int iDataIndex)
{
    return CSettingUIDelegateBase::OnAction(eActionType, iDataIndex);
}

void CSettingUICallForwardDelegate::GetPagePrivateSoftkey(CArraySoftKeyBarData& vecSoftKey)
{
    int nSize = vecSoftKey.size();
    if (nSize > 0 && SOFTKEY_COUNT == nSize)
    {
        int nEnter = nSize - 1;
        vecSoftKey[nEnter].m_iType = ST_ENTER;
        vecSoftKey[nEnter].m_strSoftkeyTitle = _LANG(TRID_ENTER);
    }
}

void CSettingUICallForwardDelegate::LoadCallForwardData(CSettingUIPageDataBase* pPageData)
{
    if (NULL == pPageData)
    {
        return;
    }

    yl::string strAlwaysPageAction = commonAPI_FormatString(kszOpenPage, kszPageIDAlwaysForward);
    yl::string strBusyPageAction = commonAPI_FormatString(kszOpenPage, kszPageIDBusyForward);
    yl::string strNoAnswerPageAction = commonAPI_FormatString(kszOpenPage, kszPageIDNoAnswerForward);

    if (1 == configParser_GetConfigInt(kszForwardMode))
    {
        AddCallForwardStatusItem(pPageData, _LANG(TRID_ALWAYS_FORWARD), "", ALWAYS_FORWARD,
                                 true, true, strAlwaysPageAction);
        AddCallForwardStatusItem(pPageData, _LANG(TRID_BUSY_FORWARD), "", BUSY_FORWARD,
                                 true, true, strBusyPageAction);
        AddCallForwardStatusItem(pPageData, _LANG(TRID_NO_ANSWER_FORWARD), "", NO_ANSWER_FORWARD,
                                 true, true, strNoAnswerPageAction);
    }
    else
    {
        AddMenuItem(pPageData, "", _LANG(TRID_ALWAYS_FORWARD), kszPageIDAlwaysForward,
                    true, m_iLineID, "", strAlwaysPageAction);
        AddMenuItem(pPageData, "", _LANG(TRID_BUSY_FORWARD), kszPageIDBusyForward,
                    true, m_iLineID, "", strBusyPageAction);
        AddMenuItem(pPageData, "", _LANG(TRID_NO_ANSWER_FORWARD), kszPageIDNoAnswerForward,
                    true, m_iLineID, "", strNoAnswerPageAction);
    }
}

void CSettingUICallForwardDelegate::LoadPrivateData(CSettingUIPageDataBase* pPageData)
{
    SetValue(ALWAYS_FORWARD, IsForwardEnable(FWD_TYPE_ALWAYS));

    SetValue(BUSY_FORWARD, IsForwardEnable(FWD_TYPE_BUSY));

    SetValue(NO_ANSWER_FORWARD, IsForwardEnable(FWD_TYPE_NO_ANSWER));
}

bool CSettingUICallForwardDelegate::IsForwardEnable(int nType)
{
    ForwardData forwardData;
    if (g_pCallForwardController->GetForwardDataByType(&forwardData, (FWD_TYPE)nType, m_iLineID)
            && forwardData.m_isOn)
    {
        return true;
    }

    return false;
}

void CSettingUICallForwardDelegate::SetValue(const yl::string& strItemId, bool bOn)
{
    if (NULL == m_pPageData)
    {
        return ;
    }

    CSettingUIItemDataBasePtr pItemInfo = m_pPageData->GetItemByItemId(strItemId);
    if (NULL == pItemInfo)
    {
        return;
    }

    if (bOn)
    {
        pItemInfo->SetValue(_LANG(TRID_ON));
    }
    else
    {
        pItemInfo->SetValue(_LANG(TRID_OFF));
    }
}

bool CSettingUICallForwardDelegate::AddCallForwardStatusItem(CSettingUIPageDataBase* pPageData, const yl::string& strDisplayName,
        const yl::string& strValue, const yl::string& strID /*= ""*/,
        bool bShowIndex /*= true*/, bool bShowColon /*= true*/,
        const yl::string& strClickAction /*= ""*/)
{
    if (NULL == pPageData)
    {
        return false;
    }

    CSettingUIDetailMenuItemDataPtr pItemData = new CSettingUIDetailMenuItemData;
    if (NULL == pItemData)
    {
        return false;
    }

    pItemData->SetDisplayName(strDisplayName);
    pItemData->SetShowIndex(bShowIndex);
    pItemData->SetValue(strValue);
    pItemData->SetItemID(strID);
    pItemData->SetClickAction(strClickAction);
    if (!m_pPageData->AddItem(pItemData))
    {
        delete pItemData;
        pItemData = NULL;
        return false;
    }
    return true;
}
