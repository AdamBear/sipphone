#include "vqrtcp_inc.h"

IMPLEMENT_GETINSTANCE(CVQReportManager)

#ifdef IF_FEATURE_RTCPXR

#if IF_DEBUG_LOG
void PrintData(ipvp_vq_report_t * pData)
{
    if (NULL == pData)
    {
        return;
    }

    TALK_DEBUG("qe_moslq [%d] qe_moscq [%d]", pData->metrics.qe_moslq, pData->metrics.qe_moscq);
}
#else
#define PrintData(...)
#endif

CVQReportManager::CVQReportManager(void)
    : m_iActiveCallId(-1)
{

}

CVQReportManager::~CVQReportManager(void)
{

}

// 初始化
void CVQReportManager::Init()
{
}

// 处理VPM发上来的Voice Quality
bool CVQReportManager::ProcessVQReportMsg(msgObject & msg)
{
    ipvp_vq_report_t * pData = (ipvp_vq_report_t *)msg.GetExtraData();
    if (NULL == pData)
    {
        return false;
    }

    ITER_VQREPORTDATA itr = m_mapVQReportData.find(msg.wParam);
    if (itr == m_mapVQReportData.end())
    {
        return false;
    }

    AUTO_SCOPED_TIMER;
    bool bHandle = true;

    switch (msg.lParam)
    {
    case IPVP_FL_VQR_SESSN:
        {
            TALK_INFO("RTCP-XR ProcessSessionReport Id[%d][%d]", msg.wParam, pData->direct);
            if (pData->direct == IPVP_VQR_DIR_REMOTE)
            {
                // 远端数据全部不上报
                itr->second.m_RemoteSessionData = *pData;
                break;
            }

            m_iRecentCallId = msg.wParam;

            itr->second.m_iActiveType = msg.lParam;
            // 数据赋值
            itr->second.m_SessionData = *pData;

            PrintData(pData);
            // 通话结束直接处理
            int iSessionID = talklogic_GetSessionIdByCallID(msg.wParam);
            TALK_DBG("IPVP_FL_VQR_SESSN session[%d] callid[%d]", iSessionID, msg.wParam);
            if (iSessionID < 0)
            {
                // 保存到文件
                SVQReportData sData;
                itr->second.GetReportDataToUI(sData);
                SaveSessionReportData(sData);

                // 转发给sip
                if (configParser_GetConfigInt(kszSessionReportSwitch) == 1)
                {
                    itr->second.ForwardReportToSip();
                }

                // 删除数据
                m_mapVQReportData.erase(itr);
            }
            else
            {
                // 转发给sip
                if (configParser_GetConfigInt(kszSessionReportSwitch) == 1)
                {
                    itr->second.ForwardReportToSip();
                }
            }
        }
        break;
    case IPVP_FL_VQR_INTVL:
        {
            TALK_INFO("RTCP-XR intvl report Id[%d][%d]", msg.wParam, pData->direct);

            // sp2 vpm按rfc6035协议修改：VPM周期上报，Alert触发后就会上报IPVP_FL_VQR_INTVL，GUI需要上报给服务器
            // 通话存在才处理
            int iSessionID = talklogic_GetSessionIdByCallID(msg.wParam);
            TALK_DBG("IPVP_FL_VQR_INTVL session[%d] callid[%d]", iSessionID, msg.wParam);
            if (iSessionID >= 0)
            {
                // 数据赋值
                if (pData->direct == IPVP_VQR_DIR_REMOTE)
                {
                    // 记录数据且刷新界面
                    itr->second.m_RemoteIntervalData = *pData;
                }
                else
                {
                    // 上报IPVP_FL_VQR_INTVL后：就不在上报本端的IPVP_FL_VQR_PRIV数据
                    itr->second.m_iActiveType = msg.lParam;
                    itr->second.m_IntervalData = *pData;
                }

                PrintData(pData);

                m_iRecentCallId = msg.wParam;
                // http://10.2.1.199/Bug.php?BugID=93314

                // 通知UI刷新
                if (talkLogic_IsSessionInTalking(iSessionID, true))
                {
                    NotifyToUI(msg.wParam, 0);
                }
            }
        }
        break;
    case IPVP_FL_VQR_ALERT:
        {
            TALK_INFO("RTCP-XR Alert Report Id[%d][%d]", msg.wParam, pData->direct);
            // 数据赋值
            if (pData->direct == IPVP_VQR_DIR_REMOTE)
            {
                // 远端数据全部不上报
                itr->second.m_RemoteAlertData = *pData;
                break;
            }

            // 通话存在才处理
            int iSessionID = talklogic_GetSessionIdByCallID(msg.wParam);
            TALK_DBG("IPVP_FL_VQR_ALERT session[%d] callid[%d]", iSessionID, msg.wParam);
            if (iSessionID >= 0)
            {
                // alert数据只保存数据及上报，不记录类型
                int iAcitveType = msg.lParam;
                itr->second.m_AlertData = *pData;

                m_iRecentCallId = msg.wParam;
                // http://10.2.1.199/Bug.php?BugID=93314

                PrintData(pData);
                // Alert数据包优先级最高 直接转发给sip
                if (talkLogic_IsSessionInTalking(iSessionID, true))
                {
                    itr->second.ForwardReportToSip(iAcitveType);
                }
            }
        }
        break;
    case IPVP_FL_VQR_PRIV:
        {
            TALK_INFO("RTCP-XR private report Id[%d][%d]", msg.wParam, pData->direct);

            // sp2 vpm按rfc6035协议修改：VPM周期上报，通话质量正常就会上报IPVP_FL_VQR_PRIV，用于UI刷新，并且不上报服务器
            // 通话存在才处理
            int iSessionID = talklogic_GetSessionIdByCallID(msg.wParam);
            TALK_DBG("IPVP_FL_VQR_PRIV session[%d] callid[%d]", iSessionID, msg.wParam);
            if (iSessionID >= 0)
            {
                // 数据赋值
                if (pData->direct == IPVP_VQR_DIR_REMOTE)
                {
                    // 远端数据:只通过IPVP_FL_VQR_PRIV上报，并且不管什么时候，都可能上报：此处需要刷新界面
                    itr->second.m_RemoteIntervalData = *pData;
                }
                else
                {
                    itr->second.m_iActiveType = msg.lParam;
                    itr->second.m_IntervalData = *pData;
                }

                PrintData(pData);

                m_iRecentCallId = msg.wParam;

                // http://10.2.1.199/Bug.php?BugID=93314

                // 通知UI刷新
                if (talkLogic_IsSessionInTalking(iSessionID, true))
                {
                    NotifyToUI(msg.wParam, 0);
                }
            }
        }
        break;
    default:
        bHandle = false;
        break;
    }

    return bHandle;
}

