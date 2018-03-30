#ifdef IF_WIFI_SIMULATION_DRIVER
#include "wifitestdriver.h"

LRESULT OnWIFITestDriverMsg(msgObject & objMsg)
{
    return _WiFiTestDriver.OnMsg(objMsg);
}

CWiFiTestDriver::CWiFiTestDriver()
{
    m_bAdapterEnable = ENABLE;

    etl_RegisterMsgHandle(WIFI_MSG_TO_TESTDRIVER, WIFI_MSG_TO_TESTDRIVER, OnWIFITestDriverMsg);

    // _ProcessHelper.LaunchLocalThread(_WIFITestThread);
    _WIFITestThread.Launch();

    WIRE_INFO("********WIFI TEST MSG[%d]**************", WIFI_MSG_TO_TESTDRIVER);

    _WIFITestData.Init();
}

CWiFiTestDriver::~CWiFiTestDriver()
{
    m_bAdapterEnable = DISABLE;
    etl_UnregisterMsgHandle(WIFI_MSG_TO_TESTDRIVER, WIFI_MSG_TO_TESTDRIVER, OnWIFITestDriverMsg);
}


//---------------------------------------------------------------------------
// Simulation of driver
//---------------------------------------------------------------------------
int CWiFiTestDriver::SetGlobalFunc(mkit_handle_t caller, int op, int timeout)
{
    int iResult = _WIFITestData.SetWifiStatus(op);

    WIRE_INFO("TestDriver---->SetGlobalFunc(),result:%d", iResult ? 0 : 1);
    return iResult ? 0 : 1;
}

int CWiFiTestDriver::SyncStatus(mkit_handle_t caller, int timeout)
{
    return 0;
}

int CWiFiTestDriver::ConfigChanged(mkit_handle_t caller, int timeout)
{
    return 0;
}

//profile_id - 保存的网络ID,范围0-4
//此处没有配置文件,直接在链表里返回随机ap id
int CWiFiTestDriver::GetAPID(mkit_handle_t caller, int profile_id, char * ssid, int len,
                             int timeout)
{
    return (_WIFITestData.GetRandNum(RAND_AP_ID) + 50);
}

int CWiFiTestDriver::ConnectAction(mkit_handle_t caller, int op, int ap_id,
                                   sys_wifi_nw_auth_t * auth, int timeout)
{
    if (ENABLE != m_bAdapterEnable || _WIFITestData.GetWifiSwitchStatus())
    {
        return -1;
    }

    if (SYS_WIFI_OP_CONNECT_START == op)
    {
        if (!_WirelessSessionManager.InitSession(SESSION_CONNECTING_STATE,
                WIFI_TEST_SESSION, ap_id, (void *)auth))
        {
            return -1;
        }

        WirelessSessionData wsData;
        wsData.ulMessage = WIRELESS_OPTION_WIFI_ANSWERPSK;
        wsData.wParam = ap_id;
        wsData.pData = (void *)auth->psk;

        WIRE_INFO("TestDriver---->Send option to session by handle[%d]", ap_id);
        int iResult = _WirelessSessionManager.SendOptionToSessionByHandle(ap_id, wsData, WIFI_TEST_SESSION);

        WIRE_INFO("TestDriver---->ConnectAction result[%d]", iResult ? 0 : 1);
        return iResult ? 0 : 1;
    }
    else if (SYS_WIFI_OP_CONNECT_STOP == op)
    {
        return _WirelessSessionManager.DestorySessionByHandle(ap_id, WIFI_TEST_SESSION, true) ? 0 : -1;
    }

    return -1;
}

