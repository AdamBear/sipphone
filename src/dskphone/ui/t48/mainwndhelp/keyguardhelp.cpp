#include "keyguardhelp.h"
#include "uimanager/moduimanager.h"
#include "globalinputhook/modinputhook.h"
#include "qtcommon/qmisc.h"
#include "keyguard/include/modkeyguard.h"// mod-KeyGuard
#include "dsklog/log.h"

#include "uicommon/messagebox/modmessagebox.h"

#include "uimanager/moduimanager.h"
#include "setting/include/modsetting.h"

#include "uikernel/languagehelper.h"
#include "mainwnd/mainwnd.h"
#include "settingui/include/modsettingui.h"

#include "etlmsghandler/modetlmsghandler.h"
#include "keyguard/include/keyguardmessagedefine.h"

#include "configparser/modconfigparser.h"
#include <configiddefine.h>


#include "keymap.h"
#include "inputmethod/inputmethod.h"

namespace
{
#define NOT_EMERGENCY_NUMBER_NOTE       46
#define UNLOCK_NOTE_TIME                2000

static const char kszUnLockPin[] = "unlock_pin";
}

bool KeyGuardHelp_ProcessKeyEvent(int nKeyCode, bool bPress);
bool KeyGuardHelp_ProcessMouseEvent(QObject * pObject, QMouseEvent * pEvent);

static bool s_bIsCurrenDlgAllowKeyguardCount;

static void HelpNotEmergencyNumber(const yl::string & strNote)
{
    g_pKeyGuardHelper->RemoveUnlockBox();

    //弹出解锁框
    MessageBox_ShowCommonMsgBox(NULL, LANG_TRANSLATE(TRID_NOT_A_EMERGENCY_NUMBER), MESSAGEBOX_NOTE,
                                2000, "", "", NOT_EMERGENCY_NUMBER_NOTE);
}

static bool CheckUnlockPsw()
{
    //弹出前先清除可能存在的解锁框
    g_pKeyGuardHelper->RemoveUnlockBox();

    //键盘锁解锁流程
    //为了保留如未接来电的弹出框，所以不清除所有弹出框
    //      MessageBox_RemoveAllMessageBox();

    //弹出解锁框
    MessageBox_ShowUnlockBox(g_pKeyGuardHelper);

    return false;
}

CKeyGuardHelper * CKeyGuardHelper::GetInstance()
{
    static CKeyGuardHelper * g_pInstance = NULL;
    if (NULL == g_pInstance)
    {
        g_pInstance = new CKeyGuardHelper();
    }

    return g_pInstance;
}

CKeyGuardHelper::CKeyGuardHelper()
{
}

CKeyGuardHelper::~CKeyGuardHelper()
{
    m_timerKeyLock.stop();

    etl_UnregisterMsgHandle(KEYGUARD_ENABLE_CHANGED, KEYGUARD_ENABLE_CHANGED,
                            &CKeyGuardHelper::OnMessage);
    etl_UnregisterMsgHandle(KEYGUARD_LOCK_CHANGED, KEYGUARD_LOCK_CHANGED, &CKeyGuardHelper::OnMessage);

    UIManager_UnregisterTopWndChangeCallBack(CKeyGuardHelper::OnTopWndChangeCallbackFun);
}

void CKeyGuardHelper::KeyGuardHelp_Init()
{
    s_bIsCurrenDlgAllowKeyguardCount = false;
    //  keyGuard_SaveUnlockPin("123");

    // 注册键盘钩子
    inputHook_RegisterKeyPadHookProc(KeyGuardHelp_ProcessKeyEvent, HOOK_MODULE_KEYGUARD);

    // 注册鼠标钩子
    inputHook_RegisterMouseHookProc(KeyGuardHelp_ProcessMouseEvent, HOOK_MODULE_KEYGUARD);

    // 注册键盘锁弹出框回调
    keyGuard_SetCheckUnlockPswFun(CheckUnlockPsw);

    //注册检查紧急拨号回调
    keyGuard_SetEmergencyTipFun(HelpNotEmergencyNumber);

    etl_RegisterMsgHandle(KEYGUARD_ENABLE_CHANGED, KEYGUARD_ENABLE_CHANGED,
                          &CKeyGuardHelper::OnMessage);
    etl_RegisterMsgHandle(KEYGUARD_LOCK_CHANGED, KEYGUARD_LOCK_CHANGED, &CKeyGuardHelper::OnMessage);

    UIManager_RegisterTopWndChangeCallBack(CKeyGuardHelper::OnTopWndChangeCallbackFun);

    connect(&m_timerKeyLock, SIGNAL(timeout()), this, SLOT(OnKeyLockTimerTimeout()));
}

// 锁定键盘的计时器时间到
void CKeyGuardHelper::OnKeyLockTimerTimeout()
{
    StopKeyLockTimer();

    if (keyGuard_IsEnable() && !keyGuard_IsLock())
    {
        keyGuard_Lock(true);
    }
}