// 辅助函数 添加节点
static void AddNodeItem(xml_node& root, const char * lpsNodeName, const yl::string & strValue)
{
    if (!root)
    {
        return ;
    }

    xml_node nodeItem = root.append_child(lpsNodeName);

    if (!nodeItem)
    {
        return ;
    }

    nodeItem.append_child(node_pcdata).set_value(strValue.c_str());
}

// 辅助函数 读取节点
static void ParserNodeItem(xml_node& root, const char * lpsNodeName, yl::string & strValue)
{
    if (!root)
    {
        return ;
    }

    xml_node nodeItem = root.child(lpsNodeName);
    if (!nodeItem)
    {
        return ;
    }

    strValue = nodeItem.child_value();
}

// 保存SessionReport数据到文件
void CVQReportManager::SaveSessionReportData(const SVQReportData & sData)
{
//  TALK_INFO("Save vq data for web");
    // 设置Xml文件头
    xml_document doc;
    xml_node nodeDecl = doc.prepend_child(node_declaration);
    nodeDecl.append_attribute("version", "1.0");
    nodeDecl.append_attribute("encoding", "utf-8");

    xml_node nodeRootNode = doc.append_child("VQSessionReport");
    AddNodeItem(nodeRootNode, kszLocalNumber, sData.strLocalNumber);
    AddNodeItem(nodeRootNode, kszRemoteNumber, sData.strRemoteNumber);
    AddNodeItem(nodeRootNode, kszStartTime, sData.strStartTime);
    AddNodeItem(nodeRootNode, kszCurrentTime, sData.strCurrentTime);
    AddNodeItem(nodeRootNode, kszJitter, sData.strJitter);
    AddNodeItem(nodeRootNode, kszPktLost, sData.strPktLost);
    AddNodeItem(nodeRootNode, kszSymmOneWayDelay, sData.strSymmOneWayDelay);
    AddNodeItem(nodeRootNode, kszRoundTripDelay, sData.strRoundTripDelay);
    AddNodeItem(nodeRootNode, kszMOSLQ, sData.strMOSLQ);
    AddNodeItem(nodeRootNode, kszMOSCQ, sData.strMOSCQ);
    AddNodeItem(nodeRootNode, kszNetworkPacketLossRate, sData.strNetworkPacketLossRate);
    AddNodeItem(nodeRootNode, kszEndSystemDelay, sData.strEndSystemDelay);
    AddNodeItem(nodeRootNode, kszInterarrivalJitter, sData.strInterarrivalJitter);
    AddNodeItem(nodeRootNode, kszRemoteIPP, sData.strRemoteIP);
    AddNodeItem(nodeRootNode, kszLocalPort, sData.strLocalPort);
    AddNodeItem(nodeRootNode, kszRemotePort, sData.strRemotePort);
    AddNodeItem(nodeRootNode, kszCodec, sData.strCodecName);
    AddNodeItem(nodeRootNode, kszJitterBufferMax, sData.strJitterBufferMax);

    doc.save_file(szSessionReportFile);
    return;
}

