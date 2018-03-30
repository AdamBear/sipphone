#ifdef IF_SUPPORT_USB_AUDIO
#include "usbheadset.h"
#include "usbheadset_api.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "extended/usbmessagedefine.h"
#include "voice/include/modvoice.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "commonunits/modcommonunits.h"
#include "talk/talklogic/include/talkmsgdefine.h"
#include "contacts/directory/include/moddirinterface.h"
//#include "adapterbox/include/modadapterbox.h"
#include "configparser/modconfigparser.h"
#include "configiddefine.h"
#include "extended/event/include/modevent.h"
#include "exp/include/exp_struct.h"
#include "taskaction/modtaskaction.h"

#define INVALID_TIMER_ID -1
#define SYNC_USBSTATE_TIMER 500

IMPLEMENT_GETINSTANCE(CUSBHeadset)

const char * strHidInput(usbhid_input_t value)
{

    switch (value)
    {
    case Undefined:
        return "Undefined";
    case OffHook:
        return "OffHook";
    case Mute:
        return "Mute";
    case Flash:
        return "Flash";
    case Redial:
        return "Redial";
    case Reject:
        return "Reject";
    case Hold:
        return "Hold";
    case Line:
        return "Line";
    case SpeedDial:
        return "SpeedDial";
    case LedRinger:
        return "LedRinger";
    case LineBusyTone:
        return "LineBusyTone";
    case Key0:
        return "Key0";
    case Key1:
        return "Key1";
    case Key2:
        return "Key2";
    case Key3:
        return "Key3";
    case Key4:
        return "Key4";
    case Key5:
        return "Key5";
    case Key6:
        return "Key6";
    case Key7:
        return "Key7";
    case Key8:
        return "Key8";
    case Key9:
        return "Key9";
    case KeyStar:
        return "KeyStar";
    case KeyPound:
        return "KeyPound";
    case KeyVolumeUp :
        return "KeyVolumeUp";
    case KeyVolumeDown:
        return "KeyVolumeDown";
    default:
        return "";
    }

    return "";
}


CUSBHeadset::CUSBHeadset()
{
}

CUSBHeadset::~CUSBHeadset()
{
    commonUnits_RemoveSysLogFunc(CUSBHeadset::SetUsbHeadsetLog);
}

LRESULT usbheadset_msg(msgObject & msg)
{
    switch (msg.message)
    {
    case USB_MSG_HEADSET_PLUG:
        _USBHeadset.Plug(msg.wParam == 1);
        break;
    case USB_MSG_HEADSET_KEY:
        {
            if (NULL == msg.GetExtraData())
            {
                break;
            }

            UsbHeadsetKeyInfo * pKeyInfo = (UsbHeadsetKeyInfo *)msg.GetExtraData();
            _USBHeadset.ProcessKey(pKeyInfo->iKey, pKeyInfo->iButtonData, pKeyInfo->bRelative);
        }
        break;
    case VOICE_CHANGE_CHANNEL:
        {
            if (CHANNEL_HEADSET == msg.lParam
                    || CHANNEL_HEADSET == msg.wParam)
            {
                _USBHeadset.SyncState(true);
            }
            else if (CHANNEL_RING == msg.lParam)
            {
                _USBHeadset.SetRingState(false);
            }
        }
        break;
    case TM_TIMER:
        {
            _USBHeadset.OnTimer(msg.wParam);
        }
        break;
    case USB_MSG_HEADSET_ACTION_RESULT:
        {
            _USBHeadset.ProcessActionResult((USBHEADSET_CTRL_TYPE)msg.wParam, msg.lParam);
        }
        break;
    default:
        break;
    }

    return 1;
}

// 初始化
bool CUSBHeadset::Init()
{
    if (configParser_GetConfigInt(kszUsbPortEnable) == 0)
    {
        USBDEVICE_INFO("Usb headset init fail, usb port disabled");
        return false;
    }

    USBDEVICE_INFO("usb headset device init");
    SingleMsgReg(USB_MSG_HEADSET_PLUG, usbheadset_msg);
    SingleMsgReg(USB_MSG_HEADSET_KEY, usbheadset_msg);
    SingleMsgReg(VOICE_CHANGE_CHANNEL, usbheadset_msg);
    SingleMsgReg(TM_TIMER, usbheadset_msg);
    SingleMsgReg(USB_MSG_HEADSET_ACTION_RESULT, usbheadset_msg);

    TaskAction_CreateSpecialThread(NS_TA::TA_REQUEST_USBHEADSET_CTRL,
                                   TA_THREAD_USB_HEADSET,
                                   CUsbHeadsetAction::OnMessage);
    AddCmd(USBHEADSET_INIT, 0);

    m_iTimerID = INVALID_TIMER_ID;
    m_iHoldState = 0;
    m_iHookState = 0;
    m_iRingerState = 0;
    m_iMuteState = 0;
    m_bForceSync = false;
    m_strDialNum = "";
    m_bUSBPlugIn = false;

    commonUnits_AddSysLogFunc(CUSBHeadset::SetUsbHeadsetLog);

    return true;
}

