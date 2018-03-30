#ifdef IF_SUPPORT_WIFI
///////////////////////////////////////////////////////////
//  CWifiManager.cpp
//  Implementation of the Class CWifiManager
//  Created on:      15-五月-2014 9:17:48
//  Original author: songk
///////////////////////////////////////////////////////////
#include "wifi_inc.h"

#if IF_WIFI_GLOBAL_OFF
static bool g_bEnterTestMode = false;
#endif // WIFI_GLOBAL_OFF

#ifdef IF_WIFI_TEST_DRIVER
#include "../test/test.h"
#endif

IMPLEMENT_GETINSTANCE(CWiFiManager)
CWiFiManager::CWiFiManager()
    : m_iWpsTimer((UINT) & m_iWpsTimer),
      m_iAutoConnectTimer((UINT) & m_iAutoConnectTimer)
{
    m_bEnable = false;//(configParser_GetConfigInt(kszWifiSwitch) == 1);
}

CWiFiManager::~CWiFiManager()
{

}

yl::string GetWiFiMsgName(UINT uMsg)
{
    switch (uMsg)
    {
        //_MYCASE(TM_TIMER);
        _MYCASE(CONFIG_MSG_BCAST_CHANGED);
        _MYCASE(MSG_WAN_CABLE_CONNECTED);
        _MYCASE(MSG_WAN_CABLE_DISCONNECT);
        _MYCASE(SYS_MSG_WIFI_SCAN_RESULT);
        _MYCASE(SYS_MSG_WIFI_CONNECT_STATUS);
        _MYCASE(SYS_MSG_WIFI_WPS_RESULT);
        _MYCASE(SYS_MSG_WIFI_WPS_PBC_CATCH);
        _MYCASE(WIFI_MSG_AUTHORIZE);
        _MYCASE(WIFI_MSG_TEST);
        _MYCASE(WIFI_MSG_GETSTATUS);
    default:
        break;
    }
    return "";
}

yl::string GetWiFiConnectStatusName(UINT uStatus)
{
    switch (uStatus)
    {
        _MYCASE(WIFI_CALLBACK_EVENT_CONNECTED);
        _MYCASE(WIFI_CALLBACK_EVENT_DISCONNECTED);
        _MYCASE(WIFI_CALLBACK_EVENT_CONNECTTING);
        _MYCASE(WIFI_CALLBACK_EVENT_AUTHFAILURE);
        _MYCASE(WIFI_CALLBACK_EVENT_CONNECTFAILED);
        _MYCASE(WIFI_CALLBACK_EVENT_DONGLE_REMOVED);
        _MYCASE(WIFI_CALLBACK_EVENT_DONGLE_ADDED);
        _MYCASE(WIFI_CALLBACK_EVENT_INACTIVE);
        _MYCASE(SYS_MSG_WIFI_WPS_PBC_CATCH);
    default:
        break;
    }
    return commonAPI_FormatString("%d", uStatus);
}

void PrintWiFiMsg(msgObject & objMessage)
{
    if (!GetWiFiMsgName(objMessage.message).empty())
    {
        WIRE_INFO("Recieve WiFi Message:%s:%0x:%d:%d", GetWiFiMsgName(objMessage.message).c_str(),
                  objMessage.message, objMessage.wParam, objMessage.lParam);
    }
}

void CWiFiManager::OnTaskActionCallback(NS_TA::CTaskAction* pTaskAction)
{
    CWifiAction* pAction = dynamic_cast<CWifiAction*>(pTaskAction);
    if (NULL == pAction)
    {
        return;
    }

    WiFiTaskParam* pParam = pAction->GetWifiExecParam();
    if (NULL == pParam)
    {
        WIRE_WARN("[Wifi task][Call back][Exec param null]");
        return;
    }

    WIRE_INFO("[Wifi task][Call back][Task:%s][Result:%d]",
              pAction->GetTaskInfo(pAction->GetTaskType()).c_str(), pParam->bResult);

    bool bHandle = true;       //是否被处理
    bool bUpdateWnd = false;    //是否更新窗口
    bool bRefreshList = false; //是否刷新列表

    switch (pAction->GetTaskType())
    {
    case WIFI_TASK_CONFIG_CHANGE:
        bRefreshList = true;
        bUpdateWnd = true;
        WIRE_INFO("[Wifi task][call back][Update list][Update Wnd]");
        break;
    case WIFI_TASK_OPEN:
        DoInit(pParam->bResult, pParam->wParam == 1);
        break;
    case WIFI_TASK_CLOSE:
        DoClose(pParam->bResult);
        break;
#if IF_SUPPORT_WIFI_CLYCLESCAN
    case WIFI_TASK_CYCLE_SCAN_START:
        //
        break;
    case WIFI_TASK_CYCLE_SCAN_STOP:
        //
        break;
#else
    case WIFI_TASK_SCAN_START:
        //
        break;
    case WIFI_TASK_SCAN_STOP:
        //
        break;
#endif
    case WIFI_TASK_REFRESH:
        WIRE_INFO("[Wifi task][call back][Update list][Update Wnd:%d]", pParam->wParam);
        bRefreshList = true;
        bUpdateWnd = pParam->wParam;
        break;
    default:
        bHandle = false;
        break;
    }

    if (bRefreshList)
    {
        _WiFiData.UpdateScanInfoList();
    }

    if (bHandle && bUpdateWnd)
    {
        UpdateUI();
    }
}

