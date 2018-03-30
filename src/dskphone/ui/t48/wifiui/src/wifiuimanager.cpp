#include "wifi_inc.h"

/********************************** CWifiUIManagerBase ***************************************/
CWifiUIManagerBase::CWifiUIManagerBase()
    : m_iCurNetID(-1)
    , m_bConnecting(false)
    , m_bOperatering(false)
    , m_bNeedUpdate(false)
    , m_ePageType(ENUM_WIFI_NONE_PAGE_TYPE)
{
    //
}

CWifiUIManagerBase::~CWifiUIManagerBase()
{
    //
}

/************************************* wifi logic *******************************************/
Operatoer_Reslut CWifiUIManagerBase::IsDongleValid()
{
#ifdef IF_SUPPORT_WIFI
    return WIFI_IsDongleVaild() ? OR_INTERFACE_SUCCESS : OR_INTERFACE_FAIL;
#else
    static int ii = 0;
    ++ii;
    return (Operatoer_Reslut)(ii % 2 + 4);
#endif
}

Operatoer_Reslut CWifiUIManagerBase::IsWiredPlugIn()
{
#ifdef IF_SUPPORT_WIFI
    return WIFI_IsCableConnected() ? OR_INTERFACE_SUCCESS : OR_INTERFACE_FAIL;
#else
    static int ii = 0;
    ++ii;
    return (Operatoer_Reslut)(ii % 2 + 4);
#endif
}

Operatoer_Reslut CWifiUIManagerBase::IsWifiHasConnected()
{
#ifdef IF_SUPPORT_WIFI
    return WIFI_IsConnected() ? OR_INTERFACE_SUCCESS : OR_INTERFACE_FAIL;
#else
    static int ii = 0;
    ++ii;
    return (Operatoer_Reslut)(ii % 2 + 4);
#endif
}

Operatoer_Reslut CWifiUIManagerBase::IsConnectingWifiDevice(int iNetID)
{
#ifdef IF_SUPPORT_WIFI
//  SETTINGUI_INFO("[CWifiUIManagerBase::IsConnectingWifiDevice] ID[%d], Connecting[%d]"
//                  , iNetID, WIFI_IsConnecting(iNetID));
    return WIFI_IsConnecting(iNetID) ? OR_INTERFACE_SUCCESS : OR_INTERFACE_FAIL;
#else
    static int ii = 0;
    ++ii;
    return (Operatoer_Reslut)(ii % 2 + 4);
#endif
}

Operatoer_Reslut CWifiUIManagerBase::IsScanning()
{
#ifdef IF_SUPPORT_WIFI
    SETTINGUI_INFO("[CWifiUIManagerBase::IsScanning] isScanning[%d]"
                   , WIFI_IsScan());
    return WIFI_IsScan() ? OR_INTERFACE_SUCCESS : OR_INTERFACE_FAIL;
#else
    static int ii = 0;
    ++ii;
    return (Operatoer_Reslut)(ii % 2 + 4);
#endif
}

Operatoer_Reslut CWifiUIManagerBase::AuthNetWork(int iNetID, const yl::string & strName,
        const yl::string & strPassword)
{
    SETTING_INFO("[CWifiUIManagerBase::AuthNetWork] NetId[%d], Name[%s], Password[%s].", iNetID,
                 strName.c_str(), strPassword.c_str());

    return WIFI_AuthNetwork(iNetID, strName, strPassword) ? OR_INTERFACE_SUCCESS : OR_INTERFACE_FAIL;
};

Operatoer_Reslut CWifiUIManagerBase::AskWifiPassword(int iNetID, const char * pKey)
{
#ifdef IF_SUPPORT_WIFI
//  SETTINGUI_INFO("[CWifiUIManagerBase::AskWifiPassword] ID[%d], key[%s]"
//                  , iNetID, pKey);
    return WIFI_AnswerPassword(iNetID, pKey) ? OR_INTERFACE_SUCCESS : OR_INTERFACE_FAIL;
#else
    static int ii = 0;
    ++ii;
    return (Operatoer_Reslut)(ii % 2 + 4);
#endif
}

Operatoer_Reslut CWifiUIManagerBase::ConnectWifi(std::string strDlgName, int iNetID)
{
#ifdef IF_SUPPORT_WIFI
    SETTINGUI_INFO("[CWifiUIManagerBase::ConnectWifi] ID[%d]", iNetID);

    return WIFI_StartConnect(iNetID) ? OR_INTERFACE_SUCCESS : OR_INTERFACE_FAIL;
#else
    static int ii = 0;
    ++ii;
    return (Operatoer_Reslut)(ii % 2 + 3);
#endif
}

