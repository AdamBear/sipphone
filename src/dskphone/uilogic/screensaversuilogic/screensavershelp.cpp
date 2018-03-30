#include "screensavershelp.h"
#include "uimanager/basedialog.h"
#include "globalinputhook/modinputhook.h"
#include "messagebox/modmessagebox.h"
#include "screensaversuilogic/modscreensaversuilogic.h"
#include "talkuilogic/modtalkuilogic.h"


bool ScreenSaverHelper_ProcessKeyEvent(int nKeyCode, bool bPress);


#if IF_FEATURE_XMLB_SCREENSAVER || IS_COLOR

CScreenSaverHelper::CScreenSaverHelper()
    : m_pScreenSaversUI(NULL)
{

}

CScreenSaverHelper::~CScreenSaverHelper()
{

}

CScreenSaverHelper* CScreenSaverHelper::GetInstance()
{
    static CScreenSaverHelper s_ScreenSaverHelpreInstance;

    return &s_ScreenSaverHelpreInstance;
}

void CScreenSaverHelper::ScreenSaverHelper_OnTopWndChangeCallbackFun(CTopWndChangeInfo & WndChangInfo)
{
    if (NULL != WndChangInfo.m_dlgNewTop)
    {
        bool bAllowScreenSaver = WndChangInfo.m_dlgNewTop->IsAllowScreenSaverCount();

        UILogic_SetEnterScreenSaver(bAllowScreenSaver);
    }
}
//弹框回调
void CScreenSaverHelper::ScreenSaverHelper_MessageCountChangeCallbackFun(/*bool bIsAddMessageBox, int iLastMsgBoxCount, int iCurrentMsgBoxCount*/)
{
    UIlogic_ExitScreensaver();
}

//通话话路变化回调
void CScreenSaverHelper::ScreenSaverHelper_TalkUICountChangeCallbackFun(/*bool bFirstIntalk, bool bAllTalkExit*/)
{
    UILogic_ScreenSaversWakeUpByTalkCountChange(/*bFirstIntalk, bAllTalkExit*/);
}


//获取顶层窗口属性是否进去屏保
bool CScreenSaverHelper::GetTopWndAllowScreenCount()
{
    CBaseDialog *pDialog = UIManager_GetTopWindow();
    if (NULL != pDialog)
    {
        bool bAllowScreenSaverCount = pDialog->IsAllowScreenSaverCount();
        return bAllowScreenSaverCount;
    }

    return false;
}

//创建屏保界面
CBaseScreenSaversUI* CScreenSaverHelper::CreateDlgScreenSavers()
{

    return m_pScreenSaversUI;
}

//显示或者隐藏屏保层
void CScreenSaverHelper::SetScreenSaversVisible(bool bVisible)
{

    if (NULL != m_pScreenSaversUI)
    {
        m_pScreenSaversUI->SetScreenSaversVisible(bVisible);
    }

}

//屏保消息钩子
void CScreenSaverHelper::ScreenSaverHelper_Init()
{
    // 注册键盘钩子
    inputHook_RegisterKeyPadHookProc(ScreenSaverHelper_ProcessKeyEvent, HOOK_MODULE_SCREENSAVER);
    //注册窗口堆栈变化回调（问题所在）
    UIManager_RegisterTopWndChangeCallBack(CScreenSaverHelper::ScreenSaverHelper_OnTopWndChangeCallbackFun);


    // 注册鼠标钩子
    //inputHook_RegisterMouseHookProc(ScreenSaverHelper_ProcessMouseEvent, HOOK_MODULE_SCREENSAVER);

    //注册弹框回调
#warning MessageBox_RegisterMessageBoxCountChangeCallBack
    //MessageBox_RegisterOnMessageboxCallBack(CScreenSaverHelper::ScreenSaverHelper_MessageCountChangeCallbackFun);
    //注册通话状态改变回调
#warning taluilogic_RegisterTalkUICountChangeCallBack
    //talkuilogic_RegisterOnTalkCallBack(CScreenSaverHelper::ScreenSaverHelper_TalkUICountChangeCallbackFun);
}

void CScreenSaverHelper::ScreenSaverHelper_UnInit()
{
    inputHook_UnRegisterKeyPadHookProc(HOOK_MODULE_SCREENSAVER);
    UIManager_UnregisterTopWndChangeCallBack(CScreenSaverHelper::ScreenSaverHelper_OnTopWndChangeCallbackFun);
    //inputHook_UnRegisterMouseHookProc(ScreenSaverHelper_ProcessMouseEvent);
    //MessageBox_UnregisterOnMessageboxCallBack(CScreenSaverHelper::ScreenSaverHelper_MessageCountChangeCallbackFun);
    //talkuilogic_UnregisterOnTalkCallBack(CScreenSaverHelper::ScreenSaverHelper_TalkUICountChangeCallbackFun);
}


bool ScreenSaverHelper_ProcessKeyEvent(int nKeyCode, bool bPress)
{
    if (UIlogic_ScreenSaversProcessKeyEvent(nKeyCode, bPress))
    {
        return true;
    }

    return false;
}
#endif




