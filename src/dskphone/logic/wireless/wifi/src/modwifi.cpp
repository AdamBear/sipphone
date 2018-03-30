#ifdef IF_SUPPORT_WIFI
#include "wifi_inc.h"

//模块初始化
bool WIFI_Init()
{
    return _WiFiManager.Init();
}

//适配器是否可用
bool WIFI_IsDongleVaild()
{
    return _WiFiManager.IsDongleValid();
}

//有线是否可用
bool WIFI_IsCableConnected()
{
    return netIsCableConnected();
}

//开关状态
bool WIFI_IsEnable(bool bIsEnableFromMem /* = true */)
{
    if (bIsEnableFromMem)
    {
        return _WiFiManager.IsEnable();
    }

    return configParser_GetConfigInt(kszWifiSwitch) == 1 ? true : false;
}

//是否已连接
bool WIFI_IsConnected()
{
    return _WiFiManager.IsConnected();
}

//网络是否连接中
bool WIFI_IsConnecting(int iNetID/* = WIFI_NET_ID_INVAILED*/)
{
    return _WiFiManager.IsConnecting(iNetID);
}

//是否在搜索状态
bool WIFI_IsScan()
{
    return _WiFiManager.IsInScan();
}

//获取搜索到的网络列表
bool WIFI_GetScanNetworkList(UI_WIFI_NETINFO_LIST & listNetInfo)
{
    return _WiFiData.GetScanedNetInfoList(listNetInfo);
}

//获取搜索到的网络列表的指定区域
int WIFI_GetScanNetworkList(UI_WIFI_NETINFO_LIST & listNetInfo, int iStart, int iEnd)
{
    if (iStart < 0 || iEnd < 0 || iStart > iEnd)
    {
        return -1;
    }

    UI_WIFI_NETINFO_LIST listNetInfoBuf;
    _WiFiData.GetScanedNetInfoList(listNetInfoBuf);

    int iCount = 0;
    for (WiFiNetInfoItem itor = listNetInfoBuf.begin();
            itor != listNetInfoBuf.end();
            itor ++, iCount++)
    {
        if (iCount >= iStart && iCount <= iEnd)
        {
            listNetInfo.push_back(*itor);
        }
    }

    return listNetInfoBuf.size();
}

//获取已保存的网络列表
bool WIFI_GetSavedNetworkList(UI_WIFI_NETINFO_LIST & listNetInfo)
{
    return _WiFiData.GetSavedNetInfoList(listNetInfo);
}

//获取已保存网络数量
int WIFI_GetSavedNetworkNum()
{
    return _WiFiManager.GetSavedNetworkNum();
}

//获取当前网络信息
bool WIFI_GetCurrentNetConnectInfo(WiFiNetInfo & netInfo)
{
    return _WiFiManager.GetConnectedNetInfo(netInfo);
}

//获取网络信息
bool WIFI_GetNetworkInfoByID(int iNetID, WiFiNetInfo & netInfo)
{
    WiFiNetInfo * pScanInfo = _WiFiData.GetNetInfoByID(iNetID);
    WiFiNetInfo * pSavedInfo = _WiFiData.GetNetInfoByID(iNetID, true);

    if (NULL == pSavedInfo
            && NULL == pScanInfo)
    {
        return false;
    }

    //保存的网络可能不存在于扫描列表
    if (NULL == pScanInfo)
    {
        netInfo = *pSavedInfo;
        return true;
    }

    netInfo = *pScanInfo;

    //扫描列表与保存列表都存在时，需要从保存列表获取密码
    if (NULL != pSavedInfo)
    {
        netInfo.strPsk = pSavedInfo->strPsk;
    }
    return true;
}

// 获取保存的网络信息
bool WIFI_GetSavedNetInfoByID(int iNetID, WiFiNetInfo & netInfo)
{
    WiFiNetInfo * pSavedInfo = _WiFiData.GetNetInfoByID(iNetID, true);

    if (!pSavedInfo)
    {
        return false;
    }

    netInfo = *pSavedInfo;
    return true;
}

//获取WPS-PIN CODE
bool WIFI_GetWpsPin(yl::string & strPin)
{
    return _WiFiManager.GetWpsPinCode(strPin);
}

//开关WIFI
bool WIFI_SetEnable(bool bEnable, bool bNotify/* = false*/)
{
    return _WiFiManager.SetEnable(bEnable, bNotify);
}

//开始搜索
bool WIFI_StartScan()
{
    return _WiFiManager.ProcessScan();
}

