#ifndef CDLGTALKUIFRAME_H
#define CDLGTALKUIFRAME_H

//mod-STL
#include <list>

//mod-QT
#include <QDialog>

//mod-UIManager
#include "uimanager/uimanager_inc.h"

//mod-this
#include "talkui_define.h"
#include "dlgbasetalkui.h"
#include "talkuiframetopbar.h"
#include "talksessiontab.h"
#include "voice/include/modvoice.h"

//前置声明
class QDtmfLine;
class qScrollTextLabel;
//class CTalkTabManager;

//TalkUI窗口框架，即外部统一模板，内部采用第三层窗口进行刷新
class CDlgTalkUIFrame : public CBaseDialog
{
    Q_OBJECT

public:
    //构造函数
    CDlgTalkUIFrame(QWidget * pParent = 0);
    //析构函数
    virtual ~CDlgTalkUIFrame();

public:
    //取得实例
    static CBaseDialog * CreateInstance();

    // 初始化
    virtual void Init();

    // 释放数据,将窗口清空回复到初始化状态
    virtual void Uninit();

    // 设置小窗口数据
    virtual void SetData(void * pData = NULL);

    // 是否显示Title
    virtual bool IsShowTitle(TitleLayoutInfo & objInfo);

    virtual void AfterShowTitle();

    //是否显示音量条
    virtual bool IsShowVolumeBar(int & iVolumeType, int & iMin, int & iMax,
                                 QRect & rtVolume, yl::string & strBGPath) const;

    // 是否显示Dsskey，默认认为显示
    virtual bool IsShowDsskey(bool & bExpend);

    // 焦点改变.
    // | bFocused | [in] 为将要改变的焦点状态. true为获取焦点，false为失去焦点
    virtual void OnFocusChanged(bool bFocused);

    // 获取界面的背景图片
    virtual QPixmap GetCustomBackGround();

    // 通道键处理
    virtual bool OnChannelKeyPress(QObject * pObject, QEvent * pEvent, int iKey);
    // 功能键处理
    virtual bool OnFunckeyPress(QObject * pObject, QEvent * pEvent, int iKey, bool bEXP = FALSE);
    // 数字键处理
    virtual bool OnDigitkeyPress(QObject * pObject, QEvent * pEvent, int iKey);

    virtual bool IsShowBackButton()
    {
        return false;
    }

    virtual bool IsShowHomeButton()
    {
        return false;
    }

    virtual bool OnAction(const QString & strAction);

    virtual bool OnTransparencyChange();

    void ResetWidgetByContactPhotoVisibleChange(bool bUpdate = true);

public Q_SLOTS:
    //由于联系人头像显示的变化导致的布局变化
    void SlotResetWidgetByContactPhotoVisibleChange();

public:
    // 设置子窗口指针
    void SetTalkUI(CDlgBaseTalkUI * pBaseTalkUI, void * pData = NULL);

    // 获取子窗口指针
    CDlgBaseTalkUI * GetTalkUI() const;

    // 发送焦点变化事件
    void SendFocusChangeAction(bool bFocus);

    //设置加密图标提示
    void SetEncryptTip(bool bEncrypt);

    //设置录音图标提示
    void SetRecordTip(ICON_TYPE eIconType);

    // 设置当前通话窗口类型，包括设置窗口大小等
    EWindowType SetTalkUIWindowType();

    //设置DTMF条是否显示
    void SetDTMFLineVisible(bool bVisible);

    void VisibleCCStatusLabel(bool bVisible);

    //处理各种类型的通话提示类型
    void ProcessTalkTipType(TALK_TIP_TYPE eType);

    //检查网络，异常则给出提示，否则清除提示
    void CheckNetwork();

