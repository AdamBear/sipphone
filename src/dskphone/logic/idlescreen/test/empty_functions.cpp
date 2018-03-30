#include <ETLLib.hpp>
#include "devicelib/networkserver.h"
#include "commonunits/sipsender.h"
#include "lamp/backlight/include/modbacklight.h"
#include "dsskey/include/idsskeyimplaction.h"
#include "extended/actionuri_url/include/action_def.h"

/************************
测试用,其他模块接口的空函数
****************************/
void backlight_RegisterEvent(BACK_LIGHT_TYPE, bool) {}

void actURL_SendActionURL(ActionUrlType) {}

void actURL_SendActionURL(ActionUrlType, ActionUrlData const &) {}

void accessCtr_SetPermissions(int) {}

enum VOLUME_TYPE {};
int voice_GetVolume(VOLUME_TYPE)
{
    return 3;
}

class CallCompletionData;
BOOL CallCompletion_GetSubscribeData(CallCompletionData *)
{
    return TRUE;
}
BOOL CallCompletion_CallOut(int)
{
    return TRUE;
}
BOOL CallCompletion_UnsubscribeByCallId(int)
{
    return TRUE;
}
BOOL CallCompletion_Wait(int)
{
    return TRUE;
}

BOOL UIManager_IsInIdle()
{
    return TRUE;
}
BOOL UIManager_ExitIdleBox()
{
    return TRUE;
}
BOOL UIManager_PopupBoxInIdle(int)
{
    return TRUE;
}
BOOL UIManager_IdleUpdate(int)
{
    return TRUE;
}
void powerled_FlushPowerLed() {}

SYSTEMTIME getCurrentTime()
{
    time_t tCurrent = 0;
    netGetLocalTime();
    SYSTEMTIME ShowTime;
    struct tm * tmLocal;

    tmLocal = localtime(&tCurrent);

    ShowTime.wYear = tmLocal->tm_year + 1900;
    ShowTime.wMonth = tmLocal->tm_mon + 1;
    ShowTime.wDay = tmLocal->tm_mday;
    ShowTime.wHour = tmLocal->tm_hour;
    ShowTime.wMinute = tmLocal->tm_min;
    ShowTime.wSecond = tmLocal->tm_sec;
    ShowTime.wDayOfWeek = tmLocal->tm_wday;
    return ShowTime;
}

//SIPSender
CSIPSender::CSIPSender() {}
CSIPSender::~CSIPSender() {}
CSIPSender & CSIPSender::GetInstance()
{
    static CSIPSender re;
    return re;
}

int CSIPSender::QueryLineState(int)
{
    return 0;
}

#if LINUX_SYSTEM
IDsskeyImplAction::IDsskeyImplAction(DssKey_Type) {}
#endif
