#ifdef IF_SUPPORT_WIFI
///////////////////////////////////////////////////////////
//  CWiFiData.cpp
//  Implementation of the Class CWiFiData
//  Created on:      15-五月-2014 9:17:54
//  Original author: songk
///////////////////////////////////////////////////////////
#include "wifi_inc.h"
#include "commonapi/stringhelperapi.h"
#include "ylstl/ylstringalgorithm.h"
#include "commonunits/commonunits_def.h"

//#ifndef _T49
//
//#ifdef kszWiFiPriority
//#undef kszWiFiPriority
//#undef kszWiFiName
//#undef kszWiFiProfileName
//#undef kszWiFiSecureMode
//#undef kszWiFiEncryptType
//#undef kszWiFiPassword
//#undef kszWiFiEapName
//#undef kszWiFiEapPassword
//#undef kszWiFiEapType
//#endif
//
//#define  kszWiFiPriority                "priority"
//#define  kszWiFiName                    "ssid"
//#define  kszWiFiProfileName             "profilename"
//#define  kszWiFiSecureMode              "key_mgmt"
//#define  kszWiFiEncryptType             "pairwise"
//#define  kszWiFiPassword                "psk"
//#define  kszWiFiEapName                 "username"
//#define  kszWiFiEapPassword             "password"
//#define  kszWiFiEapType                 "eap"
//
#define  WIFI_INI_FILE                  CONFIG_PATH "system/system.ini"
#define  WIFI_INI_SECTION_PREFIX        "WifiNetwork"
#define  WIFI_INI_MAXLENGTH             512
#define  WIFI_NETWORK_EAP_TYPE          "TTLS PEAP TLS"

//#endif // _T49

/* used to calculate the chan_freq = chan_factor * 500Mhz + 5 * chan_number */
#define WF_CHAN_FACTOR_2_4_G            4814    /* 2.4 GHz band, 2407 MHz */
#define WF_CHAN_FACTOR_5_G              10000   /* 5   GHz band, 5000 MHz */
#define WF_CHAN_FACTOR_4_G              8000    /* 4.9 GHz band for Japan */


bool CompareByPriority(const WiFiNetInfo & lParam, const WiFiNetInfo & rParam)
{
    return lParam.iPriority > rParam.iPriority;
}

bool CompareBySignal(const WiFiNetInfo & lParam, const WiFiNetInfo & rParam)
{
    if (lParam.iSignal != rParam.iSignal)
    {
        return (lParam.iSignal > rParam.iSignal);
    }
    else
    {
        yl::string strLowerLparam = lParam.strProfileName;
        yl::string strLowerRparam = rParam.strProfileName;
        yl::to_lower(strLowerLparam);
        yl::to_lower(strLowerRparam);
        return (strcmp(strLowerLparam.c_str(), strLowerRparam.c_str()) < 0);
    }
}

CWiFiData::CWiFiData()
{
    m_listSavedNet.clear();
    m_listScanedNet.clear();
}

CWiFiData::~CWiFiData()
{
}

//编辑网络连接信息
bool CWiFiData::ModifyNetwork(int iNetID, const WiFiNetInfo * pNetInfo)
{
    if (NULL == pNetInfo)
    {
        WIRE_WARN("pNetworkInfo is null");
        return false;
    }

    //先判断网络是否在列表中
    WiFiNetInfo * pSaveInfo = GetNetInfoByID(iNetID, true);
    if (NULL == pSaveInfo)
    {
        WIRE_WARN("can not found network[%d]", iNetID);
        return false;
    }

    //校验数据的有效性
    if (!IsSaveAvailable(*pNetInfo))
    {
        return false;
    }

    WIRE_INFO("modify network[%s]:[%d]", pNetInfo->strSSID.c_str(), pNetInfo->iNetID);

    pSaveInfo->strSSID = pNetInfo->strSSID;
    pSaveInfo->strProfileName = pNetInfo->strProfileName;
    pSaveInfo->strEapName = pNetInfo->strEapName;
    pSaveInfo->strEapPwd = pNetInfo->strEapPwd;
    pSaveInfo->strPsk = pNetInfo->strPsk;
    pSaveInfo->iSercureMode = pNetInfo->iSercureMode;
    pSaveInfo->iEncrypt = pNetInfo->iEncrypt;

    if (SERCURE_NONE == pSaveInfo->iSercureMode)
    {
        pSaveInfo->strEapName.clear();
        pSaveInfo->strEapPwd.clear();
        pSaveInfo->strPsk.clear();
    }
    else if (pSaveInfo->iSercureMode == SERCURE_WPA_EAP
             || pSaveInfo->iSercureMode == SERCURE_WPA2_EAP)
    {
        pSaveInfo->strEapType = WIFI_NETWORK_EAP_TYPE;
    }

    WriteNetworkInfo(pSaveInfo->iSectionID, *pSaveInfo);

    //通知rtServer重新加载配置
    WLAsyncHelper_WifiAsync(WIFI_TASK_CONFIG_CHANGE, pSaveInfo->iSectionID, -1);

    return true;
}

