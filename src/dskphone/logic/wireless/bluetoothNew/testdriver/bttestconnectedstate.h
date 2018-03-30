#ifdef IF_BT_SIMULATION_DRIVER
#ifndef __BTTESTCONNECTEDSTATE_H__
#define __BTTESTCONNECTEDSTATE_H__
#include "wireless/bluetoothNew/src/bluetooth_inc.h"

class CBTTestConnectedState : public CBtState
{
public:
    CBTTestConnectedState();
    virtual ~CBTTestConnectedState();

    virtual bool AfterStateCreate(long ulDeviceHandle, void * pUserData);
    virtual bool BeforeStateTransfer(bool bEndConnect = false);
    virtual bool ProcessOption(const WirelessSessionData & wsData);

    int GetFoucsCallID()
    {
        return iFoucuCallID;
    }
    int GetCallCount()
    {
        return m_listcallstatus.size();
    }
    int GetCallStatus(int iCallID);

protected:
    bool ReplyMessage(unsigned long ulMsg);
    bool ReplyCallMessage(unsigned long ulMsg, btkit_mb_call_status_t & callStatus);
    bool ProcessCallIncoming(const yl::string & strNum);
    bool ProcessCallOut(const yl::string & strNum);
    bool ProcessCallHold(int iCallID, int iHold);
    bool ProcessCallAnswer(int iCallID);
    bool ProcessCallEnd(int iCallID = -1);
    void SetFocusCall(int iCallID = -1);

    void DoEndCall(int iCallID);
private:
    list_callstatus   m_listcallstatus ;
    int iFoucuCallID;
};

#endif // __BTTESTCONNECTEDSTATE_H__
#endif // IF_BT_SIMULATION_DRIVER
