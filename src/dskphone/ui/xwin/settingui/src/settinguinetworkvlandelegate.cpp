#include "settinguinetworkvlandelegate.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "settingui/include/itemnamedefine.h"
#include "settingui/include/pageiddefine.h"
#include <configiddefine.h>
#include "configparser/modconfigparser.h"
#include "settinguilogic/include/common.h"
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"

namespace
{
#define DHCP_OPTION_MAX_NUMBER 255
#define DHCP_OPTION_MIN_NUMBER 1
#define INDEX_DHCP_OPTION 1
#define DHCP_OPTION_MAX_LENGTH 4
}

CSettinguiNetworkVlanDelegate::CSettinguiNetworkVlanDelegate()
{

}

CSettinguiNetworkVlanDelegate::~CSettinguiNetworkVlanDelegate()
{

}

CSettingUIDelegateBase * CSettinguiNetworkVlanDelegate::CreatNetworkVlanDelegate()
{
    CSettinguiNetworkVlanDelegate * pDelegate = new CSettinguiNetworkVlanDelegate;

    return (CSettingUIDelegateBasePtr)pDelegate;
}

bool CSettinguiNetworkVlanDelegate::PageDataValid(CSettingUIPageDataBase * pPagedata,
        CSettingUIDataValidInfo & validInfo)
{
    if (NULL == pPagedata)
    {
        return false;
    }

    yl::string strPageId = GetPageID();
    if (kszPageIDNetworkVlanPcPort == strPageId || kszPageIDNetworkVlanWanPort == strPageId)
    {
        yl::string strItemName = "";
        strItemName = (kszPageIDNetworkVlanPcPort == strPageId) ? VLAN_PCPORT_STATUS : VLAN_WANPORT_STATUS;
        int nValue = GetComboboxValue(strItemName);

        if (1 == nValue && WAN_PPPOE == configParser_GetConfigInt(kszNetworkWanType))
        {
            validInfo.SetValidIndex(0);
            validInfo.SetValidNote(_LANG(TRID_VLAN_UNSURPPORTED_IN_PPPOE_MODE));
            return false;
        }
    }
    else if (kszPageIDNetworkVlanDhcp == strPageId)
    {
        yl::string strOption = GetValue(VLAN_DHCP_OPTION);
        return IsDHCPOptionValid(strOption, validInfo);
    }

    return CSettingUIDelegateBase::PageDataValid(pPagedata, validInfo);
}

bool CSettinguiNetworkVlanDelegate::IsDHCPOptionValid(const yl::string & strOption,
        CSettingUIDataValidInfo & validInfo)
{
    if (strOption.empty())
    {
        return SetInValidData(_LANG(TRID_INVALID_DHCP_OPTION), 1, validInfo);
    }

    // 验证数据合法性  1--255 最多5个 ","隔开
    int iValue = 0;
    int iNum = 0;
    int iLen = strOption.size();
    for (int i = 0; i < iLen; ++i)
    {
        if (',' == strOption.at(i))
        {
            ++iNum;
            if (iValue < DHCP_OPTION_MIN_NUMBER || iValue > DHCP_OPTION_MAX_NUMBER)
            {
                return SetInValidData(_LANG(TRID_INVALID_DHCP_OPTION), INDEX_DHCP_OPTION, validInfo);
            }
            else if (iNum > DHCP_OPTION_MAX_LENGTH)
            {
                return SetInValidData(_LANG(TRID_THE_COUNT_OF_DHCP_OPTION_VALUE_CANNOT_BE_MORE_THAN_5), 1,
                                      validInfo);
            }
            iValue = 0;
        }
        else if ('0' <= strOption.at(i) && strOption.at(i) <= '9')
        {
            iValue = iValue * 10 + (strOption.at(i) - '0');
            if (iValue > DHCP_OPTION_MAX_NUMBER)
            {
                return SetInValidData(_LANG(TRID_INVALID_DHCP_OPTION), INDEX_DHCP_OPTION, validInfo);
            }
        }
        else
        {
            return SetInValidData(_LANG(TRID_INVALID_DHCP_OPTION), INDEX_DHCP_OPTION, validInfo);
        }
    }

    if (iValue > DHCP_OPTION_MAX_NUMBER || iValue < DHCP_OPTION_MIN_NUMBER)
    {
        return SetInValidData(_LANG(TRID_INVALID_DHCP_OPTION), INDEX_DHCP_OPTION, validInfo);
    }

    return true;
}

bool CSettinguiNetworkVlanDelegate::SetInValidData(const yl::string & strOption, int nIndex,
        CSettingUIDataValidInfo & validInfo)
{
    validInfo.SetValidIndex(nIndex);
    validInfo.SetValidNote(strOption);
    return false;
}
