#ifndef __MOCK_SUB_INTERFACE_H__
#define __MOCK_SUB_INTERFACE_H__

#include <gmock/gmock.h>

#include <dskthread/mkthread.h>


class MockMkThread
{
public:
	MOCK_METHOD0(mkGetHandle, mkit_handle_t());
};

extern MockMkThread * g_pMockMkThread;

#define EXPECT_CALL_mkGetHandle(handle) \
        EXPECT_CALL(*g_pMockMkThread, mkGetHandle()).Times(1).WillOnce(testing::Return(handle))

#endif // !__MOCK_SUB_INTERFACE_H__