//调整链表中项的位置
bool CWiFiData::MoveNetworkPostion(int iNetID, int iPos)
{
    if (WIFI_NET_ID_INVAILED == iNetID)
    {
        return false;
    }

    if (iPos <= 0
            || iPos > (m_listSavedNet.size()))
    {
        return false;
    }

    WiFiNetInfo * pNetwork = GetNetInfoByID(iNetID, true);
    if (NULL == pNetwork)
    {
        return false;
    }

    WIRE_INFO("Move network[%d] to Position[%d]", iNetID, iPos);

    //先记录原位置,链表从0计数
    int iOldPos = m_listSavedNet.size() - pNetwork->iPriority;
    if (iOldPos < 0
            || iOldPos == (iPos - 1))
    {
        return false;
    }

    // 往上拖动时，已经新插入一个相同item，旧的位置需要+1,调整的位置需要-1
    int iAdjustPos = iPos;

    if (iOldPos >= iPos)
    {
        iOldPos ++;
        iAdjustPos --;
    }

    int iIndex = 0;

    if (iPos == m_listSavedNet.size())
    {
        m_listSavedNet.push_back(*pNetwork);
    }
    else
    {
        for (WiFiNetInfoItem itBeg = m_listSavedNet.begin();
                itBeg != m_listSavedNet.end();
                iIndex++, itBeg++)
        {
            if (iIndex == iAdjustPos)
            {
                m_listSavedNet.insert(itBeg, *pNetwork);
                break;
            }
        }
    }

    //移除掉旧的item
    iIndex = 0;
    for (WiFiNetInfoItem itBeg = m_listSavedNet.begin();
            itBeg != m_listSavedNet.end();
            itBeg++, iIndex ++)
    {
        if (iIndex == iOldPos)
        {
            m_listSavedNet.erase(itBeg);
            break;
        }
    }

    return ResetPriority(m_listSavedNet);
}

//设置网络的优先级
bool CWiFiData::SetNetworkPriority(int iNetID, bool bUp)
{
    WiFiNetInfo * pNetwork = GetNetInfoByID(iNetID, true);
    if (NULL == pNetwork)
    {
        return false;
    }

    int iPos = m_listSavedNet.size() - pNetwork->iPriority + 1;
    if (iPos < 1)
    {
        return false;
    }

    if ((iPos <= 1 && bUp)
            || (iPos >= m_listSavedNet.size()) && !bUp)
    {
        WIRE_INFO("network [%d]postion[%d] no change", iNetID, iPos);
        return false;
    }

    bUp ? iPos-- : iPos++;

    return MoveNetworkPostion(iNetID, iPos);
}

//添加新的网络至已保存列表
bool CWiFiData::AddNewNetwork(WiFiNetInfo * pNetworkInfo)
{
    if (NULL == pNetworkInfo)
    {
        WIRE_WARN("netinfo can not be null");
        return false;
    }

    //添加规则
    if (NULL != GetNetInfoByID(pNetworkInfo->iNetID, true)
            || !IsSaveAvailable(*pNetworkInfo))
    {
        return false;
    }

    //添加一个网络,先获取一个可用的 SECTION ID
    pNetworkInfo->iSectionID = GetAvailableSectionID();
    if (WIFI_SECTION_ID_INVAILED == pNetworkInfo->iSectionID)
    {
        return false;
    }

    WIRE_INFO("add network[%s]:[%d]", pNetworkInfo->strSSID.c_str(), pNetworkInfo->iNetID);

    //手动添加网络需要分配一个net id
    if (WIFI_NET_ID_INVAILED == pNetworkInfo->iNetID)
    {
        pNetworkInfo->iNetID = _WifiDriver.GetNetIDBySectionID(pNetworkInfo->iSectionID,
                               pNetworkInfo->strSSID);
        if (pNetworkInfo->iNetID < 0)
        {
            WIRE_WARN("can not get available net id by section[%d]", pNetworkInfo->iSectionID + 1);
            return false;
        }
    }

    pNetworkInfo->iPriority = WIFI_MIN_PRIORITY;

    if (pNetworkInfo->iSercureMode == SERCURE_WPA_EAP
            || pNetworkInfo->iSercureMode == SERCURE_WPA2_EAP)
    {
        pNetworkInfo->strEapType = WIFI_NETWORK_EAP_TYPE;
    }

    m_listSavedNet.push_back(*pNetworkInfo);

    //更新到配置文件中
    WriteNetworkInfo(pNetworkInfo->iSectionID, *pNetworkInfo);

    //重新调整整个列表的优先级数值,但此时不需要通知rtServer，等到配置文件更新后一起通知
    ResetPriority(m_listSavedNet, false, false);

    //通知rtServer
    WiFiTaskParam sParam(pNetworkInfo->iSectionID, pNetworkInfo->iNetID);
    WLAsyncHelper_WifiAsync(WIFI_TASK_CONFIG_CHANGE, &sParam);

    return true;
}

//删除网络
bool CWiFiData::DeleteNetwork(int iNetID, bool bNotify/* = true*/)
{
    if (iNetID < 0)
    {
        return false;
    }

    //判断网络是否在列表中
    WiFiNetInfo * pSavedInfo = GetNetInfoByID(iNetID, true);
    if (NULL == pSavedInfo
            || WIFI_SECTION_ID_INVAILED == pSavedInfo->iSectionID)
    {
        WIRE_WARN("can not found network[%d], delete failed", iNetID);
        return false;
    }

    WIRE_INFO("delete network[%s]:[%d] section[%d]", pSavedInfo->strSSID.c_str(), pSavedInfo->iNetID,
              pSavedInfo->iSectionID);

    //删除一个网络，文件写空数据，再从链表中删除。
    WiFiNetInfo netInfo;
    netInfo.iNetID = pSavedInfo->iNetID;
    netInfo.iSectionID = pSavedInfo->iSectionID;
    WriteNetworkInfo(pSavedInfo->iSectionID, netInfo);

    for (WiFiNetInfoItem it = m_listSavedNet.begin(); it != m_listSavedNet.end(); it++)
    {
        if ((*it).iSectionID == pSavedInfo->iSectionID)
        {
            m_listSavedNet.erase(it);
            break;
        }
    }

    //重新调整优先级
    return ResetPriority(m_listSavedNet, false, bNotify);
}

//通过网络ID获取网络信息
WiFiNetInfo * CWiFiData::GetNetInfoByID(int iNetID, bool bSaved/* = false*/)
{
    UI_WIFI_NETINFO_LIST * tmpList = bSaved ? &m_listSavedNet : &m_listScanedNet;
    for (WiFiNetInfoItem it = tmpList->begin(); it != tmpList->end(); ++it)
    {
        if ((*it).iNetID == iNetID)
        {
            return &(*it);
        }
    }

    return NULL;
}

