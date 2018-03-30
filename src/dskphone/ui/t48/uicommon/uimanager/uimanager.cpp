#include "uimanager_header.h"
#include "uimanager_inc.h"
#include "basemainwnd.h"
#include "dsklog/log.h"
#include <algorithm>

CUIManager & GetUIManagerInstance()
{
    // 用函数内static方式控制全局对象的构造顺序
    static CUIManager s_UIManager;
    return s_UIManager;
}

// 初始化
void CUIManager::Init()
{
    m_objDialogCache.Init();
    m_pMainWnd = NULL;
}

// 释放
void CUIManager::Uninit()
{
    m_objDialogCache.Uninit();
    m_pMainWnd = NULL;
}

// 窗口入栈
void CUIManager::PushWnd(CBaseDialog * pWnd)
{
    if (pWnd == NULL
            || !pWnd->IsTopWnd())
    {
        //非顶层窗口不予入栈
        return;
    }

    // 已经在栈中则不进行处理
    List_Wnd::iterator it = m_ListWnd.begin();
    for (; it != m_ListWnd.end(); ++it)
    {
        if (pWnd == *it)
        {
            return;
        }
    }

    CBaseDialog * pLastTopWnd = GetTopWnd();

    m_ListWnd.push_front(pWnd);

    // 顶层窗口改变
    if (pLastTopWnd != NULL
            && pLastTopWnd != GetTopWnd())
    {
        // 需要通知窗口焦点改变
        OnTopWndChange(pLastTopWnd, pWnd);
        pLastTopWnd->OnFocusChanged(false);
    }
}

// 窗口出栈
void CUIManager::PopWnd(CBaseDialog * pWnd)
{
    if (pWnd == NULL
            || !pWnd->IsTopWnd())
    {
        UIMANAGER_INFO("CUIManager::PopWnd not top wnd");
        // 非顶层窗口无需出栈
        // 但需要将将小窗口释放（或还回到小窗口缓存中）
        m_objDialogCache.PutPrivateDlg(pWnd);
        return;
    }

    CBaseDialog * pLastTopUI = GetTopWnd();

    // 找到对应的窗口，并出栈
    List_Wnd::iterator itBegin = m_ListWnd.begin();
    for (; itBegin != m_ListWnd.end(); ++itBegin)
    {
        if (pWnd == *itBegin)
        {
            m_ListWnd.erase(itBegin);
            break;
        }
    }

    // 将小窗口释放（或还回到小窗口缓存中）
    m_objDialogCache.PutPrivateDlg(pWnd);

    CBaseDialog * pNewTopUI = GetTopWnd();

    // 新旧top不相等，通知新的窗口获取焦点
    if (pLastTopUI != NULL
            && pNewTopUI != NULL
            && pLastTopUI != pNewTopUI)
    {
        OnTopWndChange(pLastTopUI, pNewTopUI);
        pNewTopUI->OnFocusChanged(true);
    }

    UpdateWnd(pNewTopUI);
}

// 获取顶层窗口
CBaseDialog * CUIManager::GetTopWnd()
{
    return m_ListWnd.size() == 0 ? NULL : *m_ListWnd.begin();
}

// 设置顶层窗口
bool CUIManager::SetTopWnd(CBaseDialog * pWnd)
{
    if (pWnd == NULL || pWnd == GetTopWnd())
    {
        return false;
    }

    // 记录当前顶层窗口
    CBaseDialog * pLastTopWnd = GetTopWnd();

    //!!!!!
    bool bRetVal = false;
    // 需要满足条件：是当前堆栈中的窗口
    List_Wnd::iterator itBegin = m_ListWnd.begin();
    for (; itBegin != m_ListWnd.end(); ++itBegin)
    {
        if (pWnd == *itBegin)
        {
            m_ListWnd.erase(itBegin);
            // 将窗口在堆栈中的位置置顶
            m_ListWnd.push_front(pWnd);
            bRetVal = true;
            break;
        }
    }

    OnTopWndChange(pLastTopWnd, pWnd);

    // 通知之前的top窗口变成非焦点
    if (pLastTopWnd != NULL)
    {
        // 当窗口从焦点成为非焦点时调用各模块提供的回调函数
        pLastTopWnd->OnFocusChanged(false);
    }

    // 通知新的焦点窗口
    pWnd->OnFocusChanged(true);

    return bRetVal;
}

