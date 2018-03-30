#ifndef __CSTA_PROCESSER_H__
#define __CSTA_PROCESSER_H__
#include "xmlparser.hpp"
#include "ylstring.h"
#include "ETLLib.hpp"

#include "ylhashmap.h"
#include "../../actionuri_url/include/action_def.h"
#include "talk/fwd_dnd/include/fwddndmsgdefine.h"

#if IF_FEATURE_EVENT_MODULE
#include "extended/event/src/eventbase.h"
#endif

enum DeliveredEventType
{
    DELIVERED_CALLOUT,

    DELIVERED_CALLIN,
};


class CCallInfo;

struct MonitorInfo
{
    yl::string m_strCrossId;
    yl::string m_strAccountInfo;
};

typedef YLHashMap<int, MonitorInfo> MonitorMap;

#if IF_FEATURE_EVENT_MODULE
class CCSTAProcesser : public CEventBase
#else
class CCSTAProcesser
#endif
{
public:

    static CCSTAProcesser & GetInstace();

    bool ProcessCSTA(msgObject & objMessage);

#if !IF_FEATURE_EVENT_MODULE
    bool ProcessDndStatusChanged(msgObject & objMessage);
    bool ProcessFwdStatusChanged(msgObject & objMessage);
#endif // !IF_FEATURE_EVENT_MODULE

    bool IsNeedSendCstaEvent(int nLineId);

    //void OnRemoteControlConfigClose();


#if IF_FEATURE_EVENT_MODULE
    // 发送事件
    virtual bool SendEvent(EventDataBase * pData);
#endif

public:
#if IF_FEATURE_EVENT_MODULE
    // 发送基础事件
    bool SendBaseEvent(EventDataBase * pData);
    // 发送Froward事件
    bool SendForwardEvent(EventDataBase * pData);
    // 发送通话事件
    bool SendTalkEvent(EventDataBase * pData);
#else
    void SendCSTAEvent(ActionUrlType eType, const CCallInfo * pCallInfo);
#endif
    bool SendDeviceChangeInfo();

    bool SendDeviceInfo();

#if !IF_FEATURE_EVENT_MODULE
    void SendForwardEvent(int nLine, FwdMessageStatusData * pFwdData);
#endif

    void SendDnDEvent(int nLine, bool bEnable);

    void SendConferenceEvent(const CCallInfo * pFirst, const CCallInfo * pSecond);

private:
    CCSTAProcesser();
    virtual ~CCSTAProcesser();

    //上报事件
private:

    //拨号，预拨号触发事件
    void SendServiceInitiatedEvent(const CCallInfo * pCallInfo);

    //发起通话事件
    void SendOriginatedEvent(const CCallInfo * pCallInfo);

    //来电，呼出事件
    void SendDeliveredEvent(const CCallInfo * pCallInfo, DeliveredEventType eType);

    //通话建立事件
    void SendEstablishedEvent(const CCallInfo * pCallInfo);

    //断开连接事件
    void SendConnectionClearedEvent(const CCallInfo * pCallInfo);

    //hold事件
    void SendHeldEvent(const CCallInfo * pCallInfo);

    //解hold事件
    void SendRetrievedEvent(const CCallInfo * pCallInfo);

    //忙转，咨询转事件
    void SendSigleTransferedEvent(const CCallInfo * pCallInfo, bool isAttend = false);

    //void SendConferenceEvent(const CCallInfo* pCallInfo);



    //通话中dtmf事件
    void SendDtmfEvent(const CCallInfo * pCallInfo, int iKey);

    void SendDivertedEvent(const CCallInfo * pCallInfo);

    void SendCallFailedEvent(const CCallInfo * pCallInfo);

    MonitorInfo & AddHeadInfoNode(xml_node & nodeRootNode, const char * pNodeName,
                                  const CCallInfo * pCallInfo, bool isNeedConf = false, bool isTest = false);

    void AddDevice(xml_node & nodeRootNode, const yl::string & strName, const yl::string & strValue);

    void AddLocalConnectInfo(xml_node & nodeRootNode, const yl::string & strValue,
                             const yl::string & strCauseValue);

//建立连接
private:

    bool ProcessReqSystemStatus();

    bool ProcessGetSwitchingFunDevices();

    bool ProcessSystemRegister(xml_node & node);

    bool ProcessMonitorStart(xml_node & node);

    bool ProcessMonitorStop(xml_node & node);

    bool ProcessGetForwarding(xml_node & node);

    bool ProcessGetDoNotDisturb(xml_node & node);

    bool ProcessSnapshotDevice();

    bool ProcessGetMessageWaitingIndicator(xml_node & node);

//控制命令
private:

    bool ProcessCmd(xml_node & node);

    //呼出通话处理
    bool ProcessMakeCall(xml_node & node);

    //接起通话接起
    bool ProcessAnswerCall(xml_node & node);

    //Hold通话处理
    bool ProcessHoldCall(xml_node & node);

    //unHold通话处理
    bool ProcessRetrieveCall(xml_node & node);

    //取消呼出，结束通话处理
    bool ProcessClearConnection(xml_node & node);

    //盲转
    bool ProcessBindTransfer(xml_node & node);

    //咨询转
    bool ProcessAttendTransfer(xml_node & node);

    //Forward Call
    bool ProcessDeflectCall(xml_node & node);

    //切换焦点通话
    bool ProcessAlternateCall(xml_node & node);

    //挂断当前通话，取回被hold路通话
    bool ProcessReconnectCall(xml_node & node);

    //hold当前通话，新起一路通话
    bool ProcessConsultationCall(xml_node & node);

    //通话中发送dtmf
    bool ProcessSendDtmf(xml_node & node);

    //发起会议
    bool ProcessConferenceCall(xml_node & node);

    //设置fwd
    bool ProcessSetForward(xml_node & node);

    //设置DND
    bool ProcessSetDnD(xml_node & node);

    void SendErrorResponse();

private:

    void AddResponseInfo(xml_node & node, const yl::string & strChildNodeName, const char * pExt);

    //回复
    void SetResponseXmlDoc(const char * pChar, const char * pExt = NULL);

    //会议回复
    void SetConfereceReponse(const CCallInfo * pActivCallInfo, const CCallInfo * pHeldICallInfo,
                             const char * pExt);

    //异常回复
    void SetErrorReponseXmlDoc(const yl::string & strErrorInfo);

    //通过sip url获取账号id
    int GetAcountIdBySipURI(const yl::string & strSipURI);

    //获取一个新回复文件
    const yl::string & GetNewResFile();

    //向sip回复消息
    bool SendSipMsg(bool bIsEvent = false, bool bIsEndCsta = false);

    yl::string GetCallId(xml_node & node, const yl::string & strChildName);

    yl::string GetAccountInfo(xml_node & node, const yl::string & strChildName);

    void AddConnectionListItem(xml_node & node, const yl::string & strConfId,
                               const CCallInfo * pCallInfo, const yl::string & strDeviceUrl);

    bool GetCallId(yl::string & strCallId);

private:
    int m_nLineId;


    int m_nTid;

    yl::string m_strResFile;

    bool m_bIsUME;

    MonitorMap m_monitorMap;

    int m_nCSTACallId;

    int m_nMyCallId;

    yl::string m_strMyCallId;
#if !IF_FEATURE_EVENT_MODULE
    bool m_bCSTAEnable;
#endif
};

#define g_objCSTAManager (CCSTAProcesser::GetInstace())

#endif
