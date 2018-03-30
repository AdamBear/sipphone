#ifndef __NETWORKCONF_CONNECT_ROUTINE_H__
#define __NETWORKCONF_CONNECT_ROUTINE_H__


// 网络会议呼出的过程
class CNetworkConfConnectRoutine : public CConnectingRoutine
{
public:
    // 创建Routine之后的处理
    virtual void AfterRoutineCreate()
    {
        memset(m_pCallInfo->m_iConfCallID, 0x00, sizeof(m_pCallInfo->m_iConfCallID));
    }

    // 状态
    virtual ROUTINE_STATE GetRoutineState() const
    {
        return ROUTINE_NETWORKCONF_CONNECT;
    }
    // 名称
    virtual LPCSTR GetName() const
    {
        return "NetConfConnect";
    }

    // Routine的SIP消息响应
    virtual bool OnSIPMsg(msgObject& msg);

    // 获取对应session的状态
    virtual SESSION_STATE GetHostSessionState()
    {
        return SESSION_NETWORKCONF_CONNECT;
    }

    virtual bool IsAllowAddToConf()
    {
        return false;
    }
};

#endif //__NETWORKCONF_CONNECT_ROUTINE_H__
