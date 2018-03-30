#include "settinguistatusdelegate.h"
#include <settinguilogic/include/settingdefine.h>
#include "settinguilogic/src/datafactory.h"
#include "settinguilogic/src/dataloader.h"
#include "settinguilogic/include/pagedatafactory.h"
#include "settinguilogic/src/settinguibaseitemdata.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "settingui/include/itemnamedefine.h"
#include "settinguilogic/include/common.h"
#include "settinguilogic/src/statuscontroller.h"
#include "settinguilogic/src/networkcontroller.h"
#include "common/common_data_define.h"
#include "configparser/modconfigparser.h"
#include <configiddefine.h>
#include "netmsgtypedefine.h"
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"

CSettingUIStatusDelegate::CSettingUIStatusDelegate()
    : CSettingUIDelegateBase()
{
}

CSettingUIStatusDelegate::~CSettingUIStatusDelegate()
{
}

CSettingUIDelegateBase * CSettingUIStatusDelegate::CreateStatusDelegate()
{
    CSettingUIStatusDelegate * pStatusDelegate = new CSettingUIStatusDelegate;

    return (CSettingUIDelegateBasePtr)pStatusDelegate;
}

CSettingUIPageDataBase * CSettingUIStatusDelegate::LoadPageData(bool bReLoad /*= false*/)
{
    CSettingUIDelegateBase::LoadPageData(bReLoad);

    if (NULL == m_pPageData || !bReLoad)
    {
        return NULL;
    }

    PhoneStatusData phoneData;
    if (g_pStatusController->GetPhoneStatusData(&phoneData))
    {
        SetValue(STATUS_MAC, phoneData.m_strMac);
        SetValue(STATUS_FIRMWARE, phoneData.m_strFirmWare);
    }
#ifdef IF_SUPPORT_WIFI
        //WifiMac
    yl::string strWifiMac = phoneData.m_strWifiMac;
    strWifiMac = strWifiMac.empty() ? _LANG(TRID_NA) : strWifiMac;
    SetValue(STATUS_WiFi_MAC, strWifiMac);
#endif

    PhoneNetworkStatusData networkData;

    //IPV4
    if (g_pStatusController->GetPhoneIPv4NetworkStatus(&networkData))
    {
        SetValue(STATUS_IPV4, networkData.m_strWanIP);
    }
    //IPV6
    networkData.Reset();

    if (g_pStatusController->GetPhoneIPv6NetworkStatus(&networkData))
    {
        SetValue(STATUS_IPV6, networkData.m_strWanIP);
    }

    return m_pPageData;
}

bool CSettingUIStatusDelegate::SavePageData(CSettingUIPageDataBase * pPageData,
        bool bBackToPrevious /*= true*/, bool bPopHint /*= true*/)
{
    return true;
}

bool CSettingUIStatusDelegate::PageDataFilter(CSettingUIPageDataBase * pPagedata,
        bool bReload /*= false*/)
{
    if (NULL == pPagedata)
    {
        return true;
    }

    bool bResult = CSettingUIDelegateBase::PageDataFilter(pPagedata, bReload);

    int nIPMode = g_pNetworkController->GetWanIPMode();
    if (IPAM_IPV4 == nIPMode)
    {
        SetItemShow(pPagedata, STATUS_IPV6);
    }
    else if (IPAM_IPV6 == nIPMode)
    {
        SetItemShow(pPagedata, STATUS_IPV4);
    }
    bool bWifiShow = false;
#ifdef IF_SUPPORT_WIFI
        bWifiShow = true;
#endif
    if (!bWifiShow)
    {
        SetItemShow(pPagedata, STATUS_WiFi_MAC);
    }

    return bResult;
}

void CSettingUIStatusDelegate::SetValue(const yl::string & strItemId, const yl::string & strValue)
{
    if (NULL != m_pPageData)
    {
        CSettingUIItemDataBasePtr pItemInfo = m_pPageData->GetItemByItemId(strItemId);
        if (NULL != pItemInfo)
        {
            pItemInfo->SetValue(strValue);
        }
    }
}

void CSettingUIStatusDelegate::SetItemShow(CSettingUIPageDataBase * pPagedata,
        const yl::string & strItemId)
{
    if (NULL != pPagedata)
    {
        CSettingUIItemDataBasePtr pItemData = pPagedata->GetItemByItemId(strItemId);
        if (NULL != pItemData)
        {
            pItemData->SetShow(false);
        }
    }
}
