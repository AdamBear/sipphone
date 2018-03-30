///////////////////////////////////////////////////////////
//  CBTManager.cpp
//  Implementation of the Class CBTManager
//  Created on:     2014年7月15日 11:14:36
//  Original author: songk
///////////////////////////////////////////////////////////
#ifdef IF_SUPPORT_BLUETOOTH
#include "bluetooth_inc.h"
#include "commonunits/modcommonunits.h"
#include "contacts/directory/include/moddirinterface.h"
#include "keyguard/include/modkeyguard.h"
#include "talk/talklogic/include/talkmsgdefine.h"
#include "taskaction/modtaskaction.h"
#include "btaction.h"
#if IF_FEATURE_EVENT_MODULE
#include "extended/event/include/modevent.h"
#endif

IMPLEMENT_GETINSTANCE(CBTManager)
CBTData CBTManager::m_btData;

LRESULT OnBluetoothtManagerMsg(msgObject & objMsg)
{
    return _BTManager.OnRecieveMsg(objMsg);
}

yl::string GetCallbackEventName(unsigned long lMsg);
void PrintBtMsg(msgObject & objMessage);

//构造函数
CBTManager::CBTManager():
    m_iAutoConnectTimer((UINT) & m_iAutoConnectTimer),
    m_iAutoConnectIndex(0)
{
    ResetAutoConnectIndex();
    SetDeviceState(DS_NO_DONGLE);
}

//析构函数
CBTManager::~CBTManager()
{
    SetAutoConnectTimer(false);
    DoClose();
    SetDeviceState(DS_UNINIT);
    commonUnits_RemoveSysLogFunc(CBTManager::SetBtLogLevel);
}

bool CBTManager::Init()
{
    if ((!IsBuiltInBluetooth()
            && configParser_GetConfigInt(kszUsbPortEnable) == 0)
            || configParser_GetConfigInt(kszBluetoothFuncEnable) == 0)
    {
        WIRE_WARN("Bt init fail, usb port or bt func disabled.");
        return false;
    }

    RegisterMsg();
    _BTSync;
    commonUnits_AddSysLogFunc(CBTManager::SetBtLogLevel);
    m_bEnable = configParser_GetConfigInt(kszBluetoothSwitch) == 1 ;

    // 初始化蓝牙
    InitBluetooth();
    WIRE_INFO("Bluetooth support talk[%d], support contact[%d]",
              IsSupportFeature(BT_FEATURE_AUDIO), IsSupportFeature(BT_FEATURE_CONTACT));

    return true;
}

// 初始化蓝牙
bool CBTManager::InitBluetooth()
{
    // 初始化驱动
    if (!_BTDriver.InitDriver())
    {
        return false;
    }

    // 设置蓝牙状态
    SetDeviceState(DS_UNINIT);

    // 打开蓝牙开关
    if (m_bEnable)
    {
        SetPowerByThread(m_bEnable);
    }

    // 设置蓝牙显示名称
    return SetDeviceName(configParser_GetConfigString((const char *)kszBluetoothAdapterName).c_str());
}

void CBTManager::SetDeviceState(DEVICE_STATE eState)
{
    WIRE_INFO("change bt state[%d] to [%d]", m_eState, eState);
    m_eState = eState;
}

int CBTManager::GetCurrentAutoConnectCount()
{
    //WIRE_INFO("Current AutoConnect Count[%d]", m_iAutoConnectCount);
    return m_iAutoConnectCount;
}

// 获取设备状态
DEVICE_STATE CBTManager::GetDeviceState()
{
    return m_eState;
}

//适配器是否已经初始化
bool CBTManager::IsDongleValid()
{
    return (GetDeviceState() != DS_NO_DONGLE);
}

//是否正在搜索设备
bool CBTManager::IsInScan()
{
    return _WirelessSessionManager.IsInState(BT_SESSION, SESSION_SCAN_STATE, BT_DEVICE_HANDLE_INVAILED);
}

//初始化session
bool CBTManager::InitSession(WIRELESS_SESSION_STATE eState,
                             unsigned long ulDeviceHandle /* = BT_DEVICE_HANDLE_INVAILED */, void * pData /* = NULL */)
{
    if (_WirelessSessionManager.InitSession(eState, BT_SESSION, ulDeviceHandle, pData))
    {
        if (SESSION_SCAN_STATE == eState)
        {
            SetAutoConnectEnable(false);
        }
        SetAutoConnectTimer(false);
        return true;
    }

    return false;
}

//删除话路
bool CBTManager::DeleteSession(CBtSession * pSession, bool bEndConnect /* = false */,
                               bool bAutoConnect/* = true*/)
{
    if (NULL == pSession)
    {
        return false;
    }

#if 0
    //停止扫描，及断开设备时，重置自动连接次数
    if (pSession->GetSessionState() == SESSION_SCAN_STATE
            || pSession->GetSessionState() == SESSION_CONNECTED_STATE)
    {
        ResetAutoConnectCount();
    }
#endif // 0

    _WirelessSessionManager.DeleteSession(pSession, bEndConnect);

    if (IsAutoConnectEnable())
    {
        SetAutoConnectTimer(true);
    }

    return true;
}

//连接等待对列中的设备
bool CBTManager::ConnectWaittingDevice(unsigned long ulDeviceHandle/* = BT_DEVICE_HANDLE_INVAILED*/)
{
    if (BT_DEVICE_HANDLE_INVAILED == ulDeviceHandle
            && _BTData.GetWaittingDeviceCount() > 0)
    {
        ulDeviceHandle = _BTData.GetWaittingDevice();
    }

    if (BT_DEVICE_HANDLE_INVAILED == ulDeviceHandle)
    {
        return false;
    }

    return ProcessConnectDevice(ulDeviceHandle);
}

//初始化设备信息
bool CBTManager::DoInit()
{
    //初始化蓝牙设备信息,
    _BTData.InitDeviceInfo();

    //POWER_ON 之后再设置一次device name,避免底层device name有误
    //http://10.2.1.199/Bug.php?BugID=77967
    SetDeviceName(configParser_GetConfigString((const char *)kszBluetoothAdapterName).c_str());

    //蓝牙打开时，通知UI显示图标
    etl_NotifyApp(false, BLUETOOTH_MSG_STATE_CHANGE, 1, 0);

    if (m_bEnable)
    {
        SetAutoConnectEnable(true);
    }

    // 蓝牙开启成功, 自动连接
    // 最后一个连接过的是耳机, 则设置自动连接定时器, 否则立即开始自动连接
    SetAutoConnectStragety(AC_STRATEGY_SHORT_TIME);
    if (IsAutoConnectEnable())
    {
        if (_BTData.IsLastDevSupportAC())
        {
            AutoConnectDevice(true);
        }
        else
        {
            SetAutoConnectTimer(true);
        }
    }

    return true;
}

//处理关闭
bool CBTManager::DoClose()
{
    bool bUpdate = false;
    //还在扫描中时，清除后需要刷新下UI
    if (IsInScan())
    {
        bUpdate = true;
    }

    //已连接设备句柄
    long lDeviceHandle = BT_DEVICE_HANDLE_INVAILED;
    CSessionBase * pSession = _WirelessSessionManager.GetSessionByState(SESSION_CONNECTED_STATE,
                              BT_SESSION);
    if (NULL != pSession)
    {
        lDeviceHandle = pSession->GetHandle();
    }

    _WirelessSessionManager.DeleteAllSession(BT_SESSION, true);

    //保证断开与powerset 时序
    WLAsyncHelper_BtAsync(BT_TASK_POWER_OFF);

    //通知AutoSet 还原dsskey
    _BTSync.SetDsskey(false);

    SetAutoConnectTimer(false);

    //清空内存中蓝牙设备信息
    _BTData.ClearDeviceList();
    _BTData.ClearScanDeviceList();
    _BTData.SetLastSupportACDevHandle(BT_DEVICE_HANDLE_INVAILED);

    ResetAutoConnectIndex();
    ResetAutoConnectCount();
    // 通知UI断开连接
    if (lDeviceHandle != BT_DEVICE_HANDLE_INVAILED)
    {
        etl_NotifyApp(false, BLUETOOTH_MSG_CONNECT_STATE, 0, _BTData.GetDeviceType(lDeviceHandle));
    }

    if (bUpdate)
    {
        UpdateUI();
    }

    etl_NotifyApp(false, BLUETOOTH_MSG_STATE_CHANGE, 0, 0);

    return true;
}

