#include "talklistener_inc.h"

CBackLightListener::CBackLightListener()
    : LSN_CLS_LOG_NAME
{
    REG_ASYNC_SIGNAL(TALK_SIG_SESSION_HOLD_CHANGE, CBackLightListener::FlushLight);
    REG_ASYNC_SIGNAL(TALK_SIG_SESSION_HELD_CHANGE, CBackLightListener::FlushLight);
    REG_ASYNC_SIGNAL(TALK_SIG_IDLE_TO_TALK, CBackLightListener::RegisterHandle);
    REG_SYNC_SIGNAL(TALK_SIG_TALK_TO_IDLE, CBackLightListener::UnRegisterHandle);
    REG_ASYNC_SIGNAL(TALK_SIG_SESSION_FOCUS_CHANGE, CBackLightListener::FlushLight);
    REG_ASYNC_SIGNAL(TALK_SIG_SESSION_ROUTINE_CHANGED, CBackLightListener::FlushLight);
    REG_ASYNC_SIGNAL(TALK_SIG_SESSION_PRE_DELETE, CBackLightListener::FlushLight);
}

DECL_SIGNAL_HANDLER(CBackLightListener::RegisterHandle)
{
    // 点亮背光灯
    backlight_RegisterEvent(BACK_LIGHT_TALK, true);
    return true;
}

DECL_SIGNAL_HANDLER(CBackLightListener::UnRegisterHandle)
{
    // 灭背光灯
    backlight_RegisterEvent(BACK_LIGHT_TALK, false);
    //更新电源灯
    powerled_FlushPowerLed();
    return true;
}

DECL_SIGNAL_HANDLER(CBackLightListener::FlushLight)
{
    //更新电源灯
    powerled_FlushPowerLed();
    return true;
}
