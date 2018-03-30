#ifdef IF_WIFI_SIMULATION_DRIVER
#include "wifitestwpsstate.h"

CWiFiTestWpsState::CWiFiTestWpsState()
{
    m_eState = SESSION_WPS_STATE;
}

CWiFiTestWpsState::~CWiFiTestWpsState()
{
    _WIFITestData.SetLinkApId(-1);
}

bool CWiFiTestWpsState::AfterStateCreate(long ulDeviceHandle, void * pUserData)
{
    _WIFITestData.SetLinkApId(ulDeviceHandle);
    m_lDeviceHandle = ulDeviceHandle;

    WIRE_INFO("TEST_DRIVER---->Connect wps net id: %d\n", ulDeviceHandle);
    etl_NotifyApp(false, SYS_MSG_WIFI_WPS_RESULT, ulDeviceHandle, 0);
}

bool CWiFiTestWpsState::BeforeStateTransfer(bool bEndConnect /* = false */)
{

}

#endif //IF_WIFI_SIMULATION_DRIVER