//处理定时器消息
bool CBTManager::ProcessTimer(int iTimerID)
{
    bool bHandle = true;

    if (iTimerID == m_iAutoConnectTimer)
    {
        AutoConnectDevice(GetAutoConnectStragety() == AC_STRATEGY_SHORT_TIME ? true : false);
    }
    else
    {
        bHandle = _WirelessSessionManager.SendTimerMsgToSession(iTimerID, BT_SESSION);
    }

    if (!bHandle)
    {
        bHandle = _BTSync.ProcessTimer(iTimerID);
    }

    return bHandle;
}

//注册消息
void CBTManager::RegisterMsg()
{
    //注册用户消息
    etl_RegisterMsgHandle(BLUETOOTH_MESSAGE_BEGIN, BLUETOOTH_MESSAGE_END, OnBluetoothtManagerMsg);
    //注册定时器接收消息
    etl_RegisterMsgHandle(TM_TIMER, TM_TIMER, OnBluetoothtManagerMsg);
    //注册接收配置改变
    etl_RegisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED, OnBluetoothtManagerMsg);
}

//反注册消息
void CBTManager::UnRegisterMsg()
{
    //用户消息
    etl_UnregisterMsgHandle(BLUETOOTH_MESSAGE_BEGIN, BLUETOOTH_MESSAGE_END, OnBluetoothtManagerMsg);
    //定时器消息
    etl_UnregisterMsgHandle(TM_TIMER, TM_TIMER, OnBluetoothtManagerMsg);
    //配置改变
    etl_UnregisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED, OnBluetoothtManagerMsg);
}

//接收消息
bool CBTManager::OnRecieveMsg(msgObject & objMessage)
{
    bool bHanlde = true;
    PrintBtMsg(objMessage);
    if (BLUETOOTH_MSG_CALLBACK == objMessage.message)
    {
        return ProcessCallBackEvent(objMessage);
    }
    else if (TM_TIMER == objMessage.message)
    {
        return ProcessTimer(objMessage.wParam);
    }
    else if (CONFIG_MSG_BCAST_CHANGED == objMessage.message)
    {
        if (ST_BT_CONFIG != objMessage.wParam)
        {
            return false;
        }

        if (objMessage.lParam == 0
                || objMessage.lParam == 1)
        {
            SetBTOpenStatus(1 == configParser_GetConfigInt(kszBluetoothSwitch), false);
        }
        else if (objMessage.lParam == 2)
        {
            yl::string strName = configParser_GetConfigString(kszBluetoothAdapterName);
            if (strName.size() <= BT_MAX_NAME_LEN)
            {
                _BTDriver.SetModuleName(strName.c_str());
            }
        }
    }
    else
    {
        switch (objMessage.message)
        {
        case BLUETOOTH_MSG_GETSTATUS:
            {
                _BTData.ShowBluetoothList();

                WIRE_INFO("Dongle[%d], Enable[%d], Scan[%d], Connecting[%d], Connected[%d]",
                          IsDongleValid(),
                          IsEnable(),
                          IsInScan(),
                          IsConnectingSessionExist(),
                          IsConnectedSessionExist());


                if (IsConnectingSessionExist())
                {
                    CBtSession * pSession = GetSessionByState(SESSION_CONNECTING_STATE);
                    if (NULL != pSession)
                    {
                        WIRE_INFO("device[%d] is connecting", pSession->GetHandle());
                    }

                    pSession = GetSessionByState(SESSION_PAIRING_STATE);
                    if (NULL != pSession)
                    {
                        WIRE_INFO("device[%d] is pairing", pSession->GetHandle());
                    }
                }

                if (IsConnectedSessionExist())
                {
                    CBtSession * pSession = GetSessionByState(SESSION_CONNECTED_STATE);
                    if (NULL != pSession)
                    {
                        WIRE_INFO("device[%d] is connected", pSession->GetHandle());
                    }
                }
            }
            break;
        default:
            {
                bHanlde = false;
            }
            break;
        }
    }
    return bHanlde;
}

// 开关蓝牙
bool CBTManager::SetBTOpenStatus(bool bEnable, bool bSetConfig/* = true*/)
{
    if (m_bEnable == bEnable)
    {
        WIRE_INFO("status not change ,do not set [%s]", bEnable ? "enable" : "disable");
        return true;
    }

    SetEnable(bEnable, bSetConfig);
    SetPowerByThread(bEnable);

    return true;
}

// 子线程设置驱动开关
bool CBTManager::SetPowerByThread(bool bEnable)
{
    if (!IsDongleValid())
    {
        WIRE_INFO("no dongle ,do not set power [%s]", bEnable ? "on" : "off");
        return false;
    }

    if (m_bEnable && bEnable)
    {
        WLAsyncHelper_BtAsync(BT_TASK_POWER_ON);
    }
    else
    {
        DoClose();
        SetDeviceState(DS_UNINIT);
    }

    return true;
}

//删除设备
bool CBTManager::DeleteDevice(unsigned long ulDeviceHandle, bool bDelAll)
{
    if (!IsDongleValid())
    {
        return false;
    }

    WIRE_INFO("delete %s device[%d]", bDelAll ? "all" : "", ulDeviceHandle);

    bool bClearAll = true;

    if (!bDelAll
            && !_BTData.IsInWaittingList(ulDeviceHandle))
    {
        DeleteSession(GetSessionByHandle(ulDeviceHandle), true);
        bClearAll = false;
    }

    if (bDelAll
            || bClearAll)
    {
        DeleteSession(GetSessionByState(SESSION_PAIRING_STATE), true, false);
#ifdef IF_BLUETOOTH_SUPPORT_REMOTE_PAIR_REQ
        DeleteSession(GetSessionByState(SESSION_REMOTE_REQ_PAIRING_STATE), true, false);
#endif
        DeleteSession(GetSessionByState(SESSION_CONNECTING_STATE), true, false);
        DeleteSession(GetSessionByState(SESSION_CONNECTED_STATE), true, false);
    }

    //http://bugfree.yealink.com/Bug.php?BugID=78238
    //删除当前自动连接手机后，需要恢复到开始状态，可以去循环连接列表
    /*if (!bDelAll)
    {
        DeviceInfo* pDeviceInfo = _BTData.GetDeviceInfoByHandle(ulDeviceHandle);
        if (NULL != pDeviceInfo
            && pDeviceInfo->bAutoConnect
            && _ISBTPHONE(pDeviceInfo->eType))
        {
            _BTData.UpdateAutoConnectDevice();
        }
    }*/

    //删除并更新显示
    bool result =  _BTData.DeleteDevice(ulDeviceHandle, bDelAll);
    UpdateUI();

    //可能此时等待队列里还有设备，清空掉
    if (bDelAll)
    {
        _BTData.GetWaittingDevice();
    }
    else
    {
        ConnectWaittingDevice();
    }

    return result;
}

//设置是否自动连接设备
bool CBTManager::SetDeviceAutoConnect(bool bAutoConnnect, unsigned long ulDeviceHandle)
{
    if (BT_DEVICE_HANDLE_INVAILED == ulDeviceHandle)
    {
        return false;
    }

    _BTData.SetAutoConnect(bAutoConnnect, ulDeviceHandle);

    //调整后，判断是否要开启自动连接定时器
    if (_BTData.GetAutoConnectNum() > 0
            && IsAllowAutoConnect())
    {
        SetAutoConnectTimer(true);
    }
    else
    {
        SetAutoConnectTimer(false);
    }

    return true;
}

