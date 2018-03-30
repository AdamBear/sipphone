#ifdef IF_SUPPORT_BLUETOOTH
#include "bluetooth_inc.h"
#include "commonapi/stringhelperapi.h"
#include <time.h>

#if IF_BT_CONNECTING_OVERTIME
#define  BT_PAIR_OVERTIMER   30*1000
#endif

CBTPairState::CBTPairState(void)
{
    m_eState = SESSION_PAIRING_STATE;
    m_bPinConfirm = false;
}

CBTPairState::~CBTPairState(void)
{
    if (!_BTData.IsDevicePaired(_BTData.GetDeviceState(m_lDeviceHandle)))
    {
        _BTData.SetDeviceState(m_lDeviceHandle, WIRE_DEVICE_INIT_STATE);
    }
}

bool CBTPairState::AfterStateCreate(long ulDeviceHandle, void * pUserData)
{
    m_lDeviceHandle = ulDeviceHandle;

    if (!CreatePairDevice(m_lDeviceHandle))
    {
        ExitState();
        return false;
    }

#if IF_BT_CONNECTING_OVERTIME
    SetTimer(true, BT_PAIR_OVERTIMER);
#endif // IF_BT_CONNECTING_OVERTIME

    return true;
}

bool CBTPairState::BeforeStateTransfer(bool bEndConnect/* = false*/)
{
#if IF_BT_CONNECTING_OVERTIME
    SetTimer(false, BT_PAIR_OVERTIMER);
#endif // IF_BT_CONNECTING_OVERTIME

    //可能是被取消，手动调用取消配对，否则会导致下次匹配失败。
    if (bEndConnect)
    {
        return CancelPair(m_lDeviceHandle);
    }

    return true;
}

bool CBTPairState::CancelPair(unsigned long ulDeviceHandle)
{
    if (NULL == m_pSession)
    {
        return false;
    }

    DeviceInfo * pDevice = _BTData.GetDeviceInfoByHandle(ulDeviceHandle);
    if (NULL == pDevice)
    {
        return false;
    }

    if (_ISBTPHONE(pDevice->eType))
    {
        return _BTDriver.PinConfirm(ulDeviceHandle, false);
    }
    else if (_ISBTHEADSET(pDevice->eType))
    {
        BTTaskParam sParam;
        sParam.lSessionID = m_pSession->GetSessionID();
        WLAsyncHelper_BtAsync(BT_TASK_CANCLEPAIR, ulDeviceHandle, &sParam);
    }

    return true;
}

bool CBTPairState::ProcessOption(const WirelessSessionData & wsData)
{
    bool bHandle = true;
    switch (wsData.ulMessage)
    {
    case WIRELESS_OPTION_BT_ANSWERPIN:
        {
            if (1 == wsData.lParam)
            {
                m_pSessionInfo->bUserCancel = true;
            }
            ProcessAnswerPin((const AuthPinInfo *)wsData.pData);
        }
        break;
    case WIRELESS_OPTION_BT_CONNECTING:
        {
            if (NULL != m_pSession)
            {
                m_pSession->EnterState(SESSION_CONNECTING_STATE, m_lDeviceHandle, m_pSessionInfo);
            }
        }
        break;
    case WIRELESS_OPTION_UPDATESPONOR:
        {
            if (NULL != m_pSessionInfo)
            {
                m_pSessionInfo->bAutoConnect = wsData.lParam;
            }
        }
        break;
    default:
        bHandle = false;
        break;
    }

    return bHandle;
}

bool CBTPairState::ProcessCallbackEvent(msgObject & objMsg)
{
    bool bHandle = true;
    if (NULL == objMsg.GetExtraData())
    {
        return false;
    }
    const btkit_msg_t * pMsg = (const btkit_msg_t *)objMsg.GetExtraData();
    switch (pMsg->bt_event)
    {
    case BT_REQ_PIN:
        {
            if (!ProcessRequestPin(pMsg->bt_callback_param.dev_hnd))
            {
                CancelPair(pMsg->bt_callback_param.dev_hnd);
            }
        }
        break;
    case BT_PIN_CONFIRM:
        {
            if (!ProcessRequestPin(pMsg->bt_callback_param.pin_info.dev_hnd,
                                   pMsg->bt_callback_param.pin_info.pin))
            {
                CancelPair(pMsg->bt_callback_param.pin_info.dev_hnd);
            }
        }
        break;
    case BT_DEV_PAIR_CHANGED:
        {
            if (!ProcessPairChange(pMsg))
            {
                ExitState();
            }
        }
        break;
    case BT_PAIR_CANCEL:
        {
            ExitState();
        }
        break;
    default:
        {
            bHandle = false;
        }
    }
    return bHandle;
}

