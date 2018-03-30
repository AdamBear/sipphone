#include "vqrtcp_inc.h"

#define TIME_FORMAT_TO_SHOW "%d-%d-%d %d:%02d:%02d"
#define TIME_FORMAT_TO_SIP "%d-%02d-%02dT%02d:%02d:%02dZ"

#ifdef IF_FEATURE_RTCPXR
CVQReportElement::CVQReportElement()
    : m_nLineID(-1)
    , m_iCallID(-1)
    , m_iActiveType(-1)
    , m_iTimerSend(-1)
    , m_iTimeOut(3000)
    , m_bCaller(false)
{
    memset(&m_IntervalData, 0, sizeof(ipvp_vq_report_t));
    memset(&m_SessionData, 0, sizeof(ipvp_vq_report_t));
    memset(&m_AlertData, 0, sizeof(ipvp_vq_report_t));
    memset(&m_RemoteIntervalData, 0, sizeof(ipvp_vq_report_t));
    memset(&m_RemoteSessionData, 0, sizeof(ipvp_vq_report_t));
    memset(&m_RemoteAlertData, 0, sizeof(ipvp_vq_report_t));
}

CVQReportElement::~CVQReportElement()
{
    timerKillThreadTimer(TIMER_ID(m_iTimerSend));
    timerKillThreadTimer(TIMER_ID(m_iTimeOut));
}

// 获取数据供UI显示
bool CVQReportElement::GetReportDataToUI(SVQReportData& sData)
{
    return TransformData(GetReportDataByType(m_iActiveType), sData);
}

// 转发数据包给sip
bool CVQReportElement::ForwardReportToSip(int iActiveType /*= IPVP_FL_VQR_NONE*/)
{
#ifdef IF_FEATURE_H323
    if (ACCOUNT_TYPE_H323 == acc_GetAccountType(m_nLineID))
    {
        TALK_INFO("H323 not support report");
        return false;
    }
#endif //IF_FEATURE_H323

    // 服务器地址未填写
    if (configParser_GetCfgItemStringValue(kszAccountRTCPCollectorAddress, m_nLineID).empty())
    {
        TALK_INFO("Failed send report to sip, CollectorServer is null");
        return false;
    }

    // 有传参时则使用传入的iActiveType类型
    if (iActiveType == IPVP_FL_VQR_NONE)
    {
        iActiveType = m_iActiveType;
    }

    const ipvp_vq_report_t* pLocalData = GetReportDataByType(iActiveType);
    if (NULL == pLocalData)
    {
        //TALK_INFO("Failed send report to sip, Data is null");
        return false;
    }

    const ipvp_vq_report_t* pRemoteData = GetReportDataByType(iActiveType, true);
    if (NULL == pRemoteData)
    {
        //TALK_INFO("Failed send report to sip, Data is null");
        return false;
    }

    // 获取信息
    sip_call_proto_info2_t info;
    modVoip_GetCallInfo(m_nLineID, m_iCallID, &info);

    yl::string strToSip = "";
    if (TransformData(pLocalData, pRemoteData, &info, strToSip, iActiveType) && !strToSip.empty())
    {
        modVoip_SendReportVQSession(m_nLineID, m_iCallID, strToSip.c_str());

        TALK_INFO("Success send report to sip, EVQReportType[%0x]", iActiveType);
        return true;
    }

    return false;
}

// 开启定时器发送Interval Report
void CVQReportElement::ControlSendTimer(bool bStart/* = true*/)
{
    if (bStart)
    {
        timerSetThreadTimer(TIMER_ID(m_iTimerSend), m_iTimerSend);
    }
    else
    {
        timerKillThreadTimer(TIMER_ID(m_iTimerSend));
    }
}

// 开启超时定时器
void CVQReportElement::StartTimerOut()
{
    timerSetThreadTimer(TIMER_ID(m_iTimeOut), m_iTimeOut);
}

// 定时器处理
bool CVQReportElement::OnTimerOut(UINT uTimerID, bool& bSaveToFile)
{
    bSaveToFile = false;

    if (uTimerID != TIMER_ID(m_iTimeOut))
    {
        return false;
    }

    TALK_WARN("RTCP-XR Timer out[%#x]", m_iActiveType);

    if (IPVP_FL_VQR_SESSN == m_iActiveType)
    {
        ForwardReportToSip();
        bSaveToFile = true;
    }
    else if (IPVP_FL_VQR_PRIV == m_iActiveType
             || IPVP_FL_VQR_INTVL == m_iActiveType)
    {
        // 通话结束，ipvp不一定会上报session类型：非session类型就保存数据即可
        bSaveToFile = true;
    }

    timerKillThreadTimer(TIMER_ID(m_iTimeOut));

    return true;
}