// 同步状态到耳机
bool CUSBHeadset::SyncState(bool bForce/* = false*/)
{
    if (!m_bUSBPlugIn && !bForce)
    {
        return false;
    }

    SetForceSync(bForce);
    SetSyncTimer(true);

    return true;
}

// 定时器处理
bool CUSBHeadset::OnTimer(UINT uTimer)
{
    if (uTimer == (UINT)&m_iTimerID)
    {
        SetSyncTimer(false);

        if (GetForceSync()
                || (IsUSBHeadsetAvaliable()
                    && configParser_GetConfigInt(kszHeadsetPortEnable)))
        {
            SetForceSync(false);
            SyncDeviceState();
        }

        return true;
    }

    return false;
}

// 定时器到,同步usb设备状态
bool CUSBHeadset::SyncDeviceState(bool bSyncHook/* = true*/, bool bSyncMute/* = true*/,
                                  bool bSyncHold/* = true*/, bool bSyncRing/* = true*/)
{
    USBDEVICE_INFO("Start compare state usb headset mode[%d].", voice_IsUSBHeadsetMode());

    int iRing = m_iRingerState;
    int iOffHook = m_iHookState;
    int iMute = m_iMuteState;
    int iHold = m_iHoldState;

    // 耳机是否能够响铃需要受配置(kszRingDevice)控制
    m_iRingerState = (talklogic_IsFoucsInRingSession() ? 1 : 0)
#if 0
                     && !voice_IsTheMinVolume()
#endif
                     && ((RD_HEADSET == (RING_DEVICE)configParser_GetConfigInt(kszRingerDevice))
                         || (RD_HEADSET_GROUP == (RING_DEVICE)configParser_GetConfigInt(kszRingerDevice)));

    if (!voice_IsUSBHeadsetMode())
    {
        // USB耳麦移除, 响铃状态也需重置
        // http://bugfree.yealink.com/Bug.php?BugID=136581
        if (!m_bUSBPlugIn)
        {
            m_iRingerState = 0;
        }
        m_iHookState = 0;
        m_iMuteState = 0;
        m_iHoldState = 0;
    }
    else
    {
        m_iHookState = !m_iRingerState && talklogic_SessionExist() ? 1 : 0;
        m_iMuteState = !m_iRingerState && talklogic_GetMuteStatus() ? 1 : 0;
        TALK_STATE eState = talklogic_GetFocusedSessionTalkState();
        m_iHoldState = !m_iRingerState && (eState == TS_HOLD || eState == TS_HOLD_HELD) ? 1 : 0;
    }

    bool bSet = false;
    if (iRing != m_iRingerState && bSyncRing)
    {
        bSet |= AddCmd(USBHEADSET_RING, m_iRingerState, false);
    }

    if (iOffHook != m_iHookState && bSyncHook)
    {
        bSet |= AddCmd(USBHEADSET_HOOK, m_iHookState, false);
    }

    if (iMute != m_iMuteState && bSyncMute)
    {
        bSet |= AddCmd(USBHEADSET_MUTE, m_iMuteState, false);
    }

    if (iHold != m_iHoldState && bSyncHold)
    {
        //if (m_iHoldState)
        //{
        //  m_iHookState = 0;
        //}
        //else
        //{
        //  m_iHookState = 1;
        //}
        //bSet |= AddCmd(USBHEADSET_HOOK, m_iHookState, false);
        //bSet |= AddCmd(USBHEADSET_HOLD, m_iHoldState, false);
    }

    if (bSet)
    {
        USBDEVICE_INFO("USB headset sync state, ring[%d],hook[%d],mute[%d],hold[%d]", m_iRingerState,
                       m_iHookState, m_iMuteState, m_iHoldState);
        RunCmd();
    }

    return true;
}

