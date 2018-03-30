#ifndef _DLG_TALK_MENU_BASE_H_
#define _DLG_TALK_MENU_BASE_H_

#include "xwindow/xListView.h"

#include "uimanager/basedialog.h"

#include "talkbaseui.h"

namespace talkui
{
class CDlgTalkMenuBase: public CBaseDialog, public CTalkBaseUI
{

public:
    CDlgTalkMenuBase(EWindowType eWndType, const yl::string & strTalkUIClassName);
    virtual ~CDlgTalkMenuBase();

public:
    // 初始化
    virtual void Init() = 0;

    // 释放数据，将窗.口清空回复到初始化状态
    virtual void Uninit() = 0;

    // 刷新窗口
    virtual void UpdateWnd(bool bUIReturn = false);

    // 焦点改变.
    // | bFocused | [in] 为将要改变的焦点状态. true为获取焦点，false为失去焦点
    virtual void OnFocusChanged(bool bFocused);

    // 是否跳过Return To Idle，包括通话、Idle
    virtual bool IsRejectReturnToIdle();

    //回到Idle前的处理函数
    virtual void OnReturnToIdle();

public:
    //设置数据
    virtual void SetData(void * pData) = 0;

public:
    // 是否显示Softkey，默认认为显示
    virtual bool IsShowSoftkey();

    // about dsskey api
    // 是否显示Dsskey，默认认为不显示 bExpend
    virtual bool IsShowDsskey(bool & bExpend);

#if IF_TALKUI_BG_PICTURE
    ////////////////背景///////////////////////////
    //设置整个大背景的图片（不是界面自身的背景）
//     virtual xBrush GetBackGroundBrush(bool& bResult);
    ///////////////////////////////////////////////
#endif

    // 是否允许键盘事件通过
    //virtual bool AllowKeyPadEventOnKeyLock(int nKeyCode, bool bPress);


    //////////////////处理弹出框回调///////////////
    //virtual void MessageBoxCallBack(CMessageBoxBase* pMessageBox) {}
    ///////////////////////////////////////////////////

    // 是否为顶层窗体
    virtual bool IsTopWnd();

    ////////////////////////////////////////////////
    //virtual xView* GetFocusWidget();
    ////////////////////////////////////////////////


    ////是否显示title
    //virtual bool IsShowTitle();
    ////获取显示title信息
    //virtual void GetTitleInfo(CTalkTitleInfo& objTalkTitleInfo);

public:
    virtual bool onKeyPressedEvent(int key);

protected:
    virtual bool OnSoftkeyAction(int iType, bool bLongPress);

protected:
    //刷新显示当前焦点项序号信息
    void UpdatePageInfo(int nTotalCount = -1);
    void HidePageInfo();

    void FocusList(bool bCanFocus);

    //刷新 softkey
    void UpdateDlgSoftkey();

protected:
    xTextView * m_pTitle;       //显示title
    xTextView * m_pPageInfo;    //显示选择页信息

    xListView * m_pListView;    //列表
};

}           //namespace talkui


#endif  //_DLG_TALK_MENU_BASE_H_
