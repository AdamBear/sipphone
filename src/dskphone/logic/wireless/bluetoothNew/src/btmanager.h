///////////////////////////////////////////////////////////
//  CBTManager.h
//  Implementation of the Class CBTManager
//  Created on:     2014年7月15日 11:14:36
//  Original author: songk
///////////////////////////////////////////////////////////
#ifdef IF_SUPPORT_BLUETOOTH
#ifndef __BTMAMAGER_H__
#define __BTMAMAGER_H__

#include "taskaction/taskactioncallbackbase.h"

#define _BTData  _BTManager.GetBTDataInstance()

class CBTManager : public NS_TA::CTaskActionCallbackBase
{
    DECLARE_SINGLETON_CLASS(CBTManager)

public:
    //初始化
    bool Init();

    // 初始化蓝牙
    bool InitBluetooth();

    // 获取设备状态
    DEVICE_STATE GetDeviceState();

    //是否插入适配器
    bool IsDongleValid();
    //（蓝牙功能是否为开启）

    bool IsEnable()
    {
        return m_bEnable;
    }

    //是否在搜索状态
    bool IsInScan();

    //
    bool IsInPair(unsigned long ulDevHandle = BT_DEVICE_HANDLE_INVAILED);
    //开关蓝牙会话
    bool SetBTOpenStatus(bool bEnable, bool bSetConfig = true);

    //子线程设置驱动开关
    bool SetPowerByThread(bool bEnable);

    //消息处理入口
    bool OnRecieveMsg(msgObject & objMsg);

    //删除设备
    bool DeleteDevice(unsigned long ulDeviceHandle, bool bDelAll);

    //设置蓝牙显示名称
    bool SetDeviceName(const char * pName, bool bUpdateLevel = false);

    //设置蓝牙自动连接属性
    bool SetDeviceAutoConnect(bool bAutoConnnect, unsigned long ulDeviceHandle);
    //设置同步联系人
    bool SetSyncContact(bool bSync, unsigned long ulDeviceHandle);

    bool SetDevMobileEnable(unsigned long ulHandle, bool bEnable, bool bProcessProfile = true);


    bool SetDevA2dpEnable(unsigned long ulHandle, bool bEnable, bool bProcessProfile = true);

    //通知UI刷新
    void UpdateUI(OPERATION_RESULT eState = OP_UPDATE_LIST, bool bSuccessed = true,
                  unsigned long ulDevHandle = BT_DEVICE_HANDLE_INVAILED);

    //连接等待对列的设备
    bool ConnectWaittingDevice(unsigned long ulDeviceHandle = BT_DEVICE_HANDLE_INVAILED);

    //设置蓝牙模块状态
    void SetDeviceState(DEVICE_STATE eState);

    //重置自动连接索引
    void ResetAutoConnectIndex()
    {
        m_iAutoConnectIndex = -1;
    }

    //重置自动连接次数
    void ResetAutoConnectCount(int iCount = 0)
    {
        m_iAutoConnectCount = iCount;
    }

    int GetCurrentAutoConnectCount();

    //获取数据类引用
    CBTData & GetBTDataInstance()
    {
        return m_btData;
    }

    //开关自动连接定时器
    bool SetAutoConnectTimer(bool bEnable);

    //设置自动连接策略
    void SetAutoConnectStragety(AUTOCONNECT_STRATEGY eStragety);

    AUTOCONNECT_STRATEGY GetAutoConnectStragety()
    {
        return m_AutoConnectInfo.eACStrategy;
    };

    //设置自动连接开关
    void SetAutoConnectEnable(bool bEnable);

    //获取自动连接状态
    bool IsAutoConnectEnable()
    {
        return m_AutoConnectInfo.bAutoConnect;
    }

    //设置是否为自动连接
    bool SetSessionAutoConnect(unsigned long ulDeviceHandle, bool bAutoConnect);

    //是否已连接
    bool IsConnectedSessionExist();

    //设备是否连接中
    bool IsConnectingSessionExist(CSessionBase * pExceptThis = NULL);

    //获取Mac地址
    DeviceInfo * GetCurrentDevice(WIRELESS_SESSION_STATE eState);

    //连接设备
    bool ProcessConnectDevice(unsigned long ulDevHandle, bool bAutoConnect = false);

    //断开/取消连接设备
    bool ProcessUnConnectDevice(unsigned long ulDevHandle, bool bEndConnect = true);

    //扫描消息
    bool ProcessScanDevice(bool bScan);

    //HeadSet PlayRing
    bool HeadSetDevicePlayRing(bool bRing);

    bool HeadSetDeviceEvent(HEADSET_EVENT eHeadSetEvent, bool bChoice = true, const char * pNum = NULL);