    //设置提示区域的显示策略
    void SetTipRegionVisiblePolicy();

#ifdef IF_FEATURE_BROADSOFT_CALL_CENTER
    // 更新Queue状态
    void UpdateQueueStatus(const yl::string & strQueueStatus);
#endif
    // 刷新DND图标
    void RefreshDNDIcon();
    // 刷新蓝牙图标
    void RefreshBlueToothIcon();

#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
    void USBRecordStateChange();
#endif

#ifdef IF_FEATURE_ENTRANCE_GUARD
    // Message处理
    bool OnDoorLineMessage(msgObject & refMessage);
    // 显示文字
    void DrawEntranceGuard(const QString & strContent, bool bAsTxt = true);

    void RefreshDoorLine(bool bShow = true);
#endif

public:
    //事件过滤器
    virtual bool eventFilter(QObject * pObject, QEvent * pEvent);

    bool AllowKeyPadEventOnKeyLock(int nKeyCode, bool bPress);

    // 是否允许鼠标事件通过
    virtual bool AllowMouseEventOnKeyLock(QObject * pObject, QMouseEvent * pEvent);

    //!!!!!,移至RingUI
Q_SIGNALS:
    void RingSilence(bool bIsSilence);

protected:
    //获取子窗口的 softkey
    void GetTalkUISoftkey();

    // 切换到下一个Session
    void ChangeNextSession();

    // 切换到上一个Session
    void ChangePrevSession();

    //获取当前是否要显示 新来电提醒
    bool IsNeedShowIncomingCallNotify();

private:
    // 根据录音图标类型获取图标路径
    LPCSTR GetIconPath(ICON_TYPE eIconType);

    // 删除蓝牙图标
    void DeleteBlueToothIcon();

    //删除BSFT录制图标
    void DeleteBSFTRecordIcon();

    //删除所有图标
    void DeleteAllIcon();

    //显示 来电提醒
    void ShowIncomingCallNotify();

    //隐藏来电提醒
    void HideIncomingCallNotify();

    void DrawDsskeyBtnOnBackground();

    bool IsBluetoothEnable() const;

protected slots:
    //DTMF条可见状态改变
    void OnDTMFLineVisableChanged(bool bVisible);

    //改变Title
    void OnChangeTitle(const yl::string & strTitle);

    // 刷新Softkey数据
    void RefreshSoftkey();

    // 切换话路
    void OnSwitchSession();

    void OnFocusUIGeometryChange(bool bShow, const QRect & geometry);

    //切页操作
    void OnNotifyScroll();

    //来电提醒提示条点击
    void OnIncomingCallNotifyClick();

    // 响应Back按钮的操作
    virtual void OnBackButtonClick();

public:
    void OnAddTitleIcon(const yl::string & strIconPath, bool bAdd);
private:
    //小窗口指针
    CDlgBaseTalkUI   *  m_pTalkUI;

    //DTMF输入框
    QDtmfLine     *     m_pDtmfLine;

    QLabel       *      m_pLabNetworkNote;

    //保存网络状态，判断网络状态有无改变，避免不必要的刷新
    bool                m_bIsCableConnect;


    // 当前Channel
    CHANNEL_MODE        m_eCurrentChannel;

#ifdef IF_FEATURE_BROADSOFT_CALL_CENTER
    // 显示Call Center队列状态的Label
    qScrollTextLabel  * m_pCCStatusLabel;
#endif

    // 录音图标路径
    yl::string          m_strRecordIconPath;

    CTalkTabManager  *  m_pTabManager;

    // http://10.3.5.199/Bug.php?BugID=62243
    CTalkSessionTab     m_qIncomingCallNotify;      //新来电提醒 , 只在通话界面会有提醒

    // 背景
    QPixmap             m_pmBG;

    // 无头像时将小图标移回至通道
    qFrameStatusImage   m_qTopBar;

#ifdef IF_FEATURE_ENTRANCE_GUARD
    QLabel       *      m_pLabEntranceGuard;
    QRect               m_RectDoorLineLast;
#endif
};

#endif // CDLGTALKUIFRAME_H