// 获取SessionReport
void CVQReportManager::GetSessionReportData(SVQReportData & sData)
{
    xml_document doc;
    if (doc.load_file(szSessionReportFile).status == status_ok)
    {
        xml_node nodeRootNode = doc.child("VQSessionReport");
        ParserNodeItem(nodeRootNode, kszLocalNumber, sData.strLocalNumber);
        ParserNodeItem(nodeRootNode, kszRemoteNumber, sData.strRemoteNumber);
        ParserNodeItem(nodeRootNode, kszStartTime, sData.strStartTime);
        ParserNodeItem(nodeRootNode, kszCurrentTime, sData.strCurrentTime);
        ParserNodeItem(nodeRootNode, kszJitter, sData.strJitter);
        ParserNodeItem(nodeRootNode, kszPktLost, sData.strPktLost);
        ParserNodeItem(nodeRootNode, kszSymmOneWayDelay, sData.strSymmOneWayDelay);
        ParserNodeItem(nodeRootNode, kszRoundTripDelay, sData.strRoundTripDelay);
        ParserNodeItem(nodeRootNode, kszMOSLQ, sData.strMOSLQ);
        ParserNodeItem(nodeRootNode, kszMOSCQ, sData.strMOSCQ);
        ParserNodeItem(nodeRootNode, kszNetworkPacketLossRate, sData.strNetworkPacketLossRate);
        ParserNodeItem(nodeRootNode, kszEndSystemDelay, sData.strEndSystemDelay);
        ParserNodeItem(nodeRootNode, kszInterarrivalJitter, sData.strInterarrivalJitter);
        ParserNodeItem(nodeRootNode, kszRemoteIPP, sData.strRemoteIP);
        ParserNodeItem(nodeRootNode, kszLocalPort, sData.strLocalPort);
        ParserNodeItem(nodeRootNode, kszRemotePort, sData.strRemotePort);
        ParserNodeItem(nodeRootNode, kszCodec, sData.strCodecName);
        ParserNodeItem(nodeRootNode, kszJitterBufferMax, sData.strJitterBufferMax);
    }
}

void CVQReportManager::SaveSessionReportDataToWeb()
{
    if (m_mapVQReportData.size() == 0)
    {
        return;
    }

    TALK_INFO("web get rtp[%d]", m_iRecentCallId);
    ITER_VQREPORTDATA itr = m_mapVQReportData.find(m_iRecentCallId);
    if (itr != m_mapVQReportData.end())
    {
        SVQReportData sData;
        itr->second.GetReportDataToUI(sData);
        SaveSessionReportData(sData);
    }

    return;
}

// 获取Interval或者alert数据
void CVQReportManager::GetCurrentReportData(int iCallId, SVQReportData & sData)
{
    ITER_VQREPORTDATA itr = m_mapVQReportData.find(iCallId);
    if (itr != m_mapVQReportData.end())
    {
        itr->second.GetReportDataToUI(sData);
    }
}

