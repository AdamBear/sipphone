#ifndef __UI_MANAGER__
#define __UI_MANAGER__

#include "autovolumebar.h"

typedef void (*FunCustomSoftkey)(CArraySoftKeyBarData & objSoftkeyData);

// 全局窗口管理（窗口堆栈和窗口缓存的管理）
class CUIManager : public xTimerHandler
{
    typedef YLList<CBaseDialog *> List_Wnd;
    typedef YLList<ON_TOPWND_CHANGE_PROC>   ListTopWndChangeProc;

public:
    CUIManager();
    ~CUIManager();

    static CUIManager & GetInstance();

    // 键盘回调函数
    static bool UIManagerHookCallbackFun(int nKeyCode, bool bPress);
    static void EnableSoftkey(bool bEnable);

public:
    // 初始化
    void Init();
    // 释放
    void Uninit();

    // 窗口入栈
    void PushWnd(CBaseDialog * pWnd, int nShowCmd = xView::SHOW_CMD_DEFAULT);
    // 窗口出栈
    void PopWnd(CBaseDialog * pWnd);

    // 获取顶层窗口
    CBaseDialog * GetTopWnd();
    // 设置顶层窗口（将界面提到同层级的顶层）
    bool SetTopWnd(CBaseDialog * pWnd);

    // 返回Idle界面, bClickToIdle是否为点击IDLE发起的回退
    void ReturnToIdle(bool bClickToIdle = false);

    // 更新界面（参数为窗口名）
    void UpdateWnd(const yl::string & strDlgObjectName, int nShowCmd = xView::SHOW_CMD_DEFAULT, bool bUIReturn = false);
    // 更新界面（参数为窗口模板中对应的小窗口）
    void UpdateWnd(CBaseDialog * pDlg, int nShowCmd = xView::SHOW_CMD_DEFAULT, bool bUIReturn = false);

    // 获取小窗口
    CBaseDialog * CreatePrivateDlg(const yl::string & strDlgClassName,
                                   const yl::string & strDlgObjectName = "",
                                   bool bWaitSetActive = false);
    // 获取小窗口，参数为窗口名
    CBaseDialog * GetPrivateDlg(const yl::string & strDlgObjectName);
    // 还回小窗口，参数为窗口指针
    void PutPrivateDlg(CBaseDialog * pDlg);
    // 还回小窗口，参数为窗口名
    void PutPrivateDlg(const yl::string & strDlgObjectName);

    // 顶层窗口类型
    EWindowType GetTopWndType();
    // 判断某种界面类型是否存在
    bool IsWndExist(EWindowType eType);

    // 注册顶层窗口改变时候的回调
    void RegisterTopWndChangeCallBack(ON_TOPWND_CHANGE_PROC funCallBack);
    // 注销顶层窗口改变时候的回调
    void UnregisterTopWndChangeCallBack(ON_TOPWND_CHANGE_PROC funCallBack);

    //是否开启计时器
    void SetIdleCheckTimerActive(bool bActive);

    void LockReturnToIdle(bool bLock);
    void ResetReturnToIdle();

    CAutoVolumeBar * GetVolumeBar()
    {
        return m_pAutoVolumeBar;
    }
    bool ProcessVolumeBarKey(int nKeyCode, bool bPress);
    void SetVolumeBarVisible(bool bVisible);

    // 设置背景
    void SetFrameBackGround(xBrush brushBackGround);

    void UpdateFrameBackGround();

    void SetCustomSoftkeyCallback(FunCustomSoftkey pFunCustomSoftkey);

    void GetCustomSoftkeyData(CArraySoftKeyBarData & objSoftkeyData);

    bool IsEnableCustomSoftkey();

private:
    // 隐藏其他窗口
    void HideOtherWnd(CBaseDialog * pNotHideWnd);

    //是否将大背景、弹框背景、音量条都放到uimanager中统一管理
    //60秒自动返回idle还未实现
    void PreReturnToIdle();

    // 将所有非Idle界面的窗口出栈
    void PopupNotIdleWnd(bool bClickToIdle = false);

    // 当顶层窗口改变时
    void OnTopWndChange(bool bBeforeTopWndChange,
                        CBaseDialog * dlgPreTop,
                        CBaseDialog * dlgNewTop);

private:
    // 检测界面状态定时器
    void OnIdleCheckTimer(xThreadTimer* timer, LPVOID pExtraData);

private:
    // 大窗口堆栈
    List_Wnd                m_ListWnd;
    // 小窗口缓存
    CDialogCache            m_objDialogCache;
    // 顶层窗口改变的回调函数
    ListTopWndChangeProc    m_listTopWndChangeProc;

    xThreadTimer            m_IdleCheckTimer;
    // 是否锁定返回idle的计时器
    bool                    m_bLockRetrunToIdle;

    CAutoVolumeBar     *    m_pAutoVolumeBar;

    static bool             s_bSoftkeyEnable;

    FunCustomSoftkey        m_pFunCustomSoftkey;
};

// 获取全局唯一的窗口管理对象
#define _UIManager CUIManager::GetInstance()

#endif // __UI_MANAGER__
