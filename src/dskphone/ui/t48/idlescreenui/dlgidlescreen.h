#ifndef IDLE_SCREEN_H
#define IDLE_SCREEN_H

#include <map>
#include <QtGui>
#include "uimanager/basedialog.h"
#include "idlescreen/include/idlescreen_def.h"
#include "baseui/qiconview.h"
#include "extended/xmlbrowser/include/logicanduicommon.h"

#include "xmlbrowserui/cuiregiontextmenu.h"
#include "xmlbrowserui/baselabel.h"

// 前向声明
class qSoftButton;
class CNoteLabel;
class XMLLogicToUIData;
class qDsskeyFrame;

class CXMLBrowserUIFrame;
class CMessageBoxBase ;

// idleScreen类
class CDlgIdleScreen : public CBaseDialog
{
    Q_OBJECT

public:
    CDlgIdleScreen();
public://lcm
    virtual void MessageBoxCallBack(CMessageBoxBase * pMessageBox) ;

    virtual ~CDlgIdleScreen();

    // 初始化
    virtual void Init();

    // 释放数据，将窗口清空回复到初始化状态
    virtual void Uninit();

    // 是否显示Title
    virtual bool IsShowTitle(TitleLayoutInfo & objInfo);

    // 焦点改变.
    virtual void OnFocusChanged(bool bFocused);

    // 设置小窗口数据
    virtual void SetData(void * pData = NULL);

    //是否显示音量条
    virtual bool IsShowVolumeBar(int & iVolumeType, int & iMin, int & iMax,
                                 QRect & rtVolume, yl::string & strBGPath) const;

    virtual bool IsAllowKeyguardCount()
    {
        return true;
    }

    // 是否显示Dsskey，默认认为显示
    virtual bool IsShowDsskey(bool & bExpend);

    virtual bool IsUseTransparency()
    {
        return true;
    }

    // 返回该窗口指针
    static CBaseDialog * CreateInstance();

    // 构造数据
    void InitData();

    // 刷新XMLBrowser的数据
    void UpdateXMLBrowser(const XMLLogicToUIData & objData);

    // 激活XMLBrowser
    void ActivateXMLBrowser(bool bActivate);

    //自定义背景图片
    virtual QPixmap GetCustomBackGround();

    //设置提示内容
    void SetNoteContent(const QString & strIconPath, const QString & strNoteText);

    BOOL OnACDHotelingMsg(msgObject & objMessage);

    // 处理ProgramableKey配置改变
    void OnProgramableKeyConfigChange();

    // 重置键盘锁的计时器
    void ResetKeypadLock();

    // 设置XMLBrowser的数据
    void SetCustomData(YLList<CustomNotify> & listNotifyData);

    // 刷新XMLBrowser的数据
    void RefreshCustomData();

    void OnScreensaverWaitTimeChange();

#if !IF_FEATURE_SHOW_DEFAULT_ACCOUNT
    //更新默认账号label
    void UpdateDefaultAccountLabel();
#endif

    // 切换Dsskey页面
    bool OnDssKeyPageAction();

    virtual bool IsShowBackButton()
    {
        return false;
    }

    virtual bool IsShowHomeButton()
    {
        return false;
    }
    void UpdateCallForwardNotify();
    void UpdateTextMsgNotify();
    void UpdateMissCallNotify();
    void UpdateVoiceMailNotify();
    void OnRTPActiveChange();
    void ProcessFlexibleMsg(msgObject & objMessage);
    void UpdateFeatureSoftkey();

#if IF_FEATURE_DND_LARGE_ICON
    // 刷新状态
    void RefreshStatus(const YLList<IdleStatusItem> & listStatusItem);
    void CheckDNDStatus();
#endif

    void ShowDsskeyLockPassword(int iDssKeyID);

#if IF_BUG_33812
    void InitWallpaperLogo();
    void CheckWallpaperLogo(bool bShow = true);
#endif

#if IF_FEATURE_WALLPAPER_MODE
    void CheckWallpaperChange();
    bool SetBackgroundColor(yl::string & strBackgroundColor);
    void DrawWallpaper(QPixmap & pBackGround);
#endif

public:
    //事件过滤器
    //在idle界面点击ok后，应当进入status界面显示
    virtual bool eventFilter(QObject * pObject, QEvent * pEvent);

    // Balance消息处理
    void ProcessBalanceMsg(const QVariant & objData);

    void ProcessOnFocusEvent();

    void StartScreensaverTimer();

#if IF_FEATURE_XMLBROWSER_HOTELING
    void UpdateXMLBrowserHotelingStatus(int iType);
    void ResetHotelingLogoutPromptTime();

    void StartHotelingLogoutTime();
#endif

    void ResetAutoPWaitTime(int nTimes);

    // 处理屏保定时器到时场景
    void ProcessScreensaverTimeOut();
private:
    // 处理UI的操作
    virtual bool OnAction(const QString & strAction);

    // 获取默认铃声
    yl::string GetCurrentRingPath();

