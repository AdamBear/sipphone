#include "settinguistatusnetipv6delegate.h"
#include <settinguilogic/include/settingdefine.h>
#include "settingui/include/itemnamedefine.h"
#include "settinguilogic/src/datafactory.h"
#include "settinguilogic/src/dataloader.h"
#include "settinguilogic/include/pagedatafactory.h"
#include "settinguilogic/src/settinguibaseitemdata.h"
#include "settinguilogic/src/settinguibasepagedata.h"

#include "settinguilogic/include/common.h"
#include "settinguilogic/src/statuscontroller.h"
#include "common/common_data_define.h"
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"

CSettingUIStatusNetIPv6Delegate::CSettingUIStatusNetIPv6Delegate()
    : CSettingUIDelegateBase()
{
}

CSettingUIStatusNetIPv6Delegate::~CSettingUIStatusNetIPv6Delegate()
{
}

CSettingUIDelegateBase * CSettingUIStatusNetIPv6Delegate::CreateStatusNetIPv6Delegate()
{
    CSettingUIStatusNetIPv6Delegate * pStatusDelegate = new CSettingUIStatusNetIPv6Delegate;

    return (CSettingUIDelegateBasePtr)pStatusDelegate;
}

CSettingUIPageDataBase * CSettingUIStatusNetIPv6Delegate::LoadPageData(bool bReLoad /*= false*/)
{
    CSettingUIDelegateBase::LoadPageData(bReLoad);
    if (NULL == m_pPageData || !bReLoad)
    {
        return m_pPageData;
    }

    PhoneNetworkStatusData NetworkData;
    if (g_pStatusController->GetPhoneIPv6NetworkStatus(&NetworkData))
    {
        yl::string strValue;
        // LAN Type
        switch (NetworkData.m_iWanType)
        {
        case WAN_DHCP:
            strValue = _LANG(TRID_DHCP);
            break;
        case WAN_PPPOE:
            strValue = _LANG(TRID_PPPOE);
            break;
        case WAN_STATIC:
            strValue = _LANG(TRID_STATIC_IP);
            break;
        default:
            strValue = "";
        }
        SetValue(STATUS_NETWORK_IPV6_WANTYPE, strValue);
        // IP
        SetValue(STATUS_NETWORK_IPV6_WANIP, NetworkData.m_strWanIP);
        // Gateway
        SetValue(STATUS_NETWORK_IPV6_GETWAY, NetworkData.m_strGateway);
        // DNS Primary Server
        SetValue(STATUS_NETWORK_IPV6_PRIDNS, NetworkData.m_strDnsPrimary);
        // DNS Second Server
        SetValue(STATUS_NETWORK_IPV6_SECDNS, NetworkData.m_strDnsSecond);
    }

    return m_pPageData;
}

bool CSettingUIStatusNetIPv6Delegate::SavePageData(CSettingUIPageDataBase * pPageData,
        bool bBackToPrevious /*= true*/, bool bPopHint /*= true*/)
{
    return true;
}

void CSettingUIStatusNetIPv6Delegate::SetValue(const yl::string & strItemId,
        const yl::string & strValue)
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

