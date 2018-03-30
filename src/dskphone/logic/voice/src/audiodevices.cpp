#include "audiodevices.h"
#include "voicemanager.h"

#include <devicelib/keypad_drv.h>
#include <etlmsghandler/modetlmsghandler.h>
#include <configparser/modconfigparser.h>
#include <configiddefine.h>

#include "exp/include/exp_struct.h"
#include "extended/bluetoothmessagedefine.h"
#include "extended/usbmessagedefine.h"
#include "commonunits/modcommonunits.h"
#include "idlescreen/include/modidlescreen.h"
#include "lamp/ledlamp/include/modledlamp.h"
#ifdef BLUETOOTH_USED_OLD_VERSION
#include "wireless/bluetoothOld/include/btdefines.h"
#else
#include "wireless/bluetoothNew/include/btdefines.h"
#endif // IF_OLD_BLUETOOTH


CAudioDevices::CAudioDevices()
    : m_bOffHook(IsHookOffByDriver())
    , m_bHeadset(false)
    , m_bHandfree(false)
    , m_pEHS(NULL)
{
    // EHS消息
    SingleMsgReg(DEV_MSG_EHS_INSERT, CMSGSubscriber(this, &CAudioDevices::OnEHSMessage));
    SingleMsgReg(DEV_MSG_EHS_REMOVE, CMSGSubscriber(this, &CAudioDevices::OnEHSMessage));
    SingleMsgReg(DEV_MSG_EHS_KEY, CMSGSubscriber(this, &CAudioDevices::OnEHSMessage));
#ifdef IF_SUPPORT_BLUETOOTH
    // 蓝牙耳机消息
    SingleMsgReg(BLUETOOTH_MSG_CONNECT_STATE, CMSGSubscriber(this, &CAudioDevices::OnBTHeadsetMessage));
#endif // IF_SUPPORT_BLUETOOTH
#ifdef IF_SUPPORT_USB_AUDIO
    // USB耳机消息
    SingleMsgReg(USB_MSG_HEADSET_CONNECT_STATE, CMSGSubscriber(this,
                 &CAudioDevices::OnUSBHeadsetMessage));
#endif // IF_SUPPORT_USB_AUDIO
}

CAudioDevices::~CAudioDevices()
{
    SingleMsgUnReg(DEV_MSG_EHS_INSERT, CMSGSubscriber(this, &CAudioDevices::OnEHSMessage));
    SingleMsgUnReg(DEV_MSG_EHS_REMOVE, CMSGSubscriber(this, &CAudioDevices::OnEHSMessage));
    SingleMsgUnReg(DEV_MSG_EHS_KEY, CMSGSubscriber(this, &CAudioDevices::OnEHSMessage));
#ifdef IF_SUPPORT_BLUETOOTH
    SingleMsgUnReg(BLUETOOTH_MSG_CONNECT_STATE,
                   CMSGSubscriber(this, &CAudioDevices::OnBTHeadsetMessage));
#endif // IF_SUPPORT_BLUETOOTH
#ifdef IF_SUPPORT_USB_AUDIO
    SingleMsgUnReg(USB_MSG_HEADSET_CONNECT_STATE,
                   CMSGSubscriber(this, &CAudioDevices::OnUSBHeadsetMessage));
#endif // IF_SUPPORT_USB_AUDIO
    SAFE_DELETE(m_pEHS);
}

bool CAudioDevices::GetHandsetStatus()
{
    return m_bOffHook;
}

void CAudioDevices::SetHandsetStatus(bool bStatus)
{
    m_bOffHook = bStatus;
}

bool CAudioDevices::GetHandfreeStatus()
{
    return m_bHandfree;
}

void CAudioDevices::SetHandfreeStatus(bool bOpen)
{
    if (GetHandfreeStatus() == bOpen)
    {
        return;
    }
    VOICE_INFO("SetHandfreeStatus = %d", bOpen);
    ledlamp_PostEvent(LAMP_EVENTTYPE_SPEAKER, bOpen);
    m_bHandfree = bOpen;
}

bool CAudioDevices::GetHeadsetStatus()
{
    return m_bHeadset;
}

void CAudioDevices::SetHeadsetStatus(bool bStatus)
{
    if (GetHeadsetStatus() == bStatus)
    {
        return;
    }
    VOICE_INFO("SetHeadsetStatus = %d", bStatus);
    // 切换通道灭耳麦灯
    ledlamp_PostEvent(LAMP_EVENTTYPE_HEADSET, bStatus);
    // 切换耳机状态图标
    if (bStatus)
    {
        idleScreen_SetStatus(PS_STATE_HEADSET);
    }
    else
    {
        idleScreen_DropStatus(PS_STATE_HEADSET);
    }
    m_bHeadset = bStatus;
}

HEADSET_TYPE CAudioDevices::GetHeadsetDeviceType()
{
#if defined(IF_SUPPORT_BLUETOOTH) || defined(IF_SUPPORT_USB_AUDIO)
    if (m_listHeadsetType.size() > 0)
    {
        return m_listHeadsetType.back();
    }
#endif
    return HEADSET_NORMAL;
}

