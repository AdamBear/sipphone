#include <ETLLib.hpp>
#include "configiddefine.h"
#include "powerledmessagedefine.h"
#include "account/include/accountmessagedefine.h"
#include "lamp/ledlamp/include/modledlamp.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "configparser/modconfigparser.h"
#include "account/include/modaccount.h"
//#include "setting/include/modsetting.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "powerledmanager.h"

#include "broadsoft/callcenter/include/modcallcenter.h"
#include "autopcontrol/include/modautopcontrol.h"
#include "adapterbox/include/modadapterbox.h"
#include "powersaving/include/modpowersaving.h"
#include "backlight/include/modbacklight.h"

#if IF_BUG_11498
#include "idlescreen/include/modidlescreen.h"
#endif
#include "contacts/directory/include/moddirinterface.h"

//消息回调函数
static LRESULT OnPowerLedManagerMsg(msgObject & objMessage)
{
    if (CONFIG_MSG_BCAST_CHANGED == objMessage.message)
    {
        // 配置改变消息
        return _PowerLedManager.OnConfigChanged(objMessage);
    }
    else
    {
        // 事件消息
        return _PowerLedManager.OnPowerLedEvent(objMessage);
    }
}

CPowerLedManager::CPowerLedManager()
{
    m_bPowerLedLogicEnable = false;
    m_bEnableLedIdleTalk = false;
    m_bEnableLedMail = false;
    m_bForbidden = false;
    m_iLastStatus = 0;
}

CPowerLedManager::~CPowerLedManager()
{

}

CPowerLedManager & CPowerLedManager::GetInstance()
{
    static CPowerLedManager objManager;
    return objManager;
}

void CPowerLedManager::Init()
{
    // 注册消息
    RegisterMessage();

    LEDLAMP_INFO("CPowerLedManager::Init Enable[%d] Idle&Talk[%d] Mail[%d]", m_bPowerLedLogicEnable,
                 m_bEnableLedIdleTalk, m_bEnableLedMail);
    if (!m_bPowerLedLogicEnable)
    {
        LEDLAMP_INFO("PowerLed Logic Disable");
        return;
    }

    FlushPowerLed();
}

void CPowerLedManager::RegisterMessage()
{
    // 监视配置改变消息
    etl_RegisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED, OnPowerLedManagerMsg);

    // 监视PowerLed事件
    etl_RegisterMsgHandle(POWER_LED_MESSAGE_BEGIN, POWER_LED_MESSAGE_END, OnPowerLedManagerMsg);
}

bool CPowerLedManager::OnPowerLedEvent(msgObject & objMessage)
{
    FlushPowerLed();
    return true;
}

bool CPowerLedManager::OnConfigChanged(msgObject & objMessage)
{
    if (objMessage.wParam != ST_POWERLED)
    {
        return false;
    }
    FlushPowerLed();
    return true;
}

void CPowerLedManager::Forbid(BOOL bForbidden)
{
    FlushPowerLed();
}

