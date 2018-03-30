#ifndef __WIRELESS_ASYNC_HELPER_H__
#define __WIRELESS_ASYNC_HELPER_H__

#include "wireless/bluetoothNew/include/btdefines.h"
#include "wireless/wifi/include/wifidefines.h"

#ifdef IF_SUPPORT_BLUETOOTH
bool WLAsyncHelper_BtAsync(BT_TASK_TYPE eType,
                           unsigned long ulDevHandle = BT_DEVICE_HANDLE_INVAILED,
                           const BTTaskParam* pParam = NULL, bool bSync = false);
#endif // IF_SUPPORT_BLUETOOTH

#ifdef IF_SUPPORT_WIFI
bool WLAsyncHelper_WifiAsync(WIFI_TASK_TYPE eType, const WiFiTaskParam* pParam = NULL,
                             bool bSync = false);
#endif // IF_SUPPORT_WIFI

#endif // __WIRELESS_ASYNC_HELPER_H__