// 隐藏其他窗口
void CUIManager::HideOtherWnd(CBaseDialog * pNotHideWnd)
{
    // 如果pNotHideWnd==NULL则隐藏所有窗口
    List_Wnd::iterator itBegin = m_ListWnd.begin();
    for (; itBegin != m_ListWnd.end(); ++itBegin)
    {
        // 将其他窗口的小窗口隐藏
        CBaseDialog * pWnd = (*itBegin);
        if (pWnd != NULL && pWnd != pNotHideWnd)
        {
            pWnd->hide();
        }
    }
}

// 将所有非Idle界面的窗口出栈
void CUIManager::PopupNotIdleWnd(RETURN_IDLE_TYPE eReturnIdle/* = RETURN_IDLE_TYPE_NORMAL*/)
{
    // 寻找窗口
    CCheckWndRejectReturnToIdle ObjFunCheck(eReturnIdle);
    ItWnd it = find_if(m_ListWnd.begin(),
                       m_ListWnd.end(),
                       ObjFunCheck);

    while (it != m_ListWnd.end())
    {
        CBaseDialog * pBaseDlg = (*it);
        m_ListWnd.erase(it);

        if (NULL != pBaseDlg)
        {
            //调用ReturnToIdle函数
            pBaseDlg->OnReturnToIdle();

            // 清除该窗口
            PutPrivateDlg(pBaseDlg);
        }

        it = find_if(m_ListWnd.begin(),
                     m_ListWnd.end(),
                     ObjFunCheck);
    }
}

// 退出联系人和Calllog模块界面
void CUIManager::ExitDirAndCalllogWnd()
{
    // 记录上一次的顶层窗口
    CBaseDialog * pLastTopWnd = GetTopWnd();

    // 找到对应的窗口，并出栈
    List_Wnd::iterator itBegin = m_ListWnd.begin();
    while (itBegin != m_ListWnd.end())
    {
        CBaseDialog * pWnd = (*itBegin);
        if (pWnd != NULL
                && (pWnd->GetWindowType() == eWT_CallLog
                    || pWnd->GetWindowType() == eWT_Directory))//TODO:逻辑耦合需要修改!!!!!!
        {
            PutPrivateDlg(pWnd);

            // 通话界面只能由通话主动退出
            itBegin = m_ListWnd.erase(itBegin);
        }
        else
        {
            //此处再前移一个
            ++itBegin;
        }
    }

    // 更新最新的顶层窗口
    CBaseDialog * pTopWnd = GetTopWnd();
    if (pTopWnd != NULL)
    {
        // 当窗口从非焦点pop出来成为焦点时调用各模块提供的回调函数
        if (pLastTopWnd != pTopWnd)
        {
            pTopWnd->OnFocusChanged(true);
        }

        UpdateWnd(pTopWnd);
    }
}

// 返回Idle界面
void CUIManager::ReturnToIdle(RETURN_IDLE_TYPE eReturnIdle/* = RETURN_IDLE_TYPE_NORMAL*/)
{
    //回到Idle之前的预操作
    if (NULL != m_pMainWnd)
    {
        m_pMainWnd->PreReturnToIdle();
    }

    // 记录上一次的顶层窗口
    CBaseDialog * pLastTopWnd = GetTopWnd();

    // 将所有非Idle界面的窗口出栈
    PopupNotIdleWnd(eReturnIdle);

    // 更新最新的顶层窗口
    CBaseDialog * pTopWnd = GetTopWnd();
    if (pTopWnd != NULL)
    {
        // 当窗口从非焦点pop出来成为焦点时调用各模块提供的回调函数
        if (pLastTopWnd != pTopWnd)
        {
            OnTopWndChange(pLastTopWnd, pTopWnd);
            pTopWnd->OnFocusChanged(true);
        }

        UpdateWnd(pTopWnd);
    }
}

// 更新界面（参数为窗口名）
void CUIManager::UpdateWnd(const yl::string & strDlgObjectName)
{
    // 找到窗口
    CBaseDialog * pWnd = GetPrivateDlg(strDlgObjectName);
    if (pWnd != NULL)
    {
        // 更新窗口
        UpdateWnd(pWnd);
    }
}