// 通话开启的处理
void CVQReportManager::ProcessStartTalk(int iCallID, int nLineId, bool bCaller,
                                        const yl::string & strRemoteNumber)
{
    TALK_DBG("ProcessStartTalk callid[%d] line[%d] out[%d] num[%s]",
             iCallID, nLineId, bCaller, strRemoteNumber.c_str());

    // 存在 则不处理
    if (m_mapVQReportData.find(iCallID) != m_mapVQReportData.end())
    {
        return ;
    }

    TALK_INFO("RTCP-XR ProcessStartTalk, iCallId = %d", iCallID);

    CVQReportElement sData;
    sData.m_iCallID = iCallID;
    sData.m_nLineID = nLineId;
    sData.m_bCaller = bCaller;
    sData.m_strRemoteNumber = strRemoteNumber;

    sData.m_iTimerSend = configParser_GetConfigInt(kszRTCPIntervalPeriod);
    if (sData.m_iTimerSend < 5)
    {
        sData.m_iTimerSend = 5 * 1000;
    }
    else if (sData.m_iTimerSend > 20)
    {
        sData.m_iTimerSend = 20 * 1000;
    }
    else
    {
        sData.m_iTimerSend *= 1000;
    }

    // 创建CVQReportElement
    m_mapVQReportData[iCallID] = sData;

    // 开启定时发送Interval Report
    if (configParser_GetConfigInt(kszRTCPIntervalReport) == 1)
    {
        m_mapVQReportData[iCallID].ControlSendTimer();
    }
}

// 处理定时器消息
bool CVQReportManager::OnTimer(UINT uTimerID)
{
    ITER_VQREPORTDATA itr = m_mapVQReportData.begin();
    for (; itr != m_mapVQReportData.end(); ++itr)
    {
        bool bSaveToFile = false;

        if (itr->second.OnTimerOut(uTimerID, bSaveToFile))
        {
            if (bSaveToFile)
            {
                // 保存到文件
                SVQReportData sData;
                itr->second.GetReportDataToUI(sData);
                SaveSessionReportData(sData);
            }
            m_mapVQReportData.erase(itr);

            return true;
        }
        else if (itr->second.OnTimerSend(uTimerID))
        {
            return true;
        }
    }

    return false;
}

// 通话结束后的处理
void CVQReportManager::ProcessEndCall(int iCallId)
{
    TALK_DBG("ProcessEndCall callid[%d] session[%d]",
             iCallId, talklogic_GetSessionIdByCallID(iCallId, false));

    TALK_INFO("RTCP-XR ProcessEndCall, iCallId = %d", iCallId);

    ITER_VQREPORTDATA itr = m_mapVQReportData.find(iCallId);
    if (itr != m_mapVQReportData.end())
    {
        itr->second.ControlSendTimer(false);
        itr->second.StartTimerOut();
    }

    // 通知UI退出
    NotifyToUI(iCallId, 1);
}

// 焦点话路变更处理
void CVQReportManager::OnFocusSessionChange(int iSessionId)
{
    int iCallID = talklogic_GetCallIdBySessionID(iSessionId);
    TALK_DBG("OnFocusSessionChange session[%d] callid[%d]", iSessionId, iCallID);
    // 会议取第一路可用的
    if (talklogic_GetHostSessionID(iSessionId) != INVALID_SESSION_ID)
    {
        m_iActiveCallId = GetConfFirstSupportRTP(iSessionId);
    }
    // 单路通话就是活跃
    else
    {
        m_iActiveCallId = iCallID;
    }

    TALK_INFO("RTCP-XR OnFocusSessionChange, iCallId = %d", m_iActiveCallId);
}

// 通话Held的处理
void CVQReportManager::ProcessHeldTalk(int iSessionID)
{
    // 数据出错
    int iCallId = talklogic_GetCallIdBySessionID(iSessionID);
    TALK_DBG("ProcessHeldTalk callid[%d] session[%d]", iCallId, iSessionID);
    ITER_VQREPORTDATA itr = m_mapVQReportData.find(iCallId);
    if (itr == m_mapVQReportData.end())
    {
        return ;
    }

    int iHostSessionID = talklogic_GetSessionIdByCallID(iCallId);
    // 当前是普通通话界面
    if (!talklogic_IsLocalConf(iCallId))
    {
        if (!IsRTPAvaliable(iHostSessionID))
        {
            NotifyToUI(iCallId, 1);
        }

        return ;
    }

    // 当前是会议通话界面
    TALK_INFO("RTCP-XR ProcessHeldTalk, iCallId = %d, iActiveCallId = %d", iCallId, m_iActiveCallId);

    int iActiveSessionID = talklogic_GetSessionIdByCallID(m_iActiveCallId, false);
    // 操作的并非当前话路 则仅需通知UI刷新Softkey
    if (iActiveSessionID < 0)
    {
        return ;
    }

    if (IsRTPAvaliable(iActiveSessionID))
    {
        NotifyToUI(m_iActiveCallId, 0);
        return ;
    }

    YLVector<int> list;
    if (!talklogic_GetSubSesseionID(iHostSessionID, list))
    {
        return;
    }

    for (YLVector<int>::iterator it = list.begin(); it != list.end(); ++it)
    {
        int iSubSessionID = *it;
        if (IsRTPAvaliable(iSubSessionID))
        {
            m_iActiveCallId = talklogic_GetCallIdBySessionID(iSubSessionID);
            NotifyToUI(m_iActiveCallId, 0);
            return;
        }
    }

    NotifyToUI(iCallId, 1);
}