Operatoer_Reslut CWifiUIManagerBase::CancelWifiConnect(std::string strDlgName, int iNetID)
{
#ifdef IF_SUPPORT_WIFI
    SETTINGUI_INFO("[CWifiUIManagerBase::CancelWifiConnect] ID[%d]", iNetID);
    WIFI_CancelConnect(iNetID);

    return OR_INTERFACE_SUCCESS;
#else
    static int ii = 0;
    ++ii;
    return (Operatoer_Reslut)(ii % 2 + 3);
#endif
}

Operatoer_Reslut CWifiUIManagerBase::DisconnectWifi(std::string strDlgName, int iNetID)
{
#ifdef IF_SUPPORT_WIFI
    SETTINGUI_INFO("[CWifiUIManagerBase::DisconnectWifi] ID[%d]", iNetID);
    WIFI_StartDisconnect(iNetID);

    return OR_INTERFACE_SUCCESS;
#else
    static int ii = 0;
    ++ii;
    return (Operatoer_Reslut)(ii % 2 + 4);
#endif
}

Operatoer_Reslut CWifiUIManagerBase::DeleteWifiDevice(std::string strDlgName, int iNetID)
{
#ifdef IF_SUPPORT_WIFI
    SETTINGUI_INFO("[CWifiUIManagerBase::DeleteWifiDevice] ID[%d]", iNetID);
    WIFI_DeleteNetwork(iNetID);

    return OR_INTERFACE_SUCCESS;
#else
    static int ii = 0;
    ++ii;
    return (Operatoer_Reslut)(ii % 2 + 4);
#endif
}

Operatoer_Reslut CWifiUIManagerBase::DeleteAllWifiDevices(std::string strDlgName)
{
#ifdef IF_SUPPORT_WIFI
    return OR_INTERFACE_SUCCESS;
#else
    static int ii = 0;
    ++ii;
    return (Operatoer_Reslut)(ii % 2 + 4);
#endif
}

Operatoer_Reslut CWifiUIManagerBase::ScanWifiDevices(std::string strDlgName)
{
#ifdef IF_SUPPORT_WIFI
    return WIFI_StartScan() ? OR_INTERFACE_SUCCESS : OR_INTERFACE_FAIL;
#else
    static int ii = 0;
    ++ii;
    return (Operatoer_Reslut)(ii % 2 + 4);
#endif
}

Operatoer_Reslut CWifiUIManagerBase::CancelScanWifiDevices(std::string strDlgName)
{
#ifdef IF_SUPPORT_WIFI
    return WIFI_StopScan() ? OR_INTERFACE_SUCCESS : OR_INTERFACE_FAIL;
#else
    static int ii = 0;
    ++ii;
    return (Operatoer_Reslut)(ii % 2 + 4);
#endif
}

Operatoer_Reslut CWifiUIManagerBase::IsWifiEnable()
{
#ifdef IF_SUPPORT_WIFI
    return WIFI_IsEnable() ? OR_INTERFACE_SUCCESS : OR_INTERFACE_FAIL;
#else
    static int ii = 0;
    ++ii;
    return (Operatoer_Reslut)(ii % 2 + 4);
#endif
}

Operatoer_Reslut CWifiUIManagerBase::SetWifiEnable(bool bEnable)
{
#ifdef IF_SUPPORT_WIFI
    SETTINGUI_INFO("[CWifiUIManagerBase::SetWifiEnable] status[%d]", bEnable);
    return WIFI_SetEnable(bEnable) ? OR_INTERFACE_SUCCESS : OR_INTERFACE_FAIL;
#else
    return OR_NONE;
#endif
}

Operatoer_Reslut CWifiUIManagerBase::GetWifiWPSPIN(std::string & strWpsPin)
{
#ifdef IF_SUPPORT_WIFI
    yl::string strPin;
    WIFI_GetWpsPin(strPin);
    std::string strWPSPin(strPin.c_str());
    strWpsPin = strWPSPin;
    SETTINGUI_INFO("[CWifiUIManagerBase::GetWifiWPSPIN] wpsPin[%s]", strWPSPin.c_str());

    return OR_INTERFACE_SUCCESS;
#else
    return OR_INTERFACE_SUCCESS;
#endif
}

Operatoer_Reslut CWifiUIManagerBase::StartWifiWPSPIN()
{
#ifdef IF_SUPPORT_WIFI
    SETTINGUI_INFO("[CWifiUIManagerBase::StartWifiWPSPIN()]");
    return WIFI_StartWpspin() ? OR_INTERFACE_SUCCESS : OR_INTERFACE_FAIL;
#else
    return OR_NONE;
#endif
}

Operatoer_Reslut CWifiUIManagerBase::StartWifiWPS()
{
#ifdef IF_SUPPORT_WIFI
    SETTINGUI_INFO("[CWifiUIManagerBase::StartWifiWPS()]");
    return WIFI_StartWps() ? OR_INTERFACE_SUCCESS : OR_INTERFACE_FAIL;
#else
    return OR_NONE;
#endif
}

