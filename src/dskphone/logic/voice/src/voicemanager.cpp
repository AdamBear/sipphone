#include "voicechannel_include.h"
#include "ehsheadset.h"
#include "channelforresume.h"

#include "commonunits/modcommonunits.h"
#include "devicelib/phonedevice.h"

#include "commonapi/stringhelperapi.h"
#include "commonapi/systemhelperapi.h"
#include "dsskey/include/moddsskey.h"

#include "service_ipvp.h"

#include "record/include/modrecord.h"

#include "ylstl/ylalgorithm.h"
#ifdef IF_SUPPORT_USB_AUDIO
#include "usb_headset/include/modusbheadset.h"
#endif //IF_SUPPORT_USB_AUDIO



CVoiceManager * CVoiceManager::s_pInstance = NULL;

CVoiceManager & CVoiceManager::GetInstance()
{
    if (!s_pInstance)
    {
        s_pInstance = new CVoiceManager();
    }
    return *s_pInstance;
}

void CVoiceManager::ReleaseInstance()
{
    SAFE_DELETE(s_pInstance);
}

bool CVoiceManager::IsSupportHandset()
{
    return !DEVICELIB_IS_CP(devicelib_GetPhoneType());
}

bool CVoiceManager::IsSupportHeadset()
{
    return !DEVICELIB_IS_CP(devicelib_GetPhoneType());
}

CVoiceManager::CVoiceManager()
{
    m_iLocalDtmfOffTimer = 0;
    m_funUpdateChannelInfo = NULL;
    UpdateAccessStatus();
    SingleMsgReg(TM_TIMER, CMSGSubscriber(this, &CVoiceManager::OnTimer));
    SingleMsgReg(DSK_MSG_SELECT_CHANNEL, CMSGSubscriber(this, &CVoiceManager::OnSelectChannel));
    SingleMsgReg(MKIT_MSG_SRVREG, CMSGSubscriber(this, &CVoiceManager::OnVPMRegistMsg));
    SingleMsgReg(CONFIG_MSG_BCAST_CHANGED, CMSGSubscriber(this, &CVoiceManager::OnConfigChange));
}

CVoiceManager::~CVoiceManager()
{
    SingleMsgUnReg(TM_TIMER, CMSGSubscriber(this, &CVoiceManager::OnTimer));
    SingleMsgUnReg(DSK_MSG_SELECT_CHANNEL, CMSGSubscriber(this, &CVoiceManager::OnSelectChannel));
    SingleMsgUnReg(MKIT_MSG_SRVREG, CMSGSubscriber(this, &CVoiceManager::OnVPMRegistMsg));
    SingleMsgUnReg(CONFIG_MSG_BCAST_CHANGED, CMSGSubscriber(this, &CVoiceManager::OnConfigChange));
}

void CVoiceManager::InitVoice()
{
    // msgkit 机制下不需要等待ipvp启动
    if (mkservice_get_data(mkGetHandle(), MKSRV_ID_IPVP, 0) <= 0)
    {
        return;
    }

    VOICE_INFO("Init ipvp");
    ipvp_init(mkGetHandle(), 0);

    // 通道切换为idle
    m_pIdleSession = AllocSession(CHANNEL_IDLE);

    // 通知外部vpm启动
    etl_NotifyApp(false, VOICE_VPM_START, 0, 0);
}

VoiceSessionPtr CVoiceManager::AllocSession(CHANNEL_MODE eChannel)
{
    VoiceSessionPtr pSession(new CVoiceSession(eChannel));
    VOICE_INFO("Allocate voice session [%p]", pSession.Get());
    PauseActiveSession();
    m_sessions.push_back(yl::make_pair<CVoiceSession *, VoiceSessionWeakPtr>(pSession.Get(),
                         pSession));
    ResumeActiveSession();
    VOICE_INFO("Voice session cnt: %d", m_sessions.size());
    return pSession;
}

