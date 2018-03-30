#include "modscreensaversuilogic.h"
#include "screensaversmanager.h"
#include "screensavercontroller.h"

#if IF_FEATURE_XMLB_SCREENSAVER || IS_COLOR
void UIlogic_ScreenSaversInit()
{
    g_ScreenSaverController.LoadDefault();

    g_ScreenSaverManger.ScreenSaverHelpe_Init();

}

void UIlogic_ScreenSaversUninit()
{
    g_ScreenSaverManger.ScreenSaverHelpe_UnInit();

}

//进入屏保
void UIlogic_EnterScreenSavers()
{
    g_ScreenSaverManger.OnShowScreenSavers();
}

//是否显示屏保
bool UIlogic_ScreenSaversIsShow()
{
    return g_ScreenSaverManger.IsShowScreenSavers();
}

//退出屏保
void UIlogic_ExitScreensaver()
{
    g_ScreenSaverManger.OnExitScreenSavers();

}
////屏保移植时残留
void UILogic_ScreenSaversWakeUpByTalkCountChange(/*bool bFirstIntalk, bool bAllTalkExit*/)
{

    if (1/*bFirstIntalk*/)
    {
        //退出屏保
        g_ScreenSaverManger.OnExitScreenSavers();
    }

}

//设置是否可以进入屏保
void UILogic_SetEnterScreenSaver(bool bNewAllow)
{
    g_ScreenSaverManger.EnterScreenSaverOnTopWndChange(bNewAllow);
}

bool UIlogic_ScreenSaversProcessKeyEvent(int nKeyCode, int bPress)
{


    if (g_ScreenSaverManger.ProcessKeyEvent(nKeyCode, bPress))
    {
        return true;
    }

    return false;
}

bool UIlogic_ScreenSaversProcessMouseEvent()
{

    if (g_ScreenSaverManger.ProcessMouseEvent())
    {
        return true;
    }

    return false;
}
#endif

