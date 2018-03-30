#include "keyguardmanager.h"
#include "configiddefine.h"
#include "keyguard/include//keyguardmessagedefine.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "configparser/modconfigparser.h"
#include "interfacedefine.h"
#include "commonapi/stringhelperapi.h"
#include "commonapi/systemhelperapi.h"

// 初始化静态指针
CKeyGuardManager * CKeyGuardManager::m_pOneSelf = NULL;

static LRESULT OnKeyGuardMessage(msgObject & refMsg)
{
    switch (refMsg.message)
    {
    case CONFIG_MSG_BCAST_CHANGED:
        {
            if (refMsg.wParam == ST_LOCK)  // 键盘锁配置.
            {
                g_pKeyGuardManager->ReloadConfig();
                return TRUE;
            }
        }
        break;
    case TM_TIMER:
        {
            return g_pKeyGuardManager->OnTimer(refMsg.wParam);
        }
    default:
        break;
    }

    return FALSE;
}

CKeyGuardManager::CKeyGuardManager()
    : m_bLock(false)
    , m_pCheckUnlockPswFun(NULL)
    , m_pEmergencyTip(NULL)
    , m_eKeyGuardType(KG_ALL_NONE)
    , m_bEnable(false)
    , m_iAutoLockTime(-1)
{
    // load config
    ReloadConfig();

    // 绕过Key_1的模拟按键，延时锁按键
    LockKeyPad(IsEnable());// 启动话机的时候如果键盘锁开启默认上锁

    etl_RegisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED, OnKeyGuardMessage);
    etl_RegisterMsgHandle(TM_TIMER, TM_TIMER, OnKeyGuardMessage);
}

CKeyGuardManager::~CKeyGuardManager()
{
    etl_UnregisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED, OnKeyGuardMessage);
    etl_UnregisterMsgHandle(TM_TIMER, TM_TIMER, OnKeyGuardMessage);
}

// Get the singleton instance.
CKeyGuardManager * CKeyGuardManager::GetInstance()
{
    if (NULL == m_pOneSelf)
    {
        m_pOneSelf = new CKeyGuardManager();
    }

    return m_pOneSelf;
}

// Release the singleton instance.
void CKeyGuardManager::ReleaseInstance()
{
    if (NULL != m_pOneSelf)
    {
        delete m_pOneSelf;
        m_pOneSelf = NULL;
    }
}

// 重新读取话机配置
void CKeyGuardManager::ReloadConfig()
{
    m_eKeyGuardType = (KEY_GUARD_TYPE)configParser_GetConfigInt(kszKeyGuardType);
    EnableKeyGuard(configParser_GetConfigInt(kszKeyGuardEnable));
    SetLockTimerValue(configParser_GetConfigInt(kszLockTimeout) * 1000);
}

// 开启/关闭键盘锁
void CKeyGuardManager::EnableKeyGuard(bool bEnable)
{
    if (m_bEnable != bEnable)
    {
        m_bEnable = bEnable;

        // save config
        configParser_SetConfigInt(kszKeyGuardEnable, m_bEnable, CONFIG_LEVEL_PHONE);

        // 关闭键盘锁功能,即处于解锁状态
        if (!m_bEnable)
        {
            LockKeyPad(false);
        }

        // send switch msg
        etl_NotifyApp(false,
                      KEYGUARD_ENABLE_CHANGED,
                      m_bEnable,
                      0);
    }
}

// 是否开启/关闭
bool CKeyGuardManager::IsEnable()
{
    return m_bEnable;
}

void CKeyGuardManager::LockKeyPad(bool bLock)
{
    if (!m_bEnable
            && bLock)
    {
        return;
    }

    if (m_bLock != bLock)
    {
        m_bLock = bLock;
        //解锁时停止自动加锁记时
        if (m_bLock == false)
        {
            timerKillThreadTimer((UINT)&m_iAutoLockTime);
        }
        // send lock change msg
        etl_NotifyApp(false, KEYGUARD_LOCK_CHANGED, m_bLock, 0);
    }
}

// 是否上锁
bool CKeyGuardManager::IsKeyPadLock()
{
    return m_bLock; //由于上电时有模拟按键进入通话界面，所以未初始化前的按键不锁定
}

