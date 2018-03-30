#ifdef IF_WIFI_SIMULATION_DRIVER
#include "wifitestconnectingstate.h"

CWIFITestConnectingState::CWIFITestConnectingState()
{
    m_eState = SESSION_CONNECTING_STATE;
}

CWIFITestConnectingState::~CWIFITestConnectingState()
{
}

bool CWIFITestConnectingState::AfterStateCreate(long ulDeviceHandle, void * pUserData)
{
    long m_lDeviceHandle = ulDeviceHandle;

    etl_NotifyApp(false, SYS_MSG_WIFI_CONNECT_STATUS, m_lDeviceHandle, WIFI_CALLBACK_EVENT_CONNECTTING);
    WIRE_INFO("TEST_DRIVER---->Connecting net id: %d", m_lDeviceHandle);

    sys_wifi_ap_t apInfo;
    sys_wifi_nw_auth_t * auth = (sys_wifi_nw_auth_t *)pUserData;
    _WIFITestData.GetApInfo(m_lDeviceHandle, apInfo);
    //PSK为空判断安全模式和加密方式为NONE,若不是,直接连接失败
    /*if (!(NULL == auth->psk &&
        (apInfo.auth & WIFI_AUTH_NONE ||
        !((apInfo.auth & WIFI_AUTH_WPA) ||
        (apInfo.auth & WIFI_AUTH_WPA2) ||
        (apInfo.caps & WIFI_CAP_PRIVACY)))))
    {
        etl_NotifyApp(false, SYS_MSG_WIFI_CONNECT_STATUS, m_lDeviceHandle, WIFI_CALLBACK_EVENT_CONNECTFAILED);
        WIRE_INFO("TEST_DRIVER---->Connect failed net id: %d", m_lDeviceHandle);
        return false;
    }*/

    return true;
}

bool CWIFITestConnectingState::BeforeStateTransfer(bool bEndConnect /*= false*/)
{
    return true;
}

bool CWIFITestConnectingState::ProcessOption(const WirelessSessionData & wsData)
{
    if (WIRELESS_OPTION_WIFI_ANSWERPSK == wsData.ulMessage)
    {
        int iRandom = _WIFITestData.GetRandNum(SUCCESS_RATE) % 10;

        //设置1/10的连接失败概率
        int iResult = iRandom == 1 ? WIFI_CALLBACK_EVENT_CONNECTFAILED : WIFI_CALLBACK_EVENT_CONNECTED;

        WIRE_INFO("TestDriver---->iRandom = [%d], iResult = [%d]", iRandom, iResult);
        if (WIFI_CALLBACK_EVENT_CONNECTFAILED == iResult)
        {
            //ExitState(true);
            etl_NotifyApp(false, SYS_MSG_WIFI_CONNECT_STATUS, wsData.wParam, WIFI_CALLBACK_EVENT_CONNECTFAILED);
            WIRE_INFO("TEST_DRIVER---->Connect failed net id: %d", m_lDeviceHandle);
        }
        else if (WIFI_CALLBACK_EVENT_CONNECTED == iResult)
        {
            return m_pSession->EnterState(SESSION_CONNECTED_STATE, wsData.wParam, NULL);
        }
    }
    return true;
}

#endif //IF_WIFI_SIMULATION_DRIVER
