///////////////////////////////////////////////////////////
//  CBluetoothDriver.cpp
//  Implementation of the Class CBluetoothDriver
//  Created on:      25-三月-2014 17:22:21
//  Original author: songk
///////////////////////////////////////////////////////////
#ifdef IF_SUPPORT_BLUETOOTH
#include "bluetooth_inc.h"
IMPLEMENT_GETINSTANCE(CBTDriver)

#ifdef IF_BT_TEST_LOGIC
#include "../testdriver/btestdriver.h"
#endif

#define NUM_MAX_SIZE    (64)

#ifdef IF_BT_SIMULATION_DRIVER
int   EmptyBT_INIT(const btkit_cfg_t * cfg, btkit_callback_t * cb)
{
    return 0;
}
int   EmptyBT_EXIT()
{
    return -1;
}
int   EmptyBT_Ring(btkit_handle_t hnd, btkit_dev_handle_t dev_hnd)
{
    return -1;
}
int   EmptyBT_SendDtmf(btkit_handle_t hnd, btkit_dev_handle_t dev_hnd, int callid,
                       const char dtmf)
{
    return -1;
}
int   EmptyBT_AnswerAuth(btkit_handle_t hnd, btkit_dev_handle_t dev_hnd, int right)
{
    return -1;
}
int   EmptyBT_SetLogLevel(int iLevel) {;}

