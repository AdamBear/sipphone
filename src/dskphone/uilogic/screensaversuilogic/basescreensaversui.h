#ifndef _BASE_SCREENSAVERS_H_
#define _BASE_SCREENSAVERS_H_

#include <ETLLib.hpp>
#include <interfacedefine.h>
#include <yllist.h>
#include <ylhashmap.h>
#include <ylvector.h>
#include <ylstring.h>
#include <utility.hpp>
#include "idlescreen/include/idlescreen_def.h"

//屏保界面显示数据
struct ScreenUiLogicData
{
    bool            bBGIsChanging;    //背景是渐变状态
    bool            bPowerSaving;     //是否是节电状态
    unsigned int    uBgColorRgb;      //遮罩背景颜色，用于渐变背景绘制以及节电状态背景绘制
    //int             nCurColorAlpha;   //当前渐变透明度
    yl::string      strBGPic;         //屏保背景图片路径

    ScreenUiLogicData()
    {
        Reset();
    }

    void Reset()
    {
        bBGIsChanging = false;
        bPowerSaving = false;
        uBgColorRgb = 0;
        //nCurColorAlpha = 0;
        strBGPic = "";
    }

    ScreenUiLogicData& operator=(const ScreenUiLogicData& other)
    {
        if (this != &other)
        {
            this->bBGIsChanging = other.bBGIsChanging;
            this->bPowerSaving = other.bPowerSaving;
            this->uBgColorRgb = other.uBgColorRgb;
            //this->nCurColorAlpha = other.nCurColorAlpha;
            this->strBGPic = other.strBGPic;
        }

        return *this;
    }
};

class CBaseScreenSaversUI
{
public:
    //初始化
    virtual void OnInit() = 0;

    //释放数据
    virtual void OnUnInit() = 0;

    //界面显示
    virtual void OnShow() = 0;

    //隐藏界面
    virtual void OnHide() = 0;

    //删除界面
    virtual void OnDeleteLater() = 0;

    //刷新界面
    virtual void OnUpdateWnd(ScreenUiLogicData& ScreenData) = 0;

    //刷新时间格式变化
    virtual void OnClockFormatChange() = 0;

    //刷新时钟是否显示
    virtual void OnShowClock(bool bshow) = 0;

    //刷新图标
    virtual void OnStatusChange(YLList<IdleStatusItem>& ylVecStatusList) = 0;

    //时钟移动
    virtual void OnMoveClock(int nLeft, int nTop) = 0;

    //获取时钟、屏幕显示宽度高度，用于计算时钟移动位置
    virtual void OnGetClockAndScreenRect(int &nClockWidth, int &nClockHeight, int &nScreenWidth, int &nScreenHeight) = 0;

    //显示或者隐藏屏保层
    virtual void SetScreenSaversVisible(bool bVisible) = 0;

    //xml屏保模式
    virtual void OnXMLScreenSavers(bool bXML) = 0;

    //开始动画
    virtual void StartAnimation(bool bAlphaUp) {}

    // 设置是否exp同步退出屏保
    virtual void SetIsEXPExitSync(bool bIsEXPExitSync) = 0;

};

#endif
