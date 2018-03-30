#ifndef __MOD_USB_HEADSET_MOCK_H__
#define __MOD_USB_HEADSET_MOCK_H__

#include <gmock/gmock.h>


// modusbheadset.h接口mock
class USBHeadsetMock
{
public:
    MOCK_METHOD0(StopRing, void());
};

// 全局mock实例指针
extern USBHeadsetMock * g_pUSBHeadsetMock;

#endif
