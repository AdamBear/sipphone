#ifdef IF_SUPPORT_WIFI
///////////////////////////////////////////////////////////
//  CWifiStatePairing.cpp
//  Implementation of the Class CWifiStatePairing
//  Created on:      15-五月-2014 9:18:38
//  Original author: songk
///////////////////////////////////////////////////////////
#include "wifi_inc.h"

CWiFiPairingState::CWiFiPairingState()
{
    m_eState = SESSION_PAIRING_STATE;
}

CWiFiPairingState::~CWiFiPairingState()
{
    _WiFiData.SetNetworkState(m_lDeviceHandle, WIRE_DEVICE_INIT_STATE);
}

bool CWiFiPairingState::AfterStateCreate(long ulDeviceHandle, void * pUserData)
{
    m_lDeviceHandle = ulDeviceHandle;

    //将网络设置成输入密码中状态
    _WiFiData.SetNetworkState(ulDeviceHandle, WIRE_DEVICE_PAIRING_STATE);

    //先通知UI取消显示搜索界面当前连接成功的网络
    int iCurrentNetID =  _WiFiData.GetConnectedNetID();
    _WiFiManager.UpdateUI(OP_DISCONNECT_WIFI_RESULT, WIRELESS_OP_DISCONNECT_SUCCESS, iCurrentNetID);

    //通知UI输入密码
    WiFiNetInfo * pNetInfo = _WiFiData.GetNetInfoByID(ulDeviceHandle);
    bool bEAPNetwork = pNetInfo && (pNetInfo->iSercureMode == SERCURE_WPA_EAP
                                    || pNetInfo->iSercureMode == SERCURE_WPA2_EAP);
    _WiFiManager.UpdateUI(OP_ENTER_WIFI_PWD, bEAPNetwork, ulDeviceHandle);

    return true;
}

//输入psk码
bool CWiFiPairingState::AuthNetwork(int iNetID, const AuthInfo & autoInfo)
{
    if (NULL != m_pSession
            && iNetID == m_lDeviceHandle)
    {
        //开始尝试连接
        if (NULL != m_pSessionInfo)
        {
            m_pSessionInfo->bAutoConnect = false;
            m_pSessionInfo->authInfo.strEapName = autoInfo.strEapName;
            m_pSessionInfo->authInfo.strEapPwd = autoInfo.strEapPwd;
            m_pSessionInfo->authInfo.strPsk = autoInfo.strPsk;
            m_pSessionInfo->iNetID = iNetID;
            m_pSessionInfo->bAutoAdd = true;
        }

        return m_pSession->EnterState(SESSION_CONNECTING_STATE, iNetID, m_pSessionInfo);
    }

    ExitState();
    return false;
}

//处理rtServer通知消息
bool CWiFiPairingState::ProcessCallbackEvent(msgObject & objMsg)
{
    bool bHandle = true;
    switch (objMsg.lParam)
    {
    case WIFI_CALLBACK_EVENT_INACTIVE:                   //信号消失
        {
            //通知UI,连接失败
            _WiFiManager.UpdateUI(OP_CONNECT_WIFI_RESULT, WIRELESS_OP_CONNECT_FAILED, m_lDeviceHandle);

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

//处理用户消息
bool CWiFiPairingState::ProcessETLMsg(msgObject & objMsg)
{
    bool bHandle = true;
    switch (objMsg.message)
    {
    case WIFI_MSG_AUTHORIZE:
        {
            AuthInfo * authInfo = (AuthInfo *)objMsg.GetExtraData();
            if (authInfo)
            {
                AuthNetwork(objMsg.wParam, *authInfo);
            }
        }
        break;
    default:
        bHandle = false;
        break;
    }
    return bHandle;
}

bool CWiFiPairingState::ProcessOption(const WirelessSessionData& wsData)
{
    bool bHandle = false;
    switch (wsData.ulMessage)
    {
    case WIRELESS_OPTION_WIFI_AUTHRIZE:
        if (wsData.pData)
        {
            AuthInfo* authInfo = (AuthInfo*)wsData.pData;
            if (authInfo)
            {
                bHandle = AuthNetwork(wsData.wParam, *authInfo);
            }
        }
        break;
    default:
        break;
    }

    return bHandle;
}

#endif // IF_SUPPORT_WIFI
