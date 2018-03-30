#include "linedevicemanager.h"

#ifdef IF_SUPPORT_LINE_DEVICE
#include "service_ipvp.h"
#include "interfacedefine.h"
#include "devicelib/keypad_drv.h"
#include "autopcontrol/include/modautopcontrol.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "extended/testmodemessagedefine.h"
#include "adapterbox/include/modadapterbox.h"
#include "talk/talklogic/include/modtalklogic.h"

IMPLEMENT_GETINSTANCE(CLineDeviceManager)

CLineDeviceManager::CLineDeviceManager()
{
    m_iCheckTimer = 1000;
}

CLineDeviceManager::~CLineDeviceManager()
{
}

// 接收VPM下发的消息
LRESULT CLineDeviceManager::OnMessage(msgObject & msg)
{
    switch (msg.message)
    {
    case TM_TIMER:
        {
            _LineDeviceManager.OnTimer(msg.wParam);
        }
        break;
    case TESTMODE_MESSAGE_EXIT:
    case SETTING_MESSAGE_WIZARD_EXIT:
    case SETTING_MESSAGE_AUTOPCONTROL_EXIT:
        {
            if (_LineDeviceManager.IsNeedShowLineDevice()
                    && _LineDeviceManager.IsConnect())
            {
                talklogic_EnterLineDeviceTalk();
            }
        }
        break;
    default:
        break;
    }

    return 1;
}

// 初始化
void CLineDeviceManager::Init()
{
    // 退出测试模式/退出向导/退出Zero、RPS
    SingleMsgReg(SETTING_MESSAGE_AUTOPCONTROL_EXIT, CLineDeviceManager::OnMessage);
    SingleMsgReg(SETTING_MESSAGE_WIZARD_EXIT, CLineDeviceManager::OnMessage);
    SingleMsgReg(TESTMODE_MESSAGE_EXIT, CLineDeviceManager::OnMessage);
    SingleMsgReg(TM_TIMER, CLineDeviceManager::OnMessage);

    // 设置定时器，检测是否已插上设备
    timerSetThreadTimer((UINT)&m_iCheckTimer, m_iCheckTimer);
}

// 定时器处理
bool CLineDeviceManager::OnTimer(UINT uTimerID)
{
    if (uTimerID != (UINT)&m_iCheckTimer)
    {
        return false;
    }

    timerKillThreadTimer((UINT)&m_iCheckTimer);

    if (IsNeedShowLineDevice() && IsConnect())
    {
        // 进入MobilePhone通话
        talklogic_EnterLineDeviceTalk();
    }

    return true;
}

bool CLineDeviceManager::IsConnect(bool bCheckMobile/* = true*/)
{
    int iStatus = -1;
    bool bRet = false;

#if LINUX_SYSTEM
    extern int g_iKeyDes;

    if (!bCheckMobile)
    {
        bRet = (0 == keypad_helper_get_hsdsta(g_iKeyDes, &iStatus));
        bRet = (bRet && (1 == iStatus));
    }
    else
    {
        bRet = (0 == keypad_helper_get_mbssta(g_iKeyDes, &iStatus));
        bRet = (bRet && (0 == iStatus));
    }
#endif

    APP_INFO("MobilePhone Connect, iRet[%d], iStatus[%d]", bRet, iStatus);
    return bRet;
}

// 插出拔出处理
bool CLineDeviceManager::ProcessDeviceEvent(int iKeyEvent)
{
    // 测试模式, 不处理
    if (AdapterBox_IsTestMode())
    {
        return false;
    }

    // Zero/向导, 已处理
    if (!IsNeedShowLineDevice())
    {
        return true;
    }

    bool bHandle = true;
    switch (iKeyEvent)
    {
    case PHONE_KEY_LINEDEVICE_ON_HOOK:
        {
            if (IsConnect())
            {
                talklogic_EnterLineDeviceTalk();
            }
        }
        break;
    case PHONE_KEY_LINEDEVICE_OFF_HOOK:
        {
            talklogic_ExitLineDeviceTalk();
        }
        break;
    default:
        bHandle = false;
        break;
    }

    return bHandle;
}

// 是否允许处理外界设备
bool CLineDeviceManager::IsNeedShowLineDevice()
{
    return (!AdapterBox_IsTestMode()
            && AUT_NONE == Autop_GetUpdateType()
            && !AdapterBox_IsIsInWizard());
}

#endif // IF_SUPPORT_LINE_DEVICE