//设置同步联系人
bool CBTManager::SetSyncContact(bool bSync, unsigned long ulDeviceHandle)
{
    if (BT_DEVICE_HANDLE_INVAILED == ulDeviceHandle)
    {
        return false;
    }

    _BTData.SetProfileEnable(ulDeviceHandle, bSync, PROFILE_PBA);

    DeviceInfo * pDevice = GetCurrentDevice(SESSION_CONNECTED_STATE);
    if (NULL == pDevice
            || pDevice->devHandle != ulDeviceHandle
            || !_ISBTPHONE(pDevice->eType))
    {
        return true;
    }

    //立即进行一次同步
    OPERATION_RESULT eOpResult = OP_CONNECT_RESULT;
    if (bSync)
    {
        _BTSync.StartDownloadDirectory();
    }
    else
    {
        _BTSync.StopDownload(false);
        eOpResult = OP_DISCONNECT_RESULT;
    }

    //通知联系人状态
    etl_NotifyApp(FALSE, BLUETOOTH_MSG_DIR_DONE, eOpResult, true);

    return true;
}

//设置mobile profile开关
bool CBTManager::SetDevMobileEnable(unsigned long ulHandle, bool bEnable, bool bProcessProfile /*= true*/)
{
    if (BT_DEVICE_HANDLE_INVAILED == ulHandle)
    {
        return false;
    }

    // 设置profile开关状态到device info
    if (_BTData.SetProfileEnable(ulHandle, bEnable, PROFILE_MOBILE))
    {
        // 当前操作profile的设备与已连接设备不是同一个, 则不能操作profile连接/断开
        DeviceInfo* pConnectedDev = GetCurrentDevice(SESSION_CONNECTED_STATE);
        if (pConnectedDev && pConnectedDev->devHandle != ulHandle)
        {
            return true;
        }

        // 立即连接/断开profile
        BTTaskParam sParam(0, PROFILE_MOBILE);
        if (bEnable)
        {
            WLAsyncHelper_BtAsync(BT_TASK_CONNECT_PROFILES, ulHandle, &sParam);
        }
        else
        {
            WLAsyncHelper_BtAsync(BT_TASK_DISCONNECT_PROFILES, ulHandle, &sParam);
        }
    }

    return true;
}

//设置a2dp profile开关
bool CBTManager::SetDevA2dpEnable(unsigned long ulHandle, bool bEnable, bool bProcessProfile /*= true*/)
{
    if (BT_DEVICE_HANDLE_INVAILED == ulHandle)
    {
        return false;
    }

    // 设置profile开关状态到device info
    if (_BTData.SetProfileEnable(ulHandle, bEnable, PROFILE_A2DP))
    {
        // 当前操作profile的设备与已连接设备不是同一个, 则不能操作profile连接/断开
        DeviceInfo* pConnectedDev = GetCurrentDevice(SESSION_CONNECTED_STATE);
        if (pConnectedDev && pConnectedDev->devHandle != ulHandle)
        {
            return true;
        }

        // 立即连接/断开profile
        BTTaskParam sParam(0, PROFILE_A2DP);
        if (bEnable)
        {
            WLAsyncHelper_BtAsync(BT_TASK_CONNECT_PROFILES, ulHandle, &sParam);
        }
        else
        {
            WLAsyncHelper_BtAsync(BT_TASK_DISCONNECT_PROFILES, ulHandle, &sParam);
        }
    }

    return true;
}

//HeadSet PlayRing
bool CBTManager::HeadSetDevicePlayRing(bool bRing)
{
    if (0 == configParser_GetConfigInt(kszHeadsetPortEnable))
    {
        return false;
    }

    DeviceInfo * pDevice = GetCurrentDevice(SESSION_CONNECTED_STATE);

    if (NULL != pDevice
            && _ISBTHEADSET(pDevice->eType))
    {
        WIRE_INFO("set headset playring[%d]", bRing);
        return _BTDriver.Ring(pDevice->devHandle, bRing);
    }

    return false;
}

bool CBTManager::HeadSetDeviceEvent(HEADSET_EVENT eHeadSetEvent, bool bChoice /* = true */,
                                    const char * pNum /* = NULL */)
{
    if (0 == configParser_GetConfigInt(kszHeadsetPortEnable))
    {
        return false;
    }

    DeviceInfo * pDevice = GetCurrentDevice(SESSION_CONNECTED_STATE);

    if (NULL != pDevice
            && _ISBTHEADSET(pDevice->eType))
    {
        WirelessSessionData wsData;
        wsData.ulMessage = WIRELESS_OPTION_BT_HEADSET_EVENT;
        wsData.wParam = eHeadSetEvent;
        wsData.lParam = bChoice;
        wsData.pData = (void *)pNum;

        return _WirelessSessionManager.SendOptionToSessionByHandle(pDevice->devHandle, wsData, BT_SESSION);
    }

    return false;
}

//设置蓝牙本身显示名称
bool CBTManager::SetDeviceName(const char * pName, bool bUpdateLevel/* =false*/)
{
    if (NULL == pName
            || (0 == strcmp(pName, "")))
    {
        return false;
    }

    yl::string strName = pName;
    strName = commUnit_ReplaceWildcard(strName);

    WIRE_INFO("set bluetooth name = [%s]: [%s]", pName, strName.c_str());

    configParser_SetConfigString(kszBluetoothAdapterName, pName,
                                 bUpdateLevel ? CONFIG_LEVEL_PHONE : CONFIG_LEVEL_IGNORE);

    if (!IsDongleValid())
    {
        return true;
    }

    return _BTDriver.SetModuleName(strName.c_str());
}

