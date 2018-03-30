#ifdef IF_SUPPORT_WIFI
///////////////////////////////////////////////////////////
//  CWifiDriver.h
//  Implementation of the Class CWifiDriver
//  Created on:      15-五月-2014 9:18:03
//  Original author: songk
///////////////////////////////////////////////////////////
#ifndef CWIFIDRIVER__INCLUDE_H
#define CWIFIDRIVER__INCLUDE_H
#include "service_system.h"

class CWifiDriver
{
    DECLARE_SINGLETON_CLASS(CWifiDriver)

public:
    //关闭无线网路
    bool CloseWifi();

    //切换为wifi网路
    bool OpenWifi();

    //刷新数据
    bool RefreshState();

    //保存配置
    bool SaveConfig(int iSectionID, int  iNetID);

    //开始连接
    bool StartConnect(int iNetID, const AuthInfo & authInfo);

    //开始断开
    bool StartDisconnect(int iNetID);

    //开始搜索
    bool StartScan(int iLoop, bool bRest);

    //停止搜索
    bool StopScan();

    //开始周期扫描
    bool StartCycleScan();

    //停止周期扫描
    bool StopCycleScan();

    //wps
    bool StartWps();

    //刷新WPS-PIN code
    bool RefreshPinCode();

    //wps-pin
    bool StartWpspin();

    //停止wps
    bool StopWps(int iType);

    //适配器是否可用
    bool IsDongleValid(bool bCheck = false);

    //通过section id 获取 net id
    int GetNetIDBySectionID(int iSectionID, const yl::string & strSSID);

    //设置Dongle状态
    void SetDongleEnable(bool bEnable)
    {
        m_bAdapterValid = bEnable;
    }

    //获取网络状态
    bool GetNetworkStatus(_networkStatusInfo & statusInfo);

    //获取扫描列表
    bool GetNeworkList(YLList<_networkPropertyInfo> & networkList);

    //获取Dongle Mac地址
    bool GetDongleMac(yl::string & strMac);

protected:
    // 切换为有线网路
    bool OpenCable();
    //关闭有线网路
    bool CloseCable();
    //查询适配器状态
    bool CheckDongleStatus();
    //适配器是否可用
    bool m_bAdapterValid;
};

#define _WifiDriver GET_SINGLETON_INSTANCE(CWifiDriver)

#endif // !defined(CWIFIDRIVER__INCLUDE_H)
#endif // IF_SUPPORT_WIFI
