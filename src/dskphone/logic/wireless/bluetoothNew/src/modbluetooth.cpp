#ifdef IF_SUPPORT_BLUETOOTH
#include "bluetooth_inc.h"
#include "dsskey/include/moddsskey.h"

// 模块初始化
void Bluetooth_Init()
{
    _BTManager.Init();
}

// 返回当前设备状态
DEVICE_STATE Bluetooth_GetDeviceState()
{
    return _BTManager.GetDeviceState();
}

// 蓝牙耳麦播放铃声
bool Bluetooth_HSPlayRing()
{
    return _BTManager.HeadSetDevicePlayRing(true);
}

// 蓝牙耳麦停止铃声
void Bluetooth_HSStopRing()
{
    _BTManager.HeadSetDevicePlayRing(false);
}

// 蓝牙耳麦停止播放声音
void Bluetooth_HSStopPlay()
{
    _BTManager.HeadSetDeviceEvent(HEADSET_PLAY, false);
}

// 蓝牙耳麦播放声音
void Bluetooth_HSStartPlay()
{
    _BTManager.HeadSetDeviceEvent(HEADSET_PLAY, true);
}

// 蓝牙耳麦是否连接
bool Bluetooth_IsHeadsetConnected()
{
    DeviceInfo* pDevice = _BTManager.GetCurrentDevice(SESSION_CONNECTED_STATE);
    if (NULL == pDevice)
    {
        return false;
    }

    return BT_HEADSET == pDevice->eType;
}

// 蓝牙是否可用
bool Bluetooth_IsEnable()
{
    return _BTManager.IsEnable();
}

// 蓝牙是否连接
bool Bluetooth_IsConnected()
{
    return _BTManager.IsConnectedSessionExist();
}

// 设置蓝牙是否可用
bool Bluetooth_SetEnable(bool bEnable/* = true*/)
{
    return _BTManager.SetBTOpenStatus(bEnable);
}

// 获取指定序号的蓝牙信息
bool Bluetooth_GetBluetoothInfoByMac(const char * pMac, DeviceInfo & deviceInfo)
{
    if (NULL == pMac)
    {
        return false;
    }

    unsigned long ulDeviceHandle = _BTData.GetDeviceHandleByMac(pMac);

    DeviceInfo * pDeviceInfo = _BTData.GetDeviceInfoByHandle(ulDeviceHandle);
    if (NULL == pDeviceInfo)
    {
        return false;
    }

    deviceInfo = *pDeviceInfo;
    return true;
}

bool Bluetooth_GetBluetoothInfoByHandle(unsigned long ulHandle, DeviceInfo & deviceInfo)
{
    if (ulHandle == BT_DEVICE_HANDLE_INVAILED)
    {
        return false;
    }

    DeviceInfo * pDeviceInfo = _BTData.GetDeviceInfoByHandle(ulHandle);
    if (NULL == pDeviceInfo)
    {
        return false;
    }

    deviceInfo = *pDeviceInfo;

    return true;
}

// 获取本地设备信息
bool Bluetooth_GetDeviceInfo(DeviceInfo & deviceInfo)
{
    return _BTDriver.GetBluetoothModuleInfo(deviceInfo);
}

bool Bluetooth_GetPairedInfo( DeviceInfo & deviceInfo )
{
	return _BTData.GetPairedDevice(deviceInfo);
}

// 返回蓝牙列表
void Bluetooth_GetDeviceList(UI_LIST_BT_DEVICE & listDevice,
                             DEVICE_LIST_TYPE eType/* DEVICE_LIST_TYPE*/)
{
    _BTData.GetDeviceList(listDevice, eType);
}

// 返回蓝牙扫描列表
bool Bluetooth_GetScanedDeviceList(UI_LIST_BT_DEVICE & listDevice, int iStartPos/* = 0*/,
                                   int iNumber/* = 911*/)
{
    if (iStartPos < 0 || iNumber <= 0
            || iStartPos > Bluetooth_GetScanedDeviceListSize())
    {
        return false;
    }

    WIRE_INFO("UI get scaned list, start pos:%d, number:%d", iStartPos, iNumber);

    return _BTData.GetScanedDeviceList(listDevice, iStartPos, iNumber);
}

// 清空扫描列表
void Bluetooth_ClearScanedList()
{
    _BTData.ClearScanDeviceList();
}

// 获取扫描列表大小
int Bluetooth_GetScanedDeviceListSize()
{
    return _BTData.GetScanedDeviceListSize();
}

