///////////////////////////////////////////////////////////
//  CWIFIDefine.h
//  Created on:      13-五月-2014 20:00:13
//  Original author: songk
///////////////////////////////////////////////////////////
#ifndef CWIFIDEFINES__INCLUDE_H
#define CWIFIDEFINES__INCLUDE_H

#include <service_system.h>
#include "extended/wifimessagedefine.h"
#include <singletonclassdefine.h>

#include "wireless/common/wirelessdefines.h"

#define  WIFI_WAITTING_TIMER (3*1000)
#define  WIFI_AUTOCONNECT_TIMER (15*1000)

// http://10.3.5.199/Bug.php?BugID=57105                     在wifi管理界面，点击某个已连接热点断开后，最好提示正在连接其他热点
#define IF_BUG_57105                                0
#define IF_WIFI_GLOBAL_OFF                          1       //通过命令直接开关wifi，一般挂机时使用，正式版要去除
#define IF_SUPPORT_WIFI_CLYCLESCAN                  0       //wifi 周期性扫描
//wifi线程名
#define VP_WIFI_THREAD_NAME     "wifithread"

#define WIFI_SAVED_MAX              5 //
#define WIFI_SECTION_ID_INVAILED   -1 //
#define WIFI_MIN_PRIORITY           0 //最小优先级

#define WIFI_DEBUG_LOG              1 //wifi调试信息

//安全模式
enum WIFI_SERCURE_MODE
{
    SERCURE_DEFAULT = -1,
    SERCURE_NONE = 0,             //开放
    SERCURE_WEP,                       //WEP
    SERCURE_WPA,
    SERCURE_WPA2,
    SERCURE_WPA_PSK,             //WPA个人
    SERCURE_WPA2_PSK,
    SERCURE_WPA_EAP,     //WPA企业
    SERCURE_WPA2_EAP,
    SERCURE_WPS                        //
};

//加密类型
enum WIFI_ENCRYPT_TYPE
{
    CIPHER_DEFAULT = -1,
    CIPHER_NONE = 0,                 //无
    CIPHER_W40,                          //WEP40  (WEP)
    CIPHER_W104,                       //WEP104 (WEP)
    CIPHER_TKIP,                          //TKIP(WPA-PSK)
    CIPHER_CCMP,                       //AES(WPA2-PSK),AES
    CIPHER_AUTO,                        //自动(AES/TKIP)
    CIPHER_GCMP                        //GCMP(WAP)
};

//信号强度
enum WIFI_SINAL_NEVEL
{
    WIFI_SINAL_NEVEL1 = 0,
    WIFI_SINAL_NEVEL2,
    WIFI_SINAL_NEVEL3,
    WIFI_SINAL_NEVEL4,
    WIFI_SINAL_NEVEL5
};

enum WIFI_USER_OPERATE
{
    WIFI_SCAN_OPTION,       //UI扫描操作 0/1 停止/启动
};

//线程数据结构
typedef struct WIFI_TASK_PARAM
{
    long lSessionID;
    WPARAM lParam;
    LPARAM wParam;
    bool bResult;

    WIFI_TASK_PARAM(WPARAM wP = 0, LPARAM lP = 0, long lSessID = -1)
    {
        wParam = wP;
        lParam = lP;
        lSessionID = lSessID;
    }

} WiFiTaskParam;

#define WIFI_MIN_SIGNAL             -85
#define WIFI_LOW_SIGNAL             -75
#define WIFI_MID_SIGNAL             -65
#define WIFI_HIGH_SIGNAL            -55