LRESULT wifi_ProcessEtlMessage(msgObject & objMsg)
{
#ifdef IF_WIFI_TEST_DRIVER
    if (objMsg.message == WIFI_MSG_TEST_THREAD)
    {
        return _TestThread.OnMessage(objMsg);
    }
#endif
    return _WiFiManager.OnRecieveMessage(objMsg);
}

//初始化
bool CWiFiManager::Init()
{
    if (configParser_GetConfigInt(kszUsbPortEnable) == 0
            || configParser_GetConfigInt(kszWifiFuncEnable) == 0)
    {
        WIRE_WARN("Wifi init fail, usb port or wifi func disabled.");
        return false;
    }

    //注册接收消息
    etl_RegisterMsgHandle(TM_TIMER, TM_TIMER, wifi_ProcessEtlMessage);
    etl_RegisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED, wifi_ProcessEtlMessage);
    etl_RegisterMsgHandle(MSG_WAN_CABLE_CONNECTED, MSG_WAN_CABLE_CONNECTED, wifi_ProcessEtlMessage);
    etl_RegisterMsgHandle(MSG_WAN_CABLE_DISCONNECT, MSG_WAN_CABLE_DISCONNECT, wifi_ProcessEtlMessage);
    etl_RegisterMsgHandle(SYS_MSG_WIFI_SCAN_RESULT, SYS_MSG_WIFI_SCAN_RESULT, wifi_ProcessEtlMessage);
    etl_RegisterMsgHandle(SYS_MSG_WIFI_CONNECT_STATUS, SYS_MSG_WIFI_CONNECT_STATUS,
                          wifi_ProcessEtlMessage);
    etl_RegisterMsgHandle(SYS_MSG_WIFI_WPS_RESULT, SYS_MSG_WIFI_WPS_RESULT, wifi_ProcessEtlMessage);
    etl_RegisterMsgHandle(SYS_MSG_WIFI_WPS_PBC_CATCH, SYS_MSG_WIFI_WPS_PBC_CATCH,
                          wifi_ProcessEtlMessage);
    etl_RegisterMsgHandle(WIFI_MESSAGE_BEGIN, WIFI_MESSAGE_END, wifi_ProcessEtlMessage);

    WIRE_INFO("Init WifiManager...");
    WIRE_INFO("Wifi test Msg[%d]:wParam[10086]:lParam[1:ON/0:OFF]", WIFI_MSG_TEST);

#ifdef IF_WIFI_TEST_DRIVER
    _TestThread;
#endif // IF_WIFI_TEST_DRIVER

    if (IsDongleValid(true))
    {
        ProcessDongleAdded(false);

        if (configParser_GetConfigInt(kszWifiSwitch) == 1)
        {
            SetEnable(true, true);
        }
    }

    return true;
}

//获取已连接网络信息
bool CWiFiManager::GetConnectedNetInfo(WiFiNetInfo & netInfo)
{
    CWiFiSession * pSession = GetSessionByState(SESSION_CONNECTED_STATE);
    if (NULL == pSession)
    {
        return false;
    }

    return _WiFiData.GetConnectedNetInfo(netInfo, pSession->GetNetID());
}

//网络鉴权
bool CWiFiManager::AuthNetwork(int iNetId, const AuthInfo& autoInfo, bool bEapNetwork /*= false*/)
{
    CWiFiSession* pSession = GetSessionByState(SESSION_PAIRING_STATE);
    if (!pSession)
    {
        return false;
    }

    WirelessSessionData wsData;
    wsData.ulMessage = WIRELESS_OPTION_WIFI_AUTHRIZE;
    wsData.wParam = iNetId;
    wsData.lParam = bEapNetwork;
    wsData.pData = (void*)&autoInfo;

    return pSession->ProcessOption(wsData);
}

//获取已保存网络个数
int CWiFiManager::GetSavedNetworkNum()
{
    UI_WIFI_NETINFO_LIST listNetInfo;
    _WiFiData.GetSavedNetInfoList(listNetInfo);
    return listNetInfo.size();
}

//获取pin code
bool CWiFiManager::GetWpsPinCode(yl::string & strPinCode)
{
    //调用驱动刷新pin code
    _WifiDriver.RefreshPinCode();
    _WiFiData.GetWifiWpsPinCode(strPinCode);

    WIRE_INFO("get pin code[%s]", strPinCode.c_str());
    return !(strPinCode.empty());
}

