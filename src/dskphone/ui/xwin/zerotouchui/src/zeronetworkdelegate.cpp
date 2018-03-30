#include "zeronetworkdelegate.h"
#include <settinguilogic/include/settingdefine.h>
#include "settingui/include/itemnamedefine.h"
#include "common/common_data_define.h"
#include "translateiddefine.h"
#include "settinguilogic/include/common.h"
#include "settinguilogic/include/zero_common.h"
#include "netmsgtypedefine.h"

#include "settinguilogic/src/datafactory.h"
#include "settinguilogic/src/dataloader.h"
#include "settinguilogic/include/pagedatafactory.h"
#include "settinguilogic/src/settinguibaseitemdata.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "settinguilogic/src/networksettingmanager.h"
#include "settinguilogic/src/statuscontroller.h"
#include "settinguilogic/src/networkcontroller.h"
#include "languagehelper/modlangtranslate.h"
#include "settingui/src/uicallbackfun.h"
#include "settinguilogic/include/modsetuilogic.h"
#include "zerotouchui/include/modzerotouchui.h"

namespace
{
#define   feature_enable        1
#define   feature_disable       0

#define   SOFT_KEY_BACK_INDEX   0
#define   SOFT_KEY_NEXT_INDEX   3

#define DHCP_OPTION_MAX_NUMBER 255
#define DHCP_OPTION_MIN_NUMBER 1
#define DHCP_OPTION_MAX_LENGTH 4
}

CZeroNetworkDelegate::CZeroNetworkDelegate()
    : CSettinguiDataDelegateBase()
{
}

CZeroNetworkDelegate::~CZeroNetworkDelegate()
{
}

CSettingUIDelegateBase * CZeroNetworkDelegate::CreateZeroNetworkDelegate()
{
    CZeroNetworkDelegate * pStatusDelegate = new CZeroNetworkDelegate;

    return (CSettingUIDelegateBasePtr)pStatusDelegate;
}

bool CZeroNetworkDelegate::SavePageData(CSettingUIPageDataBase * pPageData,
                                        bool bBackToPrevious /*= true*/, bool bPopHint /*= true*/)
{
    if (NULL == pPageData || !pPageData->IsEnable())
    {
        return false;
    }

    bool bValueChange = IsPageValueChange(pPageData);
    bool bRet = CSettingUIDelegateBase::SavePageData(pPageData, false, false);
    if (bRet && bValueChange)
    {
        //设置网络改变标记
        _NetworkChangeFlagManager.SetNetworkChangeFlag(true);
    }
    return bRet;
}

void CZeroNetworkDelegate::SetValue(const yl::string & strItemId, const yl::string & strValue)
{
    if (NULL != m_pPageData)
    {
        CSettingUIItemDataBasePtr pItemInfo = m_pPageData->GetItemByItemId(strItemId);
        if (NULL != pItemInfo)
        {
            pItemInfo->SetValue(strValue.empty() ? _LANG("--") : strValue);
        }
    }
}

bool CZeroNetworkDelegate::PageDataFilter(CSettingUIPageDataBase * pPagedata,
        bool bReload /*= false*/)
{
    if (NULL == pPagedata)
    {
        return true;
    }

    bool bResult = CSettingUIDelegateBase::PageDataFilter(pPagedata, bReload);

    FilterIpModeData(pPagedata);
    FilterVLanData(pPagedata);
    Filter8021Data(pPagedata);
    FilterVLanDhcp(pPagedata);
    FilterLLDPData(pPagedata);
    FilterCdpData(pPagedata);
    FilterStunData(pPagedata);

    return bResult;
}

void CZeroNetworkDelegate::FilterIpModeData(CSettingUIPageDataBase * pPagedata)
{
    if (NULL == pPagedata)
    {
        return;
    }

    int nIPMode = GetComboboxValue(ZERO_IPMODE);

    if (IPAM_IPV4 == nIPMode)
    {
        SetItemShow(pPagedata, ZERO_IPV6WANTYPE, false);
        HideIpV6StaticIPItem(pPagedata);
        SetItemShow(pPagedata, ZERO_IPV6STATICDNS, false);
        HideIpV6StaticDnsItem(pPagedata);
    }
    else if (IPAM_IPV6 == nIPMode)
    {
        SetItemShow(pPagedata, ZERO_IPV4WANTYPE, false);
        HideIpV4StaticIPItem(pPagedata);
        SetItemShow(pPagedata, ZERO_IPV4STATICDNS, false);
        HideIpV4StaticDnsItem(pPagedata);
    }

    if (IPAM_BOTH == nIPMode)
    {
        FilterIpV4Data(pPagedata);
        FilterIpV6Data(pPagedata);
    }
    else if (IPAM_IPV4 == nIPMode)
    {
        FilterIpV4Data(pPagedata);
    }
    else if (IPAM_IPV6 == nIPMode)
    {
        FilterIpV6Data(pPagedata);
    }
}

