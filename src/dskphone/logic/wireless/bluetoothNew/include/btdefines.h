#ifndef __CBLUETOOTHDEFINES_H
#define __CBLUETOOTHDEFINES_H

#include <singletonclassdefine.h>
#include <cstring>
#include "extended/bluetoothmessagedefine.h"
#include "interfacedefine.h"
#include "bluetooth/btkit_api.h"
#include "wireless/common/wirelessdefines.h"

//蓝牙子线程名称
#define VP_BLUETOOTH_THREAD_NAME       "btThread"
#define IF_BT_CONNECTING_OVERTIME 1    //连接超时

//是否为Mobile类型
#define _ISBTPHONE(x) ((x == BT_PHONE) || (x == BT_COMPUTER))
//是否支持自动连接的类型
#define IS_SUPPORT_AUTOCONNECT(x) (x == BT_PHONE)
//是否为耳机类型
#define _ISBTHEADSET(x) ((x == BT_HEADSET))
//列表最多保存
#define BT_MAX_SCAN  50
#define BT_MAX_SAVE  10
#define BT_DEVICE_MAC_INVAILED     ""
//扫描周期
#define BT_SCAN_TIME    (15 * 1000)
//自动连接间隔时长
#define BT_AUTOCONNECT_LONG_TIME    (10 * 1000)
#define BT_AUTOCONNECT_SHORT_TIME   (8 * 1000)
//自动连接次数
#define BT_AUTOCONEECT_COUNT        10

#define BT_INIT_ADAPTER_INTERVAL (6 * 1000)

//连接手机过程中, 手机返回的拒绝码
#define BT_DELETE_BY_PHONE (-22)
//获取蓝牙账号ID
#define  BT_ACCOUNT_ID     10086
// http://bugfree.yealink.com/Bug.php?BugID=74365
// http://bugfree.yealink.com/Bug.php?BugID=77955
#define IF_SUPPORT_REPAIR                 1


// profile类型
enum PROFILE_TYPE
{
    PROFILE_NONE,
    PROFILE_HEADSET = 1,            /*headset*/
    PROFILE_MOBILE  = 1 << 1,       /*mobile -- talk*/
    PROFILE_A2DP    = 1 << 2,       /*a2dp -- music*/
    PROFILE_PBA     = 1 << 3,       /*pba -- contact*/
    /*PROFILE_ALL       = PROFILE_HEADSET | PROFILE_MOBILE | PROFILE_A2DP | PROFILE_PBA*/
};

// a2dp 状态
enum LOGIC_A2DP_STATE
{
    LOGIC_A2DP_STATE_BEGIN = -1,
    LOGIC_A2DP_STATE_DISCONNECTED,
    LOGIC_A2DP_STATE_CONNECTED,
    LOGIC_A2DP_STATE_PLAYING,
    LOGIC_A2DP_STATE_PAUSED,
    LOGIC_A2DP_STATE_END,
};

// a2dp profile info
struct A2dpProfileInfo
{
    LOGIC_A2DP_STATE        eState;     // a2dp当前状态
    bool                    bEnable;    // a2dp是否开启

    void Reset()
    {
        eState = LOGIC_A2DP_STATE_DISCONNECTED;
        bEnable = true;
    }

    A2dpProfileInfo()
    {
        Reset();
    }

    A2dpProfileInfo& operator= (const A2dpProfileInfo& a2dpInfo)
    {
        if (this != &a2dpInfo)
        {
            eState = a2dpInfo.eState;
            bEnable = a2dpInfo.bEnable;
        }

        return *this;
    }
};
// 设备信息结构体
struct DeviceInfo
{
    //设备句柄
    btkit_dev_handle_t devHandle;
    //设备名称
    yl::string strName;
    //设备物理地址
    yl::string strMac;
    //设备类型
    btkit_dev_type_t eType;
    //设备状态
    WIRELESS_DEVICE_STATE eState;
    //是否自动连接
    bool bAutoConnect;
    //是否自动同步联系人
    bool bSyncContact;
    //是否是扫描到的设备
    bool bScanedDev;
    //扫描到的顺序
    int iScanIndex;
    //设备是否配对过
    bool bPaired;
    // 连接成功次数
    UINT uConnectSuccessTimes;
    bool bMobileEnable;
    //设备是否可被发现--本地
    bool bDiscoverable;
    //a2dp profile info
    A2dpProfileInfo a2dpInfo;

