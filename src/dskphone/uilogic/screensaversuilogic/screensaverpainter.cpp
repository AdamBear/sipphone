
#include "screensaverpainter.h"
#include "lamp/backlight/include/modbacklight.h"
#include "baselibs/include/dsklog/log.h"
#if IF_FEATURE_XMLB_SCREENSAVER || IS_COLOR
namespace
{
//变换透明度的单步变化量
#define SCREENSAVER_OPACITY_CHANGE_STEP                     25

//透明度边界值
#define SCREENSAVER_MAX_OPACITY                             255
#define SCREENSAVER_MIN_OPACITY                             0

//变化值的最大值
#define SCREENSAVER_SCALE_MAX_PERCENT                       100
//变化值的最小值
#define SCREENSAVER_SCALE_MIN_PERCENT                       0

//背景过渡时变换透明度的时间间隔
#define SCREENSAVER_CHANGE_INTERVAL                         2

//时钟和状态图标整体的左上角可以使用的值的范围
#define SCREENSAVER_MIN_FRAME_LEFT                          -28
#define SCREENSAVER_MIN_FRAME_TOP                           30

//黑色背景RGB值
#define SCREENSAVER_BG_COLOR_RGB                           0x000000
}

ScreenSaverPainter& ScreenSaverPainter::GetInstance()
{
    static ScreenSaverPainter s_Instance;

    return s_Instance;
}

ScreenSaverPainter::ScreenSaverPainter()
    : m_bIsChanging(false)
    , m_bPicNeedChange(false)
    , m_nScalePercent(SCREENSAVER_SCALE_MIN_PERCENT)
    , m_strBGPic("")
    , m_pBaseUI(NULL)
    , m_bPowerSaving(false)
{

}

ScreenSaverPainter::~ScreenSaverPainter()
{

}

#if IS_COLOR
void ScreenSaverPainter::StartChanging()
{
    //总标志置位，开始变化，并且处于进入状态
    m_bIsChanging = true;
    m_bPicNeedChange = true;

    //m_nScalePercent = SCREENSAVER_SCALE_MIN_PERCENT;
}

void ScreenSaverPainter::ResetChanging()
{
    //如果在变化过程中界面退出了，则需要调用该接口来清空标志
    m_bIsChanging = false;
    m_bPicNeedChange = true;

    //m_nScalePercent = SCREENSAVER_SCALE_MIN_PERCENT;
}

#endif

void ScreenSaverPainter::InitData()
{
    if (NULL == m_pBaseUI)
    {
        IDLEUI_INFO("ScreenSaverPainter ScreenSaversUI is NULL");
        return ;
    }

    //重置数据
    m_UiLogicData.Reset();

    //判断背光灯
    if (backlight_GetCurrenLightLevel() <= 0)
    {
        m_bPowerSaving = true;
    }

#if IS_COLOR
    //是否开启屏保图片切换定时器
    if (IsStartChangePic())
    {
        StartBGChangeTimer();
    }
    else
    {
        StopBGChangeTimer();
    }

    //是否开启时钟移动定时器
    if (IsShowClock())
    {
        StartMoveTimer();
    }
    else
    {
        StopMoveTimer();
    }

    //通知UI是否显示时钟
    if (IsShowClock() && !m_bPowerSaving)
    {
        m_pBaseUI->OnShowClock(true);
    }
    else
    {
        m_pBaseUI->OnShowClock(false);
    }

    m_strBGPic = g_ScreenSaverController.GetNextScreenSaverPicPath();
#endif

    //通知UI刷新屏保背景
    UpdateUiLogicDataToWnd();
}

void ScreenSaverPainter::UnInitData()
{
    m_bPowerSaving = false;
    m_strBGPic = "";
    //重置数据
    m_UiLogicData.Reset();

#if IS_COLOR
    ResetChanging();
    StopBGChangeTimer();
    StopMoveTimer();
#endif
}

void ScreenSaverPainter::BlindScreenSaversUI(CBaseScreenSaversUI* pBaseUI)
{
    m_pBaseUI = pBaseUI;

    //数据化数据
    InitData();
}

void ScreenSaverPainter::UnBlindScreenSaversUI()
{
    m_pBaseUI = NULL;

    UnInitData();
}

#if IS_COLOR
bool ScreenSaverPainter::IsShowClock()
{
    return g_ScreenSaverController.IsShowScreensaverClock();
}

bool ScreenSaverPainter::IsStartChangePic()
{
    bool bResult = false;

    // 根据产品部意见，当使用系统图片时，只有一张图片,不进行“切换”行为
#if SCREEN_SAVE_FEATRUE_CUSTOM_MODE
    SCREENSAVER_MODE_CHANGE_TYPE eUseSysPic = g_ScreenSaverController.GetSSPicType();

    if (SCREENSAVER_MODE_CHANGE_TYPE_SYS != eUseSysPic && SCREENSAVER_MODE_CHANGE_TYPE_NO_USER_PIC != eUseSysPic)
    {
        bResult = true;
    }
#endif

    return bResult;
}