//处理回调消息
bool CBTManager::ProcessCallBackEvent(msgObject & objMsg)
{
    const btkit_msg_t* pMsg = (const btkit_msg_t *)objMsg.GetExtraData();
    if (NULL == pMsg)
    {
        WIRE_WARN("[Btlib Info] Extra data is null");
        return false;
    }

    WIRE_INFO("[Btlib Info][Event:%s,%d][dev_handle:%d]"
              , GetCallbackEventName(pMsg->bt_event).c_str()
              , pMsg->bt_event
              , pMsg->bt_callback_param.dev_hnd
             );

    //这里需要判断下是否是当前dongle的信息
    //dongle的插拔不需要判断
    if (pMsg->bt_instance != _BTDriver.GetDongleInstance()
            && BT_ADAPTER_ADDED != pMsg->bt_event
            && BT_ADAPTER_REMOVED != pMsg->bt_event)
    {
        WIRE_WARN("[Btlib Info] Dongle instance not match");
        return false;
    }

    if (_BTSync.IsBtSyncMsg(pMsg->bt_event))
    {
        return  _BTSync.OnSyncMsg(objMsg);
    }
    else if (_BTA2DP.IsA2dpMsg(pMsg->bt_event))
    {
        return  _BTA2DP.OnA2dpEvent(objMsg);
    }

    bool bHandle = true;
    switch (pMsg->bt_event)
    {
    case BT_ADAPTER_ADDED:
        {
            ProcessAdapterAdded(pMsg);
            //蓝牙适配器插入
            event_SendConnectedDeviceEvent(EVENT_PERIPHERAL_INFORMATION);
        }
        break;
    case BT_ADAPTER_REMOVED:
        {
            ProcessAdapterRemoved(pMsg);
            //蓝牙适配器拔出
            event_SendConnectedDeviceEvent(EVENT_PERIPHERAL_INFORMATION);
        }
        break;
    case BT_DEV_DISCONNECTED:
        {
            // 已连接会话存在时，才需要通知UI
            SendCallbackMsgToSession(pMsg->bt_callback_param.dev_hnd, objMsg);

            ProcessCallBackDisconnected(pMsg->bt_callback_param.dev_hnd);
        }
        break;
    case BT_REQ_PIN:
        {
            //Step 1. 当前设备正在配对,对应的session 处理
            if (_WirelessSessionManager.IsInState(BT_SESSION, SESSION_PAIRING_STATE,
                                                  pMsg->bt_callback_param.dev_hnd, NULL)
                    || _WirelessSessionManager.IsInState(BT_SESSION, SESSION_CONNECTING_STATE,
                            pMsg->bt_callback_param.dev_hnd, NULL))
            {
                return SendCallbackMsgToSession(pMsg->bt_callback_param.dev_hnd, objMsg);
            }

            //Step 2. 已匹配的设备,直接回复默认PIN码
            DeviceInfo * pDevice = _BTData.GetDeviceInfoByHandle(pMsg->bt_callback_param.dev_hnd);
            if (NULL != pDevice
                    && _BTData.IsDevicePaired(pDevice->eState))
            {
                WIRE_INFO("answer device[%d] pin[%s]", pMsg->bt_callback_param.dev_hnd, "0000");
                return _BTDriver.AnswerPin(pMsg->bt_callback_param.dev_hnd, "0000");
            }

            WIRE_INFO("reject device[%d] pair ask", pMsg->bt_callback_param.dev_hnd);
            WLAsyncHelper_BtAsync(BT_TASK_CANCLEPAIR, pMsg->bt_callback_param.dev_hnd);
        }
        break;
    case BT_AUTHORIZE:
        {
            //Step 1.设备在扫描,拒绝蓝牙耳机请求
            if (IsInScan())
            {
                WIRE_INFO("reject device[%d] authorize,bt is in scan.", pMsg->bt_callback_param.dev_hnd);
                return _BTDriver.AnswerAuth(pMsg->bt_callback_param.dev_hnd, false);
            }

            //Step 2.设备正在连接，对应的session处理
            if (_WirelessSessionManager.IsInState(BT_SESSION, SESSION_CONNECTING_STATE,
                                                  pMsg->bt_callback_param.dev_hnd))
            {
                return SendCallbackMsgToSession(pMsg->bt_callback_param.dev_hnd, objMsg);
            }

            //Step 3. 其他设备正在操作，直接拒绝
            if (_WirelessSessionManager.IsInState(BT_SESSION, SESSION_CONNECTING_STATE,
                                                  BT_DEVICE_HANDLE_INVAILED)
                    || _WirelessSessionManager.IsInState(BT_SESSION, SESSION_CONNECTED_STATE, BT_DEVICE_HANDLE_INVAILED)
                    || _WirelessSessionManager.IsInState(BT_SESSION, SESSION_PAIRING_STATE, BT_DEVICE_HANDLE_INVAILED))
            {
                WIRE_INFO("reject device[%d] authorize", pMsg->bt_callback_param.dev_hnd);
                return _BTDriver.AnswerAuth(pMsg->bt_callback_param.dev_hnd, false);
            }

            //Step 4. 进入连接中状态，不需要调用连接接口
            BTSessionInfo * pSessionInfo = new BTSessionInfo;
            pSessionInfo->bSponor = true;
            pSessionInfo->lDevHandle = pMsg->bt_callback_param.dev_hnd;
            return InitSession(SESSION_CONNECTING_STATE, pSessionInfo->lDevHandle, pSessionInfo);
        }
        break;
    case BT_DEV_CONNECTED:
    case BT_DEV_CONNECTED_CANCEL:
    case BT_DEV_CONNECTED_FAILE:
        {
            SendCallbackMsgToSession(pMsg->bt_callback_param.dev_hnd, objMsg);
        }
        break;
    case BT_FOUND_DEV:
        {
            SendCallbackMsgToSession(SESSION_SCAN_STATE, objMsg);
        }
        break;
    case BT_DEV_PROPERTY_CHANGED:
        {
            ProcessCallbackPropertyChanged(pMsg);
        }
        break;
    case BT_DEV_PAIR_CHANGED:
    case BT_PAIR_CANCEL:
        {
            SendCallbackMsgToSession(pMsg->bt_callback_param.pair_info.dev_hnd, objMsg);
        }
        break;
    case BT_PIN_CONFIRM:
        {
            ProcessPinConfirm(objMsg);
        }
        break;
    case BT_FOUND_DEV_DISAPPEARED:
        {
            ProcessCallbackDeviceDisappeared(pMsg);
        }
        break;
    case BT_HF_ANSWER_REQUEST:
    case BT_HF_CALL_TERMINATED:
    case BT_HF_CALL_REDIAL:
    case BT_HF_CALL_ANSWER:
        {
            ProcessKey(objMsg);
        }
        break;
    case BT_POWER_ON:
        {
            // 初始化蓝牙信息
            DoInit();
            // 底层初始化成功, 设置state为DS_OK
            SetDeviceState(DS_OK);
            // 通知外部模块, 蓝牙初始化成功
            etl_NotifyApp(false, BLUETOOTH_MSG_DEVICE_POWER, 0, 0);
        }
        break;
    case BT_DEV_PROFILE_CONNECTED:
    case BT_DEV_PROFILE_CONNECT_FAIL:
    case BT_DEV_PROFILE_DISCONNECTED:
        {
            ProcessProfileConnectStatusChange(objMsg);
        }
        break;
    default:
        {
            bHandle = false;
        }
        break;
    }

    return bHandle;
}

//处理重拨
bool CBTManager::ProcessCallBackRedial()
{
    //键盘锁上锁时，不能Redial
    if (keyGuard_IsEnable()
            && keyGuard_IsLock())
    {
        WIRE_INFO("Phone is In Lock Mode, Cannot Redial By BT HeadSet");
        return false;
    }

    yl::string strNumber = "";
    int iAccountID = -1;
    LocalLog_GetLastOutCallNumAndAccountID(strNumber, iAccountID);
    talklogic_CallOut(strNumber, "", iAccountID);
    // http://10.2.1.199/Bug.php?BugID=89473
    commonUnits_SendSimulateKey(DEV_MSG_FUNKEY_EVENT, PHONE_KEY_EHS_OPEN);

    return true;
}

//断开后处理
bool CBTManager::ProcessCallBackDisconnected(unsigned long ulDeviceHandle)
{
    return ConnectWaittingDevice();
}

//适配器插入消息
bool CBTManager::ProcessAdapterAdded(const btkit_msg_t * pMsg)
{
    if (pMsg == NULL)
    {
        return false;
    }

    // 初始化蓝牙
    InitBluetooth();

    // 写dongle mac地址
    DeviceInfo devInfo;
    _BTDriver.GetBluetoothModuleInfo(devInfo);
    configParser_SetConfigString(kszBluetoothAdapterMac, devInfo.strMac.c_str(), CONFIG_LEVEL_PHONE);

    if (IsBuiltInBluetooth())
    {
        return false;
    }

    idleScreen_DropStatus(PS_STATE_ADAPTER_REMORE);
    if (!m_bEnable)
    {
        idleScreen_SetStatus(PS_STATE_ADAPTER_ADD, "");
    }
    etl_NotifyApp(false, BLUETOOTH_MSG_DONGLE_CHANGE, 1, 0);

    return true;
}

//适配器拔出消息
bool CBTManager::ProcessAdapterRemoved(const btkit_msg_t * pMsg)
{
    if (IsBuiltInBluetooth())
    {
        return false;
    }

    if (pMsg == NULL)
    {
        return false;
    }

    SetPowerByThread(false);
    _BTDriver.DestoryDongle();
    SetDeviceState(DS_NO_DONGLE);

    // 清除dongle mac地址
    configParser_SetConfigString(kszBluetoothAdapterMac, "", CONFIG_LEVEL_PHONE);

    idleScreen_DropStatus(PS_STATE_ADAPTER_ADD);
    idleScreen_SetStatus(PS_STATE_ADAPTER_REMORE, "");
    etl_NotifyApp(false, BLUETOOTH_MSG_DONGLE_CHANGE, 0, 0);

    return true;
}

//处理设备消失
bool CBTManager::ProcessCallbackDeviceDisappeared(const btkit_msg_t * pMsg)
{
    if (NULL == pMsg)
    {
        return false;
    }

    //删除设备
    DeviceInfo * pDeviceinfo = _BTData.GetDeviceInfoByHandle(pMsg->bt_callback_param.dev_hnd);
    if (NULL == pDeviceinfo)
    {
        return false;
    }

    //扫描状态的设备要删除
    if (WIRE_DEVICE_INIT_STATE == pDeviceinfo->eState
            && pDeviceinfo->bScanedDev)
    {
        _BTData.DeleteDevice(pMsg->bt_callback_param.dev_hnd);
        UpdateUI(OP_UPDATE_LIST, false, pDeviceinfo->devHandle);
    }

    return true;
}

