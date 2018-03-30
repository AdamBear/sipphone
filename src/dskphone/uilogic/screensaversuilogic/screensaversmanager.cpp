#include "screensaversmanager.h"
#include "configparser/modconfigparser.h"
#include "configiddefine/configiddefine.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "keyguard/include/modkeyguard.h"
#include "screensaverpainter.h"
#include "idlescreen/include/modidlescreen.h"
#include "screensaversuilogic/screensavershelpbase.h"
#include "exp/include/exp_struct.h"
#include "blf/include/blflistmsgdefine.h"

#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
#include "record/include/recordmessagedefine.h"
#endif

// push xml消息
#define PHONE_MSG_PUSH_XML 0x60d08
#if IF_FEATURE_XMLB_SCREENSAVER || IS_COLOR

LRESULT CScreenSaverManager::OnScreenSaverMessage(msgObject& refMessage)
{
    IDLEUI_INFO("CScreenSaverManager MsgID(%d), wPram(%d), lParm(%d)",  refMessage.message, refMessage.wParam, refMessage.lParam);

    g_ScreenSaverManger.OnMessage(refMessage);

    return false;
}

CScreenSaverManager::CScreenSaverManager()
    : m_pDlgScreenSavers(NULL)
    , m_bShowScreenSavers(false)
    , m_bAllowScreenSaverCount(false)
{

}

CScreenSaverManager::~CScreenSaverManager()
{

}

CScreenSaverManager& CScreenSaverManager::GetInstance()
{
    static CScreenSaverManager s_Instance;

    return s_Instance;
}


void CScreenSaverManager::EnterScreenSaverOnTopWndChange(bool bNewAllow)
{
    m_bAllowScreenSaverCount = bNewAllow;

    if (!m_bAllowScreenSaverCount)
    {
        StopScreenSaverTimer();
    }
    else
    {
        if (!IsTimerActive())
        {
            ResetScreenSaverTimer();
        }
    }
}

// 文本状态改变通知响应函数
void CScreenSaverManager::StatusIconNotifyFunction(YLList<IdleStatusItem>& ylVecStatusList)
{
    g_ScreenSaverManger.OnIconStatusChange(ylVecStatusList);
}

void CScreenSaverManager::OnIconStatusChange(YLList<IdleStatusItem>& ylVecStatusList)
{
    if (m_bShowScreenSavers)
    {
        if (NULL != m_pDlgScreenSavers)
        {
            m_pDlgScreenSavers->OnStatusChange(ylVecStatusList);
        }
    }
}

void CScreenSaverManager::OnMessage(msgObject& refMessage)
{
    if (ACCOUNT_STATUS_CHANGED == refMessage.message)
    {
        LINE_STATE eState = acc_GetState(refMessage.wParam);
        if (eState != refMessage.lParam)
        {
            //账号登出
            if (LS_DISABLE == eState
                    || LS_BE_UNREGISTERED == eState)
                //|| LS_REG_CANCEL_COMPLETED == eState)
            {
                OnExitScreenSavers();
            }
        }
    }
    else if (CONFIG_MSG_BCAST_CHANGED == refMessage.message)
    {
        if (ST_SCREEN_SAVER_TIME == refMessage.wParam)
        {
            ResetScreenSaverTimer();
        }
    }
    else if (DEV_MSG_EXP_KEYDOWN == refMessage.message)
    {
        OnExitScreenSavers();
    }
    else if (CONFIG_MSG_BCAST_CHANGED == refMessage.message)
    {
        if (ST_SCREEN_SAVER_TIME == refMessage.wParam)
        {
            if (configParser_GetConfigInt(kszScreenSaverWaitTime) <= 0)
            {
                OnExitScreenSavers();
            }
        }
    }
#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
    else if (RECORD_STORAGE_STATE_CHANGE == refMessage.message)
    {
        if (0 == refMessage.wParam || 2 == refMessage.wParam || 3 == refMessage.wParam)
        {
            OnExitScreenSavers();
        }
    }
#endif
    else if (BLF_MESSAGE_FLUSH == refMessage.message)
    {
        if (configParser_GetConfigInt(kszBLFActiveBackLight) == 1)
        {
            if (m_pDlgScreenSavers != NULL)
            {
                m_pDlgScreenSavers->SetIsEXPExitSync(false);
            }
            OnExitScreenSavers();
        }
    }
    else if (PHONE_MSG_PUSH_XML == refMessage.message)
    {
        OnExitScreenSavers();
    }
}

// 处理按键事件
bool CScreenSaverManager::ProcessKeyEvent(int nKeyCode, bool bPress)
{
    if (m_bShowScreenSavers)
    {
        //当按键为数字键和*#时需要进入拨号界面
        if ((PHONE_KEY_0 <= nKeyCode && nKeyCode <= PHONE_KEY_9)
                || PHONE_KEY_STAR == nKeyCode
                || PHONE_KEY_POUND == nKeyCode
           )
        {
            //未上锁才直接进入拨号
            if (!keyGuard_IsLock())
            {
                talklogic_EnterPredialUI(nKeyCode);
            }

            OnExitScreenSavers();
            return true;
        }
        //免提跟手柄事件不要拦截，需要直接响应
        else if (PHONE_KEY_HANDFREE == nKeyCode ||
                 PHONE_KEY_HANDSET_OFF_HOOK == nKeyCode)
        {
            OnExitScreenSavers();
            return false;
        }
        //其余按键拦截
        else
        {
            OnExitScreenSavers();
            return true;
        }
    }
    else
    {
        ResetScreenSaverTimer();
    }

    return false;
}

