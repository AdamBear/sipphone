#ifndef __DOORPHONE_LISTENER_H__
#define __DOORPHONE_LISTENER_H__

#include "basetalklistener.h"

#ifdef IF_FEATURE_ENTRANCE_GUARD

/* 彩屏机门禁业务 */

CSessionProxy GetCurrentDoorLineSession();

class CDooorPhoneListener : public CBaseTalkListener
{
public:
    CDooorPhoneListener();

    /* 逆序查找第一路来电 */
    struct ReverseFindIncomeVistor
        : public CBaseSessionTraver
    {
        virtual bool operator()(CSessionProxy& proxy);

        CSessionProxy& GetResult()
        {
            return last ? last : first;
        }

        CSessionProxy first;
        CSessionProxy last;
    };

    static CSessionProxy GetRingSession();

    static void GetTargetProxy(CSessionProxy& proxy);

    static bool IsDoorLineCall(const CSessionProxy& proxy);

private:

    bool OpenDoor(const CSessionProxy& proxy);

    bool RestoreHoldStatus(const CSessionProxy& proxy);

private:

    DECL_SIGNAL_HANDLER(OnFocusChange);

    DECL_SIGNAL_HANDLER(OnReleaseEvent);

    DECL_SIGNAL_HANDLER(OnDtmfFinished);

    DECL_SIGNAL_HANDLER(OnTalkHide);

    DECL_SIGNAL_HANDLER(OnRoutineChanged);

    DECL_SIGNAL_HANDLER(OnUIEvent);

};

#endif
#endif
