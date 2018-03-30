#ifndef __BASECONNECT_ROUTINE_H__
#define __BASECONNECT_ROUTINE_H__

class CBaseConnectingRoutine : public CBaseRoutine
{
public:
    CBaseConnectingRoutine();
    virtual ~CBaseConnectingRoutine();

    // 状态
    virtual ROUTINE_STATE GetRoutineState() const
    {
        return ROUTINE_CONNECTING;
    }

    // 名称
    virtual LPCSTR GetName() const
    {
        return "Connecting";
    }

protected:
    // 定时取消呼出
    int         m_iTimerCancelCall;

    int         m_iTimeEarlyMediaRtpSniffer;
};

#endif //__BASECONNECT_ROUTINE_H__
