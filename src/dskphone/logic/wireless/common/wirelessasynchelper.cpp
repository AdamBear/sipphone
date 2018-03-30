#include "wirelessasynchelper.h"
#include "taskaction/modtaskaction.h"
#include "taskaction/taskaction.h"
#ifdef IF_SUPPORT_BLUETOOTH
#include "wireless/bluetoothNew/src/btaction.h"
#include "wireless/bluetoothNew/src/btmanager.h"
#endif
#ifdef IF_SUPPORT_WIFI
#include "wireless/wifi/src/wifiaction.h"
#include "wireless/wifi/src/wifimanager.h"
#endif

#ifdef IF_SUPPORT_BLUETOOTH
bool WLAsyncHelper_BtAsync(BT_TASK_TYPE eType,
                           unsigned long ulDevHandle /*= BT_DEVICE_HANDLE_INVAILED*/,
                           const BTTaskParam* pParam /*= NULL*/, bool bSync /*= false*/)
{

    CBTAction* pAction = new CBTAction();
    pAction->SetSync(bSync);
    pAction->SetCallback(&_BTManager);
    pAction->SetNeedQueue(true);
    pAction->SetTaskType(eType);
    pAction->SetRelateHandle(ulDevHandle);

    // 组装额外参数
    if (NULL != pParam)
    {
        BTTaskParam sParam;
        sParam.lSessionID = pParam->lSessionID;
        sParam.ulDeviceHandle = ulDevHandle;
        sParam.wParam = pParam->wParam;
        sParam.lParam = pParam->lParam;
        sParam.strParam = pParam->strParam;
        pAction->SetBtExecParam(sParam);
    }

    TaskAction_ExecAction(pAction);

    return true;
}
#endif // IF_SUPPORT_BLUETOOTH

#ifdef IF_SUPPORT_WIFI
bool WLAsyncHelper_WifiAsync(WIFI_TASK_TYPE eType, const WiFiTaskParam* pParam /*= NULL*/,
                             bool bSync /*= false*/)
{
    CWifiAction* pAction = new CWifiAction();
    pAction->SetSync(bSync);
    pAction->SetCallback(&_WiFiManager);
    pAction->SetNeedQueue(true);
    pAction->SetTaskType(eType);

    // 组装额外参数
    WiFiTaskParam sParam;
    if (NULL != pParam)
    {
        sParam.wParam = pParam->wParam;
        sParam.lParam = pParam->lParam;
        sParam.lSessionID = pParam->lSessionID;
    }

    pAction->SetWifiExecParam(sParam);
    TaskAction_ExecAction(pAction);

    return true;
}

#endif // IF_SUPPORT_WIFI
