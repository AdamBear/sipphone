#ifndef __MOCK_MK_SERVICE_H__
#define __MOCK_MK_SERVICE_H__

#include <gmock/gmock.h>

#include <msgkit/mkservice.h>


class MockMkService
{
public:
    MOCK_METHOD3(GetData, unsigned long(mkit_handle_t server, int id, int index));
};

extern MockMkService * g_pMockMkService;

#endif // !__MOCK_MK_SERVICE_H__