WiFiNetInfo * CWiFiData::GetNetInfoBySSID(const yl::string & strSSID, bool bSaved /* = true */)
{
    UI_WIFI_NETINFO_LIST * tmpList = bSaved ? &m_listSavedNet : &m_listScanedNet;
    for (WiFiNetInfoItem it = tmpList->begin(); it != tmpList->end(); ++it)
    {
        WiFiNetInfo & pNetInfo = (*it);

        if (pNetInfo.strSSID == strSSID)
        {
            return &(*it);
        }
    }

    return NULL;
}

//获取当前连接网络的连接信息
bool CWiFiData::GetConnectedNetInfo(WiFiNetInfo & netInfo, int iNetID/* = WIFI_NET_ID_INVAILED*/)
{
    //如果为空，再从驱动中获取
    if (WIFI_NET_ID_INVAILED == iNetID)
    {
        iNetID = GetConnectedNetID();
    }

    if (iNetID < 0)
    {
        WIRE_WARN("wifi is not connected state, can not get connected network info");
        return false;
    }

    WiFiNetInfo * pNetInfo = GetNetInfoByID(iNetID);
    if (NULL == pNetInfo)
    {
        return false;
    }

    netInfo = *pNetInfo;

    //获取连接速度
    netInfo.iSpeed = GetConnectedNetSpeed();
    return true;
}

//清空内存数据
void CWiFiData::ClearNetworkInfo()
{
    m_listScanedNet.clear();
}

//获取已保存的网络列表
bool CWiFiData::GetSavedNetInfoList(UI_WIFI_NETINFO_LIST & listNetInfo)
{
    //拷贝已保存的网络信息到列表
    listNetInfo = m_listSavedNet;
    return  true;
}

//获取搜索到的网络列表
bool CWiFiData::GetScanedNetInfoList(UI_WIFI_NETINFO_LIST & listNetInfo)
{
    //拷贝已搜索到的网络信息
    listNetInfo = m_listScanedNet;
    return  true;
}

//重新载入配置
bool CWiFiData::ReloadConfig()
{
    m_listSavedNet.clear();
    //从ini文件中读配置到Saved List
    for (int i = 1; i <= WIFI_SAVED_MAX; i++)
    {
        LoadConfig(i);
    }

    ResetPriority(m_listSavedNet, true);
    return true;
}

//重新设置列表优先级
bool CWiFiData::ResetPriority(UI_WIFI_NETINFO_LIST & listInfo, bool bNeedSort/* = false*/,
                              bool bNotify/* = true*/)
{
    if (bNeedSort)
    {
        Sort(listInfo, CompareByPriority);
    }

    int iIndex = 0;
    int iListSize = listInfo.size();

    for (WiFiNetInfoItem itBeg = listInfo.begin(); itBeg != listInfo.end(); ++itBeg, iIndex++)
    {
        WiFiNetInfo & netInfo = *itBeg;

        WIRE_INFO("reset net[%s] section[%d] apid[%d] configid[%s] value[%d] to new value[%d]"
                  , netInfo.strSSID.c_str()
                  , netInfo.iSectionID
                  , netInfo.iNetID
                  , kszWiFiPriority
                  , netInfo.iPriority
                  , iListSize - iIndex + WIFI_MIN_PRIORITY);

        netInfo.iPriority = iListSize - iIndex + WIFI_MIN_PRIORITY;

        WriteNetworkInfo(netInfo.iSectionID, kszWiFiPriority, netInfo.iPriority);
    }

    if (bNotify)
    {
        WLAsyncHelper_WifiAsync(WIFI_TASK_CONFIG_CHANGE);
    }

    return true;
}

//读取一个配置项到内存
bool CWiFiData::LoadConfig(int iSectionID)
{
    WiFiNetInfo netInfo;
    netInfo.iSectionID = iSectionID;

    if (!ReadNetworkInfo(netInfo.iSectionID, netInfo))
    {
        return false;
    }

    netInfo.iNetID = _WifiDriver.GetNetIDBySectionID(iSectionID, netInfo.strSSID);
    // http://10.2.1.199/Bug.php?BugID=87102
    netInfo.eNetState = GetNetworkState(netInfo.iNetID);

    WIRE_INFO("Load Profile[%d]", iSectionID);

    PrintSavedNetInfo(netInfo);

    m_listSavedNet.push_back(netInfo);

    return true;
}

//检查保存网络数据的有效性
bool CWiFiData::IsSaveAvailable(const WiFiNetInfo & netInfo)
{
    //判断SSID是否为空
    if (netInfo.strSSID.empty()
            || netInfo.strProfileName.empty())
    {
        WIRE_INFO("ssid or profilename can not be empty");
        return false;
    }

    //安全模式不为NONE时，psk不能为空
    if ((netInfo.iSercureMode == SERCURE_WEP
            || netInfo.iSercureMode == SERCURE_WPA_PSK
            || netInfo.iSercureMode == SERCURE_WPA2_PSK)
            && netInfo.strPsk.empty())
    {
        WIRE_INFO("psk can not be empty");
        return false;
    }

    //SSID不能相同&PROFILENAME不能相同
    for (WiFiNetInfoItem itBeg = m_listSavedNet.begin(); itBeg != m_listSavedNet.end(); itBeg++)
    {
        if ((*itBeg).iNetID == netInfo.iNetID)
        {
            continue;
        }

        if ((*itBeg).strSSID == netInfo.strSSID)
        {
            WIRE_WARN("ssid[%s] is exist in net[%d]", netInfo.strSSID.c_str(), (*itBeg).iNetID);
            return false;
        }

        if ((*itBeg).strProfileName == netInfo.strProfileName)
        {
            WIRE_WARN("profilename[%s] is exist in net[%d]", netInfo.strProfileName.c_str(),
                      (*itBeg).iNetID);
            return false;
        }
    }

    return true;
}

