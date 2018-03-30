#ifndef __BASERING_ROUTINE_H__
#define __BASERING_ROUTINE_H__
class CBaseRingRoutine : public CBaseRoutine
{
public:
    CBaseRingRoutine();
    virtual ~CBaseRingRoutine();

    // 状态
    virtual ROUTINE_STATE GetRoutineState() const
    {
        return ROUTINE_RINGING;
    }

    // 名称
    virtual LPCSTR GetName() const
    {
        return "Ring";
    }

    // 拒绝新来电
    // | bSaveMisscall | [in] 是否保存呼叫记录。T2X中在自动拒绝时有此区分
    virtual void RefuseIncomingCall(int iRefuseType = RT_NORMAL) {}
    // 接起新来电，是否需要调整通道
    virtual bool AnswerIncomingCall(bool bSwitchChannel = true, bool bAudioOnly = true)
    {
        return false;
    }

protected:
    //初始化相关变量标记
    virtual void InitStateFlagData()
    {
        m_pCallInfo->SetRingMute(false);
        m_pCallInfo->m_bAnswered = false;
    }
    // 处理通话中来电
    virtual bool ProcessIncomingCall(bool bAnswer, int iCode);
};

#endif //__BASERING_ROUTINE_H__
