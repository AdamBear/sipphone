#ifdef IF_SUPPORT_BLUETOOTH
#include "bluetooth_inc.h"

#ifdef IF_BT_CONNECTING_OVERTIME
#define BT_CONNECTING_TIMER 30*1000
#endif // IF_BT_CONNECTING_OVERTIME

CBTConnectingState::CBTConnectingState(void)
{
    m_eState = SESSION_CONNECTING_STATE;
}

CBTConnectingState::~CBTConnectingState(void)
{

}

bool CBTConnectingState::AfterStateCreate(long ulDeviceHandle, void * pUserData)
{
    m_lDeviceHandle = ulDeviceHandle;

    DeviceInfo * pDevice = _BTData.GetDeviceInfoByHandle(ulDeviceHandle);
    if (NULL == pDevice)
    {
        ExitState();
        return false;
    }

    bool bIsPhone = _ISBTPHONE(pDevice->eType);

    //step 1. 检查设备状态
    bool bAlreadyConnectting = false;
    DeviceProperty devProperty;
    if (_BTDriver.GetDeviceProperty(m_lDeviceHandle, devProperty)
            && !bIsPhone)
    {
#warning SUBTODO
        //WIRE_INFO("device property state[%s]", devProperty.dev_property.hs_property.state);
        //if (strcmp("connecting", devProperty.dev_property.hs_property.state) == 0)
        //{
        //    bAlreadyConnectting = true;
        //}
    }

    //Step 2.耳机主动连接，直接回复确认
    if (NULL != m_pSessionInfo
            && m_pSessionInfo->bSponor
            && !bIsPhone)
    {
        _BTDriver.AnswerAuth(m_lDeviceHandle, true);
    }

    bool bResult = true;

    //step 3. 判断是否需要调用连接接口
    if (!bAlreadyConnectting
            && (NULL != m_pSessionInfo && !m_pSessionInfo->bSponor)
            || bIsPhone)
    {
        bResult = ConnectPairedDevice(ulDeviceHandle);
    }

    if (!bResult)
    {
        ExitState();
        return false;
    }

    _BTData.SetDeviceState(ulDeviceHandle, WIRE_DEVICE_CONNECTING_STATE);

    //通知蓝牙连接中
    etl_NotifyApp(FALSE, BLUETOOTH_MSG_CONNECT_STATE, 2, pDevice->eType);

#if IF_BT_CONNECTING_OVERTIME
    SetTimer(true, BT_CONNECTING_TIMER);
#endif //IF_BT_CONNECTING_OVERTIME

    return true;
}

bool CBTConnectingState::DisconnectDevice(unsigned long ulDeviceHandle)
{
    if (NULL == m_pSession)
    {
        return false;
    }

    BTTaskParam sParam;
    sParam.lSessionID = m_pSession->GetSessionID();
    return WLAsyncHelper_BtAsync(BT_TASK_DISCONNECT, ulDeviceHandle, &sParam);
}

//退出状态前处理
bool CBTConnectingState::BeforeStateTransfer(bool bEndConnect /* = false */)
{
#if IF_BT_CONNECTING_OVERTIME
    SetTimer(false, BT_CONNECTING_TIMER);
#endif // IF_BT_CONNECTING_OVERTIME

    //可能此时驱动已经连接上了，消息还没上报上来
    if (bEndConnect)
    {
        //if (_BTDriver.IsDeviceConnectedInDriver(m_lDeviceHandle))
        {
            DisconnectDevice(m_lDeviceHandle);
        }
    }

    //通知连接失败结果
    DeviceInfo * pDevice = _BTData.GetDeviceInfoByHandle(m_lDeviceHandle);
    if (!_BTDriver.IsDeviceConnectedInDriver(m_lDeviceHandle)
            && NULL != pDevice)
    {
        etl_NotifyApp(FALSE, BLUETOOTH_MSG_CONNECT_STATE, 0, pDevice->eType);
    }

    _BTData.SetDeviceState(m_lDeviceHandle, WIRE_DEVICE_DISCONNECT_STATE);
    return true;
}

#if IF_BT_CONNECTING_OVERTIME
//定时器处理
bool CBTConnectingState::ProcessTimer(int iTimerID)
{
    if (m_iTimer == iTimerID)
    {
        WIRE_INFO("connect[%d] over time", m_lDeviceHandle);
        //DisconnectDevice(m_lDeviceHandle);
        // 连接底层没响应, 超时则退出
        ExitState(true);
        return true;
    }

    return false;
}
#endif // IF_BT_CONNECTING_OVERTIME

//处理操作
bool CBTConnectingState::ProcessOption(const WirelessSessionData & wsData)
{
    bool bHandle = true;
    switch (wsData.ulMessage)
    {
    case WIRELESS_OPTION_UPDATESPONOR:
        {
            if (NULL != m_pSessionInfo)
            {
                m_pSessionInfo->bAutoConnect = wsData.lParam;
            }
        }
        break;
    case WIRELESS_OPTION_BT_CANCEL:
        {
            if (NULL != m_pSessionInfo)
            {
                m_pSessionInfo->bUserCancel = true;
            }
            DisconnectDevice(wsData.wParam);
        }
        break;
#if IF_SUPPORT_REPAIR
    case WIRELESS_OPTION_BT_DEVICE_REPAIR:
        {
            if (0 == wsData.wParam)
            {
                WIRE_INFO("unpair thread failed.");
                ExitState();
            }
            else if (1 == wsData.wParam)
            {
                WIRE_INFO("unpair thread success.");
                RepairPhoneDevice();
            }
            else if (BT_DELETE_BY_PHONE == wsData.wParam)
            {
                if (NULL != m_pSessionInfo
                        && m_pSessionInfo->bAutoConnect)
                {
                    return ExitState(true);
                }

                WIRE_INFO("device removed by mobile,unpair phone device,dev_handle[%d]", m_lDeviceHandle);

                BTTaskParam sParam;
                sParam.lSessionID = m_pSession->GetSessionID();
                WLAsyncHelper_BtAsync(BT_TASK_UNPAIR, m_lDeviceHandle, &sParam);
            }
        }
        break;
#endif
    default:
        bHandle = false;
        break;
    }

    return bHandle;
}