//是否正在连接网络
bool CWiFiManager::IsConnecting(int iNetID/* = WIFI_NET_ID_INVAILED*/)
{
    return _WirelessSessionManager.IsInState(WIFI_SESSION, SESSION_CONNECTING_STATE, iNetID);
}

//获取当前会话状态
bool CWiFiManager::IsInScan()
{
    return _WirelessSessionManager.IsInState(WIFI_SESSION, SESSION_SCAN_STATE, WIFI_NET_ID_INVAILED);
}

//是否已连接
bool CWiFiManager::IsConnected(int iNetID/* = WIFI_NET_ID_INVAILED*/)
{
    return _WirelessSessionManager.IsInState(WIFI_SESSION, SESSION_CONNECTED_STATE, iNetID);
}

// 适配器是否可用
// bCheckNow [1]:检测实时状态,检测完需设置一次本地dongle状态
bool CWiFiManager::IsDongleValid(bool bCheckNow/*= false*/)
{
    return _WifiDriver.IsDongleValid(bCheckNow);
}

//是否已开启
bool CWiFiManager::IsEnable()
{
    return m_bEnable;
}

//删除网络
bool CWiFiManager::DeleteNetwork(int iNetID, bool bNotify/* = true*/)
{
    //当前正连接或已连接该网络,断开与该网络的连接
    if (IsConnecting(iNetID)
            || IsConnected(iNetID))
    {
        DeleteSession(GetSessionByNetID(iNetID), true);
    }

    return  _WiFiData.DeleteNetwork(iNetID, bNotify);
}

//做初始化的工作
bool CWiFiManager::DoInit(bool bSucc, bool bNotify)
{
    SetWifiStatus(bSucc);
    WIRE_INFO("do init wifi [%s], notify[%d]", bSucc ? "success" : "failed", bNotify);

    if (bSucc)
    {
        //初始化配置，同步下数据
        _WiFiData.ReloadConfig();

        if (bNotify)
        {
            //外部开启Wi-Fi 需要执行一次扫描
            ProcessScan();

            //设置IDLE提示定时器
            SetAutoConnectTimer(true);
        }
        else
        {
            _WiFiData.RefreshData(true);
        }

        UpdateUI(OP_WIFI_OPEN_RESULT, WIRELESS_OP_OPEN_RESULT_SUCCESS);

        _WiFiData.UpdateScanInfoList();
        //reServer可能已经连接成功，GUI还没起来错过消息。
        ProcessConnected();
    }
    else
    {
        //先通知开关结果
        UpdateUI(OP_WIFI_OPEN_RESULT, WIRELESS_OP_OPEN_RESULT_FAILED);
    }

    return true;
}

//设置wifi开关状态
void CWiFiManager::SetWifiStatus(bool bEnable, bool bSetConfig/* = true*/)
{
    if (m_bEnable == bEnable)
    {
        return ;
    }

    m_bEnable = bEnable;

    if (bSetConfig)
    {
        configParser_SetConfigInt(kszWifiSwitch, m_bEnable, CONFIG_LEVEL_PHONE);
    }
}

//做关闭wifi的工作
bool CWiFiManager::DoClose(bool bSucc)
{
    WIRE_INFO("close wifi");

    //退出并注销所有会话
    _WirelessSessionManager.DeleteAllSession(WIFI_SESSION, true);

    _WiFiData.ClearNetworkInfo();

    SetWifiStatus(false);

    UpdateUI(OP_WIFI_CLOSE_RESULT,
             bSucc ? WIRELESS_OP_OPEN_RESULT_SUCCESS : WIRELESS_OP_OPEN_RESULT_FAILED);

    return true;
}