    DeviceInfo()
    {
        devHandle = BT_DEVICE_HANDLE_INVAILED;
        strName = "";
        strMac = BT_DEVICE_MAC_INVAILED;
        eType = BT_UNKNOWN_TYPE;
        eState = WIRE_DEVICE_INIT_STATE;
        bAutoConnect = true;
        bSyncContact = false;
        bScanedDev = false;
        iScanIndex = -1;
        bPaired = false;
        uConnectSuccessTimes = 0;
        bMobileEnable = false;
        bDiscoverable = false;
        a2dpInfo.Reset();
    }

    DeviceInfo & operator= (const DeviceInfo & deviceInfo)
    {
        if (this != &deviceInfo)
        {
            devHandle = deviceInfo.devHandle;
            strName = deviceInfo.strName;
            strMac = deviceInfo.strMac;
            eType = deviceInfo.eType;
            eState = deviceInfo.eState;
            bAutoConnect = deviceInfo.bAutoConnect;
            bSyncContact = deviceInfo.bSyncContact;
            bScanedDev = deviceInfo.bScanedDev;
            iScanIndex = deviceInfo.iScanIndex;
            bPaired = deviceInfo.bPaired;
            uConnectSuccessTimes = deviceInfo.uConnectSuccessTimes;
            bMobileEnable = deviceInfo.bMobileEnable;
            bDiscoverable = false;
            a2dpInfo = deviceInfo.a2dpInfo;
        }

        return *this;
    }
};

//手机状态信息结构体
struct MobileProperty
{
    yl::string strMac;
    yl::string strNum;
    yl::string strState;
    bool bService;
    bool bRoam;
    int iSignal;
    int iBattery;
    MobileProperty()
    {
        bService = false;
        bRoam = true;
        iSignal = 0;
        iBattery = 0;
    }

    MobileProperty & operator = (const MobileProperty & mProperty)
    {
        if (this != &mProperty)
        {
            strMac = mProperty.strMac;
            strNum = mProperty.strNum;
            strState = mProperty.strState;
            bService = mProperty.bService;
            bRoam = mProperty.bRoam;
            iSignal = mProperty.iSignal;
            iBattery = mProperty.iBattery;
        }
        return *this;
    }
};

enum ContactSyncState
{
    BT_CONTACT_UNINIT,
    BT_CONTACT_DOWNLOADING,
    BT_CONTACT_PRASEING,
    BT_CONTACT_DOWNLOAD_SUCCESS,
    BT_CONTACT_DOWNLOAD_FAILED,
    BT_CONTACT_DOWNLOAD_CANCEL
};

typedef struct
{
    btkit_dev_handle_t dev_hnd;
    btkit_dev_t dev_property;
    btkit_dev_type_t dev_type;
} DeviceProperty;

//UI传递的匹配信息
struct AuthPinInfo
{
    unsigned long ulDeviceHandle;
    char szPin[BT_MAX_PIN_LEN];
    bool bConfirmPair;
    AuthPinInfo()
    {
        ulDeviceHandle = BT_DEVICE_HANDLE_INVAILED;
        memset(szPin, 0, BT_MAX_PIN_LEN);
        bool bConfirmPair = false;
    }
};

// 会话状态
enum DEVICE_STATE
{
    DS_NO_DONGLE,       // 未插入dongle
    DS_OK,              // 蓝牙开启，dongle可用
    DS_UNINIT,          // dongle已插入，未初始化蓝牙
    DS_SCAN             // 处于扫描状态
};

