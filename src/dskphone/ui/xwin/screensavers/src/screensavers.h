#if IF_FEATURE_XMLB_SCREENSAVER || IS_COLOR
#ifndef _SCREENSAVERS_H_
#define _SCREENSAVERS_H_

#include "screensaversuilogic/basescreensaversui.h"
#include "screensaversuilogic/screensavercontroller.h"
//#include "baseidlehandle.h"
#include "idlescreenui/idlescreenuidefine.h"
#include "idleuilogic/include/idlehandledefine.h"


#include "baseui/include/xdatetimeview.h"
#include "ylvector.h"
//#include "uimanager/basedialog.h"
//#include "mainwndhelp/statusiconsbar.h"
#include "xmlbrowserui/src/cuiregionformattext.h"
#include "animation/include/sequentialanimationgroup.h"



class CDlgScreenSavers : public xFrameLayout, public CBaseScreenSaversUI, public xSlotHandler
{

public:
    CDlgScreenSavers();
    ~CDlgScreenSavers();

    // 初始化
    virtual void OnInit();

    // 释放数据，将窗.口清空回复到初始化状态
    virtual void OnUnInit();

    //界面显示
    virtual void OnShow();

    //隐藏界面
    virtual void OnHide();

    //删除界面
    virtual void OnDeleteLater();

    //刷新图片显示
    virtual void OnStatusChange(YLList<IdleStatusItem>& StatusList);

    virtual void OnStatusChange();

    //刷新界面
    virtual void OnUpdateWnd(ScreenUiLogicData& ScreenData);

    //刷新时间格式变化
    virtual void OnClockFormatChange();

    //是否显示时钟
    virtual void OnShowClock(bool bshow);

    //时钟移动
    virtual void OnMoveClock(int nLeft, int nTop);

    //获取时钟、屏幕显示宽度高度，用于计算时钟移动位置
    virtual void OnGetClockAndScreenRect(int &nClockWidth, int &nClockHeight, int &nScreenWidth, int &nScreenHeight);

    //添加状态
    bool addTitleIcon(const yl::string& strFileName, bool bFlashIcon, xLinearLayout* pTitleIcons);

    //更新状态栏
    void UpdateIcon(const YLVector<IdleIconInfo>& Statuslist);

    //清楚Icon
    bool ClearIcon();

    static CDlgScreenSavers* GetInstance();

    //显示或者隐藏屏保层
    virtual void SetScreenSaversVisible(bool bVisible);

    //xml屏保模式
    virtual void OnXMLScreenSavers(bool bEnter);

#if IS_COLOR
    //开始动画
    virtual void StartAnimation(bool bAlphaUp);

    //结束动画
    void StopAnimation();

    //动画响应
    void OnAnimationStateChanged(xSignal* sender, WPARAM wParam, LPARAM lParam,
                                 int nDataBytes, LPVOID pData);
#endif
    // 设置是否exp同步退出屏保
    virtual void SetIsEXPExitSync(bool bIsEXPExitSync)
    {
        m_bIsEXPExitSync = bIsEXPExitSync;
    }

protected:
    //virtual void paintEvent(QPaintEvent* p);

private:
    void InitWigdet();

    //获取背景
    xBrush GetBackGroundBrush();

private:

    //xLinearLayout*    m_pFrmAll;

    //时钟+状态图标
    xFrameLayout*   m_pFrmBG;
    //时钟
    xDateTimeView*  m_pClockLab;

    //状态图标
    xLinearLayout*      m_pTitleIcons;

    xTextView*          m_pLabelLoadError;

    yl::string          m_strBGPic; //背景图片路径

    xBrush              m_pbrushBG; //背景刷

    xImageView*         m_pShade;   //遮罩

    xColor              m_corScreenBlack;//屏保渐变颜色

    ScreenUiLogicData   m_UiLogicData; //显示数据

    CUIRegionFormatText * m_pXMLTextScreen;

    CSequentialAnimationGroup * m_pAnimation;

    bool                m_bAlphaUp; //透明度变化

    bool                m_bIsEXPExitSync;//EXP是否同步退出屏保
};

#define g_pDlgScreenSavers  CDlgScreenSavers::GetInstance()

#endif // _SCREENSAVERS_H_
#endif
