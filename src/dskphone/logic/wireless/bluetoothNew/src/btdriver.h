///////////////////////////////////////////////////////////
//  CBTDriver.h
//  Implementation of the Class CBTDriver
//  Created on:      25-三月-2014 17:22:21
//  Original author: songk
///////////////////////////////////////////////////////////
#ifdef IF_SUPPORT_BLUETOOTH
#ifndef __BTDRIVER__INCLUDED_
#define __BTDRIVER__INCLUDED_

#include "wireless/bluetoothNew/include/btdefines.h"

class CBTDriver
{
    DECLARE_SINGLETON_CLASS(CBTDriver)

public:
    // bluetooth事件处理回调
    static void BlueToothEventCallBack(const btkit_msg_t * msg, void * user_data);

    //获取设备地址
    void GetDeviceMac(btkit_dev_handle_t devHandle, yl::string & strMac);

    //蓝牙适配器是否初始化
    btkit_adp_handle_t GetDongleInstance()
    {
        return m_instDongle;
    }

    // 获取蓝牙设备的属性
    bool GetDeviceProperty(btkit_dev_handle_t devHandle, DeviceProperty & devProperty);

    //获取蓝牙手机的属性
    bool GetMbProperty(btkit_dev_handle_t devHandle, MobileProperty & mbProperty);

    //判断设备是否已连接
    bool IsDeviceConnectedInDriver(btkit_dev_handle_t devHandle);

    //判断设备是否已匹配(驱动中判断)
    bool IsDevicePairedInDriver(btkit_dev_handle_t devHandle);

    //获取蓝牙设备类型
    btkit_dev_type_t GetDeviceType(btkit_dev_handle_t devHandle);

    int ConnectPairedDevice(btkit_dev_handle_t devHandle, bool bAutoConnect);
    bool DisconnectDevice(btkit_dev_handle_t devHandle);
    // 返回值0:连接成功，1:已匹配，可以直接进行连接
    int PairDevice(btkit_dev_handle_t devHandle);
    bool CancelPairDevice(btkit_dev_handle_t devHandle);
    bool UnPairDevice(btkit_dev_handle_t devHandle);
    bool DeleteDevice(btkit_dev_handle_t devHandle);
    bool ScanDevice();
    bool StopScanDevice();
    bool SetModuleName(const char * pDeviceName);
    bool AnswerPin(btkit_dev_handle_t devHandle, const char * pPin);
    bool CallIn(btkit_dev_handle_t devHandle, const char * pNum);
    bool CallAnswer(btkit_dev_handle_t devHandle);
    bool CallEnd(btkit_dev_handle_t devHandle);
    bool CallOut(btkit_dev_handle_t devHandle, const char * pNum);
    bool Ring(btkit_dev_handle_t devHandle, bool bRing);
    bool Play(btkit_dev_handle_t devHandle, bool bPlay);
    bool AnswerAuth(btkit_dev_handle_t devHandle, bool bAuth);
    bool PinConfirm(btkit_dev_handle_t devHandle, bool bConfirm);
    //获取蓝牙模块信息
    bool GetBluetoothModuleInfo(DeviceInfo & info);
    //初始化蓝牙驱动
    bool InitDriver();

    //设置日志等级
    void SetDriverLogLevel(int iLevel);

    //初始化蓝牙适配器
    bool PowerSet(bool bEnable);

    //关闭蓝牙适配器
    bool DestoryDongle();

    //枚举设备
    bool IterateDevice(YLList<DeviceProperty> & listProperty);

    // 呼出话路
    int MobileCallInvite(btkit_dev_handle_t devHandle, const char * pNum);
    //通话话路
    bool MobileCallAnswer(btkit_dev_handle_t devHandle, int iCallID);
    //拒绝话路
    bool MobileCallRefuse(btkit_dev_handle_t devHandle, int iCallID);
    //挂断话路
    bool MobileCallHangup(btkit_dev_handle_t devHandle, int iCallID);
    //Hold 话路
    bool MobileCallHold(btkit_dev_handle_t devHandle, int iCallID);
    //Resume 话路
    bool MobileCallResume(btkit_dev_handle_t devHandle, int iCallID);
    // 发送dtmf
    bool MobileSendDtmf(btkit_dev_handle_t devHandle, int iCallID, char cDtmf);
    //获取通话状态
    int MobileGetCallStatus(btkit_dev_handle_t devHandle, int iCallID);
    //获取话路数量
    int MobileGetCallCount(btkit_dev_handle_t devHandle);
    //开始下载手机联系人
    bool MobileStartDownloadContact(btkit_dev_handle_t devHandle,
                                    int iType = BT_PB_TYPE_PHONE_PB, bool bPhoto = false);
    //停止下载
    bool MobileStopDownload(btkit_dev_handle_t devHandle);
    //切换声音源
    bool MobileSwitchChannel(btkit_dev_handle_t devHandle, bool bLink, int iCallID);
    // --------------- profile operation ------------------
    // 连接profile
    int ConnectProfiles(btkit_dev_handle_t devHandle, int iProfiles);
    // 断开profile
    int DisconnectProfiles(btkit_dev_handle_t devHandle, int iProfiles);
    // 设置profile开关
    bool SetProfileEnable(btkit_dev_handle_t devHandle, int iProfile, bool bEnable);
    // profile开关是否开启
    bool IsProfileEnable(unsigned long ulHandle, int iProfileType);
    // profile是否连接
    bool IsProfileConnected(btkit_dev_handle_t devHandle, int iProfile);
    // 查询设备是否支持某种profile
    bool IsSupportProfile(btkit_dev_handle_t devHandle, int iProfile);
    // 设置设备蓝牙是否可被发现
    bool SetDiscoverable(bool bEnable);
    // 将profile类型转换为UUID
    int GetUUIDByProfileType(int iProfileType);

private:

protected:
    //蓝牙驱动句柄
    btkit_adp_handle_t  m_instDongle;
};

#define _BTDriver GET_SINGLETON_INSTANCE(CBTDriver)

#endif // !defined(__BTDRIVER__INCLUDED_)
#endif // IF_SUPPORT_BLUETOOTH