void CPowerLedManager::FlushPowerLed()
{
    if (PT_DECT == devicelib_GetPhoneType())
    {
        //dect不需要处理Led
        return;
    }

    // bEnable 判断当前电源灯对应逻辑功能注册还是注销
    int iStatus = 0;
    bool bEnable = false;

    do
    {
        // AutopFinish闪灯
        bEnable = AUT_NORMAL == Autop_GetUpdateType();
        ledlamp_PostEvent(LAMP_EVENTTYPE_AUTOPFINISH, bEnable);

        //TODO. 这里存在问题，移位超出了范围
        iStatus += bEnable << LAMP_EVENTTYPE_AUTOPFINISH;

        // Ringing闪灯
        bEnable = talklogic_IsRingSessionExist()
                  && (configParser_GetConfigInt(kszPowerLightControlRinging) != 0) ;
        ledlamp_PostEvent(LAMP_EVENTTYPE_INCALL, bEnable);
        iStatus += bEnable << LAMP_EVENTTYPE_INCALL;

        // ACD闪灯
        bEnable = CallCenter_IsACDOn()
                  && AS_IDLE != ACD_GetAcdStatus()
                  && ACD_BROADSOFT == Get_ACDType()
                  && QUEUE_THRESHOLD_EXCEEDED == ACD_GetQueueStatus().m_eStatus
                  && CallCenter_IsEnableQueueStatusLightOn();
        ledlamp_PostEvent(LAMP_EVENTTYPE_ACDALERT, bEnable);
        iStatus += bEnable << LAMP_EVENTTYPE_ACDALERT;

        // hold闪灯
        TALK_STATE eState = talklogic_GetFocusedSessionTalkState();

        //http://bugfree.yealink.com/Bug.php?BugID=122673
        //不允许半咨询转时用盲转替代时， 话路隐藏 存在问题，将hold灯增加焦点话路是否可见判断
        bool bHide = talklogic_GetFocusedSessionHideState();
        bEnable = (configParser_GetConfigInt(kszPowerLightControlHold) != 0) && (eState == TS_HOLD
                  || eState == TS_HELD);
        ledlamp_PostEvent(LAMP_EVENTTYPE_HOLD, bEnable);
        iStatus += bEnable << LAMP_EVENTTYPE_HOLD;

        // Mute闪灯
        bEnable = talklogic_GetMuteStatus() && (configParser_GetConfigInt(kszPowerLightControlMute) != 0) ;
        ledlamp_PostEvent(LAMP_EVENTTYPE_MUTE_POWER_LIGHT, bEnable);
        iStatus += bEnable << LAMP_EVENTTYPE_MUTE_POWER_LIGHT;
        // Talk闪灯
        bEnable = BeHaveSession() && (configParser_GetConfigInt(kszPowerLightControlTalk) != 0) ;
        ledlamp_PostEvent(LAMP_EVENTTYPE_TALKING, bEnable);
        iStatus += bEnable << LAMP_EVENTTYPE_TALKING;

        // 有通话，则需要根据kszPowerLightControlTalk配置确认电源灯亮灭
        if (talklogic_SessionExist()
                && (0 == configParser_GetConfigInt(kszPowerLightControlTalk)))
        {
            ledlamp_PostEvent(LAMP_EVENTTYPE_MESSAGE, false);
            ledlamp_PostEvent(LAMP_EVENTTYPE_COMMON, false);
            ledlamp_PostEvent(LAMP_EVENTTYPE_MISSCALL, false);
            if (DSK_PHONE_TYPE_COLOR == commonUnits_GetDskPhoneType())
            {
                ledlamp_PostEvent(LAMP_EVENTTYPE_POWERSAVING, false);
            }

            break;
        }
        // Voice mail Text Message闪灯
        bEnable = BeHaveMail() && (configParser_GetConfigInt(kszPowerLightControlMessage) != 0) ;
        ledlamp_PostEvent(LAMP_EVENTTYPE_MESSAGE, bEnable);
        iStatus += bEnable << LAMP_EVENTTYPE_MESSAGE;

        // Miss Call闪灯
        bEnable = BeHaveMissCall() && (configParser_GetConfigInt(kszPowerLightControlMissCall) != 0) ;
        ledlamp_PostEvent(LAMP_EVENTTYPE_MISSCALL, bEnable);
        iStatus += bEnable << LAMP_EVENTTYPE_MISSCALL;

        if (DSK_PHONE_TYPE_COLOR == commonUnits_GetDskPhoneType())
        {
            // PowerSaving闪灯
            bEnable = PowerSaving_IsInPowerSavingMode() || backlight_GetCurrenLightLevel() == 0;
            ledlamp_PostEvent(LAMP_EVENTTYPE_POWERSAVING, bEnable);
            iStatus += bEnable << LAMP_EVENTTYPE_POWERSAVING;
        }

        // Common
        bEnable = (configParser_GetConfigInt(kszPowerLightControlCommon) != 0) ;
        ledlamp_PostEvent(LAMP_EVENTTYPE_COMMON, bEnable);
        iStatus += bEnable << LAMP_EVENTTYPE_COMMON;
    }
    while (false);

    //状态改变，取消CUSTOM灯
    //http://10.2.1.199/Bug.php?BugID=57035
    if (m_iLastStatus != iStatus)
    {
        ledlamp_PostEvent(LAMP_EVENTTYPE_CUSTOM, false);
        m_iLastStatus = iStatus;
    }
}

// 是否有语音或短信 信息
bool CPowerLedManager::BeHaveMail()
{
#ifndef _T49
    return acc_GetUnReadVoiceMailNum() > 0 || AdapterBox_GetGetTotalUnReadMessage();
    return false;
#else
    return AdapterBox_VoiceMailGetUnReadNum(-1, true) > 0 || Setting_GetTotalUnReadMessage() > 0;
#endif
}

bool CPowerLedManager::BeHaveSession()
{
    if ((DEVICELIB_IS_CP(devicelib_GetPhoneType()))
            && talklogic_GetSessionNum() == 1
            && talklogic_IsPreDialSessionExist())
    {
        return false;
    }

    return talklogic_SessionExist();
}


bool CPowerLedManager::BeHaveMissCall()
{
#if IF_BUG_11498
    int nStatus = idleScreen_GetPopupStatus(PS_STATE_MISSCALL);
    if (nStatus == POPUP_WINDOW_POPUPING
            || nStatus == POPUP_WINDOW_POPUP
            || nStatus == POPUP_WINDOW_TEXT_NOTIFY)
    {
        return true;
    }
    return false;
#else
    int nMissCallCount = Dir_GetContactCount(kszMissNotifId, LOCAL_LOG);
    return nMissCallCount > 0;
#endif
}
