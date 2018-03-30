#ifdef IF_WIFI_SIMULATION_DRIVER
#ifndef __WIFITEST_DRIVER__INCLUDED_
#define __WIFITEST_DRIVER__INCLUDED_
#include "../src/wifi_inc.h"

class CWiFiTestDriver
{
private:
    CWiFiTestDriver();
    ~CWiFiTestDriver();

public:
    static CWiFiTestDriver & GetInstance()
    {
        static CWiFiTestDriver sInstance;
        return sInstance;
    }

    //Simulation of driver
    BOOL OnMsg(msgObject & msgObj);
    int SetGlobalFunc(mkit_handle_t caller, int op, int timeout);
    int SyncStatus(mkit_handle_t caller, int timeout);
    int ConfigChanged(mkit_handle_t caller, int timeout);
    int GetAPID(mkit_handle_t caller, int profile_id, char * ssid, int len, int timeout);
    int ConnectAction(mkit_handle_t caller, int op, int ap_id, sys_wifi_nw_auth_t * auth, int timeout);
    int ScanAction(mkit_handle_t caller, int op, int timeout);
    int WpsAction(mkit_handle_t caller, int op, int wps_type, int timeout);
    int GetPortStatus();
    int GetNetworkStatus(sys_wifi_nw_stat_t * stat, int buf_len);
    int GetAPList(sys_wifi_ap_t * ap_list, int buf_len);
    int GetMacString(int phyPort, char * buff, int buff_max);

protected:
    //Simulation of UI
    BOOL ProcessTestThreadMsg(msgObject & objMessage);

    void _outputSaveNetworkInfo(WiFiNetInfo & wifiInfo);

    void PrintfSavedList(UI_WIFI_NETINFO_LIST & listWiFi);

    void _outputScanNetInfo(WiFiNetInfo & wifiInfo);

    void PrintfScanNetInfo(UI_WIFI_NETINFO_LIST & listWiFi);

    void GetInt(int & iNumber);

    void GetBaseNetInfo(WiFiNetInfo & netInfo);

    void GetString(yl::string & strParam);


protected:
    bool m_bAdapterEnable;

};

#define _WiFiTestDriver CWiFiTestDriver::GetInstance()

#endif //__WIFITEST_DRIVER__INCLUDED_
#endif //IF_WIFI_SIMULATION_DRIVER