bool CVQReportElement::OnTimerSend(UINT uTimerID)
{
    if (uTimerID != TIMER_ID(m_iTimerSend))
    {
        return false;
    }

    if (IPVP_FL_VQR_INTVL == m_iActiveType)
    {
        // http://10.2.1.199/Bug.php?BugID=96434
        int iSessionID = talklogic_GetSessionIdByCallID(m_iCallID, false);
        if (talkLogic_IsSessionInTalking(iSessionID, true))
        {
            ForwardReportToSip();
        }
    }

    return true;
}

// 辅助函数 时间转换
yl::string Timet2String(time_t tTime, const yl::string& strFormat = TIME_FORMAT_TO_SHOW)
{
    if (0 == strFormat.compare(TIME_FORMAT_TO_SHOW))
    {
        time_t tNow = time(NULL);
        time_t tLocal = netGetLocalTime();
        double ts = difftime(tLocal, tNow);

        tTime = time_t(tTime + ts);
    }

    tm* tmTemp = localtime(&tTime);

    return commonAPI_FormatString(strFormat.c_str(), tmTemp->tm_year + 1900,
                                  tmTemp->tm_mon + 1, tmTemp->tm_mday, tmTemp->tm_hour, tmTemp->tm_min, tmTemp->tm_sec);
}

// 转换数据
bool CVQReportElement::TransformData(const ipvp_vq_report_t* pData, SVQReportData& sData)
{
    if (NULL == pData)
    {
        return false;
    }

    // 重置原来数据
    sData.Reset();

    // 本方号码
    sData.strLocalNumber = acc_GetCustomShowText(m_nLineID);
    sData.strLocalNumber = sData.strLocalNumber.empty() ? acc_GetUsername(
                               m_nLineID) : sData.strLocalNumber;

    // 对方号码
    sData.strRemoteNumber = m_strRemoteNumber;

    // 通话开启时间
    sData.strStartTime = Timet2String(pData->start_time);

    // 当前时间
    sData.strCurrentTime = Timet2String(pData->current_time);

    sData.strJitter = commonAPI_FormatString("%d", pData->statics.jitter);
    sData.strPktLost = commonAPI_FormatString("%d", pData->statics.pkt_lost);
    sData.strSymmOneWayDelay = commonAPI_FormatString("%d", pData->metrics.delay_sowd);
    sData.strRoundTripDelay = commonAPI_FormatString("%d", pData->metrics.delay_rtd);

    // it is expressed as an integer in the range 10 to 50,corresponding to MOS x 10, and a value of 127 stands for unavailable
    if (pData->metrics.qe_moslq != 127)
    {
        sData.strMOSLQ = commonAPI_FormatString("%f", pData->metrics.qe_moslq * 0.1);
    }
    else
    {
        sData.strMOSLQ = "NA";
    }

    // it is expressed as an integer in the range 10 to 50,corresponding to MOS x 10, and a value of 127 stands for unavailable
    if (pData->metrics.qe_moscq != 127)
    {
        sData.strMOSCQ = commonAPI_FormatString("%f", pData->metrics.qe_moscq * 0.1);
    }
    else
    {
        sData.strMOSCQ = "NA";
    }

    sData.strNetworkPacketLossRate = commonAPI_FormatString("%f", pData->metrics.pl_nlr / 256.0);
    sData.strEndSystemDelay = commonAPI_FormatString("%d", pData->metrics.delay_esd);
    sData.strInterarrivalJitter = commonAPI_FormatString("%d", pData->metrics.delay_iaj);
    sData.strLocalPort = commonAPI_FormatString("%d", pData->local_port);
    sData.strRemotePort = commonAPI_FormatString("%d", pData->remote_port);
    sData.strJitterBufferMax = commonAPI_FormatString("%d", pData->metrics.jb_jbm);
    sData.strRemoteIP = (char*)(pData->remote_ip_addr);
    sData.strCodecName = (char*)(pData->codec_name);

#if 0
    // 彩屏性能限制，正式商用业版本不打印log
    printf("~~~~~~~~~~~~~~~~~~Rtcp xr~~~~~~~~~~~~~~~~~\n");
    printf("sData.strStartTime [%s]\n", sData.strStartTime.c_str());
    printf("sData.strJitter [%s]\n", sData.strJitter.c_str());
    printf("sData.strMOSLQ [%s]\n", sData.strMOSLQ.c_str());
    printf("sData.strNetworkPacketLossRate [%s]\n", sData.strNetworkPacketLossRate.c_str());
    printf("sData.strInterarrivalJitter [%s]\n", sData.strInterarrivalJitter.c_str());
    printf("sData.strRemoteIP [%s]\n", sData.strRemoteIP.c_str());
    printf("sData.strCodecName [%s]\n", sData.strCodecName.c_str());
#endif
    return true;
}

