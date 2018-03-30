#ifdef IF_BT_SIMULATION_DRIVER
#include "../src/bluetooth_inc.h"

#define  BTTEST_SCANTIMER 1000

CBTTestScanState::CBTTestScanState()
{
    m_eState = SESSION_SCAN_STATE;
}

CBTTestScanState::~CBTTestScanState()
{

}

bool CBTTestScanState::AfterStateCreate(long ulDeviceHandle, void * pUserData)
{
    SetTimer(true, BTTEST_SCANTIMER);
    return true;
}

bool CBTTestScanState::BeforeStateTransfer(bool bEndConnect /* = false */)
{
    //退出时停定时器
    SetTimer(false, BTTEST_SCANTIMER);
    return true;
}

bool CBTTestScanState::ProcessTimer(int iTimerID)
{
    //定时器到，模拟生成一个新设备
    if (m_iTimer ==  iTimerID)
    {
        btkit_msg_t btkitMsg;
        btkitMsg.bt_event = BT_FOUND_DEV;
        btkitMsg.bt_callback_param.dev_hnd = _BTTestData.GetRandDeviceHandle();
        bttest_dev_t * pDevice = _BTTestData.GetDeviceInfo(btkitMsg.bt_callback_param.dev_hnd);
        if (NULL != pDevice
                && pDevice->bPower)
        {
            _BTTestDriver.SendCallBackMsg(&btkitMsg, NULL);
        }
        return true;
    }

    return false;
}
#endif // IF_BT_SIMULATION_DRIVER
