#ifdef IF_SUPPORT_WIFI
///////////////////////////////////////////////////////////
//  CWifiStateConnect.cpp
//  Implementation of the Class CWifiStateConnect
//  Created on:      15-五月-2014 9:18:38
//  Original author: songk
///////////////////////////////////////////////////////////
#include "wifi_inc.h"

CWiFiConnectingState::CWiFiConnectingState()
{
    m_eState = SESSION_CONNECTING_STATE;
}

CWiFiConnectingState::~CWiFiConnectingState()
{
    _WiFiData.SetNetworkState(m_lDeviceHandle, WIRE_DEVICE_INIT_STATE);
}

bool CWiFiConnectingState::AfterStateCreate(long lDeviceHandle, void * pUserData)
{
    //销毁当前已连接状态的会话
    _WiFiManager.DestorySessionByState(SESSION_CONNECTED_STATE);

    m_lDeviceHandle = lDeviceHandle;

    //将网络的状态设置为连接中
    _WiFiData.SetNetworkState(lDeviceHandle, WIRE_DEVICE_CONNECTING_STATE);

    if (NULL != m_pSessionInfo
            && m_pSessionInfo->bAddBeforeConnect)
    {
        if (NULL != _WiFiData.GetNetInfoByID(m_lDeviceHandle, true))
        {
            WiFiNetInfo * pNetInfo = _WiFiData.GetNetInfoByID(m_lDeviceHandle);
            if (NULL != pNetInfo)
            {
                WIRE_INFO("add profile[%d] before connect", m_lDeviceHandle);
                pNetInfo->strEapName = m_pSessionInfo->authInfo.strEapName;
                pNetInfo->strEapPwd = m_pSessionInfo->authInfo.strEapPwd;
                pNetInfo->strPsk = m_pSessionInfo->authInfo.strPsk;
                _WiFiData.ModifyNetwork(m_lDeviceHandle, pNetInfo);

                //sleep下保证已经通知rtServer
                etlSleep(100);
            }
        }
    }

#if IF_BUG_57105
    //自动连接时，不需要再次向rtServer发起连接
    if (NULL != m_pSessionInfo
            && m_pSessionInfo->bAutoConnect)
    {
        return true;
    }
#endif // IF_BUG_57105

    //开始连接网络
    AuthInfo authInfo;
    if (m_pSessionInfo)
    {
        authInfo = m_pSessionInfo->authInfo;
    }

    if (!_WifiDriver.StartConnect(lDeviceHandle, authInfo))
    {
        WIRE_WARN("start connect net[%d] error", lDeviceHandle);
        ExitState();
        return false;
    }

    _WiFiManager.UpdateUI(OP_CONNECT_WIFI_RESULT, WIRELESS_OP_CONNECTING, m_lDeviceHandle);

    return true;
}

bool CWiFiConnectingState::BeforeStateTransfer(bool bEndConnect/* = false*/)
{
    if (bEndConnect)
    {
        _WiFiManager.UpdateUI(OP_CONNECT_WIFI_RESULT, WIRELESS_OP_CONNECT_CANCLE, m_lDeviceHandle);
        return _WifiDriver.StartDisconnect(m_lDeviceHandle);
    }

    return bEndConnect;
}

//处理rtServer通知消息
bool CWiFiConnectingState::ProcessCallbackEvent(msgObject & objMsg)
{
    bool bHandle = true;
    bool bExit = true;
    switch (objMsg.lParam)
    {
    case WIFI_CALLBACK_EVENT_CONNECTED:               //连接成功
        {
            if (NULL != m_pSession)
            {
                //bug:http://10.2.1.199/Bug.php?BugID=74818
                //Net连接上Wi-Fi带psk给Logic
                sys_wifi_network * pWifiNetwork = (sys_wifi_network *)objMsg.GetExtraData();
                if (NULL != pWifiNetwork)
                {
                    m_pSessionInfo->authInfo.strPsk = pWifiNetwork->psk;
                }
                m_pSession->EnterState(SESSION_CONNECTED_STATE, objMsg.wParam, m_pSessionInfo);
            }
            bExit = false;
        }
        break;
    case WIFI_CALLBACK_EVENT_AUTHFAILURE:             //密码错误
    case WIFI_CALLBACK_EVENT_CONNECTFAILED:            //连接失败
    case WIFI_CALLBACK_EVENT_DISCONNECTED:             //断开连接
    case WIFI_CALLBACK_EVENT_INACTIVE:                 //信号消失
        {
            //通知UI,连接失败
            _WiFiManager.UpdateUI(OP_CONNECT_WIFI_RESULT, WIRELESS_OP_CONNECT_FAILED, m_lDeviceHandle);
        }
        break;
    default:
        {
            bHandle = false;
            bExit = false;
            break;
        }
    }

    if (bExit)
    {
        ExitState();
    }

    return bHandle;
}
#endif // IF_SUPPORT_WIFI