//处理设备配置更改
bool CBTManager::ProcessCallbackPropertyChanged(const btkit_msg_t * pMsg)
{
    if (NULL == pMsg)
    {
        return false;
    }

    DeviceInfo * pDeviceInfo = _BTData.GetDeviceInfoByHandle(pMsg->bt_callback_param.dev_hnd);
    if (NULL == pDeviceInfo)
    {
        return false;
    }

    DeviceProperty devProperty;
    if (!_BTDriver.GetDeviceProperty(pMsg->bt_callback_param.dev_hnd, devProperty))
    {
        return false;
    }

    // 如果别名为空，则使用name
    if (0 == strcmp(devProperty.dev_property.alias, ""))
    {
        pDeviceInfo->strName = devProperty.dev_property.name;
    }
    else
    {
        pDeviceInfo->strName = devProperty.dev_property.alias;
    }

    // 通知UI更新该设备的信息
    UpdateUI(OP_UPDATE_DEVICE, true, pDeviceInfo->devHandle);
    WIRE_INFO("device[%d] PropertyChanged, name[%s]", pDeviceInfo->devHandle,
              pDeviceInfo->strName.c_str());

    return true;
}

//主动发起/停止扫描
bool CBTManager::ProcessScanDevice(bool bScan)
{
    WIRE_INFO("%s scan device", bScan ? "start" : "stop");
    if (bScan)
    {
        if (!IsDongleValid()
                || !IsEnable())
        {
            WIRE_WARN("dongle has been removed or bluetooth is closed");
            UpdateUI(OP_SCAN_RESULT, false);
            return false;
        }

        if (IsInScan())
        {
            WIRE_INFO("Already in scaning");
            return true;
        }

        // 扫描前停止连接操作
        //DeleteSession(GetSessionByState(SESSION_PAIRING_STATE), true, false);
        //DeleteSession(GetSessionByState(SESSION_CONNECTING_STATE), true, false);

        if (!InitSession(SESSION_SCAN_STATE))
        {
            UpdateUI(OP_SCAN_RESULT, false);
        }
    }
    else
    {
        CBtSession * pScanSession = GetSessionByState(SESSION_SCAN_STATE);
        if (NULL != pScanSession)
        {
            WirelessSessionData wsData;
            wsData.ulMessage = WIRELESS_OPTION_BT_STOP_SCAN;
            wsData.wParam = true;   // 上层触发的停止, 包括stop, connect触发

            return pScanSession->ProcessOption(wsData);
        }
    }

    return true;
}

//处理接口调用失败
bool CBTManager::DoTaskFailed(BT_TASK_TYPE eType, const BTTaskParam* pParam)
{
    if (NULL == pParam)
    {
        WIRE_WARN("[Task failed][Exec param is null]");
    }

    bool bAutoConnect = false;

    CBtSession * pSession = dynamic_cast<CBtSession *>(_WirelessSessionManager.
                            GetSessionBySessionID(pParam->lSessionID));

    if (NULL == pSession || pSession->GetHandle() != pParam->ulDeviceHandle)
    {
        WIRE_WARN("[Task failed][Not process][Session changed][Session:%d]",
                  pParam->lSessionID);
        return false;
    }

    //非主动发起不需要弹出提示窗
    BTSessionInfo * pSessionInfo = (BTSessionInfo *)pSession->GetSessionInfo();
    if (NULL != pSessionInfo
            && (pSessionInfo->bSponor
                || pSessionInfo->bAutoConnect))
    {
        bAutoConnect = true;
    }

    // 调用驱动失败，只销毁会话，不用再调用接口。
    DeleteSession(GetSessionByHandle(pSession->GetHandle()),
                  (BT_TASK_CONNECT == eType) ? true : false);

    //重新更新蓝牙设备状态
    _BTData.SetDeviceState(pSession->GetHandle(), (eType == BT_TASK_CONNECT) ?
                           WIRE_DEVICE_PAIRED_STATE : WIRE_DEVICE_INIT_STATE);
    if (BT_TASK_PAIR == eType)
    {
        DeviceInfo * pDevInfo = _BTData.GetDeviceInfoByHandle(pSession->GetHandle());
        if (pDevInfo)
        {
            pDevInfo->bPaired = false;
        }
    }

    if (!bAutoConnect)
    {
        OPERATION_RESULT eState = OP_UPDATE_LIST;
        if (BT_TASK_CONNECT == eType)
        {
            eState = OP_CONNECT_RESULT;
        }
        else if (BT_TASK_PAIR == eType)
        {
            eState = OP_PAIR_RESULT;
        }

        UpdateUI(eState, false, pParam->ulDeviceHandle);
    }

    return true;
}

//是否允许连接
bool CBTManager::IsAllowConnect(unsigned long ulDevHandle)
{
    if (BT_DEVICE_HANDLE_INVAILED == ulDevHandle
            || !IsDongleValid()
            || !IsEnable()
            || NULL == _BTData.GetDeviceInfoByHandle(ulDevHandle))
    {
        return false;
    }

    return true;
}

//判断设备是否需要自动连接 ---- 断开原因在net记录
bool CBTManager::IsDevAllowACInDriver(unsigned long ulDevHandle)
{
    DeviceProperty devProperty;
    if (_BTDriver.GetDeviceProperty(ulDevHandle, devProperty))
    {
        return devProperty.dev_property.auto_reconnect == 1;
    }

    return false;
}

//是否已连接
bool CBTManager::IsConnectedSessionExist()
{
    return _WirelessSessionManager.IsInState(BT_SESSION, SESSION_CONNECTED_STATE,
            BT_DEVICE_HANDLE_INVAILED);
}

//是否连接中
bool CBTManager::IsConnectingSessionExist(CSessionBase * pExceptThis/* = NULL*/)
{
    //匹配与连接都算连接中
    return _WirelessSessionManager.IsInState(BT_SESSION, SESSION_CONNECTING_STATE,
            BT_DEVICE_HANDLE_INVAILED, pExceptThis)
           || _WirelessSessionManager.IsInState(BT_SESSION, SESSION_PAIRING_STATE, BT_DEVICE_HANDLE_INVAILED,
                   pExceptThis);
}

//获取已连接设备地址
DeviceInfo * CBTManager::GetCurrentDevice(WIRELESS_SESSION_STATE eState)
{
    CBtSession * pSession = GetSessionByState(eState);
    if (NULL == pSession)
    {
        return false;
    }

    return _BTData.GetDeviceInfoByHandle(pSession->GetHandle());
}

//是否允许自动连接
bool CBTManager::IsAllowAutoConnect()
{
    if (!IsEnable()
            || IsInScan()
            || IsConnectingSessionExist()
            || IsConnectedSessionExist()
       )
    {
        return false;
    }

    bool bAllow = false;
    if (GetAutoConnectStragety() == AC_STRATEGY_SHORT_TIME)
    {
        bAllow = GetCurrentAutoConnectCount() < BT_AUTOCONEECT_COUNT;
    }
    else if (GetAutoConnectStragety() == AC_STRATEGY_LONG_TIME)
    {
        bAllow = _BTData.IsLastDevSupportAC() && IsDevAllowACInDriver(_BTData.GetLastSupportACDevHandle());
    }

    return bAllow;
}

//是否支持某项功能特性
bool CBTManager::IsSupportFeature(BT_FEATURE_SUPPROT eFeature)
{
    bool bSupportFeature = false;

    switch (eFeature)
    {
    case BT_FEATURE_AUDIO:
        bSupportFeature = configParser_GetConfigInt(kszBluetoothSupportTalk);
        break;
    case BT_FEATURE_CONTACT:
#ifdef IF_SUPPORT_BLUETOOTH_CONTACT
        bSupportFeature = true;
#endif
        bSupportFeature |= configParser_GetConfigInt(kszBluetoothSupportContact);
    default:
        break;
    }

    return bSupportFeature;
}

