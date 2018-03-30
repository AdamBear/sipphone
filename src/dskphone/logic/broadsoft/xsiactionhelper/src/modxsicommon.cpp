#include "broadsoft/xsiactionhelper/include/modxsicommon.h"

yl::string Xsi_GetXsiCallId(int nLocalCallId)
{
    return g_objCallIdProcesser.GetXsiCallId(nLocalCallId);
}
#ifdef IF_FEATURE_BROADSOFT_CALLRECORDING
int Xsi_GetConfCallId(ListXsiCallsID & listConfCallID)
{
    return g_objCallIdProcesser.GetConfCallId(listConfCallID);
}

bool Xsi_IsNetworkConfCallId(int iCallId)
{
    return g_objCallIdProcesser.IsNetworkConfCallId(iCallId);
}
#endif

bool Xsi_IsExecutiveCall(int nLocalSessionId)
{
    return g_objCallIdProcesser.IsExecutiveCall(nLocalSessionId);
}

bool Xsi_IsExecutiveCallPushRecall(int nLocalSessionId)
{
    return g_objCallIdProcesser.IsExecutiveCallPushRecall(nLocalSessionId);
}