// 处理鼠标事件
bool CScreenSaverManager::ProcessMouseEvent()
{
    //有屏保显示，触摸要先退出屏保
    if (m_bShowScreenSavers)
    {
        OnExitScreenSavers();

        return true;
    }
    else
    {
        ResetScreenSaverTimer();
    }

    return false;
}

void CScreenSaverManager::ScreenSaverHelpe_Init()
{
    m_bAllowScreenSaverCount = false;

    idleScreen_SetNotifyFunction(&StatusIconNotifyFunction, SNT_ICON);

    etl_RegisterMsgHandle(ACCOUNT_STATUS_CHANGED, ACCOUNT_STATUS_CHANGED, &CScreenSaverManager::OnScreenSaverMessage);
    etl_RegisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED, &CScreenSaverManager::OnScreenSaverMessage);
    etl_RegisterMsgHandle(DEV_MSG_EXP_KEYDOWN, DEV_MSG_EXP_KEYDOWN, &CScreenSaverManager::OnScreenSaverMessage);
#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
    etl_RegisterMsgHandle(RECORD_STORAGE_STATE_CHANGE, RECORD_STORAGE_STATE_CHANGE, &CScreenSaverManager::OnScreenSaverMessage);
#endif
}

void CScreenSaverManager::ScreenSaverHelpe_UnInit()
{
    etl_UnregisterMsgHandle(ACCOUNT_STATUS_CHANGED, ACCOUNT_STATUS_CHANGED, &CScreenSaverManager::OnScreenSaverMessage);
    etl_UnregisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED, &CScreenSaverManager::OnScreenSaverMessage);
    etl_UnregisterMsgHandle(DEV_MSG_EXP_KEYDOWN, DEV_MSG_EXP_KEYDOWN, &CScreenSaverManager::OnScreenSaverMessage);
#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
    etl_UnregisterMsgHandle(RECORD_STORAGE_STATE_CHANGE, RECORD_STORAGE_STATE_CHANGE, &CScreenSaverManager::OnScreenSaverMessage);
#endif
}

// 关闭屏保计时器
void CScreenSaverManager::StopScreenSaverTimer()
{
    if (m_timerScreenSaver.IsTimerRuning())
    {
        m_timerScreenSaver.KillTimer();
    }
}

// 重置计时器
void CScreenSaverManager::ResetScreenSaverTimer()
{
    if (m_bAllowScreenSaverCount)
    {
        StartScreenSaverTimer();
    }
}

// 启动屏保计时器
void CScreenSaverManager::StartScreenSaverTimer()
{
    StopScreenSaverTimer();

    int iLockTimeOut = configParser_GetConfigInt(kszScreenSaverWaitTime) * 1000;
    if (iLockTimeOut > 0)
    {
        IDLEUI_INFO("star screensaver time");
        m_timerScreenSaver.SetTimer(iLockTimeOut, (void*)&m_timerScreenSaver, this, static_cast<mkMsgEntry>(&CScreenSaverManager::OnScreenSaverTimerTimeout));
    }
}

bool CScreenSaverManager::IsTimerActive()
{
    return m_timerScreenSaver.IsTimerRuning();
}

BOOL CScreenSaverManager::OnScreenSaverTimerTimeout(msgObject& msg)
{
    //通话中不进入屏保
    if (talklogic_SessionExist())
    {
        return TRUE;
    }

    //再次询问顶层是否可以进入屏保
    m_bAllowScreenSaverCount = m_pScreenSaverHelperBase->GetTopWndAllowScreenCount();

    if (!m_bAllowScreenSaverCount)
    {
        return TRUE;
    }

    StopScreenSaverTimer();

    OnShowScreenSavers();

    return TRUE;
}

void CScreenSaverManager::ResetTopWndSaverTimer()
{
    m_bAllowScreenSaverCount = m_pScreenSaverHelperBase->GetTopWndAllowScreenCount();
    if (m_bAllowScreenSaverCount)
    {
        ResetScreenSaverTimer();
    }
}

//显示屏保界面
void CScreenSaverManager::OnShowScreenSavers()
{
    IDLEUI_INFO("CScreenSaverManager OnShowScreenSavers[%d]", m_bShowScreenSavers);
    if (!m_bShowScreenSavers)
    {
        m_bShowScreenSavers = true;

        m_pDlgScreenSavers = m_pScreenSaverHelperBase->CreateDlgScreenSavers();

        if (NULL != m_pDlgScreenSavers)
        {
            m_pDlgScreenSavers->OnInit();
            //m_pDlgScreenSavers->OnShow();
            m_pScreenSaverHelperBase->SetScreenSaversVisible(true);
        }
    }
}


//退出屏保界面
void CScreenSaverManager::OnExitScreenSavers()
{
    IDLEUI_INFO("CScreenSaverManager OnExitScreenSavers[%d]", m_bShowScreenSavers);
    if (m_bShowScreenSavers)
    {
        m_bShowScreenSavers = false;
        if (NULL != m_pDlgScreenSavers)
        {
            m_pDlgScreenSavers->OnUnInit();
            //m_pDlgScreenSavers->OnHide();
            m_pDlgScreenSavers = NULL;

            m_pScreenSaverHelperBase->SetScreenSaversVisible(false);
        }

        //根据顶层窗口属性重新设置定时器
        ResetTopWndSaverTimer();
    }
}

//是否显示屏保界面
bool CScreenSaverManager::IsShowScreenSavers()
{
    return m_bShowScreenSavers;
}

void CScreenSaverManager::setScreenSaverHelpeBase(CScreenSaverHelperBase* pScreenSaverHelperBase)
{
    if (NULL != pScreenSaverHelperBase)
    {
        m_pScreenSaverHelperBase = pScreenSaverHelperBase;
    }

}
#endif
