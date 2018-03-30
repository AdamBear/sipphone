///////////////////////////////////////////////////////////
//  CBTConnectedState.h
//  Implementation of the Class CBTConnectedState
//  Created on:      2014年7月15日 11:14:36
//  Original author: songk
///////////////////////////////////////////////////////////
#ifdef IF_SUPPORT_BLUETOOTH
#ifndef __BTCONNECTEDSTATE_H__
#define __BTCONNECTEDSTATE_H__

class CBTConnectedState : public CBtState
{
public:
    CBTConnectedState(void);
    virtual ~CBTConnectedState(void);

    //父类继承
    virtual bool AfterStateCreate(long ulDeviceHandle, void * pUserData);
    virtual bool BeforeStateTransfer(bool bEndConnect = false);
    virtual bool ProcessCallbackEvent(msgObject & objMsg);
    virtual bool ProcessOption(const WirelessSessionData & wsData);
    bool ExitState(bool bEndConnect  = false, bool bConnectWattingDevice = true);

protected:
    //连接成功处理函数
    bool ProcessConnected(unsigned long ulDeviceHandle);
    //处理断开连接
    bool ProcessDisconnected();

    bool ProcessHeadSetEvent(const WirelessSessionData & wsData);

    //断开设备
    bool DisconnectDevice(unsigned long ulDeviceHandle);
    //处理声音通道
    void SetChannel(bool bOpen);

private:
    bool m_bIsPlay;
};

#endif //__BTCONNECTEDSTATE_H__
#endif // IF_SUPPORT_BLUETOOTH
