#include "uimanager_header.h"
#include "uimanager_inc.h"
#include "dsklog/log.h"
#include "baseui/include/baseuicommon.h"
#include "messagebox/modmessagebox.h"
#include "globalinputhook/modinputhook.h"
#include "xwindow/xWindowManager.h"
#include "keymap.h"
#include "adapterbox/include/modadapterbox.h"


static const int RETURN_IDLE_TIME = 60 * 1000;

bool CUIManager::s_bSoftkeyEnable = true;


#if IF_MULTI_WORKSTATION_ENBALE
IMPLEMENT_MULTI_WORKSTATION_INSTANCE(CUIManager)
#else
IMPLEMENT_WORKSTATION_INSTANCE(CUIManager)
#endif

bool CUIManager::UIManagerHookCallbackFun(int nKeyCode, bool bPress)
{
    CBaseDialog * pDialog = _UIManager.GetTopWnd();
    if (NULL == pDialog)
    {
        return false;
    }

    // 重新计时
    _UIManager.SetIdleCheckTimerActive(!pDialog->IsRejectReturnToIdle());

    //界面处理
    if (pDialog->ProcessKeyEvent(nKeyCode, bPress))
    {
        return true;
    }

    return false;
}

void CUIManager::EnableSoftkey(bool bEnable)
{
    s_bSoftkeyEnable = bEnable;
}

CUIManager::CUIManager()
    : m_bLockRetrunToIdle(false)
    , m_pAutoVolumeBar(NULL)
    , m_pFunCustomSoftkey(NULL)
{
    m_ListWnd.clear();
    m_listTopWndChangeProc.clear();

    m_pAutoVolumeBar = new CAutoVolumeBar();
    if (NULL != m_pAutoVolumeBar)
    {
        m_pAutoVolumeBar->hide();
    }

    inputHook_RegisterKeyPadHookProc(CUIManager::UIManagerHookCallbackFun, HOOK_MODULE_UIMANAGER);
}

CUIManager::~CUIManager()
{

}

// 初始化
void CUIManager::Init()
{
    m_objDialogCache.Init();
}

// 释放
void CUIManager::Uninit()
{
    m_objDialogCache.Uninit();
}

