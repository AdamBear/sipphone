#ifndef __WIRELESS_SESSIONMANAGER_H
#define __WIRELESS_SESSIONMANAGER_H
#include "wirelessdefines.h"
#include "sessionbase.h"

typedef  YLList<CSessionBase *>    LIST_WIRELESS_SESSION;
#define _WirelessSessionManager CWirelessSessionManager::GetInstance()

class CWirelessSessionManager
{
public:

    static CWirelessSessionManager & GetInstance()
    {
        static CWirelessSessionManager sIns;
        return sIns;
    }

    //获取会话指针
    CSessionBase * GetSessionByHandle(long lHandle, WIRELESS_SESSION_TYPE eType = All_SESSION);
    CSessionBase * GetSessionByState(WIRELESS_SESSION_STATE eState, WIRELESS_SESSION_TYPE eType);
    CSessionBase * GetSessionBySessionID(int iSessionID);

    //通过State传消息给session
    bool SendCallBackMsgToSessionByState(WIRELESS_SESSION_STATE eState, msgObject & objMsg,
                                         WIRELESS_SESSION_TYPE eType);

    //通过ID传消息ui给session
    bool SendUserMsgToSessionByHandle(long lHandle, msgObject & objMsg, WIRELESS_SESSION_TYPE eType);

    //通过ID传callback消息给session
    bool SendCallBackMsgToSessionByHandle(long lHandle, msgObject & objMsg,
                                          WIRELESS_SESSION_TYPE eType);

    //通过ID传操作给session
    bool SendOptionToSessionByHandle(long lHandle, const WirelessSessionData & wsData,
                                     WIRELESS_SESSION_TYPE eType);

    bool SendOptionToSessionByState(WIRELESS_SESSION_STATE eState, const WirelessSessionData & wsData,
                                    WIRELESS_SESSION_TYPE eType);

    //传递定时器消息
    bool SendTimerMsgToSession(int iTimerID, WIRELESS_SESSION_TYPE eType);

    //初始化一路会话
    bool InitSession(WIRELESS_SESSION_STATE eState, WIRELESS_SESSION_TYPE eType, long lHandle = -1,
                     void * pUserData = NULL);

    //删除会话
    bool DeleteSession(CSessionBase * pSession, bool bEndConnect = false);

    //删除指定Handle的会话
    bool DestorySessionByHandle(long ulDeviceHandle, WIRELESS_SESSION_TYPE eType = All_SESSION,
                                bool bEndConnect = false);

    //注销某个状态的会话
    bool DestorySessionByState(WIRELESS_SESSION_STATE eState, WIRELESS_SESSION_TYPE eType,
                               bool bEndConnect = false);

    //删除类型的所有会话
    bool DeleteAllSession(WIRELESS_SESSION_TYPE eType = All_SESSION, bool bEndConnect = false);

    //会话是否处于某种状态
    bool IsInState(WIRELESS_SESSION_TYPE eType, WIRELESS_SESSION_STATE eState, long lHandle,
                   CSessionBase * pExceptThis = NULL);

    //判断是否为某种类型话路
    template <class sessionName>
    bool IsKindofSession(CSessionBase * pSession);

    //新建一个会话
    CSessionBase * AddSession(WIRELESS_SESSION_TYPE eType);

protected:
    long GetInvalidHandle(WIRELESS_SESSION_TYPE eType);
    bool IsKindofSession(CSessionBase * pSession, WIRELESS_SESSION_TYPE eType);
    CSessionBase * CreateSession(WIRELESS_SESSION_TYPE eType);
    YLList<WirelessFoucsSessionInfo> m_listFoucsSession;

private:
    LIST_WIRELESS_SESSION m_listSession;
    CWirelessSessionManager();
    ~CWirelessSessionManager();
};

template <class sessionName>
bool CWirelessSessionManager::IsKindofSession(CSessionBase * pSession)
{
    if (NULL == pSession)
    {
        return false;
    }

    return (NULL != dynamic_cast<sessionName *>(pSession));
}

#endif //__WIRELESS_SESSIONMANAGER_H
