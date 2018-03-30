#ifdef IF_WIFI_SIMULATION_DRIVER
#include "wifitestscanstate.h"

CWIFITestScanState::CWIFITestScanState()
{
    m_eState = SESSION_SCAN_STATE;
}

CWIFITestScanState::~CWIFITestScanState()
{
}

bool CWIFITestScanState::AfterStateCreate(long ulDeviceHandle, void * pUserData)
{
    WIRE_ERR("TestDriver---->create scan state success");
    return true;
}

bool CWIFITestScanState::BeforeStateTransfer(bool bEndConnect /* = false */)
{
    return true;
}

#endif //IF_WIFI_SIMULATION_DRIVER
