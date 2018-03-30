#ifdef IF_BT_SIMULATION_DRIVER
#include "../src/bluetooth_inc.h"

CBaseState * CBTTestStateFactory::CreateState(WIRELESS_SESSION_STATE eState)
{
    switch (eState)
    {
    case SESSION_PAIRING_STATE:
        return new CBTTestPairState;
    case SESSION_SCAN_STATE:
        return new CBTTestScanState;
    case SESSION_CONNECTING_STATE:
        return new CBTTestConnectingState;
    case SESSION_CONNECTED_STATE:
        return new CBTTestConnectedState;
    default:
        break;
    }
    return NULL;
}

CBTTestSession::CBTTestSession()
{
    WIRE_INFO("add a bttestsession, session id=[%d],session size=[%d]", m_lSessionID, m_lSessionCount);
}

CBTTestSession::~CBTTestSession()
{
    SAFE_DELETE(m_pState);
    WIRE_INFO("delete bttestsession, id=[%d]", m_lSessionID);
    m_lSessionID = WIRE_SESSION_ID_INVAILED;
}

//注销会话状态
bool CBTTestSession::BeforeDestorySession(bool bEndConnect/* = false*/)
{
    if (NULL != m_pState)
    {
        return m_pState->BeforeStateTransfer(bEndConnect);
    }

    return true;
}

//状态转换
bool CBTTestSession::EnterState(WIRELESS_SESSION_STATE eState, long ulDeviceHandle,
                                void * pUserData)
{
    //先退出之前的状态
    if (NULL != m_pState)
    {
        m_pState->BeforeStateTransfer();
        SAFE_DELETE(m_pState);
    }
    //进入新的状态

    m_pState = CBTTestStateFactory::CreateState(eState);
    if (NULL == m_pState)
    {
        WIRE_ERR("create state[%d] error", eState);
        return false;
    }

    m_pState->SetSession(this);

    return m_pState->AfterStateCreate(ulDeviceHandle, pUserData);
}

bool CBTTestSession::ExitSession(bool bEndConnect/* = false*/)
{
    return _WirelessSessionManager.DeleteSession(this, bEndConnect);
}


#endif //IF_BT_SIMULATION_DRIVER
