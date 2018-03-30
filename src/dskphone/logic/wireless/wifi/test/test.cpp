#ifdef IF_WIFI_TEST_DRIVER
#include "test.h"

IMPLEMENT_GETINSTANCE(MyWiFiTestThread)


void MyWiFiTestThread::GetBaseNetInfo(WiFiNetInfo & netInfo)
{
    yl::string strName = "Wifi_Test1";

    if (strName == "empty")
    {
        strName.clear();
    }
    netInfo.strSSID = strName.c_str();

    netInfo.iSercureMode = SERCURE_WPA2_PSK;
    netInfo.iEncrypt = CIPHER_CCMP;

    yl::string strPsk = "12345678";
    if (strPsk == "empty")
    {
        strPsk.clear();
    }
    netInfo.strPsk = strPsk.c_str();

    strName = "Wifi_Test1";
    if (strName == "empty")
    {
        strName.clear();
    }
    netInfo.strProfileName = strName.c_str();
}

MyWiFiTestThread::MyWiFiTestThread()
{
    srand((unsigned)time(NULL));

    WIRE_INFO("WIFI TEST MODE[%d]", WIFI_MSG_TEST_THREAD);
}

MyWiFiTestThread::~MyWiFiTestThread()
{

}

BOOL MyWiFiTestThread::OnMessage(msgObject & objMessage)
{
    if (objMessage.message != WIFI_MSG_TEST_THREAD)
    {
        return FALSE;
    }

    WIRE_INFO("test thread msg[%d]wparam[%d]lparam[%d]", objMessage.message, objMessage.wParam,
              objMessage.lParam);

    int iChoise = objMessage.wParam;
    int iParam;
    int iRst = 0;
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
        WIFI_StartConnect(objMessage.lParam);
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
    else if (30 == iChoise)
    {
        if (0 == objMessage.lParam)
        {
            /*
            WIRE_INFO("test---->First time to check dongle. lParam[%d]\n", objMessage.lParam);
            iRst = WIFI_IsDongleVaild(true);
            WIRE_INFO("test---->Dongle is %s\n", iRst ? "vaild" : "unvaild");
            */
        }
        else
        {
            WIRE_INFO("test---->Check dongle. lParam[%d]\n", objMessage.lParam);
            iRst = WIFI_IsDongleVaild();
            WIRE_INFO("test---->Dongle is %s\n", iRst ? "vaild" : "unvaild");
        }
    }
    else if (31 == iChoise)
    {
        WIRE_INFO("test---->Check is enable.\n");
        iRst = WIFI_IsEnable();
        WIRE_INFO("test---->Wifi is %s\n", iRst ? "enable" : "disable");
    }
    else if (32 == iChoise)
    {
        WIRE_INFO("test---->Check is cable connected.\n");
        iRst = WIFI_IsCableConnected();
        WIRE_INFO("test---->Cable is %s\n", iRst ? "connected" : "disconnected");
    }
    else if (33 == iChoise)
    {
        WIRE_INFO("test---->check is scaning.\n");
        iRst = WIFI_IsScan();
        WIRE_INFO("test---->Wifi is in scaning[%s]\n", iRst ? "yes" : "no");
    }
    else if (34 == iChoise)
    {
        WIRE_INFO("test---->check is connecting.\n");
        iRst = WIFI_IsConnecting();
        WIRE_INFO("test---->Wifi is in connecting[%s]\n", iRst ? "yes" : "no");

    }
    else if (35 == iChoise)
    {
        WIRE_INFO("test---->check is connected.");
        iRst = WIFI_IsConnected();
        WIRE_INFO("test---->Wifi is in connected[%s]\n", iRst ? "yes" : "no");
    }
    else if (36 == iChoise)
    {
        WIRE_INFO("test-->Wifi set enable from idle. lParam[%d],0 Enable,otherwise Disable\n",
                  objMessage.lParam);
        WIFI_SetEnable(objMessage.lParam == 0, 1);
    }
    else if (37 == iChoise)
    {
        WIRE_INFO("test-->Wifi set enable from menu. lParam = [%d],0 Enable,otherwise Disable\n",
                  objMessage.lParam);
        WIFI_SetEnable(objMessage.lParam == 0);
    }
    else if (38 == iChoise)
    {
        WIRE_INFO("test-->lParam[%d], 0 Start scan,otherwise Stop scan\n", objMessage.lParam);
        if (objMessage.lParam == 0)
        {
            WIFI_StartScan();
        }
        else
        {
            WIFI_StopScan();
        }
    }
    else if (39 == iChoise) // connect
    {
        WIRE_INFO("test-->lParam[%d], connect netid[%d]\n", objMessage.lParam, objMessage.lParam);
        WIFI_StartConnect(objMessage.lParam);
    }
    else if (40 == iChoise) // cancle connect
    {
        WIRE_INFO("test-->lParam[%d], cancel connect netid[%d]\n", objMessage.lParam, objMessage.lParam);
        WIFI_CancelConnect(objMessage.lParam);
    }
    else if (41 == iChoise) // answer right psk
    {
        yl::string strPsk = "12345678";
        WIRE_INFO("test-->lParam[%d], answer psk netid[%d]\n", objMessage.lParam, objMessage.lParam);

        WIFI_AnswerPassword(objMessage.lParam, strPsk.c_str());
    }
    else if (42 == iChoise) // answer error psk
    {
        yl::string strPsk = "00000000";
        WIRE_INFO("test-->lParam[%d], answer psk netid[%d]\n", objMessage.lParam, objMessage.lParam);

        WIFI_AnswerPassword(objMessage.lParam, strPsk.c_str());
    }
    else if (43 == iChoise) // disconnect
    {
        WIRE_INFO("test-->lParam[%d], disconnect netid[%d]\n", objMessage.lParam, objMessage.lParam);
        WIFI_StartDisconnect(objMessage.lParam);
    }
    else if (44 == iChoise) //add network
    {
        //std::cout << "" << std::endl;
    }
    else if (45 == iChoise) // postion up
    {
        UI_WIFI_NETINFO_LIST listWiFi;
        WIFI_GetSavedNetworkList(listWiFi);
        PrintfSavedList(listWiFi);

        WIRE_INFO("test-->lParam[%d],postion up ,netid[%d]\n", objMessage.lParam, objMessage.lParam);

        WIFI_AdjustPosition(objMessage.lParam, 0);

        WIFI_GetSavedNetworkList(listWiFi);
        PrintfSavedList(listWiFi);
    }
    else if (46 == iChoise) // postion down
    {
        UI_WIFI_NETINFO_LIST listWiFi;
        WIFI_GetSavedNetworkList(listWiFi);
        PrintfSavedList(listWiFi);

        WIRE_INFO("test-->lParam[%d], postion down, netid[%d]\n", objMessage.lParam, objMessage.lParam);

        WIFI_AdjustPosition(objMessage.lParam, 1);

        WIFI_GetSavedNetworkList(listWiFi);
        PrintfSavedList(listWiFi);
    }
    else if (47 == iChoise) // move position
    {
        WIRE_INFO("test-->lParam[%d], move postion, netid[%d]", objMessage.lParam, objMessage.lParam);

        UI_WIFI_NETINFO_LIST listWiFi;
        WIFI_GetSavedNetworkList(listWiFi);
        PrintfSavedList(listWiFi);

        WIFI_MovePosition(objMessage.lParam, GetRandomNum());

        WIFI_GetSavedNetworkList(listWiFi);
        PrintfSavedList(listWiFi);
    }
    else if (48 == iChoise) // delete net
    {
        UI_WIFI_NETINFO_LIST listWiFi;
        WIFI_GetSavedNetworkList(listWiFi);
        PrintfSavedList(listWiFi);

        printf("test-->lParam[%d], delete netid[%d]\n", objMessage.lParam, objMessage.lParam);

        WIFI_DeleteNetwork(objMessage.lParam);

        WIFI_GetSavedNetworkList(listWiFi);
        PrintfSavedList(listWiFi);
    }
    else if (49 == iChoise) // modify currently connected network
    {
        WIRE_INFO("test-->lParam[%d], modify currently connected network\n", objMessage.lParam);

        UI_WIFI_NETINFO_LIST listWiFi;
        WIFI_GetSavedNetworkList(listWiFi);
        PrintfSavedList(listWiFi);

        WiFiNetInfo netInfo;
#if 0
        WIFI_GetCurrentNetConnectInfo(netInfo);
        if (0 == objMessage.lParam) // reset wifi infomation
        {
            GetBaseNetInfo(netInfo);

            WIFI_ModifyNetwork(netInfo.iNetID, &netInfo);
        }
        else if (1 == objMessage.lParam) // modify SSID
        {
            WIRE_INFO("test-->modify network SSID");

            netInfo.strSSID = "NewSSID";
            WIFI_ModifyNetwork(netInfo.iNetID, &netInfo);
        }
        else if (2 == objMessage.lParam) // modify profile name
        {
            WIRE_INFO("test-->modify network profile name");

            netInfo.strProfileName = "NewProfileName";
            WIFI_ModifyNetwork(netInfo.iNetID, &netInfo);
        }
        else if (3 == objMessage.lParam) // modify secure mode
        {
            WIRE_INFO("test-->modify network secure mode");

            netInfo.iSercureMode = SERCURE_WPA;
            WIFI_ModifyNetwork(netInfo.iNetID, &netInfo);
        }
        else if (4 == objMessage.lParam) // modify encrypt
        {
            WIRE_INFO("test-->modify network encrypt mode");

            netInfo.iEncrypt = CIPHER_TKIP;
            WIFI_ModifyNetwork(netInfo.iNetID, &netInfo);
        }
        else if (5 == objMessage.lParam) // modify psk
        {
            WIRE_INFO("test-->modify network psk");

            netInfo.strPsk = "00005555";
            WIFI_ModifyNetwork(netInfo.iNetID, &netInfo);
        }
#endif

        WIFI_GetCurrentNetConnectInfo(netInfo);
        netInfo.strSSID = "Wifi_Yealink";
        netInfo.strProfileName = "Wifi_Yealink";
        netInfo.iSercureMode = SERCURE_WEP;
        netInfo.iEncrypt = CIPHER_TKIP;
        netInfo.strPsk = "00005555";

        WIFI_ModifyNetwork(objMessage.lParam, &netInfo);

        listWiFi.clear();
        WIFI_GetSavedNetworkList(listWiFi);
        PrintfSavedList(listWiFi);
    }
    else if (50 == iChoise) // modify network info that not currently connected
    {
        WIRE_INFO("test-->lParam[%d], modify network's SSID that not currently connected\n",
                  objMessage.lParam);

        UI_WIFI_NETINFO_LIST listWiFi;
        WIFI_GetSavedNetworkList(listWiFi);
        PrintfSavedList(listWiFi);

        WiFiNetInfo netInfo;

        netInfo.strSSID = "Yealink__T";
        netInfo.strProfileName = "Yealink__T";
        netInfo.iSercureMode = SERCURE_NONE;
        netInfo.iEncrypt = CIPHER_CCMP;
        netInfo.strPsk = "98765432";
        WIFI_ModifyNetwork(objMessage.lParam, &netInfo);

        listWiFi.clear();
        WIFI_GetSavedNetworkList(listWiFi);
        PrintfSavedList(listWiFi);
    }
    else if (51 == iChoise) // modify network's SSID and profile name the same as profile

    {
        WIRE_INFO("test-->lParam[%d], modify network's SSID and profile name the same as profile\n",
                  objMessage.lParam);

        UI_WIFI_NETINFO_LIST listWiFi;
        WIFI_GetSavedNetworkList(listWiFi);
        PrintfSavedList(listWiFi);

        WiFiNetInfo netInfo;

        netInfo.strSSID = "Wifi_Test";
        netInfo.strProfileName = "Wifi_Test";
        netInfo.iSercureMode = SERCURE_WPA2_PSK;
        netInfo.iEncrypt = CIPHER_CCMP;
        netInfo.strPsk = "12345678";
        WIFI_ModifyNetwork(objMessage.lParam, &netInfo);

        listWiFi.clear();
        WIFI_GetSavedNetworkList(listWiFi);
        PrintfSavedList(listWiFi);
    }
    else if (52 == iChoise) // modify open network's psk
    {

    }
    else if (53 == iChoise) // add exist network of scan list to profile
    {
        int iNetID;
        UI_WIFI_NETINFO_LIST listWiFi;
        WIFI_GetScanNetworkList(listWiFi);
        PrintfScanNetInfo(listWiFi);
        WIRE_INFO("test-->lParam[%d], add exist network", objMessage.lParam);

        WiFiNetInfo netInfo;
        if (WIFI_GetNetworkInfoByID(objMessage.lParam, netInfo))
        {
            if (netInfo.iSercureMode > 0)
            {
                yl::string strPsk = "87654321";
                netInfo.strPsk = strPsk.c_str();
            }

            WIFI_AddNetwork(&netInfo);

            listWiFi.clear();
            WIFI_GetSavedNetworkList(listWiFi);
            PrintfSavedList(listWiFi);
        }
    }
    else if (54 == iChoise) // add network to profile
    {
        WIRE_INFO("test-->lParam[%d],add a network.", objMessage.lParam);
        UI_WIFI_NETINFO_LIST listWiFi;
        WiFiNetInfo netInfo;

        netInfo.strSSID = "NewSSID";
        netInfo.strProfileName = "NewProfileName";
        netInfo.iSercureMode = SERCURE_WEP;
        netInfo.iEncrypt = CIPHER_NONE;
        netInfo.strPsk = "12345678";

        if (0 == objMessage.lParam) // add network if SSID existed
        {
            WIRE_INFO("test-->add network if SSID existed");

            netInfo.strSSID = "Wifi_Test";
            WIFI_AddNetwork(&netInfo);
        }
        else if (1 == objMessage.lParam) // add network if profile name existed
        {
            WIRE_INFO("test-->add network if profile name existed");

            netInfo.strProfileName = "Wifi_Test";
            WIFI_AddNetwork(&netInfo);
        }
        else if (2 == objMessage.lParam) // add network if SSID is empty
        {
            WIRE_INFO("test-->add network if SSID is empty");

            netInfo.strSSID = "";
            WIFI_AddNetwork(&netInfo);
        }
        else if (3 == objMessage.lParam) // add network if profile name is empty
        {
            WIRE_INFO("test-->add network if profile name is empty");

            netInfo.strProfileName = "";
            WIFI_AddNetwork(&netInfo);
        }
        else if (4 == objMessage.lParam) // add network if SSID and profile name are empty
        {
            WIRE_INFO("test-->add network if SSID and profile name are empty");

            netInfo.strSSID = "";
            netInfo.strProfileName = "";
            WIFI_AddNetwork(&netInfo);
        }
        else if (5 == objMessage.lParam) // add encrypted network, psk is empty
        {
            WIRE_INFO("test-->add encrypted network and psk is empty");

            netInfo.strSSID = "Wifi_Test1";
            netInfo.strProfileName = "Wifi_Test1";
            netInfo.strPsk = "";

            WIFI_AddNetwork(&netInfo);
        }
        else if (6 == objMessage.lParam) // add open network, psk is not empty
        {
            WIRE_INFO("test-->add open network and psk is not empty");

            netInfo.strSSID = "Wifi_Test1";
            netInfo.strProfileName = "Wifi_Test1";
            netInfo.strPsk = "12345678";

            WIFI_AddNetwork(&netInfo);
        }
        else if (7 == objMessage.lParam) // add hidden network
        {
            WIRE_INFO("test-->add hidden network.");

            netInfo.strSSID = "Wifi_Test";
            netInfo.strProfileName = "Wifi_Test";
            netInfo.iSercureMode = SERCURE_WPA2_PSK;
            netInfo.iEncrypt = CIPHER_CCMP;
            netInfo.strPsk = "12345678";

            WIFI_AddNetwork(&netInfo);
        }
        listWiFi.clear();
        WIFI_GetSavedNetworkList(listWiFi);
        PrintfSavedList(listWiFi);
    }
    else if (55 == iChoise) // get current net info
    {
        WIRE_INFO("test-->lParam[%d], get current net info.", objMessage.lParam);

        WiFiNetInfo netInfo;
        iRst = WIFI_GetCurrentNetConnectInfo(netInfo);

        if (iRst != false)
        {
            WIRE_INFO("currnt net[%d]:profilename[%s]:ssid[%s]:encrypt[%d]:sercure[%d]:priority[%d]:psk[%s]\n"
                      , netInfo.iNetID
                      , netInfo.strProfileName.c_str()
                      , netInfo.strSSID.c_str()
                      , netInfo.iEncrypt
                      , netInfo.iSercureMode
                      , netInfo.iPriority
                      , netInfo.strPsk.c_str()
                     );
        }
    }
    else if (56 == iChoise) // start wps-pin
    {
        WIRE_INFO("test-->lParam[%d],start wps-pin.", objMessage.lParam);

        yl::string strPin;
        WIFI_GetWpsPin(strPin);
        WIRE_INFO("test---->PIN[%s]", strPin.c_str());
        etlSleep(5 * 1000);

        WIFI_StartWpspin();
    }
    else if (57 == iChoise) //start wps
    {
        WIRE_INFO("test-->lParam[%d],start wps-pin.", objMessage.lParam);

        WIFI_StartWps();
    }
    else if (58 == iChoise) //cancel wps/wps-pin
    {
        WIRE_INFO("test-->lParam[%d],cancel wps/wps-pin.", objMessage.lParam);

        WIFI_CancelWps();
    }
    else if (59 == iChoise)
    {
        WIRE_INFO("test-->lParam[%d],modify network info.", objMessage.lParam);


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
				30 : check dongle\n\
				31 : check is enable\n\
				32 : check cable is connected \n\
				33 : check is scaning \n\
				34 : check is connecting \n\
				35 : check is connected \n\
				36 : set enable/disable from idle \n\
				37 : set enable/disable from menu \n\
				38 : scan \n\
				39 : start connect \n\
				40 : cancel connect \n\
				41 : answer right psk \n\
				42 : answer wrong psk \n\
				43 : disconnct \n\
				44 : add network \n\
				45 : position up \n\
				46 : position down \n\
				");
    }

    return TRUE;
}

