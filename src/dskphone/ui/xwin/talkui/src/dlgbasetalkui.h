#ifndef _TALKUI_DLG_BASE_TALKUI_H
#define _TALKUI_DLG_BASE_TALKUI_H

#include <xwindow/xWindow.h>
#include "uimanager/basedialog.h"

/////////////////// talkui ///////////////////////
//#include "talkbaseui.h"
#include "talkbaseui.h"
#include "talkuititlebar.h"
#include "dlgselectlineui.h"

/////////////////// talkuilogic ///////////////////////
#include "talkuilogic/talkuilogiccommon.h"
#include "talkuilogic/basetalkuiprocessor.h"

#include "messagebox/modmessagebox.h"

namespace talkui
{

enum TALKUI_TITLE_TYPE
{
    TALKUI_TITLE_TYPE_NORMAL,
    TALKUI_TITLE_TYPE_MENU,
    TALKUI_TITLE_TYPE_NO_TITLE,
};

class CDlgBaseTalkUI : public CTalkBaseUI, public CMsgBoxCallBackAction, public LineSelectResult
{
public:
    CDlgBaseTalkUI(const yl::string & strTalkUIClassName, EWindowType eType = eWT_Talk);
    virtual ~CDlgBaseTalkUI();

public:
    void JudgeLoadLayout();
private:
    virtual void LoadLayout() = 0;

public:
    //获取session状态
    SESSION_STATE GetSessionState();

    virtual bool GetIsFullScreen();

public:
    //硬按键预处理
    virtual bool OnKeyPreProcess(int iKey);

    //硬按键按下处理
    virtual bool OnKeyPress(int iKey);

    // 通道键处理
    virtual bool OnChannelKeyPress(int iKey);

    // 功能键处理
    virtual bool OnFunckeyPress(int iKey);

    // 数字键处理
    virtual bool OnDigitkeyPress(int iKey);

    //按键弹起处理
    virtual bool OnKeyRelease(int iKey);

public:
    //音量条改变
    virtual void OnTalkVolumnValueChange(int iValue);
    //音量条显示状态改变
    virtual void OnTalkVolumnVisibleChange(bool bVisible);

public:
    //刷新通话UI数据  这边需要从processor 获取 数据 设置到通话UI子界面上
    virtual void RefreshTalkUIData()  = 0;

    //当新的session和当前焦点session一致时，界面是否需要重刷
    virtual bool NeedReUpdate()
    {
        return true;
    }

public:
    //刷新通话时间显示
    virtual void UpdateTalkUITime(uint iHour, uint iMin, uint iSecond);
    //获取通话图标
    virtual void GetIconList(YLList<TALKUI_ICON>& listIcons);

public:
    //统一在 UILOGIC中   CUILogicBaseTalkUI 处理
    ////自身发起刷新
    ////刷新softkey
    //void UpdateTalkUISoftkey();

public:
    //是否显示DTMF条
    virtual bool IsShowDtmfLine() const;
    // 是否显示切换话路的滚动条
    virtual bool IsShowSessionScrollBar();
    // 能否切换Session
    virtual bool IsChangeSession(int iKey = PHONE_KEY_UP);

//继承talkuibaseui ///////////////////////////////////
public:
    // 初始化
    virtual void Init();
    // 释放数据，将窗口清空回复到初始化状态
    virtual void Uninit();

    // 刷新窗口
    virtual void UpdateWnd(bool bUIReturn = false);

    //内部控件焦点改变
    virtual bool onFocusMove(xView * pNewFocus, xView * pOldFocus);

    // 焦点改变.
    // | bFocused | [in] 为将要改变的焦点状态. true为获取焦点，false为失去焦点
    virtual void OnFocusChanged(bool bFocused);

    // 是否跳过Return To Idle，包括通话、Idle
    virtual bool IsRejectReturnToIdle();

    //回到Idle前的处理函数
    virtual void OnReturnToIdle();

    virtual void OnReturnToIdleAction() {}

    //获取窗体类型
    EWindowType GetWindowType() const;

    //获取title类型
    TALKUI_TITLE_TYPE GetTitleType() const;

    // 获取当前页面音量条的数据
    // 返回值：是否显示音量条,默认不显示音量条
    // iVolumeType | [out] 音量条的类型
    // iMin | [out] 音量条的最小值
    // iMax | [out] 音量条的最大值
    // strBGPath | [out] 音量条的背景图片
    virtual bool IsShowVolumeBar(int & iVolumeType, int & iMin, int & iMax, yl::string & strBGPath);

    //获取Softkey数据
    virtual void GetSoftkey(CArraySoftKeyBarData & objSoftkeyData);

    ////设置Softkey数据
    //virtual void SetSoftKey(const CArraySoftKeyBarData& objSoftkeyData);

    // 是否显示Softkey，默认认为显示
    virtual bool IsShowSoftkey();

#if IF_SUPPORT_SPECIAL_SOFTKEY_BG
    virtual yl::string GetSoftkeyBG();
#endif

    // about dsskey api
    // 是否显示Dsskey，默认认为不显示 bExpend
    virtual bool IsShowDsskey(bool & bExpend);

#if IF_TALKUI_BG_PICTURE
    ////////////////背景///////////////////////////
    //设置整个大背景的图片（不是界面自身的背景）
    virtual xBrush GetBackGroundBrush(bool& bResult);
    //界面自身背景
    virtual yl::string getTalkUIBackGround();
    ///////////////////////////////////////////////
#endif

    // 是否允许键盘事件通过
    virtual bool AllowKeyPadEventOnKeyLock(int nKeyCode, bool bPress);

    //////////////////弹出security框///////////////
    virtual void ShowSecurityMessageBox(int nAccount);

    //处理security回调
    virtual void ProcessSecurityCallBack(const yl::string & strResult) {}

    //////////////////处理弹出框回调///////////////
    virtual void MessageBoxCallBack(CMessageBoxBase * pMessageBox);

public:
    virtual void OnLineSelectResult(LINE_SELECT_RESULT eResult, int nSelectIndex, int nSelectID) {}

    // 是否为顶层窗体
    virtual bool IsTopWnd();


    ////////////////////////////////////////////////
    virtual xView * GetFocusWidget();
    ////////////////////////////////////////////////

public:
    //处理softkey
    virtual bool OnSoftkeyPress(SoftKey_TYPE eSoftType, bool bLongPress);

    //继承talkuibaseui 结束 ///////////////////////////////////

public:
    //获取 processor
    virtual CBaseTalkUIProcessor * GetTalkUIProcessor() = 0;
    //获取sessionID
    int GetSessionID();

public:
    ////根据softkeytype 获取 softkeybardata
    //void GetSoftkeyBarDataFromSoftKeyType(CSoftKeyBarData& softkeyBarData, SoftKey_TYPE eSoftkeyType);

protected:
    //窗体类型
    EWindowType             m_eWndType;

    TALKUI_TITLE_TYPE   m_eTitleType;

    bool                                m_bShowTimePiece;

protected:
    YLList<yl::string> m_lstAllowLevels;//安全等级列表

    bool m_bLayoutLoaded;
};


}       //namespace talkui


#endif


