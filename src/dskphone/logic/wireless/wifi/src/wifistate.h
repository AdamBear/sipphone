///////////////////////////////////////////////////////////
//  CWiFiState.h
//  Implementation of the Class CWiFiState
//  Created on:      2014年11月24日19:56:01
//  Original author: songk
///////////////////////////////////////////////////////////
#ifdef IF_SUPPORT_WIFI
#ifndef __WIFISTATE_H__
#define __WIFISTATE_H__

class CWiFiState : public CBaseState
{
public:
    CWiFiState(void)
    {
        m_lDeviceHandle = WIFI_NET_ID_INVAILED;
        m_pSessionInfo = NULL;
    }
    virtual ~CWiFiState(void) {}

    void SetSessionInfo(WiFiSessionInfo * pSessionInfo)
    {
        m_pSessionInfo = pSessionInfo;
    }
    WiFiSessionInfo * GetSessionInfo()
    {
        return m_pSessionInfo;
    }
    void DeleteSessionInfo()
    {
        SAFE_DELETE(m_pSessionInfo);
    }

protected:
    WiFiSessionInfo * m_pSessionInfo;
};

#endif //__WIFISTATE_H__
#endif // IF_SUPPORT_WIFI
