#ifndef _SCREEN_SAVERS_MANAGER_H_
#define _SCREEN_SAVERS_MANAGER_H_

#if IF_FEATURE_XMLB_SCREENSAVER || IS_COLOR
#include "screensaverpainter.h"
#include "basescreensaversui.h"

class CScreenSaverHelperBase;

class CScreenSaverManager : public CMKProcessor
{
public:
    CScreenSaverManager();
    virtual ~CScreenSaverManager();

public:
    static CScreenSaverManager& GetInstance();

    static LRESULT OnScreenSaverMessage(msgObject& msg);

    //窗口堆栈变化设置新窗口是否进去屏保
    void EnterScreenSaverOnTopWndChange(bool bNewAllow);

    // 处理按键事件
    bool ProcessKeyEvent(int nKeyCode, bool bPress);

    // 处理鼠标事件
    bool ProcessMouseEvent();

    void ScreenSaverHelpe_Init();

    void ScreenSaverHelpe_UnInit();

    //显示屏保界面
    void OnShowScreenSavers();

    //退出屏保界面
    void OnExitScreenSavers();

    //是否显示屏保界面
    bool IsShowScreenSavers();

    // Icon状态改变通知响应函数
    static void StatusIconNotifyFunction(YLList<IdleStatusItem>& ylVecStatusList);

    void OnIconStatusChange(YLList<IdleStatusItem>& ylVecStatusList);

    void setScreenSaverHelpeBase(CScreenSaverHelperBase *pScreenSaverHelperBase);

    CScreenSaverHelperBase* m_pScreenSaverHelperBase;

private:

    void OnMessage(msgObject& refMessage);

    // 重置计时器
    void ResetScreenSaverTimer();

    bool IsTimerActive();

    void ResetTopWndSaverTimer();

private:
    // 启动屏保计时器
    void StartScreenSaverTimer();

    // 关闭屏保计时器
    void StopScreenSaverTimer();

    BOOL OnScreenSaverTimerTimeout(msgObject& msg);

private:
    // 自动进入屏保计时器
    mkThreadTimer m_timerScreenSaver;

    CBaseScreenSaversUI* m_pDlgScreenSavers;

    bool m_bShowScreenSavers;    //标志是否显示屏保界面

    bool m_bAllowScreenSaverCount;  //是否开启屏保计时器
};


#define g_ScreenSaverManger (CScreenSaverManager::GetInstance())

#endif
#endif // _SCREEN_SAVERS_MANAGER_H_
