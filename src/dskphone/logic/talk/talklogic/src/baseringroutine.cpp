#include "talklogic_inc.h"

CBaseRingRoutine::CBaseRingRoutine()
{
}

CBaseRingRoutine::~CBaseRingRoutine()
{
}

bool CBaseRingRoutine::ProcessIncomingCall(bool bAnswer, int iCode/* = 0*/)
{
    if (bAnswer)
    {
        AnswerIncomingCall();
    }
    else
    {
        RefuseIncomingCall(iCode);
    }

    return true;
}