#define INTERFACE_BT_INIT        EmptyBT_INIT
#define INTERFACE_BT_EXIT        EmptyBT_EXIT
#define INTERFACE_BT_CREATE      _BTTestDriver.InitDriver
#define INTERFACE_BT_DESTORY     _BTTestDriver.DestoryDriver
#define INTERFACE_BT_POWERSET    _BTTestDriver.PowerSet
#define INTERFACE_BT_NAMESET     _BTTestDriver.SetDongleName
#define INTERFACE_BT_PROPERTYGET _BTTestDriver.GetDongleProperty
#define INTERFACE_BT_SUMGET      _BTTestDriver.GetDeviceSum
#define INTERFACE_BT_DEVICESGET  _BTTestDriver.GetDeviceHandle
#define INTERFACE_BT_SCANSTART   _BTTestDriver.StartScan
#define INTERFACE_BT_SCANSTOP    _BTTestDriver.StopScan
#define INTERFACE_BT_PAIR        _BTTestDriver.PairDevice
#define INTERFACE_BT_CANCEL_PAIR _BTTestDriver.CancelPairDevice
#define INTERFACE_BT_UNPAIR      _BTTestDriver.CancelPairDevice
#define INTERFACE_BT_REMOVE      _BTTestDriver.RemoveDevice
#define INTERFACE_BT_TYPEGET     _BTTestDriver.GetDeviceType
#define INTERFACE_BT_MBPROPERTYGET _BTTestDriver.GetMbProperty
#define INTERFACE_BT_PINANSWER     _BTTestDriver.AnswerPin
#define INTERFACE_BT_AUTHANSWER    EmptyBT_AnswerAuth
#define INTERFACE_BT_ISCONNECTED   _BTTestDriver.IsDeviceConnected
#define INTERFACE_BT_ISPAIRED      _BTTestDriver.IsDevicePaired
#define INTERFACE_BT_DEVPROPERTYGET _BTTestDriver.GetDeviceProperty
#define INTERFACE_BT_DEVMACGET    _BTTestDriver.GetDeviceMac
#define INTERFACE_BT_CONNECT      _BTTestDriver.ConnectDevice
#define INTERFACE_BT_DISCONNECT   _BTTestDriver.DisconnectDevice
#define INTERFACE_BT_RINGSTART    EmptyBT_Ring
#define INTERFACE_BT_RINGSTOP     EmptyBT_Ring
#define INTERFACE_BT_PLAYSTART    EmptyBT_Ring
#define INTERFACE_BT_PLAYSTOP     EmptyBT_Ring
#define INTERFACE_BT_CALLINVITE   _BTTestDriver.DeviceCallInvite
#define INTERFACE_BT_CALLIN       _BTTestDriver.DeviceCallIn
#define INTERFACE_BT_CALLANSWER   _BTTestDriver.DeviceCallAnswer
#define INTERFACE_BT_CALLHANGUP   _BTTestDriver.DeviceCallEnd
#define INTERFACE_BT_CALLHOLD     _BTTestDriver.DeviceCallHold
#define INTERFACE_BT_CALLTOTALGET _BTTestDriver.GetCallSum
#define INTERFACE_BT_CALLSTATUSGET _BTTestDriver.GetCallStatus
#define INTERFACE_BT_DTMFSEND      EmptyBT_SendDtmf
#define INTERFACE_BT_CONTACTDOWNSTART _BTTestDriver.StartDownloadContact
#define INTERFACE_BT_CONTACTDOWNSTOP  _BTTestDriver.CancelDownloadContact
#define INTERFACE_BT_CHANNELSWITCH    _BTTestDriver.SwitchChannel
#define INTERFACE_BT_CONNECT_PROFILE        _BTTestDriver.ConnectProfile
#define INTERFACE_BT_DISCONNECT_PROFILE     _BTTestDriver.DisconnectProfile
#define INTERFACE_BT_IS_PROFILE_CONNECTED   _BTTestDriver.IsProfileConnected
#define INTERFACE_BT_SETLOGLEVEL    EmptyBT_SetLogLevel
#define INTERFACE_BT_PINCONFIRM     _BTTestDriver.PinConfirm
#else
#define INTERFACE_BT_INIT        bt_init
#define INTERFACE_BT_EXIT        bt_exit
#define INTERFACE_BT_CREATE      btkit_create
#define INTERFACE_BT_DESTORY     btkit_destroy
#define INTERFACE_BT_POWERSET    btkit_power_set
#define INTERFACE_BT_NAMESET     btkit_name_set
#define INTERFACE_BT_PROPERTYGET btkit_property_get
#define INTERFACE_BT_SUMGET      btkit_devices_sum_get
#define INTERFACE_BT_DEVICESGET  btkit_devices_get
#define INTERFACE_BT_SCANSTART   btkit_scan_start
#define INTERFACE_BT_SCANSTOP    btkit_scan_stop
#define INTERFACE_BT_PAIR        btkit_dev_pair
#define INTERFACE_BT_CANCEL_PAIR btkit_dev_cancel_pair
#define INTERFACE_BT_UNPAIR      btkit_dev_unpair
#define INTERFACE_BT_REMOVE      btkit_dev_delete
#define INTERFACE_BT_TYPEGET     btkit_dev_type_get
#define INTERFACE_BT_MBPROPERTYGET btkit_mb_get_property
#define INTERFACE_BT_PINANSWER     btkit_dev_pin_answer
#define INTERFACE_BT_AUTHANSWER    btkit_dev_pin_authorize
#define INTERFACE_BT_ISCONNECTED   btkit_dev_connected
#define INTERFACE_BT_ISPAIRED      btkit_dev_paired
#define INTERFACE_BT_DEVPROPERTYGET btkit_dev_property_get
#define INTERFACE_BT_DEVMACGET    btkit_dev_addr_get
#define INTERFACE_BT_CONNECT      btkit_dev_connect
#define INTERFACE_BT_AUTO_CONNECT btkit_dev_reconnect
#define INTERFACE_BT_DISCONNECT   btkit_dev_release
#define INTERFACE_BT_RINGSTART    btkit_dev_ring_start
#define INTERFACE_BT_RINGSTOP     btkit_dev_ring_stop
#define INTERFACE_BT_PLAYSTART    btkit_dev_play_start
#define INTERFACE_BT_PLAYSTOP     btkit_dev_play_stop
#define INTERFACE_BT_CALLINVITE   btkit_dev_call_invite
#define INTERFACE_BT_CALLIN       btkit_dev_call_in
#define INTERFACE_BT_CALLANSWER   btkit_dev_call_answer
#define INTERFACE_BT_CALLHANGUP   btkit_dev_call_hangup
#define INTERFACE_BT_CALLHOLD     btkit_dev_call_hold
#define INTERFACE_BT_CALLTOTALGET btkit_dev_call_total
#define INTERFACE_BT_CALLSTATUSGET btkit_dev_call_status
#define INTERFACE_BT_DTMFSEND      btkit_dev_call_send_dtmf
#define INTERFACE_BT_CONTACTDOWNSTART btkit_pba_download_start
#define INTERFACE_BT_CONTACTDOWNSTOP  btkit_pba_download_stop
#define INTERFACE_BT_CHANNELSWITCH    btkit_snd_mode_set
#define INTERFACE_BT_CONNECT_PROFILE        btkit_dev_profile_connect
#define INTERFACE_BT_DISCONNECT_PROFILE     btkit_dev_profile_disconnect
#define INTERFACE_BT_IS_PROFILE_CONNECTED   btkit_dev_profile_connected
#define INTERFACE_BT_SETLOGLEVEL      btkit_set_log_level
#define INTERFACE_BT_PINCONFIRM       btkit_dev_pin_confirm
#endif //IF_BT_SIMULATION_DRIVER

