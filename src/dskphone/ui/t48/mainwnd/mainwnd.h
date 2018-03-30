#ifndef __MAIN_WND__
#define __MAIN_WND__

#include "uimanager/uimanager_header.h"
#include "uimanager/uimanager_common.h"
#include "uimanager/basemainwnd.h"
#include "uimanager/basedialog.h"

// 前向声明
class qUIPushButton;
class CAutoVolumeBar;
class CMaskFrame;
class CMessageFrame;
class CTitleBar;
class qDsskeyFrame;
class qDsskeyButton;
class CMainWndWgtDelegateBase;

// 定义MainWnd刷新界面前的回调函数
typedef void (*ON_UPDATEWND_HOOK)(const CBaseDialog * pDialog);
typedef std::list<ON_UPDATEWND_HOOK> UpdateWndHookList;
typedef UpdateWndHookList::iterator  IterUpdateWndHook;

namespace mainwnd
{
class CBackGroundFrame: public QWidget
{
    Q_OBJECT

public:
    CBackGroundFrame();
    ~CBackGroundFrame();

public:
    void SetBackGroundPixMap(const QPixmap & pixmapBack);


protected:
    void paintEvent(QPaintEvent * pEvent);

protected:
    QPixmap m_pixmapBack;

};
}

// 全局唯一的主窗口
class CMainWnd : public CBaseMainWnd
{
    friend class CUIManager;
    Q_OBJECT

public:
    static void OnTopWndChangeCallbackFun(CBaseDialog * dlgPreTop, CBaseDialog * dlgNewTop);

public:
    CMainWnd();
    virtual ~CMainWnd();

    // 重置softkey按下状态
    void ResetDownSoftkey();

    // 根据传入的控件指针设置IME的soft信息
    void DisplayIMESoftkey(QWidget * pFocusWidget);

    // 根据传入的控件指针设置下一个IME的soft信息
    void SwitchNextIMEName(QWidget * pFocusWidget);

    //获取音量条对象
    const CAutoVolumeBar * GetVolumeBar();

    //设置音量条的可见性
    void SetVolumeBarVisible(bool bVisible);

    // 刷新音量条图标
    void RefreshVolumeBarIconByChannelType(int nChannelType);

    // 用于提示重启的消息
    // 用户恢复出厂和升级,完成之后要杀GUI进程的情况
    // 普通情况下请勿使用会造成内存泄露
    void ShowRebootMessage(const QString & strText);

    //事件过滤器
    virtual bool eventFilter(QObject * pObject, QEvent * pEvent);

    // 画softkey
    void DrawSoftkey(CBaseDialog * pDialog);

    // 返回Idle前的预操作,由UIManager调用
    virtual void PreReturnToIdle();

    // about title
    // 设置Title的值
    void SetTitle(const QString & strTitle);

    // 设置左边区域文字
    // 显示默认账号类型的时候不生效
    void SetTitleAccountDisplayName(const QString & strText);

    // 设置左边区域图标
    // 显示默认账号类型的时候不生效
    void SetTitleAccountIcon(const QString & strIcon);

    // 设置选择账号的ID,-1为选中默认账号
    void SetTitleSelectAccountID(int nAccountID);

    // 设置Title信息
    void SetTitleLayoutInfo(const TitleLayoutInfo & objTitleInfo);

    QWidget * GetTitle() const;

    //获取灰色遮罩
    CMaskFrame * GetMaskFrame();

    //获取灰色遮罩，原接口先保留
    CMaskFrame * GetGrayWidget();

    //设置灰色遮罩是否显示
    void SetGrayShow(bool bShow = true);

    // 是否开启计时器
    void SetTimerLock(bool bLock);

    // 处理音量条事件
    bool ProcessVolumeBarKeyEvent(int nKeyCode, bool bPress);

    // 处理Dsskey点击
    bool OnLineKeyClick(int iIndex);

    // 根据顶层窗口的类型重新刷新背景
    void UpdateBackGround();

    // 注册刷新窗口时候的钩子
    void RegisterUpdateWndHook(ON_UPDATEWND_HOOK pFun);

    // 取消刷新窗口时候的钩子
    void UnregisterUpdateWndHook(ON_UPDATEWND_HOOK pFun);

    // 设置控件相关的代理类
    void SetSoftkeyDelegate(CMainWndWgtDelegateBase * pDelegate);
    void SetTitleDelegate(CMainWndWgtDelegateBase * pDelegate);
    void SetDsskeyDelegate(CMainWndWgtDelegateBase * pDelegate);
    void SetHomeAndBackBtnDelegate(CMainWndWgtDelegateBase * pDelegate);

    // 画Linekey
    void DrawLinekey(CBaseDialog * pDialog);

    void ShowBlackFrame();
    void HideBlackFrame();

    void HideFullKeyboard();

    // 全键盘显示与隐藏回调函数
    static bool MainWndKeyBoardShowCallbackFun(bool bKeyBoardShow);

protected:
    void paintEvent(QPaintEvent * pEvent);

    // 处理音量条事件
    bool ProcessVolumeKey(int nKeyCode, bool bPress);

Q_SIGNALS:
    void OnPreReturnToIdle();

private:
    //是否开启计时器
    void SetCheckTimerActive(bool bActive);

    // get Wnd type
    // 和autoTest的交互如何处理
    void WndType(const EWindowType & eWindowType);
    // 更新窗口
    virtual void UpdateWnd(CBaseDialog * pDialog);

    // 初始化控件
    void InitWidget();

    // 设置窗口大小及位置
    void SetWndSize();
    // 画Title
    void DrawTitle(CBaseDialog * pDialog);
    // 画各模块的小窗口
    void DrawPrivateDlg(CBaseDialog * pDialog);
    // 绘制背景
    void DrawBackGround(CBaseDialog * pDialog);
    // 绘制HomeButton
    void DrawHomeAndBackBtn(CBaseDialog * pDialog);

    //初始化音量条
    void InitVolumeBar();

    bool GetVolumeBarInfo();
    // 画音量条
    void DrawVolumeBar(CBaseDialog * pDialog);


private slots:
    // 检测界面状态定时器
    void CheckStateTimer();

    bool OnKeyboardShow();

    bool OnKeyboardHide();

private:
    // 定义所有模板需要用到的控件，类似于控件池
    // 背景
    mainwnd::CBackGroundFrame     m_frmBackGround;

    // softkey的代理类
    CMainWndWgtDelegateBase  *  m_pSoftkeyDelegate;

    // Title的代理类
    CMainWndWgtDelegateBase  *  m_pTitleDelegate;

    // Dsskey的代理类
    CMainWndWgtDelegateBase  *  m_pDsskeyDelegate;

    CMainWndWgtDelegateBase  *  m_pHomeAndBackBtnDelegate;

    //音量调节控件成员变量
    CAutoVolumeBar     *    m_pVolumeBar;

    QTimer         *        m_pCheckTimer;
    //计时器是否可开启
    bool                    m_bIsTimerLock;

    // 蒙板
    CMaskFrame       *      m_pMaskFrame;

    UpdateWndHookList       m_listUpdateWndHook;

    QFrame         *        m_pBlackFrame;
};

// 获取全局唯一的主窗口对象
#define _MainWND GetMainWndInstance()
CMainWnd & GetMainWndInstance();

// 接收消息并传递给StatusBarControl的函数
LRESULT MainWndMessage(msgObject & refMessage);

#endif // __MAIN_WND__
