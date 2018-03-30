#ifdef IF_WIFI_SIMULATION_DRIVER
#include "wifitestwpspinstate.h"

CWiFiTestWpsPinState::CWiFiTestWpsPinState()
{
    m_eState = SESSION_WPSPIN_STATE;
}

CWiFiTestWpsPinState::~CWiFiTestWpsPinState()
{

}

bool CWiFiTestWpsPinState::AfterStateCreate(long ulDeviceHandle, void * pUserData)
{
    _WIFITestData.SetLinkApId(ulDeviceHandle);
    m_lDeviceHandle = ulDeviceHandle;

    WIRE_INFO("TEST_DRIVER---->Connect wps-pin net id: %d\n", ulDeviceHandle);
    etl_NotifyApp(false, SYS_MSG_WIFI_WPS_RESULT, ulDeviceHandle, 0);

}

bool CWiFiTestWpsPinState::BeforeStateTransfer(bool bEndConnect /* = false */)
{

}

#endif //IF_WIFI_SIMULATION_DRIVER