void CZeroNetworkDelegate::FilterIpV4Data(CSettingUIPageDataBase * pPagedata)
{
    if (NULL == pPagedata)
    {
        return;
    }

    int nWanType = GetComboboxValue(ZERO_IPV4WANTYPE);

    if (WAN_DHCP == nWanType)
    {
        HideIpV4StaticIPItem(pPagedata);
        int nStaticDns = GetComboboxValue(ZERO_IPV4STATICDNS);
        if (feature_enable == nStaticDns)
        {
            return;
        }
        HideIpV4StaticDnsItem(pPagedata);
    }
    else if (WAN_STATIC == nWanType)
    {
        SetItemShow(pPagedata, ZERO_IPV4STATICDNS, false);
        HideIpV4StaticDnsItem(pPagedata);
    }
}

void CZeroNetworkDelegate::FilterIpV6Data(CSettingUIPageDataBase * pPagedata)
{
    if (NULL == pPagedata)
    {
        return;
    }

    int nWanType = GetComboboxValue(ZERO_IPV6WANTYPE);

    if (IPv6_WAN_DHCP == nWanType)
    {
        HideIpV6StaticIPItem(pPagedata);
        int nStaticDns = GetComboboxValue(ZERO_IPV6STATICDNS);
        if (feature_enable == nStaticDns)
        {
            return;
        }
        HideIpV6StaticDnsItem(pPagedata);
    }
    else if (IPv6_WAN_STATIC == nWanType)
    {
        SetItemShow(pPagedata, ZERO_IPV6STATICDNS, false);
        HideIpV6StaticDnsItem(pPagedata);
    }
}

void CZeroNetworkDelegate::HideIpV4StaticIPItem(CSettingUIPageDataBase * pPagedata)
{
    SetItemShow(pPagedata, ZERO_IPV4IP, false);
    SetItemShow(pPagedata, ZERO_IPV4SUBNETMASK, false);
    SetItemShow(pPagedata, ZERO_IPV4GATEWAY, false);
    SetItemShow(pPagedata, ZERO_IPV4PRIDNS, false);
    SetItemShow(pPagedata, ZERO_IPV4SECDNS, false);
}

void CZeroNetworkDelegate::HideIpV6StaticIPItem(CSettingUIPageDataBase * pPagedata)
{
    SetItemShow(pPagedata, ZERO_IPV6IP, false);
    SetItemShow(pPagedata, ZERO_IPV6PREFIX, false);
    SetItemShow(pPagedata, ZERO_IPV6GATEWAY, false);
    SetItemShow(pPagedata, ZERO_IPV6PRIDNS, false);
    SetItemShow(pPagedata, ZERO_IPV6SECDNS, false);
}

void CZeroNetworkDelegate::HideIpV4StaticDnsItem(CSettingUIPageDataBase * pPagedata)
{
    SetItemShow(pPagedata, ZERO_IPV4STATICDNS_PRI, false);
    SetItemShow(pPagedata, ZERO_IPV4STATICDNS_SEC, false);
}

void CZeroNetworkDelegate::HideIpV6StaticDnsItem(CSettingUIPageDataBase * pPagedata)
{
    SetItemShow(pPagedata, ZERO_IPV6STATICDNS_PRI, false);
    SetItemShow(pPagedata, ZERO_IPV6STATICDNS_SEC, false);
}

void CZeroNetworkDelegate::FilterVLanData(CSettingUIPageDataBase * pPagedata)
{
    if (NULL == pPagedata)
    {
        return;
    }

    int nVlanActive = GetComboboxValue(ZERO_VLANACTIVE);
    if (feature_enable == nVlanActive)
    {
        return;
    }
    SetItemShow(pPagedata, ZERO_VLANVID, false);
    SetItemShow(pPagedata, ZERO_VLANPRIORITY, false);
}

void CZeroNetworkDelegate::Filter8021Data(CSettingUIPageDataBase * pPagedata)
{
    if (NULL == pPagedata)
    {
        return;
    }

    int nEnableMode8021 = GetComboboxValue(ZERO_8021XMODE);
    if (NETWOKR_802Dotx_Disable != nEnableMode8021)
    {
        return;
    }
    SetItemShow(pPagedata, ZERO_8021XIDENTITY, false);
    SetItemShow(pPagedata, ZERO_8021XMD5PSW, false);
}

void CZeroNetworkDelegate::FilterVLanDhcp(CSettingUIPageDataBase * pPagedata)
{
    if (NULL == pPagedata)
    {
        return;
    }

    int nEnableVLanDhcp = GetComboboxValue(ZERO_VLAN_DHCP);
    if (feature_enable == nEnableVLanDhcp)
    {
        return;
    }
    SetItemShow(pPagedata, ZERO_VLAN_DHCP_OPTION, false);
}

