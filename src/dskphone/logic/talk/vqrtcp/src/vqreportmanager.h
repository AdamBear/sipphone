#ifndef __VQREPORT_MANAGER_H__
#define __VQREPORT_MANAGER_H__

#include <ETLLib.hpp>
#include <ylhashmap.h>
#include <ylstring.h>
#include "singletonclassdefine.h"
#include "talk/vqrtcp/include/vqrtcp_data.h"

#ifdef IF_FEATURE_RTCPXR

class CVQReportElement;

typedef YLHashMap<int, CVQReportElement> VQ_REPORTDATA;
typedef VQ_REPORTDATA::iterator ITER_VQREPORTDATA;

class CVQReportManager
{
    DECLARE_SINGLETON_CLASS(CVQReportManager)

public:
    // 初始化
    void Init();

    // 处理VPM发上来的Voice Quality
    bool ProcessVQReportMsg(msgObject& msg);

    // 获取Session Report
    void GetSessionReportData(SVQReportData& sData);

    // 获取Interval/Alert Report
    void GetCurrentReportData(int iCallId, SVQReportData& sData);

    // 焦点话路变更处理
    void OnFocusSessionChange(int iSessionId);

    // 通话开启的处理
    void ProcessStartTalk(int iCallID, int nLineId, bool bCaller, const yl::string& strRemoteNumber);

    // 通话结束后的处理
    void ProcessEndCall(int iCallId);

    // 通话Held的处理
    void ProcessHeldTalk(int iSessionID);

    // 会议中的话路是否可切换
    bool IsRTPStatusSwitch();

    // 切换会议中的话路显示
    bool SwitchRTPStatus();

    // 活跃的CallId
    inline int GetActioveCallId()
    {
        return m_iActiveCallId;
    }

    // 处理定时器消息
    bool OnTimer(UINT uTimerID);

    // 通知UI去刷新界面
    void NotifyToUI(int iCallId, int iAction);

    // 保存数据到xml文件，供web使用
    void SaveSessionReportDataToWeb();

private:
    // 保存SessionReport数据到文件
    void SaveSessionReportData(const SVQReportData& sData);

    // 该话路RTP是否可用
    bool IsRTPAvaliable(int iSessionID);

    // 该话路是否禁止RTP
    bool IsRTPForbidden(int iSessionID);

    // 获取会议中第一路可用的RTP的CallID
    int GetConfFirstSupportRTP(int iSessionID);

private:
    // 数据列表
    VQ_REPORTDATA m_mapVQReportData;

    // 活跃的CallId
    int m_iActiveCallId;

    // 最近收到vpm包的通话id
    int m_iRecentCallId;
};

// 获取全局唯一的Session管理对象
#define _VQReportManager GET_SINGLETON_INSTANCE(CVQReportManager)

#endif

#endif //__VQREPORT_MANAGER_H__
