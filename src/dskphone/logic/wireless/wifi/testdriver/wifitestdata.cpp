#ifdef IF_WIFI_SIMULATION_DRIVER
#include "wifitestdata.h"
#include <stdlib.h>
#include <stdio.h>


CWIFITestData::CWIFITestData()
{
}

CWIFITestData::~CWIFITestData()
{
}

//初始化构造数据
void CWIFITestData::Init()
{
    //设置随即种子
    srand((unsigned)time(NULL) * 3);

    GET_RAND GetRand;
    sys_wifi_ap_t ApBuff;
    sys_wifi_nw_stat_t NwStatsBuff;
    char cWps_Pin[9] = "12345678";
    char cPsk[9] = "0000";

    m_iLinkApId = -1;
    m_iWifiStatus = SYS_WIFI_GLOBAL_OFF;

    for (int cnt = 0; cnt < WIFI_AP_LIST_MAX; cnt++)
    {
        NwStatsBuff.group_cipher = 0;
        NwStatsBuff.key_mgmt = 0;
        NwStatsBuff.link_speed = GetRandNum(LINK_SPEED);
        sprintf(NwStatsBuff.name, "WIFITest%d", cnt);
        NwStatsBuff.nw_id = cnt + 50;
        NwStatsBuff.pairwise_cipher = 0;
        NwStatsBuff.wpa_state = WIFI_WPA_DISCONNECTED;
        sprintf(NwStatsBuff.nw_auth.wps_pin, cWps_Pin);
        sprintf(NwStatsBuff.nw_auth.psk, cPsk);

        ApBuff.ap_id = cnt + 50;
        //sprintf(ApBuff.ap_mac,
        //  "%x%x:%x%x:%x%x:%x%x:%x%x:%x%x",
        //  GetRandNum(MAC), GetRandNum(MAC),
        //  GetRandNum(MAC), GetRandNum(MAC),
        //  GetRandNum(MAC), GetRandNum(MAC),
        //  GetRandNum(MAC), GetRandNum(MAC),
        //  GetRandNum(MAC), GetRandNum(MAC),
        //  GetRandNum(MAC), GetRandNum(MAC),
        //  GetRandNum(MAC), GetRandNum(MAC));
        ApBuff.ap_status = WIFI_AP_ACTIVE;
        ApBuff.freq = 2000;
        sprintf(ApBuff.name, "WIFITest%d", cnt);
        ApBuff.nw_id = cnt + 50;
        ApBuff.signal = GetRandNum(SIGNAL);
        SetSercure(ApBuff.auth, ApBuff.caps, cPsk);

        WIRE_INFO("TestDriver---->wifi test data init");
        m_listAp.push_back(ApBuff);
        m_listNwStat.push_back(NwStatsBuff);
    }
}


//获取当前连接网络信息
bool CWIFITestData::GetNetworkStatus(sys_wifi_nw_stat_t * stat, int buf_len)
{
    if (m_iLinkApId == -1 ||
            m_listNwStat.size() == 0 ||
            m_listAp.size() == 0)
    {
        return false;
    }

    int iNwId = -1;
    for (list_ap::iterator itBeg = m_listAp.begin();
            itBeg != m_listAp.end();
            itBeg ++)
    {
        if (m_iLinkApId == itBeg->dataEntry.ap_id)
        {
            iNwId = itBeg->dataEntry.nw_id;
            break;
        }
    }

    if (iNwId == -1)
    {
        return false;
    }

    for (list_nw_stat::iterator itBeg = m_listNwStat.begin();
            itBeg != m_listNwStat.end();
            itBeg ++)
    {
        if (iNwId == itBeg->dataEntry.nw_id)
        {
            stat->group_cipher = itBeg->dataEntry.group_cipher;
            stat->key_mgmt = itBeg->dataEntry.key_mgmt;
            stat->link_speed = itBeg->dataEntry.link_speed;
            strncpy(stat->name, itBeg->dataEntry.name, strlen(stat->name));
            strncpy(stat->nw_auth.wps_pin,
                    itBeg->dataEntry.nw_auth.wps_pin, strlen(stat->nw_auth.wps_pin));
            stat->nw_id = itBeg->dataEntry.nw_id;
            stat->pairwise_cipher = itBeg->dataEntry.pairwise_cipher;
            stat->wpa_state = itBeg->dataEntry.wpa_state;


            WIRE_INFO("TestDriver---->GetNetWorkStatus()");
            return true;
        }
    }

    return false;
}

//获取扫描链表
bool CWIFITestData::GetAPList(sys_wifi_ap_t * ap_list, int buf_len)
{
    if (0 == m_listAp.size())
    {
        WIRE_INFO("TestDriver---->m_listAp.size()=%d", m_listAp.size());
        return false;
    }

    int cnt = 0;
    for (list_ap::iterator itBeg = m_listAp.begin();
            cnt < WIFI_AP_LIST_MAX;
            cnt ++, itBeg++)
    {
        ap_list[cnt].ap_id = itBeg->dataEntry.ap_id;
        //ap_list[cnt].ap_mac
        ap_list[cnt].ap_status = itBeg->dataEntry.ap_status;
        ap_list[cnt].auth = itBeg->dataEntry.auth;
        ap_list[cnt].caps = itBeg->dataEntry.caps;
        ap_list[cnt].freq = itBeg->dataEntry.freq;
        strcpy(ap_list[cnt].name, itBeg->dataEntry.name);
        ap_list[cnt].nw_id = itBeg->dataEntry.nw_id;
        ap_list[cnt].signal = itBeg->dataEntry.signal;
    }

    WIRE_INFO("TestDriver---->GetAPList()");
    return true;
}