// 是否内置蓝牙
bool CBTManager::IsBuiltInBluetooth()
{
    return PT_T49 == devicelib_GetPhoneType()
           || PT_T54S == devicelib_GetPhoneType()
           || PT_T52 == devicelib_GetPhoneType()
           || PT_CP920 == devicelib_GetPhoneType();
}


void CBTManager::OnTaskActionCallback(NS_TA::CTaskAction* pTaskAction)
{
    CBTAction* pAction = dynamic_cast<CBTAction*>(pTaskAction);
    if (NULL == pAction)
    {
        return;
    }

    WIRE_INFO("[Bt task][Call back][Task:%s][Result:%d][Handle:%d]",
              pAction->GetTaskName(pAction->GetTaskType()).c_str(),
              pAction->GetTaskResult(),
              pAction->GetRelateHandle());

    switch (pAction->GetTaskType())
    {
    case BT_TASK_CONNECT:
        {
            if (0 == pAction->GetTaskResult())
            {
                // do nothing
            }
#if IF_SUPPORT_REPAIR
            else if (BT_DELETE_BY_PHONE == pAction->GetTaskResult())
            {
                WirelessSessionData wsData;
                wsData.ulMessage = WIRELESS_OPTION_BT_DEVICE_REPAIR;
                wsData.lParam = pAction->GetRelateHandle();
                wsData.wParam = BT_DELETE_BY_PHONE;

                _WirelessSessionManager.SendOptionToSessionByHandle(pAction->GetRelateHandle(), wsData, BT_SESSION);
            }
#endif
            else
            {
                //失败
                DoTaskFailed(pAction->GetTaskType(), pAction->GetBtExecParam());
            }
        }
        break;
    case BT_TASK_AUTO_CONNECT:
        {
            if (0 == pAction->GetTaskResult())
            {
                WIRE_INFO("[Bt task][Call back][Auto connect success]");
            }
            else if (BT_DELETE_BY_PHONE == pAction->GetTaskResult())
            {
                WIRE_INFO("[Bt task][Call back][Device removed by phone]");
                SetAutoConnectTimer(false);
            }
            else
            {
                //失败
                DoTaskFailed(pAction->GetTaskType(), pAction->GetBtExecParam());
            }
        }
        break;
    case BT_TASK_PAIR:
        {
            BTTaskParam* pParam = pAction->GetBtExecParam();
            if (NULL == pParam)
            {
                WIRE_WARN("[Bt task][Call back][Exec param is null]");
                break;
            }

            CSessionBase* pSession = _WirelessSessionManager.GetSessionBySessionID(pParam->lSessionID);
            if (NULL == pSession
                    || pSession->GetHandle() != pParam->ulDeviceHandle)
            {
                WIRE_WARN("[Bt task][Call back][Session:%d already changed]", pParam->lSessionID);
                break;
            }

            if (1 == pAction->GetTaskResult())
            {
                // bluez 已经配对，可直接进行连接
                WIRE_INFO("[Bt task][Call back][Pair succeed][Device:%d]");
                WirelessSessionData wsData;
                wsData.ulMessage = WIRELESS_OPTION_BT_CONNECTING;
                pSession->ProcessOption(wsData);
            }
            else if (0 == pAction->GetTaskResult())
            {
                //do nothing
            }
            else
            {
                DoTaskFailed(pAction->GetTaskType(), pAction->GetBtExecParam());
            }
        }
        break;
    case BT_TASK_PRASECONTACT:
        {
            if (1 == pAction->GetTaskResult())
            {
                BTTaskParam* pParam = pAction->GetBtExecParam();
                if (NULL == pParam)
                {
                    break;
                }

                _BTSync.PraseContactSuccess(pParam->strParam);
            }
            else
            {
                _BTSync.ProcessContactFailed();
            }
        }
        break;
    case BT_TASK_DISCONNECT:
        {
            if (0 == pAction->GetTaskResult())
            {
                WIRE_WARN("[Bt task][Call back][Disconnect error]");
            }
        }
        break;
    case BT_TASK_DOWNLOAD_CONTACT_START:
        {
            if (0 == pAction->GetTaskResult())
            {
                WIRE_WARN("[Bt task][Call back][Download contact error]");
                _BTSync.ProcessContactFailed(true);
            }
        }
        break;
    case BT_TASK_POWER_ON:
        {
            if (0 == pAction->GetTaskResult())
            {
                WIRE_WARN("[Bt task][Call back][Power on error]");
            }
            else if (1 == pAction->GetTaskResult())
            {
                if (!IsEnable())
                {
                    DoInit();
                    SetEnable(true);
                    SetDeviceState(DS_OK);
                }
            }
        }
        break;
    case BT_TASK_UNPAIR:
    case BT_TASK_SCAN_START:
    case BT_TASK_SCAN_STOP:
        {
            unsigned long ulDeviceHandle = pAction->GetRelateHandle();
            WirelessSessionData wsData;
            wsData.wParam = pAction->GetTaskResult();      //线程执行结果

#if IF_SUPPORT_REPAIR
            if (BT_TASK_UNPAIR == pAction->GetTaskType())
            {
                // 取消配对后, 需要用户再授权才能同步联系人
                DeviceInfo * pDevInfo = _BTData.GetDeviceInfoByHandle(ulDeviceHandle);
                if (pDevInfo)
                {
                    pDevInfo->bSyncContact = false;
                }

                wsData.lParam = ulDeviceHandle;
                wsData.ulMessage = WIRELESS_OPTION_BT_DEVICE_REPAIR;
                if (_WirelessSessionManager.IsInState(BT_SESSION, SESSION_CONNECTING_STATE, ulDeviceHandle, NULL))
                {
                    _WirelessSessionManager.SendOptionToSessionByHandle(ulDeviceHandle, wsData, BT_SESSION);
                }
            }
#endif
            if (BT_TASK_SCAN_START == pAction->GetTaskType()
                    || BT_TASK_SCAN_STOP == pAction->GetTaskType())
            {
                wsData.lParam = pAction->GetTaskResult();
                wsData.ulMessage = WIRELESS_OPTION_BT_SCAN_RESULT;
                if (_WirelessSessionManager.IsInState(BT_SESSION, SESSION_SCAN_STATE, ulDeviceHandle, NULL))
                {
                    _WirelessSessionManager.SendOptionToSessionByHandle(ulDeviceHandle, wsData, BT_SESSION);
                }
            }
        }
        break;
    default:
        break;
    }
}

//取消连接
bool CBTManager::ProcessUnConnectDevice(unsigned long ulDevHandle, bool bEndConnect/* = true*/)
{
    WIRE_INFO("disconnect device[%d]", ulDevHandle);
    WirelessSessionData wsData;
    wsData.ulMessage = WIRELESS_OPTION_BT_CANCEL;
    wsData.wParam = ulDevHandle;
    return _WirelessSessionManager.SendOptionToSessionByHandle(ulDevHandle, wsData, BT_SESSION);
}

