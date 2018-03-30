#include "uimanager_header.h"
#include "uimanager_inc.h"

// 初始化UIManager
void UIManager_Init()
{
    _UIManager.Init();
}

// 根据窗口类名与窗口名称创建小窗口，若窗口名称为空，则将其赋为类名
CBaseDialog * UIManager_CreatePrivateDlg(const yl::string & strDlgClassName,
        const yl::string & strDlgObjectName /* = "" */,
        int nShowCmd/* = xView::SHOW_CMD_DEFAULT*/, bool bPush/* = true*/)
{
    CBaseDialog * pDlg = _UIManager.CreatePrivateDlg(strDlgClassName, strDlgObjectName, true);
    if (bPush)
    {
        _UIManager.PushWnd(pDlg, nShowCmd);
    }
    return pDlg;
}

// 根据窗口名获取小窗口
CBaseDialog * UIManager_GetPrivateDlg(const yl::string & strDlgObjectName)
{
    return _UIManager.GetPrivateDlg(strDlgObjectName);
}

void UIManager_PushWnd(CBaseDialog * pDlg)
{
    _UIManager.PushWnd(pDlg);
}

// 将传入的小窗口还回到窗口缓存
void UIManager_PutPrivateDlg(CBaseDialog * pDlg)
{
    _UIManager.PopWnd(pDlg);
}

// 按传入的小窗口名称将小窗口还回到窗口缓存
void UIManager_PutPrivateDlg(const yl::string & strDlgObjectName)
{
    CBaseDialog * pDlg = UIManager_GetPrivateDlg(strDlgObjectName);
    UIManager_PutPrivateDlg(pDlg);
}

// 将某个窗口置顶
CBaseDialog * UIManager_GetTopWindow()
{
    return _UIManager.GetTopWnd();
}

// 将某个窗口置顶
void UIManager_SetTopWindow(CBaseDialog * pWnd)
{
    if (_UIManager.SetTopWnd(pWnd))
    {
        _UIManager.UpdateWnd(pWnd);
    }
}

// 返回Idle界面，清除其他窗口
void UIManager_ReturnToIdle(bool bClickToIdle)
{
    _UIManager.ReturnToIdle(bClickToIdle);
}

void UIManager_ReturnToIdle()
{
    _UIManager.ReturnToIdle(false);
}

// 更新界面，传入的窗口必须是当前顶层窗口
void UIManager_UpdateWnd(const yl::string & strDlgObjectName,
                         int nShowCmd/* = xView::SHOW_CMD_DEFAULT*/)
{
    _UIManager.UpdateWnd(strDlgObjectName, nShowCmd);
}

// 更新界面，传入的窗口必须是当前顶层窗口
void UIManager_UpdateWnd(CBaseDialog * pDlg, int nShowCmd/* = xView::SHOW_CMD_DEFAULT*/)
{
    _UIManager.UpdateWnd(pDlg, nShowCmd);
}

void UIManager_UpdateFrameBackGround()
{
    _UIManager.UpdateFrameBackGround();
}

// 更新顶层窗口
void UIManager_UpdateTopWnd()
{
    CBaseDialog * pDlg = _UIManager.GetTopWnd();
    _UIManager.UpdateWnd(pDlg);
}

void UIManager_RegisterTopWndChangeCallBack(ON_TOPWND_CHANGE_PROC funCallBack)
{
    _UIManager.RegisterTopWndChangeCallBack(funCallBack);
}

void UIManager_UnregisterTopWndChangeCallBack(ON_TOPWND_CHANGE_PROC funCallBack)
{
    _UIManager.UnregisterTopWndChangeCallBack(funCallBack);
}

bool UIManager_IsWndExist(EWindowType eType)
{
    return _UIManager.IsWndExist(eType);
}

// 是否Idle界面(for VoiceChannel)
bool UIManager_IsInIdle()
{
    return (eWT_Idle == _UIManager.GetTopWndType());
}

// 是否Talk界面(for VoiceChannel)
bool UIManager_IsInTalk()
{
    return (eWT_Talk == _UIManager.GetTopWndType());
}

void UIManager_LockReturnToIdle(bool bLock)
{
    _UIManager.LockReturnToIdle(bLock);
}

void UIManager_ResetReturnToIdle()
{
    _UIManager.ResetReturnToIdle();
}

void UIManager_SetFrameBackGround(xBrush brush)
{
    _UIManager.SetFrameBackGround(brush);
}

void UIManager_SetFunGetCustomSoftkey(FunCustomSoftkey pFunCustomSoftkey)
{
    _UIManager.SetCustomSoftkeyCallback(pFunCustomSoftkey);
}

void UIManager_GetCustomSoftkeyData(CArraySoftKeyBarData & objSoftkeyData)
{
    _UIManager.GetCustomSoftkeyData(objSoftkeyData);
}

bool UIManager_IsEnableCustomSoftkey()
{
    return _UIManager.IsEnableCustomSoftkey();
}