// usb headset key
bool CUSBHeadset::ProcessKey(int iKey, bool bPress, bool bRelative)
{
    if (configParser_GetConfigInt(kszHeadsetPortEnable) == 0
            /*|| !voice_IsUSBHeadsetMode()*/)
    {
        etl_NotifyApp(FALSE, VOICE_CHANNEL_FORBDDEN, CHANNEL_HEADSET, 0);

        USBDEVICE_INFO("Receive USB headset key. headset port enable[%d]",
                       configParser_GetConfigInt(kszHeadsetPortEnable) == 0);
        return false;
    }

    usbhid_input_t usbKey = (usbhid_input_t)iKey;
    USBDEVICE_INFO("Receive USB headset key[%s], press[%d], relative=[%d]", strHidInput(usbKey), bPress,
                   bRelative);
    switch (usbKey)
    {
    case OffHook:
        {
            if (bPress == m_iHookState)
            {
                AddCmd(USBHEADSET_HOOK, bPress);
                break;
            }

            // 1. hook是否为接起来电,是则停止响铃
            if (bPress)
            {
                if (m_iRingerState)
                {
                    AddCmd(USBHEADSET_RING, 0, false);
                    m_iRingerState = 0;
                }
            }

            m_iHookState = bPress;
            // 如果是要挂断但还有通话,则要保持在offhook
            if (0 == bPress
                    && talklogic_GetSessionNum() > 1)
            {
                // http://10.2.1.199/Bug.php?BugID=99134
                // 先屏蔽,jabra其他耳机需要该处理
                //AddCmd(USBHEADSET_HOOK, 0, false);
                //m_iHookState = 0;
                AddCmd(USBHEADSET_HOOK, 0, false);
                m_iHookState = 1;
            }

            // 2. 模拟EHS流程,切换声音通道
            bool bUsbChannelWillClose = false;
            ProcessHooKey(bUsbChannelWillClose);
            // 声道如果即将从USB耳麦切走则不再设置为offhook 1
            // http://bugfree.yealink.com/Bug.php?BugID=133939
            if (bUsbChannelWillClose)
            {
                m_iHookState = 0;
            }

            // 3. hook是否为携带号码进行呼出
            // 流程上会先收到digit key,然后再收到offhook key
            if (!m_strDialNum.empty() && bPress)
            {
                CallOut_Param callParam;
                talklogic_CallOut(m_strDialNum, "", AUTO_SELECT_LINE_ID, &callParam);
                m_strDialNum = "";
            }

            AddCmd(USBHEADSET_HOOK, m_iHookState);
        }
        break;
    case Mute:
        {
            if (bRelative
                    && !bPress)
            {
                break;
            }

            if (!bRelative
                    && bPress == m_iMuteState)
            {
                AddCmd(USBHEADSET_MUTE, m_iMuteState);
                break;
            }

            if (voice_IsUSBHeadsetMode())
            {
                m_iMuteState = !m_iMuteState;
                commonUnits_SendSimulateKey(DEV_MSG_FUNKEY_EVENT, PHONE_KEY_MUTE);
            }

            AddCmd(USBHEADSET_MUTE, m_iMuteState);
        }
        break;
    case Flash:
        {
            // 该事件待分析:话路切换、hold等都可能上报该消息
        }
        break;
    case Reject:
        {
            if (bPress)
            {
                if (talklogic_IsFoucsInRingSession())
                {
                    m_iRingerState = 0;
                    AddCmd(USBHEADSET_RING, m_iRingerState, false);
                    AddCmd(USBHEADSET_HOOK, m_iHookState);
                    talklogic_ExitFocusSession();
                }
            }
        }
        break;
    case Redial:
        {
            if (bPress)
            {
                yl::string strNum;
                int iAccountID = 0;
                LocalLog_GetLastOutCallNumAndAccountID(strNum, iAccountID);
                talklogic_CallOut(strNum, "", iAccountID);

                m_iHookState = 1;
                AddCmd(USBHEADSET_HOOK, m_iHookState);
                commonUnits_SendSimulateKey(DEV_MSG_FUNKEY_EVENT, PHONE_KEY_EHS_OPEN);
            }
        }
        break;
    case Hold:
        {
            if (bPress)
            {
                TALK_STATE eState = talklogic_GetFocusedSessionTalkState();
                if (TS_TALK == eState)
                {
                    m_iHoldState = 0;
                }
                else if (TS_HOLD == eState || TS_HOLD_HELD == eState)
                {
                    m_iHoldState = 1;
                }

                commonUnits_SendSimulateKey(DEV_MSG_FUNKEY_EVENT, PHONE_KEY_HOLD);
                AddCmd(USBHEADSET_HOLD, m_iHoldState);
            }
        }
        break;
    case Key0:
    case Key1:
    case Key2:
    case Key3:
    case Key4:
    case Key5:
    case Key6:
    case Key7:
    case Key8:
    case Key9:
    case KeyStar:
    case KeyPound:
        {
            if (!bPress)
            {
                break;
            }

            // 通话不存在,保存号码等待呼出
            if (!talklogic_SessionExist())
            {
                if (usbKey >= Key0
                        && usbKey <= Key9)
                {
                    m_strDialNum += commonAPI_FormatString("%d", usbKey - Key0);
                }
                else if (KeyStar == usbKey)
                {
                    m_strDialNum += '*';
                }
                else if (KeyPound == usbKey)
                {
                    m_strDialNum += '#';
                }
            }
            // 通话中发送dtmf
            else if (talklogic_IsFocusInTalkSession())
            {
                if (usbKey >= Key0
                        && usbKey <= Key9)
                {
                    commonUnits_SendSimulateKey(DEV_MSG_FUNKEY_EVENT, usbKey - Key0 + PHONE_KEY_0);
                }
                else if (KeyStar == usbKey)
                {
                    commonUnits_SendSimulateKey(DEV_MSG_FUNKEY_EVENT, PHONE_KEY_STAR);
                }
                else if (KeyPound == usbKey)
                {
                    commonUnits_SendSimulateKey(DEV_MSG_FUNKEY_EVENT, PHONE_KEY_POUND);
                }
            }
        }
        break;
    case KeyVolumeDown:
        if (bPress)
        {
            AddCmd(USBHEADSET_VOLUME_DEC, 0);
        }
        break;
    case KeyVolumeUp:
        if (bPress)
        {
            AddCmd(USBHEADSET_VOLUME_INC, 0);
        }
        break;
    default:
        break;
    }

    return true;
}