//打印扫描到网络信息
void CWiFiData::PrintScanNetInfo(const WiFiNetInfo & scanInfo)
{
    WIRE_INFO("scan net[%d]:ssid[%s]: mac[%s]:encrypt[%d]:sercure[%d]:signal[%d]:freq[%d]:channel[%d]:state[%s]"
              , scanInfo.iNetID
              , scanInfo.strSSID.c_str()
              , scanInfo.strMac.c_str()
              , scanInfo.iEncrypt
              , scanInfo.iSercureMode
              , scanInfo.iBaseSignal
              , scanInfo.iFrequence
              , scanInfo.iChannel
              , DeviceStateToString(scanInfo.eNetState)
             );
}

//打印保存的网络信息
void CWiFiData::PrintSavedNetInfo(const WiFiNetInfo & savedInfo)
{
    WIRE_INFO("saved section[%d] net[%d]:profilename[%s]:ssid[%s]:encrypt[%d]:sercure[%d]:priority[%d]"
              , savedInfo.iSectionID
              , savedInfo.iNetID
              , savedInfo.strProfileName.c_str()
              , savedInfo.strSSID.c_str()
              , savedInfo.iEncrypt
              , savedInfo.iSercureMode
              , savedInfo.iPriority
             );
}

//更新搜索结果
bool CWiFiData::UpdateScanInfoList()
{
    WIRE_INFO("update scaned list ");

    UI_WIFI_NETINFO_LIST listNetInfo;
    WiFiNetInfo wifiNetInfo;
    _networkInfo networkInfo;

    //Step 1 从共享内存中取最新扫描列表,并转换成WiFiNetInfo
    MapNetworkInfo(networkInfo);
    for (YLList<_networkPropertyInfo>::ListIterator it = networkInfo.listNetInfo.begin();
            it != networkInfo.listNetInfo.end();
            ++ it)
    {
        wifiNetInfo.Reset();
        if (!MapNetworkPropertyInfo((*it), wifiNetInfo))
        {
            continue;
        }

        listNetInfo.push_back(wifiNetInfo);
    }

    //Step 2 同步最新扫描列表与内存中列表的内容
    for (WiFiNetInfoItem itor = listNetInfo.begin(); itor != listNetInfo.end();)
    {
        // Step 2.1 内存中网络的状态同步到最新扫描列表
        WiFiNetInfo * pScanInfo = GetNetInfoByID((*itor).iNetID);
        if (NULL != pScanInfo)
        {
            (*itor).eNetState = pScanInfo->eNetState;
        }

        //保存的网络，以保存列表的数据为主。
        WiFiNetInfo * pSavedInfo = GetNetInfoByID((*itor).iNetID, true);
        if (NULL != pSavedInfo)
        {
            //显示的数据以扫描列表
            //itor->dataEntry.iSercureMode = pSavedInfo->iSercureMode;
            //itor->dataEntry.iEncrypt = pSavedInfo->iEncrypt;
            (*itor).strProfileName = pSavedInfo->strProfileName;
            (*itor).strPsk = pSavedInfo->strPsk;
        }

        PrintScanNetInfo(*itor);

        //Step 2.2 删除与保存列表中相同SSID的网络
        bool bFound = false;
        for (WiFiNetInfoItem savedIt = m_listSavedNet.begin(); savedIt != m_listSavedNet.end(); savedIt++)
        {
            if ((*savedIt).iNetID == (*itor).iNetID)
            {
                continue;
            }

            if ((*itor).strSSID == (*savedIt).strSSID)
            {
                bFound = true;
                break;
            }
        }

        if (bFound)
        {
            WIRE_INFO("net[%d]:ssid[%s] is erased ", (*itor).iNetID, (*itor).strSSID.c_str());
            itor = listNetInfo.erase(itor);
        }
        else
        {
            itor++;
        }
    }

    //Step 3 将最新列表覆盖到内存并重新排序
    m_listScanedNet = listNetInfo;
    Sort(m_listScanedNet, CompareBySignal);

    WIRE_INFO("scan result:get [%d] net ", m_listScanedNet.size());
    return true;
}

//获取信号值（范围0-4）
int CWiFiData::GetSignalNevel(int iSignal)
{
    if (iSignal <= WIFI_MIN_SIGNAL)
    {
        return WIFI_SINAL_NEVEL1;
    }
    else if (iSignal <= WIFI_LOW_SIGNAL)
    {
        return WIFI_SINAL_NEVEL2;
    }
    else if (iSignal <= WIFI_MID_SIGNAL)
    {
        return WIFI_SINAL_NEVEL3;
    }
    else if (iSignal <= WIFI_HIGH_SIGNAL)
    {
        return WIFI_SINAL_NEVEL4;
    }
    else
    {
        return WIFI_SINAL_NEVEL5;
    }
}

//更新本地数据
bool CWiFiData::RefreshData(bool bSync/* = false*/, bool bUpdateWnd/* = false*/)
{
    //先通知rtServer更新状态
    if (bSync)
    {
        _WifiDriver.RefreshState();
        UpdateScanInfoList();
    }
    else
    {
        WiFiTaskParam sParam;
        sParam.wParam = bUpdateWnd;
        WLAsyncHelper_WifiAsync(WIFI_TASK_REFRESH, &sParam);
    }
    return true;
}

//设置网络的连接状态
bool CWiFiData::SetNetworkState(int iNetID, WIRELESS_DEVICE_STATE eState)
{
    WiFiNetInfo * pScanNetInfo = GetNetInfoByID(iNetID);
    WiFiNetInfo * pSavedNetInfo = GetNetInfoByID(iNetID, true);

    if (NULL == pScanNetInfo
            && NULL == pSavedNetInfo)
    {
        return false;
    }

    if (NULL != pScanNetInfo)
    {
        pScanNetInfo->eNetState = eState;
    }

    if (NULL != pSavedNetInfo)
    {
        pSavedNetInfo->eNetState = eState;
    }

    WIRE_INFO("change network[%d],to state[%s]", iNetID, DeviceStateToString(eState));
    return true;
}

