#if IF_FEATURE_XMLB_SCREENSAVER || IS_COLOR
#ifndef _SCREENSAVERSHELPBASE_H
#define _SCREENSAVERSHELPBASE_H


//#include "screensaversuilogic/basescreensaversui.h"
class CBaseScreenSaversUI;

class CScreenSaverHelperBase
{
public:
    CScreenSaverHelperBase();
    ~CScreenSaverHelperBase();

public:
    virtual void ScreenSaverHelper_Init() {}

    virtual void ScreenSaverHelper_UnInit() {}


    //获取顶层窗口属性是否进去屏保
    virtual bool GetTopWndAllowScreenCount() = 0;

    //创建屏保界面
    virtual CBaseScreenSaversUI* CreateDlgScreenSavers() = 0;

    //显示或者隐藏屏保层
    virtual void SetScreenSaversVisible(bool bVisible) = 0;

    //设置屏保UI指针
    virtual void SetScreenSavers(CBaseScreenSaversUI * pScreenSaversUI) = 0;
};

#endif //_SCREENSAVERSHELPBASE_H
#endif
