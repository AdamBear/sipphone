#ifndef __PRE_RETRIEVE_PARK_ROUTINE_H__
#define __PRE_RETRIEVE_PARK_ROUTINE_H__

#ifdef IF_FEATURE_BROADSOFT_CALLPARK

#include "dialroutine.h"

// Transfer拨号过程
class CPreRetrieveParkRoutine : public CDialRoutine
{
public:
    CPreRetrieveParkRoutine();
    virtual ~CPreRetrieveParkRoutine();

    // 状态
    virtual ROUTINE_STATE GetRoutineState() const
    {
        return ROUTINE_RETRIEVE_CALLPARK;
    }
    // 名称
    virtual LPCSTR GetName() const
    {
        return "PreRetrievePark";
    }

    // 获取对应session的状态
    virtual SESSION_STATE GetHostSessionState()
    {
        return SESSION_PRE_RETRIEVEPARK;
    }

    virtual bool CallOut(const yl::string& strNumber, yl::string strName = "",
                         int iCallOutTriggerMask = COT_DEFAULT,
                         bool bAudioOnly = true);
};
#endif // #ifdef IF_FEATURE_BROADSOFT_CALLPARK
#endif
