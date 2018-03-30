#ifdef IF_WIFI_SIMULATION_DRIVER
#include "../src/wifi_inc.h"

CWIFITestSession::CWIFITestSession()
{
    WIRE_INFO("add a wifitestsession, session id=[%d],session size=[%d]", m_lSessionID,
              m_lSessionCount);
}

CWIFITestSession::~CWIFITestSession()
{
    SAFE_DELETE(m_pState);
    WIRE_INFO("TestDriver---->delete wifitestsession[ScanSession], id=[%d]", m_lSessionID);
    m_lSessionID = WIRE_SESSION_ID_INVAILED;
}

//注销会话状态
bool CWIFITestSession::BeforeDestorySession(bool bEndConnect/* = false*/)
{
    if (NULL != m_pState)
    {
        return m_pState->BeforeStateTransfer(bEndConnect);
    }

    return true;
}

//状态转换
bool CWIFITestSession::EnterState(WIRELESS_SESSION_STATE eState, long ulDeviceHandle,
                                  void * pUserData)
{
    //先退出之前的状态
    if (NULL != m_pState)
    {
        m_pState->BeforeStateTransfer();
        SAFE_DELETE(m_pState);
    }

    //进入新的状态
    m_pState = CWIFITestStateFactory::CreateState(eState);
    if (NULL == m_pState)
    {
        WIRE_ERR("TestDriver---->create state[%d] error", eState);
        return false;
    }

    m_pState->SetSession(this);

    return m_pState->AfterStateCreate(ulDeviceHandle, pUserData);
}

CBaseState * CWIFITestStateFactory::CreateState(WIRELESS_SESSION_STATE eState)
{
    switch (eState)
    {
    //case SESSION_PAIRING_STATE:return new CWIFITestPairState;
    case SESSION_SCAN_STATE:
        return new CWIFITestScanState;
    case SESSION_CONNECTING_STATE:
        return new CWIFITestConnectingState;
    case SESSION_CONNECTED_STATE:
        return new CWIFITestConnectedState;
    case SESSION_WPS_STATE:
        return new CWiFiTestWpsState;
    case SESSION_WPSPIN_STATE:
        return new CWiFiTestWpsPinState;
    default:
        break;
    }
    return NULL;
}

#endif //IF_WIFI_SIMULATION_DRIVER