    // 插入Softkey
    bool InsertSoftkey(int iIndex, const CSoftKeyBarData & softKeyData);

    // XMLBrowser是否激活
    bool IsXMLBrowserActivate();

    // 是否开启ACD队列信息
    void SetACDQueStatActivate(bool bActivate);

    //控制位置重叠的控件的显示
    void checkXMLframeAndACDQueStat();
    void CheckNoteLabAndAccountLab();

    //展开dsskeyframe时隐藏dsskeyframe位置上的控件
    void HideWidgets();

    //收起dsskeyframe时重新显示dsskeyframe位置上的其它控件
    void ShowWidgets();

    // 更新界面背景
    void RefreshBackGroundPic();
    // 产品部新需求：
    // 只开Hoteling, 则GuestIn/GuestOut显示在第二页Menu后面
    // 同时开启ACD和Hoteling, 则Login/Logout也显示在第二页Menu后面
    // 只ACD，则Login/Logout保持原样, 显示在第一页第二个
    void SetHotelingSoftkey();
    void SetACDSoftkey(bool bShowLock = false);
    void SetHotelingAndACDSoftkey();
    void SetLogoutAndLockSoftkey(int & iIndex, const CSoftKeyBarData & softKeyData);

    void StopScreensaverTimer();

private slots:
    //长按ok的操作
    void OnRecoverLongPress();

    //长按Cancel的操作
    void OnCancelLongPress();

    // 长按#号键的操作
    void OnPoundLongPress();
    //长按Message  或长按Mute
    void OnRebootWeb();

    // 延迟检测网络是否改变
    void OnDelayChecknNetworkChange();

    // 延迟检测弹出框
    void OnDelayCheckPopBox();

    void OnEnterScreensaver();

    bool OnTransparencyChange();

    void OnHotelingLogoutPrompt();

    void OnAutoPHintTimeout();

    void OnUpdateConfigTimeout();

protected slots:
    // 音量条可见状态的槽函数
    void OnVolumeBarVisibleChanged(bool bVisible);

    // 音量大小改变的槽函数
    void OnVolumeBarValueChanged(int iValue);

protected:
    void UpdateSoftKeyCache();
    void UpdateSoftKeyIcon(int nDssKeyType, QString & strIcon);
    void InitDsskeyFrame();

    void GetScreensaverWaitTime();

    yl::string GetCurrentBackground();
    void DrawDsskeyBtnOnBackground();

private:
    //长按OK键计时
    QTimer m_timerOK;

    //长按Cancel键
    QTimer m_timerCancel;

    // 长按#号键计时
    QTimer m_timerPound;

    //长按message键或长按Mute键
    QTimer m_timerRebootWeb;

    // AutoP提示框时间
    QTimer m_timerAutoPHint;

    //长按Redial键更新配置
    QTimer m_timerUpdateConfig;

    //长按的时间
    static const int LONG_PRESS_TIME = 2000;

    // OK键长按相应时间
    static const int OK_LONG_PRESS_TIME = 5000;

    //记录计时器是否超时，以判断是否进行status界面
    bool m_bIsTimeOut;

    //指向自己的指针
    static CDlgIdleScreen * m_pToMyself;

    // XMLFrame
    CXMLBrowserUIFrame * m_pXMLFrame;

    //用于界面提示的label
    CNoteLabel * m_pNoteLabel;
#if !IF_FEATURE_SHOW_DEFAULT_ACCOUNT
    //用于当前默认账号的label
    QLabel * m_pLabDefaultAccount;
#endif
    //用于ACD Queue Status Notification信息提示
    QLabel * m_pLabACDQueStat;

#if IF_FEATURE_DND_LARGE_ICON
    //DND 大图标
    QLabel * m_pDNDIcon;
    QLabel * m_pDNDLabel;
    bool    m_bShowDND;
#endif

    // 保存xml刷新数据的链表
    YLList<CustomNotify>    m_listCustomData;

    // 控制是否显示mainwnd的DsskeyFrame
    bool        m_bIdleDsskey;

    bool        m_bIsPressed;

    // idle界面自己的DsskeyFrame
    qDsskeyFrame * m_pIdleDsskey;
    bool m_bXMLActive;

    QPixmap         m_pmBG;
    yl::string      m_strBGPicPath;

    // 屏保
    QTimer m_timerScreensaver;

    unsigned int    m_nScreenSaverWaitTime;

#if IF_BUG_33812
    // 在背景上面增加一个logo
    QLabel     *    m_pWallPaperLogo;
#endif
#if IF_FEATURE_WALLPAPER_MODE
    WALLPAPAER_MODE m_eWallpaperMode;
    QColor          m_BackgroundColor;
#endif
#if IF_FEATURE_XMLBROWSER_HOTELING
    QTimer m_timerHotelingLogoutPrompt;
    YLVector<int> vecLogoutPrompt;
#endif
};

//获取单例idleScreen 指针
#define  g_pIdleScreenUI ((CDlgIdleScreen*)(CDlgIdleScreen::CreateInstance()))
#endif // IDLE_SCREEN_H