//设置Wifi开关状态
bool CWIFITestData::SetWifiStatus(int eStatus)
{
    if (m_iWifiStatus == eStatus)
    {
        WIRE_INFO("TestDriver---->Device is already in %d status\n", m_iWifiStatus);
    }
    else
    {
        WIRE_INFO("TestDriver---->Set wifi status from %d to %d\n", m_iWifiStatus, eStatus);
        m_iWifiStatus = eStatus;
        return true;
    }

    return false;
}

//获取ap id相应信息
bool CWIFITestData::GetApInfo(int ap_id, sys_wifi_ap_t & ap_info)
{
    for (list_ap::iterator itBeg = m_listAp.begin();
            itBeg != m_listAp.end();
            itBeg++)
    {
        if (ap_id == itBeg->dataEntry.ap_id)
        {
            ap_info.auth = itBeg->dataEntry.auth;
            ap_info.caps = itBeg->dataEntry.caps;
            break;
        }
    }

    return false;
}

//获取随机数
int CWIFITestData::GetRandNum(GET_RAND eType)
{
    switch (eType)
    {
    case LINK_SPEED:
        return (rand() % 1000);
    case SIGNAL:
        return (rand() % 100 - 100);
    case MAC:
        return (rand() % 15);
    case SUCCESS_RATE:
        return (rand() % 10);
    case RAND_AP_ID:
        return (rand() % (WIFI_AP_LIST_MAX - 1));
    default:
        break;
    }

    return true;
}

//根据工作模式和加密方式,设置参数
void CWIFITestData::SetSercure(int & auth, int & caps, char * psk)
{
    static signed int cnt = 0;

    if (cnt >= 0 && cnt <= 2)   //NONE
    {
        auth = WIFI_AUTH_NONE;
        caps = WIFI_CAP_PRIVACY;
        memset(psk, 0, strlen(psk));
    }
    else if (cnt <= 5)          //WEP
    {
        auth = 0;
        caps = WIFI_CAP_PRIVACY;
    }
    else if (cnt <= 8)          //wpa+ccmp
    {
        auth = WIFI_AUTH_PSK | WIFI_AUTH_WPA | WIFI_AUTH_CCMP;
        caps = 0;
    }
    else if (cnt <= 11)         //wpa+tkip
    {
        auth = WIFI_AUTH_PSK | WIFI_AUTH_WPA | WIFI_AUTH_TKIP;
        caps = 0;
    }
    else if (cnt <= 14)         //wpa + tkip+ccmp
    {
        auth = WIFI_AUTH_PSK | WIFI_AUTH_WPA |
               WIFI_AUTH_CCMP | WIFI_AUTH_TKIP;
        caps = 0;
    }
    else if (cnt <= 17)         //wpa2 + ccmp
    {
        auth = WIFI_AUTH_WPA2 | WIFI_AUTH_CCMP;
        caps = 0;
    }
    else if (cnt <= 20)         //wpa2+tkip
    {
        auth = WIFI_AUTH_PSK | WIFI_AUTH_WPA2 | WIFI_AUTH_TKIP;
        caps = 0;
    }
    else if (cnt <= 23)         //wpa2 + tkip+ccmp
    {
        auth = WIFI_AUTH_PSK | WIFI_AUTH_WPA2 |
               WIFI_AUTH_CCMP | WIFI_AUTH_TKIP;
        caps = 0;
    }
    else if (cnt <= 26)         //wpa+wpa2+ccmp
    {
        auth = WIFI_AUTH_PSK | WIFI_AUTH_WPA |
               WIFI_AUTH_WPA2 | WIFI_AUTH_CCMP;
        caps = 0;
    }
    else if (cnt <= 29)         //wpa+wpa2+tkip
    {
        auth = WIFI_AUTH_PSK | WIFI_AUTH_WPA |
               WIFI_AUTH_WPA2 | WIFI_AUTH_TKIP;
        caps = 0;
    }
    else if (cnt <= 32)         //wpa+wpa2+tkip+ccmp
    {
        auth = WIFI_AUTH_PSK | WIFI_AUTH_WPA | WIFI_AUTH_WPA2 |
               WIFI_AUTH_CCMP | WIFI_AUTH_TKIP;
        caps = 0;
    }
    cnt ++;
}

int CWIFITestData::GetWifiSwitchStatus(void)
{
    return m_iWifiStatus;
}

bool CWIFITestData::SetLinkApId(long linkapid)
{
    m_iLinkApId = linkapid;

    return true;
}

#endif //IF_WIFI_SIMULATION_DRIVER