bool CBTPairState::StartPairDevice(unsigned long ulDeviceHandle)
{
    if (NULL == m_pSession)
    {
        return false;
    }

    BTTaskParam sParam;
    sParam.lSessionID = m_pSession->GetSessionID();
    return WLAsyncHelper_BtAsync(BT_TASK_PAIR, ulDeviceHandle, &sParam);
}

bool CBTPairState::CreatePairDevice(unsigned long ulDeviceHandle)
{
    if (!_BTManager.IsDongleValid())
    {
        return false;
    }

    WIRE_INFO("start paird device[%d]", ulDeviceHandle);
    if (!StartPairDevice(ulDeviceHandle))
    {
        return false;
    }

    _BTData.SetDeviceState(ulDeviceHandle, WIRE_DEVICE_PAIRING_STATE);
    return true;
}

bool CBTPairState::ProcessAnswerPin(const AuthPinInfo * pAuthInfo)
{
    if (NULL == pAuthInfo)
    {
        return false;
    }

    if (!pAuthInfo->bConfirmPair)
    {
        CancelPair(pAuthInfo->ulDeviceHandle);
        return false;
    }

    DeviceInfo * pDevice = _BTData.GetDeviceInfoByHandle(pAuthInfo->ulDeviceHandle);
    if (NULL == pDevice
            || ! AnswerPin(pDevice->devHandle, pAuthInfo->szPin))
    {
        CancelPair(pAuthInfo->ulDeviceHandle);
        return false;
    }

    return true;
}

#if IF_BT_CONNECTING_OVERTIME
//定时器处理
bool CBTPairState::ProcessTimer(int iTimerID)
{
    if (m_iTimer == iTimerID)
    {
        WIRE_INFO("pair[%d] over time", m_lDeviceHandle);
        //CancelPair(m_lDeviceHandle);
        // 配对底层没响应, 超时则退出
        ExitState(true);
        return true;
    }
    return false;
}
#endif // IF_BT_CONNECTING_OVERTIME

bool CBTPairState::ProcessPairChange(const btkit_msg_t * pMsg)
{
    if (NULL == pMsg)
    {
        return false;
    }

    DeviceInfo * pDeviceInfo = _BTData.GetDeviceInfoByHandle(pMsg->bt_callback_param.pair_info.dev_hnd);

    if (NULL == pDeviceInfo
            || NULL == m_pSession)
    {
        return false;
    }

    bool bPairedSucc = (pMsg->bt_callback_param.pair_info.paired == 1);
    bool bIsRepair = false;
    if (m_pSessionInfo)
    {
        bIsRepair = m_pSessionInfo->bRePair;
    }

    WIRE_INFO("pair_dev mac[%s], state[%d], is repair[%d]",
              pDeviceInfo->strMac.c_str(), bPairedSucc, bIsRepair);

    if (m_pSessionInfo && bIsRepair && !bPairedSucc)
    {
        //unpair之后,收到pair_change消息属于repair流程,再次收到时是正常pair流程
        m_pSessionInfo->bRePair = false;
        return true;
    }

    bool bSucc = true;
    //匹配结果
    if (bPairedSucc)
    {
        if (!_BTData.IsDevicePaired(pDeviceInfo->eState))
        {
            pDeviceInfo->bPaired = true;
            _BTData.SetDeviceState(pDeviceInfo->devHandle, WIRE_DEVICE_PAIRED_STATE);
        }

        //如果有正在连接的设备，加入等待对列，否则直接进行连接
        if (_BTManager.IsConnectingSessionExist(m_pSession))
        {
            _BTData.AddDeviceToWaittingConnectList(pDeviceInfo->devHandle);
        }
        else
        {
            if (_BTDriver.IsDeviceConnectedInDriver(pDeviceInfo->devHandle))
            {
                m_pSession->EnterState(SESSION_CONNECTED_STATE, pDeviceInfo->devHandle, (void *)m_pSessionInfo);
            }
            else
            {
                if (NULL != m_pSessionInfo)
                {
                    m_pSessionInfo->bAutoConnect = false;
                    m_pSessionInfo->bRePair = false;
                    m_pSession->EnterState(SESSION_CONNECTING_STATE, pDeviceInfo->devHandle, (void *)m_pSessionInfo);
                }
            }
        }
    }
    else
    {
        bSucc = false;
    }

    return bSucc;
}