void CVoiceManager::FreeSession(const CVoiceSession * session)
{
    if (IsSessionStackEmpty())
    {
        VOICE_ERR("Voice session stack is empty! Cannot release session!");
        return;
    }
    VOICE_INFO("Release voice session [%p]", session);
    bool isTop = session == GetActiveSession().lock().Get(); // 是否释放顶部Session
    for (SessionStack::iterator it = m_sessions.begin(); it != m_sessions.end();)
    {
        if (session == (*it).first)
        {
            it = m_sessions.erase(it);
        }
        else
        {
            ++it;
        }
    }
    VOICE_INFO("Voice session cnt: %d", m_sessions.size());
    // 如果释放顶部Session（且全局初始化完成），则应该切换通道
    if (isTop)
    {
        // 新的顶部Session获得焦点
        ResumeActiveSession();
    }
}

void CVoiceManager::ActivateSession(CVoiceSession * session)
{
    // 如果指定Session已经在顶部，则直接返回
    if (GetActiveSession().lock().Get() == session)
    {
        VOICE_INFO("Voice session [%p] already activated", session);
        session->Resume();
        return;
    }
    VOICE_INFO("Activate voice session [%p]", session);
    // 从列表中移除指定Session
    VoiceSessionWeakPtr ptr;
    for (SessionStack::iterator it = m_sessions.begin(); it != m_sessions.end();)
    {
        if (session == (*it).first)
        {
            ptr = (*it).second;
            it = m_sessions.erase(it);
        }
        else
        {
            ++it;
        }
    }
    // 挂起原来的顶部Session
    PauseActiveSession();
    // 指定Session添加到顶部
    m_sessions.push_back(yl::make_pair<CVoiceSession *, VoiceSessionWeakPtr>(session, ptr));
    ResumeActiveSession();
}

bool CVoiceManager::IsSessionStackEmpty() const
{
    return m_sessions.size() == 0;
}

VoiceSessionWeakPtr CVoiceManager::GetActiveSession() const
{
    if (IsSessionStackEmpty())
    {
        VOICE_WARN("No top voice session");
        return VoiceSessionWeakPtr();
    }
    // 反向遍历（队位是栈顶）
    SessionStack::const_iterator it = --m_sessions.end();
    do
    {
        VoiceSessionPtr pSession = (*it).second.lock();
        if (pSession.IsEmpty())
        {
            VOICE_WARN("Invalid session in stack! [%p]", (*it).first);
            if (it == m_sessions.begin())
            {
                break;
            }
            else
            {
                --it;
                continue;
            }
        }
        VOICE_INFO("Got top voice session [%p]", pSession.Get());
        return pSession;
    }
    while (true);
    VOICE_WARN("No valid voice session");
    return VoiceSessionWeakPtr();
}

// 摘挂机一定会更改摘挂状态
void CVoiceManager::ChangeHandsetStatus(int iKey)
{
    switch (iKey)
    {
    case PHONE_KEY_HANDSET_OFF_HOOK:
        {
            m_objDevices.SetHandsetStatus(true);
        }
        break;
    case PHONE_KEY_HANDSET_ON_HOOK:
        {
            m_objDevices.SetHandsetStatus(false);
        }
        break;
    default:
        break;
    }
}

