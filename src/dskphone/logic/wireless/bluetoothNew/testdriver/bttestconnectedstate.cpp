#ifdef IF_BT_SIMULATION_DRIVER
#include "../src/bluetooth_inc.h"

CBTTestConnectedState::CBTTestConnectedState()
{
    m_eState = SESSION_CONNECTED_STATE;
    iFoucuCallID = -1;
}

CBTTestConnectedState::~CBTTestConnectedState()
{

}

bool CBTTestConnectedState::AfterStateCreate(long ulDeviceHandle, void * pUserData)
{
    m_lDeviceHandle = ulDeviceHandle;

    bttest_dev_t * pDevice = _BTTestData.GetDeviceInfo(m_lDeviceHandle);

    if (NULL == pDevice)
    {
        ExitState(true);
        return false;
    }

    pDevice->dev_property.connected = 1;
    pDevice->state = WIRE_DEVICE_CONNECTED_STATE;
    return ReplyMessage(BT_DEV_CONNECTED);
}

bool CBTTestConnectedState::ReplyMessage(unsigned long ulMsg)
{
    btkit_msg_t btkitMsg;

    btkitMsg.bt_event = ulMsg;
    btkitMsg.bt_callback_param.dev_hnd = m_lDeviceHandle;
    etlSleep(500);
    return _BTTestDriver.SendCallBackMsg(&btkitMsg, NULL);
}

bool CBTTestConnectedState::ReplyCallMessage(unsigned long ulMsg,
        btkit_mb_call_status_t & callStatus)
{
    btkit_msg_t btkitMsg;

    btkitMsg.bt_event = ulMsg;
    btkitMsg.bt_callback_param.dev_hnd = m_lDeviceHandle;
    btkitMsg.bt_callback_param.mb_call = callStatus;
    etlSleep(500);
    return _BTTestDriver.SendCallBackMsg(&btkitMsg, NULL);
}

int CBTTestConnectedState::GetCallStatus(int iCallID)
{
    bttest_callstatus_t * pCallStatus = _BTTestData.GetCallInfo(m_listcallstatus, m_lDeviceHandle,
                                        iCallID);
    if (NULL == pCallStatus)
    {
        return -1;
    }

    return pCallStatus->callstatus.status;
}

bool CBTTestConnectedState::BeforeStateTransfer(bool bEndConnect /* = false */)
{
    if (bEndConnect)
    {
        //Step 1.清空通话
        ProcessCallEnd();

        //Step 2.断开设备
        bttest_dev_t * pDevice = _BTTestData.GetDeviceInfo(m_lDeviceHandle);
        if (NULL != pDevice)
        {
            pDevice->state = WIRE_DEVICE_DISCONNECT_STATE;
            pDevice->dev_property.connected = false;
            ReplyMessage(BT_DEV_DISCONNECTED);
        }
    }

    return true;
}

bool CBTTestConnectedState::ProcessOption(const WirelessSessionData & wsData)
{
    bool bHandle = true;

    switch (wsData.ulMessage)
    {
    case WIRELESS_OPTION_BT_DEVICECALLIN:
        {
            if (NULL == wsData.pData)
            {
                break;
            }
            ProcessCallIncoming((const char *)wsData.pData);
        }
        break;
    case WIRELESS_OPTION_BT_DEVICECALLOUT:
        {
            if (NULL == wsData.pData)
            {
                break;
            }

            ProcessCallOut((const char *)wsData.pData);
        }
        break;
    case WIRELESS_OPTION_BT_DEVICECALLANSWER:
        {
            ProcessCallAnswer(wsData.lParam);
        }
        break;
    case WIRELESS_OPTION_BT_DEVICECALLEND:
        {
            ProcessCallEnd(wsData.lParam);
        }
        break;
    case WIRELESS_OPTION_BT_DEVICECALLHOLD:
        {
            if (NULL == wsData.pData)
            {
                break;
            }
            int * iHold = (int *)wsData.pData;
            ProcessCallHold(wsData.lParam, *iHold);
        }
        break;
    default:
        bHandle = false;
        break;
    }

    return bHandle;
}

bool CBTTestConnectedState::ProcessCallIncoming(const yl::string & strNum)
{
    bttest_dev_t * pDevice = _BTTestData.GetDeviceInfo(m_lDeviceHandle);
    if (NULL == pDevice)
    {
        return false;
    }

    int iCallID = _BTTestData.AddCall(m_listcallstatus, m_lDeviceHandle, BT_MB_CALL_STATUS_INCOME,
                                      strNum);

    if (iCallID == -1
            || _ISBTHEADSET(pDevice->dev_type))
    {
        return false;
    }

    bttest_callstatus_t * pCallStatus = _BTTestData.GetCallInfo(m_listcallstatus, pDevice->dev_hnd,
                                        iCallID);
    if (NULL == pCallStatus)
    {
        return false;
    }

    SetFocusCall(iCallID);

    return ReplyCallMessage(BT_MB_CALL_STATUS_CHANGED, pCallStatus->callstatus);
}

