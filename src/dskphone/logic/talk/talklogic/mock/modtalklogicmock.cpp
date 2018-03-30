#include "modtalklogicmock.h"

#include <ylstring.h>
#include "modtalklogic.h"

MockTalkLogic * g_pMockTalkLogic = NULL;

bool talklogic_SessionExist(bool bIngoreAutoRedialSession /*= false*/)
{
    return g_pMockTalkLogic ? g_pMockTalkLogic->SessionExist(bIngoreAutoRedialSession) : false;
}

void talklogic_GetTalkAndColorRingbackStatus(bool & bTalk, bool & bColorRingback, bool & bPaging)
{
    if (g_pMockTalkLogic)
    {
        g_pMockTalkLogic->GetTalkAndColorRingbackStatus(bTalk, bColorRingback, bPaging);
    }
}

bool talklogic_IsAllSessionRinging()
{
    return g_pMockTalkLogic ? g_pMockTalkLogic->IsAllSessionRinging() : false;
}

bool talklogic_GetMuteStatus(bool bVideo /*= false*/)
{
    return g_pMockTalkLogic ? g_pMockTalkLogic->GetMuteStatus(bVideo) : false;
}

bool talkLogic_IsSessionInTalking(int iSessionID /*= INVALID_SESSION_ID*/, bool bSkipHoldHeld /*= false*/)
{
    return g_pMockTalkLogic ? g_pMockTalkLogic->IsSessionInTalking(iSessionID, bSkipHoldHeld) : false;
}

bool talklogic_IsBluetoothSessionByCallID(int iCallID)
{
    return g_pMockTalkLogic ? g_pMockTalkLogic->IsBluetoothSessionByCallID(iCallID) : false;
}

bool talklogic_IsFoucsInRingSession()
{
    return g_pMockTalkLogic ? g_pMockTalkLogic->IsFoucsInRingSession() : false;
}

#if IF_FEATURE_PAGING
bool talklogic_IsMulticastPage(int iCallID /*= -1*/)
{
    return g_pMockTalkLogic ? g_pMockTalkLogic->IsMulticastPage(iCallID) : false;
}

int talklogic_GetPagingVolume(int iCallID /*= -1*/)
{
    return g_pMockTalkLogic ? g_pMockTalkLogic->GetPagingVolume(iCallID) : 0;
}
#endif // IF_FEATURE_PAGING