//发起连接
bool CBTManager::ProcessConnectDevice(unsigned long ulDevHandle, bool bAutoConnect /*= false*/)
{
    if (IsInScan())
    {
        ProcessScanDevice(false);
    }

    if (!IsAllowConnect(ulDevHandle))
    {
        UpdateUI(bAutoConnect ? OP_UPDATE_LIST : OP_CONNECT_RESULT, false, ulDevHandle);
        return false;
    }

    //当前设备已经在连接中, 改变连接类型
    if (_WirelessSessionManager.IsInState(BT_SESSION, SESSION_CONNECTING_STATE, ulDevHandle)
            || _WirelessSessionManager.IsInState(BT_SESSION, SESSION_PAIRING_STATE, ulDevHandle))
    {
        SetSessionAutoConnect(ulDevHandle, bAutoConnect);
        //同时需要加入到队列,此时队尾可能已经是别的设备
        _BTData.AddDeviceToWaittingConnectList(ulDevHandle);
        return true;
    }

    //存在其他正在连接中的设备，加入待连接队列
    if (IsConnectingSessionExist())
    {
        _BTData.AddDeviceToWaittingConnectList(ulDevHandle);

        //需要将正在连接的设备设置成自动连接类型, 不再弹出连接失败
        CBtSession * pConnectingSession = GetSessionByState(SESSION_CONNECTING_STATE);
        if (NULL != pConnectingSession)
        {
            SetSessionAutoConnect(pConnectingSession->GetHandle(), true);
        }

        pConnectingSession = GetSessionByState(SESSION_PAIRING_STATE);
        if (NULL != pConnectingSession)
        {
            SetSessionAutoConnect(pConnectingSession->GetHandle(), true);
        }
        return true;
    }

    WIRE_INFO("ready connect device[%d]", ulDevHandle);

    if (!bAutoConnect)
    {
        SetAutoConnectEnable(false);
    }

    //断开已连接的其他蓝牙设备.
    CBtSession * pConnectedSession = GetSessionByState(SESSION_CONNECTED_STATE);
    if (NULL != pConnectedSession
            && pConnectedSession->GetHandle() != ulDevHandle)
    {
        _BTData.AddDeviceToWaittingConnectList(ulDevHandle);
        ProcessUnConnectDevice(pConnectedSession->GetHandle());
        return true;
    }

    bool bConnectResult = false;
    BTSessionInfo * pSessionInfo = new BTSessionInfo;
    pSessionInfo->lDevHandle = ulDevHandle;
    pSessionInfo->bAutoConnect = bAutoConnect;

    //未匹配，需要先进行匹配
    if (!_BTDriver.IsDevicePairedInDriver(ulDevHandle))
    {
        pSessionInfo->bAutoConnect = false;
        bConnectResult = InitSession(SESSION_PAIRING_STATE, ulDevHandle, pSessionInfo);
    }
    else
    {
        if (!_BTDriver.IsDeviceConnectedInDriver(ulDevHandle))
        {
            bConnectResult = InitSession(SESSION_CONNECTING_STATE, ulDevHandle, pSessionInfo);
        }
        else
        {
            //当前已经连接上，直接进入已连接状态
            InitSession(SESSION_CONNECTED_STATE, ulDevHandle, pSessionInfo);
            bConnectResult = true;
        }
    }

    return bConnectResult;
}

//设置是否为自动连接
bool CBTManager::SetSessionAutoConnect(unsigned long ulDeviceHandle, bool bAutoConnect)
{
    WirelessSessionData wsData;
    wsData.ulMessage = WIRELESS_OPTION_UPDATESPONOR;
    wsData.wParam = ulDeviceHandle;
    wsData.lParam = bAutoConnect;

    WIRE_INFO("update[%d] sponor[%d]", ulDeviceHandle, bAutoConnect);

    return _WirelessSessionManager.SendOptionToSessionByHandle(ulDeviceHandle, wsData, BT_SESSION);
}

//设置开关状态
void CBTManager::SetEnable(bool bEnable, bool bSetConfig/* = true*/)
{
    if (m_bEnable == bEnable)
    {
        return;
    }

    m_bEnable = bEnable;

    if (bSetConfig)
    {
        configParser_SetConfigInt(kszBluetoothSwitch, m_bEnable, CONFIG_LEVEL_PHONE);
    }

    WIRE_INFO("set switch [%s]", m_bEnable ? "enable" : "disable");
}

//设置自动连接定时器
bool CBTManager::SetAutoConnectTimer(bool bEnable)
{
    WIRE_INFO("Set auto connect timer [%s], Stragety[%d]", bEnable ? "enable" : "disable",
              GetAutoConnectStragety());

    if (bEnable)
    {
        int iAutoConenctTimeOut = GetAutoConnectStragety() == AC_STRATEGY_SHORT_TIME ?
                                  BT_AUTOCONNECT_SHORT_TIME : BT_AUTOCONNECT_LONG_TIME;
        timerSetThreadTimer(m_iAutoConnectTimer, iAutoConenctTimeOut);
    }
    else
    {
        timerKillThreadTimer(m_iAutoConnectTimer);
    }

    return true;
}

//设置自动连接策略
void CBTManager::SetAutoConnectStragety(AUTOCONNECT_STRATEGY eStragety)
{
    if (GetAutoConnectStragety() == eStragety)
    {
        return;
    }

    m_AutoConnectInfo.eACStrategy = eStragety;
}

//设置自动连接开关
void CBTManager::SetAutoConnectEnable(bool bEnable)
{
    // cp920不需要自动重连
    if (devicelib_GetPhoneType() == PT_CP920)
    {
        return false;
    }

    if (m_AutoConnectInfo.bAutoConnect == bEnable)
    {
        return;
    }

    WIRE_INFO("Set auto connect switch [%s]", bEnable ? "enable" : "disable");
    m_AutoConnectInfo.bAutoConnect = bEnable;
}

//处理自动连接
bool CBTManager::AutoConnectDevice(bool bUpdateIndex /* = false */)
{
    // AC开关为关/不允许AC/AC列表为空, 则关闭定时器
    if (!IsAutoConnectEnable()
            || !IsAllowAutoConnect()
            || _BTData.GetAutoConnectNum() <= 0)
    {
        SetAutoConnectTimer(false);

        // 长时连接策略, 如果不允许重连, 则直接关闭AC
        if (GetAutoConnectStragety() == AC_STRATEGY_LONG_TIME
                && !IsDevAllowACInDriver(_BTData.GetLastSupportACDevHandle()))
        {
            SetAutoConnectEnable(false);
        }
        return false;
    }

    DeviceInfo * deviceInfo = NULL;
    {
        // 获取最后一个连接的设备
        deviceInfo = _BTData.GetDeviceInfoByHandle(_BTData.GetLastSupportACDevHandle());
        if (!deviceInfo || !_BTData.IsDevicePaired(deviceInfo->eState)
                || !_BTData.IsTypeSupportAC(deviceInfo->eType))
        {
            return false;
        }
    }

    if (NULL != deviceInfo)
    {
        m_iAutoConnectCount ++;

        WIRE_INFO("beg auto connect[%s], iIndex[%d], count[%d]", deviceInfo->strMac.c_str(),
                  m_iAutoConnectIndex, GetCurrentAutoConnectCount());
        return ProcessConnectDevice(deviceInfo->devHandle, true);
    }

    return false;
}
bool CBTManager::ProcessChannel(bool bConnected)
{
    //if (!talklogic_SessionExist())
    //}

    //voice_SwitchBTChannel(bConnected);

    return true;
}

bool CBTManager::ProcessKey(msgObject & objMsg)
{
    // 耳麦通道关闭
    if (0 == configParser_GetConfigInt(kszHeadsetPortEnable))
    {
        return false;
    }

    const btkit_msg_t * pMsg = (const btkit_msg_t *)objMsg.GetExtraData();
    if (pMsg == NULL)
    {
        return false;
    }

    switch (pMsg->bt_event)
    {
    case BT_HF_CALL_TERMINATED:
        {
#if IF_BUG_38956
            if (talklogic_IsFoucsInRingSession()
                    && 0 == configParser_GetConfigInt(kszRejectIncomingCallSwitch))
            {
                return false;
            }
#endif
            talklogic_ExitFocusSession();
        }
        break;
    case BT_HF_CALL_REDIAL:
        {
            ProcessCallBackRedial();
        }
        break;
    case BT_HF_ANSWER_REQUEST:
    case BT_HF_CALL_ANSWER:
        {
            // 模拟EHS的处理
            // 在通话中，如果实际物理通道是耳麦，则发送EHS关闭按键，否则发送打开
            // IDLE下都发打开按键，这样的效果是在idle下会将通道切换至耳麦，并进入拨号界面
            // 在通话中，如果耳麦开启，效果是按下通道键
            // 在来电时会接起，不管通道是否在耳麦，效果是通道键接起来电。
            int iKeyCode;
            if (talklogic_SessionExist())
            {
                if (voice_IsBluetoothMode()
                        && !talklogic_IsFoucsInRingSession())
                {
                    iKeyCode = PHONE_KEY_EHS_CLOSE;
                }
                else
                {
                    iKeyCode = PHONE_KEY_EHS_OPEN ;
                }
            }
            else
            {
                iKeyCode = PHONE_KEY_EHS_OPEN;
            }

            commonUnits_SendSimulateKey(DEV_MSG_FUNKEY_EVENT, iKeyCode);
        }
        break;
    default:
        break;
    }
    return true;
}

