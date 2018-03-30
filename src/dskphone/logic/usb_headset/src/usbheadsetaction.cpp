#ifdef IF_SUPPORT_USB_AUDIO
#include "usbheadsetaction.h"
#include "usbheadset_api.h"
#include "extended/usbmessagedefine.h"
#include "usbheadset.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "configparser/modconfigparser.h"
#include "configiddefine.h"
#include "commonunits/modcommonunits.h"

usbheadset_device_info_t * g_pDevice_info_g = NULL;
usbhid_handle_t g_hHandle = NULL;

// usb headset设备按键的回调函数
void usbheadset_api_init();
void usbheadset_api_uninit();
void usbhid_button_in_data_raw(unsigned short usagePage, unsigned short usage, int buttonInData,
                               bool relative);
void usbhid_button_in_data_translated(usbhid_input_t translatedInData, int buttonInData,
                                      bool relative);

IMPLEMENT_GETINSTANCE(CUsbHeadsetAction)
CUsbHeadsetAction::CUsbHeadsetAction()
{
}

CUsbHeadsetAction::~CUsbHeadsetAction()
{
    usbheadset_api_uninit();
}

LRESULT CUsbHeadsetAction::OnMessage(msgObject & objMessage)
{
    if (objMessage.message != USB_MSG_HEADSET_ACTION
            && objMessage.message != USB_MSG_HEADSET_CMD)
    {
        return FALSE;
    }

    switch (objMessage.message)
    {
    case USB_MSG_HEADSET_ACTION:
        _UsbHeadsetAction.ProcessCmd();
        break;
    case USB_MSG_HEADSET_CMD:
        {
            if (objMessage.GetExtraSize() <= 0)
            {
                return FALSE;
            }

            UsbHeadsetCtrl * pHeadsetCmd = (UsbHeadsetCtrl *)objMessage.GetExtraData();
            if (pHeadsetCmd)
            {
                _UsbHeadsetAction.AddCmd(pHeadsetCmd->eCmd, pHeadsetCmd->iValue);
            }
        }
        break;
    default:
        break;
    }

    return TRUE;
}

// 添加命令
bool CUsbHeadsetAction::AddCmd(USBHEADSET_CTRL_TYPE eType, int iValue)
{
    UsbHeadsetCtrl headsetCmd;
    headsetCmd.eCmd = eType;
    headsetCmd.iValue = iValue;
    USBDEVICE_INFO("add cmd[%d], value[%d]", eType, iValue);

    //m_headsetCtrlCs.Lock();
    m_listCmd.push_back(headsetCmd);
    //m_headsetCtrlCs.Unlock();

    return true;
}

// 命令处理
bool CUsbHeadsetAction::ProcessCmd()
{
    if (m_listCmd.size() <= 0)
    {
        return false;
    }

    //m_headsetCtrlCs.Lock();
    YLList<UsbHeadsetCtrl> listCmd;
    for (YLList<UsbHeadsetCtrl>::iterator it = m_listCmd.begin();
            it != m_listCmd.end();
            ++ it)
    {
        UsbHeadsetCtrl & headsetCtrl = *it;

        // 跳过已经完成的,容错
        if (headsetCtrl.bFinished)
        {
            continue;
        }

        // 先标记已完成
        headsetCtrl.bFinished = true;
        listCmd.push_back(headsetCtrl);
    }
    //m_headsetCtrlCs.Unlock();

    RunCmd(listCmd);

    USBDEVICE_INFO("usb headset finish size[%d]", m_listCmd.size());
    m_listCmd.clear();
    /*int iUnFinishSize;
    int iFinishSize = ProcessFinishedCmd(iUnFinishSize);
    USBDEVICE_INFO("usb headset finish size[%d], unfinish size[%d]", iFinishSize, iUnFinishSize);
    if (iUnFinishSize > 0)
    {
        ProcessCmd();
    }*/

    return true;
}

