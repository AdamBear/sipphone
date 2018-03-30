#ifndef __BASETALK_ROUTINE_H__
#define __BASETALK_ROUTINE_H__

class CBaseTalkRoutine : public CBaseRoutine
{
public:
    CBaseTalkRoutine();
    virtual ~CBaseTalkRoutine();

    // 状态
    virtual ROUTINE_STATE GetRoutineState() const
    {
        return ROUTINE_TALKING;
    }

    // 名称
    virtual LPCSTR GetName() const
    {
        return "Talk";
    }

    //发送dtmf
    void SendTalkDTMF(const yl::string& strDtmf, UINT ulSleepTime = 80);

protected:
    // Hold防抖
    int         m_iTimerHoldDelay;
};

#endif //__BASETALK_ROUTINE_H__