void CZeroNetworkDelegate::FilterLLDPData(CSettingUIPageDataBase * pPagedata)
{
    if (NULL == pPagedata)
    {
        return;
    }

    int nEnableLLDP = GetComboboxValue(ZERO_LLDP_STATUS);
    if (feature_enable == nEnableLLDP)
    {
        return;
    }
    SetItemShow(pPagedata, ZERO_LLDP_PACKET_INTERVAL, false);
}

void CZeroNetworkDelegate::FilterCdpData(CSettingUIPageDataBase * pPagedata)
{
    if (NULL == pPagedata)
    {
        return;
    }

    int nEnableCDP = GetComboboxValue(ZERO_CDP_STATUS);
    if (feature_enable == nEnableCDP)
    {
        return;
    }
    SetItemShow(pPagedata, ZERO_CDP_INTERVAL, false);
}

void CZeroNetworkDelegate::FilterStunData(CSettingUIPageDataBase * pPagedata)
{
    if (NULL == pPagedata)
    {
        return;
    }

    int nEnableStun = GetComboboxValue(ZERO_NAT_STATUS);
    if (feature_enable == nEnableStun)
    {
        return;
    }
    SetItemShow(pPagedata, ZERO_STUN_SERVER, false);
    SetItemShow(pPagedata, ZERO_PORT, false);
}

void CZeroNetworkDelegate::GetPagePrivateSoftkey(CArraySoftKeyBarData & vecSoftKey)
{
    vecSoftKey[0].m_iType = ST_BACK;
    vecSoftKey[0].m_strSoftkeyTitle = _LANG(TRID_BACK);

    vecSoftKey[3].m_iType = ST_NEXT;
    vecSoftKey[3].m_strSoftkeyTitle = _LANG(TRID_NEXT);
}

bool CZeroNetworkDelegate::OnSoftkeyAction(int iType, bool bLongPress, int nDataIndex)
{
    if (!bLongPress && ST_NEXT == iType)
    {
        if (CSettingUIDelegateBase::OnSoftkeyAction(ST_SAVE, bLongPress, nDataIndex))
        {
            SettingUILogic_EnterZeroNextStage(ZERO_NETWORK_PAGE, ZERO_NEXT);
        }
        return true;
    }

    return CSettingUIDelegateBase::OnSoftkeyAction(iType, bLongPress, nDataIndex);
}

bool CZeroNetworkDelegate::ProcessKeyEvent(int nKeyCode, bool bPress)
{
    if (PHONE_KEY_OK == nKeyCode && bPress
            && CSettingUIDelegateBase::OnSoftkeyAction(ST_SAVE, false, GetFocusIndex()))
    {
        SettingUILogic_EnterZeroNextStage(ZERO_NETWORK_PAGE, ZERO_NEXT);
        return true;
    }
    return false;
}

bool CZeroNetworkDelegate::PageDataValid(CSettingUIPageDataBase* pPagedata, CSettingUIDataValidInfo& validInfo)
{
    if (NULL == pPagedata || NULL == g_pNetworkController)
    {
        return false;
    }

    // VPN
    yl::string strValue = GetValue(ZERO_VPNACTIVE);
    int nDataIndex = GetItemDataIndexByItemId(ZERO_VPNACTIVE);

    if ("1" == strValue && !g_pNetworkController->IsOpenVPNConfigExist())
    {
        validInfo.SetValidIndex(nDataIndex);
        validInfo.SetValidNote(_LANG(TRID_VPN_CANNOT_BE_ON_WHEN_FILE_DO_NOT_EXIST));
        return false;
    }

    // dhcpvlan option
    // 验证数据合法性  1--255 最多5个 ","隔开
    nDataIndex = GetItemDataIndexByItemId(ZERO_VLAN_DHCP_OPTION);
    strValue = GetValue(ZERO_VLAN_DHCP_OPTION);

    LIST_STRING listString;
    commonAPI_splitStringBySeparator(strValue, ',', listString);

    if (listString.size() > DHCP_OPTION_MAX_LENGTH)
    {
        validInfo.SetValidIndex(nDataIndex);
        validInfo.SetValidNote(_LANG(TRID_THE_COUNT_OF_DHCP_OPTION_VALUE_CANNOT_BE_MORE_THAN_5));
        return false;
    }

    LIST_STRING::iterator it = listString.begin();
    for (; it != listString.end(); it++)
    {
        const yl::string& strOption = (*it);
        bool bInvalid = true;

        if (commonAPI_IsDigital(strOption))
        {
            int iValue = atoi(strOption.c_str());
            if (iValue >= DHCP_OPTION_MIN_NUMBER && iValue <= DHCP_OPTION_MAX_NUMBER)
            {
                bInvalid = false;
            }
        }

        if (bInvalid)
        {
            validInfo.SetValidIndex(nDataIndex);
            validInfo.SetValidNote(_LANG(TRID_INVALID_DHCP_OPTION));
            return false;
        }
    }

    return CSettinguiDataDelegateBase::PageDataValid(pPagedata, validInfo);
}

