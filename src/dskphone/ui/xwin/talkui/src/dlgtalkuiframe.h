#ifndef _TALKUI_DLG_TALKUI_FRAME_H
#define _TALKUI_DLG_TALKUI_FRAME_H

#include "dlgbasetalkui.h"
#include "talkuititlebar.h"
#include "talkuimenutitle.h"
#include "talkbaseui.h"
#include "publicwidget.h"

#include "uimanager/basedialog.h"

/////////////////////talkuilogic///////////////
#include "talkuilogic/talkuilogiccommon.h"

#include "baseui/include/baseuidefine.h"

namespace talkui
{

class CDlgTalkUIFrame
    : public xLinearLayout
    , public CBaseDialog
{
public:
    //取得实例
    static CBaseDialog * CreateInstance();

public:
    CDlgTalkUIFrame();
    virtual ~CDlgTalkUIFrame();

public:
    // 设置子窗口指针
    void SetTalkUI(CDlgBaseTalkUI * pTalkUI);

    // 获取子窗口指针
    CDlgBaseTalkUI * GetTalkUI();

    void loadTalkContent();

public:
    //给TalkUIFrame设置数据
    void SetData(CUILogicToTalkUIData & dataUILogicToTalkUI);

    //调用完设置数据 SetData 后会调用该函数
    //刷新窗口
    void UpdateTalkUI(bool bNewTalkUI);


public:
    //刷新title
    void UpdateTalkUITitle(yl::string strTitle);

    //改变通道图标
    void UpdateChannelIcon(TALKUI_CHANNEL_ICON eTalkUIChannelIcon);

    //刷新pagenum显示
    //iCurrentIndex是实际的序号 开始序号会在iCurrentIndex的基础上加1
    virtual void UpdateTalkUIPageNum(int iCurrentIndex, int iSessionCount);

    //刷新通话时间显示
    void UpdateTalkUITime(uint iHour, uint iMin, uint iSecond);

    //刷新softkey
    void UpdateTalkUISoftkey();

    //刷新蓝牙图标
    void UpdateBluetoothIcon(TALKUI_BLUETOOTH_ICON eTalkUIBlueToothIcon);

#if IF_TALKUI_USB_ENABLE
    //刷新录制图标
    void UpdateRecordStatus(TALK_RECORD_STATUS eStatus);

    void UpdateRecordIcon(int iState);

    void OnUSBRecordMsgWarning(int iTime);

    void OnUSBRecordMsgNotenough();
#endif

    //更新通话图标
    void UpdateTalkUIIcons();

    virtual DSSKEY_DISPLAY_MODE GetDsskeyDisplayMode();

public:
    // 音量条可见状态的槽函数
    void OnVolumeBarVisibleChanged(xSignal * sender, WPARAM wParam, LPARAM lParam, int nDataBytes,
                                   LPVOID pData);

    //音量值改变
    virtual void OnVolumeValueChanged(xSignal * sender, WPARAM wParam, LPARAM lParam, int nDataBytes,
                                      LPVOID pData);

public:
    //刷新通话UI
    void RefreshTalkUIData();

    virtual bool ProcessKeyEvent(int nKeyCode, bool bPress);

public:
    virtual bool onKeyPressedEvent(int key);
    virtual bool onKeyReleasedEvent(int key);

    virtual bool onFocusMoveEvent(xView * pNewFocus, xView * pOldFocus);

//继承talkuibaseui ///////////////////////////////////
public:
    // 初始化
    virtual void Init();

    // 释放数据，将窗.口清空回复到初始化状态
    virtual void Uninit();

    // 刷新窗口
    virtual void UpdateWnd(bool bUIReturn = false);

    // 焦点改变.
    // | bFocused | [in] 为将要改变的焦点状态. true为获取焦点，false为失去焦点
    virtual void OnFocusChanged(bool bFocused);

    // 是否跳过Return To Idle，包括通话、Idle
    virtual bool IsRejectReturnToIdle();

    //回到Idle前的处理函数
    virtual void OnReturnToIdle();

    virtual void OnReturnToIdleAction();

    //获取窗体类型
    EWindowType GetWindowType() const;

    // 获取当前页面音量条的数据
    // 返回值：是否显示音量条,默认不显示音量条
    // iVolumeType | [out] 音量条的类型
    // iMin | [out] 音量条的最小值
    // iMax | [out] 音量条的最大值
    // strBGPath | [out] 音量条的背景图片
    virtual bool IsShowVolumeBar(int & iVolumeType, int & iMin, int & iMax, yl::string & strPix);

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
#if IF_TALKUI_BG_IDLE_STYLE
    void InitBackground();
#endif
    //设置整个大背景的图片（不是界面自身的背景）
    virtual xBrush GetBackGroundBrush();
    ///////////////////////////////////////////////
#endif

    // 是否允许键盘事件通过
    virtual bool AllowKeyPadEventOnKeyLock(int nKeyCode, bool bPress);


    //////////////////处理弹出框回调///////////////
    //virtual void MessageBoxCallBack(CMessageBoxBase* pMessageBox) {}
    ///////////////////////////////////////////////////

    // 是否为顶层窗体
    virtual bool IsTopWnd();

    ////////////////////////////////////////////////
    virtual xView * GetFocusWidget();
    ////////////////////////////////////////////////


    ////是否显示title
    //virtual bool IsShowTitle();
    ////获取显示title信息
    //virtual void GetTitleInfo(CTalkTitleInfo& objTalkTitleInfo);

protected:
    virtual bool OnSoftkeyAction(int iType, bool bLongPress);

//继承talkuibaseui 结束///////////////////////////////////
#if IF_TITLE_ICON_FIXED
private:
    //重置通道类型图标
    void ResetFixedChannelIcon();
    //设置通道类型图标
    void ShowFixedChannelIcon(TALKUI_CHANNEL_ICON eTalkUIChannelIcon);
    //重置其他提示图标
    void ResetFixedOtherIcon();
    //设置其他提示图标
    bool ShowFixedOtherIcon(TALKUI_ICON iconType);
    void SetFixedIcon(LCD_ICON_TYPE eType, bool bShow);
#endif

//其他函数
public:
#ifdef IF_FEATURE_BROADSOFT_CALL_CENTER
    //更新ACD队列状态
    void UpdateACDQueueStatus(yl::string strACDQueueStatus);
#endif

    //更新网络连接状态  bIsReady = TRUE 连接正常   bIsReady = FALSE 连接不正常
    void UpdateNetworkStatus(TALK_NETWORK_STATUS eStatus);

    //更新WIFI信号状态  bIsReady = TRUE 连接正常   bIsReady = FALSE 连接不正常
    void UpdateWifiStatus(bool bIsReady);

protected:
    void GetStationName();

public:
    CPublicWidget& getPublicWidget()
    {
        return m_PublicWidget;
    }

protected:
    //刷新softkey
    void RefreshSoftkey();

protected:
    // 切换到下一个Session
    void ChangeNextSession();

    // 切换到上一个Session
    void ChangePrevSession();

protected:
    //显示通话子界面
    CDlgBaseTalkUI * m_pTalkUI;

    CTalkUITitleBar m_TitleBar;

    CTalkUIMenuTitle m_MenuTitle;

    CPublicWidget m_PublicWidget;

#if IF_TALKUI_BG_IDLE_STYLE
    xBrush m_brushBG;
#endif

    bool m_bStationName;
    bool m_bStationNameScroll;
    yl::string m_strStationName;
};

}       //namespace talkui

#endif


