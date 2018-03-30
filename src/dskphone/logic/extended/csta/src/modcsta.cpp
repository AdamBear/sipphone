#include "modcsta.h"
#include "cstaprocesser.h"

void csta_Init()
{
    g_objCSTAManager;
}
#if !IF_FEATURE_EVENT_MODULE
void csta_SendCSTAEvent(ActionUrlType eType, const CCallInfo * pCallInfo)
{
    g_objCSTAManager.SendCSTAEvent(eType, pCallInfo);
}

void csta_SendConferenceEvent(const CCallInfo * pFirst, const CCallInfo * pSecond)
{
    g_objCSTAManager.SendConferenceEvent(pFirst, pSecond);
}


ActionUrlType csta_GetCSTAType(ActionUrlType eType)
{
    if (eType == ACTION_URL_ESTABLISHED)
    {
        return ACTION_CSTA_ESTABLISHED;
    }
    else if (eType == ACTION_URL_TERMINATED)
    {
        return ACTION_CSTA_TERMINATED;
    }
    else if (eType == ACTION_URL_CALLOUT)
    {
        return ACTION_CSAT_BEFORE_CALLOUT;
    }
    else if (eType == ACTION_URL_INCOMING_CALL)
    {
        return ACTION_CSTA_INCOMING_CALL;
    }
    else if (eType == ACTION_URL_HOLD)
    {
        return ACTION_CSTA_HOLD;
    }
    else if (eType == ACTION_URL_UNHOLD)
    {
        return ACTION_CSTA_RETRIEVE;
    }
    else if (eType == ACTION_URL_BLIND_TRANSFER)
    {
        return ACTION_CSTA_SINGLE_TRANSFER;
    }
    else if (eType == ACTION_URL_ATTENDED_TRANSFER)
    {
        return ACTION_CSTA_ATTEND_TRANSFER;
    }
    else if (eType == ACTION_URL_FORWARD_INCOMING_CALL)
    {
        return ACTION_CSTA_FORWARD_CALL;
    }
    else if (eType == ACTION_URL_CALL_REMOTE_CANCELED)
    {
        return ACTION_CSTA_TERMINATED;
    }
    else
    {
        return ACTION_CSTA_NONE;
    }
}
#endif
