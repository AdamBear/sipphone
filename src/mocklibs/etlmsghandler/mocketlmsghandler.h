#ifndef __MOCK_ETL_MSG_HANDLER_H__
#define __MOCK_ETL_MSG_HANDLER_H__

#include <gmock/gmock.h>

#include <etlmsghandler/modetlmsghandler.h>

class MockEtlMsgHandler
{
public:
    MockEtlMsgHandler();

    MOCK_METHOD3(RegisterMsgHandle, BOOL(UINT uMsgFrom, UINT uMsgTo,
                                         const CMSGSubscriber & subscriber));
    MOCK_METHOD3(UnregisterMsgHandle, BOOL(UINT uMsgFrom, UINT uMsgTo,
                                           const CMSGSubscriber & subscriber));
    MOCK_METHOD4(NotifyApp, BOOL(BOOL bSync, UINT uMsg, WPARAM wParam, LPARAM lParam));
    MOCK_METHOD6(NotifyAppEx, BOOL(BOOL bSync, UINT uMsg, WPARAM wParam, LPARAM lParam,
                                   int nExtraSize, LPCVOID lpExtraData));
};

extern MockEtlMsgHandler * g_pMockEtlMsgHandler;

#define EXPECT_REG_MSG_HANDLE(msg) \
        EXPECT_CALL(*g_pMockEtlMsgHandler, RegisterMsgHandle(msg, msg, ::testing::_))\
            .Times(1).WillOnce(Return((0)));\
        EXPECT_CALL(*g_pMockEtlMsgHandler, UnregisterMsgHandle(msg, msg, ::testing::_))\
            .Times(1).WillOnce(Return((0)))

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

template<>
class IsEqual<const char *>
{
public:
    IsEqual(const char * value) : m_pValue(value)
    { }
    bool operator()(const void * ptr) const
    {
        return strcmp((const char *)ptr, m_pValue) == 0;
    }

private:
    const char * m_pValue;
};

inline void EXPECT_CALL_NotifyApp(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    EXPECT_CALL(
        *g_pMockEtlMsgHandler,
        NotifyApp(false, uMsg, wParam, lParam)
    ).Times(1).WillOnce(testing::Return(TRUE));
}

template<typename T>
inline void EXPECT_CALL_NotifyAppEx(UINT uMsg, WPARAM wParam, LPARAM lParam, const T & objExtraData)
{
    EXPECT_CALL(
        *g_pMockEtlMsgHandler,
        NotifyAppEx(false, uMsg, wParam, lParam,
                    sizeof(T), testing::Truly(IsEqual<T>(objExtraData)))
    ).Times(1).WillOnce(testing::Return(TRUE));
}

inline void EXPECT_CALL_NotifyAppEx(UINT uMsg, WPARAM wParam, LPARAM lParam,
                                    const char * pExtraString)
{
    EXPECT_CALL(
        *g_pMockEtlMsgHandler,
        NotifyAppEx(false, uMsg, wParam, lParam,
                    strlen(pExtraString) + 1, testing::Truly(IsEqual<const char *>(pExtraString)))
    ).Times(1).WillOnce(testing::Return(TRUE));
}

#endif // !__MOCK_ETL_MSG_HANDLER_H__
