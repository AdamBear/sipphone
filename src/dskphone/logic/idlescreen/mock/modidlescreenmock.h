#ifndef __MOD_IDLE_SCREEN_MOCK_H__
#define __MOD_IDLE_SCREEN_MOCK_H__

#include <gmock/gmock.h>

#include <ylstring.h>


// modidlescreen.h接口mock
class IdleScreenMock
{
public:
    MOCK_METHOD2(SetStatus, bool(int, const yl::string));
    MOCK_METHOD2(DropStatus, bool(int, bool));
};

// 全局mock实例指针
extern IdleScreenMock * g_pIdleScreenMock;

#endif