// 处理hook按键
bool CUSBHeadset::ProcessHooKey(bool & bUsbChannelClose)
{
    // 模拟EHS的处理
    // 1. 存在话路且是响铃中, 如果实际物理通道是耳麦, 不管声道在哪, 都发送EHS OPEN接起来电
    // 2. 存在话路且是预拨号, 如果实际物理通道是耳麦，则发送EHS关闭
    // 3. 存在话路, 除了预拨号和响铃, 都按照声道切换来处理: 手柄摘起则切换声道至手柄, 否则发送EHS关闭
    // 4. IDLE下都发打开按键, 这样的效果是在idle下会将通道切换至耳麦，并进入拨号界面

    USBDEVICE_INFO("CUSBHeadset::ProcessHooKey voice_IsUSBHeadsetMode[%d]", voice_IsUSBHeadsetMode());

    int iKeyCode = -1;
    if (talklogic_SessionExist()
            && voice_IsUSBHeadsetMode())
    {
        if (talklogic_IsFoucsInRingSession())
        {
            iKeyCode = PHONE_KEY_EHS_OPEN;
        }
        else if (talklogic_IsFocusInPreDialSession())
        {
            iKeyCode = PHONE_KEY_EHS_CLOSE;
        }
        else
        {
            if (voice_GetHandsetStatus())
            {
                voice_SwitchChannel(CHANNEL_HANDSET);
                bUsbChannelClose = true;
            }
            else
            {
                iKeyCode = PHONE_KEY_EHS_CLOSE;
            }
        }
    }
    else
    {
        iKeyCode = PHONE_KEY_EHS_OPEN;
    }

    if (iKeyCode != -1)
    {
        commonUnits_SendSimulateKey(DEV_MSG_FUNKEY_EVENT, iKeyCode);
    }

    bUsbChannelClose |= iKeyCode == PHONE_KEY_EHS_CLOSE;

    return true;
}

// usb headset插入拔出
bool CUSBHeadset::Plug(bool bIn)
{
    USBDEVICE_INFO("usb headset plug %s", bIn ? "in" : "out");
    AddCmd(bIn ? USBHEADSET_PLUGIN : USBHEADSET_PLUGOUT, 0);
    return true;
}

