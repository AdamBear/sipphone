#ifndef __MOD_TALK_LOGIC_MOCK_H__
#define __MOD_TALK_LOGIC_MOCK_H__

#include <gmock/gmock.h>

class MockTalkLogic
{
public:
    MOCK_METHOD1(SessionExist, bool(bool bIngoreAutoRedialSession));
    MOCK_METHOD3(GetTalkAndColorRingbackStatus, void(bool & bTalk, bool & bColorRingback, bool & bPaging));
    MOCK_METHOD0(IsAllSessionRinging, bool());
    MOCK_METHOD1(GetMuteStatus, bool(bool bVideo));
    MOCK_METHOD2(IsSessionInTalking, bool(int iSessionID, bool bSkipHoldHeld));
    MOCK_METHOD1(IsBluetoothSessionByCallID, bool(int iCallID));
    MOCK_METHOD0(IsFoucsInRingSession, bool());
#if IF_FEATURE_PAGING
    MOCK_METHOD1(IsMulticastPage, bool(int iCallID));
    MOCK_METHOD1(GetPagingVolume, int(int iCallID));
#endif // IF_FEATURE_PAGING
};

extern MockTalkLogic * g_pMockTalkLogic;

#endif // __MOD_TALK_LOGIC_MOCK_H__
