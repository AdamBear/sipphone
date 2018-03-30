#ifndef __WIFI_UI_MANAGER_H
#define __WIFI_UI_MANAGER_H

#include <string>
#include <QString>
#include "wifiui/include/wirelesscommondata.h"
#include "messagebox/messageboxbase.h"

class CMsgBoxCallBackAction;

class CWifiUIManagerBase
{
public:
    CWifiUIManagerBase();
    ~CWifiUIManagerBase();

    /************************************* UI aider *********************************************/
    inline bool IsOperatering()
    {
        return m_bOperatering;
    }
    void SetOperatering(bool bOp);
    inline bool IsConnecting()
    {
        return m_bConnecting;
    }
    void SetConnecting(bool bConnecting);
    inline int GetPageType()
    {
        return m_ePageType;
    }
    void SetPageType(int eType);
    inline int GetCurID()
    {
        return m_iCurNetID;
    }
    void SetCurID(int iCurID);
    inline bool IsNeedUpdate()
    {
        return m_bNeedUpdate;
    }
    void SetNeedUpdate(bool bNeedUpdate);

    /************************************* wifi logic *******************************************/
    Operatoer_Reslut IsDongleValid();
    Operatoer_Reslut IsWiredPlugIn();
    Operatoer_Reslut IsWifiHasConnected();
    Operatoer_Reslut IsConnectingWifiDevice(int iNetID);

    Operatoer_Reslut IsScanning();
    Operatoer_Reslut
    IsWifiEnable();                                        // is wifi switch open or not
    Operatoer_Reslut SetWifiEnable(bool bEnable);                           // set wifi switch

    Operatoer_Reslut GetWifiWPSPIN(std::string & strWpsPin);                // get WPS PIN code
    Operatoer_Reslut StartWifiWPSPIN();                                     // start WPS PIN
    Operatoer_Reslut StartWifiWPS();                                        // start WPS
    Operatoer_Reslut StopWifiWPS_WPSPIN();                                  // stop WPS or WPS PIN

    Operatoer_Reslut AuthNetWork(int iNetID, const yl::string & strName,
                                 const yl::string & strPassword);
    Operatoer_Reslut AskWifiPassword(int iNetID, const char * pKey);
    Operatoer_Reslut ConnectWifi(std::string strDlgName, int iNetID);       // connect wifi
    Operatoer_Reslut CancelWifiConnect(std::string strDlgName, int iNetID); // cannel connecting-wifi
    Operatoer_Reslut DisconnectWifi(std::string strDlgName, int iNetID);    // disconnect connected-wifi
    Operatoer_Reslut DeleteWifiDevice(std::string strDlgName, int iNetID);  // delete one wifi device
    Operatoer_Reslut DeleteAllWifiDevices(std::string
                                          strDlgName);          // delete all wifi devices(invalid interface)

    virtual Operatoer_Reslut ScanWifiDevices(std::string
            strDlgName);       // start scan round wifi devices
    Operatoer_Reslut CancelScanWifiDevices(std::string
                                           strDlgName);         // cancel scan round wifi devices

    Operatoer_Reslut AdjustWifiLocation(int iNetID, bool bUp);              // adjust wifi location
    Operatoer_Reslut AdjustWifiLocation(int iNetID, int iPos);              // adjust wifi location

    /************************************** pop dialog *****************************************/
    void Wifi_ShowCommonMsgBox(CMsgBoxCallBackAction * pAction,
                               const QString & strNote,
                               MESSAGEBOX_TYPE eType,
                               int nMsTime = -1,
                               const QString & strCancelBtnTitle = "",
                               const QString & strOkBtnTitle = "",
                               int iID = -1,
                               const QVariant & extraData = QVariant(),
                               bool bShowCountDown = false);

private:
    bool        m_bOperatering;
    int         m_ePageType;                // enter page by type
    int         m_iCurNetID;                // cur connecting or connected wifi device ID
    bool        m_bConnecting;              // is connecting or not
    bool        m_bNeedUpdate;              // judge return to base dlg need to update or not
};

class CT48WifiUIManager : public CWifiUIManagerBase
{
public:
    static CT48WifiUIManager & GetInstance();

private:
    CT48WifiUIManager();
    ~CT48WifiUIManager();
public:

    virtual Operatoer_Reslut ScanWifiDevices(std::string
            strDlgName);       // start scan round wifi devices
};

class CT49WifiUIManager : public CWifiUIManagerBase
{
public:
    static CT49WifiUIManager & GetInstance();

private:
    CT49WifiUIManager();
    ~CT49WifiUIManager();
public:

    virtual Operatoer_Reslut ScanWifiDevices(std::string
            strDlgName);       // start scan round wifi devices
};

//typedef CT49WifiUIManager CWifiUIManager;
typedef CT48WifiUIManager CWifiUIManager;

#define _UIWifiManager (CWifiUIManager::GetInstance())

#endif