void ScreenSaverPainter::OnReallyChangeBG()
{
    //获取下一张切换的屏保图片路径
    m_strBGPic = g_ScreenSaverController.GetNextScreenSaverPicPath();

    //通知UI更新背景
    UpdateUiLogicDataToWnd();
    if (m_bPicNeedChange)
    {
        m_bPicNeedChange = false;
    }
}

#endif

yl::string ScreenSaverPainter::GetBGPicPath()
{
    return m_strBGPic;
}

bool ScreenSaverPainter::IsPowerSaving()
{
    return m_bPowerSaving;
}

void ScreenSaverPainter::UpdateUiLogicDataToWnd()
{
#if IS_COLOR
    bool bChangeFlag = false;
#else
    bool bChangeFlag = true;
#endif

    m_UiLogicData.uBgColorRgb = SCREENSAVER_BG_COLOR_RGB;

    if (m_UiLogicData.bBGIsChanging != m_bIsChanging)
    {
        m_UiLogicData.bBGIsChanging = m_bIsChanging;
        bChangeFlag = true;
    }

    if (m_UiLogicData.bPowerSaving != m_bPowerSaving)
    {
        m_UiLogicData.bPowerSaving = m_bPowerSaving;
        bChangeFlag = true;
    }

    if (m_UiLogicData.strBGPic != m_strBGPic)
    {
        m_UiLogicData.strBGPic = m_strBGPic;
        bChangeFlag = true;
    }

    //数据有变化才更新UI
    if (bChangeFlag)
    {
        //调用界面刷新数据
        if (NULL != m_pBaseUI)
        {
            m_pBaseUI->OnUpdateWnd(m_UiLogicData);
        }
    }
}

#if IS_COLOR
void ScreenSaverPainter::StartBGChangeTimer()
{
    StopBGChangeTimer();

    int nTime = g_ScreenSaverController.GetBGChangeTime();

#ifdef _WIN32
    m_timerChangeBG.SetTimer(nTime, (void*)&m_timerChangeBG, this, static_cast<msgEntry>(&ScreenSaverPainter::OnChangeBGTimeOut));
#else
    m_timerChangeBG.SetTimer(nTime, (void*)&m_timerChangeBG, this, static_cast<mkMsgEntry>(&ScreenSaverPainter::OnChangeBGTimeOut));
#endif
}

void ScreenSaverPainter::StopBGChangeTimer()
{
    if (m_timerChangeBG.IsTimerRuning())
    {
        m_timerChangeBG.KillTimer();
    }
}

// http://bugfree.yealink.com/Bug.php?BugID=119473
//iTimeSpan 时间间隔 为-1 表示  从配置中获取,  时间到的时候 如果正在切换图片过程中 那么延时 2S中跳动
void ScreenSaverPainter::StartMoveTimer(int iTimeSpan /*= -1*/)
{
    StopMoveTimer();

    int nTime = 0;
    if (iTimeSpan > 0)
    {
        nTime = iTimeSpan;
    }
    else
    {
        nTime = g_ScreenSaverController.GetClockMoveTime();
    }

#ifdef _WIN32
    m_timerMove.SetTimer(nTime, (void*)&m_timerMove, this, static_cast<msgEntry>(&ScreenSaverPainter::OnMoveTimeOut));
#else
    m_timerMove.SetTimer(nTime, (void*)&m_timerMove, this, static_cast<mkMsgEntry>(&ScreenSaverPainter::OnMoveTimeOut));
#endif
}

void ScreenSaverPainter::StopMoveTimer()
{
    if (m_timerMove.IsTimerRuning())
    {
        m_timerMove.KillTimer();
    }
}

void ScreenSaverPainter::StartBGChanging()
{
    SCREENSAVER_MODE_CHANGE_TYPE eSSPicType = g_ScreenSaverController.GetSSPicType();

    IDLEUI_INFO("Start BG Changing, nUseSysPic = %d", eSSPicType);

    //根据产品部意见，当使用系统图片的时候，又只有一张图片，不进行“切换”行为
    if (SCREENSAVER_MODE_CHANGE_TYPE_SYS == eSSPicType)
    {
        StopBGChangeTimer();
    }
    else
    {
        //重置计时器
        StartBGChangeTimer();
    }

    StartChanging();
    OnChangeing();
}


void ScreenSaverPainter::OnBGChangFinish()
{
    m_bIsChanging = false;
    if (m_bPicNeedChange)
    {
        StartChanging();
        OnChangeing();
    }
}

