#include "mocksub-interface.h"

#include <map>
#include <list>

#include "mocklibs/etlmsghandler/mocketlmsghandler.h"


MockSubInterface * g_pMockSubInterface = NULL;
std::map<UINT, UINT> g_mapThreadTimers; // timerid => timeout

MockSubInterface::MockSubInterface()
{
    // 严格模式：如果用例中没有EXPECT_CALL，则不允许被测程序调用接口
    using ::testing::_;
    EXPECT_CALL(*this, timerSetThreadTimer1(_, _)).Times(0);
}

bool TriggerThreadTimerOfID(UINT nTimerID)
{
    std::map<UINT, UINT>::iterator it = g_mapThreadTimers.find(nTimerID);
    if (it == g_mapThreadTimers.end())
    {
        return false;
    }
    msgObject msg;
    msg.message = TM_TIMER;
    msg.wParam = nTimerID;
    return TriggerEtlMsgCallback(msg);
}

bool TriggerThreadTimerOfInterval(UINT uTimerOut)
{
    std::list<UINT> timerIDs;
    for (std::map<UINT, UINT>::iterator it = g_mapThreadTimers.begin(); it != g_mapThreadTimers.end();
            ++it)
    {
        if (it->second == uTimerOut)
        {
            timerIDs.push_back(it->first);
        }
    }
    bool result = false;
    for (std::list<UINT>::iterator it = timerIDs.begin(); it != timerIDs.end(); ++it)
    {
        msgObject msg;
        msg.message = TM_TIMER;
        msg.wParam = *it;
        result = TriggerEtlMsgCallback(msg);
        if (!result)
        {
            break;
        }
    }
    return result;
}

void ClearThreadTimerCache(UINT nTimerID)
{
    g_mapThreadTimers.clear();
}

#ifdef __cplusplus
extern "C" {
#endif

UINT timerSetThreadTimer1(UINT nTimerID, UINT uTimerOut, const char * file, int line)
{
    g_mapThreadTimers[nTimerID] = uTimerOut;
    if (g_pMockSubInterface)
    {
        return g_pMockSubInterface->timerSetThreadTimer1(nTimerID, uTimerOut);
    }
    return 0;
}

void timerKillThreadTimer1(UINT nTimerID)
{
    g_mapThreadTimers.erase(nTimerID);
}

#ifdef __cplusplus
} // extern "C"
#endif