//获取网络的状态
WIRELESS_DEVICE_STATE CWiFiData::GetNetworkState(int iNetID)
{
    //优先获取扫描列表中的状态
    WiFiNetInfo * pScanNetInfo = GetNetInfoByID(iNetID);
    if (NULL != pScanNetInfo)
    {
        return pScanNetInfo->eNetState;
    }

    //手动添加的不可用网络状态
    WiFiNetInfo * pSavedNetInfo = GetNetInfoByID(iNetID, true);
    if (NULL != pSavedNetInfo)
    {
        return pSavedNetInfo->eNetState;
    }

    return WIRE_DEVICE_INIT_STATE;
}

//数据转换
void CWiFiData::MapNetworkInfo(_networkInfo & networkInfo)
{
    _WifiDriver.GetNeworkList(networkInfo.listNetInfo);
    _WifiDriver.GetDongleMac(networkInfo.strDongleMac);
}

bool CWiFiData::MapNetworkPropertyInfo(const _networkPropertyInfo & netPropertyInfo,
                                       WiFiNetInfo & wifiNetInfo)
{
    //过滤无信号 || ssid 为空的网络
    if ((WIFI_NET_ID_INVAILED == netPropertyInfo.iNetID)
            || WIFI_AP_INACTIVE == netPropertyInfo.iStatus
            || netPropertyInfo.strName.empty())
    {
        return false;
    }

    wifiNetInfo.iDeviceID = netPropertyInfo.iDeviceID;                  //nw id
    wifiNetInfo.iNetID = netPropertyInfo.iNetID;                        // ap id
    wifiNetInfo.iCapbility = netPropertyInfo.iCaps;                     //capibility
    wifiNetInfo.iSignal = GetSignalNevel(netPropertyInfo.iSignal);    //信号强度
    wifiNetInfo.iFrequence = netPropertyInfo.iFreq;                   //网络频段
    wifiNetInfo.strSSID = netPropertyInfo.strName;
    wifiNetInfo.strMac = netPropertyInfo.strMac;

    //保存信号原始值，提供给测试模式
    wifiNetInfo.iBaseSignal = netPropertyInfo.iSignal;

    //profilename 默认与SSID相同
    wifiNetInfo.strProfileName = wifiNetInfo.strSSID;

    //通过auth获取加密类型安全方式
    GenerateSercure(wifiNetInfo.iSercureMode, wifiNetInfo.iEncrypt,
                    netPropertyInfo.iAuth, netPropertyInfo.iCaps);

    //Channel与Frequence是相对应的
    wifiNetInfo.iChannel = GenerateChannel(wifiNetInfo.iFrequence);

    return true;
}

//获取当前连接的传输速率
int CWiFiData::GetConnectedNetSpeed()
{
    _networkStatusInfo networkStatus;
    _WifiDriver.GetNetworkStatus(networkStatus);
    return networkStatus.iLinkSpeed;
}

//获取当前连接网络的NetID
int CWiFiData::GetConnectedNetID()
{
    _networkStatusInfo networkStatus;
    _WifiDriver.GetNetworkStatus(networkStatus);

    int iDeviceID = networkStatus.iDeviceID;
    if (WIFI_WPA_COMPLETED != networkStatus.iWpaState
            || WIFI_NET_ID_INVAILED == iDeviceID)
    {
        WIRE_WARN("can not get connect netid from share memory");
        return WIFI_NET_ID_INVAILED;
    }

    for (WiFiNetInfoItem it = m_listScanedNet.begin(); it != m_listScanedNet.end(); it++)
    {
        if ((*it).iDeviceID == iDeviceID)
        {
            WIRE_INFO("get connected netid[%d], by nw_id[%d] from share memmory", (*it).iNetID,
                      iDeviceID);
            return (*it).iNetID;
        }
    }

    return WIFI_NET_ID_INVAILED;
}

//获取WPS-PIN的pin码
void CWiFiData::GetWifiWpsPinCode(yl::string & strPinCode)
{
    _networkStatusInfo networkStatus;
    _WifiDriver.GetNetworkStatus(networkStatus);
    strPinCode = networkStatus.authInfo.strWpsPin;
}

void CWiFiData::CopySavedNetInfo(WiFiNetInfo & savedInfo, const sys_wifi_network * pWifiNetwork)
{
    if (NULL == pWifiNetwork)
    {
        return;
    }

    WIRE_INFO("mode[%d],pairwise[%d]", pWifiNetwork->mode, pWifiNetwork->pairwise);

    WIFI_SERCURE_MODE iMode = SERCURE_NONE;
    WIFI_ENCRYPT_TYPE iType = CIPHER_NONE;

    switch (pWifiNetwork->mode)
    {
    case SYS_WIFI_MODE_OPEN:
        iMode = SERCURE_NONE;
        break;
    case SYS_WIFI_MODE_WPA2_PSK:
        iMode = SERCURE_WPA2_PSK;
        break;
    case SYS_WIFI_MODE_WPA_PSK:
        iMode = SERCURE_WPA_PSK;
        break;
    case SYS_WIFI_MODE_WEP:
        iMode = SERCURE_WEP;
        break;
    case SYS_WIFI_MODE_WPA_EAP:
        iMode = SERCURE_WPA_EAP;
        break;
    case SYS_WIFI_MODE_WPA2_EAP:
        iMode = SERCURE_WPA2_EAP;
        break;
    default:
        iMode = SERCURE_NONE;
        break;
    }

    switch (pWifiNetwork->pairwise)
    {
    case SYS_WIFI_CIPHER_CCMP_TKIP:
        iType = CIPHER_AUTO;
        break;
    case SYS_WIFI_CIPHER_CCMP:
        iType = CIPHER_CCMP;
        break;
    case SYS_WIFI_CIPHER_TKIP:
        iType = CIPHER_TKIP;
        break;
    default:
        iType = CIPHER_NONE;
        break;
    }

    if (pWifiNetwork->mode == SYS_WIFI_MODE_WPA_EAP)
    {
        savedInfo.strEapName = pWifiNetwork->identity;
        savedInfo.strEapPwd = pWifiNetwork->psk;
    }
    else
    {
        savedInfo.strPsk = pWifiNetwork->psk;
    }

    savedInfo.iSercureMode = iMode;
    savedInfo.iEncrypt = iType;
}