//停止搜索
bool WIFI_StopScan()
{
    return _WiFiManager.ProcessStopScan();
}

//开始连接
bool WIFI_StartConnect(int iNetID)
{
    return _WiFiManager.ProcessConnect(iNetID);
}

bool WIFI_StartConnect(const yl::string & strSSID)
{
    WiFiNetInfo * pNetInfo = _WiFiData.GetNetInfoBySSID(strSSID);
    if (NULL == pNetInfo)
    {
        return false;
    }

    return WIFI_StartConnect(pNetInfo->iNetID);
}

//取消连接
bool WIFI_CancelConnect(int iNetID)
{
    return _WiFiManager.ProcessCancelConnect(iNetID);
}

//断开连接
bool WIFI_StartDisconnect(int iNetID)
{
    return _WiFiManager.ProcessDisconnect(iNetID);
}

bool WIFI_StartDisconnect(const yl::string & strSSID)
{
    WiFiNetInfo * pNetInfo = _WiFiData.GetNetInfoBySSID(strSSID);
    if (NULL == pNetInfo)
    {
        return false;
    }

    return WIFI_StartDisconnect(pNetInfo->iNetID);
}

//添加网络
bool WIFI_AddNetwork(WiFiNetInfo * pNetInfo)
{
    if (NULL == pNetInfo)
    {
        return false;
    }

    return _WiFiData.AddNewNetwork(pNetInfo);
}

//设置优先级
bool WIFI_AdjustPosition(int iNetID, bool bUp)
{
    return _WiFiData.SetNetworkPriority(iNetID, bUp);
}

//调整位置
bool WIFI_MovePosition(int iNetID, int iPos)
{
    return _WiFiData.MoveNetworkPostion(iNetID, iPos);
}

//删除网络
bool WIFI_DeleteNetwork(int iNetID)
{
    return _WiFiManager.DeleteNetwork(iNetID);
}

//删除所有网络
bool WIFI_DeleteAllNetwork()
{
    UI_WIFI_NETINFO_LIST listNetInfo;
    if (WIFI_GetSavedNetworkNum() == 0)
    {
        return false;
    }
    WIFI_GetSavedNetworkList(listNetInfo);

    for (WiFiNetInfoItem itor = listNetInfo.begin();
            itor != listNetInfo.end();
            ++itor)
    {
        WiFiNetInfo & netInfo = (*itor);

        _WiFiManager.DeleteNetwork(netInfo.iNetID, false);
    }

    //只需要通知rtServer一次
    WLAsyncHelper_WifiAsync(WIFI_TASK_CONFIG_CHANGE);

    return WIFI_GetSavedNetworkNum() == 0;
}

//编辑网络信息
bool WIFI_ModifyNetwork(int iNetID, const WiFiNetInfo * pNeConnectInfo)
{
    if (NULL == pNeConnectInfo)
    {
        return false;
    }
    return _WiFiData.ModifyNetwork(iNetID, pNeConnectInfo);
}

//输入网络密码
bool WIFI_AnswerPassword(int iNetID, const yl::string& strPsk)
{
    WIRE_INFO("Network answer password.net id[%d]", iNetID);

    AuthInfo authInfo;
    authInfo.strPsk = strPsk;
    return _WiFiManager.AuthNetwork(iNetID, authInfo);
}

// 网络授权
bool WIFI_AuthNetwork(int iNetID, const yl::string & strAuthName, const yl::string & strAuthPwd)
{
    WIRE_INFO("Auth network.net id[%d], name[%s]", iNetID, strAuthName.c_str());

    if (strAuthName.empty())
    {
        return false;
    }

    AuthInfo authInfo;
    authInfo.strEapName = strAuthName;
    authInfo.strEapPwd = strAuthPwd;
    return _WiFiManager.AuthNetwork(iNetID, authInfo, true);
}

//刷新网络状态信息
bool WIFI_RefreshData()
{
    return _WiFiData.RefreshData(true);
}

//模拟硬按键通知WPS-PBC消息通知
bool WIFI_NotifyWpsPbc()
{
    return etl_NotifyApp(false, SYS_MSG_WIFI_WPS_PBC_CATCH, 0, 0);
}

//开启wps-pin
bool WIFI_StartWpspin()
{
    return _WiFiManager.ProcessWPSConnect(SESSION_WPSPIN_STATE);
}

//开启WPS-PBC(wps硬按键)
bool WIFI_StartWps()
{
    return _WiFiManager.ProcessWPSConnect(SESSION_WPS_STATE);
}

//关闭wps/wps-pin功能
bool WIFI_CancelWps()
{
    return _WiFiManager.ProcessCancelWPSConnect();
}

