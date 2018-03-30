#ifndef __ACD_MODEL_H__
#define __ACD_MODEL_H__

#ifdef IF_FEATURE_BROADSOFT_CALL_CENTER
// #include <ylstring.h>
// #include "cccommon.h"
// #include <ylhashmap.h>
// #include <ETLLib.hpp>
// #include "interfacedefine.h"
#include "acdmodelbase.h"

class CDsskeyAcdTrace;
class CDssKeyACD;
class CDsskeyDispositionCode;
class CDsskeyEmergencyEscalation;

class ACDModel : public CACDModelBase
{
public:
    virtual bool ReqLogin(acd_accout * pAcd = NULL);
    //请求状态改变，包括登陆，登陆时需考虑考虑初始化状态
    virtual bool ReqChangeACDStatus(ACD_ACTION eACDAction);

    virtual bool ProcessAfterLogin();

    //绑定账号
    void SetAccountId(int iAccountId);

    //设置初始化状态
    bool SetInitStatus(ACD_STATUS eStatus);

    //获取初始登陆状态
    ACD_STATUS GetInitStatus();

    //设置Unavailable Reason
    void SetCurReason(const yl::string & strReason);

    void GetCurReason(yl::string & strReasonName);

    //通过Reason Code 获取ReasonCode Name
    yl::string GetReasonCodeNameByCode(const yl::string & strCode);

    //设置当前Call Center信息
    bool SetCurCallInfo(yl::string & refData, int nCallerId);

    //返回Call Center信息
    const CallCenterInfo & GetCallInfo();

    //返回队列状态
    const CallCenterQueueStatus & GetQueueStatus()
    {
        return m_curQueueStatus;
    }

    //向服务器发送Disposition Code
    bool SendDispCode(const yl::string & strCode, int nCallId = -1);

    //像服务器发送追踪信令
    bool OriginatedTrace(const int nCallId = -1);

    //向班长发起3方通话
    bool EmergencyEscalation(const yl::string & strNumber, int nCallId = -1);

    void GetDispCodeList(DispCodeList & refList);

    void GetSupervisorList(SupervisorList & refList);

    //返回CallInfoCallerId
    int GetCallInfoCallerId()
    {
        return m_nCallerId;
    }

    bool IsUnavailReasonCodeOn();

    // 处理计时器到时消息.
    BOOL OnTimeoutMessage(msgObject & refObj);

    virtual void SetStatus(ACD_STATUS eStatus);

private:
    //解析sip发送的QueueStatus信息
    bool ProcessQueueStatusXml(yl::string & strInfo);

    bool ReqQueueStatus(bool isOn = true);

    ACDModel();

    virtual ~ACDModel();
private:

    //ACD_STATUS m_eInitStatus;//初始化状态，登陆时开始状态

    yl::string m_strCurReason;//当前Reason

    CallCenterQueueStatus m_curQueueStatus;//当前队列信息

    CallCenterInfo m_curCallInfo;//当前Call Center信息

    int m_nCallerId;

    bool m_bIsQueueStatusOn;

    bool m_bIslaterQeqQueueStatus;

    mkThreadTimer m_objTimer;  // 计时器.

    friend class CallCenterController;

    CDsskeyAcdTrace * m_pDsskeyAcdTrace;

    CDssKeyACD * m_pDsskeyAcd;

    CDsskeyDispositionCode * m_pDsskeyDis;

    CDsskeyEmergencyEscalation * m_pDsskeyEm;

};
#endif //IF_FEATURE_BROADSOFT_CALL_CENTER

#endif
