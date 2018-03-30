#ifndef __BTDIAL_ROUTINE_H__
#define __BTDIAL_ROUTINE_H__

#ifdef IF_BT_SUPPORT_PHONE
class CBTDialRoutine : public CBaseDialRoutine
{
public:

    CBTDialRoutine();
    virtual ~CBTDialRoutine();

    // 状态
    virtual ROUTINE_STATE GetRoutineState() const
    {
        return ROUTINE_BTDIALING;
    }
    // 名称
    virtual LPCSTR GetName() const
    {
        return "BTDial";
    }

    // 获取对应session的状态
    virtual SESSION_STATE GetHostSessionState()
    {
        return SESSION_BTDIALING;
    }

    // 呼出当前已拨号码
    virtual bool CallOut(const yl::string& strNumber, yl::string strName = "",
                         int iCallOutTriggerMask = COT_DEFAULT,
                         bool bAudioOnly = true);
};

#endif // IF_BT_SUPPORT_PHONE
#endif //__BTDIAL_ROUTINE_H__
