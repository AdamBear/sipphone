#ifndef __MOCK_PHONE_DEVICE_H__
#define __MOCK_PHONE_DEVICE_H__

#include <gmock/gmock.h>
#include <devicelib/phonedevice.h>

class MockPhoneDevice
{
public:
    MOCK_METHOD0(GetPhoneType, PHONE_TYPE_DEF());
};

extern MockPhoneDevice * g_pMockPhoneDevice;

#endif // !__MOCK_PHONE_DEVICE_H__