typedef struct WIFI_NETINFO
{
    //  网络ID
    int iNetID;
    //承载网络的设备ID
    int iDeviceID;
    // 网络的连接速度
    int iSpeed;
    //capability information field in host byte order
    int iCapbility;
    //section id
    int iSectionID;
    //priority
    int iPriority;
    //加密方式
    WIFI_ENCRYPT_TYPE iEncrypt;
    //安全模式
    WIFI_SERCURE_MODE iSercureMode;
    //加密方式(暂不可用)
    WIFI_ENCRYPT_TYPE iCipherGroup;
    //是否可自动连接
    bool bEnable;
    //是否隐藏SSID
    bool bScanSSID;
    //已连接网络的频率
    int iFrequence;
    //信号强度
    int iSignal;
    //信号强度原始值
    int iBaseSignal;
    //网络所在频段，与iFrequence是对应的
    int iChannel;
    //网络的Qos，驱动目前实现不了，留空。
    int iQos;
    //网络的连接状态
    WIRELESS_DEVICE_STATE eNetState;
    //网络的Mac
    yl::string strMac;
    //网络的SSID
    yl::string strSSID;
    //profile name
    yl::string strProfileName;
    // psk密码
    yl::string strPsk;
    //eap name
    yl::string strEapName;
    //eap pwd
    yl::string strEapPwd;
    // eap type
    yl::string strEapType;

    void Reset()
    {
        iNetID = WIFI_NET_ID_INVAILED;
        iDeviceID = WIFI_NET_ID_INVAILED;
        iSectionID = WIFI_SECTION_ID_INVAILED;
        iPriority = 0;
        iSpeed = 0;
        iCapbility = 0;
        iFrequence = 0;
        iChannel = 0;
        iSignal = WIFI_SINAL_NEVEL1;
        iQos = 0;
        eNetState = WIRE_DEVICE_INIT_STATE;
        iSercureMode = SERCURE_NONE;
        iCipherGroup = CIPHER_NONE;
        iEncrypt = CIPHER_NONE;
        strSSID.clear();
        strPsk.clear();
        strMac.clear();
        strProfileName.clear();
        iBaseSignal = 0;
        strEapName = "";
        strEapPwd = "";
        strEapType = "";
    }

    //构造函数
    WIFI_NETINFO()
    {
        Reset();
    }

    WIFI_NETINFO & operator= (const WIFI_NETINFO & netInfo)
    {
        if (this != &netInfo)
        {
            iNetID = netInfo.iNetID;
            iDeviceID = netInfo.iDeviceID;
            iSectionID = netInfo.iSectionID;
            iPriority = netInfo.iPriority;
            iSpeed = netInfo.iSpeed;
            iCapbility = netInfo.iCapbility;
            iEncrypt = netInfo.iEncrypt;
            iSercureMode = netInfo.iSercureMode;
            iFrequence = netInfo.iFrequence;
            iSignal = netInfo.iSignal;
            iChannel = netInfo.iChannel;
            iQos = netInfo.iQos;
            eNetState = netInfo.eNetState;
            strMac = netInfo.strMac;
            strSSID = netInfo.strSSID;
            strProfileName = netInfo.strProfileName;
            strPsk = netInfo.strPsk;
            iBaseSignal = netInfo.iBaseSignal;
            strEapName = netInfo.strEapName;
            strEapPwd = netInfo.strEapPwd;
            strEapType = netInfo.strEapType;
        }

        return *this;
    }

    bool operator == (const WIFI_NETINFO & rParam)
    {
        if (this == &rParam)
        {
            return true;
        }

        return (iNetID == rParam.iNetID
                && strSSID == rParam.strSSID
                && iSercureMode == rParam.iSercureMode);
    }
} WiFiNetInfo;


//rtServer的中网络信息
struct _networkAuthInfo
{
    yl::string strIdentity;
    yl::string strPasswd;
    yl::string strPsk;
    yl::string strWepKey;
    yl::string strWpsPin;
    int iWepKeyIndex;
};

struct _networkStatusInfo
{
    int iDeviceID;
    int iLinkSpeed;
    int iPairCipher;
    int iGroupCipher;
    int iKeyMgmt;
    int iWpaState;
    yl::string strName;
    _networkAuthInfo authInfo;
};

struct _networkPropertyInfo
{
    int iNetID;
    int iStatus;
    int iDeviceID;
    int iCaps;
    int iAuth;
    int iFreq;
    int iSignal;
    yl::string strMac;
    yl::string strName;

