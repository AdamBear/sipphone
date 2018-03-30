#include "zerostatusdelegate.h"
#include <settinguilogic/include/settingdefine.h>
#include "settingui/include/itemnamedefine.h"
#include "settingui/include/itemnamedefine.h"
#include "settinguilogic/src/datafactory.h"
#include "settinguilogic/src/dataloader.h"
#include "settinguilogic/include/pagedatafactory.h"
#include "settinguilogic/src/settinguibaseitemdata.h"
#include "settinguilogic/src/settinguibasepagedata.h"


#include "settinguilogic/include/common.h"
#include "settinguilogic/src/statuscontroller.h"
#include "settinguilogic/src/networkcontroller.h"
#include "netmsgtypedefine.h"
#include "common/common_data_define.h"
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"

CZeroStatusDelegate::CZeroStatusDelegate()
    : CSettingUIDelegateBase()
{
}

CZeroStatusDelegate::~CZeroStatusDelegate()
{
}

CSettingUIDelegateBase * CZeroStatusDelegate::CreateZeroStatusDelegate()
{
    CZeroStatusDelegate * pStatusDelegate = new CZeroStatusDelegate;

    return (CSettingUIDelegateBasePtr)pStatusDelegate;
}

CSettingUIPageDataBase * CZeroStatusDelegate::LoadPageData(bool bReLoad /*= false*/)
{
    CSettingUIDelegateBase::LoadPageData(bReLoad);

    if (NULL == m_pPageData)
    {
        return NULL;
    }

    PhoneStatusData phoneData;
    if (g_pStatusController->GetPhoneStatusData(&phoneData))
    {
        SetValue(ZERO_STATUS_MAC, phoneData.m_strMac);
        SetValue(ZERO_STATUS_FIRMWARE, phoneData.m_strFirmWare);
    }

    PhoneNetworkStatusData NetworkData;
    if (g_pStatusController->GetPhoneIPv4NetworkStatus(&NetworkData))
    {
        // IP
        SetValue(ZERO_STATUS_IPV4IP, NetworkData.m_strWanIP);
        // Sub Mask
        SetValue(ZERO_STATUS_IPV4SUBNETMASK, NetworkData.m_strWanMask);
        // Gateway
        SetValue(ZERO_STATUS_IPV4GATEWAY, NetworkData.m_strGateway);
        // DNS Primary Server
        SetValue(ZERO_STATUS_IPV4PRIDNS, NetworkData.m_strDnsPrimary);
        // DNS Second Server
        SetValue(ZERO_STATUS_IPV4SECDNS, NetworkData.m_strDnsSecond);
    }

    if (g_pStatusController->GetPhoneIPv6NetworkStatus(&NetworkData))
    {
        // IP
        SetValue(ZERO_STATUS_IPV6IP, NetworkData.m_strWanIP);
        // Prefix
        SetValue(ZERO_STATUS_IPV6PREFIX, NetworkData.m_strPrefix);
        // Gateway
        SetValue(ZERO_STATUS_IPV6GATEWAY, NetworkData.m_strGateway);
        // DNS Primary Server
        SetValue(ZERO_STATUS_IPV6PRIDNS, NetworkData.m_strDnsPrimary);
        // DNS Second Server
        SetValue(ZERO_STATUS_IPV6SECDNS, NetworkData.m_strDnsSecond);
    }

    return m_pPageData;
}

bool CZeroStatusDelegate::SavePageData(CSettingUIPageDataBase * pPageData,
                                       bool bBackToPrevious /*= true*/, bool bPopHint /*= true*/)
{
    return true;
}

void CZeroStatusDelegate::SetValue(const yl::string & strItemId, const yl::string & strValue)
{
    if (NULL == m_pPageData)
    {
        return;
    }

    CSettingUIItemDataBasePtr pItemInfo = m_pPageData->GetItemByItemId(strItemId);
    if (NULL == pItemInfo)
    {
        return;
    }

    if (strValue.empty())
    {
        pItemInfo->SetValue(_LANG("--"));
    }
    else
    {
        pItemInfo->SetValue(strValue);
    }
}

bool CZeroStatusDelegate::PageDataFilter(CSettingUIPageDataBase* pPagedata, bool bReload /*= false*/)
{
    if (NULL == pPagedata)
    {
        return true;
    }

    bool bResult = CSettingUIDelegateBase::PageDataFilter(pPagedata, bReload);

    int nIPMode = g_pNetworkController->GetWanIPMode();
    if (IPAM_IPV4 == nIPMode)
    {
        SetItemShow(pPagedata, ZERO_STATUS_IPV6IP, false);
        SetItemShow(pPagedata, ZERO_STATUS_IPV6PREFIX, false);
        SetItemShow(pPagedata, ZERO_STATUS_IPV6GATEWAY, false);
        SetItemShow(pPagedata, ZERO_STATUS_IPV6PRIDNS, false);
        SetItemShow(pPagedata, ZERO_STATUS_IPV6SECDNS, false);
    }
    else if (IPAM_IPV6 == nIPMode)
    {
        SetItemShow(pPagedata, ZERO_STATUS_IPV4IP, false);
        SetItemShow(pPagedata, ZERO_STATUS_IPV4SUBNETMASK, false);
        SetItemShow(pPagedata, ZERO_STATUS_IPV4GATEWAY, false);
        SetItemShow(pPagedata, ZERO_STATUS_IPV4PRIDNS, false);
        SetItemShow(pPagedata, ZERO_STATUS_IPV4SECDNS, false);
    }

    return bResult;
}

