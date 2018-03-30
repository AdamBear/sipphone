///////////////////////////////////////////////////////////
//  CBTPairState.h
//  Implementation of the Class CBTPairState
//  Created on:      2014年7月15日 11:14:36
//  Original author: songk
///////////////////////////////////////////////////////////
#ifdef IF_SUPPORT_BLUETOOTH
#ifndef __BTPAIRSTATE_H__
#define __BTPAIRSTATE_H__
#include "../include/btdefines.h"

class CBTPairState : public CBtState
{
public:
    CBTPairState(void);
    virtual ~CBTPairState(void);

    virtual bool AfterStateCreate(long ulDeviceHandle, void * pUserData);
    virtual bool BeforeStateTransfer(bool bEndConnect = false);
    virtual bool ProcessCallbackEvent(msgObject & objMsg);
    virtual bool ProcessOption(const WirelessSessionData & wsData);
    bool ExitState(bool bEndConnect = false, bool bConnectWaittingDevice = true);
#if IF_BT_CONNECTING_OVERTIME
    //定时器处理
    virtual bool ProcessTimer(int iTimerID);
#endif // IF_BT_CONNECTING_OVERTIME

protected:
    bool CreatePairDevice(unsigned long ulDeviceHandle);
    bool AnswerPin(unsigned long ulDeviceHandle, const char * pKey);
    bool ProcessRequestPin(unsigned long ulDeviceHandle, const char * pPinCode = NULL);
    bool ProcessPairChange(const btkit_msg_t * pMsg);
    bool ProcessAnswerPin(const AuthPinInfo * pAuthInfo);
    bool StartPairDevice(unsigned long ulDeviceHandle);
    bool CancelPair(unsigned long ulDeviceHandle);

private:
    bool m_bPinConfirm;
};

#endif //__BTPAIRSTATE_H__
#endif // IF_SUPPORT_BLUETOOTH