// 通道按键处理
bool CVoiceManager::OnKeyPress(int iKey)
{
#if VOICE_INFO_DEBUG
    VOICE_INFO("CVoiceManager::OnKeyPress [%d]", iKey);
#endif //

#ifdef IF_FEATURE_HEADSETKEY_IN_TALK
    // http://192.168.1.99/Bug.php?BugID=19141
    // 焦点是通话界面，且耳麦打开的情况下不再处理channel按键
    if (m_objDevices.GetHeadsetStatus()
            && configParser_GetConfigInt(kszHeadSetKeyInTalkSwitch) == 0
            && talkLogic_IsSessionInTalking())
    {
        return false;
    }
#endif

#ifdef IF_FEATURE_DUALHEADSET
    if (voice_IsCallCenterMode())
    {
        // 呼叫中心模式时的通道键单独处理
        return OnKeyPressForCallCenter(iKey);
    }
#endif
    VoiceSessionPtr pSession = GetActiveSession().lock();
    switch (iKey)
    {
    case PHONE_KEY_HANDSET_OFF_HOOK:
        {
            m_objDevices.SetHandsetStatus(true);
            if (!pSession.IsEmpty())
            {
                pSession->SwitchChannel(CHANNEL_HANDSET);
            }
        }
        break;
    case PHONE_KEY_HANDSET_ON_HOOK:
        {
            m_objDevices.SetHandsetStatus(false);
        }
        break;
    case PHONE_KEY_HANDFREE:
        {
            if (!pSession.IsEmpty()
                    && pSession->GetCurrentChannel() == CHANNEL_GROUP
                    && pSession->GetGroupType() == CVoiceSession::GROUP_HEADSET)
            {
                // 特殊处理，因为群听时会将m_bHandfree置为TRUE
                // 耳麦群听时按免提，则要打开免提通道
                pSession->SwitchChannel(CHANNEL_HANDFREE);
            }
            else
            {
                m_objDevices.SetHandfreeStatus(!m_objDevices.GetHandfreeStatus());
                if (!pSession.IsEmpty())
                {
                    if (m_objDevices.GetHandfreeStatus())
                    {
                        pSession->SwitchChannel(CHANNEL_HANDFREE);
                    }
                    else if (m_objDevices.GetHandsetStatus())
                    {
                        pSession->SwitchChannel(CHANNEL_HANDSET);
                    }
                }
            }
        }
        break;
    case PHONE_KEY_HEADSET_CTRL:
        {
            if (!m_objDevices.GetHeadsetStatus())
            {
                // 耳麦关闭则将耳麦打开
                m_objDevices.SetHeadsetStatus(true);
                if (!pSession.IsEmpty())
                {
                    pSession->SwitchChannel(CHANNEL_HEADSET, true);

                    if (configParser_GetConfigInt(kszHeadsetCtrlCall) == 1)
                    {
                        // 打开耳麦操作, 进入耳麦通话, 不需要再切换到idle
                        break;
                    }

                    // 如果不存在通话，则应该是IDLE通道状态
                    if (!talklogic_SessionExist())
                    {
                        pSession->SwitchChannel(CHANNEL_IDLE);
                    }
                }
            }
            else if (!pSession.IsEmpty())
            {
                // 如果耳麦已打开
                if (pSession->GetCurrentChannel() == CHANNEL_HEADSET)
                {
                    // 当前通道在耳麦，则关闭耳麦
                    m_objDevices.SetHeadsetStatus(false);

                    if (m_objDevices.GetHandsetStatus())
                    {
                        pSession->SwitchChannel(CHANNEL_HANDSET);
                    }
                    else
                    {
                        // 耳麦控制通话, 关闭耳麦不能再切换到免提
                        if (configParser_GetConfigInt(kszHeadsetCtrlCall) == 0)
                        {
                            pSession->SwitchChannel(CHANNEL_HANDFREE);
                        }
                    }
                }
                else if (pSession->GetCurrentChannel() == CHANNEL_IDLE)
                {
                    if (configParser_GetConfigInt(kszHeadsetCtrlCall) == 0)
                    {
                        // Idle界面时，当前通道在耳麦，则关闭耳麦，并打开手柄通道
                        CloseHeadsetOnIdle();
                    }
                }
                else
                {
                    // 当前通道不在耳麦，则打开耳麦
                    pSession->SwitchChannel(CHANNEL_HEADSET, true);
                }
            }
        }
        break;
    case PHONE_KEY_EHS_OPEN:
        {
            if (!pSession.IsEmpty())
            {
                pSession->SwitchChannel(CHANNEL_HEADSET, true);
            }
        }
        break;
    case PHONE_KEY_EHS_CLOSE:
    default:
        return FALSE;
        break;
    }

    // 通道按键对重置通道的影响
    _ChannelResume.ChannelKeyPress();

    return TRUE;
}

// Idle界面下按耳麦键关闭耳麦
void CVoiceManager::CloseHeadsetOnIdle()
{
#ifdef IF_FEATURE_HEADSET_PRIOR
    // idle界面按耳麦键关闭耳麦才真正关闭
    if (configParser_GetConfigInt(kszLastHeadSet) != 0)
    {
        configParser_SetConfigInt(kszLastHeadSet, 0, CONFIG_LEVEL_PHONE);
    }
#endif

    m_objDevices.SetHeadsetStatus(false);
    VoiceSessionPtr pSession = GetActiveSession().lock();
    if (!pSession.IsEmpty())
    {
        pSession->SwitchChannel(CHANNEL_IDLE);
    }
}

