#include "uimanager_header.h"
#include "uimanager_inc.h"

// 初始化UIManager
void UIManager_Init()
{
    _UIManager.Init();
}

// 设置主窗口指针
void UIManager_SetMainWnd(CBaseMainWnd * pMainWnd)
{
    _UIManager.SetMainWnd(pMainWnd);
}

//获取主窗口指针
CBaseMainWnd * UIManager_GetMainWnd()
{
    return _UIManager.GetMainWnd();
}

// 主窗口显示
void UIManager_ShowMainWnd()
{
    _UIManager.ShowMainWnd();
}

// 根据窗口类名与窗口名称创建小窗口，若窗口名称为空，则将其赋为类名
CBaseDialog * UIManager_CreatePrivateDlg(const yl::string & strDlgClassName
        , const yl::string & strDlgObjectName /* = "" */
        , bool bPush)
{
    CBaseDialog * pDlg = _UIManager.CreatePrivateDlg(strDlgClassName, strDlgObjectName);
    if (bPush)
    {
        _UIManager.PushWnd(pDlg);
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
void UIManager_ReturnToIdle(RETURN_IDLE_TYPE eReturnIdle)
{
    _UIManager.ReturnToIdle(eReturnIdle);
}

void UIManager_ReturnToIdle(bool bClickToIdle)
{
    if (bClickToIdle)
    {
        _UIManager.ReturnToIdle(RETURN_IDLE_TYPE_HOME_PRESS);
    }
    else
    {
        _UIManager.ReturnToIdle(RETURN_IDLE_TYPE_NORMAL);
    }
}

void UIManager_ReturnToIdle()
{
    _UIManager.ReturnToIdle(RETURN_IDLE_TYPE_NORMAL);
}

void UIManager_ExitDirAndCalllogWnd()
{
    _UIManager.ExitDirAndCalllogWnd();
}

// 更新界面，传入的窗口必须是当前顶层窗口
void UIManager_UpdateWnd(const yl::string & strDlgObjectName)
{
    _UIManager.UpdateWnd(strDlgObjectName);
}

// 更新界面，传入的窗口必须是当前顶层窗口
void UIManager_UpdateWnd(CBaseDialog * pDlg)
{
    _UIManager.UpdateWnd(pDlg);
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
    return (eWT_Talk == _UIManager.GetTopWndType()
            || eWT_Talk_Dial == _UIManager.GetTopWndType());
}

bool UIManager_IsInCallManager()
{
    return (eWT_CallManager == _UIManager.GetTopWndType());
}

bool UIManager_ExistCallManager()
{
    return _UIManager.IsWndExist(eWT_CallManager);
}

bool UIManager_IsInXMLBrowser()
{
    return (eWT_XMLBrower == _UIManager.GetTopWndType());
}

void UIManager_ChangeTransparency()
{
    _TransChangeMsgHelper.OnTransparencyChange();
}

void UIManager_InstallTransChangeSignalSlot(const QObject * receiver, const char * method)
{
    if (NULL != receiver)
    {
        QObject::connect(&_TransChangeMsgHelper, SIGNAL(transparencyChange()), receiver, method);
    }
}

void UIManager_UnInstallTransChangeSignalSlot(const QObject * receiver, const char * method)
{
    if (NULL != receiver)
    {
        QObject::disconnect(&_TransChangeMsgHelper, SIGNAL(transparencyChange()), receiver, method);
    }
}