int CWiFiTestDriver::ScanAction(mkit_handle_t caller, int op, int timeout)
{
    if (ENABLE != m_bAdapterEnable || _WIFITestData.GetWifiSwitchStatus())
    {
        return -1;
    }

    if (SYS_WIFI_OP_SCAN_START == op)
    {
        int iResult = _WirelessSessionManager.InitSession(SESSION_SCAN_STATE, WIFI_TEST_SESSION);

        etl_NotifyApp(false, SYS_MSG_WIFI_SCAN_RESULT, 0, !iResult);

        WIRE_INFO("TestDriver---->Post Msg to wifimanage:SYS_MSG_WIFI_SCAN_RESULT[%d],wParam[%d],lParam[%d]",
                  SYS_MSG_WIFI_SCAN_RESULT, 0, !iResult);
        return iResult ? 0 : -1;
    }
    else if (SYS_WIFI_OP_SCAN_STOP == op)
    {
        int iResult = _WirelessSessionManager.DestorySessionByState(SESSION_SCAN_STATE, WIFI_TEST_SESSION,
                      true);
        return iResult ? 0 : -1;
    }

    return -1;
}

int CWiFiTestDriver::WpsAction(mkit_handle_t caller, int op, int wps_type, int timeout)
{
    WIRELESS_SESSION_STATE eState;

    if (ENABLE != m_bAdapterEnable || _WIFITestData.GetWifiSwitchStatus())
    {
        return -1;
    }

    if (SYS_WIFI_WPS_TYPE_PBC == wps_type)
    {
        eState = SESSION_WPS_STATE;
    }
    else if (SYS_WIFI_WPS_TYPE_PIN == wps_type)
    {
        eState = SESSION_WPSPIN_STATE;
    }

    if (SYS_WIFI_OP_WPS_START == op)
    {
        _WirelessSessionManager.DestorySessionByState(SESSION_SCAN_STATE, WIFI_TEST_SESSION, true);
        _WirelessSessionManager.DestorySessionByState(SESSION_PAIRING_STATE, WIFI_TEST_SESSION, true);
        _WirelessSessionManager.DestorySessionByState(SESSION_CONNECTING_STATE, WIFI_TEST_SESSION, true);
        _WirelessSessionManager.DestorySessionByState(SESSION_CONNECTED_STATE, WIFI_TEST_SESSION, true);

        _WirelessSessionManager.InitSession(eState, WIFI_TEST_SESSION, 55);
    }
    else if (SYS_WIFI_OP_SCAN_STOP == op)
    {
        if (SYS_WIFI_WPS_TYPE_PBC == wps_type ||
                SYS_WIFI_WPS_TYPE_PIN == wps_type)
        {
            _WirelessSessionManager.DestorySessionByState(eState, WIFI_TEST_SESSION, true);
        }
        else
        {
            _WirelessSessionManager.DestorySessionByState(SESSION_WPS_STATE, WIFI_TEST_SESSION, true);
            _WirelessSessionManager.DestorySessionByState(SESSION_WPSPIN_STATE, WIFI_TEST_SESSION, true);
        }
    }
    else if (SYS_WIFI_OP_WPS_GET_PIN == op)
    {
        //
    }

    return 0;
}

int CWiFiTestDriver::GetPortStatus()
{
    return m_bAdapterEnable == ENABLE ? 1 : -1;
}

int CWiFiTestDriver::GetNetworkStatus(sys_wifi_nw_stat_t * stat, int buf_len)
{
    if (ENABLE != m_bAdapterEnable || _WIFITestData.GetWifiSwitchStatus())
    {
        return -1;
    }

    return _WIFITestData.GetNetworkStatus(stat, buf_len) ? 0 : 1;
}

int CWiFiTestDriver::GetAPList(sys_wifi_ap_t * ap_list, int buf_len)
{
    if (ENABLE != m_bAdapterEnable || _WIFITestData.GetWifiSwitchStatus())
    {
        return -1;
    }

    return _WIFITestData.GetAPList(ap_list, buf_len) ? 0 : -1;
}

int CWiFiTestDriver::GetMacString(int phyPort, char * buff, int buff_max)
{
    if (phyPort == SYS_PHY_TYPE_WIFI)
    {
        return -1;
    }

    return sys_get_MAC_string(phyPort, buff, buff_max);
}


//---------------------------------------------------------------------------
// Simulation of UI
//---------------------------------------------------------------------------
BOOL CWiFiTestDriver::OnMsg(msgObject & msgObj)
{
    bool bHandle = true;

    switch (msgObj.message)
    {
    case WIFI_MSG_TO_TESTDRIVER:
        bHandle = ProcessTestThreadMsg(msgObj);
    default:
        break;
    }

    return bHandle;
}

