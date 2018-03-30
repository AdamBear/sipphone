#ifndef __MOCK_ETL_MSG_HANDLER_H__
#define __MOCK_ETL_MSG_HANDLER_H__

#include <gmock/gmock.h>

#include <etlmsghandler/modetlmsghandler.h>

class MockEtlMsgHandler
{
public:
    MOCK_METHOD3(RegisterMsgHandle, BOOL(UINT uMsgFrom, UINT uMsgTo, FunctionETLMsg pFun));
    MOCK_METHOD3(UnregisterMsgHandle, BOOL(UINT uMsgFrom, UINT uMsgTo, FunctionETLMsg pFun));
    MOCK_METHOD6(NotifyAppEx, BOOL(BOOL bSync, UINT uMsg, WPARAM wParam, LPARAM lParam,
                                   int nExtraSize, LPCVOID lpExtraData));
    MOCK_METHOD6(msgPostMsgToThreadEx, BOOL(HTHREAD hTargetThread, UINT uMsg, WPARAM wParam,
                                            LPARAM lParam, int nExtraSize, LPCVOID lpExtraData));
    MOCK_METHOD3(msgBroadpostThreadMsg, void(UINT uMsg, WPARAM wParam, LPARAM lParam));

    MOCK_METHOD1(msgGetThreadByName, HTHREAD(LPCSTR lpszThreadName));

    MOCK_METHOD1(timerKillThreadTimer, void(UINT nTimerID));
    MOCK_METHOD4(timerSetThreadTimer1, UINT(UINT nTimerID, UINT uTimerOut, const char * file,
                                            int line));
};

extern MockEtlMsgHandler * g_pMockEtlMsgHandler;

bool TriggerEtlMsgCallback(msgObject & msg);



template<typename T>
class IsEqual
{
public:
    IsEqual(const T & value) : m_objValue(value)
    { }
    bool operator()(const void * ptr) const
    {
        return *((T *)ptr) == m_objValue;
    }

private:
    T m_objValue;
};

template<typename T>
inline void EXPECT_CALL_NotifyAppEx(UINT uMsg, WPARAM wParam, LPARAM lParam, const T & objExtraData)
{
    EXPECT_CALL(
        *g_pMockEtlMsgHandler,
        NotifyAppEx(false, uMsg, wParam, lParam,
                    sizeof(T), testing::Truly(IsEqual<T>(objExtraData)))
    ).Times(1).WillOnce(testing::Return(TRUE));
}

#endif // !__MOCK_ETL_MSG_HANDLER_H__