// 扫描设备 (独占，Scan之后不能再做其他事情)
bool Bluetooth_StartScanDevice()
{
    return _BTManager.ProcessScanDevice(true);
}

// 停止扫描设备
bool Bluetooth_StopScanDevice()
{
    return _BTManager.ProcessScanDevice(false);
}

//取消匹配
bool Bluetooth_CancelPair(const char * pMac)
{
    if (NULL == pMac)
    {
        return false;
    }
    return Bluetooth_AnswerPin(pMac, "");
}

// 配对时回复pin
bool Bluetooth_AnswerPin(const char * pMac, const char * pPin)
{
    if (NULL == pMac
            || NULL == pPin)
    {
        return false;
    }

    DeviceInfo * pDevice = _BTData.GetDeviceInfoByHandle(_BTData.GetDeviceHandleByMac(pMac));

    if (NULL == pDevice)
    {
        return false;
    }

    AuthPinInfo authInfo;
    WirelessSessionData wsData;

    //取消匹配
    if (strcmp(pPin, "") == 0)
    {
        authInfo.bConfirmPair = false;
        wsData.lParam = 1;
    }
    else
    {
        authInfo.bConfirmPair = true;
    }

    authInfo.ulDeviceHandle = pDevice->devHandle;
    strncpy(authInfo.szPin, pPin, BT_MAX_PIN_LEN);
    authInfo.szPin[BT_MAX_PIN_LEN - 1] = '\0' ;

    WIRE_INFO("handle[%d]:mac[%s]:pin[%s]", authInfo.ulDeviceHandle, pMac, authInfo.szPin);

    wsData.ulMessage = WIRELESS_OPTION_BT_ANSWERPIN;
    wsData.wParam = authInfo.ulDeviceHandle;
    wsData.pData = (void *)&authInfo;

    return _WirelessSessionManager.SendOptionToSessionByHandle(authInfo.ulDeviceHandle, wsData,
            BT_SESSION);
}

//配对手机时确认
bool Bluetooth_AnswerMobilePin(const char * pMac)
{
    return Bluetooth_AnswerPin(pMac, "0000");
}

// 连接指定序号的蓝牙
bool Bluetooth_ConnectDevice(const char * pMac)
{
    return _BTManager.ProcessConnectDevice(_BTData.GetDeviceHandleByMac(pMac));
}

// 断开连接
bool Bluetooth_DisconnectDevice(const char * pMac)
{
    if (NULL == pMac)
    {
        return false;
    }

    unsigned long ulHandle = _BTData.GetDeviceHandleByMac(pMac);

    if (ulHandle != BT_DEVICE_HANDLE_INVAILED)
    {
        _BTManager.ProcessUnConnectDevice(ulHandle, true);
    }

    return true;
}

// 通过mac删除设备信息
bool Bluetooth_DeleteDevice(const char * pMac/* = NULL*/)
{
    if (NULL == pMac)
    {
        return _BTManager.DeleteDevice(BT_DEVICE_HANDLE_INVAILED, true);
    }

    unsigned long ulHandle = _BTData.GetDeviceHandleByMac(pMac);
    if (ulHandle == BT_DEVICE_HANDLE_INVAILED)
    {
        return false;
    }

    return _BTManager.DeleteDevice(ulHandle, false);
}

// 设置设备名
bool Bluetooth_SetDeviceName(const char * pName)
{
    if (NULL == pName)
    {
        return false;
    }

    return _BTManager.SetDeviceName(pName, true);
}

// 上下调整设备位置
bool Bluetooth_AdjustDevicePos(const char * pMac, bool bUp)
{
    return false;
}

//移动设备位置
bool Bluetooth_MoveDeviceToPos(const char * pMac, UINT iPos)
{
    return false;
}

// 设置是否自动连接
bool Bluetooth_SetAutoConnect(const char * pMac, bool bAutoConnect)
{
    return false;
}

// 通知设备要呼出
bool Bluetooth_Callout(const char * pNum)
{
    if (NULL == pNum)
    {
        return false;
    }

    return _BTManager.HeadSetDeviceEvent(HEADSET_CALL_OUT, true, pNum);
}

// 通知设备结束通话
bool Bluetooth_EndCall()
{
    return _BTManager.HeadSetDeviceEvent(HEADSET_END_CALL);
}

// 通知设备进入通话
bool Bluetooth_AnswerCall()
{
    return _BTManager.HeadSetDeviceEvent(HEADSET_ANSWER_CALL);
}

