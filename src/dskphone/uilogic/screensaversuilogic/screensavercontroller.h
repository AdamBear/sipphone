#if IF_FEATURE_XMLB_SCREENSAVER || IS_COLOR
#ifndef SCREENSAVERCONTROLLER_H
#define SCREENSAVERCONTROLLER_H

#include <ETLLib.hpp>
#include "resource/modresource.h"
#include "screensavercommon.h"

typedef YLList<YLSmartPtr<BaseResItem> >    listScreenSaverPic;

enum SCREENSAVER_MODE_CHANGE_TYPE
{
#if IS_COLOR
    SCREENSAVER_MODE_CHANGE_TYPE_SYS,
    SCREENSAVER_MODE_CHANGE_TYPE_USER,
#endif
    SCREENSAVER_MODE_CHANGE_TYPE_XML,
    //SCREENSAVER_MODE_CHANGE_TYPE_SYS_TO_USER,
    //SCREENSAVER_MODE_CHANGE_TYPE_USER_TO_SYS,
#if IS_COLOR
    SCREENSAVER_MODE_CHANGE_TYPE_NO_USER_PIC,
#endif
};

class ScreenSaverController
{

public:
    static ScreenSaverController& GetInstance();

    // 处理相关消息
    static LRESULT OnMessage(msgObject& refMessage);

public:
    ScreenSaverController();
    ~ScreenSaverController();

    //加载默认内容
    void LoadDefault();

#if IS_COLOR
    //获取下一张背景图片
    yl::string GetNextScreenSaverPicPath();
#endif

    LRESULT OnReceiveMessage(msgObject& refMessage);

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
        return m_bIsShowClock && (m_eSSType != SCREENSAVER_MODE_CHANGE_TYPE_XML);
    }

    //获取[0，nMax）的整数
    int GetRandomNumber(int nMax);

    void CheckXMLScreenSavers();

private:

#if IS_COLOR
    //重新加载所有图片
    void ReloadPicFiles();

    //获取一个随机图片
    yl::string GetRandomPic(bool bUseSysPic);
    //按顺序获取下一张图片
    yl::string GetPictureByOrder(bool bUseSysPic);
    yl::string GetSysPicByOrder();
    yl::string GetUserPicByOrder();
    yl::string GetNextPicture(listScreenSaverPic& listPic);

    //根据传入的上次使用的index和列表项总数，获取一个不相等的index
    int GetNotRepeatIndex(int nLastIndex, int nIndexCount);
#endif

    //根据最大值和最小值调整读取到的计时配置，并乘以1000
    void JudgeIntervalValid(int& nInterval, const int nMaxInterval, const int nMinInterval, const int nDefault);

private:
    //记录当前正在使用的屏保是什么类型（系统、自定义）
    SCREENSAVER_MODE_CHANGE_TYPE        m_eSSType;

    //保存系统图片的列表
    listScreenSaverPic              m_listSysPic;
    //保存用户图片的列表
    listScreenSaverPic              m_listUserPic;

    //记录上一次使用的图片
    yl::string                          m_strCurrentPicPath;
    //记录上一次使用的图片的序号
    int                                 m_nLastUsedPicIndex;
    //记录上一次使用的颜色的序号
    int                                 m_nLastUsedColorIndex;

    int                                 m_nBGChangeInterval;
    int                                 m_nClockMoveInterval;

    bool                                m_bIsShowClock;
};

#define g_ScreenSaverController (ScreenSaverController::GetInstance())

#endif // SCREENSAVERCONTROLLER_H
#endif
