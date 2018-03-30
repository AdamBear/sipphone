#ifndef __BLA_CONNECT_ROUTINE_H__
#define __BLA_CONNECT_ROUTINE_H__

#ifdef IF_FEATURE_BROADSOFT_SCA
class CConnectingRoutine;
///////////////////////////////////CBLAConnectingRoutine///////////////////////////////////////
class CBLAConnectingRoutine : public CConnectingRoutine
{
public:
    // 创建Routine之后的处理
    virtual void AfterRoutineCreate()
    {

    }

    // 状态
    virtual ROUTINE_STATE GetRoutineState() const
    {
        return ROUTINE_BLACONNECTING;
    }
    // 名称
    virtual LPCSTR GetName() const
    {
        return "BLAConnect";
    }

    virtual bool IsAllowAddToConf()
    {
        return false;
    }
};
#endif // #ifdef IF_FEATURE_BROADSOFT_SCA
#endif //__BLA_CONNECT_ROUTINE_H__
