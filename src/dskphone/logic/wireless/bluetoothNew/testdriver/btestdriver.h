#ifdef IF_BT_TEST_LOGIC
#ifndef __BTTESTDRIVER_H__
#define __BTTESTDRIVER_H__

#include "wireless/bluetoothNew/src/bluetooth_inc.h"

class CBTTestDriver
{
private:
    CBTTestDriver(void);
    ~CBTTestDriver(void);

public:
    static CBTTestDriver & GetInstance()
    {
        static CBTTestDriver sInstance;
        return sInstance;
    }

    //初始化
    void Init();

    //消息处理
    BOOL OnMsg(msgObject & msgObj);

    //驱动初始化
    int InitDriver(btkit_handle_t * dongle_hnd, btkit_create_t * create_t);

    //销毁驱动
    int DestoryDriver(btkit_handle_t dongle_hnd);

    //启动
    int PowerSet(btkit_handle_t dongle_hnd, bool bEnable);

    //获取适配器属性
    int GetDongleProperty(btkit_handle_t dongle_hnd, btkit_property_t * dongleProperty);

    //设置适配器名称
    int SetDongleName(btkit_handle_t dongle_hnd, const char * szName);

    //获取设备类型
    btkit_dev_type_t GetDeviceType(btkit_handle_t dongle_hnd, btkit_dev_instance_t dev_hnd);

    //获取设备属性
    int GetDeviceProperty(btkit_handle_t dongle_hnd, btkit_dev_instance_t dev_hnd,
                          btkit_dev_t * devProperty);

    //获取蓝牙手机属性
    int GetMbProperty(btkit_handle_t dongle_hnd, btkit_dev_instance_t dev_hnd,
                      btkit_mb_property_t * property);

    //获取设备地址
    int GetDeviceMac(btkit_handle_t dongle_hnd, btkit_dev_instance_t dev_hnd, char * szMac);

    //设备是否匹配
    bool IsDevicePaired(btkit_handle_t dongle_hnd, btkit_dev_instance_t dev_hnd);

    //设备是否连接
    bool IsDeviceConnected(btkit_handle_t dongle_hnd, btkit_dev_instance_t dev_hnd);

    //获取设备数量
    int GetDeviceSum(btkit_handle_t dongle_hnd);

    //获取设备句柄列表
    int GetDeviceHandle(btkit_handle_t dongle_hnd, btkit_dev_instance_t * devList);

    //开始扫描
    int StartScan(btkit_handle_t dongle_hnd);

    //停止扫描
    int StopScan(btkit_handle_t dongle_hnd);

    //连接设备
    int ConnectDevice(btkit_handle_t dongle_hnd, btkit_dev_instance_t dev_hnd);

    //取消连接设备
    int DisconnectDevice(btkit_handle_t dongle_hnd, btkit_dev_instance_t dev_hnd);

    //匹配设备
    int PairDevice(btkit_handle_t dongle_hnd, btkit_dev_instance_t dev_hnd);

    //取消匹配
    int CancelPairDevice(btkit_handle_t dongle_hnd, btkit_dev_instance_t dev_hnd);

    //应答PIN码
    int AnswerPin(btkit_handle_t dongle_hnd, btkit_dev_instance_t dev_hnd, const char * szPin);

    //移除设备
    int RemoveDevice(btkit_handle_t dongle_hnd, btkit_dev_instance_t dev_hnd);

    //设备呼入
    int DeviceCallIn(btkit_handle_t dongle_hnd, btkit_dev_instance_t dev_hnd, const char * szNum);

    int DeviceCallInvite(btkit_handle_t dongle_hnd, btkit_dev_instance_t dev_hnd, const char * szNum,
                         int * iCallID);

    //设备呼出
    int DeviceCallOut(btkit_handle_t dongle_hnd, btkit_dev_instance_t dev_hnd, const char * szNum,
                      int * callid);

    //应答通话
    int DeviceCallAnswer(btkit_handle_t dongle_hnd, btkit_dev_instance_t dev_hnd, int iCallID);

    //结束通话
    int DeviceCallEnd(btkit_handle_t dongle_hnd, btkit_dev_instance_t dev_hnd, int iCallID);

    //Hold 通话
    int DeviceCallHold(btkit_handle_t dongle_hnd, btkit_dev_instance_t dev_hnd, int iCallID, int iHold);

    //播放铃声
    int DevicePlayRing(btkit_handle_t dongle_hnd, btkit_dev_instance_t dev_hnd);

    //停止播放铃声
    int DeviceStopRing(btkit_handle_t dongle_hnd, btkit_dev_instance_t dev_hnd);

    //发送dtmf
    int DeviceSendDtmf(btkit_handle_t dongle_hnd, btkit_dev_instance_t dev_hnd, const char cDtmf);

    //获取通话数量
    int GetCallSum(btkit_handle_t dongle_hnd, btkit_dev_instance_t dev_hnd, int * iTotal);

    //获取通话状态
    int GetCallStatus(btkit_handle_t dongle_hnd, btkit_dev_instance_t dev_hnd, int iCallID,
                      int * iCallStatus);

    //开始下载联系人
    int StartDownloadContact(btkit_handle_t hnd, btkit_dev_instance_t dev_hnd, btkit_pba_data_t * data);

    //取消下载联系人
    int CancelDownloadContact(btkit_handle_t hnd, btkit_dev_instance_t dev_hnd);

    //切换声音通道
    int SwitchChannel(btkit_handle_t dongle_hnd, btkit_dev_instance_t dev_hnd, int iCallID, int iMode);

    int PinConfirm(btkit_handle_t dongle_hnd, btkit_dev_instance_t dev_hnd, int iConfirm);
    //接口测试
    void OnInterfaceTest(msgObject & objMessage);

    //回调消息
    bool SendCallBackMsg(btkit_msg_t * pMsg, void * pUserMag);

protected:
    //定时器处理
    bool ProcessTimer(int iTimerID);

    //处理外部模拟消息
    BOOL ProcessTestThreadMsg(msgObject & objMsg);

    //耳机接起电话
    bool ProcessHSAnswerCall(int iCallID);

    //耳机挂断电话
    bool ProcessHSEndCall(int iCallID);

    //断开设备
    bool ProcessDisconnectDevice();

    //来电
    bool ProcessIncommingCall();

    //挂断
    bool ProcessHangUpCall(int iCallID);

    //HOLD
    bool ProcessHoldCall(int iCallID, int bHold);

    //处理关闭电源
    bool ProcessPowerOff();

    bool IsInScan()
    {
        return _WirelessSessionManager.IsInState(BT_TEST_SESSION, SESSION_SCAN_STATE,
                BT_DEVICE_HANDLE_INVAILED, NULL);
    }

    bool SendOptionToSession(unsigned long ulMessage, WPARAM wParam, LPARAM lParam, void * pData);

    int GetFoucsCallID(CSessionBase * pSession = NULL);
    int GetCallCount();
    int GetCallStatus(int iCallID);
    unsigned long GetCurrendDevHandle();

private:
    bool m_bPower;

    btkit_handle_t m_btHandle;

    btkit_event_t * m_callbackFunc;

    int m_iDownloadTimer;

    bool m_bChannelInMb;

};

#define _BTTestDriver CBTTestDriver::GetInstance()
#endif // __BTTESTDRIVER_H__
#endif // IF_BT_TEST_LOGIC