//回调事件处理
bool CBTConnectingState::ProcessCallbackEvent(msgObject & objMsg)
{
    if (NULL == objMsg.GetExtraData())
    {
        return false;
    }

    const btkit_msg_t * pMsg = (const btkit_msg_t *)objMsg.GetExtraData();
    bool bHandle = true;
    switch (pMsg->bt_event)
    {
    case BT_DEV_CONNECTED_CANCEL:
    case BT_DEV_CONNECTED_FAILE:
        {
            ExitState(true);
        }
        break;
    case BT_DEV_CONNECTED:
        {
            //进入已连接状态
            if (NULL != m_pSession)
            {
                m_pSession->EnterState(SESSION_CONNECTED_STATE, pMsg->bt_callback_param.dev_hnd,
                                       (void *)m_pSessionInfo);
            }
        }
        break;
    case BT_AUTHORIZE:
        {
            //直接调用接口回复确认
            WIRE_INFO("answer device[%d] authorize", pMsg->bt_callback_param.dev_hnd);
            _BTDriver.AnswerAuth(pMsg->bt_callback_param.dev_hnd, true);
        }
        break;
#if IF_SUPPORT_REPAIR
    case BT_REQ_PIN:
        {
            UnpairPhoneDevice();
        }
        break;
#endif
    default:
        {
            bHandle = false;
        }
        break;
    }
    return bHandle;
}

// 开始连接
bool CBTConnectingState::StartConnectDevice(unsigned long ulDeviceHandle)
{
    if (NULL == m_pSession)
    {
        return false;
    }

    BT_TASK_TYPE eType = m_pSessionInfo && m_pSessionInfo->bAutoConnect ?
                         BT_TASK_AUTO_CONNECT : BT_TASK_CONNECT;
    BTTaskParam sParam;
    sParam.lSessionID = m_pSession->GetSessionID();
    return WLAsyncHelper_BtAsync(eType, ulDeviceHandle, &sParam);
}

//连接已匹配设备
bool CBTConnectingState::ConnectPairedDevice(unsigned long ulDeviceHandle)
{
    if (!_BTManager.IsDongleValid())
    {
        return false;
    }

    WIRE_INFO("connect paired hs device[%d]", ulDeviceHandle);

    return StartConnectDevice(ulDeviceHandle);
}

bool CBTConnectingState::ExitState(bool bEndConnect, bool bConnectWattingDevice /* = true */)
{
    //ExitState 会析构类，内存空间已被释放,类成员变量值为不可预期。所以需要临时变量先保存
    bool bAutoConnect = (NULL == m_pSessionInfo) ? false : m_pSessionInfo->bAutoConnect;
    bool bUserCancel = (NULL == m_pSessionInfo) ? false : m_pSessionInfo->bUserCancel;
    unsigned long ulDeviceHandle = m_lDeviceHandle;

    DeviceInfo * pDevice = _BTData.GetDeviceInfoByHandle(ulDeviceHandle);
    if (NULL == pDevice)
    {
        return false;
    }

    CBaseState::ExitState(bEndConnect);

    // 1. 等待队列的设备还是自己,没有必要再进行一次连接。
    // 2. 连接等待队列的时候,不需要通知当前设备连接结果。
    // 3. 非主动发起的连接，因为不会显示连接中，所以也没必要显示连接失败。

    bool bUpdate = true;
    unsigned long ulWaittingDevice = _BTData.GetWaittingDevice();

    if (bConnectWattingDevice
            && ulDeviceHandle != ulWaittingDevice)
    {
        bUpdate = !_BTManager.ConnectWaittingDevice(ulWaittingDevice);
    }

    if (bUpdate)
    {
        //自动连接失败和取消连接只更新界面,
        if (bAutoConnect || bUserCancel)
        {
            _BTManager.UpdateUI(OP_UPDATE_LIST, true, ulDeviceHandle);
        }
        else
        {
            _BTManager.UpdateUI(OP_CONNECT_RESULT, false, ulDeviceHandle);
        }
    }

    return true;
}

#if IF_SUPPORT_REPAIR
//设备置为未匹配
bool CBTConnectingState::UnpairPhoneDevice()
{
    if (NULL != m_pSessionInfo
            && m_pSessionInfo->bAutoConnect)
    {
        //自动连接直接取消连接即可
        return ExitState(true);
    }

    WIRE_INFO("device removed by mobile,unpair phone device");

    BTTaskParam sParam;
    sParam.lSessionID = m_pSession->GetSessionID();
    return WLAsyncHelper_BtAsync(BT_TASK_UNPAIR, m_lDeviceHandle, &sParam);
}

//设备重新进入匹配流程
bool CBTConnectingState::RepairPhoneDevice()
{
    if (NULL != m_pSession
            && NULL != m_pSessionInfo)
    {
        m_pSessionInfo->bRePair = true;
        m_pSession->EnterState(SESSION_PAIRING_STATE, m_lDeviceHandle, (void *)m_pSessionInfo);
    }

    return true;
}
#endif

#endif // IF_SUPPORT_BLUETOOTH