//接收消息处理函数
bool CWiFiManager::OnRecieveMessage(msgObject & objMsg)
{
    bool bHandle = true;
    bool bResult = true;

    PrintWiFiMsg(objMsg);

    switch (objMsg.message)
    {
    case TM_TIMER:
        {
            bHandle = ProcessTimer(objMsg.wParam);
        }
        break;
    case SYS_MSG_WIFI_CONNECT_STATUS:
        {
            ProcessConnectStatus(objMsg);
        }
        break;
    case SYS_MSG_WIFI_SCAN_RESULT:                 //扫描结果 0/1/2 success/failed/start scan
        {
            SendCallBackMsgToSessionByState(SESSION_SCAN_STATE, objMsg);
#if IF_SUPPORT_WIFI_CLYCLESCAN
            SendCallBackMsgToSessionByState(SESSION_CYCLE_SCAN_STATE, objMsg);
#else
            UpdateUI(OP_SCAN_WIFI_RESULT, WIRELESS_OP_SCAN_RESULT_SUCCESS, WIFI_NET_ID_INVAILED);
#endif
        }
        break;
    case SYS_MSG_WIFI_WPS_RESULT:                  //WPS的连接结果
        {
            SendCallBackMsgToSessionByState(SESSION_WPSPIN_STATE, objMsg);
            SendCallBackMsgToSessionByState(SESSION_WPS_STATE, objMsg);
        }
        break;
    case SYS_MSG_WIFI_WPS_PBC_CATCH:
        {
            if (talklogic_SessionExist())
            {
                WIRE_INFO("talk logic exsit,watting for end");
                SetWPSWaittingTimer(true);
            }
            else
            {
                //已经在WPS-PBC状态时，不再通知UI
                if (NULL == GetSessionByState(SESSION_WPS_STATE))
                {
                    WIRE_INFO("get wps-pbc button pressed, send msg to ui");
                    UpdateUI(OP_WPS_OPEN, 0, WIFI_NET_ID_INVAILED);
                }
            }
        }
        break;
    case CONFIG_MSG_BCAST_CHANGED:                //网页端/autop profile更改
        {
            if (ST_WIFI_PROFILE == objMsg.wParam
                    && 0 == configParser_GetConfigInt(kszForbidWifi) //禁用Wifi机能则不处理此消息
               )
            {
                SetEnable((1 == configParser_GetConfigInt(kszWifiSwitch)) ? true : false, true, false);

                if (objMsg.lParam > 0
                        && objMsg.lParam <= WIFI_SAVED_MAX)
                {
                    _WiFiData.ReloadConfig();
                }
            }
        }
        break;

    case WIFI_MSG_AUTHORIZE:
        {
            SendUserMsgToSessionByID(objMsg.wParam, objMsg);
        }
        break;
    case WIFI_MSG_GETSTATUS:
        {
            UI_WIFI_NETINFO_LIST listNetwork;
            _WiFiData.GetSavedNetInfoList(listNetwork);
            WIRE_INFO("******Saved Net List******");
            for (WiFiNetInfoItem itBeg = listNetwork.begin();
                    itBeg != listNetwork.end();
                    ++itBeg)
            {
                _WiFiData.PrintSavedNetInfo((*itBeg));
            }

            WIRE_INFO("******Scaced Net List******");
            for (WiFiNetInfoItem itBeg = listNetwork.begin();
                    itBeg != listNetwork.end();
                    ++itBeg)
            {
                _WiFiData.PrintScanNetInfo((*itBeg));
            }

            _WiFiData.GetScanedNetInfoList(listNetwork);

            WIRE_INFO("WiFi Enable[%d], Scanning[%d]", IsEnable(), IsInScan());

            CWiFiSession * pConnecttingSession = GetSessionByState(SESSION_CONNECTING_STATE);
            CWiFiSession * pConnectedSession = GetSessionByState(SESSION_CONNECTED_STATE);
            CWiFiSession * pPairingSession = GetSessionByState(SESSION_PAIRING_STATE);
            CWiFiSession * pWpsSession = GetSessionByState(SESSION_WPS_STATE);
            CWiFiSession * pWpsPinSession = GetSessionByState(SESSION_WPSPIN_STATE);

            if (pConnectedSession == NULL
                    && pConnecttingSession == NULL
                    && pPairingSession == NULL
                    && pWpsPinSession == NULL
                    && pWpsSession == NULL)
            {
                break;
            }

            if (NULL != pConnecttingSession)
            {
                WIRE_INFO("WiFi is Connectting net[%d]", pConnecttingSession->GetNetID());
            }

            if (NULL != pConnectedSession)
            {
                WIRE_INFO("WiFi is Connected net[%d]", pConnectedSession->GetNetID());
            }

            if (NULL != pPairingSession)
            {
                WIRE_INFO("WiFi is pairing net[%d]", pPairingSession->GetNetID());
            }

            if (NULL != pWpsPinSession)
            {
                WIRE_INFO("WiFi is In Wps Pin");
            }

            if (NULL != pWpsSession)
            {
                WIRE_INFO("WiFi is In Wps PBC");
            }
        }
        break;
#if IF_WIFI_GLOBAL_OFF
    case WIFI_MSG_TEST:
        {
            if (objMsg.wParam == 10086)
            {
                WIRE_INFO("set wifi global off[%s]", (objMsg.lParam == 1) ? "ON" : "OFF");
                if (objMsg.lParam == 1)
                {
                    SetEnable(false);
                }
                g_bEnterTestMode = (objMsg.lParam == 1);
            }
        }
        break;
#endif // 0
    default:
        {
            bHandle = false;
            break;
        }
    }

    return bHandle ;
}