//构造函数
CBTDriver::CBTDriver()
{

#if IF_BT_TEST_LOGIC
    _BTTestDriver.Init();
#endif

    m_instDongle = 0;
}

//析构函数
CBTDriver::~CBTDriver()
{
    INTERFACE_BT_EXIT();
}

//初始化驱动
bool CBTDriver::InitDriver()
{
    static bool bInitSucc = false;

    if (!bInitSucc)
    {
        // log信息
        btkit_cfg_t init_param;
        init_param.log_type     = BT_LOG_MODE_CONS | BT_LOG_MODE_TIME | BT_LOG_MODE_SYS;
        init_param.log_level    = commonUnits_GetLogLevel();
        init_param.flags    = BTKIT_FLG_NONE;

        // 回调信息
        btkit_callback_t callback_param;
        callback_param.event_cb = CBTDriver::BlueToothEventCallBack;

        // 初始化失败，则不做任何事情
        if (INTERFACE_BT_INIT(&init_param, &callback_param) != 0)
        {
            WIRE_ERR("init bt driver error");
            return false;
        }

        bInitSucc = true;
        WIRE_INFO("init bt driver successful");
    }

    if (bInitSucc)
    {
        if (m_instDongle != 0)
        {
            DestoryDongle();
        }

        btkit_create_t create_param;
        create_param.id = 0;

        // 初始化创建 Dongle
        if (INTERFACE_BT_CREATE(&m_instDongle, &create_param) != 0
                || m_instDongle == 0)
        {
            WIRE_ERR("create bt dongle error");
            return false;
        }
        WIRE_INFO("create bt dongle sucessful");
    }

    return true;
}

//设置日志等级
void CBTDriver::SetDriverLogLevel(int iLevel)
{
    INTERFACE_BT_SETLOGLEVEL(iLevel);
}

// 启动适配器
bool CBTDriver::PowerSet(bool bEnable)
{
    if (0 == m_instDongle)
    {
        return false;
    }

    return INTERFACE_BT_POWERSET(m_instDongle, bEnable) == 0;
}

//销毁驱动
bool CBTDriver::DestoryDongle()
{
    INTERFACE_BT_DESTORY(m_instDongle);
    m_instDongle = 0;

    WIRE_INFO("destory bt dongle.");

    return true;
}

//获取本机蓝牙信息
bool CBTDriver::GetBluetoothModuleInfo(DeviceInfo & deviceInfo)
{
    btkit_property_t property;
    memset((void *)&property, 0, sizeof(btkit_property_t));
    if (INTERFACE_BT_PROPERTYGET(m_instDongle, &property) != 0)
    {
        WIRE_ERR("Get device info fail");
        return false;
    }

    deviceInfo.strName = configParser_GetConfigString(kszBluetoothAdapterName);
    deviceInfo.strName = commUnit_ReplaceWildcard(deviceInfo.strName);
    deviceInfo.strMac = property.bt_addr;

    WIRE_INFO("Get device info, mac=%s, name=%s", deviceInfo.strMac.c_str(),
              deviceInfo.strName.c_str());
    return true;
}

//获取设备类型
btkit_dev_type_t CBTDriver::GetDeviceType(btkit_dev_handle_t devHandle)
{
    if (0 == m_instDongle)
    {
        return  BT_UNKNOWN_TYPE;
    }

    return INTERFACE_BT_TYPEGET(m_instDongle, devHandle);
}