//处理profile连接状态改变
bool CBTManager::ProcessProfileConnectStatusChange(msgObject & objMsg)
{
    const btkit_msg_t* pMsg = (const btkit_msg_t*)objMsg.GetExtraData();
    if (NULL == pMsg)
    {
        return false;
    }

    // 有profile连接上且当前无连接中、已连接会话, 则进入已连接状态
    unsigned long ulHandle = pMsg->bt_callback_param.dev_hnd;
    if (BT_DEV_PROFILE_CONNECTED == pMsg->bt_event
            && !_WirelessSessionManager.IsInState(BT_SESSION, SESSION_REMOTE_REQ_PAIRING_STATE, ulHandle)
            && !_WirelessSessionManager.IsInState(BT_SESSION, SESSION_CONNECTING_STATE, ulHandle)
            && !_WirelessSessionManager.IsInState(BT_SESSION, SESSION_CONNECTED_STATE, ulHandle))
    {
        BTSessionInfo* pSessionInfo = new BTSessionInfo;
        pSessionInfo->lDevHandle = ulHandle;

        InitSession(SESSION_CONNECTED_STATE, ulHandle, pSessionInfo);
    }

    bool bHandle = true;
    switch (pMsg->bt_callback_param.pf_info.uuid)
    {
    // 手机
    case BT_HF_AGW_UUID:
        _BTSync.OnSyncMsg(objMsg);
        break;
    // 耳机
    case BT_HF_UUID:
        break;
    // 音乐接收端
    case BT_AD_SRC_UUID:
        _BTA2DP.OnA2dpEvent(objMsg);
        break;
    default:
        bHandle = false;
        break;
    }

    return bHandle;
}

//通知UI更新
void CBTManager::UpdateUI(OPERATION_RESULT eState /* = OP_UPDATE_LIST  */,
                          bool bSuccessed /* = true */, unsigned long ulDevHandle/* = BT_DEVICE_HANDLE_INVAILED*/)
{
    yl::string strMac = "";
    if (BT_DEVICE_HANDLE_INVAILED != ulDevHandle)
    {
        DeviceInfo * pDevice = _BTData.GetDeviceInfoByHandle(ulDevHandle);
        if (NULL != pDevice)
        {
            strMac = pDevice->strMac;
        }
    }

    WIRE_INFO("Update UI, state = %d, success = %d, device = %s", eState, bSuccessed, strMac.c_str());
    etl_NotifyAppEx(false, BLUETOOTH_MSG_UPDATE_LIST, eState, bSuccessed ? 1 : 0, strMac.length() + 1,
                    strMac.c_str());
}

const char * CBTManager::SessionStateToString(WIRELESS_SESSION_STATE state)
{
    for (int i = 0; test_sessionstate_name_map[i].txt != NULL; i++)
    {
        if (state == test_sessionstate_name_map[i].type)
        {
            return test_sessionstate_name_map[i].txt;
        }
    }
    return NULL;
}

void CBTManager::SetBtLogLevel(int iModuleID, int iLogLevel)
{
    _BTDriver.SetDriverLogLevel(iLogLevel);
}

bool CBTManager::ProcessPinConfirm(msgObject & objMsg)
{
    const btkit_msg_t* pMsg = (const btkit_msg_t*)objMsg.GetExtraData();
    if (!pMsg)
    {
        return false;
    }

    unsigned long ulHandle = pMsg->bt_callback_param.pin_info.dev_hnd;
    WIRE_INFO("Process device pin confirm, dev handle[%d]", ulHandle);

#ifdef IF_BLUETOOTH_SUPPORT_REMOTE_PAIR_REQ
    // 1. 如果设备已经在配对状态, 则传递msg
    if (IsInPair(ulHandle)
            && SendCallbackMsgToSession(ulHandle, objMsg))
    {
        return true;
    }

    // 2. 如果存在配对且不是ulHandle, 则拒绝
    if (IsInPair()
            && !IsInPair(ulHandle))
    {
        WIRE_INFO("Pair operation exist, refuse pair request");
        _BTDriver.PinConfirm(ulHandle, false);
        return false;
    }

    // 3. 创建对端设备配对会话
    if (!InitSession(SESSION_REMOTE_REQ_PAIRING_STATE, ulHandle))
    {
        WIRE_WARN("Init session fail, refuse pair request");
        _BTDriver.PinConfirm(ulHandle, false);
        return false;
    }

    // 4. 对端请求配对则停止扫描
    if (IsInScan())
    {
        ProcessScanDevice(false);
    }
#endif

    return SendCallbackMsgToSession(ulHandle, objMsg);
}

yl::string GetBtMsgName(unsigned long lMsg)
{
    switch (lMsg)
    {
        _MYCASE(CONFIG_MSG_BCAST_CHANGED);
        _MYCASE(BLUETOOTH_MSG_REQ_PIN);
        _MYCASE(BLUETOOTH_MSG_GETSTATUS);
    default:
        break;
    }
    return "";
}

yl::string GetCallbackEventName(unsigned long lMsg)
{
    switch (lMsg)
    {
        _MYCASE(BT_ADAPTER_ADDED);
        _MYCASE(BT_ADAPTER_REMOVED);
        _MYCASE(BT_FOUND_DEV);
        _MYCASE(BT_REQ_PIN);
        _MYCASE(BT_PIN_CONFIRM);
        _MYCASE(BT_PAIR_CANCEL);
        _MYCASE(BT_AUTHORIZE);
        _MYCASE(BT_POWER_ON);
        _MYCASE(BT_POWER_OFF);
        _MYCASE(BT_DEV_DISCONNECT_REQUESTED);
        _MYCASE(BT_DEV_PROPERTY_CHANGED);
        _MYCASE(BT_FOUND_DEV_DISAPPEARED);
        _MYCASE(BT_DEV_PAIR_CHANGED);
        _MYCASE(BT_DEV_DISCONNECTED);
        _MYCASE(BT_DEV_CONNECTED_FAILE);
        _MYCASE(BT_DEV_CONNECTED_CANCEL);
        _MYCASE(BT_HF_ANSWER_REQUEST);
        _MYCASE(BT_HF_CALL_TERMINATED);
        _MYCASE(BT_HF_CALL_REDIAL);
        _MYCASE(BT_HF_CALL_ANSWER);
        _MYCASE(BT_HF_SPK_GAIN_CHANGED);
        _MYCASE(BT_HF_MIC_GAIN_CHANGED);
        _MYCASE(BT_DEV_CONNECTED);
        _MYCASE(BT_PBA_DOWNLOAD_FAIL);
        _MYCASE(BT_PBA_DOWNLOAD_COMPLETE);
        _MYCASE(BT_AG_INDICATOR_CHANGED);
        _MYCASE(BT_MB_CALL_STATUS_ESTABLISHED);
        _MYCASE(BT_AG_CALL_STATUS_CHANGED);
        _MYCASE(BT_AG_SCO_PLAY_START);
        _MYCASE(BT_AG_SCO_PLAY_STOP);
        _MYCASE(BT_HF_SCO_PLAY_STOP);
        _MYCASE(BT_HF_SCO_PLAY_START);
        _MYCASE(BT_DEV_PROFILE_CONNECTED);
        _MYCASE(BT_DEV_PROFILE_CONNECT_FAIL);
        _MYCASE(BT_DEV_PROFILE_DISCONNECTED);
        _MYCASE(BT_A2DP_PLAY_START);
        _MYCASE(BT_A2DP_PLAY_STOP);
    default:
        break;
    }
    return "";
}

void PrintBtMsg(msgObject & objMessage)
{
    yl::string strMsgName = GetBtMsgName(objMessage.message);
    if (!strMsgName.empty())
    {
        WIRE_INFO("Recieve Bluetooth Message %s:[%d] wapram[%d] lparam[%d]", strMsgName.c_str(),
                  objMessage.message, objMessage.wParam, objMessage.lParam);
    }
}
#endif // IF_SUPPORT_BLUETOOTH
