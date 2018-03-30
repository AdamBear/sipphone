#include "settinguiwanportstaticclientdelegate.h"

#if !IF_SUPPORT_IP_SET_SINGLE_MENU
#include "settingui/include/pageiddefine.h"
#include "settingui/include/itemnamedefine.h"
#include <configiddefine.h>
#include "configparser/modconfigparser.h"
#include "settinguilogic/include/common.h"
#include "settinguilogic/src/networksettingmanager.h"
#include "editcheckmethod.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"

CSettingUIWanPortStaticClientDelegate::CSettingUIWanPortStaticClientDelegate()
{

}

CSettingUIWanPortStaticClientDelegate::~CSettingUIWanPortStaticClientDelegate()
{

}

CSettingUIDelegateBase * CSettingUIWanPortStaticClientDelegate::CreatWanPortStaticClientDelegate()
{
    CSettingUIWanPortStaticClientDelegate * pDelegate = new CSettingUIWanPortStaticClientDelegate;

    return (CSettingUIDelegateBasePtr)pDelegate;
}

bool CSettingUIWanPortStaticClientDelegate::PageDataValid(CSettingUIPageDataBase * pPagedata,
        CSettingUIDataValidInfo & validInfo)
{
    if (NULL == pPagedata || kszPageIDWanPortIpv4StaticClient != GetPageID())
    {
        return CSettinguiDataDelegateBase::PageDataValid(pPagedata, validInfo);
    }

    yl::string strIP = GetValue(STATIC_IP);
    if (!IsValidIP(strIP))
    {
        return SetInvalidNote(validInfo, TRID_INVALID_IP_ADDRESS,
                              pPagedata->GetItemDataIndexByItemId(STATIC_IP));
    }

    yl::string strMask = GetValue(STATIC_SUBNET_MASK);
    if (IsValidIPv6(strIP))
    {
        if (!strMask.empty() && IsValidNumber(strMask) && !isInRange(strMask, 0, 128))
        {
            return SetInvalidNote(validInfo, TRID_INVALID_SUBNET_MASK,
                                  pPagedata->GetItemDataIndexByItemId(STATIC_SUBNET_MASK));
        }
    }
    else if (IsValidIP(strIP) && !IsValidIP(strMask))
    {
        return SetInvalidNote(validInfo, TRID_INVALID_SUBNET_MASK,
                              pPagedata->GetItemDataIndexByItemId(STATIC_SUBNET_MASK));
    }

    yl::string strGateWay = GetValue(STATIC_DEFAULT_GATEWAY);
    if (!IsValidIP(strGateWay))
    {
        return SetInvalidNote(validInfo, TRID_INVALID_DEFAULT_GATEWAY,
                              pPagedata->GetItemDataIndexByItemId(STATIC_DEFAULT_GATEWAY));
    }

    yl::string strPriDNS = GetValue(STATIC_PRI_DNS);
    if (!IsContentEmpty(strPriDNS) && !IsValidIPv4(strPriDNS))
    {
        return SetInvalidNote(validInfo, TRID_INVALID_PRIMARY_DNS,
                              pPagedata->GetItemDataIndexByItemId(STATIC_PRI_DNS));
    }

    yl::string strSecDNS = GetValue(STATIC_SEC_DNS);
    if (!IsContentEmpty(strSecDNS) && !IsValidIPv4(strSecDNS))
    {
        return SetInvalidNote(validInfo, TRID_INVALID_SECONDARY_DNS,
                              pPagedata->GetItemDataIndexByItemId(STATIC_SEC_DNS));
    }

    return true;
}

void CSettingUIWanPortStaticClientDelegate::SaveExtraData()
{
    yl::string strPageId = GetPageID();

    if (kszPageIDWanPortIpv4StaticClient == strPageId)
    {
        if (WAN_STATIC != configParser_GetConfigInt(kszNetworkWanType))
        {
            _NetworkChangeFlagManager.SetNetworkChangeFlag(true);
            configParser_SetConfigInt(kszNetworkWanType, WAN_STATIC, CONFIG_LEVEL_PHONE);
        }
    }
    else if (kszPageIDWanPortIpv6StaticClient == strPageId)
    {
        if (IPv6_WAN_STATIC != configParser_GetConfigInt(kszNetworkIPv6WANType))
        {
            _NetworkChangeFlagManager.SetNetworkChangeFlag(true);
            configParser_SetConfigInt(kszNetworkIPv6WANType, IPv6_WAN_STATIC, CONFIG_LEVEL_PHONE);
        }
    }
}

bool CSettingUIWanPortStaticClientDelegate::SetInvalidNote(CSettingUIDataValidInfo & validInfo,
        const yl::string & strNote, int nIndex)
{
    validInfo.SetValidNote(_LANG(strNote));
    validInfo.SetValidIndex(nIndex);
    return false;
}
#endif