// 启动键盘锁的计时器
void CKeyGuardHelper::StartKeyLockTimer()
{
    if (!keyGuard_IsEnable())
    {
        return;
    }

    StopKeyLockTimer();

    int iLockTimeOut = configParser_GetConfigInt(kszLockTimeout);
    if (iLockTimeOut > 0)
    {
        m_timerKeyLock.start(1000 * iLockTimeOut);
    }
}

// 关闭键盘锁的计时器
void CKeyGuardHelper::StopKeyLockTimer()
{
    if (m_timerKeyLock.isActive())
    {
        m_timerKeyLock.stop();
    }
}

void CKeyGuardHelper::ResetKeypadLock()
{
    if (s_bIsCurrenDlgAllowKeyguardCount)
    {
        StartKeyLockTimer();
    }
}

bool CKeyGuardHelper::IsTimerActive()
{
    return m_timerKeyLock.isActive();
}

void CKeyGuardHelper::MessageBoxCallBack(CMessageBoxBase * pMessageBox)
{
    if (NULL == pMessageBox)
    {
        return;
    }

    if (MESSAGEBOX_UNLOCK == pMessageBox->GetType())
    {
        switch (pMessageBox->GetResult())
        {
        case MessageBox_OK:
        {
            RemoveUnlockBox();
            keyGuard_Lock(false);
        }
        break;
        case MessageBox_Admin:
        {
            RemoveUnlockBox();
            //V73的处理是同时也解锁
            keyGuard_Lock(false);

            bool bAdminInput = true;

            SubMenuData objSubMenuData;
            objSubMenuData.strMenuAction = "unlock_pin";
            objSubMenuData.pBeforeSetSubpageData = &bAdminInput;
            SettingUI_OpenPage(objSubMenuData);

        }
        break;
        default:
            break;
        }
    }
}

void CKeyGuardHelper::OnTopWndChangeCallbackFun(CBaseDialog * dlgPreTop, CBaseDialog * dlgNewTop)
{
    if (NULL != dlgNewTop)
    {
        s_bIsCurrenDlgAllowKeyguardCount = dlgNewTop->IsAllowKeyguardCount();

        // 如果新的顶层界面不允许键盘锁开启，则需要关闭计时器
        if (!s_bIsCurrenDlgAllowKeyguardCount)
        {
            g_pKeyGuardHelper->StopKeyLockTimer();
        }
        //如果允许，则分为目前计时器正在计时和还未开始计时两种情况
        else
        {
            //如果未开始计时说明是从一个不允许界面跳转到允许界面，或者从通知中心返回，要重开计时器
            if (!g_pKeyGuardHelper->IsTimerActive())
            {
                g_pKeyGuardHelper->ResetKeypadLock();
            }
            //else
            //否则是从一个允许的界面跳转到另一个允许的界面，不需要做任何操作
        }
    }
}

LRESULT CKeyGuardHelper::OnMessage(msgObject & refMessage)
{
    switch (refMessage.message)
    {
    case KEYGUARD_ENABLE_CHANGED:
    {
        if (refMessage.wParam == false)
        {
            CKeyGuardHelper::RemoveUnlockBox();
        }
    }
    break;
    case KEYGUARD_LOCK_CHANGED:
    {
        //上锁提示现在不在idle做
        if (1 == refMessage.wParam)
        {
            // 上锁给予提示
            MessageBox_ShowCommonMsgBox(NULL, LANG_TRANSLATE(TRID_KEYPAD_LOCKED), MESSAGEBOX_NOTE,
                                        UNLOCK_NOTE_TIME);
        }
    }
    break;
    default:
        break;
    }

    return FALSE;
}

void CKeyGuardHelper::RemoveUnlockBox()
{
    MessageBox_RemoveMessageBox(g_pKeyGuardHelper);
}

