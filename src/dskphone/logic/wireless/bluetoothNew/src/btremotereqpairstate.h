#ifdef IF_SUPPORT_BLUETOOTH
#ifndef __BT_REMOTE_REQ_PAIR_STATE_H__
#define __BT_REMOTE_REQ_PAIR_STATE_H__
#include "../include/btdefines.h"

#ifdef IF_BLUETOOTH_SUPPORT_REMOTE_PAIR_REQ
class CBTRemoteReqPairState :public CBtState
{
public:
	CBTRemoteReqPairState(void);
	virtual ~CBTRemoteReqPairState(void);

	virtual bool AfterStateCreate(long ulDeviceHandle, void* pUserData);
	virtual bool BeforeStateTransfer(bool bEndConnect = false);
	virtual bool ProcessCallbackEvent(msgObject& objMsg);
	virtual bool ProcessOption(const WirelessSessionData& wsData);
	bool ExitState(bool bIsPaired = false);
#if IF_BT_CONNECTING_OVERTIME
	//定时器处理
	virtual bool ProcessTimer(int iTimerID);
#endif // IF_BT_CONNECTING_OVERTIME

protected:
	bool CreatePairDevice(unsigned long ulDeviceHandle);
	bool ProcessRequestPin(unsigned long ulDeviceHandle, const char* pPinCode = NULL);
	bool ProcessAnswerPin(const AuthPinInfo* pAuthInfo);
	bool ProcessPairChange(const btkit_msg_t* pMsg);
	bool ReplyPairReq(unsigned long ulDeviceHandle, bool bAccept);
};
#endif // IF_BLUETOOTH_SUPPORT_REMOTE_PAIR_REQ

#endif // __BT_REMOTE_REQ_PAIR_STATE_H__
#endif // IF_SUPPORT_BLUETOOTH