//任务类型
enum BT_TASK_TYPE
{
    BT_TASK_NONE = -1,
    BT_TASK_CONNECT,
    BT_TASK_PAIR,
    BT_TASK_PRASECONTACT,
    BT_TASK_DELETEDEVICE,
    BT_TASK_DISCONNECT,
    BT_TASK_POWER_OFF,
    BT_TASK_CANCLEPAIR,
    BT_TASK_POWER_ON,
    BT_TASK_DOWNLOAD_CONTACT_START,
    BT_TASK_DOWNLOAD_CONTACT_STOP,
    BT_TASK_UNPAIR,
    BT_TASK_HEADSET_EVENT,
    BT_TASK_SCAN_START,
    BT_TASK_SCAN_STOP,
    BT_TASK_AUTO_CONNECT,
    BT_TASK_CONNECT_PROFILES,
    BT_TASK_DISCONNECT_PROFILES
};

enum DEVICE_LIST_TYPE
{
    DEVICE_LIST_ALL,  // 所有设备
    DEVICE_LIST_SCAN, // 扫描设备
    DEVICE_LIST_PAIRED // 已匹配设备
};

//会话信息
struct BTSessionInfo
{
    unsigned long lDevHandle;             //device handle
    bool bSponor;                         //是否主动发起连接方
    bool bUserCancel;                     //是否用户取消连接
    bool bAutoConnect;                    //是否自动连接手机
    bool bRePair;                         //重新配对
    BTSessionInfo()
    {
        lDevHandle = BT_DEVICE_HANDLE_INVAILED;
        bSponor = false;
        bUserCancel = false;
        bAutoConnect = false;
        bRePair = false;
    }
};

//线程消息参数
struct BTTaskParam
{
    unsigned long ulDeviceHandle;
    WPARAM wParam;
    LPARAM lParam;
    long lSessionID;
    yl::string strParam;

    BTTaskParam(WPARAM wP = 0, LPARAM lP = 0)
    {
        wParam = wP;
        lParam = lP;
        ulDeviceHandle = BT_DEVICE_HANDLE_INVAILED;
        lSessionID = -1;
        strParam = "";
    }
};

//蓝牙手机通话信息
#define  BT_MB_LOCAL_HOLD    0x01
#define  BT_MB_REMOTE_HOLD   0x04
struct BTTalkCallUserInfo
{
    int iCallID;       //call id
    int iCallStatus;   //call status
    int iHoldStatus;   // hold status[0:establish][1:hold]
    yl::string strNum; // remote phone number

    BTTalkCallUserInfo()
    {
        iCallID = -1;
        iCallStatus = 0;
        iHoldStatus = 0;
    }

    BTTalkCallUserInfo & operator = (const BTTalkCallUserInfo & rParam)
    {
        if (this != &rParam)
        {
            iCallID = rParam.iCallID;
            iCallStatus = rParam.iCallStatus;
            iHoldStatus = rParam.iHoldStatus;
            strNum = rParam.strNum;
        }
        return *this;
    }
};

typedef YLList<DeviceInfo *> LIST_BT_DEVICE;
typedef YLList<DeviceInfo> UI_LIST_BT_DEVICE;

enum HEADSET_EVENT
{
    HEADSET_PLAY = 0,
    HEADSET_CALL_OUT,
    HEADSET_CALL_IN,
    HEADSET_ANSWER_CALL,
    HEADSET_END_CALL,
};

enum AUTOCONNECT_STRATEGY
{
    AC_STRATEGY_SHORT_TIME = 1, // 短时间自动连接
    AC_STRATEGY_LONG_TIME       // 长时间自动连接
};

struct AutoConenctInfo
{
    bool bAutoConnect;                  // 自动连接开关
    AUTOCONNECT_STRATEGY eACStrategy;   // 自动连接策略

    AutoConenctInfo()
    {
        bAutoConnect = false;
        eACStrategy = AC_STRATEGY_SHORT_TIME;
    }
};

// 蓝牙支持的功能特性
enum BT_FEATURE_SUPPROT
{
    BT_FEATURE_AUDIO = 0,
    BT_FEATURE_CONTACT = 1
};

#endif

