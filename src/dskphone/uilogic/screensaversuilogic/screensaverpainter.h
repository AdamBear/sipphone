#ifndef SCREENSAVERPAINTER_H
#define SCREENSAVERPAINTER_H

#include "screensavercontroller.h"
#include "basescreensaversui.h"

#include <ETLLib.hpp>


#ifdef _WIN32
class ScreenSaverPainter : public chMsgProcesser
#else
class ScreenSaverPainter : public CMKProcessor
#endif
{
public:
    ScreenSaverPainter();
    ~ScreenSaverPainter();

    static ScreenSaverPainter& GetInstance();

public:

#if IS_COLOR
    void StartChanging();
    void ResetChanging();

    inline bool IsBeChanging()
    {
        return m_bIsChanging;
    }
    inline bool IsPicNeedChange()
    {
        return m_bIsChanging;
    }

    //当透明度变化到最不透明时，实际上的更换背景的操作
    void OnReallyChangeBG();
#endif

//UI 界面调用
public:
    void InitData();

    void UnInitData();

    void BlindScreenSaversUI(CBaseScreenSaversUI* pBaseUI);

    void UnBlindScreenSaversUI();

    //获取屏保图片路径
    yl::string GetBGPicPath();

    //是否是节电状态
    bool IsPowerSaving();

    //刷新界面
    void UpdateUiLogicDataToWnd();

public:
    //设置节电状态
    void SetPowerSavingEnable(bool bEnable);

#if IS_COLOR
    //当屏保模式配置变化，需要强制改变背景
    void OnScreenSaverPicChange(bool bStopTimer);
    //背景切换配置变化
    void OnBgIntervalChange();
    //时钟移动配置变化
    void OnMoveTimeIntervalChange();
    //当时钟显示格式配置改变
    void OnClockFormatChange();
    //当时钟配置开关配置改变
    void OnCheckShowClock();
#endif
    //xml屏保模式
    void IsOnXMLScreenSavers(bool bEnter);

public:

#if IS_COLOR
    //是否显示时钟
    bool IsShowClock();
    //是否切换背景
    bool IsStartChangePic();

    //背景变换计时器
    void StartBGChangeTimer();
    void StopBGChangeTimer();

    //控制时钟移动计时器
    // http://bugfree.yealink.com/Bug.php?BugID=119473
    //iTimeSpan 时间间隔 为-1 表示  从配置中获取,  时间到的时候 如果正在切换图片过程中 那么延时 2S中跳动
    void StartMoveTimer(int iTimeSpan = -1);
    void StopMoveTimer();

    //开始背景变化动画
    void StartBGChanging();
    //结束动画
    void OnBGChangFinish();

public:
    //当时钟移动计时器时间到，移动时钟位置
    BOOL OnMoveTimeOut(msgObject& msg);
    //当背景变化时间到，开始改变背景（先改变透明度，到最不透明时更换背景，再减少透明度）
    BOOL OnChangeBGTimeOut(msgObject& msg);
    //渐变刷新
    void OnChangeing();
#endif

private:
    //是否图片需要变化
    bool                    m_bPicNeedChange;
    //变化的比例
    int                     m_nScalePercent;

    mkThreadTimer               m_timerMove;        //时钟移动定时器
    mkThreadTimer               m_timerChangeBG;    //背景渐变定时器

    CBaseScreenSaversUI*   m_pBaseUI;
private:

    yl::string          m_strBGPic;        //当前正在使用的背景路径

    bool                m_bIsChanging;     //是否处于变化中，如果是则需要刷背景

    bool                m_bPowerSaving;    //是否是节电状态

    ScreenUiLogicData   m_UiLogicData;
};

#define g_ScreenSaverPainter (ScreenSaverPainter::GetInstance())

#endif // SCREENSAVERPAINTER_H
