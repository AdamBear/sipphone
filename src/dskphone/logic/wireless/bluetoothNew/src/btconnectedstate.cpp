#ifdef IF_SUPPORT_BLUETOOTH
#include "bluetooth_inc.h"

CBTConnectedState::CBTConnectedState(void):
    m_bIsPlay(false)
{
    m_eState = SESSION_CONNECTED_STATE;
}

CBTConnectedState::~CBTConnectedState(void)
{
}

//创建后处理
bool CBTConnectedState::AfterStateCreate(long ulDeviceHandle, void * pUserData)
{
    m_lDeviceHandle = ulDeviceHandle;

    if (!ProcessConnected(m_lDeviceHandle))
    {
        DisconnectDevice(m_lDeviceHandle);
        return false;
    }

    return true;
}

//退出前处理
bool CBTConnectedState::BeforeStateTransfer(bool bEndConnect/* = false*/)
{
    //先退出联动模式

    //处理断开连接
    ProcessDisconnected();

    //已经不在连接状态时，直接返回
    if (!bEndConnect
            /*|| !_BTDriver.IsDeviceConnectedInDriver(m_lDeviceHandle)*/)
    {
        return true;
    }

    //断开设备
    if (!DisconnectDevice(m_lDeviceHandle))
    {
        WIRE_ERR("Disconnect paired hs device[%d] error", m_lDeviceHandle);
        return false;
    }

    return true;
}

bool CBTConnectedState::ProcessOption(const WirelessSessionData & wsData)
{
    bool bHandle = true;
    switch (wsData.ulMessage)
    {
    case WIRELESS_OPTION_BT_CANCEL:
        {
            DisconnectDevice(wsData.wParam);
        }
        break;
    case WIRELESS_OPTION_BT_HEADSET_EVENT:
        {
            bHandle = ProcessHeadSetEvent(wsData);
        }
        break;
    default:
        bHandle = false;
        break;
    }

    return bHandle;
}