#ifdef IF_FEATURE_DUALHEADSET
// CallCenter模式下的通道按键处理
bool CVoiceManager::OnKeyPressForCallCenter(int iKey)
{
    //VOICE_INFO("CVoiceManager::OnKeyPressForCallCenter iKey=%d", iKey);
    VoiceSessionPtr pSession = GetActiveSession().lock();
    switch (iKey)
    {
    case PHONE_KEY_HANDSET_OFF_HOOK:
        {
            m_objDevices.SetHandsetStatus(true);

            // 对Idle无影响，来电响铃也不能影响
            if (talklogic_SessionExist()
                    && !talklogic_IsAllSessionRinging())
            {
                VOICE_INFO("OnKeyPressForCallCenter m_bHeadset=%d", m_objDevices.GetHeadsetStatus());
                // 耳麦开启，摘机则切到双耳麦，否则切回耳麦
                if (m_objDevices.GetHeadsetStatus() && !pSession.IsEmpty())
                {
                    pSession->SwitchChannel(CHANNEL_2HEADSET);
                }
            }
        }
        break;
    case PHONE_KEY_HANDSET_ON_HOOK:
        {
            m_objDevices.SetHandsetStatus(false);

            // 对Idle无影响，来电响铃也不能影响
            if (talklogic_SessionExist()
                    && !talklogic_IsAllSessionRinging())
            {
                // 耳麦开启，摘机则切到双耳麦，否则切回耳麦
                if (m_objDevices.GetHeadsetStatus() && !pSession.IsEmpty())
                {
                    pSession->SwitchChannel(CHANNEL_HEADSET);
                }
            }
        }
        break;
    case PHONE_KEY_HANDFREE:
        {
            if (!pSession.IsEmpty()
                    && pSession->GetCurrentChannel() == CHANNEL_GROUP
                    && pSession->GetGroupType() == CVoiceSession::GROUP_HEADSET)
            {
                // 特殊处理，因为群听时会将m_bHandfree置为TRUE
                // 耳麦群听时按免提，则要打开免提通道
                pSession->SwitchChannel(CHANNEL_HANDFREE);
            }
            else
            {
                m_objDevices.SetHandfreeStatus(!m_objDevices.GetHandfreeStatus());
                if (m_objDevices.GetHandfreeStatus() && !pSession.IsEmpty())
                {
                    pSession->SwitchChannel(CHANNEL_HANDFREE);
                }
                else
                {
                    // 通话中可能是挂断通话的作用，不切换通道
                }
            }
        }
        break;
    case PHONE_KEY_HEADSET_CTRL:
        {
            if (!m_objDevices.GetHeadsetStatus())
            {
                // 耳麦关闭则将耳麦打开
                m_objDevices.SetHeadsetStatus(true);
                if (!pSession.IsEmpty())
                {
                    if (!talklogic_SessionExist()
                            && configParser_GetConfigInt(kszHeadsetCtrlCall) == 0)
                    {
                        pSession->SwitchChannel(CHANNEL_IDLE);
                    }
                    else
                    {
                        if (m_objDevices.GetHandsetStatus())
                        {
                            pSession->SwitchChannel(CHANNEL_2HEADSET, true);

                        }
                        else
                        {
                            pSession->SwitchChannel(CHANNEL_HEADSET, true);
                        }
                    }
                }
            }
            else if (!pSession.IsEmpty())
            {
                // 如果耳麦已打开
                if (pSession->GetCurrentChannel() == CHANNEL_HEADSET
                        || pSession->GetCurrentChannel() == CHANNEL_2HEADSET)
                {
                    m_objDevices.SetHeadsetStatus(false);

                    if (configParser_GetConfigInt(kszHeadsetCtrlCall) == 0)
                    {
                        pSession->SwitchChannel(CHANNEL_HANDFREE);
                    }
                }
                else if (pSession->GetCurrentChannel() == CHANNEL_IDLE)
                {
                    if (configParser_GetConfigInt(kszHeadsetCtrlCall) == 0)
                    {
                        // Idle界面时，当前通道在耳麦，则关闭耳麦，并打开手柄通道
                        CloseHeadsetOnIdle();
                    }
                }
                else
                {
                    // 当前通道不在耳麦，则打开耳麦
                    pSession->SwitchChannel(CHANNEL_HEADSET, true);
                }
            }
        }
        break;
    case PHONE_KEY_EHS_OPEN:
        {
            if (!pSession.IsEmpty())
            {
                pSession->SwitchChannel(CHANNEL_HEADSET, true);
            }
        }
        break;
    case PHONE_KEY_EHS_CLOSE:
    default:
        return FALSE;
        break;
    }

    // 通道按键对重置通道的影响
    _ChannelResume.ChannelKeyPress();

    return TRUE;
}
#endif

