#ifndef __MOD_BLUETOOTH_MOCK_H__
#define __MOD_BLUETOOTH_MOCK_H__

#include <gmock/gmock.h>


// modbluetooth.h接口mock
class BlueToothMock
{
public:
    MOCK_METHOD0(HSStopRing, void());
};

// 全局mock实例指针
extern BlueToothMock * g_pBlueToothMock;

#endif