BOOL CWiFiTestDriver::ProcessTestThreadMsg(msgObject & objMessage)
{
    WIRE_INFO("Test Driver---->API test. msg[%d]wparam[%d]lparam[%d]", objMessage.message,
              objMessage.wParam, objMessage.lParam);

    int iChoise = objMessage.wParam;
    int iParam;

    yl::string strParam;
    if (1 == iChoise)   //开启Wi-Fi
    {
        printf("0 Enable,otherwise Disable\n");
        GetInt(iParam);
        printf("iParam = %d\n", iParam);
        WIFI_SetEnable(iParam == 0);
    }
    else if (2 == iChoise)  //Scan
    {
        printf("0 Start,otherwise Stop\n");
        GetInt(iParam);
        if (iParam == 0)
        {
            WIFI_StartScan();
        }
        else
        {
            WIFI_StopScan();
        }
    }
    else if (3 == iChoise) //connect
    {
        printf("endter netid\n");
        GetInt(iParam);
        WIFI_StartConnect(iParam);
    }
    else if (4 == iChoise) // cancel connect
    {
        printf("endter netid\n");
        GetInt(iParam);
        WIFI_CancelConnect(iParam);
    }
    else if (5 == iChoise) // answer pin
    {
        printf("endter netid\n");
        GetInt(iParam);

        printf("enter psk\n");
        GetString(strParam);

        WIFI_AnswerPassword(iParam, strParam.c_str());
    }
    else if (6 == iChoise) //disconnect
    {
        printf("endter netid\n");
        GetInt(iParam);
        WIFI_StartDisconnect(iParam);
    }
    else if (7 == iChoise) //add network
    {
        //std::cout << "" << std::endl;
    }
    else if (8 == iChoise) // adjust postion
    {
        UI_WIFI_NETINFO_LIST listWiFi;
        WIFI_GetSavedNetworkList(listWiFi);
        PrintfSavedList(listWiFi);

        printf("endter netid\n");
        GetInt(iParam);
        printf("0:up, otherwise down\n");
        int iDown;
        GetInt(iDown);
        WIFI_AdjustPosition(iParam, iDown == 0);

        WIFI_GetSavedNetworkList(listWiFi);
        PrintfSavedList(listWiFi);
    }
    else if (9 == iChoise) //move position
    {
        UI_WIFI_NETINFO_LIST listWiFi;
        WIFI_GetSavedNetworkList(listWiFi);
        PrintfSavedList(listWiFi);

        printf("enter netid\n");
        GetInt(iParam);
        printf("enter position\n");
        int iDown;
        GetInt(iDown);
        WIFI_MovePosition(iParam, iDown);

        WIFI_GetSavedNetworkList(listWiFi);
        PrintfSavedList(listWiFi);
    }
    else if (10 == iChoise) //delete net
    {
        UI_WIFI_NETINFO_LIST listWiFi;
        WIFI_GetSavedNetworkList(listWiFi);
        PrintfSavedList(listWiFi);

        printf("enter netid\n");
        GetInt(iParam);

        WIFI_DeleteNetwork(iParam);

        WIFI_GetSavedNetworkList(listWiFi);
        PrintfSavedList(listWiFi);
    }
    else if (11 == iChoise)  //printf network list
    {
        UI_WIFI_NETINFO_LIST listWiFi;
        WIFI_GetScanNetworkList(listWiFi);
        PrintfScanNetInfo(listWiFi);

        WIFI_RefreshData();

        WIFI_GetScanNetworkList(listWiFi);
        PrintfScanNetInfo(listWiFi);
        listWiFi.clear();
        WIFI_GetSavedNetworkList(listWiFi);
        PrintfSavedList(listWiFi);
    }
    else if (12 == iChoise)
    {
        configParser_SetCfgItemStringValue(kszAccountServerAddr, 2, 1, "ccccc");
        yl::string strValue = configParser_GetCfgItemStringValue("account.%d.sip_server.%d.address", 1, 1);
        WIRE_INFO("strValue[%s]", strValue.c_str());
    }
    else if (13 == iChoise) //add new network
    {
        WiFiNetInfo netInfo;
        GetBaseNetInfo(netInfo);
        WIFI_AddNetwork(&netInfo);

        UI_WIFI_NETINFO_LIST listWiFi;
        WIFI_GetSavedNetworkList(listWiFi);
        PrintfSavedList(listWiFi);
    }
    else if (14 == iChoise) // modify network
    {
        int iNetID;
        UI_WIFI_NETINFO_LIST listWiFi;
        WIFI_GetSavedNetworkList(listWiFi);
        PrintfSavedList(listWiFi);

        WIRE_INFO("Enter NetID:\n");
        GetInt(iNetID);
        WiFiNetInfo netInfo;
        GetBaseNetInfo(netInfo);

        WIFI_ModifyNetwork(iNetID, &netInfo);

        listWiFi.clear();
        WIFI_GetSavedNetworkList(listWiFi);
        PrintfSavedList(listWiFi);
    }
    else if (15 == iChoise) //add exist network
    {
        int iNetID;
        UI_WIFI_NETINFO_LIST listWiFi;
        WIFI_GetScanNetworkList(listWiFi);
        PrintfScanNetInfo(listWiFi);
        WIRE_INFO("Enter NetID:\n");
        GetInt(iNetID);

        WiFiNetInfo netInfo;
        if (WIFI_GetNetworkInfoByID(iNetID, netInfo))
        {
            if (netInfo.iSercureMode > 0)
            {
                WIRE_INFO("Enter psk\t\
						  						  						   'empty'=''\n");
                yl::string strPsk;
                GetString(strPsk);
                if (strPsk == "empty")
                {
                    strPsk.clear();
                }
                netInfo.strPsk = strPsk.c_str();
            }

            WIFI_AddNetwork(&netInfo);

            listWiFi.clear();
            WIFI_GetSavedNetworkList(listWiFi);
            PrintfSavedList(listWiFi);
        }
    }
    else if (16 == iChoise) //start wps
    {
        WIFI_StartWps();
    }
    else if (17 == iChoise) // start wps-pin
    {
        yl::string strPin;
        WIFI_GetWpsPin(strPin);
        WIRE_INFO("PIN[%d]", strPin.c_str());
        etlSleep(5 * 1000);

        WIFI_StartWpspin();
    }
    else if (18 == iChoise) //cancel wps/wps-pin
    {
        WIFI_CancelWps();
    }
    else if (19 == iChoise)
    {
        WIRE_INFO("suck yourself");
    }
    else if (20 == iChoise)
    {
        WIFI_StartConnect(objMessage.lParam);
    }
    else if (21 == iChoise)
    {
        //WIFI_StartConnect(objMessage.lParam, true);
    }
    else if (22 == iChoise)
    {
        WIFI_StartScan();
    }
    else if (23 == iChoise)
    {
        WIFI_StopScan();
    }
    else if (24 == iChoise)
    {
        WIFI_StartDisconnect(objMessage.lParam);
    }
    else if (25 == iChoise)
    {
        WIFI_CancelConnect(objMessage.lParam);
    }
    else if (26 == iChoise)
    {
        WIFI_DeleteNetwork(objMessage.lParam);
    }
    else
    {
        WIRE_INFO("\
				  1: WIFI_SetEnable()\n \
				  2: scan \n \
				  3: connect \n \
				  4: cancel connect \n \
				  5: answer psk \n \
				  6: disconnect \n \
				  7: add network \n \
				  8: adjust position \n \
				  9: move position \n \
				  10 :delete network \n \
				  11 :refresh data\n \
				  12 :test \n\
				  13 : add new network \n\
				  14 : modify network \n\
				  15 : add exist network \n\
				  16 : start wps \n\
				  17 : start wpspin \n\
				  18 : stop wps \n\
				  19 : suck yourself \n\
				  20 : connect lparam=net id\n\
				  21 : connect(enter psk = false) lparam=net id\n\
				  22 : start scan \n\
				  23 : stop scan \n\
				  24 : disconnect network lparam=net id\n\
				  25 : delete network lparam=net id \n\
				  ");
    }
}