//处理rtServer的状态通知消息
bool CWiFiManager::ProcessConnectStatus(msgObject & objMsg)
{
    if (objMsg.lParam != WIFI_CALLBACK_EVENT_DONGLE_ADDED
            && objMsg.lParam != WIFI_CALLBACK_EVENT_DONGLE_REMOVED
            && !IsEnable())
    {
        return false;
    }

    WIRE_INFO("Network[%d] status is[%s]", objMsg.wParam,
              GetWiFiConnectStatusName(objMsg.lParam).c_str());

    bool bHandle = true;
    switch (objMsg.lParam)
    {
    case WIFI_CALLBACK_EVENT_CONNECTED:   //连接成功
        {
            int iNetID = objMsg.wParam;
            if (iNetID == WIFI_NET_ID_INVAILED)
            {
                break;
            }

            SetAutoConnectTimer(false);
            //先更新扫描列表的数据
            _WiFiData.UpdateScanInfoList();

            //连接成功，为当前连接的网络，交给状态处理
            CWiFiSession * pSession = GetSessionByState(SESSION_CONNECTING_STATE);
            if (NULL != pSession
                    && iNetID > WIFI_NET_ID_INVAILED
                    && iNetID == pSession->GetNetID())
            {
                pSession->ProcessCallbackEvent(objMsg);
            }
#if !IF_BUG_57105
            else
            {
                //自动连接成功，进入已连接状态
                //如果有正在连接的会话，需要做一些特殊处理
                WIRE_INFO("auto connect success...");
                //断开其他连接中或连接上的设备，防止rtServer返回错误的iNetID，导致ui logic显示错误
                //rtServer 改正后可去除
                /*for test---sk*/
                DestorySessionByState(SESSION_CONNECTING_STATE);
                DestorySessionByState(SESSION_CONNECTED_STATE);

                WiFiSessionInfo * pSessionInfo = new WiFiSessionInfo;

                // http://10.2.1.199/Bug.php?BugID=110215
                // 自动连接成功, 网络未保存在profile则需要构造数据进行保存
                if (NULL == _WiFiData.GetNetInfoByID(iNetID, true))
                {
                    _WiFiData.CopySavedNetInfo(pSessionInfo->netInfo, (const sys_wifi_network *)objMsg.GetExtraData());
                    pSessionInfo->authInfo.strPsk = pSessionInfo->netInfo.strPsk;
                    pSessionInfo->authInfo.strEapName = pSessionInfo->netInfo.strEapName;
                    pSessionInfo->authInfo.strEapPwd = pSessionInfo->netInfo.strEapPwd;
                    pSessionInfo->bAutoAdd = true;
                    WIRE_INFO("struct data of auto connect network");
                }

                InitSession(SESSION_CONNECTED_STATE, iNetID, (void *)pSessionInfo);
            }
#endif // IF_BUG_57105
        }
        break;
    case WIFI_CALLBACK_EVENT_DONGLE_REMOVED:                //适配器被拔出
        {
            ProcessDongleRemoved();
            //WIFI Dongle拔出，状态改变发送事件信息
            event_SendConnectedDeviceEvent(EVENT_PERIPHERAL_INFORMATION);
        }
        break;
    case WIFI_CALLBACK_EVENT_DONGLE_ADDED:                //可以作为适配器插入的消息
        {
            ProcessDongleAdded(true);
            //WIFI Dongle插入，状态改变发送事件信息
            event_SendConnectedDeviceEvent(EVENT_PERIPHERAL_INFORMATION);
        }
        break;
    case WIFI_CALLBACK_EVENT_CONNECTTING:                 //连接中
        {
#if IF_BUG_57105
            int iNetID = objMsg.wParam;
            if (iNetID == WIFI_NET_ID_INVAILED)
            {
                break;
            }

            //对于自动连接的网络，需要新建一路会话，进入到连接中状态，此时不能再次调用StartConnect
            CWiFiSession * pSession = GetSessionByNetID(iNetID);
            if (NULL == pSession)
            {
                WiFiSessionInfo * pSessionInfo = new WiFiSessionInfo;
                pSessionInfo->iNetID = iNetID;
                pSessionInfo->bAutoConnect = true;

                InitSession(SESSION_CONNECTING_STATE, iNetID, (void *)pSessionInfo);
                UpdateUI();
            }
#endif // IF_BUG_57105
        }
        break;
    case WIFI_CALLBACK_EVENT_DISCONNECTED:              //连接被断开
    case WIFI_CALLBACK_EVENT_AUTHFAILURE:               //密码错误
    case WIFI_CALLBACK_EVENT_CONNECTFAILED:             //连接失败
    case WIFI_CALLBACK_EVENT_INACTIVE:                  //信号消失
        {
            if (WIFI_NET_ID_INVAILED != objMsg.wParam)
            {
                SendCallBackMsgToSessionByID(objMsg.wParam, objMsg);
                //重新更新扫描列表
                _WiFiData.RefreshData();
            }
        }
        break;
    default:
        {
            bHandle = false;
            break;
        }
    }
    return bHandle;
}

