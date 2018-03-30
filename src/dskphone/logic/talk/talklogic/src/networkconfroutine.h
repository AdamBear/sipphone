#ifndef __NETWORKCONF_ROUTINE_H__
#define __NETWORKCONF_ROUTINE_H__

// 网络会议通话的过程
class CNetworkConfRoutine : public CTalkRoutine
{
public:
    CNetworkConfRoutine();
    virtual ~CNetworkConfRoutine();

    // 状态
    virtual ROUTINE_STATE GetRoutineState() const
    {
        return ROUTINE_NETWORKCONF;
    }
    // 名称
    virtual LPCSTR GetName() const
    {
        return "NetConf";
    }

    // 创建Routine之后的处理
    virtual void AfterRoutineCreate();
    // 销毁Routine之前的处理
    virtual void BeforeRoutineDestroy(bool bJumpToNextRoutine = true);
    // 获取对应session的状态
    virtual SESSION_STATE GetHostSessionState()
    {
        return SESSION_NETWORKCONF;
    }

    // Routine的SIP消息响应
    virtual bool OnSIPMsg(msgObject& msg);

    // 将另外一路通话加入会议中
    virtual bool AddToConf(CSingleSession* pSession);

    bool OnActionEvent(DataUI2Logic* pActionData);

    //根据userid删除与会者
    bool RemoveConfParty(yl::string strUserId);
};

#endif //__NETWORKCONF_ROUTINE_H__