    void Reset()
    {
        iNetID = WIFI_NET_ID_INVAILED;
        iStatus = WIFI_AP_INACTIVE;
        iDeviceID = WIFI_NET_ID_INVAILED;
        iCaps = WIFI_NET_ID_INVAILED;
        iAuth = WIFI_NET_ID_INVAILED;
        iFreq = 0;
        iSignal = 0;
        strMac.clear();
        strName.clear();
    }
};

struct _networkInfo
{
    bool bWiFiConnected;
    yl::string strDongleMac;
    YLList<_networkPropertyInfo> listNetInfo;
};

struct AuthInfo
{
    yl::string strEapName;
    yl::string strEapPwd;
    yl::string strPsk;

    AuthInfo()
    {
        Reset();
    }

    void Reset()
    {
        strEapName.clear();
        strEapPwd.clear();
        strPsk.clear();
    }

    AuthInfo & operator = (const AuthInfo & authInfo)
    {
        if (this != &authInfo)
        {
            strEapName = authInfo.strEapName;
            strEapPwd = authInfo.strEapPwd;
            strPsk = authInfo.strPsk;
        }
        return *this;
    }
};

//会话信息
struct WiFiSessionInfo
{
    int iNetID;                              //net id
    AuthInfo authInfo;
    bool bAutoConnect;
    bool bAutoAdd;
    bool bModifyProfile;
    bool bAddBeforeConnect;
    WiFiNetInfo netInfo;

    WiFiSessionInfo()
    {
        bAutoConnect = false;
        bAutoAdd = true;
        bModifyProfile = false;
        bAddBeforeConnect = false;
        iNetID = WIFI_NET_ID_INVAILED;
        authInfo.Reset();
    }

    WiFiSessionInfo & operator = (const WiFiSessionInfo & sessionInfo)
    {
        if (this != &sessionInfo)
        {
            bAutoAdd = sessionInfo.bAutoAdd;
            bAutoConnect = sessionInfo.bAutoConnect;
            bModifyProfile = sessionInfo.bModifyProfile;
            bAddBeforeConnect = sessionInfo.bAddBeforeConnect;
            iNetID = sessionInfo.iNetID;
            netInfo = sessionInfo.netInfo;
            authInfo = sessionInfo.authInfo;
        }
        return *this;
    }
};

typedef YLList<WiFiNetInfo> UI_WIFI_NETINFO_LIST;
typedef UI_WIFI_NETINFO_LIST::iterator WiFiNetInfoItem;

//rtServer通知Logic事件消息
enum WiFiConnectStatus
{
    WIFI_CALLBACK_EVENT_CONNECTED = 0, //连接成功
    WIFI_CALLBACK_EVENT_DISCONNECTED = 1,   //连接断开
    WIFI_CALLBACK_EVENT_CONNECTTING = 2,     //连接中
    WIFI_CALLBACK_EVENT_AUTHFAILURE = 3,   //密码错误
    WIFI_CALLBACK_EVENT_CONNECTFAILED = 4, //连接失败
    WIFI_CALLBACK_EVENT_DONGLE_REMOVED = 5,   //适配器被拔出
    WIFI_CALLBACK_EVENT_DONGLE_ADDED = 6,   //适配器被插入
    WIFI_CALLBACK_EVENT_INACTIVE = 7   //信号消失
};

enum WIFI_TASK_TYPE
{
    WIFI_TASK_NONE = -1,
    WIFI_TASK_CONFIG_CHANGE,
    WIFI_TASK_OPEN,
    WIFI_TASK_CLOSE,
    WIFI_TASK_SCAN_START,
    WIFI_TASK_SCAN_STOP,
    WIFI_TASK_CYCLE_SCAN_START,
    WIFI_TASK_CYCLE_SCAN_STOP,
    WIFI_TASK_REFRESH,
    WIFI_TASK_MAX
};

#endif // CWIFIDEFINES__INCLUDE_H