bool CWiFiManager::ProcessAutoAdd(int iNetID, const AuthInfo & authInfo,
                                  const WiFiNetInfo * pAddInfo/* = NULL*/)
{
    WiFiNetInfo * pScanNetInfo = _WiFiData.GetNetInfoByID(iNetID);
    if (NULL == pScanNetInfo)
    {
        return false;
    }

    bool bSavedNet = (NULL != _WiFiData.GetNetInfoByID(iNetID, true));

    // 已添加的网络，再次连接成功不需要更新网络信息
    if (bSavedNet
            && NULL == pAddInfo)
    {
        return false;
    }

    //列表已满，不再添加
    if (!bSavedNet
            && GetSavedNetworkNum() >= WIFI_SAVED_MAX)
    {
        // 通知UI 已满
        UpdateUI(OP_WIFI_ADD_RESULT, WIRELESS_OP_ADD_LISTFULL);
        return false;
    }

    WiFiNetInfo tmpSavedInfo;
    if (NULL == pAddInfo)
    {
        // 如果没指定, 则使用扫描到的信息更新已保存的信息
        tmpSavedInfo.iEncrypt = pScanNetInfo->iEncrypt;
        tmpSavedInfo.iSercureMode = pScanNetInfo->iSercureMode;
        tmpSavedInfo.strEapName = authInfo.strEapName;
        tmpSavedInfo.strEapPwd = authInfo.strEapPwd;
        tmpSavedInfo.strPsk = authInfo.strPsk;
    }
    else
    {
        // 使用指定的信息
        tmpSavedInfo = *pAddInfo;
    }

    tmpSavedInfo.iNetID = iNetID;
    tmpSavedInfo.strProfileName = pScanNetInfo->strProfileName;
    tmpSavedInfo.strSSID = pScanNetInfo->strSSID;

    return (!bSavedNet ? _WiFiData.AddNewNetwork(&tmpSavedInfo) : _WiFiData.ModifyNetwork(iNetID,
            &tmpSavedInfo));
}

//处理适配器插入
bool CWiFiManager::ProcessDongleAdded(bool bIsCallback)
{
    if (PT_T49 == devicelib_GetPhoneType())
    {
        return false;
    }

    bool bDongleLastStatus = IsDongleValid(false);
    _WifiDriver.SetDongleEnable(true);

    // 写dongle mac地址
    if (_WiFiManager.IsDongleValid(true))
    {
        yl::string strMac = "";
        _WifiDriver.GetDongleMac(strMac);
        configParser_SetConfigString(kszWifiAdapterMac, strMac.c_str(), CONFIG_LEVEL_PHONE);
    }

    // 开关关闭并且dongle前一个状态为false, 通知UI弹窗
    if (configParser_GetConfigInt(kszWifiSwitch) == 0
            && !bDongleLastStatus)
    {
        UpdateUI(OP_WIFI_DONGLE_STATUS_MODIFY, WIRELESS_OP_DONGLE_ADD, WIFI_NET_ID_INVAILED);
    }
    UpdateUI(OP_WIFI_DONGLE_STATUS_MODIFY, WIRELESS_OP_DONGLE_ADD_IGNORE_SWITCH, WIFI_NET_ID_INVAILED);

    static bool bIsFirst = true;
    if (bIsCallback && bIsFirst)
    {
        bIsFirst = false;
        if (configParser_GetConfigInt(kszWifiSwitch) == 1
                && IsDongleValid(true))
        {
            SetEnable(true, true);
        }
    }

    return true;
}

//处理适配器拔出
bool CWiFiManager::ProcessDongleRemoved()
{
    if (PT_T49 == devicelib_GetPhoneType())
    {
        return false;
    }

    _WifiDriver.SetDongleEnable(false);

    // 清除dongle mac地址
    configParser_SetConfigString(kszWifiAdapterMac, "", CONFIG_LEVEL_PHONE);

    //使用有线时，不弹出提示框
    UpdateUI(OP_WIFI_DONGLE_STATUS_MODIFY, WIRELESS_OP_DONGLE_REMOVE, WIFI_NET_ID_INVAILED);


    SetWPSWaittingTimer(false);

    // 关闭wifi
    DoClose(true);
    WLAsyncHelper_WifiAsync(WIFI_TASK_CLOSE);

    return true;
}

//处理WPS-PIN的用户消息
bool CWiFiManager::ProcessWPSConnect(WIRELESS_SESSION_STATE eState)
{
    if (eState == SESSION_WPS_STATE
            || eState == SESSION_WPSPIN_STATE)
    {
        //要删除掉当前可能存在的会话，不需要发送cancel命令
        DestorySessionByState(SESSION_CONNECTING_STATE);
        DestorySessionByState(SESSION_PAIRING_STATE);
        DestorySessionByState(SESSION_CONNECTED_STATE);
        DestorySessionByState(SESSION_WPS_STATE);
        DestorySessionByState(SESSION_WPSPIN_STATE);

        WiFiSessionInfo * pSessionInfo = new WiFiSessionInfo;
        pSessionInfo->bAutoAdd = true;

        return InitSession(eState, WIFI_NET_ID_INVAILED, (void *)pSessionInfo);
    }

    return false;
}