//获取安全模式与加密方式
void CWiFiData::GenerateSercure(WIFI_SERCURE_MODE & iSercure, WIFI_ENCRYPT_TYPE & iEncrypt,
                                int iAuth, int iCap)
{
    //NONE
    if (iAuth & WIFI_AUTH_NONE
            || !((iAuth & WIFI_AUTH_WPA)
                 || (iAuth & WIFI_AUTH_WPA2)
                 || (iCap & WIFI_CAP_PRIVACY))
       )
    {
        iSercure = SERCURE_NONE;
        iEncrypt = CIPHER_NONE;
    }
    //WEP
    else if (iCap & WIFI_CAP_PRIVACY)
    {
        iSercure = SERCURE_WEP;
        iEncrypt = CIPHER_NONE;
    }

    //wpa+ccmp
    else if ((iAuth & WIFI_AUTH_PSK)
             && (iAuth & WIFI_AUTH_WPA)
             && !(iAuth & WIFI_AUTH_WPA2)
             && (iAuth & WIFI_AUTH_CCMP)
             && !(iAuth & WIFI_AUTH_TKIP))
    {
        iSercure = SERCURE_WPA_PSK;
        iEncrypt = CIPHER_CCMP;
    }
    //wpa+tkip
    else if ((iAuth & WIFI_AUTH_PSK)
             && (iAuth & WIFI_AUTH_WPA)
             && !(iAuth & WIFI_AUTH_WPA2)
             && !(iAuth & WIFI_AUTH_CCMP)
             && (iAuth & WIFI_AUTH_TKIP))
    {
        iSercure = SERCURE_WPA_PSK;
        iEncrypt = CIPHER_TKIP;
    }
    //wpa + tkip+ccmp
    else if ((iAuth & WIFI_AUTH_PSK)
             && (iAuth & WIFI_AUTH_WPA)
             && !(iAuth & WIFI_AUTH_WPA2)
             && (iAuth & WIFI_AUTH_CCMP)
             && (iAuth & WIFI_AUTH_TKIP))
    {
        iSercure = SERCURE_WPA_PSK;
        iEncrypt = CIPHER_AUTO;
    }
    //wpa2 + ccmp
    else if ((iAuth & WIFI_AUTH_PSK)
             && !(iAuth & WIFI_AUTH_WPA)
             && (iAuth & WIFI_AUTH_WPA2)
             && (iAuth & WIFI_AUTH_CCMP)
             && !(iAuth & WIFI_AUTH_TKIP))
    {
        iSercure = SERCURE_WPA2_PSK;
        iEncrypt = CIPHER_CCMP;
    }
    //wpa2+tkip
    else if ((iAuth & WIFI_AUTH_PSK)
             && !(iAuth & WIFI_AUTH_WPA)
             && (iAuth & WIFI_AUTH_WPA2)
             && !(iAuth & WIFI_AUTH_CCMP)
             && (iAuth & WIFI_AUTH_TKIP))
    {
        iSercure = SERCURE_WPA2_PSK;
        iEncrypt = CIPHER_TKIP;
    }
    //wpa2 + tkip+ccmp
    else if ((iAuth & WIFI_AUTH_PSK)
             && !(iAuth & WIFI_AUTH_WPA)
             && (iAuth & WIFI_AUTH_WPA2)
             && (iAuth & WIFI_AUTH_CCMP)
             && (iAuth & WIFI_AUTH_TKIP))
    {
        iSercure = SERCURE_WPA2_PSK;
        iEncrypt = CIPHER_AUTO;
    }
    //wpa+wpa2+ccmp
    else if ((iAuth & WIFI_AUTH_PSK)
             && (iAuth & WIFI_AUTH_WPA)
             && (iAuth & WIFI_AUTH_WPA2)
             && (iAuth & WIFI_AUTH_CCMP)
             && !(iAuth & WIFI_AUTH_TKIP))
    {
        iSercure = SERCURE_WPA2_PSK;
        iEncrypt = CIPHER_CCMP;
    }
    //wpa+wpa2+tkip
    else if ((iAuth & WIFI_AUTH_PSK)
             && (iAuth & WIFI_AUTH_WPA)
             && (iAuth & WIFI_AUTH_WPA2)
             && !(iAuth & WIFI_AUTH_CCMP)
             && (iAuth & WIFI_AUTH_TKIP))
    {
        iSercure = SERCURE_WPA2_PSK;
        iEncrypt = CIPHER_TKIP;
    }
    //wpa+wpa2+tkip+ccmp
    else if ((iAuth & WIFI_AUTH_PSK)
             && (iAuth & WIFI_AUTH_WPA)
             && (iAuth & WIFI_AUTH_WPA2)
             && (iAuth & WIFI_AUTH_CCMP)
             && (iAuth & WIFI_AUTH_TKIP))
    {
        iSercure = SERCURE_WPA2_PSK;
        iEncrypt = CIPHER_AUTO;
    }
    else if (iAuth & WIFI_AUTH_EAP)
    {
        // 设置secure mode
        if (iAuth & WIFI_AUTH_WPA)
        {
            iSercure = SERCURE_WPA_EAP;
        }
        else if (iAuth & WIFI_AUTH_WPA2)
        {
            iSercure = SERCURE_WPA2_EAP;
        }

        // 设置cipher type
        if ((iAuth & WIFI_AUTH_CCMP) && (iAuth & WIFI_AUTH_TKIP))
        {
            iEncrypt = CIPHER_AUTO;
        }
        else if (iAuth & WIFI_AUTH_CCMP)
        {
            iEncrypt = CIPHER_CCMP;
        }
        else if (iAuth & WIFI_AUTH_TKIP)
        {
            iEncrypt = CIPHER_TKIP;
        }
    }
    else
    {
        iSercure = SERCURE_NONE;
        iEncrypt = CIPHER_NONE;
    }
}