void CWiFiTestDriver::GetInt(int & iNumber)
{
    int iInput;
    scanf("%d", &iInput);
    iNumber = iInput;
    printf("%d\n", iNumber);
}

void CWiFiTestDriver::GetString(yl::string & strParam)
{
    char szInput[1024] = { 0 };
    scanf("%s", szInput);
    strParam = szInput;
    printf("%s\n", strParam.c_str());
}

void CWiFiTestDriver::_outputSaveNetworkInfo(WiFiNetInfo & wifiInfo)
{
    WIRE_INFO("saved net[%d]:profilename[%s]:ssid[%s]:encrypt[%d]:sercure[%d]:priority[%d]:psk[%s]\n"
              , wifiInfo.iNetID
              , wifiInfo.strProfileName.c_str()
              , wifiInfo.strSSID.c_str()
              , wifiInfo.iEncrypt
              , wifiInfo.iSercureMode
              , wifiInfo.iPriority
              , wifiInfo.strPsk.c_str()
             );
}

void CWiFiTestDriver::PrintfSavedList(UI_WIFI_NETINFO_LIST & listWiFi)
{
    for (WiFiNetInfoItem itBeg = listWiFi.begin();
            itBeg != listWiFi.end();
            ++itBeg)
    {
        _outputSaveNetworkInfo((*itBeg));
    }
}