//获取设备地址
void CBTDriver::GetDeviceMac(btkit_dev_handle_t devHandle, yl::string & strMac)
{
    if (0 == m_instDongle)
    {
        return;
    }

    char szMac[BT_ADDR_STR_LEN] = {0};
    if (INTERFACE_BT_DEVMACGET(m_instDongle, devHandle, szMac) == 0)
    {
        strMac = szMac;
        return ;
    }

    strMac = "";
}

// 获取蓝牙设备的属性
bool CBTDriver::GetDeviceProperty(btkit_dev_handle_t devHandle, DeviceProperty & devProperty)
{
    devProperty.dev_type = INTERFACE_BT_TYPEGET(m_instDongle, devHandle);
    devProperty.dev_hnd = devHandle;
    return INTERFACE_BT_DEVPROPERTYGET(m_instDongle, devHandle, &devProperty.dev_property) == 0;
}

//获取蓝牙手机的属性
bool CBTDriver::GetMbProperty(btkit_dev_handle_t devHandle, MobileProperty & mbProperty)
{
    btkit_mb_property_t mb_property;
    if (INTERFACE_BT_MBPROPERTYGET(m_instDongle, devHandle, &mb_property) != 0)
    {
        return false;
    }

    mbProperty.strNum = mb_property.local_phonenum;
    mbProperty.strState = mb_property.state;

    return true;
}

// 蓝牙设备是否连接
bool CBTDriver::IsDeviceConnectedInDriver(btkit_dev_handle_t devHandle)
{
    return INTERFACE_BT_ISCONNECTED(m_instDongle, devHandle);
}

//判断设备是否已匹配
bool CBTDriver::IsDevicePairedInDriver(btkit_dev_handle_t devHandle)
{
    return INTERFACE_BT_ISPAIRED(m_instDongle, devHandle);
}

//枚举设备
bool CBTDriver::IterateDevice(YLList<DeviceProperty> & listProperty)
{
    //Step 1. 获取设备总数
    int iDevCount = INTERFACE_BT_SUMGET(m_instDongle);
    if (0 == iDevCount)
    {
        return false;
    }

    //Step 2. 获取设备Handle
    btkit_dev_handle_t * dev = new btkit_dev_handle_t[iDevCount];
    if (0 != INTERFACE_BT_DEVICESGET(m_instDongle, dev))
    {
        delete [] dev;
        return false;
    }

    //Step 3. 根据Handle获取设备信息
    DeviceProperty devProperty;
    for (int i = 0; i < iDevCount; ++i)
    {
        if (GetDeviceProperty(dev[i], devProperty))
        {
            listProperty.push_back(devProperty);
        }
    }

    delete [] dev;
    return listProperty.size() > 0;
}

//连接设备
int CBTDriver::ConnectPairedDevice(btkit_dev_handle_t devHandle, bool bAutoConnect)
{
    //会卡住，需要单独开一个线程来执行
    if (bAutoConnect)
    {
        return INTERFACE_BT_AUTO_CONNECT(m_instDongle, devHandle);
    }
    else
    {
        return INTERFACE_BT_CONNECT(m_instDongle, devHandle);
    }
}

//断开设备
bool CBTDriver::DisconnectDevice(btkit_dev_handle_t devHandle)
{
    return (0 == INTERFACE_BT_DISCONNECT(m_instDongle, devHandle));
}

//匹配设备
int CBTDriver::PairDevice(btkit_dev_handle_t devHandle)
{
    //会卡住，需要单独开一个线程来执行
    return (/*0 ==*/ INTERFACE_BT_PAIR(m_instDongle, devHandle));
}

bool CBTDriver::UnPairDevice(btkit_dev_handle_t devHandle)
{
    return (0 == INTERFACE_BT_UNPAIR(m_instDongle, devHandle));
}

//取消匹配
bool CBTDriver::CancelPairDevice(btkit_dev_handle_t devHandle)
{
    return (0 == INTERFACE_BT_CANCEL_PAIR(m_instDongle, devHandle));
}

bool CBTDriver::DeleteDevice(btkit_dev_handle_t devHandle)
{
    return (0 == INTERFACE_BT_REMOVE(m_instDongle, devHandle));
}

//扫描设备
bool CBTDriver::ScanDevice()
{
    return (0 == INTERFACE_BT_SCANSTART(m_instDongle));
}

