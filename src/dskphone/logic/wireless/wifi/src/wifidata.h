#ifdef IF_SUPPORT_WIFI
///////////////////////////////////////////////////////////
//  CWiFiData.h
//  Implementation of the Class CWiFiData
//  Created on:      15-五月-2014 9:17:54
//  Original author: songk
///////////////////////////////////////////////////////////

#ifndef CWIFIDATA__INCLUDED_H
#define CWIFIDATA__INCLUDED_H

#include "wireless/wifi/include/wifidefines.h"

class CWiFiData
{
public:
    CWiFiData();
    virtual ~CWiFiData();

    //添加新的网络到已保存列表
    bool AddNewNetwork(WiFiNetInfo * pNetworkInfo);

    //删除网络
    bool DeleteNetwork(int iNetID, bool bNotify = true);

    //修改网络的连接信息
    bool ModifyNetwork(int iNetID, const WiFiNetInfo * pNetInfo);

    //设置自动连接优先级
    bool SetNetworkPriority(int iNetID, bool bUp);

    //移动设备位置
    bool MoveNetworkPostion(int iNetID, int iPos);

    //更新当前网络
    bool UpdateScanInfoList();

    //获取已保存的网络连接信息
    bool GetSavedNetInfoList(UI_WIFI_NETINFO_LIST & listInfo);

    //获取最近一次搜索结果列表
    bool GetScanedNetInfoList(UI_WIFI_NETINFO_LIST & listInfo);

    //获取当前网络的连接状态
    bool GetConnectedNetInfo(WiFiNetInfo & netInfo, int iNetID = WIFI_NET_ID_INVAILED);

    //通过ID获取网络信息
    WiFiNetInfo * GetNetInfoByID(int iNetID, bool bSaved = false);

    //通过SSID获取网络信息
    WiFiNetInfo * GetNetInfoBySSID(const yl::string & strSSID, bool bSaved = true);

    //更新本地数据
    bool RefreshData(bool bSync = false, bool bUpdateWnd = false);

    //设置网络的连接状态
    bool SetNetworkState(int iNetID, WIRELESS_DEVICE_STATE eState);

    //获取一个网络的状态
    WIRELESS_DEVICE_STATE GetNetworkState(int iNetID);

    //打印扫描到的网络信息
    void PrintScanNetInfo(const WiFiNetInfo & scanInfo);
    void PrintSavedNetInfo(const WiFiNetInfo & savedInfo);

    //重新载入配置
    bool ReloadConfig();

    //重新设置优先级
    //bNeedSorted[列表是否需要排序] || bNotify[是否需要通知rtServer]
    bool ResetPriority(UI_WIFI_NETINFO_LIST & listInfo, bool bNeedSort = false, bool bNotify = true);

    //从共享内存获取当前ID信息
    int  GetConnectedNetID();

    //清内存数据
    void ClearNetworkInfo();

    //获取WPS-PIN PIN码
    void GetWifiWpsPinCode(yl::string & strPinCode);

    // 拷贝网络信息
    void CopySavedNetInfo(WiFiNetInfo & savedInfo, const sys_wifi_network * pWifiNetwork);

protected:
    //获取加密方式与安全模式
    void GenerateSercure(WIFI_SERCURE_MODE & iSercure, WIFI_ENCRYPT_TYPE & iEncrypt, int iAuth,
                         int iCap);

    //获取信号强度
    int  GetSignalNevel(int iSignal);

    //获取连接速度
    int GetConnectedNetSpeed();

    //网络的Channel值
    int  GenerateChannel(int iFrequence, int iStartfactor = 0);

    //检查网络是否可保存
    bool IsSaveAvailable(const WiFiNetInfo & savedInfo);

    //获取一个可用的section id
    int  GetAvailableSectionID();

    //读取一个section里的配置
    bool LoadConfig(int iSectionID);

    //读配置文件
    bool ReadNetworkInfo(int iSectionID, WiFiNetInfo & netInfo);
    //bool ReadNetworkInfo(int iSectionID, const yl::string& strKeyName, yl::string& strValue);

    //写配置日志文件
    bool WriteNetworkInfo(int iSectionID, const WiFiNetInfo & netInfo);
    bool WriteNetworkInfo(int iSectionID, const yl::string & strKeyName, const int iValue);

    //清除配置
    bool ClearNetworkConfigInfo(int iSectionID);

    bool GetValueFromSection(int iSectionID, const yl::string & strKeyName, yl::string & strValue);
    WIFI_SERCURE_MODE GetSercureMode(int iSectionID);
    WIFI_ENCRYPT_TYPE GetEncrptType(int iSectionID);

    //更新扫描网络数据
    void MapNetworkInfo(_networkInfo & networkInfo);

    //数据转换
    bool MapNetworkPropertyInfo(const _networkPropertyInfo & netInfo, WiFiNetInfo & wifiNetInfo);

protected:
    //已保存的网络连接信息列表
    UI_WIFI_NETINFO_LIST  m_listSavedNet;

    //扫描到的网络状态信息列表
    UI_WIFI_NETINFO_LIST m_listScanedNet;
};

#endif // !defined(CWIFIDATA__INCLUDED_H)
#endif // IF_SUPPORT_WIFI
