#include "settinguistatusnetworkdelegate.h"
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

CSettingUIStatusNetworkDelegate::CSettingUIStatusNetworkDelegate()
    : CSettingUIDelegateBase()
    , m_nIPMode(-1)
{
}

CSettingUIStatusNetworkDelegate::~CSettingUIStatusNetworkDelegate()
{
}

CSettingUIDelegateBase * CSettingUIStatusNetworkDelegate::CreateStatusNetworkDelegate()
{
    CSettingUIStatusNetworkDelegate * pStatusDelegate = new CSettingUIStatusNetworkDelegate;

    return (CSettingUIDelegateBasePtr)pStatusDelegate;
}

CSettingUIPageDataBase * CSettingUIStatusNetworkDelegate::LoadPageData(bool bReLoad /*= false*/)
{
    // 从内存绥存中获取
    CSettingUIDelegateBase::LoadPageData(bReLoad);

    if (NULL == m_pPageData || !bReLoad)
    {
        return NULL;
    }

    PhoneStatusData phoneData;
    if (g_pStatusController->GetPhoneStatusData(&phoneData))
    {
        SetValue(STATUS_NETWORK_MAC, phoneData.m_strMac);
    }

    m_nIPMode = g_pNetworkController->GetWanIPMode();
    yl::string strIPMode = "";
    if (IPAM_IPV4 == m_nIPMode)
    {
        strIPMode = _LANG(TRID_IPV4);
    }
    else if (IPAM_IPV6 == m_nIPMode)
    {
        strIPMode = _LANG(TRID_IPV6);
    }
    else
    {
        strIPMode = _LANG(TRID_IPV4_AND_IPV6);
    }
    // model
    SetValue(STATUS_NETWORK_IP_MODE, strIPMode);
#ifdef IF_SUPPORT_WIFI
    //wifi_mac
    yl::string strWifiMac = phoneData.m_strWifiMac;
    strWifiMac = strWifiMac.empty() ? _LANG(TRID_NA) : strWifiMac;
    SetValue(STATUS_NETWORK_WiFi_MAC, strWifiMac);
#endif

    return m_pPageData;
}

bool CSettingUIStatusNetworkDelegate::SavePageData(CSettingUIPageDataBase * pPageData,
        bool bBackToPrevious /*= true*/, bool bPopHint /*= true*/)
{
    return true;
}

bool CSettingUIStatusNetworkDelegate::PageDataFilter(CSettingUIPageDataBase * pPagedata,
        bool bReload /*= false*/)
{
    if (NULL == pPagedata)
    {
        return true;
    }

    bool bResult = CSettingUIDelegateBase::PageDataFilter(pPagedata, bReload);

    if (IPAM_IPV4 == m_nIPMode)
    {
        SetItemShow(pPagedata, STATUS_NETWORK_IPV6, false);
    }
    else if (IPAM_IPV6 == m_nIPMode)
    {
        SetItemShow(pPagedata, STATUS_NETWORK_IPV4, false);
    }
    bool bWifiShow = false;
#ifdef IF_SUPPORT_WIFI
    bWifiShow = true;
#endif
    SetItemShow(pPagedata, STATUS_NETWORK_WiFi_MAC, bWifiShow);

    return bResult;
}

void CSettingUIStatusNetworkDelegate::SetValue(const yl::string & strItemId,
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