BOOL ScreenSaverPainter::OnMoveTimeOut(msgObject& msg)
{
    IDLEUI_INFO("there is clock_move");
    if (!IsShowClock())
    {
        return TRUE;
    }
    IDLEUI_INFO("there is clock_move  1 step");
    if (IsBeChanging())
    {
        // http://bugfree.yealink.com/Bug.php?BugID=119473
        //iTimeSpan 时间间隔 为-1 表示  从配置中获取,  时间到的时候 如果正在切换图片过程中 那么延时 2S中跳动
        StartMoveTimer(2000);
        return TRUE;
    }
    IDLEUI_INFO("there is clock_move  2 step");
    //根据屏幕、时钟大小计算随机随机位置
    if (NULL != m_pBaseUI)
    {
        int nClockWidth = 0;
        int nClockHeight = 0;
        int nScreenWidth = 0;
        int nScreenHeight = 0;

        //获取时钟、屏幕显示宽度高度，用于计算时钟移动位置
        m_pBaseUI->OnGetClockAndScreenRect(nClockWidth, nClockHeight, nScreenWidth, nScreenHeight);

        int nMaxleft = nScreenWidth - nClockWidth - SCREENSAVER_MIN_FRAME_LEFT;
        int nMaxTop = nScreenHeight - nClockHeight - SCREENSAVER_MIN_FRAME_TOP;

        int nLeft = SCREENSAVER_MIN_FRAME_LEFT + g_ScreenSaverController.GetRandomNumber(nMaxleft - SCREENSAVER_MIN_FRAME_LEFT);
        int nTop = SCREENSAVER_MIN_FRAME_TOP + g_ScreenSaverController.GetRandomNumber(nMaxTop - SCREENSAVER_MIN_FRAME_TOP);

        m_pBaseUI->OnMoveClock(nLeft, nTop);
    }

    return TRUE;
}

BOOL ScreenSaverPainter::OnChangeBGTimeOut(msgObject& msg)
{
    //如果正在进行切换则不允许再次切换
    if (!IsBeChanging())
    {
        IDLEUI_INFO("Change background, start change!");
        StartBGChanging();
    }
    else
    {
        IDLEUI_INFO("Change background, is in changing!");
    }

    return TRUE;
}

void ScreenSaverPainter::OnChangeing()
{
    if (NULL == m_pBaseUI)
    {
        return ;
    }
    if (m_bPicNeedChange)
    {
        m_pBaseUI->StartAnimation(true);
    }
}

//当屏保模式配置变化，需要强制改变背景
void ScreenSaverPainter::OnScreenSaverPicChange(bool bStopTimer)
{
    if (bStopTimer)
    {
        StopBGChangeTimer();
    }
    else
    {
        StartBGChangeTimer();
    }

    //当配置改变时，需要强制改变背景，所以强行调用背景变换时间到的槽
    //如果已经在进行透明度变化
    if (IsBeChanging())
    {
        IDLEUI_INFO("Change pic, IsBeChanging");
        //切换背景结束后会根据参数判断是否还需要切换背景
        StartChanging();
    }
    //如果没有正在进行透明度变化，则可以直接进行背景切换
    else
    {
        IDLEUI_INFO("Change background, start change!");
        StartBGChanging();
    }
}

void ScreenSaverPainter::OnBgIntervalChange()
{
    StartBGChangeTimer();
}

void ScreenSaverPainter::OnMoveTimeIntervalChange()
{
    StartMoveTimer();
}

//当时钟显示格式配置改变
void ScreenSaverPainter::OnClockFormatChange()
{
    if (NULL != m_pBaseUI)
    {
        m_pBaseUI->OnClockFormatChange();
    }
}

//当时钟配置开关配置改变
void ScreenSaverPainter::OnCheckShowClock()
{
    if (NULL != m_pBaseUI)
    {
        if (IsShowClock() && !m_bPowerSaving)
        {
            m_pBaseUI->OnShowClock(true);
        }
        else
        {
            m_pBaseUI->OnShowClock(false);
        }
    }
}
#endif

void ScreenSaverPainter::IsOnXMLScreenSavers(bool bEnter)
{
    if (NULL == m_pBaseUI)
    {
        return;
    }

#if IS_COLOR
    //是否开启屏保图片切换定时器
    if (IsStartChangePic())
    {
        StartBGChangeTimer();
    }
    else
    {
        StopBGChangeTimer();
    }

    //是否开启时钟移动定时器
    if (IsShowClock())
    {
        StartMoveTimer();
    }
    else
    {
        StopMoveTimer();
    }

    //通知UI是否显示时钟
    if (IsShowClock() && !m_bPowerSaving)
    {
        m_pBaseUI->OnShowClock(true);
    }
    else
    {
        m_pBaseUI->OnShowClock(false);
    }
#endif
    m_pBaseUI->OnXMLScreenSavers(bEnter);
}

void ScreenSaverPainter::SetPowerSavingEnable(bool bEnable)
{
    IDLEUI_INFO("ScreenSavers Set PowerSaving Enable[%d]", bEnable);
    if (bEnable == m_bPowerSaving)
    {
        return;
    }

    m_bPowerSaving = bEnable;

    //节电变化要刷新界面
    if (NULL != m_pBaseUI)
    {
#if IS_COLOR
        if (IsShowClock() && !m_bPowerSaving)
        {
            m_pBaseUI->OnShowClock(true);
        }
        else
        {
            m_pBaseUI->OnShowClock(false);
        }
#endif

        //刷新界面
        UpdateUiLogicDataToWnd();
    }
}
#endif