bool CBTTestConnectedState::ProcessCallOut(const yl::string & strNum)
{
    bttest_dev_t * pDevice = _BTTestData.GetDeviceInfo(m_lDeviceHandle);
    if (NULL == pDevice)
    {
        return false;
    }

    int iCallStatus = (_ISBTHEADSET(pDevice->dev_type)) ? BT_MB_CALL_STATUS_TRYING :
                      BT_MB_CALL_STATUS_MOBILE_TRYING ;

    int iCallID = _BTTestData.AddCall(m_listcallstatus, m_lDeviceHandle, iCallStatus, strNum);

    bttest_callstatus_t * pCallStatus = _BTTestData.GetCallInfo(m_listcallstatus, m_lDeviceHandle,
                                        iCallID);

    if (NULL == pCallStatus)
    {
        return false;
    }

    SetFocusCall(iCallID);

    if (_ISBTPHONE(pDevice->dev_type))
    {
        return ReplyCallMessage(BT_MB_CALL_STATUS_CHANGED, pCallStatus->callstatus);
    }

    return true;
}

bool CBTTestConnectedState::ProcessCallAnswer(int iCallID)
{
    bttest_callstatus_t * pCallStatus = _BTTestData.GetCallInfo(m_listcallstatus, m_lDeviceHandle,
                                        iCallID);

    if (NULL == pCallStatus
            || (pCallStatus->callstatus.status != BT_MB_CALL_STATUS_INCOME
                && pCallStatus->callstatus.status != BT_MB_CALL_STATUS_TRYING
                && pCallStatus->callstatus.status != BT_MB_CALL_STATUS_MOBILE_TRYING))
    {
        return false;
    }

    //TODO 添加接起失败策略
    pCallStatus->callstatus.status = BT_MB_CALL_STATUS_ESTABLISHED;
    pCallStatus->callstatus.hold = 0;

    bttest_dev_t * pDevice = _BTTestData.GetDeviceInfo(m_lDeviceHandle);
    if (NULL != pDevice
            && _ISBTHEADSET(pDevice->dev_type))
    {
        return ReplyMessage(BT_PF_HS_CALL_ANSWER);
    }

    SetFocusCall(iCallID);
    return ReplyCallMessage(BT_MB_CALL_STATUS_CHANGED, pCallStatus->callstatus);
}

void CBTTestConnectedState::DoEndCall(int iCallID)
{
    bttest_dev_t * pDevice = _BTTestData.GetDeviceInfo(m_lDeviceHandle);
    if (NULL == pDevice)
    {
        return ;
    }

    bttest_callstatus_t * pCallStatus = _BTTestData.GetCallInfo(m_listcallstatus, m_lDeviceHandle,
                                        iCallID);

    if (NULL == pCallStatus)
    {
        return ;
    }

    if (_ISBTHEADSET(pDevice->dev_type))
    {
        ReplyMessage(BT_PF_HS_CALL_TERMINATED);
    }
    else
    {
        pCallStatus->callstatus.status = BT_MB_CALL_STATUS_FINISHED;
        ReplyCallMessage(BT_MB_CALL_STATUS_CHANGED, pCallStatus->callstatus);
    }
}

bool CBTTestConnectedState::ProcessCallEnd(int iCallID/*= -1*/)
{
    if (iCallID == -1)
    {
        for (list_callstatus::iterator itBeg = m_listcallstatus.begin();
                itBeg != m_listcallstatus.end();
                ++itBeg)
        {
            bttest_callstatus_t & callStatus = (*itBeg);
            DoEndCall(callStatus.callstatus.callid);
        }

        _BTTestData.DeleteAllCall(m_listcallstatus);
    }
    else
    {
        DoEndCall(iCallID);
        _BTTestData.DeleteCall(m_listcallstatus, m_lDeviceHandle, iCallID);
    }

    SetFocusCall();

    return true;
}

bool CBTTestConnectedState::ProcessCallHold(int iCallID, int iHold)
{
    bttest_dev_t * pDevice = _BTTestData.GetDeviceInfo(m_lDeviceHandle);
    if (NULL != pDevice
            && _ISBTHEADSET(pDevice->dev_type))
    {
        return false;
    }

    bttest_callstatus_t * pCallStatus = _BTTestData.GetCallInfo(m_listcallstatus, m_lDeviceHandle,
                                        iCallID);

    if (NULL == pCallStatus
            || pCallStatus->callstatus.status != BT_MB_CALL_STATUS_ESTABLISHED
            || (pCallStatus->callstatus.hold == iHold))
    {
        return false;
    }

    //TODO添加HOLD失败策略
    pCallStatus->callstatus.hold = (iHold == 1);
    ReplyCallMessage(BT_MB_CALL_STATUS_CHANGED, pCallStatus->callstatus);

    if (m_listcallstatus.size() > 2)
    {
        return true;
    }

    //unhold 时，需要激活另外一路
    for (list_callstatus::iterator itBeg = m_listcallstatus.begin();
            itBeg != m_listcallstatus.end();
            ++ itBeg)
    {
        bttest_callstatus_t & callStatus = (*itBeg);
        //跳过自己
        if (callStatus.callstatus.callid == iCallID)
        {
            continue;
        }

        //hold一路，需要激活另外一路.
        //unhold 一路，需要hold另外一路.
        if (callStatus.callstatus.hold == iHold)
        {
            callStatus.callstatus.hold = !(iHold == 1);
        }

        ReplyCallMessage(BT_MB_CALL_STATUS_CHANGED, callStatus.callstatus);
    }

    return true;
}

void CBTTestConnectedState::SetFocusCall(int iCallID /* = -1 */)
{
    if (-1 != iCallID)
    {
        iFoucuCallID = iCallID;
    }

    if (m_listcallstatus.size() == 0)
    {
        iFoucuCallID = -1;
    }

    list_callstatus::iterator itBeg = m_listcallstatus.begin();

    iFoucuCallID = itBeg->dataEntry.callstatus.callid;
}
#endif // IF_BT_SIMULATION_DRIVER