// 窗口入栈
void CUIManager::PushWnd(CBaseDialog * pWnd, int nShowCmd/* = xView::SHOW_CMD_DEFAULT*/)
{
    if (pWnd == NULL
            || !pWnd->IsTopWnd())
    {
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

    pWnd->CreateWindowObject(true);
    pWnd->BringWindowObjectTop();

    CBaseDialog * pLastTopWnd = GetTopWnd();
    bool bTopWndChange = (pLastTopWnd != pWnd);

    if (bTopWndChange)
    {
        OnTopWndChange(true, pLastTopWnd, pWnd);
    }

    m_ListWnd.push_front(pWnd);

    // 顶层窗口改变
    if (pLastTopWnd != NULL
            && bTopWndChange)
    {
        // 需要通知窗口焦点改变
        OnTopWndChange(false, pLastTopWnd, pWnd);
        pLastTopWnd->OnFocusChanged(false);
        pWnd->OnFocusChanged(true);
    }

    UpdateWnd(pWnd, nShowCmd, false);
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

    //idle不能出栈
    if (eWT_Idle == pWnd->GetWindowType())
    {
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

    CBaseDialog * pNewTopUI = GetTopWnd();

    bool bTopWndChange = (pLastTopUI != pNewTopUI);

    if (bTopWndChange)
    {
        OnTopWndChange(true, pLastTopUI, pWnd);
    }

    // 新旧top不相等，通知新的窗口获取焦点
    if (pLastTopUI != NULL
            && pNewTopUI != NULL
            && bTopWndChange)
    {
        OnTopWndChange(false, pLastTopUI, pNewTopUI);
        pLastTopUI->OnFocusChanged(false);
        pNewTopUI->OnFocusChanged(true);
    }

    UpdateWnd(pNewTopUI, xView::SHOW_CMD_DEFAULT, true);

    // 将小窗口释放（或还回到小窗口缓存中）
    m_objDialogCache.PutPrivateDlg(pWnd);
}

// 获取顶层窗口
CBaseDialog * CUIManager::GetTopWnd()
{
    return m_ListWnd.size() <= 0 ? NULL : *m_ListWnd.begin();
}

// 设置顶层窗口
bool CUIManager::SetTopWnd(CBaseDialog * pWnd)
{
    if (pWnd == NULL
            || !pWnd->IsTopWnd()
            || pWnd == GetTopWnd())
    {
        return false;
    }

    // 记录当前顶层窗口
    CBaseDialog * pLastTopWnd = GetTopWnd();

    bool bRetVal = false;
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

    if (!bRetVal)
    {
        PushWnd(pWnd);
        return bRetVal;
    }

    if (pLastTopWnd != pWnd)
    {
        OnTopWndChange(false, pLastTopWnd, pWnd);
    }

    // 通知之前的top窗口变成非焦点
    if (pLastTopWnd != NULL)
    {
        // 当窗口从焦点成为非焦点时调用各模块提供的回调函数
        pLastTopWnd->OnFocusChanged(false);
    }

    // 通知新的焦点窗口
    pWnd->OnFocusChanged(true);

    UpdateWnd(pWnd, xView::SHOW_CMD_DEFAULT, true);
    pWnd->DialogRaise();

    return bRetVal;
}

// 隐藏其他窗口
void CUIManager::HideOtherWnd(CBaseDialog * pNotHideWnd)
{
    bool bHideOther = true;
    if (NULL != pNotHideWnd)
    {
        bHideOther = pNotHideWnd->IsHideOtherWnd();
    }

    List_Wnd listShowWnd;

    List_Wnd::iterator itBegin = m_ListWnd.begin();
    for (; itBegin != m_ListWnd.end(); ++itBegin)
    {
        CBaseDialog * pWnd = (*itBegin);
        if (pWnd != NULL && pWnd != pNotHideWnd)
        {
            // 如果当前窗口是需要隐藏其他窗口的话，则将其他窗口隐藏
            if (bHideOther)
            {
                pWnd->DialogHide();
                continue;
            }

            if (pWnd->IsHideOtherWnd())
            {
                continue;
            }

            listShowWnd.push_front(pWnd);
        }
    }

    for (itBegin = listShowWnd.begin(); itBegin != listShowWnd.end(); ++itBegin)
    {
        CBaseDialog * pWnd = (*itBegin);

        if (NULL != pWnd)
        {
            pWnd->DialogShow(xView::SHOW_CMD_NONE);
        }
    }
}

// 将所有非Idle界面的窗口出栈
void CUIManager::PopupNotIdleWnd(bool bClickToIdle /* = false */)
{
    //先全部置为未处理
    List_Wnd::iterator itBegin = m_ListWnd.begin();
    for (; itBegin != m_ListWnd.end(); ++itBegin)
    {
        CBaseDialog * pWnd = (*itBegin);
        pWnd->SetProcessed(false);
    }

    while (true)
    {
        itBegin = m_ListWnd.begin();
        // 找到第一个还未处理的窗口
        bool bFind = false;
        for (; itBegin != m_ListWnd.end(); ++itBegin)
        {
            CBaseDialog * pWnd = (*itBegin);
            if (!pWnd->GetProcessed())
            {
                bFind = true;
                break;
            }
        }

        //如果已经找不到未处理的窗口，就可以退出
        if (!bFind)
        {
            break;
        }

        CBaseDialog * pWnd = (*itBegin);
        if (pWnd != NULL
                && !pWnd->IsRejectReturnToIdle()
                && eWT_Idle != pWnd->GetWindowType())
        {
            pWnd->SetProcessed(true);

            //调用ReturnToIdle函数
            pWnd->OnReturnToIdle();

            PutPrivateDlg(pWnd);

            m_ListWnd.erase(itBegin);
        }
        else
        {
            pWnd->SetProcessed(true);
            pWnd->OnReturnToIdleAction();
        }

    }
}

void CUIManager::PreReturnToIdle()
{
    MessageBox_RemoveAllMessageBox(MessageBox_Destory);
    SetVolumeBarVisible(false);
    AdapterBox_ExitScreenSaver();
}

// 返回Idle界面
void CUIManager::ReturnToIdle(bool bClickToIdle /* = false */)
{
    //回到Idle之前的预操作
    PreReturnToIdle();

    // 记录上一次的顶层窗口
    CBaseDialog * pLastTopWnd = GetTopWnd();

    // 将所有非Idle界面的窗口出栈
    PopupNotIdleWnd(bClickToIdle);

    // 更新最新的顶层窗口
    CBaseDialog * pTopWnd = GetTopWnd();
    if (pTopWnd != NULL)
    {
        UIMANAGER_INFO("ReturnToIdle: pLastTopWnd(%p) pTopWnd(%p)", pLastTopWnd, pTopWnd);
        // 当窗口从非焦点pop出来成为焦点时调用各模块提供的回调函数
        bool bTopWndChange = (pLastTopWnd != pTopWnd);
        if (bTopWndChange)
        {
            OnTopWndChange(false, pLastTopWnd, pTopWnd);
            pTopWnd->OnFocusChanged(true);
        }

        UpdateWnd(pTopWnd, xView::SHOW_CMD_DEFAULT, true);
    }
}

// 更新界面（参数为窗口名）
void CUIManager::UpdateWnd(const yl::string & strDlgObjectName
                           , int nShowCmd/* = xView::SHOW_CMD_DEFAULT*/
                           , bool bUIReturn/* = false*/)
{
    // 找到窗口
    CBaseDialog * pWnd = GetPrivateDlg(strDlgObjectName);
    if (pWnd != NULL)
    {
        // 更新窗口
        UpdateWnd(pWnd, nShowCmd, bUIReturn);
    }
}

// 更新界面（参数为窗口模板中对应的小窗口）
void CUIManager::UpdateWnd(CBaseDialog * pDlg
                           , int nShowCmd/* = xView::SHOW_CMD_DEFAULT*/
                           , bool bUIReturn/* = false*/)
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

    UpdateFrameBackGround();

    // 更新softkey的显示状态
    if (s_bSoftkeyEnable)
    {
        pDlg->UpdateSoftkey(pDlg->IsShowSoftkey());
    }
    bool bExp;
    pDlg->UpdateDsskey(pDlg->IsShowDsskey(bExp));

    // 更新界面后先将其他小窗口隐藏
    HideOtherWnd(pDlg);

    pDlg->DialogShow(nShowCmd);
    pDlg->UpdateWnd(bUIReturn);
}


// 获取小窗口
CBaseDialog * CUIManager::CreatePrivateDlg(const yl::string & strDlgClassName,
        const yl::string & strDlgObjectName /* = "" */,
        bool bWaitSetActive/* = false*/)
{
    return m_objDialogCache.CreatePrivateDlg(strDlgClassName, strDlgObjectName, bWaitSetActive);
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

////----后续要重新整理focusChange的逻辑
void CUIManager::OnTopWndChange(bool bBeforeTopWndChange,
                                CBaseDialog * dlgPreTop,
                                CBaseDialog * dlgNewTop)
{
    if (!bBeforeTopWndChange)
    {
        MessageBox_RemoveAllMessageBox(MessageBox_Destory);
        LockReturnToIdle(false);
    }

    ListTopWndChangeProc::iterator it = m_listTopWndChangeProc.begin();
    for (; it != m_listTopWndChangeProc.end(); ++it)
    {
        if ((*it) != NULL)
        {
            (*it)(CTopWndChangeInfo(bBeforeTopWndChange, dlgPreTop, dlgNewTop));
        }
    }
}

void CUIManager::RegisterTopWndChangeCallBack(ON_TOPWND_CHANGE_PROC funCallBack)
{
    ListTopWndChangeProc::iterator it = yl::find(m_listTopWndChangeProc.begin(),
                                        m_listTopWndChangeProc.end(), funCallBack);

    if (it == m_listTopWndChangeProc.end())
    {
        m_listTopWndChangeProc.push_back(funCallBack);
    }
}

void CUIManager::UnregisterTopWndChangeCallBack(ON_TOPWND_CHANGE_PROC funCallBack)
{
    ListTopWndChangeProc::iterator it = yl::find(m_listTopWndChangeProc.begin(),
                                        m_listTopWndChangeProc.end(), funCallBack);

    if (it != m_listTopWndChangeProc.end())
    {
        m_listTopWndChangeProc.erase(it);
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

void CUIManager::SetIdleCheckTimerActive(bool bActive)
{
    if (bActive && !m_bLockRetrunToIdle)
    {
        if (m_IdleCheckTimer.IsTimerRuning())
        {
            m_IdleCheckTimer.KillTimer();
        }

        m_IdleCheckTimer.SingleShot(RETURN_IDLE_TIME, TIMER_METHOD(this, CUIManager::OnIdleCheckTimer));
    }
    else
    {
        m_IdleCheckTimer.KillTimer();
    }

}

void CUIManager::OnIdleCheckTimer(xThreadTimer* timer, LPVOID pExtraData)
{
    CBaseDialog * pDialog = UIManager_GetTopWindow();
    if (NULL != pDialog
            && pDialog->IsRejectReturnToIdle()
            && !m_bLockRetrunToIdle)
    {
        return;
    }

    ReturnToIdle(true);
}

void CUIManager::LockReturnToIdle(bool bLock)
{
    if (bLock != m_bLockRetrunToIdle)
    {
        m_bLockRetrunToIdle = bLock;
        SetIdleCheckTimerActive(!m_bLockRetrunToIdle);
    }
}

void CUIManager::ResetReturnToIdle()
{
    CBaseDialog * pDialog = UIManager_GetTopWindow();
    if (NULL != pDialog)
    {
        // 重新计时
        SetIdleCheckTimerActive(!pDialog->IsRejectReturnToIdle());
    }
}

bool CUIManager::ProcessVolumeBarKey(int nKeyCode, bool bPress)
{
    if (NULL == m_pAutoVolumeBar)
    {
        return false;
    }

    return m_pAutoVolumeBar->ProcessKeyEvent(nKeyCode, bPress);
}

void CUIManager::SetVolumeBarVisible(bool bVisible)
{
    if (NULL == m_pAutoVolumeBar)
    {
        return;
    }

    return m_pAutoVolumeBar->SetVolumeBarVisible(bVisible);
}

void CUIManager::SetFrameBackGround(xBrush brushBackGround)
{
    g_WorkStation.setStationBackground(brushBackGround);
}

void CUIManager::UpdateFrameBackGround()
{
    CBaseDialog * pDlg = GetTopWnd();

    if (NULL == pDlg)
    {
        return;
    }

    g_WorkStation.setStationBackground(pDlg->GetBackGroundBrush());
}

void CUIManager::SetCustomSoftkeyCallback(FunCustomSoftkey pFunCustomSoftkey)
{
    m_pFunCustomSoftkey = pFunCustomSoftkey;
}

void CUIManager::GetCustomSoftkeyData(CArraySoftKeyBarData & objSoftkeyData)
{
    if (NULL == m_pFunCustomSoftkey)
    {
        return;
    }
    m_pFunCustomSoftkey(objSoftkeyData);
}

bool CUIManager::IsEnableCustomSoftkey()
{
    if (NULL == m_pFunCustomSoftkey)
    {
        return false;
    }
    return true;
}