// 播放按键音
void CVoiceManager::PlayDTMF(int iKey, bool bPlayNow/* = false*/)
{
    bool bPlayDTMF = FALSE;
    VoiceSessionPtr pSession = GetActiveSession().lock();
    if (!pSession.IsEmpty() && pSession->IsPlayingRing())
    {
        // 播放铃声时不再播放按键音
        bPlayDTMF = FALSE;
    }
    else if (dsskey_IsDsskey(iKey)
             || iKey == PHONE_KEY_VOLUME_DECREASE || iKey == PHONE_KEY_VOLUME_INCREASE
             || voice_IsChannelKey(iKey))
    {
        bPlayDTMF = FALSE;
    }
    else
    {
        bPlayDTMF = TRUE;
    }

    if (bPlayDTMF)
    {
        // 播放按键音之前先停止播放信号音
        if (!pSession.IsEmpty() && pSession->IsPlayingTone())
        {
            pSession->StopTone();
        }

        PlayLocalDTMF(iKey, bPlayNow);
    }
}

void CVoiceManager::StopPlayLocalDTMF()
{
    // 如果关闭按键音，则不播放
    if (configParser_GetConfigInt(kszKeypressSound) == 0)
    {
        return;
    }

    if (m_listDtmfKey.size() <= 0
            || m_iLocalDtmfOffTimer == 0)
    {
        return;
    }

    timerKillThreadTimer((UINT &) m_listDtmfKey);
    m_listDtmfKey.clear();
    m_iLocalDtmfOffTimer = 0;
}

// 播放本地DTMF音
void CVoiceManager::PlayLocalDTMF(int iKey, bool bPlayNow/* = false*/)
{
    // 如果关闭按键音，则不播放
    if (PHONE_KEY_TOUCH == iKey
            && configParser_GetConfigInt(kszTouchSound) == 0
            || PHONE_KEY_TOUCH != iKey
            && configParser_GetConfigInt(kszKeypressSound) == 0)
    {
        return;
    }
    using namespace yl;
    int iOnTime = max(configParser_GetConfigInt(kszToneDtmfOntime), LOCALDTMF_ONTIME_DEF);
    int iOffTime = max(configParser_GetConfigInt(kszToneDtmfOftime), LOCALDTMF_OFFTIME_DEF);

    //默认值时，保持与v80 体验不变，不需要队列
    bool bDefValue = (iOnTime == LOCALDTMF_ONTIME_DEF) && (iOffTime == LOCALDTMF_OFFTIME_DEF);

    if (bDefValue || bPlayNow)
    {
        _VPMMSGHandler.PlayLocalDTMF(iKey);
    }
    else
    {
        VOICE_INFO("CVoiceManager::PlayLocalDTMF[%d] ,list size[%d] m_iLocalDtmfOffTimer[%d]", iKey,
                   m_listDtmfKey.size(), m_iLocalDtmfOffTimer);
        // 队列为空或者已经在播放中，需要重新设置定时器
        int iTime = 10;
        if (m_listDtmfKey.size() > 0 || m_iLocalDtmfOffTimer > 0)
        {
            //  iTime = iOnTime;
            m_listDtmfKey.push_back(iKey);
        }
        else
        {
            _VPMMSGHandler.PlayLocalDTMF(iKey);
            iTime = iOffTime;
            m_iLocalDtmfOffTimer = iTime;
            timerSetThreadTimer((UINT) &m_listDtmfKey, iTime);
        }
    }
}