//消息处理
bool CBTConnectedState::ProcessCallbackEvent(msgObject & objMsg)
{
    if (NULL == objMsg.GetExtraData())
    {
        return false;
    }

    const btkit_msg_t * pMsg = (const btkit_msg_t *)objMsg.GetExtraData();
    bool bHandle = true;
    switch (pMsg->bt_event)
    {
    case BT_DEV_DISCONNECTED:
        {
            ExitState(true);
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

//处理连接成功
bool CBTConnectedState::ProcessConnected(unsigned long ulDeviceHandle)
{
    _BTData.SetDeviceState(ulDeviceHandle, WIRE_DEVICE_CONNECTED_STATE);

    unsigned long ulWaittingDevice = _BTData.GetWaittingDevice();

    //等待队列是自己，不再重复连接
    if (ulWaittingDevice != ulDeviceHandle
            && _BTManager.ConnectWaittingDevice(ulWaittingDevice))
    {
        return true;
    }

    DeviceInfo * pDeviceInfo = _BTData.GetDeviceInfoByHandle(ulDeviceHandle);

    // 连接成功次数统计
    if (pDeviceInfo)
    {
        pDeviceInfo->uConnectSuccessTimes ++;
        WIRE_INFO("Connect successful times[%d]", pDeviceInfo->uConnectSuccessTimes);
    }

    _BTManager.UpdateUI(OP_CONNECT_RESULT, true, ulDeviceHandle);
    SetChannel(true);

    // 设置auto connect
    _BTData.SetLastSupportACDevHandle(ulDeviceHandle);
    _BTManager.SetAutoConnectEnable(false);
    _BTData.SaveBluetoothConfig();

    // 通知蓝牙设备连接上
    etl_NotifyApp(false, BLUETOOTH_MSG_CONNECT_STATE, 1, _BTData.GetDeviceType(ulDeviceHandle));

    // 第一次连接手机成功需要通知UI是否自动同步联系人
    // 有可能配对成功, 但是没有连接成功
    if (pDeviceInfo && _ISBTPHONE(pDeviceInfo->eType)
            && pDeviceInfo->uConnectSuccessTimes == 1)
    {
        etl_NotifyAppEx(false, BLUETOOTH_MSG_REQ_SYNC,
                        pDeviceInfo->eType, pDeviceInfo->devHandle,
                        pDeviceInfo->strMac.size() + 1, (void *)pDeviceInfo->strMac.c_str());
    }

    return true;
}

//断开设备
bool CBTConnectedState::DisconnectDevice(unsigned long ulDeviceHandle)
{
    if (NULL == m_pSession)
    {
        return false;
    }

    BTTaskParam sParam;
    sParam.lSessionID = m_pSession->GetSessionID();
    return WLAsyncHelper_BtAsync(BT_TASK_DISCONNECT, ulDeviceHandle, &sParam);
}

// 处理声音通道
void CBTConnectedState::SetChannel(bool bOpen)
{
    // 蓝牙手机不需要设置通道
    DeviceInfo * pDevice = _BTData.GetDeviceInfoByHandle(m_lDeviceHandle);
    if (NULL != pDevice
            && _ISBTPHONE(pDevice->eType))
    {
        return ;
    }

    WIRE_INFO("voice_IsBluetoothMode[%d] talklogic_SessionExist[%d]", voice_IsBluetoothMode(),
              talklogic_SessionExist());

    if (bOpen)
    {
        //通道切换之前设置耳机类型为HEADSET_BT
        voice_SetHeadsetDevice(HEADSET_BT, bOpen);
        if (talklogic_SessionExist())
        {
            // 多路通话则耳机不响铃
            bool bAllowPlayRing = talklogic_IsAllSessionRinging();
            if (talkLogic_IsSessionInTalking())
            {
                _BTManager.HeadSetDeviceEvent(HEADSET_ANSWER_CALL);
            }
            else if (bAllowPlayRing)
            {
                _BTManager.HeadSetDevicePlayRing(true);
            }

            // 切换声道vpm会调用play接口,导致耳机播放话机声音,话机ring状态只需响铃,不必开启蓝牙声道
            if (!bAllowPlayRing)
            {
                voice_OpenBluetoothChannel();
            }

            if (!m_bIsPlay)
            {
                _BTManager.HeadSetDeviceEvent(HEADSET_PLAY, true);
            }
        }
    }
    else
    {
        if (talklogic_SessionExist()
                && voice_IsBluetoothMode())
        {
            voice_SwitchChannel(CHANNEL_HANDFREE);
        }

        //退出时如果还在play状态，需要调用一次
        if (m_bIsPlay)
        {
            _BTManager.HeadSetDeviceEvent(HEADSET_PLAY, false);
        }

        // 通道切换之后移除耳机类型
        voice_SetHeadsetDevice(HEADSET_BT, bOpen);
    }
}

bool CBTConnectedState::ExitState(bool bEndConnect, bool bConnectWattingDevice /* = true */)
{
    bool bAutoConnect = (NULL == m_pSessionInfo) ? false : m_pSessionInfo->bAutoConnect;
    bool bUserCancel = (NULL == m_pSessionInfo) ? false : m_pSessionInfo->bUserCancel;
    unsigned long ulDeviceHandle = m_lDeviceHandle;

    DeviceInfo * pDevice = _BTData.GetDeviceInfoByHandle(ulDeviceHandle);
    if (NULL == pDevice)
    {
        return false;
    }

    CBaseState::ExitState(bEndConnect);

    if ((_BTData.GetWaittingDeviceCount() <= 0)
            && bUserCancel)
    {
        _BTManager.UpdateUI(OP_UPDATE_LIST, true, ulDeviceHandle);
    }
    else
    {
        _BTManager.UpdateUI(OP_DISCONNECT_RESULT, true, ulDeviceHandle);
    }

    return true;
}

//处理断开连接
bool CBTConnectedState::ProcessDisconnected()
{
    _BTData.SetDeviceState(m_lDeviceHandle, WIRE_DEVICE_DISCONNECT_STATE);

    SetChannel(false);

    // 设置自动重连策略和开关
    _BTManager.SetAutoConnectStragety(AC_STRATEGY_LONG_TIME);
    _BTManager.SetAutoConnectEnable(true);

    etl_NotifyApp(false, BLUETOOTH_MSG_CONNECT_STATE, 0, _BTData.GetDeviceType(m_lDeviceHandle));

    return true;
}

bool CBTConnectedState::ProcessHeadSetEvent(const WirelessSessionData & wsData)
{
    const char * arr_strHeasSetEvent[] = {"PLAY", "CALL_OUT", "CALL_IN", "ANSWER_CALL", "END_CALL"};

    WIRE_INFO("Headset event[%s], choice[%d]", arr_strHeasSetEvent[wsData.wParam], wsData.lParam);

    BTTaskParam sParam;
    bool bHandle = true;

    switch (wsData.wParam)
    {
    case HEADSET_PLAY:
        {
            if (wsData.lParam == 1
                    && !m_bIsPlay)
            {
                _BTDriver.Play(m_lDeviceHandle, true);
                m_bIsPlay = true;
            }
            else if (wsData.lParam == 0
                     && m_bIsPlay)
            {
                _BTDriver.Play(m_lDeviceHandle, false);
                m_bIsPlay = false;
            }
        }
        break;
    case HEADSET_CALL_IN:
        {
            if (NULL != wsData.pData)
            {
                sParam.wParam = HEADSET_CALL_IN;
                sParam.strParam = (const char *)wsData.pData;
                WLAsyncHelper_BtAsync(BT_TASK_HEADSET_EVENT, m_lDeviceHandle, &sParam);
            }
        }
        break;
    case HEADSET_CALL_OUT:
    case HEADSET_ANSWER_CALL:
        {
            // http://10.2.1.199/Bug.php?BugID=92816
            // 不管是会议还是普通通话, 在耳机上的表现就是一路
            if (talklogic_GetSessionNum() > 1)
            {
                WIRE_INFO("Call end interface before HEADSET_CALL_OUT/HEADSET_ANSWER_CALL.");
                sParam.wParam = HEADSET_END_CALL;
                WLAsyncHelper_BtAsync(BT_TASK_HEADSET_EVENT, m_lDeviceHandle, &sParam);
            }

            if (HEADSET_CALL_OUT == wsData.wParam
                    && NULL != wsData.pData)
            {
                sParam.wParam = HEADSET_CALL_OUT;
                sParam.strParam = (const char *)wsData.pData;
                WLAsyncHelper_BtAsync(BT_TASK_HEADSET_EVENT, m_lDeviceHandle, &sParam);
            }
            else if (HEADSET_ANSWER_CALL == wsData.wParam)
            {
                sParam.wParam = HEADSET_ANSWER_CALL;
                WLAsyncHelper_BtAsync(BT_TASK_HEADSET_EVENT, m_lDeviceHandle, &sParam);
            }
        }
        break;
    case HEADSET_END_CALL:
        {
            sParam.wParam = HEADSET_END_CALL;
            WLAsyncHelper_BtAsync(BT_TASK_HEADSET_EVENT, m_lDeviceHandle, &sParam);

            if (talklogic_GetSessionNum(false) > 1)
            {
                WIRE_INFO("Call answer interface after HEADSET_END_CALL.");

                sParam.wParam = HEADSET_ANSWER_CALL;
                WLAsyncHelper_BtAsync(BT_TASK_HEADSET_EVENT, m_lDeviceHandle, &sParam);
            }
        }
        break;
    default:
        bHandle = false;
        break;
    }
    return bHandle;
}
#endif // IF_SUPPORT_BLUETOOTH