void MyWiFiTestThread::GetInt(int & iNumber)
{
    int iInput;
    scanf("%d", &iInput);
    iNumber = iInput;
    printf("%d\n", iNumber);
}

void MyWiFiTestThread::GetString(yl::string & strParam)
{
    char szInput[1024] = {0};
    scanf("%s", szInput);
    strParam = szInput;
    printf("%s\n", strParam.c_str());
}

int MyWiFiTestThread::GetRandomNum(void)
{
    return ((rand() % 3) + 1);
}

void MyWiFiTestThread::_outputSaveNetworkInfo(WiFiNetInfo & wifiInfo)
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

void MyWiFiTestThread::PrintfSavedList(UI_WIFI_NETINFO_LIST & listWiFi)
{
    for (WiFiNetInfoItem itBeg = listWiFi.begin();
            itBeg != listWiFi.end();
            ++itBeg)
    {
        _outputSaveNetworkInfo((*itBeg));
    }
}

void MyWiFiTestThread::_outputScanNetInfo(WiFiNetInfo & wifiInfo)
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

void MyWiFiTestThread::PrintfScanNetInfo(UI_WIFI_NETINFO_LIST & listWiFi)
{
    for (WiFiNetInfoItem itBeg = listWiFi.begin();
            itBeg != listWiFi.end();
            ++itBeg)
    {
        _outputScanNetInfo((*itBeg));
    }
}

#endif // IF_WIFI_TEST_DRIVER
