#ifndef __MOD_ADAPTER_BOX_MOCK_H__
#define __MOD_ADAPTER_BOX_MOCK_H__

#include <gmock/gmock.h>


// modadapterbox.h接口mock
class AdapterBoxMock
{
public:
    MOCK_METHOD0(IsTestMode, bool());
};

// 全局mock实例指针
extern AdapterBoxMock * g_pAdapterBoxMock;

#endif
