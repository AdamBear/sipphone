#include "settinguiwanportipsetdelegate.h"

#if IF_SUPPORT_IP_SET_SINGLE_MENU
#include "settingui/include/pageiddefine.h"
#include "settingui/include/itemnamedefine.h"
#include "settinguilogic/include/common.h"
#include "settinguilogic/src/networksettingmanager.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "uicallbackfun.h"

namespace
{
#define   feature_enable        1
#define   feature_disable       0
}

CSettingUIWanPortIPSetDelegate::CSettingUIWanPortIPSetDelegate()
{

}

CSettingUIWanPortIPSetDelegate::~CSettingUIWanPortIPSetDelegate()
{

}

CSettingUIDelegateBase* CSettingUIWanPortIPSetDelegate::CreatWanPortIPSetDelegate()
{
    CSettingUIWanPortIPSetDelegate* pDelegate = new CSettingUIWanPortIPSetDelegate;

    return (CSettingUIDelegateBasePtr)pDelegate;
}

bool CSettingUIWanPortIPSetDelegate::PageDataFilter(CSettingUIPageDataBase* pPagedata, bool bReload /*= false*/)
{
    if (NULL == pPagedata)
    {
        return false;
    }

    bool bResult = CSettingUIDelegateBase::PageDataFilter(pPagedata, bReload);

    int nWanType;
    if (kszPageIDWanPortIPV4 == m_strPageID)
    {
        nWanType = GetComboboxValue(IPV4WANTYPE);
    }
    else if (kszPageIDWanPortIPV6 == m_strPageID)
    {
        nWanType = GetComboboxValue(IPV6WANTYPE);
    }

    if (WAN_DHCP == nWanType)
    {
        if (kszPageIDWanPortIPV4 == m_strPageID)
        {
            HideIpV4StaticIPItem(pPagedata);
        }
        else if (kszPageIDWanPortIPV6 == m_strPageID)
        {
            HideIpV6StaticIPItem(pPagedata);
        }

        int nStaticDns = GetComboboxValue(WANPORT_DHCP_ENABLE);
        if (feature_enable == nStaticDns)
        {
            return true;
        }
        HideStaticDnsItem(pPagedata);
    }
    else if (WAN_STATIC == nWanType)
    {
        SetItemShow(pPagedata, WANPORT_DHCP_ENABLE, false);
        HideStaticDnsItem(pPagedata);
    }

    return bResult;
}

void CSettingUIWanPortIPSetDelegate::OnComboBoxValueChange(int nIndex)
{
    g_SetUICallBackFun.RefreshUI(false, GetPageID());
}

void CSettingUIWanPortIPSetDelegate::HideIpV4StaticIPItem(CSettingUIPageDataBase* pPagedata)
{
    if (NULL == pPagedata)
    {
        return;
    }

    SetItemShow(pPagedata, STATIC_IP, false);
    SetItemShow(pPagedata, STATIC_SUBNET_MASK, false);
    SetItemShow(pPagedata, STATIC_DEFAULT_GATEWAY, false);
    SetItemShow(pPagedata, STATIC_PRI_DNS, false);
    SetItemShow(pPagedata, STATIC_SEC_DNS, false);
}

void CSettingUIWanPortIPSetDelegate::HideIpV6StaticIPItem(CSettingUIPageDataBase* pPagedata)
{
    if (NULL == pPagedata)
    {
        return;
    }

    SetItemShow(pPagedata, STATIC_IPV6, false);
    SetItemShow(pPagedata, STATIC_IPV6_PREFIX, false);
    SetItemShow(pPagedata, STATIC_IPV6_DEFAULT_GATEWAY, false);
    SetItemShow(pPagedata, STATIC_IPV6_PRI_DNS, false);
    SetItemShow(pPagedata, STATIC_IPV6_SEC_DNS, false);
}

void CSettingUIWanPortIPSetDelegate::HideStaticDnsItem(CSettingUIPageDataBase* pPagedata)
{
    if (NULL == pPagedata)
    {
        return;
    }

    SetItemShow(pPagedata, WANPORT_DHCP_PEIDNS, false);
    SetItemShow(pPagedata, WANPORT_DHCP_SECDNS, false);
}

#endif