/*
* Return the channel number for a given frequency and base frequency.
* The returned channel number is relative to the given base frequency.
* If the given base frequency is zero, a base frequency of 5 GHz is assumed for
* frequencies from 5 - 6 GHz, and 2.407 GHz is assumed for 2.4 - 2.5 GHz.
*
* Frequency is specified in MHz.
* The base frequency is specified as (start_factor * 500 kHz).
* Constants WF_CHAN_FACTOR_2_4_G, WF_CHAN_FACTOR_5_G are defined for
* 2.4 GHz and 5 GHz bands.
*
* The returned channel will be in the range [1, 14] in the 2.4 GHz band
* and [0, 200] otherwise.
* -1 is returned if the start_factor is WF_CHAN_FACTOR_2_4_G and the
* frequency is not a 2.4 GHz channel, or if the frequency is not and even
* multiple of 5 MHz from the base frequency to the base plus 1 GHz.
*
* Reference 802.11 REVma, section 17.3.8.3, and 802.11B section 18.4.*/
int CWiFiData::GenerateChannel(int iFrequence, int iStartfactor/* = 0*/)
{
    int ch = -1;
    UINT base;
    int offset;
    /* take the default channel start frequency */
    if (iStartfactor == 0)
    {
        if (iFrequence >= 2400
                && iFrequence <= 2500)
        {
            iStartfactor = WF_CHAN_FACTOR_2_4_G;
        }
        else if (iFrequence >= 5000
                 && iFrequence <= 6000)
        {
            iStartfactor = WF_CHAN_FACTOR_5_G;
        }
    }
    if (iFrequence == 2484
            && iStartfactor == WF_CHAN_FACTOR_2_4_G)
    {
        return 14;
    }
    base = iStartfactor / 2;
    /* check that the frequency is in 1GHz range of the base */
    if ((iFrequence < base)
            || (iFrequence > base + 1000))
    {
        return -1;
    }
    offset = iFrequence - base;
    ch = offset / 5;
    /* check that frequency is a 5MHz multiple from the base */
    if (offset != (ch * 5))
    {
        return -1;
    }
    /* restricted channel range check for 2.4G */
    if (iStartfactor == WF_CHAN_FACTOR_2_4_G
            && (ch < 1 || ch > 13))
    {
        return -1;
    }
    return ch;
}

//获取一个可用的section id
int CWiFiData::GetAvailableSectionID()
{
    for (int i = 1; i <= WIFI_SAVED_MAX; i++)
    {
        WiFiNetInfoItem it = m_listSavedNet.begin();
        for (; it != m_listSavedNet.end(); it++)
        {
            if (i == (*it).iSectionID)
            {
                break;
            }
        }

        if (it == m_listSavedNet.end())
        {
            WIRE_INFO("get a avaliable section id[%d]", i);
            return i;
        }
    }

    return WIFI_SECTION_ID_INVAILED;
}

//从section中获取值
bool CWiFiData::GetValueFromSection(int iSectionID, const yl::string & strKeyName,
                                    yl::string & strValue)
{
    strValue = configParser_GetCfgItemStringValue(strKeyName.c_str(), iSectionID);
    return true;
}

//计算安全模式
WIFI_SERCURE_MODE CWiFiData::GetSercureMode(int iSectionID)
{
    yl::string strSercure;
    GetValueFromSection(iSectionID, kszWiFiSecureMode, strSercure);
    if ("NONE" == strSercure)
    {
        return SERCURE_NONE;
    }
    else if ("WEP" == strSercure)
    {
        return SERCURE_WEP;
    }
    else if ("WPA-PSK" == strSercure)
    {
        return SERCURE_WPA_PSK;
    }
    else if ("WPA2-PSK" == strSercure)
    {
        return SERCURE_WPA2_PSK;
    }
    else if ("WPA-EAP" == strSercure)
    {
        return SERCURE_WPA_EAP;
    }
    else if ("WPA2-EAP" == strSercure)
    {
        return SERCURE_WPA2_EAP;
    }
    else
    {
        return  SERCURE_NONE;
    }
}

//计算加密类型
WIFI_ENCRYPT_TYPE CWiFiData::GetEncrptType(int iSectionID)
{
    yl::string strPair;
    GetValueFromSection(iSectionID, kszWiFiEncryptType, strPair);

    if ("NONE" == strPair)
    {
        return CIPHER_NONE;
    }
    else if ("TKIP" == strPair)
    {
        return CIPHER_TKIP;
    }
    else if ("CCMP" == strPair)
    {
        return CIPHER_CCMP;
    }
    else if ("CCMP TKIP" == strPair
             || "TKIP CCMP" == strPair)
    {
        return CIPHER_AUTO;
    }
    else
    {
        return CIPHER_NONE;
    }
}

//bool CWiFiData::ReadNetworkInfo(int iSectionID, const yl::string& strKeyName, yl::string& strValue)
//{
//  strValue = configParser_GetCfgItemStringValue(strKeyName.c_str(), iSectionID);
//  return true;
//}

