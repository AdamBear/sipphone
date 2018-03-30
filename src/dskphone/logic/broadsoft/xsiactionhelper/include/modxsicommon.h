#ifndef _MOD_XSICOMMON_H_
#define _MOD_XSICOMMON_H_

#include <ylstring.h>
#include "broadsoft/xsiactionhelper/src/callidprocess.h"
#include "globalmodel.h"

yl::string Xsi_GetXsiCallId(int nLocalCallId);

#ifdef IF_FEATURE_BROADSOFT_CALLRECORDING
int Xsi_GetConfCallId(ListXsiCallsID & listConfCallID);

bool Xsi_IsNetworkConfCallId(int iCallId);
#endif

bool Xsi_IsExecutiveCall(int nLocalSessionId);

bool Xsi_IsExecutiveCallPushRecall(int nLocalSessionId);

#endif //_MOD_XSICOMMON_H_
