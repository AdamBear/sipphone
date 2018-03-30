#include "settinguistatusphonedelegate.h"
#include "settinguilogic/src/datafactory.h"
#include "settinguilogic/src/dataloader.h"
#include "settinguilogic/include/pagedatafactory.h"
#include "settinguilogic/src/settinguibaseitemdata.h"
#include "settinguilogic/src/settinguibasepagedata.h"

#include "settingui/include/itemnamedefine.h"
#include "settinguilogic/include/common.h"
#include "settinguilogic/src/statuscontroller.h"
#include "common/common_data_define.h"
#include <configiddefine.h>
#include "configparser/modconfigparser.h"
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"

#ifndef _WIN32
#include "dl/dl.h"
#include "devicelib/networkserver.h"
#endif

CSettingUIStatusPhoneDelegate::CSettingUIStatusPhoneDelegate()
{
}

CSettingUIStatusPhoneDelegate::~CSettingUIStatusPhoneDelegate()
{
}

CSettingUIDelegateBase * CSettingUIStatusPhoneDelegate::CreateStatusPhoneDelegate()
{
    CSettingUIStatusPhoneDelegate * pStatusDelegate = new CSettingUIStatusPhoneDelegate;

    return (CSettingUIDelegateBase *)pStatusDelegate;
}

CSettingUIPageDataBase * CSettingUIStatusPhoneDelegate::LoadPageData(bool bReLoad /*= false*/)
{
    CSettinguiDataDelegateBase::LoadPageData(bReLoad);
    if (!bReLoad || NULL == m_pPageData)
    {
        return m_pPageData;
    }

    PhoneStatusData phoneData;
    if (g_pStatusController->GetPhoneStatusData(&phoneData))
    {
        // model
        SetValue(STATUS_PHONE_MODEL, phoneData.m_strModel);

        // Hardware
        SetValue(STATUS_PHONE_HARDWARE, phoneData.m_strHardWare);

        // Firmware
        SetValue(STATUS_PHONE_FIRMWARE, phoneData.m_strFirmWare);

        yl::string strConfigVersion = configParser_GetConfigString(kszCfgVersion);
        SetValue(STATUS_PHONE_CONFIG_VERSION, strConfigVersion);

        // Product ID
        SetValue(STATUS_PHONE_PRODUCTID, phoneData.m_strProductId);

        // Mac
        SetValue(STATUS_PHONE_MAC, phoneData.m_strMac);

#ifdef IF_SUPPORT_WIFI
       //WifiMac
        yl::string strWifiMac = phoneData.m_strWifiMac;
        strWifiMac = strWifiMac.empty() ? _LANG(TRID_NA) : strWifiMac;
        SetValue(STATUS_PHONE_WiFi_MAC, strWifiMac);
#endif
#ifdef IF_SUPPORT_BLUETOOTH
        //Bluetooth
        yl::string strBluetoothMac = phoneData.m_strBluetoothMac;
        strBluetoothMac = strBluetoothMac.empty() ? _LANG(TRID_NA) : strBluetoothMac;
        SetValue(STATUS_PHONE_BLUTOOTH_MAC, strBluetoothMac);
#endif

        yl::string strDiveceCert = "";
        if (GetDeviceCertStatus())
        {
            strDiveceCert = _LANG(TRID_FACTORY_INSTALLED);
        }
        else
        {
            strDiveceCert = _LANG(TRID_NOT_INSTALLED);
        }
        //Device Cert
        SetValue(STATUS_PHONE_DEVICECERT, strDiveceCert);
    }

    return m_pPageData;
}