//处理停止WPS连接
bool CWiFiManager::ProcessCancelWPSConnect(WIRELESS_SESSION_STATE
        eState /* = SESSION_UNINIT_STATE */)
{
    if (eState == SESSION_WPS_STATE
            || eState == SESSION_WPSPIN_STATE)
    {
        return DestorySessionByState(eState, true);
    }
    else
    {
        DestorySessionByState(SESSION_WPS_STATE, true);
        DestorySessionByState(SESSION_WPSPIN_STATE, true);
        return true;
    }
}

//取消连接后，判断是否有已连接成功的网络
bool CWiFiManager::ProcessConnected()
{
    //读取共享内存，判断是否有连接成功的网络
    int iNetID = _WiFiData.GetConnectedNetID();
    if (WIFI_NET_ID_INVAILED != iNetID)
    {
        CWiFiSession * pNewSession = GetSessionByNetID(iNetID);
        if (NULL == pNewSession)
        {
            pNewSession = AddSession();
        }
        if (NULL != pNewSession)
        {
            WiFiSessionInfo * pSessionInfo = new WiFiSessionInfo;

            pNewSession->EnterState(SESSION_CONNECTED_STATE, iNetID, (void *)pSessionInfo);
            return true;
        }
    }

    return false;
}

//处理扫描的用户消息
#if IF_SUPPORT_WIFI_CLYCLESCAN
bool CWiFiManager::ProcessScan(bool bFirstScan /* = true */)
#else
bool CWiFiManager::ProcessScan()
#endif
{
    if (!IsEnable()
            || IsInScan())
    {
        WIRE_INFO("wifi is not open or already in scan ,ignore ui scan msg");
        return false;
    }
#if IF_SUPPORT_WIFI_CLYCLESCAN
#if 0
    CWiFiSession * pSession = _WiFiManager.GetSessionByState(SESSION_SCAN_STATE);
    if (NULL != pSession)
    {
        WirelessSessionData wsData;
        wsData.ulMessage = WIFI_SCAN_OPTION;
        wsData.wParam = 1;

        return _WirelessSessionManager.SendOptionToSessionByState(SESSION_SCAN_STATE, wsData, WIFI_SESSION);
    }
#else
    DestorySessionByState(SESSION_CYCLE_SCAN_STATE, true);
#endif
#endif
    return InitSession(SESSION_SCAN_STATE);
}

bool CWiFiManager::ProcessStopScan()
{
#if IF_SUPPORT_WIFI_CLYCLESCAN
    CWiFiSession * pSession = _WiFiManager.GetSessionByState(SESSION_SCAN_STATE);
    if (NULL != pSession)
    {
        WirelessSessionData wsData;
        wsData.ulMessage = WIFI_SCAN_OPTION;
        wsData.wParam = 0;

        return _WirelessSessionManager.SendOptionToSessionByState(SESSION_SCAN_STATE, wsData, WIFI_SESSION);
    }

    return false;
#else
    return DestorySessionByState(SESSION_SCAN_STATE, true);
#endif
}

//处理断开连接
bool CWiFiManager::ProcessDisconnect(int iNetID)
{
    return DeleteSession(GetSessionByNetID(iNetID), true);
}

//处理取消连接
bool CWiFiManager::ProcessCancelConnect(int iNetID)
{
    CWiFiSession * pSession = GetSessionByNetID(iNetID);
    if (NULL == pSession)
    {
        return false;
    }

    WIRELESS_SESSION_STATE eState = pSession->GetSessionState();
    DeleteSession(pSession, true);

    //输入密码时取消，需要连接回当前已连接的网络
    if (eState == SESSION_PAIRING_STATE)
    {
        ProcessConnected();
        UpdateUI();
    }

    return true;
}

//处理用户的连接消息
bool CWiFiManager::ProcessConnect(int iNetID)
{
    if (!IsEnable())
    {
        WIRE_INFO("wifi is not open,ignore ui connect msg");
        return false;
    }

    WIRELESS_SESSION_STATE eState = SESSION_CONNECTING_STATE;
    WiFiNetInfo * pScanNetInfo = _WiFiData.GetNetInfoByID(iNetID);
    WiFiNetInfo * pSavedNetInfo = _WiFiData.GetNetInfoByID(iNetID, true);

    if (NULL == pSavedNetInfo
            && NULL == pScanNetInfo)
    {
        return false;
    }

    bool bEnterPsk = false;

    if (NULL != pScanNetInfo
            && NULL != pSavedNetInfo)
    {
        if (!(*pScanNetInfo == *pSavedNetInfo))
        {
            bEnterPsk = true;
        }
    }

    WiFiSessionInfo * pSessionInfo = new WiFiSessionInfo;

    if (bEnterPsk
            || (NULL == pSavedNetInfo
                && NULL != pScanNetInfo))
    {
        WIRE_INFO("connect network[%d]:ssid[%s]", iNetID, pScanNetInfo->strSSID.c_str());

        if (SERCURE_NONE == pScanNetInfo->iSercureMode
                && CIPHER_NONE == pScanNetInfo->iEncrypt)
        {
            eState = SESSION_CONNECTING_STATE;
        }
        else
        {
            eState = SESSION_PAIRING_STATE;
        }
        pSessionInfo->bAddBeforeConnect = true;
        pSessionInfo->bAutoAdd = true;
    }

    //可能存在pairing状态的网络，需要断开
    DestorySessionByState(SESSION_PAIRING_STATE);
    //切换连接时，断开其他正在连接的网络。
    DestorySessionByState(SESSION_CONNECTING_STATE);

    return InitSession(eState, iNetID, (void *)pSessionInfo);
}