//停止扫描
bool CBTDriver::StopScanDevice()
{
    return (0 == INTERFACE_BT_SCANSTOP(m_instDongle));
}

//设置显示名称
bool CBTDriver::SetModuleName(const char * pDeviceName)
{
    if (NULL == pDeviceName
            || strlen(pDeviceName) > BT_MAX_NAME_LEN)
    {
        return false;
    }

    return (0 == INTERFACE_BT_NAMESET(m_instDongle, pDeviceName));
}

//回复PIN码
bool CBTDriver::AnswerPin(btkit_dev_handle_t devHandle, const char * pDeviceAddr)
{
    if (NULL == pDeviceAddr
            || 0 == m_instDongle)
    {
        return false;
    }

    return (0 == INTERFACE_BT_PINANSWER(m_instDongle, devHandle, pDeviceAddr));
}

//确认配对
bool CBTDriver::PinConfirm(btkit_dev_handle_t devHandle, bool bConfirm)
{
    WIRE_INFO("[%s] [%d] pin confirm.", bConfirm ? "Accept" : "Refuse", devHandle);
    return (0 == INTERFACE_BT_PINCONFIRM(m_instDongle, devHandle, bConfirm));
}

//Answer Auth
bool CBTDriver::AnswerAuth(btkit_dev_handle_t devHandle, bool iRight)
{
    return (0 == INTERFACE_BT_AUTHANSWER(m_instDongle, devHandle, iRight));
}

//电话呼入
bool CBTDriver::CallIn(btkit_dev_handle_t devHandle, const char * pNum)
{
    if (NULL == pNum  || strlen(pNum) > NUM_MAX_SIZE || 0 == m_instDongle)
    {
        WIRE_WARN("[Call in][Num size illegal][num:%s]", pNum);
        return false;
    }

    return (0 == INTERFACE_BT_CALLIN(m_instDongle, devHandle, pNum));
}

//电话呼出
bool CBTDriver::CallOut(btkit_dev_handle_t dev_hnd, const char * pNum)
{
    if (NULL == pNum || strlen(pNum) > NUM_MAX_SIZE || 0 == m_instDongle)
    {
        WIRE_WARN("[Call out][Num size illegal][num:%s]", pNum);
        return false;
    }

    int iCallID = 0;
    return (0 == INTERFACE_BT_CALLINVITE(m_instDongle, dev_hnd, pNum, &iCallID));
}

//应答电话
bool CBTDriver::CallAnswer(btkit_dev_handle_t dev_hnd)
{
    return (0 == INTERFACE_BT_CALLANSWER(m_instDongle, dev_hnd, 0));
}

//通话结束
bool CBTDriver::CallEnd(btkit_dev_handle_t dev_hnd)
{
    return (0 == INTERFACE_BT_CALLHANGUP(m_instDongle, dev_hnd, 0));
}

//响铃
bool CBTDriver::Ring(btkit_dev_handle_t dev_hnd, bool bRing)
{
    if (bRing)
    {
        return INTERFACE_BT_RINGSTART(m_instDongle, dev_hnd) == 0;
    }
    else
    {
        return INTERFACE_BT_RINGSTOP(m_instDongle, dev_hnd) == 0;
    }
}

//播放声音
bool CBTDriver::Play(btkit_dev_handle_t devHandle, bool bPlay)
{
    int iRst = -1;
    if (bPlay)
    {
        iRst = INTERFACE_BT_PLAYSTART(m_instDongle, devHandle);
    }
    else
    {
        iRst = INTERFACE_BT_PLAYSTOP(m_instDongle, devHandle);
    }

    WIRE_INFO("@@@@ after play[%s], result[%d]", bPlay == 1 ? "start" : "stop", iRst);
    return iRst == 0;
}

// 呼出话路
int CBTDriver::MobileCallInvite(btkit_dev_handle_t dev_hnd, const char * pNum)
{
    int iCallID = -1;

    if (INTERFACE_BT_CALLINVITE(m_instDongle, dev_hnd, pNum, &iCallID) == 0)
    {
        return iCallID;
    }

    return -1;
}

//通话话路
bool CBTDriver::MobileCallAnswer(btkit_dev_handle_t dev_hnd, int iCallID)
{
    return INTERFACE_BT_CALLANSWER(m_instDongle, dev_hnd, iCallID) == 0;
}