Operatoer_Reslut CWifiUIManagerBase::StopWifiWPS_WPSPIN()
{
#ifdef IF_SUPPORT_WIFI
    SETTINGUI_INFO("[CWifiUIManagerBase::StopWifiWPS_WPSPIN()]");
    return WIFI_CancelWps() ? OR_INTERFACE_SUCCESS : OR_INTERFACE_FAIL;
#else
    return OR_NONE;
#endif
}

// adjust wifi location
Operatoer_Reslut CWifiUIManagerBase::AdjustWifiLocation(int iNetID, bool bUp)
{
#ifdef IF_SUPPORT_WIFI
    SETTINGUI_INFO("[CWifiUIManagerBase::AdjustWifiLocation] ID[%d], bUp[%d]", iNetID, bUp);
    return WIFI_AdjustPosition(iNetID, bUp) ? OR_INTERFACE_SUCCESS : OR_INTERFACE_FAIL;
#else
    return OR_NONE;
#endif
}

// adjust wifi location
Operatoer_Reslut CWifiUIManagerBase::AdjustWifiLocation(int iNetID, int iPos)
{
#ifdef IF_SUPPORT_WIFI
    SETTINGUI_INFO("[CWifiUIManagerBase::AdjustWifiLocation] ID[%d], pos[%d]", iNetID, iPos);
    return WIFI_AdjustPosition(iNetID, iPos) ? OR_INTERFACE_SUCCESS : OR_INTERFACE_FAIL;
#else
    return OR_NONE;
#endif
}

/************************************* UI aider *********************************************/
void CWifiUIManagerBase::SetOperatering(bool bOp)
{
    m_bOperatering = bOp;
}

void CWifiUIManagerBase::SetConnecting(bool bConnecting)
{
    m_bConnecting = bConnecting;
}

void CWifiUIManagerBase::SetPageType(int eType)
{
    if (eType >= ENUM_WIFI_NONE_PAGE_TYPE && eType < ENUM_WIFI_MAX_PAGE_TYPE)
    {
        m_ePageType = eType;
    }
}

void CWifiUIManagerBase::SetCurID(int iCurID)
{
    m_iCurNetID = iCurID;
}

void CWifiUIManagerBase::SetNeedUpdate(bool bNeedUpdate)
{
    SETTINGUI_INFO("set update[%d]", bNeedUpdate);
    m_bNeedUpdate = bNeedUpdate;
}

/************************************** pop dialog *****************************************/
// common pop dlg
void CWifiUIManagerBase::Wifi_ShowCommonMsgBox(CMsgBoxCallBackAction * pAction,
        const QString & strNote,
        MESSAGEBOX_TYPE eType,
        int nMsTime /*= -1*/,
        const QString & strCancelBtnTitle /*= ""*/,
        const QString & strOkBtnTitle /*= ""*/,
        int iID /*= -1*/,
        const QVariant & extraData /*= QVariant()*/,
        bool bShowCountDown /*= false*/)
{
    MessageBox_ShowCommonMsgBox(pAction, LANG_TRANSLATE(strNote), eType, nMsTime
                                , strCancelBtnTitle, strOkBtnTitle, iID, extraData
                                , bShowCountDown
                               );
}

/********************************** CT48WifiUIManager ***************************************/
//获取全局唯一实例
CT48WifiUIManager & CT48WifiUIManager::GetInstance()
{
    static CT48WifiUIManager s_Instance;
    return s_Instance;
}

CT48WifiUIManager::CT48WifiUIManager()
{
    //
}

CT48WifiUIManager::~CT48WifiUIManager()
{
    //
}

Operatoer_Reslut CT48WifiUIManager::ScanWifiDevices(std::string strDlgName)
{
#ifdef IF_SUPPORT_WIFI
    if (OR_INTERFACE_FAIL == IsDongleValid())
    {
        return OR_NO_DONGLE;
    }

    return WIFI_StartScan() ? OR_INTERFACE_SUCCESS : OR_INTERFACE_FAIL;
#else
    static int ii = 0;
    ++ii;
    return (Operatoer_Reslut)(ii % 2 + 4);
#endif
}

/********************************** CT49WifiUIManager ***************************************/
//获取全局唯一实例
CT49WifiUIManager & CT49WifiUIManager::GetInstance()
{
    static CT49WifiUIManager s_Instance;
    return s_Instance;
}

CT49WifiUIManager::CT49WifiUIManager()
{
    //
}

CT49WifiUIManager::~CT49WifiUIManager()
{
    //
}

Operatoer_Reslut CT49WifiUIManager::ScanWifiDevices(std::string strDlgName)
{
#ifdef IF_SUPPORT_WIFI
    return WIFI_StartScan() ? OR_INTERFACE_SUCCESS : OR_INTERFACE_FAIL;
#else
    static int ii = 0;
    ++ii;
    return (Operatoer_Reslut)(ii % 2 + 4);
#endif
}
