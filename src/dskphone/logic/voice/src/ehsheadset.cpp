#include "ehsheadset.h"
#include "voicechannel_include.h"
#include "modvoice.h"
#include "commonapi/stringhelperapi.h"
#include "commonapi/systemhelperapi.h"
#include "exp/src/expdrive/deviceplugin/modexpscan.h"
#include "commonunits/modcommonunits.h"

// 注：在构造和析构中不要调用threadtimer相关的函数，否则会出错
// 原因是在扫描EXP按键的线程中会去调用EHS插拔的接口，那个线程是匿名的不支持timer
CEHSHeadset::CEHSHeadset(int iIndex)
{
    m_iIndex = iIndex;
    m_bStatus = FALSE;
    m_bRing = FALSE;
    m_iRingInterval = 3000;
    m_iDelayStopRing = 0;
#if IF_BUG_27190
    m_bPhoneRing = false;
#endif
    InitStatus();
}

CEHSHeadset::~CEHSHeadset()
{
    timerKillThreadTimer((UINT)&m_iRingInterval);
    timerKillThreadTimer((UINT)&m_iDelayStopRing);
}

void CEHSHeadset::InitStatus()
{
    if (voice_GetCurrentChannel() == CHANNEL_HEADSET
            || voice_GetCurrentChannel() == CHANNEL_IDLE && VoiceManager.m_objDevices.GetHeadsetStatus())
    {
        VOICE_INFO("InitStatus Open");
        Open(TRUE);
    }
    else
    {
        VOICE_INFO("InitStatus Close");
        // 初始化时强制关掉EHS,否则状态会错
        Close(TRUE);
    }
}

void CEHSHeadset::SwitchEHSStatus()
{
    if (voice_GetCurrentChannel() == CHANNEL_HEADSET
            || voice_GetCurrentChannel() == CHANNEL_IDLE && VoiceManager.m_objDevices.GetHeadsetStatus())
    {
        Open();
    }
}

void CEHSHeadset::Open(bool bForceToClose/* = FALSE*/)
{
    if (!bForceToClose && m_bStatus)
    {
        VOICE_WARN("EHS is already opened");
        return;
    }

//     BYTE byTmp[8] = {0};
//     byTmp[0] = m_iIndex << 2;
//     byTmp[1] = 0x00;

    m_bStatus = TRUE;

    extern bool g_bAfterInit;
    if (!g_bAfterInit)
    {
        return;
    }
    exp_EHSCtrl(m_iIndex, 0x00);
    // 激活无线耳麦命令字:0x00
    //commonAPI_Ioctl(EXT_LED_CTRL, (int)byTmp);
}

void CEHSHeadset::Close(bool bForceToClose/* = FALSE*/)
{
    if (!bForceToClose && !m_bStatus)
    {
        VOICE_WARN("EHS is already closed");
        return;
    }

    if (m_iIndex < 0)
    {
        return;
    }

    VOICE_INFO("Close EHS:index:%d", m_iIndex);
    //StopRing();
    m_bRing = FALSE;
    timerKillThreadTimer((UINT)&m_iRingInterval);
    m_bStatus = FALSE;

    BYTE byTmp[8] = {0};//命令字
    byTmp[1] = 0x01;
    byTmp[0] = m_iIndex << 2;

    extern bool g_bAfterInit;
    if (!g_bAfterInit)
    {
        return;
    }
    exp_EHSCtrl(m_iIndex, 0x01);
    //激活无线耳麦命令字:0x000
    //commonAPI_Ioctl(EXT_LED_CTRL, (int)byTmp);
}

void CEHSHeadset::PlayRing()
{
    if (m_bRing)
    {
        return;
    }

    m_bRing = TRUE;
    SendRingMsgToDev();
    //PlayRing之前杀死已经存在的延时（500ms）StopRing，否则播放铃声失败
    timerKillThreadTimer((UINT)&m_iDelayStopRing);
    timerSetThreadTimer((UINT)&m_iRingInterval, m_iRingInterval);
}

#if IF_BUG_27190
void CEHSHeadset::PlayPhoneRing()
{
    Open();
    m_bPhoneRing = true;
    m_bStatus = false;
}
#endif

void CEHSHeadset::SendRingMsgToDev()
{
    // 播放无线耳麦铃声命令字:0x02
    //exp_helper_set_ehs_hook(g_iEXPHandle, m_iIndex, 0x02);
    if (m_iIndex < 0)
    {
        return;
    }

//     BYTE byTmp[10] = {0};//命令字
//     byTmp[1] = 0x02;
//     byTmp[0] = m_iIndex<<2;

    extern bool g_bAfterInit;
    if (!g_bAfterInit)
    {
        return;
    }

    exp_EHSCtrl(m_iIndex, 0x02);
    // commonAPI_Ioctl(EXT_LED_CTRL, (int)byTmp);
}

void CEHSHeadset::StopRing(bool bForceStopEHS/*= TRUE*/)
{
#if IF_BUG_27190
    if (m_bPhoneRing)
    {
        Open(TRUE);
        m_bPhoneRing = false;
        return;
    }
#endif
    if (m_bRing)
    {
        VOICE_INFO("EHS StopRing...");
        m_bRing = FALSE;
        timerKillThreadTimer((UINT)&m_iRingInterval);

//      if (!bForceStopEHS)
//      {
//          // 非强制关闭的方式则不发送开关命令
//          return;
//      }

        // 来电接起时,为了关闭无线耳麦响铃,会先关闭无线耳麦再开启
        // 短时间进行关闭在开启会导致无线耳麦自动上传一个按键,导致挂断通话
        // 延迟500ms处理,进入通过后
        timerSetThreadTimer((UINT)&m_iDelayStopRing, 500);
    }
}

bool CEHSHeadset::OnTimer(unsigned int uTimeID)
{
    bool bHandled = FALSE;
    if ((UINT)&m_iRingInterval == uTimeID)
    {
        bHandled = TRUE;
        SendRingMsgToDev();
    }
    else if ((UINT)&m_iDelayStopRing == uTimeID)
    {
        VOICE_INFO("time out to stop ring...");
        timerKillThreadTimer(uTimeID);
        if (m_bStatus)
        {
            // 给EHS强制发送一次打开的命令,以停止铃声
            Open(TRUE);
        }
        else
        {
            // 给EHS强制发送一次关闭的命令,以停止铃声
            Close(TRUE);
        }
    }

    return bHandled;
}

void CEHSHeadset::OnKeyPressed(bool bOnlyChangeChannel /*= FALSE*/)
{
    m_bStatus = !m_bStatus;
    int iKeyCode = 0;
    if (m_bStatus)
    {
        iKeyCode = PHONE_KEY_EHS_OPEN;
    }
    else
    {
        iKeyCode = PHONE_KEY_EHS_CLOSE;
    }

    if (!bOnlyChangeChannel)
    {
        commonUnits_SendSimulateKey(DEV_MSG_FUNKEY_EVENT, iKeyCode);
    }
    else
    {
        voice_OnKeyPress(iKeyCode);
    }
}

int CEHSHeadset::GetIndex()
{
    return m_iIndex;
}
