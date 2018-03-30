#include "bluetoothuicommon.h"

bool BluetoothUI_IsConnectedDevice(DeviceInfo * pDeviceInfo)
{
    if (NULL == pDeviceInfo)
    {
        return false;
    }

#ifdef BLUETOOTH_USED_NEW_VERSION
    return pDeviceInfo->eState == WIRE_DEVICE_CONNECTED_STATE;
#endif

#ifdef BLUETOOTH_USED_OLD_VERSION
    return pDeviceInfo->eState == REMOTEDEV_CONNECTED_STATE;
#endif
}

bool BluetoothUI_GetConnectedDevice(DeviceInfo & objDeviceInfo)
{
    UI_LIST_BT_DEVICE listDeviceInfo;

    Bluetooth_GetDeviceList(listDeviceInfo
#ifdef BLUETOOTH_USED_NEW_VERSION
                            , DEVICE_LIST_PAIRED
#endif
                           );

    DeviceInfo * pDevice = NULL;
    UI_LIST_BT_DEVICE::iterator iter = listDeviceInfo.begin();

    for (; iter != listDeviceInfo.end(); ++iter)
    {
        pDevice = &(*iter);

        if (NULL == pDevice)
        {
            continue;
        }

        if (BluetoothUI_IsConnectedDevice(pDevice))
        {
            objDeviceInfo = *pDevice;
            return true;
        }
    }

    return false;
}
