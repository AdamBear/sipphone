#ifndef __BLUETOOTHUICOMMON_H
#define __BLUETOOTHUICOMMON_H

#include "wireless/bluetoothNew/include/modbluetooth.h"

#ifdef IF_OLD_BLUETOOTH
#define BLUETOOTH_USED_OLD_VERSION
#else
#define BLUETOOTH_USED_NEW_VERSION
#endif

// 是否支持蓝牙手机设备
#ifdef IF_BT_SUPPORT_PHONE
#define SUPPORT_PHONE_DEVICE
#endif

// Bluetooth detail popup dialog type
enum BLUETOOTH_DETAIL_POPUP_DLG_TYPE
{
    BLUETOOTH_DETAIL_POPUP_DLG_COMMON = 0x0001,
    BLUETOOTH_DETAIL_POPUP_DLG_MOBILE,
};

// 蓝牙界面Item宽高定义
#define BLUETOOTH_UI_ITEM_WIDTH                        536

// 蓝牙界面常驻Item个数
#define BLUETOOTH_UI_RESIDENT_ITEM_COUNT_DEFAULT    2
#define BLUETOOTH_UI_RESIDENT_ITEM_COUNT_TESTMODE   1

bool BluetoothUI_IsConnectedDevice(DeviceInfo * pDeviceInfo);

bool BluetoothUI_GetConnectedDevice(DeviceInfo & objDeviceInfo);

#endif // __BLUETOOTHUICOMMON_H