//处理定时器
bool CWiFiManager::ProcessTimer(int iTimerID)
{
    BOOL bHandle = TRUE;
    if (iTimerID == m_iWpsTimer)
    {
        if (!talklogic_SessionExist())
        {
            WIRE_INFO("waitting wps timer");
            SetWPSWaittingTimer(false);
            UpdateUI(OP_WPS_OPEN, 0, WIFI_NET_ID_INVAILED);
        }
    }
    else if (iTimerID == m_iAutoConnectTimer)
    {
        SetAutoConnectTimer(false);

        //* Wi-Fi 开启，没连接上网络才需要提示
        if (!IsConnected()
                && IsEnable())
        {
            UpdateUI(OP_AUTOCONNECT_TIMEROUT);
        }
    }
    else
    {
        //转发给session
        bHandle = _WirelessSessionManager.SendTimerMsgToSession(iTimerID, WIFI_SESSION);
    }

    return bHandle;
}

//开关wifi
bool CWiFiManager::SetEnable(bool bEnable, bool bNotify/* = false*/, bool bSetConfig/* = true*/)
{
#if IF_WIFI_GLOBAL_OFF
    if (g_bEnterTestMode)
    {
        WIRE_INFO("is in test mode ,do nothing");
        return false;
    }
#endif // 0

    WIRE_INFO("set wifi %s", bEnable ? "on" : "off");
    if (bEnable == IsEnable())
    {
        WIRE_INFO("wifi already In %s, ignore", bEnable ? "open" : "close");
        return false;
    }

    if (bEnable
            && !IsDongleValid())
    {
        //通知UI，适配器未插入，开启失败
        WIRE_INFO("wifi dongle not exist,ignore open wifi msg");
        UpdateUI(OP_WIFI_OPEN_RESULT, WIRELESS_OP_OPEN_RESULT_NODONGLE, WIFI_NET_ID_INVAILED);
        return false;
    }

    //关闭时要先设置开关状态
    if (!bEnable)
    {
        SetWifiStatus(bEnable, bSetConfig);
    }

    UpdateUI(bEnable ? OP_WIFI_OPEN_RESULT : OP_WIFI_CLOSE_RESULT,
             WIRELESS_OP_OPEN_RESULT_ING, WIFI_NET_ID_INVAILED);

    WiFiTaskParam sParam(bNotify);
    WLAsyncHelper_WifiAsync(bEnable ? WIFI_TASK_OPEN : WIFI_TASK_CLOSE, &sParam);

    return true;
}

//设置wps硬按键按下定时器
bool CWiFiManager::SetWPSWaittingTimer(bool bEnable)
{
    WIRE_INFO("set wps timer[%s]", bEnable ? "on" : "off");
    if (bEnable)
    {
        timerSetThreadTimer(m_iWpsTimer, WIFI_WAITTING_TIMER);
    }
    else
    {
        timerKillThreadTimer(m_iWpsTimer);
    }

    return true;
}

//设置自动连接定时器
bool CWiFiManager::SetAutoConnectTimer(bool bEnable)
{
    WIRE_INFO("set autoconnect timer[%s]", bEnable ? "on" : "off");
    if (bEnable)
    {
        timerSetThreadTimer(m_iAutoConnectTimer, WIFI_AUTOCONNECT_TIMER);
    }
    else
    {
        timerKillThreadTimer(m_iAutoConnectTimer);
    }
    return true;
}

//通知UI更新
bool CWiFiManager::UpdateUI(WIFI_UI_OPERATOR iOperatorType  /*= 0*/, int iOperatorResult /* = 0*/,
                            int iNetID /*= 0*/)
{
    WIRELESS_OP_MSG msg;
    msg.iOperationType = iOperatorType;
    msg.iOperationResult = iOperatorResult;
    msg.lSessionID = iNetID;
    WIRE_INFO("Post Msg To UI :wparam[%d],lparam[%d],net id[%d]", iOperatorType, iOperatorResult,
              iNetID);
    return etl_NotifyAppEx(false, WIFI_MSG_UPDATE_UI, iOperatorType, iOperatorResult,
                           sizeof(WIRELESS_OP_MSG), (void *)&msg);
}

#endif // IF_SUPPORT_WIFI