// 通知设备有来电
bool Bluetooth_CallIn(const char * pNum)
{
    if (NULL == pNum)
    {
        return false;
    }

    return _BTManager.HeadSetDeviceEvent(HEADSET_CALL_IN, true, pNum);
}

//蓝牙手机是否连接
bool Bluetooth_IsMobileConnected()
{
    return _BTSync.IsConnected();
}

//调整自动连接开关
bool Bluetooth_SwitchAutoConnect(bool bEnable)
{
    //已经处于扫描、连接中、已连接状态不去开启
    //没有可自动连接的设备不去连接
    if (bEnable
            && (_BTManager.IsInScan()
                || _BTManager.IsConnectedSessionExist()
                || _BTManager.IsConnectingSessionExist()
                || _BTData.GetAutoConnectNum() <= 0))
    {
        return false;
    }

    //已经在自动连接了，不改变下一个连接设备
    if (!_BTManager.IsAutoConnectEnable())
    {
        _BTManager.ResetAutoConnectIndex();
    }

    _BTManager.ResetAutoConnectCount();

    //先设置dsskey 状态
    YLList<int> listID;
    if (dsskey_GetAllDsskeyDataByModule(listID, DMT_LINEKEY_MODULE, 0, DT_BTLINE))
    {
        int iDssskeyID = *listID.begin();
        dsskey_SetDsskeyStatus(iDssskeyID, DS_BTLINE_CONNECTTING);
        dsskey_refreshDSSKeyUI(iDssskeyID);
    }

    _BTManager.SetAutoConnectEnable(true);
    _BTManager.SetAutoConnectStragety(AC_STRATEGY_SHORT_TIME);
    return _BTManager.AutoConnectDevice(true);
}

//设置自动连接开关
void Bluetooth_SetAutoConnectEnable(bool bEnable)
{
    _BTManager.SetAutoConnectEnable(bEnable);
}

//呼出蓝牙话路
int Bluetooth_CallMobileInvite(const char * pNum)
{
    if (NULL == pNum
            || !Bluetooth_IsMobileConnected())
    {
        return false;
    }

    DeviceInfo * pDevice = _BTManager.GetCurrentDevice(SESSION_CONNECTED_STATE);
    if (NULL == pDevice)
    {
        return false;
    }

    return _BTDriver.MobileCallInvite(pDevice->devHandle, pNum);
}

//接起蓝牙话路
bool Bluetooth_AnswerMobile(int iCallID)
{
    if (!Bluetooth_IsMobileConnected())
    {
        return false;
    }

    DeviceInfo * pDevice = _BTManager.GetCurrentDevice(SESSION_CONNECTED_STATE);
    if (NULL == pDevice)
    {
        return false;
    }

    WIRE_INFO("answer mobile[%s][%d] call [%d]", pDevice->strName.c_str(), pDevice->devHandle, iCallID);
    return _BTDriver.MobileCallAnswer(pDevice->devHandle, iCallID);
}

//拒接蓝牙话路
bool Bluetooth_RefuseMobile(int iCallID)
{
    if (!Bluetooth_IsMobileConnected())
    {
        return false;
    }

    DeviceInfo * pDevice = _BTManager.GetCurrentDevice(SESSION_CONNECTED_STATE);
    if (NULL == pDevice)
    {
        return false;
    }

    WIRE_INFO("refuse mobile[%s][%d] call [%d]", pDevice->strName.c_str(), pDevice->devHandle, iCallID);
    return _BTDriver.MobileCallRefuse(pDevice->devHandle, iCallID);
}

//挂断蓝牙话路
bool Bluetooth_HangupMobile(int iCallID)
{
    if (!Bluetooth_IsMobileConnected())
    {
        return false;
    }

    DeviceInfo * pDevice = _BTManager.GetCurrentDevice(SESSION_CONNECTED_STATE);
    if (NULL == pDevice)
    {
        return false;
    }

    WIRE_INFO("hang up mobile[%s][%d] call[%d]", pDevice->strName.c_str(), pDevice->devHandle, iCallID);
    return _BTDriver.MobileCallHangup(pDevice->devHandle, iCallID);
}

