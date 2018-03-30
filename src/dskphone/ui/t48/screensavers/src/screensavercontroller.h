#ifndef SCREENSAVERCONTROLLER_H
#define SCREENSAVERCONTROLLER_H

#include <QtGui>
#include <ETLLib.hpp>
#include "resource/modresource.h"
#include "screensavers/include/screensavercommon.h"

typedef YLList<YLSmartPtr<BaseResItem> >    listScreenSaverPic;
typedef YLList<QRect>                       listScreenSaverRect;
typedef YLList<QColor>                      listScreenSaverColor;

//刚刚进入屏保时，时钟和状态图标整体的初始位置和尺寸定义
#define SCREENSAVER_RECT_LEFT               200
#define SCREENSAVER_RECT_TOP                180
#define SCREENSAVER_RECT_WIDTH              400
#define SCREENSAVER_RECT_HEIGHT             159

//随机获取1或0
#define SCREENSAVER_BOOL_BOUNDRY            2

enum SCREENSAVER_MODE_CHANGE_TYPE
{
    SCREENSAVER_MODE_CHANGE_TYPE_SYS,
    SCREENSAVER_MODE_CHANGE_TYPE_USER,
    SCREENSAVER_MODE_CHANGE_TYPE_XMLB,

    SCREENSAVER_MODE_CHANGE_TYPE_XMLB_TO_USER,
    SCREENSAVER_MODE_CHANGE_TYPE_XMLB_TO_SYS,
    SCREENSAVER_MODE_CHANGE_TYPE_SYS_TO_XMLB,
    SCREENSAVER_MODE_CHANGE_TYPE_USER_TO_XMLB,

    SCREENSAVER_MODE_CHANGE_TYPE_SYS_TO_USER,
    SCREENSAVER_MODE_CHANGE_TYPE_USER_TO_SYS,

    SCREENSAVER_MODE_CHANGE_TYPE_NO_USER_PIC,
};

class ScreenSaverController : public QObject
{
    Q_OBJECT

public:
    static ScreenSaverController * GetInstance();
    static void ReleaseInstance();

    // 处理相关消息
    static LRESULT OnMessage(msgObject & refMessage);

public:
    ScreenSaverController();
    ~ScreenSaverController();

    //加载默认内容
    void LoadDefault();

    //获取下一张背景图片
    yl::string GetNextScreenSaverPicPath();
    //获取下一个渐变颜色
    QColor GetNextScreenSaverColor();
#if SCREEN_SAVE_FEATURE_CLOCK_RECT_FROM_FIX_LIST
    //获取下一个显示时钟的位置
    QRect GetNextScreenSaverRect();
#endif

    LRESULT OnReceiveMessage(msgObject & refMessage);

    inline int GetBGChangeTime()
    {
        return m_nBGChangeInterval;
    }
    inline int GetClockMoveTime()
    {
        return m_nClockMoveInterval;
    }

    inline SCREENSAVER_MODE_CHANGE_TYPE GetSSPicType()
    {
        return m_eSSType;
    }
    inline bool IsShowScreensaverClock()
    {
        return m_bIsShowClock;
    }

    //获取[0，nMax）的整数
    int GetRandomNumber(int nMax);

signals:
    //当需要屏保主动变换背景时发出的信号
    void ScreenSaverPicChange(bool bStopTimer);

    //当需要时钟变动的时候
    void ScreenSaverClockChange();

    //当需要时钟显示格式改变
    void ScreenSaverClockFormatChange();

    void signalPicIntervalChange();
    void signalClockMoveIntervalChange();

    //当屏保模式及URL配置变更时发出信号
    void ScreenSaverModeOrURLChange();


private:
    static ScreenSaverController * s_pInstance;

private:
    //重新加载所有图片
    void ReloadPicFiles();

    //获取一个随机图片
    yl::string GetRandomPic(bool bUseSysPic);
    //按顺序获取下一张图片
    yl::string GetPictureByOrder(bool bUseSysPic);
    yl::string GetSysPicByOrder();
    yl::string GetUserPicByOrder();
    yl::string GetNextPicture(listScreenSaverPic & listPic);

#if SCREEN_SAVE_FEATRUE_COLORFUL_CHANGE
    //获取一个随机颜色
    QColor GetRandomColor();
#endif

#if SCREEN_SAVE_FEATURE_CLOCK_RECT_FROM_FIX_LIST
    //获取一个随机时钟位置
    QRect GetRandomRect();
#endif

    //根据传入的上次使用的index和列表项总数，获取一个不相等的index
    int GetNotRepeatIndex(int nLastIndex, int nIndexCount);

    //根据最大值和最小值调整读取到的计时配置，并乘以1000
    void JudgeIntervalValid(int & nInterval, const int nMaxInterval, const int nMinInterval,
                            const int nDefault);

private:
    //记录当前正在使用的屏保是什么类型（系统、自定义）
    SCREENSAVER_MODE_CHANGE_TYPE        m_eSSType;

    //保存系统图片的列表
    listScreenSaverPic              m_listSysPic;
    //保存用户图片的列表
    listScreenSaverPic              m_listUserPic;
    //保存颜色的列表
    listScreenSaverColor            m_listColor;

#if SCREEN_SAVE_FEATURE_CLOCK_RECT_FROM_FIX_LIST
    //保存时钟位置的列表
    listScreenSaverRect         m_listRect;
#endif

    //记录上一次使用的图片
    yl::string                          m_strCurrentPicPath;
    //记录上一次使用的图片的序号
    int                                 m_nLastUsedPicIndex;
    //记录上一次使用的颜色的序号
    int                                 m_nLastUsedColorIndex;
#if SCREEN_SAVE_FEATURE_CLOCK_RECT_FROM_FIX_LIST
    //记录上一次使用的位置的序号
    int                                 m_nLastUsedRectIndex;
#endif

    int                                 m_nBGChangeInterval;
    int                                 m_nClockMoveInterval;

    bool                                m_bIsShowClock;
};

#define g_pScreenSaverController (ScreenSaverController::GetInstance())

#endif // SCREENSAVERCONTROLLER_H