//获取dongle mac地址
bool WIFI_GetLocalMac(yl::string & strMac)
{
    if (!_WiFiManager.IsDongleValid(true))
    {
        return false;
    }

    return _WifiDriver.GetDongleMac(strMac);
}

bool WIFI_IsBuiltInWifi()
{
    return PT_T49 == devicelib_GetPhoneType()
           || PT_CP920 == devicelib_GetPhoneType();
}

#else
#include "modwifi.h"
//初始化
bool WIFI_Init()
{
    return false;
}

//适配器是否可用
bool WIFI_IsDongleVaild()
{
    return false;
}

//有线是否可用
bool WIFI_IsCableConnected()
{
    return false;
}

//是否正在搜索
bool WIFI_IsScan()
{
    return false;
}

//是否已连接
bool WIFI_IsConnected()
{
    return false;
}

//通过NetID判断某个网络是否在连接
bool WIFI_IsConnecting(int iNetID/* = WIFI_NET_ID_INVAILED*/)
{
    return false;
}

//开关状态
bool WIFI_IsEnable(bool bIsEnableFromMem /* = true */)
{
    return false;
}

//搜索到的网络列表
bool WIFI_GetScanNetworkList(UI_WIFI_NETINFO_LIST & listNetInfo)
{
    return false;
}

//已保存的网络列表
bool WIFI_GetSavedNetworkList(UI_WIFI_NETINFO_LIST & listNetStat)
{
    return false;
}

//已保存的网络数量
int  WIFI_GetSavedNetworkNum()
{
    return 0;
}

//当前网络的连接
bool WIFI_GetCurrentNetConnectInfo(WiFiNetInfo & netInfo)
{
    return false;
}

//获取网络信息
bool WIFI_GetNetworkInfoByID(int iNetID, WiFiNetInfo & netInfo)
{
    return false;
}

// 获取保存的网络信息
bool WIFI_GetSavedNetInfoByID(int iNetID, WiFiNetInfo & netInfo)
{
    return false;
}

//开关wifi
bool WIFI_SetEnable(bool bEnable, bool bNotify/* = false*/)
{
    return false;
}

//开始搜索
bool WIFI_StartScan()
{
    return false;
}

//停止搜索
bool WIFI_StopScan()
{
    return false;
}

//开始连接
bool WIFI_StartConnect(int iNetID)
{
    return false;
}

bool WIFI_StartConnect(const yl::string & strSSID)
{
    return false;
}
//取消连接
bool WIFI_CancelConnect(int iNetID)
{
    return false;
}

//断开连接
bool WIFI_StartDisconnect(int iNetID)
{
    return false;
}

bool WIFI_StartDisconnect(const yl::string & strSSID)
{
    return false;
}

//添加网络
bool WIFI_AddNetwork(WiFiNetInfo * pNetInfo)
{
    return false;
}

//设置连接优先级,true/false 上移/下移
bool WIFI_AdjustPosition(int iNetID, bool bUp)
{
    return false;
}

//调整位置
bool WIFI_MovePosition(int iNetID, int iPos)
{
    return false;
}

//删除网络
bool WIFI_DeleteNetwork(int iNetID)
{
    return false;
}

//删除所有网络
bool WIFI_DeleteAllNetwork()
{
    return false;
}

//编辑网络信息
bool WIFI_ModifyNetwork(int iNetID, const WiFiNetInfo * pNetInfo)
{
    return false;
}

//输入网络密码
bool WIFI_AnswerPassword(int iNetID, const yl::string& strPsk)
{
    return false;
}

bool WIFI_AuthNetwork(int iNetID, const yl::string & strAuthName, const yl::string & strAuthPwd)
{
    return false;
}

//刷新网络数据
bool WIFI_RefreshData()
{
    return false;
}

//获取WPS-PIN码
bool WIFI_GetWpsPin(yl::string & strPin)
{
    return false;
}

//模拟硬按键通知WPS-PBC消息通知
bool WIFI_NotifyWpsPbc()
{
    return false;
}

//开启WPS-PIN
bool WIFI_StartWpspin()
{
    return false;
}

//开启WPS-PBC
bool WIFI_StartWps()
{
    return false;
}

//关闭WPS功能
bool WIFI_CancelWps()
{
    return false;
}

//获取dongle mac地址
bool WIFI_GetLocalMac(yl::string & strMac)
{
    return false;
}

bool WIFI_IsBuiltInWifi()
{
    return false;
}
#endif // IF_SUPPORT_WIFI
