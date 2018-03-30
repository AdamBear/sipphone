#ifdef IF_FEATURE_DSSKEY
#include "talkdsskey_inc.h"

CDsskeyCommonAction::CDsskeyCommonAction(DssKey_Type eDsskeyType)
    : CDssTalkBaseAciton(eDsskeyType)
{

}

bool CDsskeyCommonAction::OnKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData,
                                     void* pExtraData)
{
    switch (GetDsskeyType())
    {
    case DT_BLUETOOTH:
        return OnBluetoothKeyPress(dsskeyDataID, logicData, pExtraData);
#ifdef IF_SUPPORT_WIFI
    case DT_WIFI:
        return OnWiFiKeyPress(dsskeyDataID, logicData, pExtraData);
#endif
#ifdef IF_SUPPORT_VIDEO
    case DT_CAMERA:
        return OnCameraKeyPress(dsskeyDataID, logicData, pExtraData);
    case DT_EXT_DISPLAY:
        return OnExtDisplayKeyPress(dsskeyDataID, logicData, pExtraData);
#endif
    default:
        break;
    }

    return false;
}

bool CDsskeyCommonAction::OnBluetoothKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData,
        void* pExtraData)
{
#ifdef IF_SUPPORT_BLUETOOTH
    //通话中不能按蓝牙dsskey
    //适配器未接入时按下不响应
    if (talklogic_SessionExist()
            || DS_NO_DONGLE == Bluetooth_GetDeviceState())
    {
        return false;
    }

    return Bluetooth_SetEnable(!Bluetooth_IsEnable());
#else
    return false;
#endif // IF_SUPPORT_BLUETOOTH

}

#ifdef IF_SUPPORT_WIFI
bool CDsskeyCommonAction::OnWiFiKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData,
        void* pExtraData)
{
    if (talklogic_SessionExist()
            || !WIFI_IsDongleVaild())
    {
        return false;
    }

    return WIFI_SetEnable(!WIFI_IsEnable(), true);
}
#endif

#ifdef IF_SUPPORT_VIDEO
bool CDsskeyCommonAction::OnCameraKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData,
        void* pExtraData)
{
    CSessionStateProxy focus;
    if (!focus || focus.IsHide())
    {
        return AdatapterBox_EnterCameraPage();
    }
    else
    {
        return ProcessNoBindDsskey(focus, logicData);
    }
}

bool CDsskeyCommonAction::OnExtDisplayKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData,
        void* pExtraData)
{
#ifdef IF_SUPPORT_HDMI
    if (!modhotplug_IsHdmiConnected()
            || !modhotplug_IsHdmiEnable())
    {
        return false;
    }

    return modhotplug_SetHdmiSwitch(!modhotplug_GetHdmiSwitch());
#else
    return false;
#endif //#ifdef IF_SUPPORT_HDMI
}

bool CDsskeyCommonAction::ProcessNoBindDsskey(const CSessionStateProxy& proxy, const DssKeyLogicData& logicData)
{
    if (GetDsskeyType() != DT_CAMERA)
    {
        return false;
    }

    if (proxy.IsTalkSession()
            || proxy.IsInherits(ROUTINE_CONNECTING))
    {
        return talklogic_UIEvent(proxy, TALK_ACTION_MUTE_VIDEO);
    }

    return false;
}
#endif
#endif