    //删除话路
    bool DeleteSession(CBtSession * pSession, bool bEndConnect = false, bool bAutoConnect = true);

    //for debug
    static const char * SessionStateToString(WIRELESS_SESSION_STATE state);

    static void SetBtLogLevel(int iModuleID, int iLogLevel);

    //自动连接
    bool AutoConnectDevice(bool bUpdateIndex = false);

    //是否允许自动连接
    bool IsAllowAutoConnect();

    //是否支持某项功能特性
    bool IsSupportFeature(BT_FEATURE_SUPPROT eFeature);

    // 是否内置蓝牙
    bool IsBuiltInBluetooth();

    /***
     * @brief : Action执行结束的回调
     * @param : |pTaskAction| 执行结束的action
     */
    virtual void OnTaskActionCallback(NS_TA::CTaskAction* pTaskAction);

protected:
    //初始化蓝牙模块
    bool DoInit();

    //关闭蓝牙模块
    bool DoClose();

    //是否允许连接
    bool IsAllowConnect(unsigned long ulDevHandle);

    //判断设备是否需要自动连接 ---- 断开原因在net记录
    bool IsDevAllowACInDriver(unsigned long ulDevHandle);

    //注册消息
    void RegisterMsg();

    //反注册消息
    void UnRegisterMsg();

    //适配器插入
    bool ProcessAdapterAdded(const btkit_msg_t * pMsg);

    //适配器拔出
    bool ProcessAdapterRemoved(const btkit_msg_t * pMsg);

    //模拟EHS按键处理函数
    bool ProcessKey(msgObject & objMsg);

    bool ProcessProfileConnectStatusChange(msgObject& objMsg);

    //UI应答PIN码
    bool ProcessChannel(bool bConnected);

    //设备消失
    bool ProcessCallbackDeviceDisappeared(const btkit_msg_t * pMsg);

    //设备配置更改
    bool ProcessCallbackPropertyChanged(const btkit_msg_t * pMsg);

    bool DoTaskFailed(BT_TASK_TYPE eType, const BTTaskParam* pParam);

    //设置开关状态
    void SetEnable(bool bEnable, bool bSetConfig = true);

    //处理会话层的蓝牙回调消息
    bool ProcessCallBackEvent(msgObject & objMsg);

    //处理断开后处理
    bool ProcessCallBackDisconnected(unsigned long ulDeviceHandle);

    //处理重拨
    bool ProcessCallBackRedial();

    //处理定时器消息
    bool ProcessTimer(int iTimerID);

    bool ProcessPinConfirm(msgObject& objMsg);

    //初始化一个搜索会话
    bool InitSession(WIRELESS_SESSION_STATE eState,
                     unsigned long ulDeviceHandle = BT_DEVICE_HANDLE_INVAILED, void * pData = NULL);

    bool DestorySessionByState(WIRELESS_SESSION_STATE eState, bool bEndConnect = false)
    {
        return _WirelessSessionManager.DestorySessionByState(eState, BT_SESSION, bEndConnect);
    }

    //获取一个会话
    CBtSession * GetSessionByHandle(unsigned long ulDeviceHandle)
    {
        CSessionBase * pSession = _WirelessSessionManager.GetSessionByHandle(ulDeviceHandle, BT_SESSION);
        return dynamic_cast<CBtSession *>(pSession);
    }

    CBtSession * GetSessionByState(WIRELESS_SESSION_STATE eState)
    {
        CSessionBase * pSession = _WirelessSessionManager.GetSessionByState(eState, BT_SESSION);
        return dynamic_cast<CBtSession *>(pSession);
    }

    //发送消息到会话
    bool SendCallbackMsgToSession(unsigned long ulDeviceHandle, msgObject & objMsg)
    {
        return _WirelessSessionManager.SendCallBackMsgToSessionByHandle(ulDeviceHandle, objMsg, BT_SESSION);
    }

    bool SendCallbackMsgToSession(WIRELESS_SESSION_STATE eState, msgObject & objMsg)
    {
        return _WirelessSessionManager.SendCallBackMsgToSessionByState(eState, objMsg, BT_SESSION);
    }

private:
    bool m_bEnable;
    const int m_iAutoConnectTimer;

    int m_iAutoConnectIndex;
    int m_iAutoConnectCount;

    static CBTData  m_btData;
    DEVICE_STATE    m_eState;
    AutoConenctInfo m_AutoConnectInfo;
};

#define _BTManager GET_SINGLETON_INSTANCE(CBTManager)

#endif //__BTMAMAGER_H__
#endif // IF_SUPPORT_BLUETOOTH
