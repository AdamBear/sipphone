#include "settinguiwanportdelegate.h"
#include "settinguilogic/src/networkcontroller.h"
#include "include/netmsgtypedefine.h"
#include "settingui/include/itemnamedefine.h"
#include "settinguilogic/src/settinguibaseitemdata.h"
#include "settinguilogic/src/settinguicomboboxitemdata.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "basesettinguidelegate.h"
#include "settinguilogic/src/networksettingmanager.h"
#include "configparser/modconfigparser.h"
#include <configiddefine.h>
#include "uicallbackfun.h"
namespace
{
#define IP_MODE_INDEX 0
#define IP_MODE_PREFERENCE_INDEX 1
#define MODE_PREFERENCE_IPV4 0
#define MODE_PREFERENCE_IPV6 1
#define IP_MODE_IPV4 "0"
#define IP_MODE_IPV6 "1"
#define IP_MODE_BOTH "2"
}

CSettingUIWanPortDelegate::CSettingUIWanPortDelegate()
    : m_nIPModePreference(0)
{
}
CSettingUIWanPortDelegate:: ~CSettingUIWanPortDelegate()
{


}

CSettingUIDelegateBase* CSettingUIWanPortDelegate::CreateWanPortDelegate()
{
    CSettingUIWanPortDelegate* pDelegate = new CSettingUIWanPortDelegate;

    return (CSettingUIDelegateBasePtr)pDelegate;
}

CSettingUIPageDataBase* CSettingUIWanPortDelegate::LoadPageData(bool bReLoad /*= false*/)
{
    CSettingUIPageDataBase* pPageData = CSettingUIDelegateBase::LoadPageData(bReLoad);
    if (NULL == pPageData)
    {
        return NULL;
    }

    CSettingUIItemDataBasePtr pPreferenceItemData = pPageData->GetItemByDataIndex(IP_MODE_PREFERENCE_INDEX);
    if (NULL == pPreferenceItemData)
    {
        return pPageData;
    }

    m_nIPModePreference = configParser_GetConfigInt(kszNetWorkPreference);
    if (MODE_PREFERENCE_IPV4 == m_nIPModePreference)
    {
        pPreferenceItemData->SetValue(IP_MODE_IPV4);
    }
    else if (MODE_PREFERENCE_IPV6 == m_nIPModePreference)
    {
        pPreferenceItemData->SetValue(IP_MODE_IPV6);
    }
    return pPageData;
}

bool CSettingUIWanPortDelegate::PageDataFilter(CSettingUIPageDataBase* pPagedata,
        bool bReload /*= false*/)
{
    if (NULL == pPagedata)
    {
        return false;
    }

    bool bResult = CSettingUIDelegateBase::PageDataFilter(pPagedata, bReload);
    if (!bReload)
    {
        CSettingUIItemDataBasePtr pItemData = pPagedata->GetItemByDataIndex(IP_MODE_INDEX);
        if (NULL == pItemData)
        {
            return false;
        }
        yl::string strIPMode = pItemData->GetValue();
        if (IP_MODE_BOTH != strIPMode)
        {
            SetItemShow(pPagedata, WANPORT_IPMODE_PERFERED);
        }
        return bResult;
    }

    int nIPMode = g_pNetworkController->GetWanIPMode();
    if (IPAM_BOTH != nIPMode)
    {
        SetItemShow(pPagedata, WANPORT_IPMODE_PERFERED);
    }
    return bResult;

}

bool CSettingUIWanPortDelegate::SavePageData(CSettingUIPageDataBase * pPageData,
        bool bBackToPrevious /*= true*/, bool bPopHint /*= true*/)
{
    if (NULL == pPageData)
    {
        return false;
    }

    CSettingUIItemDataBasePtr pPreferenceItemData = pPageData->GetItemByDataIndex(IP_MODE_PREFERENCE_INDEX);
    if (NULL == pPreferenceItemData)
    {
        return false;
    }

    yl::string strIPModePreference = pPreferenceItemData->GetValue();
    int nIPModePreference = atoi(strIPModePreference.c_str());

    if (nIPModePreference != m_nIPModePreference)
    {
        configParser_SetConfigInt(kszNetWorkPreference, nIPModePreference);
        _NetworkChangeFlagManager.SetNetworkChangeFlag(true);
    }

    return CSettingUIDelegateBase::SavePageData(pPageData, bBackToPrevious, bPopHint);
}

void CSettingUIWanPortDelegate::OnComboBoxValueChange(int nIndex)
{
    g_SetUICallBackFun.RefreshUI(false);
}

