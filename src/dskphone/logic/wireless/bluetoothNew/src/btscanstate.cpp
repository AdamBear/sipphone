#ifdef IF_SUPPORT_BLUETOOTH
#include "bluetooth_inc.h"

CBTScanState::CBTScanState(void)
    : m_iScanIndex(0),
      m_bUserStop(false),
      m_iDelayTimer((UINT) & m_iDelayTimer)
{
    m_eState = SESSION_SCAN_STATE;
}

CBTScanState::~CBTScanState(void)
{
}

bool CBTScanState::AfterStateCreate(long ulDeviceHandle, void * pUserData)
{
    _BTData.ClearScanDeviceList();
    return StartScan();
}

bool CBTScanState::BeforeStateTransfer(bool bEndConnect/* = false*/)
{
    _BTManager.SetDeviceState(DS_OK);
    _BTManager.UpdateUI(OP_SCAN_RESULT, true);

    return true;
}

bool CBTScanState::ProcessTimer(int iTimerID)
{
    //最长扫描时间15s扫描到设备,退出session
    if (m_iTimer == iTimerID)
    {
        StopScan();
        m_bUserStop = false;
        return true;
    }

    //延迟进入下一次扫描- -!!T5XS蓝牙芯片处理较慢, 可能会出现错误
    if (iTimerID == m_iDelayTimer)
    {
        SetDelayTimer(false);
        StartScan();
        return true;
    }

    return false;
}

//扫描到设备
void CBTScanState::ProcessDeviceFound(unsigned long ulDeviceHandle)
{
    if (BT_DEVICE_HANDLE_INVAILED == ulDeviceHandle)
    {
        return ;
    }

    // 设备信息更新则刷新该设备
    if (!_BTData.CompareDevInfo(ulDeviceHandle))
    {
        _BTManager.UpdateUI(OP_UPDATE_DEVICE, true, ulDeviceHandle);
    }

    //添加到设备列表
    bool bAddResult = _BTData.AddDevice(ulDeviceHandle, m_iScanIndex);
    if (bAddResult)
    {
        _BTManager.UpdateUI(OP_UPDATE_LIST, true, ulDeviceHandle);
        m_iScanIndex ++;
    }
}

//驱动消息
bool CBTScanState::ProcessCallbackEvent(msgObject & objMsg)
{
    if (NULL == objMsg.GetExtraData())
    {
        return false;
    }

    const btkit_msg_t * pMsg = (const btkit_msg_t *)objMsg.GetExtraData();
    bool bHandle = true;

    switch (pMsg->bt_event)
    {
    case BT_FOUND_DEV:
        {
            ProcessDeviceFound(pMsg->bt_callback_param.dev_hnd);
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

bool CBTScanState::ProcessOption(const WirelessSessionData & wsData)
{
    bool bHandle = true;

    switch (wsData.ulMessage)
    {
    case WIRELESS_OPTION_BT_STOP_SCAN:  // wParam   1   实时扫描中用户触发停止或者软件触发停止   lParam 无
        {
            // wParam   2   扫描过程中,外部设备来连接触发临时停止 lParam 无
            if (wsData.wParam == 1)
            {
                m_bUserStop = true;
                StopScan();
            }
        }
        break;
    case WIRELESS_OPTION_BT_SCAN_RESULT:
        {
            if (1 == wsData.lParam)     //开始扫描结果
            {
                if (1 == wsData.wParam) //成功
                {
                    SetTimer(true, BT_SCAN_TIME);
                }
                else
                {
                    m_bUserStop = false;
                    StopScan();
                }
            }
            else if (0 == wsData.lParam)//停止扫描结果
            {
                ProcessStopScanResult(wsData.wParam);
            }
        }
        break;
    default:
        bHandle = false;
        break;
    }

    return bHandle;
}

//开始扫描
bool CBTScanState::StartScan()
{
    _BTData.ClearScanedDevHandleList();
    if (NULL == m_pSession)
    {
        return false;
    }

    _BTManager.SetDeviceState(DS_SCAN);
    HandleScanOption(true);

    return true;
}

//停止扫描
bool CBTScanState::StopScan()
{
    SetTimer(false, BT_SCAN_TIME);

    if (!_BTManager.IsDongleValid()
            || !_BTManager.IsInScan())
    {
        return false;
    }

    HandleScanOption(false);

    return true;
}

//子线程处理扫描操作
bool CBTScanState::HandleScanOption(bool bStartOrStop)
{
    BTTaskParam sParam;
    sParam.lSessionID = m_pSession->GetSessionID();
    return WLAsyncHelper_BtAsync(bStartOrStop ? BT_TASK_SCAN_START :
                                 BT_TASK_SCAN_STOP, 0, &sParam);
}

void CBTScanState::SetDelayTimer(bool bEnable, int iDelayTime/* = 0*/)
{
    WIRE_INFO("Set delay timer [%s], time[%d]", bEnable ? "enable" : "disable", iDelayTime);
    if (bEnable)
    {
        if (iDelayTime <= 0)
        {
            return;
        }

        timerSetThreadTimer(m_iDelayTimer, iDelayTime);
    }
    else
    {
        timerKillThreadTimer(m_iDelayTimer);
    }
}

// 处理停止扫描返回结果
bool CBTScanState::ProcessStopScanResult(bool bResult)
{
    if (1 == bResult)   //成功
    {
        if (!m_bUserStop)
        {
            // 校正扫描列表
            _BTData.CheckScanedDevList();
            _BTManager.UpdateUI(OP_UPDATE_LIST, false, BT_DEVICE_HANDLE_INVAILED);
        }

        _BTData.ShowBluetoothList();
    }

    // 上层触发的停止扫描 -- 退出扫描, 否则继续下一次扫描
    if (m_bUserStop)
    {
        ExitState();
    }
    else
    {
        SetDelayTimer(true, BT_DELAYSCAN_TIME);
    }

    return true;
}

#endif // IF_SUPPORT_BLUETOOTH
