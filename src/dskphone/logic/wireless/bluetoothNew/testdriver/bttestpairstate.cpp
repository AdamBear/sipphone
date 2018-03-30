#ifdef IF_BT_SIMULATION_DRIVER
#include "../src/bluetooth_inc.h"

#define BTTEST_PAIROVERTIME 60*1000

CBTTestPairState::CBTTestPairState()
{
    m_eState = SESSION_PAIRING_STATE;
}


CBTTestPairState::~CBTTestPairState()
{

}

bool CBTTestPairState::AfterStateCreate(long ulDeviceHandle, void * pUserData)
{
    m_lDeviceHandle = ulDeviceHandle;

    bttest_dev_t * pDevice = _BTTestData.GetDeviceInfo(ulDeviceHandle);

    if (NULL == pDevice)
    {
        ExitState();
        return false;
    }

    //未上电的设备,暂时直接返回连接成功，等待超时。
    if (!pDevice->bPower)
    {
        SetTimer(true, BTTEST_PAIROVERTIME);
        return true;
    }

    //已连接或已匹配的设备不再进行匹配
    if (NULL == pDevice
            || pDevice->dev_property.connected != 0
            || pDevice->dev_property.paired != 0
       )
    {
        ExitState();
        return false;
    }

    pDevice->state = WIRE_DEVICE_ANSWERPIN_STATE;

#define TEST_PAIRCHANGE 0

    if (TEST_PAIRCHANGE == 1)
    {
        return ReplyMessage(BT_DEV_PAIR_CHANGED, 1);
    }
    else if (TEST_PAIRCHANGE == 2)
    {
        return ReplyMessage(BT_DEV_PAIR_CHANGED, 0);
    }

    if (_ISBTPHONE(pDevice->dev_type))
    {
        return ReplyMessage(BT_PIN_CONFIRM);
    }

    return ReplyMessage(BT_REQ_PIN);
}

bool CBTTestPairState::BeforeStateTransfer(bool bEndConnect /* = false */)
{
    SetTimer(false, BTTEST_PAIROVERTIME);

    if (bEndConnect)
    {
        bttest_dev_t * pDevice = _BTTestData.GetDeviceInfo(m_lDeviceHandle);
        if (NULL != pDevice)
        {
            pDevice->dev_property.paired = 0;
            pDevice->state = WIRE_DEVICE_INIT_STATE;
        }

        return ReplyMessage(BT_DEV_PAIR_CHANGED, false);
    }

    return true;
}

bool CBTTestPairState::ProcessTimer(int iTimerID)
{
    if (m_iTimer ==  iTimerID)
    {
        // 连接超时
        ExitState(true);
        return true;
    }
    return false;
}

bool CBTTestPairState::ReplyMessage(unsigned long lMsg, bool bPaired/* = false*/)
{
    btkit_msg_t btkitMsg;

    btkitMsg.bt_event = lMsg;

    if (lMsg == BT_DEV_PAIR_CHANGED)
    {
        btkitMsg.bt_callback_param.pair_info.dev_hnd = m_lDeviceHandle;
        btkitMsg.bt_callback_param.pair_info.paired = bPaired;
    }
    else if (lMsg == BT_PIN_CONFIRM)
    {
        btkitMsg.bt_callback_param.pin_info.dev_hnd = m_lDeviceHandle;
        memset(btkitMsg.bt_callback_param.pin_info.pin, 0, BT_MAX_PIN_LEN);
        strcpy(btkitMsg.bt_callback_param.pin_info.pin, "000000");
    }
    else
    {
        btkitMsg.bt_callback_param.dev_hnd = m_lDeviceHandle;
    }

    etlSleep(500);
    return _BTTestDriver.SendCallBackMsg(&btkitMsg, NULL);
}

bool CBTTestPairState::ProcessOption(const WirelessSessionData & wsData)
{
    bool bHandle = true;
    switch (wsData.ulMessage)
    {
    case WIRELESS_OPTION_BTTEST_ANSWERPIN:
    case WIRELESS_OPTION_BTTEST_PINCONFIRM:
        {
            ProcessAnswerPin(wsData);
        }
        break;
    case WIRELESS_OPTION_BTTEST_CONNECT:
        {
            if (wsData.wParam == m_lDeviceHandle
                    && NULL != m_pSession)
            {
                m_pSession->EnterState(SESSION_CONNECTING_STATE, m_lDeviceHandle, NULL);
            }
        }
        break;
    default:
        bHandle = false;
        break;
    }

    return bHandle;
}

bool CBTTestPairState::ProcessAnswerPin(const WirelessSessionData & wsData)
{
    bttest_dev_t * pDevice = _BTTestData.GetDeviceInfo(wsData.wParam);

    if (NULL == pDevice
            || !pDevice->bPower
            || WIRE_DEVICE_ANSWERPIN_STATE != pDevice->state)
    {
        ExitState(true);
        return false;
    }

    yl::string strPin;

    if (wsData.ulMessage == WIRELESS_OPTION_BTTEST_PINCONFIRM
            && wsData.lParam == 1)
    {
        strPin = "000000";
    }
    else if (wsData.ulMessage == WIRELESS_OPTION_BTTEST_ANSWERPIN
             && NULL != wsData.pData)
    {
        strPin = (const char *)wsData.pData;
    }

    pDevice->dev_property.paired = (strPin == "0000" || strPin == "000000");
    pDevice->state = (pDevice->dev_property.paired == 1) ? WIRE_DEVICE_PAIRED_STATE :
                     WIRE_DEVICE_INIT_STATE;

    etlSleep(500);
    ReplyMessage(BT_DEV_PAIR_CHANGED, pDevice->dev_property.paired);

    //ExitState();
}
#endif // IF_BT_SIMULATION_DRIVER