bool CVoiceManager::IsRingHeadset()
{
    RING_DEVICE eRingDevice = (RING_DEVICE)configParser_GetConfigInt(kszRingerDevice);
    return ((eRingDevice == RD_HEADSET
             || eRingDevice == RD_HEADSET_GROUP)
            && m_objDevices.GetHeadsetStatus());
}

//  重置声音通道
void CVoiceManager::ResetChannel()
{
    ledlamp_PostEvent(LAMP_EVENTTYPE_SPEAKER, false);
    ledlamp_PostEvent(LAMP_EVENTTYPE_HEADSET, false);

    //图标状态
    idleScreen_DropStatus(PS_STATE_HEADSET);

    _VPMMSGHandler.SetRealChannel(RC_INIT);
    VoiceSessionPtr pSession = GetActiveSession().lock();
    if (!pSession.IsEmpty())
    {
        pSession->SwitchChannel(CHANNEL_IDLE);
    }
}

void CVoiceManager::ResetChannelParam(bool bEnable)
{
    if (bEnable == m_bHeadSetEnable)
    {
        m_objDevices.SetHeadsetStatus(bEnable);
    }
    // 只开免提端口的场景下，切换到idle要去关免提
    m_objDevices.SetHandfreeStatus(bEnable);
}

void CVoiceManager::ResetChannelStatus(bool bEnable)
{
    //耳麦状态
    if (bEnable)
    {
        idleScreen_SetStatus(PS_STATE_HEADSET);
    }
    else
    {
        idleScreen_DropStatus(PS_STATE_HEADSET);
    }

    // 清空灯、免提等状态
    ledlamp_PostEvent(LAMP_EVENTTYPE_SPEAKER, bEnable);
    ledlamp_PostEvent(LAMP_EVENTTYPE_HEADSET, bEnable);
}

void CVoiceManager::UpdateAccessStatus()
{
    //重新计算权限
    m_bHandFreeEnable = configParser_GetConfigInt(kszHandFreePortEnable) == 1;
    m_bHeadSetEnable = configParser_GetConfigInt(kszHeadsetPortEnable) == 1;
    m_bHandSetEnable = configParser_GetConfigInt(kszHandsetPortEnable) == 1;
}

#ifdef IF_SUPPORT_BLUETOOTH
// 是否蓝牙模式
bool CVoiceManager::IsBluetoothMode()
{
    return HEADSET_BT == m_objDevices.GetHeadsetDeviceType() && RC_HEADSET == _VPMMSGHandler.GetRealChannel();
}
#endif //IF_SUPPORT_BLUETOOTH

#if defined(IF_SUPPORT_BLUETOOTH) || defined(IF_SUPPORT_USB_AUDIO)
// 设置耳机连接状态
void CVoiceManager::SetHeadsetDevice(HEADSET_TYPE eType, bool bConnect)
{
    if (!IsChannelFobidden(RC_HEADSET))
    {
        m_objDevices.SetHeadsetDevice(eType, bConnect);
    }
}

// 是否是耳机模式
bool CVoiceManager::IsHeadsetDeviceMode()
{
    return (HEADSET_NORMAL != m_objDevices.GetHeadsetDeviceType() && RC_HEADSET == _VPMMSGHandler.GetRealChannel());
}

// 设置耳机通道
bool CVoiceManager::SetHeadsetDeviceChannel(bool bEnable)
{
    bool bSwitch = bEnable ? true : (HEADSET_NORMAL != m_objDevices.GetHeadsetDeviceType() && m_objDevices.GetHeadsetStatus());
    VoiceSessionPtr pSession = GetActiveSession().lock();
    VOICE_INFO("CVoiceManager::SetHeadsetDeviceChannel type[%d], enable?[%d], switch?[%d] session[%p]",
               m_objDevices.GetHeadsetDeviceType(), bEnable, bSwitch, pSession.Get());
    if (pSession.IsEmpty())
    {
        return false;
    }
    CHANNEL_MODE eOldMode = pSession->GetCurrentChannel();

    /*if (蓝牙切换连接状态 TODO)
    {
    pop/push device?
    }*/

    if (bSwitch)
    {
        if (RC_HEADSET == _VPMMSGHandler.GetRealChannel())
        {
            //先设置下成别的通道
            pSession->SetCurrentChannel(CHANNEL_NULL, false);
            _VPMMSGHandler.SetRealChannel(RC_INIT);
        }

        if (HEADSET_BT == m_objDevices.GetHeadsetDeviceType() && bEnable)
        {
            pSession->SwitchChannel(CHANNEL_BTHEADSET);
        }
        else
        {
            pSession->SwitchChannel(CHANNEL_HEADSET);
        }
    }

    //关闭蓝牙通道时，不应该去改变通道
    if (!bEnable)
    {
        pSession->SetCurrentChannel(eOldMode, false);
    }

    return bSwitch;
}

