#ifdef IF_BT_SIMULATION_DRIVER
#include "../src/bluetooth_inc.h"

#define BTTEST_CONNECTINGOVERTIME 60*1000

CBTTestConnectingState::CBTTestConnectingState()
{
    m_eState = SESSION_CONNECTING_STATE;
}

CBTTestConnectingState::~CBTTestConnectingState()
{

}

bool CBTTestConnectingState::AfterStateCreate(long ulDeviceHandle, void * pUserData)
{
    m_lDeviceHandle = ulDeviceHandle;

    bttest_dev_t * pDevice = _BTTestData.GetDeviceInfo(m_lDeviceHandle);

    if (NULL == pDevice
            || !_BTData.IsDevicePaired(pDevice->state))
    {
        return false;
    }

    if (!pDevice->bPower)
    {
        SetTimer(true, BTTEST_CONNECTINGOVERTIME);
        return true;
    }

    //十分之一的连接失败几率
    int iRandom = CBTTestData::GetRandNumber();

    unsigned long ulEvent = (iRandom % 10 == 0) ? BT_DEV_CONNECTED_FAILE : BT_DEV_CONNECTED;

    if (ulEvent == BT_DEV_CONNECTED)
    {
        return m_pSession->EnterState(SESSION_CONNECTED_STATE, m_lDeviceHandle, NULL);
    }
    else
    {
        ExitState(true);
    }
}

bool CBTTestConnectingState::BeforeStateTransfer(bool bEndConnect /* = false */)
{
    if (bEndConnect)
    {
        bttest_dev_t * pDevice = _BTTestData.GetDeviceInfo(m_lDeviceHandle);
        if (NULL != pDevice)
        {
            pDevice->dev_property.connected = 0;
            pDevice->state = WIRE_DEVICE_PAIRED_STATE;
            ReplyMessage(BT_DEV_CONNECTED_FAILE);
        }
    }

    return true;
}

bool CBTTestConnectingState::ReplyMessage(unsigned long ulMsg)
{
    btkit_msg_t btkitMsg;

    btkitMsg.bt_event = ulMsg;
    btkitMsg.bt_callback_param.dev_hnd = m_lDeviceHandle;
    etlSleep(500);
    return _BTTestDriver.SendCallBackMsg(&btkitMsg, NULL);
}

bool CBTTestConnectingState::ProcessTimer(int iTimerID)
{
    if (m_iTimer == iTimerID)
    {
        ExitState(true);
        return true;
    }

    return false;
}
#endif // IF_BT_SIMULATION_DRIVER
