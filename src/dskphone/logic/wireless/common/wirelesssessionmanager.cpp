#include "wirelesssessionmanager.h"

#ifdef IF_SUPPORT_BLUETOOTH
#include "../bluetoothNew/src/btsession.h"
#endif //IF_SUPPORT_BLUETOOTH

#ifdef IF_SUPPORT_WIFI
#include "../wifi/src/wifisession.h"
#endif //IF_SUPPORT_WIFI

#if defined(IF_SUPPORT_BLUETOOTH) || defined(IF_SUPPORT_WIFI)

#ifdef IF_BT_SIMULATION_DRIVER
#include "../bluetoothNew/testdriver/bttestsession.h"
#endif //IF_BT_SIMULATION_DRIVER

#define IT_WIRELESS_SESSION LIST_WIRELESS_SESSION::iterator

const char * DeviceStateToString(WIRELESS_DEVICE_STATE eState)
{
    for (int i = 0; test_devstate_name_map[i].txt != NULL; i++)
    {
        if (eState == test_devstate_name_map[i].type)
        {
            return test_devstate_name_map[i].txt;
        }
    }

    return "";
}


CWirelessSessionManager::CWirelessSessionManager()
{
    m_listFoucsSession.clear();
    m_listSession.clear();
}


CWirelessSessionManager::~CWirelessSessionManager()
{
}

//判断话路类型
bool CWirelessSessionManager::IsKindofSession(CSessionBase * pSession, WIRELESS_SESSION_TYPE eType)
{
    if (eType == All_SESSION)
    {
        return true;
    }

#ifdef IF_SUPPORT_BLUETOOTH
    if (eType == BT_SESSION)
    {
        return IsKindofSession<CBtSession>(pSession);
    }
#endif // IF_SUPPORT_BLUETOOTH

#ifdef IF_SUPPORT_WIFI
    else if (eType == WIFI_SESSION)
    {
        return IsKindofSession<CWiFiSession>(pSession);
    }
#endif // IF_SUPPORT_WIFI

#ifdef IF_BT_SIMULATION_DRIVER
    else if (eType == BT_TEST_SESSION)
    {
        return IsKindofSession<CBTTestSession>(pSession);
    }
#endif //IF_BT_SIMULATION_DRIVER

    else
    {
        return false;
    }
}

CSessionBase * CWirelessSessionManager::CreateSession(WIRELESS_SESSION_TYPE eType)
{
    switch (eType)
    {
    case All_SESSION:
        return new CSessionBase;

#ifdef IF_SUPPORT_BLUETOOTH
    case BT_SESSION:
        return new CBtSession;
#endif // IF_SUPPORT_BLUETOOTH

#ifdef IF_SUPPORT_WIFI
    case WIFI_SESSION:
        return new CWiFiSession;
#endif // IF_SUPPORT_WIFI

#ifdef IF_BT_SIMULATION_DRIVER
    case BT_TEST_SESSION:
        return new CBTTestSession;
#endif//IF_BT_SIMULATION_DRIVER

    default:
        break;
    }

    return NULL;
}

//获取不同类型的默认非法Handle值
long CWirelessSessionManager::GetInvalidHandle(WIRELESS_SESSION_TYPE eType)
{
    switch (eType)
    {
    case BT_SESSION:
    case BT_TEST_SESSION:
        return BT_DEVICE_HANDLE_INVAILED;
    case WIFI_SESSION:
    case WIFI_TEST_SESSION:
        return WIFI_NET_ID_INVAILED;
    case All_SESSION:
    default:
        break;
    }

    return 0;
}

bool CWirelessSessionManager::IsInState(WIRELESS_SESSION_TYPE eType, WIRELESS_SESSION_STATE eState,
                                        long lHandle, CSessionBase * pExceptThis /* = NULL */)
{
    long lInvalidID = GetInvalidHandle(eType);

    CSessionBase * pSession = NULL;

    if (lHandle == lInvalidID)
    {
        pSession = GetSessionByState(eState, eType);
    }
    else
    {
        pSession = GetSessionByHandle(lHandle, eType);
    }

    if (NULL == pSession
            || pSession == pExceptThis)
    {
        return false;
    }

    return eState == pSession->GetSessionState();
}

//获取会话指针
CSessionBase * CWirelessSessionManager::GetSessionByHandle(long lHandle,
        WIRELESS_SESSION_TYPE eType /* = ALL_SESSION*/)
{
    for (IT_WIRELESS_SESSION itBeg = m_listSession.begin(); itBeg != m_listSession.end(); ++itBeg)
    {
        CSessionBase * pSession = (*itBeg);
        if (NULL == pSession)
        {
            continue;
        }

        if (pSession->GetHandle() == lHandle
                && IsKindofSession(pSession, eType))
        {
            return pSession;
        }
    }

    return NULL;
}

CSessionBase * CWirelessSessionManager::GetSessionByState(WIRELESS_SESSION_STATE eState,
        WIRELESS_SESSION_TYPE eType)
{
    for (IT_WIRELESS_SESSION itBeg = m_listSession.begin(); itBeg != m_listSession.end(); ++itBeg)
    {
        CSessionBase * pSession = (*itBeg);
        if (NULL == pSession)
        {
            continue;
        }

        if (pSession->GetSessionState() == eState
                && IsKindofSession(pSession, eType))
        {
            return pSession;
        }
    }

    return NULL;
}

CSessionBase * CWirelessSessionManager::GetSessionBySessionID(int iSessionID)
{
    if (iSessionID < 0)
    {
        return NULL;
    }

    for (IT_WIRELESS_SESSION itBeg = m_listSession.begin(); itBeg != m_listSession.end(); ++itBeg)
    {
        CSessionBase * pSession = (*itBeg);
        if (NULL == pSession)
        {
            continue;
        }

        if (pSession->GetSessionID() == iSessionID)
        {
            return pSession;
        }
    }

    return NULL;
}