bool CWiFiData::ReadNetworkInfo(int iSectionID, WiFiNetInfo & netInfo)
{
    netInfo.strSSID = configParser_GetCfgItemStringValue(kszWiFiName, iSectionID);
    if (netInfo.strSSID.empty())
    {
        return false;
    }
    netInfo.iPriority = configParser_GetCfgItemIntValue(kszWiFiPriority, iSectionID);
    netInfo.strProfileName = configParser_GetCfgItemStringValue(kszWiFiProfileName, iSectionID);
    netInfo.iSercureMode = GetSercureMode(iSectionID);
    netInfo.iEncrypt = GetEncrptType(iSectionID);
    netInfo.strPsk = configParser_GetCfgItemStringValue(kszWiFiPassword, iSectionID);
    netInfo.strEapName = configParser_GetCfgItemStringValue(kszWiFiEapName, iSectionID);
    netInfo.strEapPwd = configParser_GetCfgItemStringValue(kszWiFiEapPassword, iSectionID);

    if (SERCURE_NONE == netInfo.iSercureMode)
    {
        netInfo.strPsk.clear();
        netInfo.strEapName.clear();
        netInfo.strEapPwd.clear();
    }
    return true;
}

bool CWiFiData::WriteNetworkInfo(int iSectionID, const WiFiNetInfo & netInfo)
{
    //CIPHER
    yl::string strPairwise = "";
    if (CIPHER_AUTO == netInfo.iEncrypt)
    {
        strPairwise = "CCMP TKIP";
    }
    else if (CIPHER_CCMP == netInfo.iEncrypt)
    {
        strPairwise = "CCMP";
    }
    else if (CIPHER_TKIP == netInfo.iEncrypt)
    {
        strPairwise = "TKIP";
    }
    else
    {
        strPairwise = "NONE";
    }

    // PSK && KEY_MGMT
    yl::string strEapName = netInfo.strEapName;
    yl::string strEapPwd = netInfo.strEapPwd;
    yl::string strPsk = netInfo.strPsk;
    yl::string strKeymgmt = "NONE";

    if ((SERCURE_WPA_PSK == netInfo.iSercureMode))
    {
        strKeymgmt = "WPA-PSK";
    }
    else if (SERCURE_WPA2_PSK == netInfo.iSercureMode)
    {
        strKeymgmt = "WPA2-PSK";
    }
    else if (SERCURE_WEP == netInfo.iSercureMode)
    {
        strKeymgmt = "WEP";
        strPairwise = "WEP";
    }
    else if (SERCURE_WPA_EAP == netInfo.iSercureMode)
    {
        strKeymgmt = "WPA-EAP";
    }
    else if (SERCURE_WPA2_EAP == netInfo.iSercureMode)
    {
        strKeymgmt = "WPA2-EAP";
    }
    else if (SERCURE_NONE == netInfo.iSercureMode)
    {
        strKeymgmt = "NONE";
        strPsk = "";
        strEapPwd = "";
    }
    else
    {
        strPsk = "";
        strEapPwd = "";
    }

    // 先清除原有配置
    ClearNetworkConfigInfo(iSectionID);

    configParser_SetCfgItemIntValue(kszWiFiPriority, iSectionID, netInfo.iPriority, CONFIG_LEVEL_PHONE);
    configParser_SetCfgItemStringValue(kszWiFiName, iSectionID, netInfo.strSSID.c_str(),
                                       CONFIG_LEVEL_PHONE);
    configParser_SetCfgItemStringValue(kszWiFiProfileName, iSectionID, netInfo.strProfileName.c_str(),
                                       CONFIG_LEVEL_PHONE);
    configParser_SetCfgItemStringValue(kszWiFiSecureMode, iSectionID, strKeymgmt.c_str(),
                                       CONFIG_LEVEL_PHONE);
    configParser_SetCfgItemStringValue(kszWiFiEncryptType, iSectionID, strPairwise.c_str(),
                                       CONFIG_LEVEL_PHONE);
    if (SERCURE_WPA_EAP == netInfo.iSercureMode
            || SERCURE_WPA2_EAP == netInfo.iSercureMode)
    {
        configParser_SetCfgItemStringValue(kszWiFiEapName, iSectionID, strEapName.c_str(),
                                           CONFIG_LEVEL_PHONE);
        configParser_SetCfgItemStringValue(kszWiFiEapPassword, iSectionID, strEapPwd.c_str(),
                                           CONFIG_LEVEL_PHONE);
#if 0 // eap type目前不需要上层控制
        configParser_SetCfgItemStringValue(kszWiFiEapType, iSectionID, netInfo.strEapType.c_str(),
                                           CONFIG_LEVEL_PHONE);
#endif
    }
    else
    {
        configParser_SetCfgItemStringValue(kszWiFiPassword, iSectionID, strPsk.c_str(), CONFIG_LEVEL_PHONE);
    }

    return true;
}

bool CWiFiData::WriteNetworkInfo(int iSectionID, const yl::string & strKeyName, const int iValue)
{
    if (strKeyName.empty())
    {
        return false;
    }
    WIRE_INFO("write section[%d] key[%s] value[%d]", iSectionID, strKeyName.c_str(), iValue);

    configParser_SetCfgItemIntValue(strKeyName.c_str(), iSectionID, iValue, CONFIG_LEVEL_PHONE);
    return true;
}

// 清除配置
bool CWiFiData::ClearNetworkConfigInfo(int iSectionID)
{
    configParser_SetCfgItemStringValue(kszWiFiEapName, iSectionID, "", CONFIG_LEVEL_PHONE);
    configParser_SetCfgItemStringValue(kszWiFiEapPassword, iSectionID, "", CONFIG_LEVEL_PHONE);
    configParser_SetCfgItemStringValue(kszWiFiPassword, iSectionID, "", CONFIG_LEVEL_PHONE);
#if 0
    configParser_SetCfgItemStringValue(kszWiFiEapType, iSectionID, "", CONFIG_LEVEL_PHONE);
#endif
    return true;
}

#endif // IF_SUPPORT_WIFI
