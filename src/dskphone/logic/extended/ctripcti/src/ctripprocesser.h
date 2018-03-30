#ifndef __CTRIP_PROCESSER_H__
#define __CTRIP_PROCESSER_H__
#if IF_FEATURE_CTRIP_CTI
#include "xmlparser.hpp"
#include "ylstring.h"
#include "ETLLib.hpp"
#include "extended/event/src/eventbase.h"

class CCallInfo;

enum MessageType
{
    MT_HUAWEI, //华为
    MT_FREESWITCH, //FreeSwitch
};

class CCtripProcesser : public CEventBase
{
public:

    static CCtripProcesser & GetInstace();
    // V60 联动功能
    int ProcessMessageOfeSpace(msgObject & objMessage);
    // V73 定制Notify
    int ProcessMessageOfCallControler(msgObject & objMessage);

    bool IsControlByCtrip(int nLineId);

    void SendResponse(msgObject & objMessage, MessageType eType, int iResult);

    // 发送事件
    virtual bool SendEvent(EventDataBase * pData);

    bool GetTalkEventInfo(EventDataBase * pData, int & nLineId, int & nCallID, yl::string & strStauts,
                          yl::string & strCalltag);

    void SendRequest2eSpace(int & nLineId, int & nCallID, yl::string & strStauts,
                            yl::string & strCalltag, EventType eType);

private:
    CCtripProcesser();
    virtual ~CCtripProcesser();

//建立连接
private:
    //开启关闭账号控制
    bool ProcessControl(bool bEnable, int nLineId);

//控制命令
private:
    //解析命令
    int ProcessCmd(int nLineId, yl::string strCmd, yl::string strCallID);

    //处理CallControl命令
    int ProcessCallControlCmd(int nLineId, int iCallId, yl::string & strInfo);

    //呼出
    int ProcessMakeCall(int nLineId, yl::string strNum, yl::string strXUUI = "");

    //接起通话
    int ProcessAnswerCall(int nLineId, int nCallID, yl::string strNum);

    //挂断通话
    int ProcessHangUpCall(int nLineId, int nCallID, yl::string strNum);

    //Hold通话处理
    int ProcessHoldCall(int nLineId, int nCallID, yl::string strNum);

    //unHold通话处理
    int ProcessRetrieveCall(int nLineId, int nCallID, yl::string strNum);

    //转换FREESWITCH返回码TOHUAWEI返回码
    int TransResponseCodeToHUAWEI(const int iResCode);

private:
    YLList<int> m_listLineId;  //可控制账号列表
};

#define g_objCtripManager (CCtripProcesser::GetInstace())

#endif

#endif