// 获取当前正在使用耳机类型
HEADSET_TYPE CVoiceManager::GetHeadsetChannelAvailableType(bool bAvalibaleInTalk/* = true*/)
{
    if (talklogic_SessionExist() || !bAvalibaleInTalk
#ifdef IF_SUPPORT_USB_AUDIO
            || modRecord_IsPlaying() || modRecord_IsRecording()
#endif
       )
    {
        return m_objDevices.GetHeadsetDeviceType();
    }

    return HEADSET_NORMAL;
}
#endif // #if defined(IF_SUPPORT_BLUETOOTH) || defined(IF_SUPPORT_USB_AUDIO)


void CVoiceManager::PlayA2dp(bool bPlay)
{
    _VPMMSGHandler.PlayA2dp(bPlay);
}

#ifdef IF_SUPPORT_LINE_DEVICE
bool CVoiceManager::SetLineDeviceChannel(int iSlaveMode)
{
    return (0 == ipvp_set_slave(mkGetHandle(), IPVP_SLAVE_LINE, iSlaveMode, 0));
}
#endif // IF_SUPPORT_LINE_DEVICE

// 注册/注销回调函数
void CVoiceManager::InitUpdateChanelInfo(FunUpdateChannelInfo pCallBack, bool bRegister)
{
    if (NULL == pCallBack)
    {
        return ;
    }

    if (bRegister)
    {
        m_funUpdateChannelInfo = pCallBack;
    }
    else if (m_funUpdateChannelInfo == pCallBack)
    {
        m_funUpdateChannelInfo = NULL;
    }
}

// 获取通道处理类
CBaseChannelMode * CVoiceManager::GetChannelModePtr(CHANNEL_MODE eMode, bool bEHS/* = false*/,
        bool bForceOpenGroup/* = false*/)
{
    // 耳麦与Group 创建需要参数
    CBaseInitParam * pInitParam;

    if (CHANNEL_HEADSET == eMode
            || CHANNEL_2HEADSET == eMode
            || CHANNEL_BTHEADSET == eMode)
    {
        pInitParam = new CHeadsetInitParam(eMode, bEHS);
    }
    else if (CHANNEL_GROUP == eMode)
    {
        pInitParam = new CGroupChannelInitParam(eMode, bForceOpenGroup);
    }
    else
    {
        pInitParam = new CBaseInitParam(eMode);
    }

    if (CHANNEL_2HEADSET == eMode
            || CHANNEL_HEADSET == eMode
            || CHANNEL_BTHEADSET == eMode)
    {
        return new CHeadSetChannelMode(pInitParam);
    }
    else if (CHANNEL_GROUP == eMode)
    {
        return new CGroupChannelMode(pInitParam);
    }
    else if (CHANNEL_IDLE == eMode)
    {
        return new CIdleChannelMode(pInitParam);
    }
    else if (CHANNEL_RING == eMode)
    {
        return new CRingChannelMode(pInitParam);
    }
    else if (CHANNEL_SELECT == eMode
             || CHANNEL_TONE_IDLE == eMode)
    {
        return new CSelectChannelMode(pInitParam);
    }
    else if (CHANNEL_HANDFREE == eMode
             || CHANNEL_MIC_ONLY == eMode
             || CHANNEL_SPEAKER_ONLY == eMode
             || CHANNEL_HANDSET == eMode
             || CHANNEL_NULL == eMode)
    {
        return new CBaseChannelMode(pInitParam);
    }
    else if (CHANNEL_AUTO == eMode)
    {
        return new CAutoChannelMode(pInitParam);
    }

    return NULL;
}