// 设置键盘锁类型
void CKeyGuardManager::SetKeyGuardType(KEY_GUARD_TYPE eKGType)
{
    if (eKGType != m_eKeyGuardType)
    {
        m_eKeyGuardType = eKGType;

        // save config
        configParser_SetConfigInt(kszKeyGuardType, m_eKeyGuardType, CONFIG_LEVEL_PHONE);
    }
}

// 返回键盘锁类型
KEY_GUARD_TYPE CKeyGuardManager::GetKeyGuardType()
{
    return m_eKeyGuardType;
}

bool CKeyGuardManager::ProcessMsg(msgObject & msg)
{
    // 如果键盘锁未激活或者未上锁则不处理这些消息
    if (!IsEnable() || !IsKeyPadLock())
    {
        return false;
    }

    bool bResult = false;
    switch (msg.message)
    {
    case DSK_MSG_MAKE_CALL:
        {
            // 在Allkeys模式下密码验证通过则允许拨号
            if (m_eKeyGuardType == KG_ALL_KEYS
                    && m_pCheckUnlockPswFun != NULL
                    && m_pCheckUnlockPswFun())
            {
                bResult = true;
            }
        }
        break;
    default:
        break;
    }

    return bResult;
}

// 是否可以进入某个功能
// pExtraData[in] 附加数据
bool CKeyGuardManager::IsEnableModuld(KEY_GUARD_MODULE eModule)
{
    // 键盘锁没有开启,都可以进入
    if (!m_bEnable || !m_bLock)
    {
        return true;
    }

    // 根据不同的键盘锁类型和模块判断能否进入界面
    bool bEnable = true;// 默认可以进入
    switch (m_eKeyGuardType)
    {
    case KG_ALL_KEYS:
    case KG_FUN_KEY:
        {
            // 只允许进入通话类型的界面
            bEnable = (KGM_MODULE_TALK == eModule);
        }
        break;
    case KG_MENU_KEY:
        {
            // 非Setting才允许进入
            bEnable = (KGM_MODULE_SETTING != eModule);
        }
        break;
    default:
        break;
    }

    // 如果密码验证通过则允许通过
    if (!bEnable && NULL != m_pCheckUnlockPswFun)
    {
        m_pCheckUnlockPswFun();
    }

    return bEnable;
}

// 设置验证解锁密码的回调函数
void CKeyGuardManager::SetCheckUnlockPswFun(CheckUnlockPswFun pFun)
{
    m_pCheckUnlockPswFun = pFun;
}

// 设置非紧急号码的提示框函数
void CKeyGuardManager::SetEmergencyTip(PopupEmergencyNumNote pFun)
{
    m_pEmergencyTip = pFun;
}

// 弹出紧急号码的提示框
void CKeyGuardManager::PopEmergencyTip()
{
    if (NULL != m_pEmergencyTip)
    {
        m_pEmergencyTip(TRID_NOT_A_EMERGENCY_NUMBER);
    }
}

void CKeyGuardManager::ResetKeypadLockTimer()
{
    if (!IsEnable())
    {
        timerKillThreadTimer((UINT)&m_iAutoLockTime);
        return;
    }

    if (m_iAutoLockTime > 0)
    {
        timerSetThreadTimer((UINT)&m_iAutoLockTime, m_iAutoLockTime);
    }
}

BOOL CKeyGuardManager::OnTimer(UINT iTimerId)
{
    //超时锁定键盘
    if (iTimerId == (UINT)&m_iAutoLockTime)
    {
        if (IsEnable())
        {
            LockKeyPad(true);
        }
        timerKillThreadTimer((UINT)&m_iAutoLockTime);
        return TRUE;
    }
    return FALSE;
}

void CKeyGuardManager::StopKeypadLockTimer()
{
    timerKillThreadTimer((UINT)&m_iAutoLockTime);
}

void CKeyGuardManager::SetLockTimerValue(int iLockTimer)
{
    if (iLockTimer == m_iAutoLockTime)
    {
        return ;
    }

    m_iAutoLockTime = iLockTimer;

    if (IsEnable())
    {
        // 通知锁定时间发生改变
        etl_NotifyApp(false, KEYGUARD_AUTOLOCK_TIME_CHANGED, 0, 0);
    }
}