bool CSettingUIStatusPhoneDelegate::PageDataFilter(CSettingUIPageDataBase * pPagedata,
        bool bReload /* = false */)
{
    bool bResult = CSettingUIDelegateBase::PageDataFilter(pPagedata, bReload);

    if (NULL != pPagedata)
    {
        bool bShowConfigVersion = !GetValue(STATUS_PHONE_CONFIG_VERSION).empty();
        SetItemShow(pPagedata, STATUS_PHONE_CONFIG_VERSION, bShowConfigVersion);
    }
    bool bBluetoothShow = false;
    bool bWifiShow = false;
#ifdef IF_SUPPORT_BLUETOOTH
        bBluetoothShow = true;
#endif
#ifdef IF_SUPPORT_WIFI
        bWifiShow = true;
#endif
    SetItemShow(pPagedata, STATUS_PHONE_WiFi_MAC, bBluetoothShow);
    SetItemShow(pPagedata, STATUS_PHONE_BLUTOOTH_MAC, bWifiShow);

    return bResult;
}

bool CSettingUIStatusPhoneDelegate::SavePageData(CSettingUIPageDataBase * pPageData,
        bool bBackToPrevious /*= true*/, bool bPopHint /*= true*/)
{
    return true;
}

bool CSettingUIStatusPhoneDelegate::GetDeviceCertStatus()
{
#ifndef _WIN32
    dl_ca_info_ex info_ex;
    response_t ret_response;


    memset(&info_ex, 0, sizeof(dl_ca_info_ex));
    /* cert_flags证书类型，普通CA为0；客户端证书DL_CLIENT_CERT */
    if (dl_get_ca_info_ex("/config/certs/phonedev/WebServer.pem", &info_ex, DL_CLIENT_CERT,
                          &ret_response))
    {
        SETTINGUI_INFO("dl_cert_verify  Error %d: %s", ret_response.code, ret_response.code_msg);
        return false;
    }
    else
    {
        SETTINGUI_INFO("          version = %d", info_ex.version);
        SETTINGUI_INFO("          serial_num = %s", info_ex.serial_num);
        SETTINGUI_INFO("          signature_algorithm = %s", info_ex.signature_algorithm);
        SETTINGUI_INFO("          pubkey_algorithm = %s", info_ex.pubkey_algorithm);
        SETTINGUI_INFO("          pubkey_bits = %d", info_ex.pubkey_bits);
        SETTINGUI_INFO("          priv_key_algor = %s", info_ex.priv_key_algor);
        SETTINGUI_INFO("          priv_key_bits = %d", info_ex.priv_key_bits);
        SETTINGUI_INFO("          issued_to = %s", info_ex.issued_to);
        SETTINGUI_INFO("          issued_by = %s", info_ex.issued_by);
        SETTINGUI_INFO("          validity_begin = %s", info_ex.validity_begin);
        SETTINGUI_INFO("          expire_date = %s", info_ex.expire_date);
        SETTINGUI_INFO("          hash = 0x%x", info_ex.hash);
        SETTINGUI_INFO("          extensions_num = %d", info_ex.extensions_num);
    }
    yl::string strMac = netGetMacText();
    yl::string strInfo = info_ex.issued_to;
    strMac.MakeLower();
    strInfo.MakeLower();
    SETTINGUI_INFO("          strInfo = %s", strInfo.c_str());
    SETTINGUI_INFO("          strMac = %s", strMac.c_str());
//  if (strInfo.Compare(strMac) == 0)
    if (strInfo == strMac)
    {
        /**
        * ca_path，指定信任证书CA路径，CA证书以hash值命名存放，可通过导入到话机中查看证书hash值，然后预制到phoneca/目录下
        * ca_file，指定信任证书CA文件
        * cert_flags证书类型，普通CA为0，客户端证书DL_CLIENT_CERT
        */
        if (dl_cert_verify("/config/certs/phonedev/WebServer.pem", "/config/certs/phoneca/", NULL,
                           DL_CLIENT_CERT, &ret_response))
        {
            SETTINGUI_INFO("dl_cert_verify  Error %d: %s", ret_response.code, ret_response.code_msg);
            return false;
        }
    }
    else
    {
        return false;
    }

    SETTINGUI_INFO("GetDeviceCertStatus end");
#endif
    return true;
}