void CWiFiTestDriver::_outputScanNetInfo(WiFiNetInfo & wifiInfo)
{
    WIRE_INFO("scan net[%d]:ssid[%s]: profilename[%s]:mac[%s]:encrypt[%d]:sercure[%d]:signal[%d]:freq[%d]:channel[%d]:state[%d]\n"
              , wifiInfo.iNetID
              , wifiInfo.strSSID.c_str()
              , wifiInfo.strProfileName.c_str()
              , wifiInfo.strMac.c_str()
              , wifiInfo.iEncrypt
              , wifiInfo.iSercureMode
              , wifiInfo.iSignal
              , wifiInfo.iFrequence
              , wifiInfo.iChannel
              , wifiInfo.eNetState
             );
}

void CWiFiTestDriver::PrintfScanNetInfo(UI_WIFI_NETINFO_LIST & listWiFi)
{
    for (WiFiNetInfoItem itBeg = listWiFi.begin();
            itBeg != listWiFi.end();
            ++itBeg)
    {
        _outputScanNetInfo((*itBeg));
    }
}

void CWiFiTestDriver::GetBaseNetInfo(WiFiNetInfo & netInfo)
{
    WIRE_INFO("Enter SSID\t\
			  			  			  'empty' = '' \n");

    yl::string strName;
    GetString(strName);

    if (strName == "empty")
    {
        strName.clear();
    }
    netInfo.strSSID = strName.c_str();

    int iParam = 0;
    WIRE_INFO("Enter Secure Mode\n\
			  			  			  0: NONE \n\
									  						  			  1: WEP\n\
																		  									  			  4: SERCURE_WPA_PSK \n\
																														  												  			  5: SERCURE_WPA_PSK2 \n");

    GetInt(iParam);
    netInfo.iSercureMode = (WIFI_SERCURE_MODE)iParam;

    WIRE_INFO("Enter Secure Mode\n\
			  			  			  0: CIPHER_NONE \n\
									  						  			  3: CIPHER_TKIP\n\
																		  									  			  4: CIPHER_CCMP \n\
																														  												  			  5: CIPHER_AUTO \n");
    GetInt(iParam);
    netInfo.iEncrypt = (WIFI_ENCRYPT_TYPE)iParam;

    WIRE_INFO("Enter psk\t\
			  			  			  'empty' = '' \n");

    GetString(strName);
    if (strName == "empty")
    {
        strName.clear();
    }
    netInfo.strPsk = strName.c_str();

    WIRE_INFO("Enter ProfileName\t\
			  			  			  'empty' = '' \n");
    GetString(strName);
    if (strName == "empty")
    {
        strName.clear();
    }
    netInfo.strProfileName = strName.c_str();
}

#endif // IF_WIFI_SIMULATION_DRIVER