bool KeyGuardHelp_ProcessKeyEvent(int nKeyCode, bool bPress)
{
    //如果处于开启但未上锁的状态，则遇到键盘事件就需要重置计时器
    if (keyGuard_IsEnable() && !keyGuard_IsLock())
    {
        g_pKeyGuardHelper->ResetKeypadLock();
    }

    // 不处于锁定状态则不过滤事件
    if (!keyGuard_IsLock() || !bPress)
    {
        return false;
    }

    bool bMessageboxHandleResult = false;

    //如果有弹出框则先考虑弹出框
    CMessageBoxBase * pMessageBox = MessageBox_GetTopMessageBox();
    if (NULL != pMessageBox)
    {
        switch (pMessageBox->GetType())
        {
        // 有密码弹出框的时候不拦截弹出框的事件
        case MESSAGEBOX_UNLOCK:
        case MESSAGEBOX_LINE_SELECT:
        {
            bMessageboxHandleResult = false;
        }
        break;
        // 如果是非紧急号码提示框则给提示框处理
        case MESSAGEBOX_NOTE:
        {
            if (NOT_EMERGENCY_NUMBER_NOTE == pMessageBox->GetID())
            {
                bMessageboxHandleResult = false;
            }
        }
        break;
        case MESSAGEBOX_OkCancel:
        case MESSAGEBOX_OkBlock:
        case MESSAGEBOX_SPECIAL:
#if IF_BUG_30016
        case MESSAGEBOX_GPICKUP:
#endif
        {
            //menukey下不锁
            if (KG_MENU_KEY == keyGuard_GetKeyGuardType())
            {
                break;
            }
            else
            {
                //不允许okcancel
                if (PHONE_KEY_OK == nKeyCode
                        || PHONE_KEY_CANCEL == nKeyCode)
                {
                    // 弹出解锁框
                    CheckUnlockPsw();

                    // 不管是否解锁成功都拦截该按键
                    bMessageboxHandleResult = true;
                }

                //其他所有按键都给界面处理
                break;
            }
        }
        break;
        default:
        {
            //其他所有类型弹出框给界面处理
        }
        break;
        }
    }

    //http://10.2.1.199/Bug.php?BugID=125940
    //如果弹出框已经处理，则直接返回true
    //如果弹出框不需要处理，则界面需要处理一次
    if (bMessageboxHandleResult)
    {
        return true;
    }

    //如果没有弹出框则给界面判断
    CBaseDialog * pDlgBase = UIManager_GetTopWindow();
    if (NULL == pDlgBase)
    {
        return false;
    }

    bool bAllow = pDlgBase->AllowKeyPadEventOnKeyLock(nKeyCode, bPress);
    if (bAllow)
    {
        return false;
    }
    else
    {
        // 弹出解锁框
        CheckUnlockPsw();

        // 不管是否解锁成功都拦截该按键
        return true;
    }
}

bool KeyGuardHelp_ProcessMouseEvent(QObject * pObject, QMouseEvent * pEvent)
{
#ifndef _T48
    return false;
#else

    // 影响速度，正式版本去除
    //MAINWND_INFO("CMainWnd::ProcessMouseHook");

    //如果处于开启但未上锁的状态，则遇到鼠标事件就需要重置计时器
    if (keyGuard_IsEnable() && !keyGuard_IsLock())
    {
        g_pKeyGuardHelper->ResetKeypadLock();
    }

    // 不处于锁定状态则过滤事件
    if (!keyGuard_IsLock()
            || NULL == pObject
            || NULL == pEvent)
    {
        return false;
    }

    // 不处理move事件
    if (QEvent::MouseMove == pEvent->type())
    {
        return false;
    }


    // 有密码弹出框的时候不拦截弹出框和遮罩的鼠标事件
    CMessageBoxBase * pMessageBox = MessageBox_GetTopMessageBox();
    if (NULL != pMessageBox)
    {
        switch (pMessageBox->GetType())
        {
        // 有密码弹出框的时候不拦截弹出框的事件
        case MESSAGEBOX_UNLOCK:
        case MESSAGEBOX_LINE_SELECT:
        {
            if (isChildOf(pObject, pMessageBox) || isChildOf(pObject, pMessageBox->parent()))
            {
                return false;
            }
            else
            {
                //在顶层有弹出框的情况下，不是点击在弹出框和遮罩上说明点在了账号选择框顶端的按钮上（目前）不需要任何操作
                // 全键盘要返回false
                return !FullKeyboardEnabled();
            }
        }
        break;
        //如果是非紧急号码提示框则给提示框处理
        case MESSAGEBOX_NOTE:
        {
            if (NOT_EMERGENCY_NUMBER_NOTE == pMessageBox->GetID())
            {
                return false;
            }
        }
        break;
        //在上锁状态，okcancel类型的提示框不允许操作
        case MESSAGEBOX_OkCancel:
        case MESSAGEBOX_OkBlock:
        case MESSAGEBOX_SPECIAL:
        {
            if (QEvent::MouseButtonRelease == pEvent->type())
            {
                // 弹出解锁框
                CheckUnlockPsw();
            }
            // 拦截所有鼠标事件
            return true;
        }
        break;
        default:
        {
            //其他弹出框给界面处理
        }
        break;
        }
    }

    CBaseDialog * pDlgBase = UIManager_GetTopWindow();
    if (NULL == pDlgBase)
    {
        return false;
    }

    bool bAllow = pDlgBase->AllowMouseEventOnKeyLock(pObject, pEvent);

    if (bAllow)
    {
        return false;
    }
    else
    {
        if (QEvent::MouseButtonPress == pEvent->type())
        {
            // 弹出解锁框
            CheckUnlockPsw();
        }

        // 不管是否解锁成功都拦截该按键
        return true;
    }
#endif
}
