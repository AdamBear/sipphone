#ifdef IF_WIFI_SIMULATION_DRIVER
#ifndef _WIFITESTDATA_H_
#define _WIFITESTDATA_H_

#include "../src/wifi_inc.h"
#include "wifitestdatadefine.h"

class CWIFITestData
{
private:
    CWIFITestData();
    ~CWIFITestData();

public:
    static CWIFITestData & GetInstance()
    {
        static CWIFITestData s_Instance;
        return s_Instance;
    }

    //初始化数据
    void Init();

    //设置WIFI开关
    void SetGlobalFunc(int op);

    void SyncStatus();

    bool GetNetworkStatus(sys_wifi_nw_stat_t * stat, int buf_len);

    //获取扫描到的AP列表
    bool GetAPList(sys_wifi_ap_t * ap_list, int buf_len);

    //获取WIFI设备Mac地址
    bool GetMacString(char * buff, int buff_max);

    bool SetWifiStatus(int eStatus);

    bool GetApInfo(int ap_id, sys_wifi_ap_t & ap_info);

    int GetRandNum(GET_RAND eType);

    void SetSercure(int & auth, int & caps, char * psk);

    int GetWifiSwitchStatus(void);

    bool SetLinkApId(long linkapid);
private:
    //WIFI开关状态
    int m_iWifiStatus;

    //当前网络状态
    list_nw_stat    m_listNwStat;

    //扫描到的网络信息
    list_ap         m_listAp;

    long m_iLinkApId;
};

#define _WIFITestData CWIFITestData::GetInstance()

#endif
#endif //IF_WIFI_SIMULATION_DRIVER
