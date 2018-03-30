#ifndef __MOD_LED_LAMP_MOCK_H__
#define __MOD_LED_LAMP_MOCK_H__

#include <gmock/gmock.h>

#include "../include/modledlamp.h"


// modledlamp.h接口mock
class LedLampMock
{
public:
    MOCK_METHOD2(PostEvent, bool(LAMP_EVENTTYPE, bool));
};

// 全局mock实例指针
extern LedLampMock * g_pLedLampMock;

#endif
