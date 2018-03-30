#ifndef __MOCK_SUB_INTERFACE_H__
#define __MOCK_SUB_INTERFACE_H__

#include <gmock/gmock.h>

#include <ETLLib/ETLLib.hpp>


class MockSubInterface
{
public:
    MockSubInterface();

    MOCK_METHOD2(timerSetThreadTimer1, UINT(UINT nTimerID, UINT uTimerOut));
};

extern MockSubInterface * g_pMockSubInterface;

bool TriggerThreadTimerOfID(UINT nTimerID);
bool TriggerThreadTimerOfInterval(UINT uTimerOut);
void ClearThreadTimerCache(UINT nTimerID);

#define EXPECT_CALL_timerSetThreadTimer(timer, timeout) \
        EXPECT_CALL(*g_pMockSubInterface, timerSetThreadTimer1(timer, timeout)).Times(1).WillOnce(testing::Return(0))

#endif // !__MOCK_SUB_INTERFACE_H__