static void TransformVQData(const ipvp_vq_report_t* pData, yl::string& strToSip)
{
    if (NULL == pData)
    {
        return;
    }

    // 转换Timestamps
    strToSip += commonAPI_FormatString("\r\n%sMetrics:\r\nTimestamps:START=%s STOP=%s",
                                       pData->direct == IPVP_VQR_DIR_LOCAL ? "Local" : "Remote",
                                       Timet2String(pData->start_time, TIME_FORMAT_TO_SIP).c_str(),
                                       Timet2String(pData->current_time, TIME_FORMAT_TO_SIP).c_str());

    // 转换SessionDesc
    strToSip +=
        commonAPI_FormatString("\r\nSessionDesc:PT=%d SR=%d FD=%d FO=%d FPP=%d PPS=%d PLC=%d SSUP=%s",
                               pData->metrics.sd_pt, pData->metrics.sd_sr, pData->metrics.sd_fd,
                               pData->metrics.sd_fo
                               , pData->metrics.sd_fpp, pData->metrics.sd_pps, pData->metrics.sd_plc,
                               (0 == pData->metrics.sd_ssup ? "off" : "on"));

    // 转换JitterBuffer
    strToSip += commonAPI_FormatString("\r\nJitterBuffer:JBA=%d JBR=%d JBN=%d JBM=%d JBX=%d",
                                       pData->metrics.jb_jba, pData->metrics.jb_jbr, pData->metrics.jb_jbn,
                                       pData->metrics.jb_jbm, pData->metrics.jb_jbx);

    // 转换PacketLoss
    strToSip += commonAPI_FormatString("\r\nPacketLoss:NLR=%.1f JDR=%.1f",
                                       pData->metrics.pl_nlr / 256.0 * 100, pData->metrics.pl_jdr / 256.0 * 100);

    // 转换BurstGapLoss
    strToSip += commonAPI_FormatString("\r\nBurstGapLoss:BLD=%.1f BD=%d GLD=%.1f GD=%d GMIN=%d",
                                       pData->metrics.bgl_bld / 256.0 * 100, pData->metrics.bgl_bd,
                                       pData->metrics.bgl_gld / 256.0 * 100, pData->metrics.bgl_gd, pData->metrics.bgl_gmin);

    // 转换Delay
    strToSip += commonAPI_FormatString("\r\nDelay:RTD=%d ESD=%d SOWD=%d IAJ=%d MAJ=%d",
                                       pData->metrics.delay_rtd, pData->metrics.delay_esd, pData->metrics.delay_sowd,
                                       pData->metrics.delay_iaj, pData->metrics.delay_maj);

    // 转换Signal
    int sign_sl = (pData->metrics.sign_sl & 0x80) == 0 ? pData->metrics.sign_sl :
                  (pData->metrics.sign_sl | 0xffffff00);
    int sign_nl = (pData->metrics.sign_nl & 0x80) == 0 ? pData->metrics.sign_nl :
                  (pData->metrics.sign_nl | 0xffffff00);
    int sign_rerl = (pData->metrics.sign_rerl & 0x80) == 0 ? pData->metrics.sign_rerl :
                    (pData->metrics.sign_rerl | 0xffffff00);

    strToSip += commonAPI_FormatString("\r\nSignal:SL=%d NL=%d RERL=%d", sign_sl, sign_nl, sign_rerl);

    // 转换QualityEs
    if (pData->metrics.qe_rcq == 127 || pData->metrics.qe_extri == 127)
    {
        if (pData->metrics.qe_moslq == 127 || pData->metrics.qe_moscq == 127)
        {
            strToSip += commonAPI_FormatString("\r\nQualityEst:EXTRI=%d", pData->metrics.qe_extri);
        }
        else
        {
            strToSip += commonAPI_FormatString("\r\nQualityEst:EXTRI=%d MOSLQ=%.1f MOSCQ=%.1f",
                                               pData->metrics.qe_extri, pData->metrics.qe_moslq / 10.0,
                                               pData->metrics.qe_moscq / 10.0);
        }
    }
    else
    {
        if (pData->metrics.qe_moslq == 127 || pData->metrics.qe_moscq == 127)
        {
            strToSip += commonAPI_FormatString("\r\nQualityEst:RCQ=%d EXTRI=%d", pData->metrics.qe_rcq, pData->metrics.qe_extri);
        }
        else
        {
            strToSip += commonAPI_FormatString("\r\nQualityEst:RCQ=%d EXTRI=%d MOSLQ=%.1f MOSCQ=%.1f",
                                               pData->metrics.qe_rcq, pData->metrics.qe_extri, pData->metrics.qe_moslq / 10.0,
                                               pData->metrics.qe_moscq / 10.0);
        }
    }

    return;
}

