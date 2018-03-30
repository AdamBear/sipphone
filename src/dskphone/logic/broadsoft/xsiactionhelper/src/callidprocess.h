#ifndef __CALLID_PROCESS_H__
#define __CALLID_PROCESS_H__

#include <ylvector.h>
#include <ylhashmap.h>
#include <pugixml.hpp>
#include "broadsoft/xsiactionhelper/include/xsibaseprocess.h"

typedef YLVector<yl::string>    ListXsiCallsID;
typedef YLHashMap<int, yl::string>  MapLocalIDXsiID;

// Bsft 服务器CallID关联的通话的详细信息
struct CallIDDetails
{
    int         iApearanceId;
    bool        bExecutiveCall;
    bool        bExecutiveRecall;

    yl::string  strExecutive;
    yl::string  strRemoteCallID;

    CallIDDetails() : iApearanceId(0), bExecutiveRecall(false), bExecutiveCall(false) {}
};

typedef YLHashMap<int, CallIDDetails>   MapCallIdDetails;

using namespace pugi;

class CCallIdProcesser : public XsiBaseProcesser
{
public:

    static CCallIdProcesser & GetInstance();

    yl::string GetXsiCallId(int nLocalCallId);

#ifdef IF_FEATURE_BROADSOFT_CALLRECORDING
    int GetConfCallId(ListXsiCallsID & listConfCallID)
    {
        listConfCallID = m_listConfCallID;
        return listConfCallID.size();
    }

    bool IsNetworkConfCallId(int nCallId);
#endif

    bool IsExecutiveCall(int iLocalSessionId);

    bool IsExecutiveCallPushRecall(int iLocalSessionId);

    //是否允许获取callid
    bool IsGetCallID(int iLineId);
private:
    CCallIdProcesser();

    virtual ~CCallIdProcesser();

private:
    virtual bool ProcessByType(XsiAction* pAction);

    // 解析Remote CallId
    bool PraseCallId(const xml_node& nodeRoot, int iLocalSessionId);
    // 解析Remote CallId Details
    bool PraseCallIdDetails(const xml_node & nodeRoot, int iLocalSessionId);
    // 继续获取Details
    bool ProcessGetDetails(int iLocalSessionId);

private:

    ListXsiCallsID m_listCallsID;

    MapLocalIDXsiID m_mapLocalIDXsiID;

#ifdef IF_FEATURE_BROADSOFT_EXECUTIVE_ASSISTANT
    MapCallIdDetails m_mapLocalIDDetails;
#endif
#ifdef IF_FEATURE_BROADSOFT_CALLRECORDING
    ListXsiCallsID m_listConfCallID;
    int m_ConfId;
#endif

public:
    //[1] 以下事件流程对应解析函数封装
    //话路建立 回铃 响铃 事件情况下
    void OnDealGetCallId(int iSessionId, int iAccountId, bool isEnterTalk);

    //话路结束后
    void OnDealSessionDestory(int iSessionId);

#ifdef IF_FEATURE_BROADSOFT_CALLRECORDING
    //组件网络会议后
    void OnDealNetWorkConfEnter(int iSessionId);
    //网络会议添加成员
    void OnDealNetWorkConfAddUser(int iSessionId, int iAddSessionId);
#endif
    //[1] 以上事件流程对应解析函数封装
};

#define g_objCallIdProcesser (CCallIdProcesser::GetInstance())

#endif