//拒绝话路
bool CBTDriver::MobileCallRefuse(btkit_dev_handle_t dev_hnd, int iCallID)
{
    return INTERFACE_BT_CALLHANGUP(m_instDongle, dev_hnd, iCallID) == 0;
}

//挂断话路
bool CBTDriver::MobileCallHangup(btkit_dev_handle_t dev_hnd, int iCallID)
{
    return INTERFACE_BT_CALLHANGUP(m_instDongle, dev_hnd, iCallID) == 0;
}

//Hold 话路
bool CBTDriver::MobileCallHold(btkit_dev_handle_t dev_hnd, int iCallID)
{
    return INTERFACE_BT_CALLHOLD(m_instDongle, dev_hnd, iCallID, true) == 0;
}

//Resume 话路
bool CBTDriver::MobileCallResume(btkit_dev_handle_t dev_hnd, int iCallID)
{
    return INTERFACE_BT_CALLHOLD(m_instDongle, dev_hnd, iCallID, false) == 0;
}

// 发送dtmf
bool CBTDriver::MobileSendDtmf(btkit_dev_handle_t dev_hnd, int iCallID, char cDtmf)
{
    return INTERFACE_BT_DTMFSEND(m_instDongle, dev_hnd, iCallID, cDtmf) == 0;
}

//获取通话状态
int CBTDriver::MobileGetCallStatus(btkit_dev_handle_t dev_hnd, int iCallID)
{
    int iStatus = -1;
    if (INTERFACE_BT_CALLSTATUSGET(m_instDongle, dev_hnd, iCallID, &iStatus) != 0)
    {
        return -1;
    }

    return iStatus;
}

//获取话路数量
int CBTDriver::MobileGetCallCount(btkit_dev_handle_t dev_hnd)
{
    int iStatus = 0;
    if (INTERFACE_BT_CALLTOTALGET(m_instDongle, dev_hnd, &iStatus) != 0)
    {
        return 0;
    }

    return iStatus;
}

//开始下载手机联系人
bool CBTDriver::MobileStartDownloadContact(btkit_dev_handle_t dev_hnd,
        int iType/* = BT_PB_TYPE_PHONE_PB*/, bool bPhoto/* = false*/)
{
    btkit_pba_data_t pba_data;
    pba_data.photo = bPhoto;
    pba_data.type = iType;
    pba_data.dev_hnd = dev_hnd;
    return INTERFACE_BT_CONTACTDOWNSTART(m_instDongle, dev_hnd, &pba_data) == 0;
}

//停止下载
bool CBTDriver::MobileStopDownload(btkit_dev_handle_t dev_hnd)
{
    return INTERFACE_BT_CONTACTDOWNSTOP(m_instDongle, dev_hnd) == 0;
}

//切换声音源
bool CBTDriver::MobileSwitchChannel(btkit_dev_handle_t dev_hnd, bool bLink, int iCallID)
{
    return INTERFACE_BT_CHANNELSWITCH(m_instDongle, dev_hnd, iCallID,
                                      bLink ? BT_SND_MODE_MB_TO_LOC : BT_SND_MODE_LOC_TO_MB) == 0;
}

// 连接profile
int CBTDriver::ConnectProfiles(btkit_dev_handle_t devHandle, int iProfiles)
{
    int iResult = 0;
    if (iProfiles & PROFILE_HEADSET)
    {
        if (INTERFACE_BT_CONNECT_PROFILE(m_instDongle, devHandle, GetUUIDByProfileType(PROFILE_HEADSET)) == 0)
        {
            iResult = 1;
        }
    }

    if (iProfiles & PROFILE_MOBILE)
    {
        if (INTERFACE_BT_CONNECT_PROFILE(m_instDongle, devHandle, GetUUIDByProfileType(PROFILE_MOBILE)) == 0)
        {
            iResult |= 1 << 1;
        }
    }

    if (iProfiles & PROFILE_A2DP)
    {
        if (INTERFACE_BT_CONNECT_PROFILE(m_instDongle, devHandle, GetUUIDByProfileType(PROFILE_A2DP)) == 0)
        {
            iResult |= 1 << 2;
        }
    }

    return iResult;
}

