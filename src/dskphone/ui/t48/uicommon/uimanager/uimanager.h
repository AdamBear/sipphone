#ifndef __UI_MANAGER__
#define __UI_MANAGER__

class CBaseMainWnd;
// 全局窗口管理（窗口堆栈和窗口缓存的管理）
class CUIManager
{
    typedef list<CBaseDialog *> List_Wnd;
    typedef List_Wnd::iterator ItWnd;
    typedef list<ON_TOPWND_CHANGE_PROC> ListTopWndChangeProc;
public:
    // 初始化
    void Init();
    // 释放
    void Uninit();

    // 窗口入栈
    void PushWnd(CBaseDialog * pWnd);
    // 窗口出栈
    void PopWnd(CBaseDialog * pWnd);
    // 获取顶层窗口
    CBaseDialog * GetTopWnd();
    // 设置顶层窗口
    bool SetTopWnd(CBaseDialog * pWnd);

    // 退出联系人和Calllog模块界面
    void ExitDirAndCalllogWnd();

    // 返回Idle界面, eReturnIdle为发起回退的原因
    void ReturnToIdle(RETURN_IDLE_TYPE eReturnIdle = RETURN_IDLE_TYPE_NORMAL);
    // 更新界面（参数为窗口名）
    void UpdateWnd(const yl::string & strDlgObjectName);
    // 更新界面（参数为窗口模板中对应的小窗口）
    void UpdateWnd(CBaseDialog * pDlg);

    // 获取小窗口
    CBaseDialog * CreatePrivateDlg(const yl::string & strDlgClassName,
                                   const yl::string & strDlgObjectName = "");
    // 获取小窗口，参数为窗口名
    CBaseDialog * GetPrivateDlg(const yl::string & strDlgObjectName);
    // 还回小窗口，参数为窗口指针
    void PutPrivateDlg(CBaseDialog * pDlg);
    // 还回小窗口，参数为窗口名
    void PutPrivateDlg(const yl::string & strDlgObjectName);

    // 设置主窗口
    void SetMainWnd(CBaseMainWnd * pMainWnd);
    // 获取主窗口
    CBaseMainWnd * GetMainWnd();
    // 显示主窗口
    void ShowMainWnd();

    // 顶层窗口类型
    EWindowType GetTopWndType();

    // 注册顶层窗口改变时候的回调
    void RegisterTopWndChangeCallBack(ON_TOPWND_CHANGE_PROC funCallBack);
    // 注销顶层窗口改变时候的回调
    void UnregisterTopWndChangeCallBack(ON_TOPWND_CHANGE_PROC funCallBack);

    // 判断某种界面类型是否存在
    bool IsWndExist(EWindowType eType);

private:
    // 用于判断窗口是否返回IDLE的函数对象
    class CCheckWndRejectReturnToIdle
    {
    public:
        CCheckWndRejectReturnToIdle(RETURN_IDLE_TYPE eReturnIdle) : m_eReturnIdle(eReturnIdle) {}
        bool operator()(CBaseDialog * pBaseDlg) const
        {
            return (pBaseDlg != NULL && !pBaseDlg->IsRejectReturnToIdle(m_eReturnIdle));
        }

    private:
        RETURN_IDLE_TYPE m_eReturnIdle;
    };

private:
    // 隐藏其他窗口
    void HideOtherWnd(CBaseDialog * pNotHideWnd);

    // 将所有非Idle界面的窗口出栈
    void PopupNotIdleWnd(RETURN_IDLE_TYPE eReturnIdle = RETURN_IDLE_TYPE_NORMAL);

    // 当顶层窗口改变时
    void OnTopWndChange(CBaseDialog * dlgPreTop, CBaseDialog * dlgNewTop);

protected:
    // 主窗口指针
    CBaseMainWnd  * m_pMainWnd;

private:
    // 大窗口堆栈
    List_Wnd                m_ListWnd;
    // 小窗口缓存
    CDialogCache            m_objDialogCache;
    // 顶层窗口改变的回调函数
    ListTopWndChangeProc    m_listTopWndChangeProc;
};

// 获取全局唯一的窗口管理对象
#define _UIManager GetUIManagerInstance()
CUIManager & GetUIManagerInstance();

#endif // __UI_MANAGER__
