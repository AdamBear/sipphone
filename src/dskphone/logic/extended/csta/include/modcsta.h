#ifndef __CSTA_MOD_H__
#define __CSTA_MOD_H__

class CCallInfo;

#include "../../actionuri_url/include/action_def.h"
#include "ETLLib.hpp"


void csta_Init();
#if !IF_FEATURE_EVENT_MODULE
void csta_SendCSTAEvent(ActionUrlType eType, const CCallInfo * pCallInfo);

void csta_SendConferenceEvent(const CCallInfo * pFirst, const CCallInfo * pSecond);

ActionUrlType csta_GetCSTAType(ActionUrlType eType);
#endif

#endif