bool CUSBHeadset::AddCmd(USBHEADSET_CTRL_TYPE eType, int iValue, bool bRun/* = true*/)
{
    UsbHeadsetCtrl headsetCmd;
    headsetCmd.eCmd = eType;
    headsetCmd.iValue = iValue;
    msgPostMsgToThreadEx(msgGetThreadByName(VP_USB_THREAD_NAME),
                         USB_MSG_HEADSET_CMD, 0, 0, sizeof(UsbHeadsetCtrl), &headsetCmd);

    if (bRun)
    {
        RunCmd();
    }

    return true;
}

// 添加命令进行异步线程执行
bool CUSBHeadset::RunCmd()
{
    msgPostMsgToThread(msgGetThreadByName(VP_USB_THREAD_NAME), USB_MSG_HEADSET_ACTION, 0, 0);
    return true;
}

bool CUSBHeadset::SetSyncTimer(bool bEnable)
{
    if (bEnable && m_iTimerID != INVALID_TIMER_ID)
    {
        // 已经在运行了，不重复设置
        return true;
    }

    USBDEVICE_INFO("Set sync timer [%s]", bEnable ? "ebable" : "disable");

    if (bEnable)
    {
        m_iTimerID = SYNC_USBSTATE_TIMER;
        timerSetThreadTimer((UINT)&m_iTimerID, m_iTimerID);
    }
    else
    {
        timerKillThreadTimer((UINT)&m_iTimerID);
        m_iTimerID = INVALID_TIMER_ID;
    }

    return true;
}

void CUSBHeadset::ProcessActionResult(USBHEADSET_CTRL_TYPE eActionType, bool bResult)
{
    if (eActionType == USBHEADSET_PLUGIN)
    {
        m_bUSBPlugIn = bResult;

        event_SendConnectedDeviceEvent(EVENT_PERIPHERAL_INFORMATION);

        etl_NotifyApp(false, USB_MSG_HEADSET_CONNECT_STATE, 1, 0);

        // 将话机的状态同步给耳机
        SyncState();

        // 通话中插入USB耳机\当前是耳麦已开启
        if (bResult && talklogic_SessionExist())
        {
            voice_OpenHeadsetDeviceChannel();
        }

    }
    else if (eActionType == USBHEADSET_PLUGOUT)
    {
        m_bUSBPlugIn = false;
        event_SendConnectedDeviceEvent(EVENT_PERIPHERAL_INFORMATION);

        bool bIsUsbHeadsetMode = voice_IsUSBHeadsetMode();

        etl_NotifyApp(false, USB_MSG_HEADSET_CONNECT_STATE, 0, 0);
        if (!bIsUsbHeadsetMode)
        {
            return;
        }

        // 通话中拔掉USB耳机,声道切换:手柄-->免提
        if (talklogic_SessionExist())
        {
            if (voice_GetHandsetStatus())
            {
                voice_SwitchChannel(CHANNEL_HANDSET);
            }
            else
            {
                voice_SwitchChannel(CHANNEL_HANDFREE);
            }
        }
        else  //重置通道，切换到IDLE
        {
            //要先设置下当前通道，避免内部处理判断相同不去切换通道
            voice_SetCurrentChannel(CHANNEL_NULL);
            voice_ResetChannel();
        }
    }
}

// 设置响铃状态
void CUSBHeadset::SetRingState(int iPlay)
{
    // 1. RING_DEVICE为headset 2. 静音播放铃声时 ---->都不需要播放耳机铃声
    if (m_iRingerState == iPlay
            || RD_DEFAULT == (RING_DEVICE)configParser_GetConfigInt(kszRingerDevice)
#if 0
            || (iPlay && voice_IsTheMinVolume())
#endif
       )
    {
        return ;
    }
    m_iRingerState = iPlay;

    AddCmd(USBHEADSET_RING, m_iRingerState);
}

void CUSBHeadset::SetUsbHeadsetLog(int iModuleID, int iLogLevel)
{
    usb_headset_log_set(iLogLevel);
    USBDEVICE_INFO("Set usb headset log, level:%d", iLogLevel);
}

// usb 耳机是否可用
bool CUSBHeadset::IsUSBHeadsetAvaliable()
{
    return (voice_IsUSBHeadsetMode()
            || voice_IsUSBHeadsetChannelAvaliable());
}

#endif // IF_SUPPORT_USB_AUDIO