bool CBTPairState::ProcessRequestPin(unsigned long ulDeviceHandle, const char * pPinCode/* = NULL*/)
{
    DeviceInfo * pDevice = _BTData.GetDeviceInfoByHandle(ulDeviceHandle);

#if IF_BT_CONNECTING_OVERTIME
    SetTimer(false, BT_PAIR_OVERTIMER);
#endif

    //判断是否在匹配状态，不在时,退出。
    if (NULL == pDevice
            || pDevice->eState != WIRE_DEVICE_PAIRING_STATE)
    {
        WIRE_WARN("device[%d],it 's not pairing", ulDeviceHandle);
        return false;
    }

    //如果此时等待队列的队尾不是自己,去连接等待队列
    if (_BTData.GetWaittingDeviceCount() > 0
            && ulDeviceHandle != _BTData.GetWaittingDevice(false))
    {
        WIRE_WARN("have waitting device, do not req pin");
        return false;
    }

    // 当前扫描交互, 已允许扫描过程中配对和连接
    /*if (_BTManager.IsInScan())
    {
        WIRE_WARN("bt is in scan, do not req pin");
        return false;
    }*/

    bool bResult = true;
    if (_ISBTHEADSET(pDevice->eType))
    {
        etl_NotifyAppEx(false, BLUETOOTH_MSG_REQ_PIN, pDevice->eType, pDevice->devHandle,
                        pDevice->strMac.length() + 1, (void *)pDevice->strMac.c_str());
    }
    else if (_ISBTPHONE(pDevice->eType))
    {
        if (NULL == pPinCode)
        {
            bResult = false;
        }
        else
        {
            // ui未响应时，bluez会多次上报pin_confirm 请求弹窗
            if (m_bPinConfirm)
            {
                WIRE_INFO("already in pin confirm state");
                return true;
            }
            m_bPinConfirm = true;

            //上报的字符串 pin&mac
            yl::string strCode = commonAPI_FormatString("%s&%s", pPinCode, pDevice->strMac.c_str());
            WIRE_INFO("show pin code[%s]", strCode.c_str());
#if 0
            _BTDriver.PinConfirm(pDevice->devHandle, true);
#else
            etl_NotifyAppEx(false, BLUETOOTH_MSG_SHOW_PIN, pDevice->eType, pDevice->devHandle,
                            strCode.length() + 1, (void *)strCode.c_str());
#endif
        }
    }
    else
    {
        bResult = false;
    }

    return bResult;
}

//应答PIN
bool CBTPairState::AnswerPin(unsigned long ulDeviceHandle, const char * pKey)
{
    if (NULL == pKey
            || !_BTManager.IsDongleValid())
    {
        return false;
    }

    DeviceInfo * pDevice = _BTData.GetDeviceInfoByHandle(ulDeviceHandle);

    if (NULL == pDevice)
    {
        return false;
    }

    WIRE_INFO("device[%d] answer pin[%s]", ulDeviceHandle, pKey);

#if IF_BT_CONNECTING_OVERTIME
    // 设置配对结果超时定时器
    SetTimer(true, BT_PAIR_OVERTIMER);
#endif

    bool bResult = false;
    if (_ISBTPHONE(pDevice->eType))
    {
        bResult = _BTDriver.PinConfirm(ulDeviceHandle, true);
    }
    else if (_ISBTHEADSET(pDevice->eType))
    {
        bResult = _BTDriver.AnswerPin(pDevice->devHandle, pKey);
    }

    _BTData.SetDeviceState(ulDeviceHandle,
                           bResult ? WIRE_DEVICE_ANSWERPIN_STATE : WIRE_DEVICE_INIT_STATE);

    return bResult;
}

bool CBTPairState::ExitState(bool bEndConnect /* = false */,
                             bool bConnectWaittingDevice /* = true */)
{
    unsigned long ulDeviceHandle = m_lDeviceHandle;
    bool bUserCancel = (NULL == m_pSessionInfo) ? false : m_pSessionInfo->bUserCancel;

    CBaseState::ExitState(bEndConnect);

    unsigned long ulWaittingDeviceHandle = _BTData.GetWaittingDevice();
    if (bConnectWaittingDevice
            && BT_DEVICE_HANDLE_INVAILED != ulWaittingDeviceHandle)
    {
        return _BTManager.ConnectWaittingDevice(ulWaittingDeviceHandle);
    }

    if (!bUserCancel)
    {
        _BTManager.UpdateUI(OP_PAIR_RESULT, false, ulDeviceHandle);
    }

    return true;
}

#endif // IF_SUPPORT_BLUETOOTH