//Hold/Resume 蓝牙话路
bool Bluetooth_HoldMobile(int iCallID, bool bHold)
{
    if (!Bluetooth_IsMobileConnected())
    {
        return false;
    }

    DeviceInfo * pDevice = _BTManager.GetCurrentDevice(SESSION_CONNECTED_STATE);
    if (NULL == pDevice)
    {
        return false;
    }

    WIRE_INFO("[%s] Mobile[%s][%d], Call[%d]", bHold ? "Hold" : "Resume", pDevice->strName.c_str(),
              pDevice->devHandle, iCallID);

    return bHold ? _BTDriver.MobileCallHold(pDevice->devHandle,
                                            iCallID) : _BTDriver.MobileCallResume(pDevice->devHandle, iCallID) ;
}

//Send DTMF
bool Bluetooth_SendMobileDTMF(int iCallID, const char cDtmf)
{
    if (!Bluetooth_IsMobileConnected())
    {
        return false;
    }

    DeviceInfo * pDevice = _BTManager.GetCurrentDevice(SESSION_CONNECTED_STATE);
    if (NULL == pDevice)
    {
        return false;
    }

    return _BTDriver.MobileSendDtmf(pDevice->devHandle, iCallID, cDtmf);
}

//获取通话话路状态
int Bluetooth_GetMobileStatus(int iCallID)
{
    if (!Bluetooth_IsMobileConnected())
    {
        return 0;
    }

    DeviceInfo * pDevice = _BTManager.GetCurrentDevice(SESSION_CONNECTED_STATE);
    if (NULL == pDevice)
    {
        return false;
    }

    return _BTDriver.MobileGetCallStatus(pDevice->devHandle, iCallID);
}

//获取通话话路数量
int Bluetooth_GetMobileCallCount()
{
    if (!Bluetooth_IsMobileConnected())
    {
        return false;
    }

    DeviceInfo * pDevice = _BTManager.GetCurrentDevice(SESSION_CONNECTED_STATE);
    if (NULL == pDevice)
    {
        return false;
    }

    return _BTDriver.MobileGetCallCount(pDevice->devHandle);
}

//开始下载联系人
bool Bluetooth_StartDownloadMobileContact(bool bPhoto /* = false */, int iType /* = 500 */)
{
    if (!Bluetooth_IsMobileConnected())
    {
        return false;
    }

    return _BTSync.StartDownloadDirectory(iType, bPhoto);
}

//停止下载
bool Bluetooth_StopDownloadMobileContact()
{
    if (!Bluetooth_IsMobileConnected())
    {
        return false;
    }

    return _BTSync.StopDownload();
}

//切换声音通道
bool Bluetooth_SwitchChannel(bool bLinked, int iCallID)
{
    if (!Bluetooth_IsMobileConnected())
    {
        return false;
    }
    DeviceInfo * pDevice = _BTManager.GetCurrentDevice(SESSION_CONNECTED_STATE);
    if (NULL == pDevice)
    {
        return false;
    }

    return _BTDriver.MobileSwitchChannel(pDevice->devHandle, bLinked, iCallID);
}

//设置自动同步联系人
bool Bluetooth_SetSyncContact(const char * pMac, bool bSync)
{
    if (NULL == pMac)
    {
        return false;
    }

    unsigned long ulDeviceHandle = _BTData.GetDeviceHandleByMac(pMac);
    return _BTManager.SetSyncContact(bSync, ulDeviceHandle);
}

//蓝牙账号ID
int Bluetooth_GetBtAccountID()
{
    return _BTSync.GetAccountID();
}

//获取蓝牙手机信息
const MobileProperty & Bluetooth_GetMobileProperty()
{
    return _BTSync.GetMobileProperty();
}

// 获取蓝牙联系人下载状态
bool Bluetooth_GetMobileContactDownloadStatus(ContactSyncState & eState)
{
    if (!Bluetooth_IsMobileConnected())
    {
        return false;
    }

    eState = _BTSync.GetContactSyncState();

    return true;
}

//设置设备手机通话服务开关
bool Bluetooth_SetDevMobileEnable(unsigned long ulHandle, bool bEnable)
{
    if (ulHandle == BT_DEVICE_HANDLE_INVAILED)
    {
        return false;
    }

    return _BTManager.SetDevMobileEnable(ulHandle, bEnable);
}

//设置设备蓝牙音乐服务开关
bool Bluetooth_SetDevMusicEnable(unsigned long ulHandle, bool bEnable)
{
    if (ulHandle == BT_DEVICE_HANDLE_INVAILED)
    {
        return false;
    }

    return _BTManager.SetDevA2dpEnable(ulHandle, bEnable);
}