// 通道权限
bool CVoiceManager::IsChannelFobidden(REAL_CHANNEL eChannel)
{
    bool bForbidden = false;

    // 免提通道权限
    if (RC_HANDFREE == eChannel
            || RC_SPEAKER_ONLY == eChannel
            || RC_MIC_ONLY == eChannel
            || RC_GROUP_HANDSET == eChannel
            || RC_GROUP_HEADSET == eChannel)
    {
        bForbidden = !m_bHandFreeEnable;
    }
    else if (RC_2HEADSET == eChannel
             || RC_HEADSET == eChannel
             || RC_GROUP_HEADSET == eChannel)
    {
        //耳麦通道权限
        bForbidden = !m_bHeadSetEnable;
    }
    else if (RC_HANDSET == eChannel
             || RC_GROUP_HANDSET == eChannel)
    {
        //手柄通道权限
        bForbidden = !m_bHandSetEnable;
    }

    VOICE_INFO("realchannel [%d] is %s forbidden", eChannel, bForbidden ? "" : "not");

    return bForbidden;
}

void CVoiceManager::PauseActiveSession()
{
    VoiceSessionPtr pSession = GetActiveSession().lock();
    if (!pSession.IsEmpty())
    {
        // 顶部Session挂起
        pSession->Pause();
    }
}

void CVoiceManager::ResumeActiveSession()
{
    VoiceSessionPtr pSession = GetActiveSession().lock();
    if (!pSession.IsEmpty())
    {
        // 顶部Session恢复
        pSession->Resume();
//         // 回到idle时需要关闭免提
//         if (pSession.Get() == m_pIdleSession.Get())
//         {
//             VOICE_INFO("Return idle voice session");
//             SetHandfreeStatus(false);
//         }
    }
}

bool CVoiceManager::OnTimer(msgObject & objMsg)
{
    UINT uTimerID = objMsg.wParam;
    bool bHandled = false;
    if (uTimerID == (UINT)&m_listDtmfKey)
    {
        bHandled = true;
        timerKillThreadTimer(uTimerID);

        if (m_listDtmfKey.size() <= 0)
        {
            m_iLocalDtmfOffTimer = 0;
            return bHandled;
        }
        using namespace yl;
        // 要加上播放时长
        int iOnTime = max(configParser_GetConfigInt(kszToneDtmfOntime), LOCALDTMF_ONTIME_DEF);
        int iOffTime = max(configParser_GetConfigInt(kszToneDtmfOftime), LOCALDTMF_OFFTIME_DEF);
        m_iLocalDtmfOffTimer = iOnTime + iOffTime;

        _VPMMSGHandler.PlayLocalDTMF(*m_listDtmfKey.begin());
        m_listDtmfKey.erase(m_listDtmfKey.begin());

        VOICE_INFO("CVoiceManager::OnTimer iOffTime[%d]", m_iLocalDtmfOffTimer);
        timerSetThreadTimer((UINT)&m_listDtmfKey, m_iLocalDtmfOffTimer);
    }
    else
    {
        bHandled = m_objDevices.EHSOnTimer(uTimerID);
    }
    return bHandled;
}

bool CVoiceManager::OnSelectChannel(msgObject & objMsg)
{
    VoiceSessionPtr pSession = GetActiveSession().lock();
    if (!pSession.IsEmpty())
    {
        pSession->SwitchChannel(CHANNEL_AUTO);
    }
    return true;
}

bool CVoiceManager::OnVPMRegistMsg(msgObject & objMsg)
{
    if (MKIT_MSG_SRVREG == objMsg.message && MKSRV_ID_IPVP == objMsg.wParam)
    {
        // vpm启动起来
        InitVoice();
        return true;
    }
    return false;
}

bool CVoiceManager::OnConfigChange(msgObject & objMsg)
{
    if (CONFIG_MSG_BCAST_CHANGED == objMsg.message && ST_CHANNEL_CONFIG == objMsg.wParam)
    {
        UpdateAccessStatus();
        //自动切一下通道
        VoiceSessionPtr pSession = GetActiveSession().lock();
        if (!pSession.IsEmpty())
        {
            pSession->SwitchChannel();
        }
        return true;
    }
    return false;
}
