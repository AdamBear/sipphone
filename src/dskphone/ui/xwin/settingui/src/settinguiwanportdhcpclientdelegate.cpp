#include "settinguiwanportdhcpclientdelegate.h"

#if !IF_SUPPORT_IP_SET_SINGLE_MENU
#include "uicallbackfun.h"
#include "settingui/include/pageiddefine.h"
#include "settingui/include/itemnamedefine.h"
#include "settinguilogic/include/common.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include <configiddefine.h>
#include "configparser/modconfigparser.h"
#include "settinguilogic/src/networksettingmanager.h"

CSettingUIWanPortDhcpClientDelegate::CSettingUIWanPortDhcpClientDelegate()
{

}

CSettingUIWanPortDhcpClientDelegate::~CSettingUIWanPortDhcpClientDelegate()
{

}

CSettingUIDelegateBase * CSettingUIWanPortDhcpClientDelegate::CreatWanPortDhcpClientDelegate()
{
    CSettingUIWanPortDhcpClientDelegate * pDelegate = new CSettingUIWanPortDhcpClientDelegate;

    return (CSettingUIDelegateBasePtr)pDelegate;
}

bool CSettingUIWanPortDhcpClientDelegate::PageDataFilter(CSettingUIPageDataBase * pPagedata,
        bool bReload /* = false */)
{
    bool bResult = CSettinguiDataDelegateBase::PageDataFilter(pPagedata, bReload);

    if (NULL == pPagedata)
    {
        return bResult;
    }

    if ("1" == GetValue(WANPORT_DHCP_ENABLE))
    {
        SetItemShow(pPagedata, WANPORT_DHCP_PEIDNS, true);
        SetItemShow(pPagedata, WANPORT_DHCP_SECDNS, true);
    }
    else
    {
        SetItemShow(pPagedata, WANPORT_DHCP_PEIDNS, false);
        SetItemShow(pPagedata, WANPORT_DHCP_SECDNS, false);
    }

    return bResult;
}

void CSettingUIWanPortDhcpClientDelegate::SaveExtraData()
{
    yl::string strPageId = GetPageID();

    if (kszPageIDWanPortIpv4DhcpClient == strPageId)
    {
        if (WAN_DHCP != configParser_GetConfigInt(kszNetworkWanType))
        {
            _NetworkChangeFlagManager.SetNetworkChangeFlag(true);
            configParser_SetConfigInt(kszNetworkWanType, WAN_DHCP, CONFIG_LEVEL_PHONE);
        }
    }
    else if (kszPageIDWanPortIpv6DhcpClient == strPageId)
    {
        if (IPv6_WAN_DHCP != configParser_GetConfigInt(kszNetworkIPv6WANType))
        {
            _NetworkChangeFlagManager.SetNetworkChangeFlag(true);
            configParser_SetConfigInt(kszNetworkIPv6WANType, IPv6_WAN_DHCP, CONFIG_LEVEL_PHONE);
        }
    }
}

void CSettingUIWanPortDhcpClientDelegate::OnComboBoxValueChange(int nIndex)
{
    g_SetUICallBackFun.RefreshUI(false, GetPageID());
}
#endif