//获取蓝牙音乐服务的连接状态
bool Bluetooth_IsDevMusicConnected()
{
    return _BTA2DP.IsA2dpConnected();
}

//是否支持蓝牙音乐
bool Bluetooth_IsSupportMusic(unsigned long ulHandle)
{
    return _BTDriver.IsSupportProfile(ulHandle, PROFILE_A2DP);
}

//蓝牙音乐是否正在播放
bool Bluetooth_IsMusicPlaying()
{
    return _BTA2DP.IsPlaying();
}

//蓝牙音乐是否暂停播放
bool Bluetooth_IsMusicPlayPaused()
{
    return _BTA2DP.IsPlayPaused();
}

//停止/暂停蓝牙音乐播放
bool Bluetooth_StopPlayMusic(bool bPause /*= false*/)
{
    if (bPause && !_BTA2DP.IsPlaying())
    {
        return false;
    }

    A2DP_PLAY_ACTION eAction = bPause ? A2DP_PLAY_PAUSE : A2DP_PLAY_STOP;
    return _BTA2DP.PlayOperation(eAction);
}

//开始音乐播放
bool Bluetooth_StartPlayMusic(bool bResume /*= false*/)
{
    if (bResume && !_BTA2DP.IsPlayPaused())
    {
        return false;
    }

    A2DP_PLAY_ACTION eAction = bResume ? A2DP_PLAY_RESUME : A2DP_PLAY_START;
    return _BTA2DP.PlayOperation(eAction);
}

//设置设备是否可被发现
bool Bluetooth_SetDiscoverable(bool bEnable)
{
    return _BTDriver.SetDiscoverable(bEnable);
}

//是否内置蓝牙
bool Bluetooth_IsBuiltInBluetooth()
{
    return _BTManager.IsBuiltInBluetooth();
}

LRESULT OnBluetoothtManagerMsg(msgObject & objMsg);

#else
#include "modbluetooth.h"
// 模块初始化
void Bluetooth_Init() {}

// 返回当前设备状态
DEVICE_STATE Bluetooth_GetDeviceState()
{
    return DS_NO_DONGLE;
}

// 蓝牙耳麦播放铃声
bool Bluetooth_HSPlayRing()
{
    return false;
}

// 蓝牙耳麦停止铃声
void Bluetooth_HSStopRing() {}

// 蓝牙耳麦停止播放声音
void Bluetooth_HSStopPlay() {}

// 蓝牙耳麦播放声音
void Bluetooth_HSStartPlay() {}

// 蓝牙是否可用
bool Bluetooth_IsEnable()
{
    return false;
}

// 蓝牙是否连接
bool Bluetooth_IsConnected()
{
    return false;
}

// 设置蓝牙是否可用
bool Bluetooth_SetEnable(bool bEnable/* = true*/)
{
    return false;
}

// 获取指定序号的蓝牙信息
bool Bluetooth_GetBluetoothInfoByMac(const char * pMac, DeviceInfo & deviceInfo)
{
    return false;
}

bool Bluetooth_GetBluetoothInfoByHandle(unsigned long ulHandle, DeviceInfo & deviceInfo)
{
    return false;
}

// 获取本地设备信息
bool Bluetooth_GetDeviceInfo(DeviceInfo & deviceInfo)
{
    return false;
}

// 返回蓝牙列表
void Bluetooth_GetDeviceList(UI_LIST_BT_DEVICE & listDevice,
                             DEVICE_LIST_TYPE eType/* DEVICE_LIST_TYPE*/)
{
}

// 扫描设备 (独占，Scan之后不能再做其他事情)
bool Bluetooth_StartScanDevice()
{
    return false;
}

// 停止扫描设备
bool Bluetooth_StopScanDevice()
{
    return false;
}

// 配对时回复pin
bool Bluetooth_AnswerPin(const char * pMac, const char * pPin)
{
    return false;
}

// 配对手机时回复
bool Bluetooth_AnswerMobilePin(const char * pMac)
{
    return false;
}

//取消匹配
bool Bluetooth_CancelPair(const char * pMac)
{
    return false;
}

// 连接指定序号的蓝牙
bool Bluetooth_ConnectDevice(const char * pMac)
{
    return false;
}

// 断开连接
bool Bluetooth_DisconnectDevice(const char * pMac)
{
    return false;
}

// 通过mac删除设备信息
bool Bluetooth_DeleteDevice(const char * pMac/* = NULL*/)
{
    return false;
}

// 设置设备名
bool Bluetooth_SetDeviceName(const char * pName)
{
    return false;
}

