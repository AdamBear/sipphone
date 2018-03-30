#ifdef IF_SUPPORT_BLUETOOTH
#include "bluetooth_inc.h"

CBaseState * CBTStateFactory::CreateState(WIRELESS_SESSION_STATE eState)
{
    switch (eState)
    {
    case SESSION_PAIRING_STATE:
        return new CBTPairState;
    case SESSION_SCAN_STATE:
        return new CBTScanState;
    case SESSION_CONNECTING_STATE:
        return new CBTConnectingState;
    case SESSION_CONNECTED_STATE:
        return new CBTConnectedState;
    default:
        break;
    }
    return NULL;
}

CBtSession::CBtSession()
{
    WIRE_INFO("add a btsession, session id=[%d],session size=[%d]", m_lSessionID, m_lSessionCount);
}

CBtSession::~CBtSession(void)
{
    SAFE_DELETE(m_pState);
    WIRE_INFO("delete btsession, id=[%d]", m_lSessionID);
    m_lSessionID = WIRE_SESSION_ID_INVAILED;
}

//注销会话状态
bool CBtSession::BeforeDestorySession(bool bEndConnect/* = false*/)
{
    CBtState * pState = dynamic_cast<CBtState *>(m_pState);

    if (NULL == pState)
    {
        return false;
    }

    pState->DeleteSessionInfo();

    return pState->BeforeStateTransfer(bEndConnect);
}

//状态转换
bool CBtSession::EnterState(WIRELESS_SESSION_STATE eState, long ulDeviceHandle, void * pUserData)
{
    //先退出之前的状态
    if (NULL != m_pState)
    {
        m_pState->BeforeStateTransfer();
        SAFE_DELETE(m_pState);
    }
    //进入新的状态

    m_pState = CBTStateFactory::CreateState(eState);
    if (NULL == m_pState)
    {
        WIRE_ERR("create state[%d] error", eState);
        return false;
    }

    m_pState->SetSession(this);

    CBtState * pState = dynamic_cast<CBtState *>(m_pState);
    if (NULL != pState)
    {
        pState->SetSessionInfo((BTSessionInfo *)pUserData);
    }

    return m_pState->AfterStateCreate(ulDeviceHandle, pUserData);
}

bool CBtSession::ExitSession(bool bEndConnect/* = false*/)
{
    return _BTManager.DeleteSession(this, bEndConnect);
}

void * CBtSession::GetSessionInfo()
{
    CBtState * pBTState = dynamic_cast<CBtState *>(m_pState);

    if (NULL == pBTState)
    {
        return NULL;
    }

    return pBTState->GetSessionInfo();
}

#endif // IF_SUPPORT_BLUETOOTH
