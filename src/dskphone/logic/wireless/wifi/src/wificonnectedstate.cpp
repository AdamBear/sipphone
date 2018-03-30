#ifdef IF_SUPPORT_WIFI
///////////////////////////////////////////////////////////
//  CWifiStateConnected.cpp
//  Implementation of the Class CWifiStateConnected
//  Created on:      15-五月-2014 9:18:38
//  Original author: songk
///////////////////////////////////////////////////////////
#include "wifi_inc.h"

CWiFiConnectedState::CWiFiConnectedState()
{
    m_eState = SESSION_CONNECTED_STATE;
}

CWiFiConnectedState::~CWiFiConnectedState()
{
    //将网络的状态设置为初始状态
    _WiFiData.SetNetworkState(m_lDeviceHandle, WIRE_DEVICE_INIT_STATE);
    _WiFiManager.UpdateUI(OP_DISCONNECT_WIFI_RESULT, WIRELESS_OP_DISCONNECT_SUCCESS, m_lDeviceHandle);
}

//进入状态后处理函数
bool CWiFiConnectedState::AfterStateCreate(long ulDeviceHandle, void * pUserData)
{
    if (NULL == pUserData)
    {
        return false;
    }

    //更新profile
    if (NULL != m_pSessionInfo)
    {
        WIRE_INFO("process profile after connected add[%d], modify[%d]", m_pSessionInfo->bAutoAdd,
                  m_pSessionInfo->bModifyProfile);

        if (m_pSessionInfo->bAutoAdd)
        {
            _WiFiManager.ProcessAutoAdd(ulDeviceHandle, m_pSessionInfo->authInfo);
        }

        if (m_pSessionInfo->bModifyProfile)
        {
            _WiFiManager.ProcessAutoAdd(ulDeviceHandle, m_pSessionInfo->authInfo, &m_pSessionInfo->netInfo);
        }
    }

    //更新当前的net id
    m_lDeviceHandle = ulDeviceHandle;
    //通知UI连接成功
    _WiFiManager.UpdateUI(OP_CONNECT_WIFI_RESULT, WIRELESS_OP_CONNECT_SUCCESS, ulDeviceHandle);
    //将网络设置成已连接
    _WiFiData.SetNetworkState(ulDeviceHandle, WIRE_DEVICE_CONNECTED_STATE);

    return true;
}

//退出状态前处理函数
bool CWiFiConnectedState::BeforeStateTransfer(bool bEndConnect/* = false*/)
{
    if (bEndConnect)
    {
        return _WifiDriver.StartDisconnect(m_lDeviceHandle);
    }

    return true;
}

//处理rtServer回报消息
bool CWiFiConnectedState::ProcessCallbackEvent(msgObject & objMsg)
{
    bool bHandle = true;
    switch (objMsg.lParam)
    {
    case WIFI_CALLBACK_EVENT_DISCONNECTED:   //网络连接被断开
    case WIFI_CALLBACK_EVENT_INACTIVE:     //信号消失
        {
            ExitState();
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
#endif // IF_SUPPORT_WIFI