// 转换数据 发送给sip
bool CVQReportElement::TransformData(const ipvp_vq_report_t* pLocalData,
                                     const ipvp_vq_report_t* pRemoteData,
                                     const sip_call_proto_info2_t* pInfo, yl::string& strToSip, int iActiveType/* = IPVP_FL_VQR_NONE*/)
{
    if (NULL == pInfo
            || NULL == pLocalData)
    {
        return false;
    }

    if (iActiveType == IPVP_FL_VQR_NONE)
    {
        iActiveType = m_iActiveType;
    }

    // 转换数据包类型
    if (IPVP_FL_VQR_SESSN == iActiveType)
    {
        strToSip = "VQSessionReport";
    }
    else if (IPVP_FL_VQR_INTVL == iActiveType
             || IPVP_FL_VQR_PRIV == iActiveType)
    {
        strToSip = "VQIntervalReport";
    }
    else if (IPVP_FL_VQR_ALERT == iActiveType)
    {
        yl::string strSeverity = "Critical";
        strSeverity = (0 == pLocalData->severity) ? "Clear" : strSeverity;
        strSeverity = (1 == pLocalData->severity) ? "Warning" : strSeverity;

        strToSip = commonAPI_FormatString("VQAlertReport: Type=%s Severity=%s Dir=%s",
                                          pLocalData->alert_section_name, strSeverity.c_str(),
                                          (IPVP_VQR_DIR_LOCAL == pLocalData->direct ? "local" : "remote"));
    }

    // 转换CallId
    strToSip += commonAPI_FormatString("\r\nCallID:%s", pInfo->call_id);

    // 转换From Id... 若出现值为空, 抓包不到, 需要更新SIP版本号
    if (0 != strlen(pInfo->from))
    {
        strToSip += commonAPI_FormatString("\r\nLocalID:%s", pInfo->from);
    }

    // 转换To Id... 若出现值为空, 抓包不到, 需要更新SIP版本号
    if (0 != strlen(pInfo->to))
    {
        strToSip += commonAPI_FormatString("\r\nRemoteID:%s", pInfo->to);
    }

    // sip 初步确定OrigID:为from
    if (0 != strlen(pInfo->from))
    {
        strToSip += commonAPI_FormatString("\r\nOrigID:%s", pInfo->from);
    }

    // sip 初步确定LocalGroup:对应userAgent
    char szBuf[1024] = { 0 };
    sys_replace_wildcards((char*)configParser_GetConfigString(kszSipUserAgent).c_str(), 0, szBuf,
                          sizeof(szBuf));

    if (strlen(szBuf) > 0)
    {
        strToSip += commonAPI_FormatString("\r\nLocalGroup:%s", szBuf);
    }

    memset(szBuf, 0, sizeof(szBuf));
    // sip初步确定 RemoteGroup 对应对方userAgent
    sip_call_get_remote_user_agent(mkGetHandle(), m_nLineID, m_iCallID, szBuf, sizeof(szBuf));
    if (strlen(szBuf) > 0)
    {
        strToSip += commonAPI_FormatString("\r\nRemoteGroup:%s", szBuf);
    }

    // 转换LocalAddr
    yl::string strTmp;
    netGetWanIP(strTmp);
    strToSip += commonAPI_FormatString("\r\nLocalAddr:IP=%s PORT=%d SSRC=0x%08x",
                                       strTmp.c_str(), pLocalData->local_port, pLocalData->local_ssrc);

    // Mac地址可选，暂不处理

    // 转换RemoteAddr
    strToSip += commonAPI_FormatString("\r\nRemoteAddr:IP=%s PORT=%d SSRC=0x%08x",
                                       pLocalData->remote_ip_addr, pLocalData->remote_port, pLocalData->remote_ssrc);

    TransformVQData(pLocalData, strToSip);

    TransformVQData(pRemoteData, strToSip);

    // 转换DialogID
    strToSip += commonAPI_FormatString("\r\nDialogID:%s;to-tag=%s;from-tag=%s\r\n",
                                       pInfo->call_id, (m_bCaller ? pInfo->remote_tag : pInfo->local_tag),
                                       (m_bCaller ? pInfo->local_tag : pInfo->remote_tag));

    return true;
}

// 获取活跃的数据
const ipvp_vq_report_t* CVQReportElement::GetReportDataByType(int iType, bool bRemote/* = false*/)
{
    //vpm 只有 priv时候才会报远端数据
    switch (iType)
    {
    case IPVP_FL_VQR_SESSN:
        return bRemote ? &m_RemoteIntervalData : &m_SessionData;
    case  IPVP_FL_VQR_PRIV:
    case IPVP_FL_VQR_INTVL:
        return bRemote ? &m_RemoteIntervalData : &m_IntervalData;
    case IPVP_FL_VQR_ALERT:
        return bRemote ? &m_RemoteIntervalData : &m_AlertData;
    default:
        break;
    }

    return NULL;
}
#endif // IF_FEATURE_RTCPXR