// 断开profile
int CBTDriver::DisconnectProfiles(btkit_dev_handle_t devHandle, int iProfiles)
{
    // 用profile的位标识接口返回状态
    int iResult = 0;
    if (iProfiles & PROFILE_HEADSET)
    {
        if (INTERFACE_BT_DISCONNECT_PROFILE(m_instDongle, devHandle, GetUUIDByProfileType(PROFILE_HEADSET)) == 0)
        {
            iResult = PROFILE_HEADSET;
        }
    }

    if (iProfiles & PROFILE_MOBILE)
    {
        if (INTERFACE_BT_DISCONNECT_PROFILE(m_instDongle, devHandle, GetUUIDByProfileType(PROFILE_MOBILE)) == 0)
        {
            iResult |= PROFILE_MOBILE;
        }
    }

    if (iProfiles & PROFILE_A2DP)
    {
        if (INTERFACE_BT_DISCONNECT_PROFILE(m_instDongle, devHandle, GetUUIDByProfileType(PROFILE_A2DP)) == 0)
        {
            iResult |= PROFILE_A2DP;
        }
    }

    return iResult;
}

// 设置profile开关
bool CBTDriver::SetProfileEnable(btkit_dev_handle_t devHandle, int iProfile, bool bEnable)
{
    return btkit_dev_profile_activity_set(m_instDongle, devHandle,
                                          GetUUIDByProfileType(iProfile),
                                          bEnable ? BT_PROFILE_ON : BT_PROFILE_OFF) == 0;
}

// profile开关是否开启
bool CBTDriver::IsProfileEnable(unsigned long ulHandle, int iProfileType)
{
    int iEnable = BT_PROFILE_OFF;
    if (btkit_dev_profile_activity_get(m_instDongle, ulHandle, GetUUIDByProfileType(iProfileType), &iEnable) != 0)
    {
        return false;
    }

    return iEnable == BT_PROFILE_ON;
}

// profile是否连接
bool CBTDriver::IsProfileConnected(btkit_dev_handle_t devHandle, int iProfile)
{
    return INTERFACE_BT_IS_PROFILE_CONNECTED(m_instDongle, devHandle, GetUUIDByProfileType(iProfile));
}

// 查询设备是否支持某种profile
bool CBTDriver::IsSupportProfile(btkit_dev_handle_t devHandle, int iProfile)
{
    int iSupport = BT_PROFILE_NOTSUPPORTED;
    if (btkit_dev_profile_supported_get(m_instDongle, devHandle, GetUUIDByProfileType(iProfile), &iSupport) != 0)
    {
        return false;
    }

    WIRE_INFO("Device [%d] profile [%d] supported [%d]", devHandle, iProfile, iSupport == BT_PROFILE_SUPPORTED);
    return iSupport == BT_PROFILE_SUPPORTED;
}

bool CBTDriver::SetDiscoverable(bool bEnable)
{
    bool bRet = btkit_discoverable_set(m_instDongle, bEnable, 0);

    WIRE_INFO("Set local device discoverable[%d] ret[%d]", bEnable, bRet);
    return bRet;
}

int CBTDriver::GetUUIDByProfileType(int iProfileType)
{
    int iUUID = -1;

    switch (iProfileType)
    {
    case PROFILE_MOBILE:
        iUUID = BT_HF_AGW_UUID;
        break;
    case PROFILE_HEADSET:
        iUUID = BT_HF_UUID;
        break;
    case PROFILE_A2DP:
        iUUID = BT_AD_SRC_UUID;
        break;
    default:
        break;
    }

    return iUUID;
}

//蓝牙回调消息函数
void CBTDriver::BlueToothEventCallBack(const btkit_msg_t * msg, void * user_data)
{
    if (msg == NULL)
    {
        return;
    }

#if IS_CODE_TEST
    if (NULL != msg)
    {
        WIRE_INFO("---handle[%d]:event[%x]:dev addr[%d]", msg->bt_instance, msg->bt_event,
                  msg->bt_callback_param.dev_hnd);
    }
#endif // IS_CODE_TEST

    // 回调函数处于bt库中新开的线程，将消息交给主线程处理
    etl_NotifyAppEx(FALSE, BLUETOOTH_MSG_CALLBACK, 0, 0, sizeof(btkit_msg_t), msg);
}
#endif // IF_SUPPORT_BLUETOOTH