//通过State传递消息给Session
bool CWirelessSessionManager::SendCallBackMsgToSessionByState(WIRELESS_SESSION_STATE eState,
        msgObject & objMsg, WIRELESS_SESSION_TYPE eType)
{
    CSessionBase * pSession = GetSessionByState(eState, eType);
    if (NULL != pSession)
    {
        return pSession->ProcessCallbackEvent(objMsg);
    }
    return false;
}

//通过handle 传递消息给Session
bool CWirelessSessionManager::SendCallBackMsgToSessionByHandle(long lHandle, msgObject & objMsg,
        WIRELESS_SESSION_TYPE eType)
{
    CSessionBase * pSession = GetSessionByHandle(lHandle, eType);
    if (NULL != pSession)
    {
        return pSession->ProcessCallbackEvent(objMsg);
    }

    return false;
}

//通过handle 传Option给Sessin
bool CWirelessSessionManager::SendOptionToSessionByHandle(long lHandle,
        const WirelessSessionData & wsData, WIRELESS_SESSION_TYPE eType)
{
    CSessionBase * pSession = GetSessionByHandle(lHandle, eType);

    if (NULL != pSession)
    {
        return pSession->ProcessOption(wsData);
    }

    return false;
}

bool CWirelessSessionManager::SendOptionToSessionByState(WIRELESS_SESSION_STATE eState,
        const WirelessSessionData & wsData, WIRELESS_SESSION_TYPE eType)
{
    CSessionBase * pSession = GetSessionByState(eState, eType);

    if (NULL != pSession)
    {
        return pSession->ProcessOption(wsData);
    }

    return false;
}

//通过ID传消息ui给session
bool CWirelessSessionManager::SendUserMsgToSessionByHandle(long lHandle, msgObject & objMsg,
        WIRELESS_SESSION_TYPE eType)
{
    CSessionBase * pSession = GetSessionByHandle(lHandle, eType);
    if (NULL != pSession)
    {
        return pSession->ProcessETLMsg(objMsg);
    }

    return false;
}

//传递定时器消息
bool CWirelessSessionManager::SendTimerMsgToSession(int iTimerID, WIRELESS_SESSION_TYPE eType)
{
    IT_WIRELESS_SESSION it = m_listSession.begin();
    for (; it != m_listSession.end(); ++it)
    {
        CSessionBase * pSession = (*it);
        if (NULL == pSession)
        {
            continue;
        }

        if (IsKindofSession(pSession, eType)
                && pSession->ProcessTimer(iTimerID))
        {
            break;
        }
    }

    return it != m_listSession.end();
}

//新建一个会话
CSessionBase * CWirelessSessionManager::AddSession(WIRELESS_SESSION_TYPE eType)
{
    CSessionBase * pSession = CreateSession(eType);
    if (NULL != pSession)
    {
        m_listSession.push_back(pSession);
    }

    return pSession;
}

//初始化一路会话
bool CWirelessSessionManager::InitSession(WIRELESS_SESSION_STATE eState,
        WIRELESS_SESSION_TYPE eType, long lHandle/* = -1*/, void * pUserData/* = NULL*/)
{
    CSessionBase * pSession = AddSession(eType);
    if (NULL != pSession)
    {
        return pSession->EnterState(eState, lHandle, pUserData);
    }

    return false;
}

//删除会话
bool CWirelessSessionManager::DeleteSession(CSessionBase * pSession, bool bEndConnect /* = false */)
{
    if (NULL == pSession)
    {
        return false;
    }

    for (IT_WIRELESS_SESSION it = m_listSession.begin(); it != m_listSession.end();)
    {
        if ((*it) != pSession)
        {
            it++;
            continue;
        }

        pSession->BeforeDestorySession(bEndConnect);

        it = m_listSession.erase(it);

        SAFE_DELETE(pSession);

        return true;
    }

    return false;
}

//通过Handle删除会话
bool CWirelessSessionManager::DestorySessionByHandle(long lDeviceHandle,
        WIRELESS_SESSION_TYPE eType/*= All_SESSION*/, bool bEndConnect /* = false */)
{
    return DeleteSession(GetSessionByHandle(lDeviceHandle, eType), bEndConnect);
}

//通过State删除会话
bool CWirelessSessionManager::DestorySessionByState(WIRELESS_SESSION_STATE eState,
        WIRELESS_SESSION_TYPE eType, bool bEndConnect /* = false */)
{
#if 1
    return DeleteSession(GetSessionByState(eState, eType), bEndConnect);
#else
    while (DeleteSession(GetSessionByState(eState, eType), bEndConnect));
#endif // 0
}

//删除所有会话
bool CWirelessSessionManager::DeleteAllSession(WIRELESS_SESSION_TYPE eType /* = All_SESSION */,
        bool bEndConnect/* = false*/)
{
    for (IT_WIRELESS_SESSION it = m_listSession.begin(); it != m_listSession.end();)
    {
        CSessionBase * pSession = (*it);
        if (NULL == pSession)
        {
            it++;
            continue;
        }

        if (!IsKindofSession(pSession, eType))
        {
            it++;
            continue;
        }

        pSession->BeforeDestorySession(bEndConnect);

        it = m_listSession.erase(it);
        SAFE_DELETE(pSession);
    }

    return true;
}

#endif // defined(IF_SUPPORT_BLUETOOTH) || defined(IF_SUPPORT_WIFI)