// 会议中的话路是否可切换
bool CVQReportManager::IsRTPAvaliable(int iSessionID)
{
    if (iSessionID < 0)
    {
        return false;
    }

    return talkLogic_IsSessionInTalking(iSessionID, true) && !IsRTPForbidden(iSessionID);
}

// 该话路是否禁止RTP
bool CVQReportManager::IsRTPForbidden(int iSessionID)
{
    if (iSessionID < 0)
    {
        return true;
    }

    return (SESSION_LINEDEVICE == talklogic_GetSessionStateBySessionID(iSessionID));
}

// 会议中的话路是否可切换
bool CVQReportManager::IsRTPStatusSwitch()
{
    if (!talklogic_IsLocalConf(m_iActiveCallId))
    {
        return false;
    }

    int iSessionID = talklogic_GetSessionIdByCallID(m_iActiveCallId);

    YLVector<int> list;
    if (!talklogic_GetSubSesseionID(iSessionID, list))
    {
        // 不是会议
        return false;
    }

    int iCount = 0;
    for (YLVector<int>::iterator it = list.begin(); it != list.end(); ++it)
    {
        if (IsRTPAvaliable(*it))
        {
            iCount++;
        }

        if (iCount >= 2)
        {
            return true;
        }
    }

    return false;
}

// 切换会议中的话路显示
bool CVQReportManager::SwitchRTPStatus()
{
    int iSessionID = talklogic_GetSessionIdByCallID(m_iActiveCallId);

    YLVector<int> list;
    if (!talklogic_GetSubSesseionID(iSessionID, list))
    {
        // 不是会议
        return false;
    }

    int iTmpCallID = 0;
    bool bFoundSession = false;
    for (YLVector<int>::iterator it = list.begin(); it != list.end(); ++it)
    {
        int iSubSessionID = *it;
        int iSubCallID = talklogic_GetCallIdBySessionID(iSubSessionID);

        if (!bFoundSession)
        {
            if (m_iActiveCallId == iSubCallID)
            {
                bFoundSession = true;
            }

            if (0 == iTmpCallID
                    && IsRTPAvaliable(iSubSessionID))
            {
                iTmpCallID = iSubCallID;
            }
            continue;
        }

        if (IsRTPAvaliable(iSubSessionID))
        {
            iTmpCallID = iSubCallID;
            break;
        }
    }

    TALK_INFO("RTCP-XR SwitchRTPStatus, iCallId = %d, m_iActiveCallId = %d", iTmpCallID,
              m_iActiveCallId);

    if (0 == iTmpCallID)
    {
        return false;
    }

    if (m_iActiveCallId != iTmpCallID)
    {
        m_iActiveCallId = iTmpCallID;
        NotifyToUI(m_iActiveCallId, 0);
    }

    return true;
}

void CVQReportManager::NotifyToUI(int iCallId, int iAction)
{
    TALK_DBG("NotifyToUI callid[%d] active[%d] action[%d]", iCallId, m_iActiveCallId, iAction);
    // 焦点话路 通知UI刷新界面
    if (iCallId == m_iActiveCallId)
    {
        etl_NotifyApp(false, RTCP_UDPATE_WND, iAction, 0);
    }
}

// 获取会议中第一个可用的RTP
int CVQReportManager::GetConfFirstSupportRTP(int iSessionID)
{
    YLVector<int> list;
    if (!talklogic_GetSubSesseionID(iSessionID, list))
    {
        // 不是会议
        return -1;
    }

    int iTargetSessionID = -1;
    for (YLVector<int>::iterator it = list.begin(); it != list.end(); ++it)
    {
        int iSubSessionID = *it;

        if (-1 == iTargetSessionID
                && !IsRTPForbidden(iSubSessionID))
        {
            iTargetSessionID = iSubSessionID;
        }

        if (IsRTPAvaliable(iSubSessionID))
        {
            iTargetSessionID = iSubSessionID;
            break;
        }
    }

    TALK_DBG("GetConfFirstSupportRTP host[%d] session[%d]", iSessionID, iTargetSessionID);
    return talklogic_GetCallIdBySessionID(iTargetSessionID);
}
#endif // IF_FEATURE_RTCPXR
