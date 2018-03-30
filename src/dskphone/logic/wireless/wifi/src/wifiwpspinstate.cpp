#ifdef IF_SUPPORT_WIFI
///////////////////////////////////////////////////////////
//  CWifiStateWpspin.cpp
//  Implementation of the Class CWifiStateWpspin
//  Created on:      15-五月-2014 9:17:48
//  Original author: songk
///////////////////////////////////////////////////////////
#include "wifi_inc.h"

CWiFiWpspinState::CWiFiWpspinState()
{
    m_eState = SESSION_WPSPIN_STATE;
}

CWiFiWpspinState::~CWiFiWpspinState()
{

}

//开启WPS-PIN功能
bool CWiFiWpspinState::StartWps()
{
    return _WifiDriver.StartWpspin();
}

bool CWiFiWpspinState::StopWps()
{
    return _WifiDriver.StopWps(1);
}
#endif // IF_SUPPORT_WIFI