// 执行命令
bool CUsbHeadsetAction::RunCmd(YLList<UsbHeadsetCtrl> & listCtrl)
{
    USBDEVICE_INFO("usb headset run cmd");

    USBHEADSET_CTRL_TYPE eType = USBHEADSET_NONE;

    YLList<UsbHeadsetCtrl>::iterator it = listCtrl.begin();
    for (; it != listCtrl.end(); ++it)
    {
        UsbHeadsetCtrl & headsetCtrl = *it;

        // 连续设置相同的按键,需要延迟处理
        if (eType == headsetCtrl.eCmd)
        {
            etlSleep(500);
        }

        eType = headsetCtrl.eCmd;
        switch (headsetCtrl.eCmd)
        {
        case USBHEADSET_INIT:
            {
                usbheadset_api_init();
            }
            break;
        case USBHEADSET_PLUGIN:
            {
                if (NULL != g_pDevice_info_g)
                {
                    usb_audio_volume_initialize(g_pDevice_info_g->card);
                    g_hHandle = usb_hid_open(g_pDevice_info_g->path, usbhid_button_in_data_raw,
                                             usbhid_button_in_data_translated);
                    USBDEVICE_INFO("usb headset open hid:%s.", g_hHandle != 0 ? "success" : "failed");
                }
                etl_NotifyApp(FALSE, USB_MSG_HEADSET_ACTION_RESULT, USBHEADSET_PLUGIN, g_hHandle != 0);
            }
            break;
        case USBHEADSET_PLUGOUT:
            {
                usb_audio_volume_uninitialize();
                if (0 != g_hHandle)
                {
                    usb_hid_close(g_hHandle);
                    g_hHandle = 0;
                    USBDEVICE_INFO("usb headset close hid.");
                }
                etl_NotifyApp(FALSE, USB_MSG_HEADSET_ACTION_RESULT, USBHEADSET_PLUGOUT, g_hHandle == 0);
            }
            break;
        case USBHEADSET_RING:
            {
                if (0 != g_hHandle)
                {
                    int iRet = usb_hid_set_tel_ringer(g_hHandle, headsetCtrl.iValue);
                    if (0 != iRet)
                    {
                        iRet = usb_hid_set_led_ring(g_hHandle, headsetCtrl.iValue);
                    }
                    USBDEVICE_INFO("usb headset set ring, value=%d, ret=%d", headsetCtrl.iValue, iRet);
                }
            }
            break;
        case USBHEADSET_HOOK:
            {
                if (0 != g_hHandle)
                {
                    int iRet = usb_hid_set_offhook(g_hHandle, headsetCtrl.iValue);
                    USBDEVICE_INFO("usb headset set hook, value=%d, ret=%d", headsetCtrl.iValue, iRet);
                }
            }
            break;
        case USBHEADSET_MUTE:
            {
                if (0 != g_hHandle)
                {
                    int iRet = usb_hid_set_mute(g_hHandle, headsetCtrl.iValue);
                    if (0 != iRet)
                    {
                        iRet = usb_hid_set_micphonemute(g_hHandle, headsetCtrl.iValue);
                    }
                    USBDEVICE_INFO("usb headset set mute, value=%d, ret=%d", headsetCtrl.iValue, iRet);
                }
            }
            break;
        case USBHEADSET_HOLD:
            {
                if (0 != g_hHandle)
                {
                    int iRet = usb_hid_set_hold(g_hHandle, headsetCtrl.iValue);
                    USBDEVICE_INFO("usb headset set hold, value=%d, ret=%d", headsetCtrl.iValue, iRet);
                }
            }
            break;
        case USBHEADSET_VOLUME:
            {
                int iRet = usb_audio_volume_set(headsetCtrl.iValue);
                USBDEVICE_INFO("usb headset sync state to usb, volume=%d, ret=%d", headsetCtrl.iValue, iRet);
            }
            break;
        case USBHEADSET_VOLUME_INC:
            {
                int iRet = usb_audio_volume_inc();
                USBDEVICE_INFO("usb headset inc volume ret=%d", iRet);
            }
            break;
        case USBHEADSET_VOLUME_DEC:
            {
                int iRet = usb_audio_volume_dec();
                USBDEVICE_INFO("usb headset dec volume ret=%d", iRet);
            }
            break;
        default:
            break;
        }
    }

    return true;
}

void usbhid_button_in_data_raw(unsigned short usagePage, unsigned short usage, int buttonInData,
                               bool relative)
{
}

void usbhid_button_in_data_translated(usbhid_input_t translatedInData, int buttonInData,
                                      bool relative)
{
    UsbHeadsetKeyInfo keyInfo;
    keyInfo.iKey = translatedInData;
    keyInfo.iButtonData = buttonInData;
    keyInfo.bRelative = relative;
    etl_NotifyAppEx(false, USB_MSG_HEADSET_KEY, 0, 0, sizeof(UsbHeadsetKeyInfo), &keyInfo);
}

void usbheadset_hotplug_callback_add(usbheadset_device_info_t * device_info, void * user_data)
{
    g_pDevice_info_g = device_info;
    etl_NotifyApp(false, USB_MSG_HEADSET_PLUG, 1, 0);
}

void usbheadset_hotplug_callback_remove(usbheadset_device_info_t * device_info, void * user_data)
{
    g_pDevice_info_g = NULL;
    etl_NotifyApp(false, USB_MSG_HEADSET_PLUG, 0, 0);
}

void usbheadset_api_init()
{
    usb_headset_initialize(usbheadset_hotplug_callback_add, usbheadset_hotplug_callback_remove, NULL);
    usb_headset_log_set(commonUnits_GetLogLevel());
}

void usbheadset_api_uninit()
{
    usb_headset_uninitialize();
}

#endif //IF_SUPPORT_USB_AUDIO