// 上下移动设备位置
bool Bluetooth_AdjustDevicePos(const char * pMac, bool bUp)
{
    return false;
}

//调整设备位置
/*
*输入
 * pMac: 设备地址
 * iPos：要移动到的位置
*输出
 * 移动是否成功
*/
bool Bluetooth_MoveDeviceToPos(const char * pMac, UINT iPos)
{
    return false;
}


// 设置是否自动连接
bool Bluetooth_SetAutoConnect(const char * pMac, bool bAutoConnect)
{
    return false;
}

// 通知设备要呼出
bool Bluetooth_Callout(const char * pNum)
{
    return false;
}

// 通知设备结束通话
bool Bluetooth_EndCall()
{
    return false;
}

// 通知设备进入通话
bool Bluetooth_AnswerCall()
{
    return false;
}

// 通知设备有来电
bool Bluetooth_CallIn(const char * pNum)
{
    return false;
}

//蓝牙手机是否连接
bool Bluetooth_IsMobileConnected()
{
    return false;
}

//调整自动连接开关
bool Bluetooth_SwitchAutoConnect(bool bEnable)
{
    return false;
}

// 设置自动连接开关
void Bluetooth_SetAutoConnectEnable(bool bEnable)
{
    return;
}

//蓝牙手机呼出
int  Bluetooth_CallMobileInvite(const char * pNum)
{
    return false;
}

//接起蓝牙手机通话
bool Bluetooth_AnswerMobile(int iCallID)
{
    return false;
}

//拒接蓝牙手机通话
bool Bluetooth_RefuseMobile(int iCallID)
{
    return false;
}

//挂断蓝牙手机通话
bool Bluetooth_HangupMobile(int iCallID)
{
    return false;
}

//HOLD/RESUME 蓝牙手机话路
bool Bluetooth_HoldMobile(int iCallID, bool bHold)
{
    return false;
}

//发送DTMF信息
bool Bluetooth_SendMobileDTMF(int iCallID, const char cDtmf)
{
    return false;
}

//获取手机话路状态
int  Bluetooth_GetMobileStatus(int iCallID)
{
    return false;
}

//获取手机话路数量
int  Bluetooth_GetMobileCallCount()
{
    return false;
}

//开始下载联系人
bool Bluetooth_StartDownloadMobileContact(bool bPhoto/* = false*/, int iType/* = 0x05*/)
{
    return false;
}

//停止下载联系人
bool Bluetooth_StopDownloadMobileContact()
{
    return false;
}

//切换声音通道
bool Bluetooth_SwitchChannel(bool bLinked, int iCallID)
{
    return false;
}

bool Bluetooth_SetSyncContact(const char * pMac, bool bSync)
{
    return false;
}

int Bluetooth_GetBtAccountID()
{
    return -1;
}

const MobileProperty & Bluetooth_GetMobileProperty()
{
    const static MobileProperty mbProperty;
    return mbProperty;
}

bool Bluetooth_GetMobileContactDownloadStatus(ContactSyncState & eState)
{
    return false;
}
// 清空扫描列表
void Bluetooth_ClearScanedList()
{
}
// 返回蓝牙扫描列表
bool Bluetooth_GetScanedDeviceList(UI_LIST_BT_DEVICE & listDevice, int iStartPos/* = 0*/,
                                   int iNumber/* = 911*/)
{
    return false;
}
int Bluetooth_GetScanedDeviceListSize()
{
    return 0;
}

bool Bluetooth_SetDevMobileEnable(unsigned long ulHandle, bool bEnable)
{
    return false;
}

bool Bluetooth_SetDevMusicEnable(unsigned long ulHandle, bool bEnable)
{
    return false;
}

bool Bluetooth_IsDevMusicConnected()
{
    return false;
}

bool Bluetooth_IsSupportMusic(unsigned long ulHandle)
{
    return false;
}

bool Bluetooth_IsMusicPlaying()
{
    return false;
}

bool Bluetooth_IsMusicPlayPaused()
{
    return false;
}

bool Bluetooth_StopPlayMusic(bool bPause /*= false*/)
{
    return false;
}

bool Bluetooth_StartPlayMusic(bool /*bResume = false*/)
{
    return false;
}

bool Bluetooth_SetDiscoverable(bool bEnable)
{
    return false;
}

bool Bluetooth_IsBuiltInBluetooth()
{
    return false;
}
#endif // IF_SUPPORT_BLUETOOTH
