#ifndef __VQREPORTELEMENT_H__
#define __VQREPORTELEMENT_H__

#include "talk/vqrtcp/include/vqrtcp_data.h"

#ifdef IF_FEATURE_RTCPXR

// 通话与Element一一对应
class CVQReportElement
{
public:
    CVQReportElement();
    virtual ~CVQReportElement();

public:
    // 获取数据供UI显示
    bool GetReportDataToUI(SVQReportData& sData);

    // 转发数据包给sip
    bool ForwardReportToSip(int iActiveType = IPVP_FL_VQR_NONE);

public:
    // 控制定时器发送Interval Report
    void ControlSendTimer(bool bStart = true);

    // 开启超时定时器
    void StartTimerOut();

    // 定时器处理
    bool OnTimerOut(UINT uTimerID, bool& bSaveToFile);
    bool OnTimerSend(UINT uTimerID);

private:
    // 转换数据
    bool TransformData(const ipvp_vq_report_t* pData, SVQReportData& sData);

    // 转换数据
    bool TransformData(const ipvp_vq_report_t* pLocalData, const ipvp_vq_report_t* pRemoteData,
                       const sip_call_proto_info2_t* pInfo, yl::string& strToSip, int iActiveType = IPVP_FL_VQR_NONE);

    // 根据数据包类型获取数据
    const ipvp_vq_report_t* GetReportDataByType(int iType, bool bRemote = false);

public:
    int         m_iCallID; //通话ID

    // 发向SIP的相关数据
    bool        m_bCaller; // 是否通话的建立者
    int         m_nLineID;  // 账号ID
    int         m_iTimerSend; //定时器

    ipvp_vq_report_t m_IntervalData; // 间隔性数据包
    ipvp_vq_report_t m_SessionData; // 总数据包
    ipvp_vq_report_t m_AlertData; // 警告性数据包

    // t49 sp2 支持上报对端信息
    ipvp_vq_report_t m_RemoteIntervalData; // 间隔性数据包
    ipvp_vq_report_t m_RemoteSessionData; // 总数据包
    ipvp_vq_report_t m_RemoteAlertData; // 警告性数据包

    // 用于显示的相关数据
    int         m_iActiveType; // 当前活跃的数据包类型
    yl::string  m_strRemoteNumber; // 远程号码

    int         m_iTimeOut; // 超时保护
};

#endif

#endif //__VQREPORTELEMENT_H__
