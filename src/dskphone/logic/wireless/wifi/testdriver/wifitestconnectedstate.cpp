#ifdef IF_WIFI_SIMULATION_DRIVER
#include "wifitestconnectedstate.h"

CWIFITestConnectedState::CWIFITestConnectedState()
{
    m_eState = SESSION_CONNECTED_STATE;
}

CWIFITestConnectedState::~CWIFITestConnectedState()
{
    _WIFITestData.SetLinkApId(-1);
}

bool CWIFITestConnectedState::AfterStateCreate(long lDeviceHandle, void * pUserData)
{
    _WIFITestData.SetLinkApId(lDeviceHandle);
    m_lDeviceHandle = lDeviceHandle;

    etl_NotifyApp(false, SYS_MSG_WIFI_CONNECT_STATUS, lDeviceHandle, WIFI_CALLBACK_EVENT_CONNECTED);
    WIRE_INFO("TEST_DRIVER---->Connected net id: %d\n", lDeviceHandle);
}

bool CWIFITestConnectedState::BeforeStateTransfer(bool bEndConnect /* = false */)
{
    if (true)
    {
        _WIFITestData.SetLinkApId(-1);
    }
}

#endif //IF_WIFI_SIMULATION_DRIVER

