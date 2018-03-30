//#if IF_SUPPORT_BLUETOOTH
#ifndef __MOD_BLUETOOTH_H__
#define __MOD_BLUETOOTH_H__
#include "btdefines.h"

// 模块初始化
void Bluetooth_Init();

// 返回当前设备状态
DEVICE_STATE Bluetooth_GetDeviceState();

// 蓝牙耳麦播放铃声
bool Bluetooth_HSPlayRing();

// 蓝牙耳麦停止铃声
void Bluetooth_HSStopRing();

// 蓝牙耳麦停止播放声音
void Bluetooth_HSStopPlay();

// 蓝牙耳麦播放声音
void Bluetooth_HSStartPlay();

// 蓝牙耳麦是否连接
bool Bluetooth_IsHeadsetConnected();

// 蓝牙是否可用
bool Bluetooth_IsEnable();

// 蓝牙是否连接
bool Bluetooth_IsConnected();

// 设置蓝牙是否可用
bool Bluetooth_SetEnable(bool bEnable = true);

// 获取指定序号的蓝牙信息
bool Bluetooth_GetBluetoothInfoByMac(const char * pMac, DeviceInfo & deviceInfo);

// 获取指定handle的蓝牙信息
bool Bluetooth_GetBluetoothInfoByHandle(unsigned long ulHandle, DeviceInfo & deviceInfo);

// 获取本地设备信息
bool Bluetooth_GetDeviceInfo(DeviceInfo & deviceInfo);

// 获取连接上的设备信息
bool Bluetooth_GetPairedInfo(DeviceInfo & deviceInfo);

// 返回蓝牙列表
void Bluetooth_GetDeviceList(UI_LIST_BT_DEVICE & listDevice,
                             DEVICE_LIST_TYPE eType = DEVICE_LIST_ALL);

// 返回蓝牙列表
// 支持获取链表区间 iStartPos: 起始位置(0标识第一个), iNumber: 获取个数(911 标识获取iStartPos~结束位置)
bool Bluetooth_GetScanedDeviceList(UI_LIST_BT_DEVICE & listDevice, int iStartPos = 0,
                                   int iNumber = 911);

// 清空扫描列表
void Bluetooth_ClearScanedList();

// 返回蓝牙扫描列表设备个数
int Bluetooth_GetScanedDeviceListSize();

// 扫描设备 (独占，Scan之后不能再做其他事情)
bool Bluetooth_StartScanDevice();

// 停止扫描设备
bool Bluetooth_StopScanDevice();

// 配对时回复pin
bool Bluetooth_AnswerPin(const char * pMac, const char * pPin);

// 配对手机时回复
bool Bluetooth_AnswerMobilePin(const char * pMac);

// 取消匹配
bool Bluetooth_CancelPair(const char * pMac);

// 连接指定序号的蓝牙
bool Bluetooth_ConnectDevice(const char * pMac);

// 断开连接
bool Bluetooth_DisconnectDevice(const char * pMac);

// 通过mac删除设备信息
bool Bluetooth_DeleteDevice(const char * pMac = NULL);

// 设置设备名
bool Bluetooth_SetDeviceName(const char * pName);

// 上下调整设备位置
bool Bluetooth_AdjustDevicePos(const char * pMac, bool bUp);

// 移动设备位置
bool Bluetooth_MoveDeviceToPos(const char * pMac, UINT iPos);

// 设置是否自动连接
bool Bluetooth_SetAutoConnect(const char * pMac, bool bAutoConnect);

// 通知设备要呼出
bool Bluetooth_Callout(const char * pNum);

// 通知设备结束通话
bool Bluetooth_EndCall();

// 通知设备进入通话
bool Bluetooth_AnswerCall();

// 通知设备有来电
bool Bluetooth_CallIn(const char * pNum);

//蓝牙手机是否连接
bool Bluetooth_IsMobileConnected();

// 重新开始自动连接
bool Bluetooth_SwitchAutoConnect(bool bEnable);

// 设置自动连接开关
void Bluetooth_SetAutoConnectEnable(bool bEnable);

//蓝牙手机呼出
int  Bluetooth_CallMobileInvite(const char * pNum);

//接起蓝牙手机通话
bool Bluetooth_AnswerMobile(int iCallID);

//拒接蓝牙手机通话
bool Bluetooth_RefuseMobile(int iCallID);

//挂断蓝牙手机通话
bool Bluetooth_HangupMobile(int iCallID);

//HOLD/RESUME 蓝牙手机话路
bool Bluetooth_HoldMobile(int iCallID, bool bHold);

//发送DTMF信息
bool Bluetooth_SendMobileDTMF(int iCallID, const char cDtmf);

//获取手机话路状态
int  Bluetooth_GetMobileStatus(int iCallID);

//获取手机话路数量
int  Bluetooth_GetMobileCallCount();

//开始下载联系人
bool Bluetooth_StartDownloadMobileContact(bool bPhoto = false, int iType = 0x05);

//停止下载联系人
bool Bluetooth_StopDownloadMobileContact();

//切换声音通道
bool Bluetooth_SwitchChannel(bool bLinked, int iCallID);

//设置联系同步开关
bool Bluetooth_SetSyncContact(const char * pMac, bool bSync);

//获取蓝牙账号ID
int Bluetooth_GetBtAccountID();

//获取蓝牙手机信息
const MobileProperty & Bluetooth_GetMobileProperty();

//获取蓝牙联系人下载状态
bool Bluetooth_GetMobileContactDownloadStatus(ContactSyncState & eState);

//设置设备手机通话服务开关
bool Bluetooth_SetDevMobileEnable(unsigned long ulHandle, bool bEnable);

//设置设备蓝牙音乐服务开关
bool Bluetooth_SetDevMusicEnable(unsigned long ulHandle, bool bEnable);

//获取蓝牙音乐服务的连接状态
bool Bluetooth_IsDevMusicConnected();

//是否支持蓝牙音乐
bool Bluetooth_IsSupportMusic(unsigned long ulHandle);

//蓝牙音乐是否正在播放
bool Bluetooth_IsMusicPlaying();

//蓝牙音乐是否暂停播放
bool Bluetooth_IsMusicPlayPaused();

//停止/暂停蓝牙音乐播放
bool Bluetooth_StopPlayMusic(bool bPause = false);

//开始音乐播放
bool Bluetooth_StartPlayMusic(bool bResume = false);

//设置设备是否可被发现
bool Bluetooth_SetDiscoverable(bool bEnable);

//是否内置蓝牙
bool Bluetooth_IsBuiltInBluetooth();

#endif // __MOD_BLUETOOTH_H__
//#endif // #if IF_SUPPORT_BLUETOOTH
