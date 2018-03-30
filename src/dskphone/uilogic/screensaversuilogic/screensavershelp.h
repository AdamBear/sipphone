#if IF_FEATURE_XMLB_SCREENSAVER || IS_COLOR
#ifndef _SCREEN_SAVERS_HELLP_H_
#define _SCREEN_SAVERS_HELLP_H_

#include "screensaversuilogic/basescreensaversui.h"

#include "screensaversuilogic/screensavershelpbase.h"
#include "uimanager/moduimanager.h"

class CBaseDialog;

class CScreenSaverHelper : public CScreenSaverHelperBase
{
public:
    CScreenSaverHelper();
    ~CScreenSaverHelper();

    static CScreenSaverHelper* GetInstance();

public:
    void ScreenSaverHelper_Init();

    void ScreenSaverHelper_UnInit();

    //窗口堆栈变化通知CScreenSaverManager
    static void ScreenSaverHelper_OnTopWndChangeCallbackFun(CTopWndChangeInfo & WndChangInfo);

    //弹框变化回调
    static void ScreenSaverHelper_MessageCountChangeCallbackFun(/*bool bIsAddMessageBox, int iLastMsgBoxCount, int iCurrentMsgBoxCount*/);

    //通话话路变化回调
    static void ScreenSaverHelper_TalkUICountChangeCallbackFun(/*bool bFirstIntalk, bool bAllTalkExit*/);

    //获取顶层窗口属性是否进去屏保
    bool GetTopWndAllowScreenCount();

    //创建屏保界面
    CBaseScreenSaversUI* CreateDlgScreenSavers();

    //显示或者隐藏屏保层
    void SetScreenSaversVisible(bool bVisible);

    //设置屏保UI指针
    virtual void SetScreenSavers(CBaseScreenSaversUI * pScreenSaversUI)
    {
        if (pScreenSaversUI != NULL)
        {
            m_pScreenSaversUI = pScreenSaversUI;
        }
    }

    CBaseScreenSaversUI * m_pScreenSaversUI;

};

#define g_pScreenSaversHelper (CScreenSaverHelper::GetInstance())

#endif // _SCREEN_SAVERS_HELLP_H_
#endif