#if defined(IF_SUPPORT_BLUETOOTH) || defined(IF_SUPPORT_USB_AUDIO)
void CAudioDevices::SetHeadsetDevice(HEADSET_TYPE eType, bool bConnect)
{
    VOICE_INFO("SetHeadsetDevice type=%d, bconnect=%d,current=%d",
               eType, bConnect, GetHeadsetDeviceType());
    if (bConnect)
    {
        if (eType == GetHeadsetDeviceType())
        {
            return;
        }

        PushDevice(eType);
    }
    else
    {
        // 如果断开的设备不是正在生效的耳机,则只需弹出
        if (eType != GetHeadsetDeviceType())
        {
            PopDevice(eType);
            return;
        }

        PopDevice(eType);
    }
}

void CAudioDevices::PushDevice(HEADSET_TYPE eType)
{
    m_listHeadsetType.push_back(eType);
}

bool CAudioDevices::PopDevice(HEADSET_TYPE eType)
{
    YLList<HEADSET_TYPE>::iterator it = m_listHeadsetType.begin();
    for (; it != m_listHeadsetType.end(); ++it)
    {
        if (*it == eType)
        {
            m_listHeadsetType.erase(it);
            return true;
        }
    }

    return false;
}
#endif // defined(IF_SUPPORT_BLUETOOTH) || defined(IF_SUPPORT_USB_AUDIO)

void CAudioDevices::EHSOpen(bool bForceToClose /*= false*/)
{
    if (m_pEHS)
    {
        m_pEHS->Open(bForceToClose);
    }
}

void CAudioDevices::EHSClose(bool bForceToClose /*= false*/)
{
    if (m_pEHS)
    {
        m_pEHS->Close(bForceToClose);
    }
}

void CAudioDevices::EHSSwitchStatus()
{
    if (m_pEHS)
    {
        m_pEHS->SwitchEHSStatus();
    }
}

void CAudioDevices::EHSPlayRing()
{
    if (m_pEHS)
    {
        m_pEHS->PlayRing();
    }
}

#if IF_BUG_27190
void CAudioDevices::EHSPlayPhoneRing()
{
    if (m_pEHS)
    {
        m_pEHS->PlayPhoneRing();
    }
}
#endif

void CAudioDevices::EHSStopRing(bool bForceStopEHS /*= true*/)
{
    if (m_pEHS)
    {
        m_pEHS->StopRing(bForceStopEHS);
    }
}

int CAudioDevices::EHSGetIndex()
{
    if (m_pEHS)
    {
        return m_pEHS->GetIndex();
    }
    return -1;
}

bool CAudioDevices::EHSOnTimer(unsigned int uTimeID)
{
    if (m_pEHS)
    {
        return m_pEHS->OnTimer(uTimeID);
    }
    return false;
}

bool CAudioDevices::OnEHSMessage(msgObject & objMsg)
{
    switch (objMsg.message)
    {
    case DEV_MSG_EHS_INSERT:
        {
            // 插上EHS无线耳麦
            if (m_pEHS != NULL)
            {
                delete m_pEHS;
            }
            m_pEHS = new CEHSHeadset(objMsg.wParam);
        }
        break;
    case DEV_MSG_EHS_REMOVE:
        {
            // 拔下EHS无线耳麦
            SAFE_DELETE(m_pEHS);
        }
        break;
    case DEV_MSG_EHS_KEY:
        {
            // EHS无线耳麦按键按下
            if (m_pEHS != NULL)
            {
                m_pEHS->OnKeyPressed();
            }
        }
        break;
    default:
        break;
    }
    return true;
}

#ifdef IF_SUPPORT_BLUETOOTH
bool CAudioDevices::OnBTHeadsetMessage(msgObject & objMsg)
{
    if (!_ISBTPHONE(objMsg.lParam))
    {
        // wParam == 1 --->连接上, wParam == 0 ---> 断开
        if (1 == objMsg.wParam)
        {
            VoiceManager.SetHeadsetDevice(HEADSET_BT, true);
        }
        else if (0 == objMsg.wParam)
        {
            VoiceManager.SetHeadsetDevice(HEADSET_BT, false);
        }
    }
    return true;
}
#endif // IF_SUPPORT_BLUETOOTH

#ifdef IF_SUPPORT_USB_AUDIO
bool CAudioDevices::OnUSBHeadsetMessage(msgObject & objMsg)
{
    if (objMsg.wParam == 1)
    {
        // USB耳机连接，声道切换到USB耳麦
        VoiceManager.SetHeadsetDevice(HEADSET_USB, true);
    }
    else if (objMsg.wParam == 0)
    {
        // USB耳机断开
        VoiceManager.SetHeadsetDevice(HEADSET_USB, false);
    }
    return true;
}
#endif // IF_SUPPORT_USB_AUDIO

bool CAudioDevices::IsHookOffByDriver() const
{
    bool bHookOn = false;
#ifndef WIN32
    if (CVoiceManager::IsSupportHandset())
    {
        int fd = keypad_helper_open();
        if (fd < 0)
        {
            VOICE_ERR("Cannot open keyboard device!");
            return false;
        }
        unsigned long keyArray[KEY_MAX / 8 + 1] = { 0L };
        memset(keyArray, 0, sizeof(keyArray));
        ioctl(fd, EVIOCGKEY(sizeof(keyArray)), keyArray);
        bHookOn = ((1UL << (KEYPAD_EV_KEY_HOOK & 0x1F)) & (keyArray[KEYPAD_EV_KEY_HOOK >> 5])) == 0;
        keypad_helper_close(fd);
        VOICE_INFO("Hook is %s", bHookOn ? "on" : "off");
    }
#endif
    return bHookOn;
}
