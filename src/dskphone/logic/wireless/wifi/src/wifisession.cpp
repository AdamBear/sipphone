#ifdef IF_SUPPORT_WIFI
///////////////////////////////////////////////////////////
//  CWifiSession.cpp
//  Implementation of the Class CWifiSession
//  Created on:      15-五月-2014 9:17:31
//  Original author: songk
///////////////////////////////////////////////////////////
#include "wifi_inc.h"

CBaseState * CWifiStateFactory::CreateWifiState(WIRELESS_SESSION_STATE eState)
{
    switch (eState)
    {
    case SESSION_CONNECTED_STATE:
        return new CWiFiConnectedState;
    case SESSION_CONNECTING_STATE:
        return new CWiFiConnectingState;
    case SESSION_PAIRING_STATE:
        return new CWiFiPairingState;
    case SESSION_WPS_STATE:
        return new CWiFiWpsState;
    case SESSION_WPSPIN_STATE:
        return new CWiFiWpspinState;
    case SESSION_CYCLE_SCAN_STATE:
        return new CWiFiCycleScanState;
    case SESSION_SCAN_STATE:
        return new CWiFiScanState;
    default:
        break;
    }
    return NULL;
}

CWiFiSession::CWiFiSession()
{
    WIRE_INFO("add a wifisession, session id=[%d],session size=[%d]", m_lSessionID, m_lSessionCount);
}

CWiFiSession::~CWiFiSession()
{
    SAFE_DELETE(m_pState);
    WIRE_INFO("delete wifisession, id=[%d]", m_lSessionID);
    m_lSessionID = WIRE_SESSION_ID_INVAILED;
}

//注销会话状态
bool CWiFiSession::BeforeDestorySession(bool bEndConnect/* = false*/)
{
    CWiFiState * pState = dynamic_cast<CWiFiState *>(m_pState);

    if (NULL == pState)
    {
        return false;
    }

    pState->DeleteSessionInfo();
    return m_pState->BeforeStateTransfer(bEndConnect);
}

//状态转换
bool CWiFiSession::EnterState(WIRELESS_SESSION_STATE eState, long lDeviceHandle, void * pUserData)
{
    //先退出之前的状态
    if (NULL != m_pState)
    {
        m_pState->BeforeStateTransfer();
        SAFE_DELETE(m_pState);
    }

    //进入新的状态
    m_pState = CWifiStateFactory::CreateWifiState(eState);
    if (NULL == m_pState)
    {
        WIRE_ERR("create state error");
        return false;
    }

    m_pState->SetSession(this);

    CWiFiState * pState = dynamic_cast<CWiFiState *>(m_pState);
    if (NULL != pState)
    {
        pState->SetSessionInfo((WiFiSessionInfo *)pUserData);
    }

    return m_pState->AfterStateCreate(lDeviceHandle, pUserData);
}

int CWiFiSession::GetNetID()
{
    CWiFiState * pState = dynamic_cast<CWiFiState *>(m_pState);
    return (pState == NULL) ? WIFI_NET_ID_INVAILED : pState->GetHandle();
}

bool CWiFiSession::ExitSession(bool bEndConnect/* = false*/)
{
    return _WirelessSessionManager.DeleteSession(this, bEndConnect);
}
#endif // IF_SUPPORT_WIFI