// 更新界面（参数为窗口模板中对应的小窗口）
void CUIManager::UpdateWnd(CBaseDialog * pDlg)
{
    if (pDlg == NULL)
    {
        UIMANAGER_INFO("[UIManager] UpdateWnd dlg is null!");
        return;
    }

    // 如果不是顶层窗口则不更新
    if (pDlg != GetTopWnd())
    {
        UIMANAGER_INFO("[UIManager] Invalid to update wnd which is not on top!");
        return;
    }

    // 更新界面前先将其他小窗口隐藏
    HideOtherWnd(pDlg);

    // 更新窗口数据
    if (m_pMainWnd != NULL)
    {
        m_pMainWnd->UpdateWnd(pDlg);
    }
}

// 获取小窗口
CBaseDialog * CUIManager::CreatePrivateDlg(const yl::string & strDlgClassName,
        const yl::string & strDlgObjectName /* = "" */)
{
    return m_objDialogCache.CreatePrivateDlg(strDlgClassName, strDlgObjectName);
}

// 获取小窗口，参数为窗口名
CBaseDialog * CUIManager::GetPrivateDlg(const yl::string & strDlgObjectName)
{
    return m_objDialogCache.GetPrivateDlg(strDlgObjectName);
}

// 还回小窗口，参数为窗口指针
void CUIManager::PutPrivateDlg(CBaseDialog * pDlg)
{
    m_objDialogCache.PutPrivateDlg(pDlg);
}

// 还回小窗口，参数为窗口名
void CUIManager::PutPrivateDlg(const yl::string & strDlgObjectName)
{
    m_objDialogCache.PutPrivateDlg(strDlgObjectName);
}

// 设置主窗口
void CUIManager::SetMainWnd(CBaseMainWnd * pMainWnd)
{
    m_pMainWnd = pMainWnd;
}

// 设置主窗口
CBaseMainWnd * CUIManager::GetMainWnd()
{
    return m_pMainWnd;
}

// 显示主窗口
void CUIManager::ShowMainWnd()
{
    if (m_pMainWnd != NULL)
    {
        m_pMainWnd->show();
    }
}

// 顶层窗口类型
EWindowType CUIManager::GetTopWndType()
{
    CBaseDialog * pDlg = GetTopWnd();
    if (pDlg == NULL)
    {
        return eWT_Unknown;
    }

    return pDlg->GetWindowType();
}

////----后续要重新整理focusChange的逻辑
void CUIManager::OnTopWndChange(CBaseDialog * dlgPreTop, CBaseDialog * dlgNewTop)
{
    ListTopWndChangeProc::iterator it = m_listTopWndChangeProc.begin();
    for (; it != m_listTopWndChangeProc.end(); ++ it)
    {
        if ((*it) != NULL)
        {
            (*it)(dlgPreTop, dlgNewTop);
        }
    }
}

void CUIManager::RegisterTopWndChangeCallBack(ON_TOPWND_CHANGE_PROC funCallBack)
{
    ListTopWndChangeProc::iterator it = find(m_listTopWndChangeProc.begin(),
                                        m_listTopWndChangeProc.end(),
                                        funCallBack);

    if (it == m_listTopWndChangeProc.end())
    {
        m_listTopWndChangeProc.push_back(funCallBack);
    }
}

void CUIManager::UnregisterTopWndChangeCallBack(ON_TOPWND_CHANGE_PROC funCallBack)
{
    ListTopWndChangeProc::iterator it = find(m_listTopWndChangeProc.begin(),
                                        m_listTopWndChangeProc.end(),
                                        funCallBack);

    if (it != m_listTopWndChangeProc.end())
    {
        m_listTopWndChangeProc.erase(it);
    }
}

// 判断某种界面类型是否存在
bool CUIManager::IsWndExist(EWindowType eType)
{
    List_Wnd::iterator it = m_ListWnd.begin();
    for (; it != m_ListWnd.end(); ++it)
    {
        if (NULL != *it
                && eType == (*it)->GetWindowType())
        {
            return true;
        }
    }

    return false;
}
