#if IF_FEATURE_PAGING
#include "talklogic_inc.h"
#include "talk/ippaging/include/ippaging.h"
#include "configiddefine.h"
#include <winimp/winimp.h>

#define MAX_LISTENING_LIST_NUMBER   31

//////////////////// COMMON FUNCTIONS /////////////////////////////
static int StartPageCallIn(PAGING_PARAM* param)
{
    // 转为消息处理的原因：调用些函数时是Pagin的线程执行的，如果执行的函数复杂，与主线程可能存在同步问题
    return msgPostMsgToThreadEx(msgGetThreadByName(VP_PHONE_THREAD_NAME), DSK_MSG_LISTEN_MULTICAST,
                                1, 0, sizeof(PAGING_PARAM), param);
}

static int StopPageCallIn(PAGING_PARAM* param)
{
    return msgPostMsgToThreadEx(msgGetThreadByName(VP_PHONE_THREAD_NAME), DSK_MSG_LISTEN_MULTICAST,
                                0, 0, sizeof(PAGING_PARAM), param);
}

// 处理paging的消息
static LRESULT OnMulticastPageMessage(msgObject& msg)
{
    TALK_INFO("DSK_MSG_LISTEN_MULTICAST [%d][%d]", msg.wParam, msg.lParam);
    PAGING_PARAM* pInfo = (PAGING_PARAM*)msg.GetExtraData();
    if (pInfo == NULL)
    {
        return -1;
    }

    if (msg.wParam)
    {
        _MulticastPage.ReceiveCallInPage(pInfo);
    }
    else
    {
        _MulticastPage.BreakOffCallInPage(pInfo);
    }

    return 1;
}

// 处理paging的消息
static LRESULT OnMulticastCfgChange(msgObject& msg)
{
    if (ST_MULTICAST == msg.wParam)
    {
        _MulticastPage.InitListenInfo();
    }

    return 1;
}

// 处理网络状态改变的消息
static LRESULT OnNetworkChange(msgObject& msg)
{
    bool bOk = (netGetNetworkSta() == WPS_READY
                || netGetIPv6Status() == WPS_READY);
    _MulticastPage.OnNetworkChange(bOk);
    return 1;
}

bool ComparePriority(PAGING_PARAM* lpData, PAGING_PARAM* rpData)
{
    if (lpData == NULL || rpData == NULL)
    {
        return false;
    }

    int iLID = _MulticastPage.GetPriorityByIP(lpData->remote_ip_address, lpData->remote_udp_port,
               lpData->paging_channel);
    int iRID = _MulticastPage.GetPriorityByIP(rpData->remote_ip_address, rpData->remote_udp_port,
               rpData->paging_channel);

    return iLID < iRID;
}

///////////////////// class CMulticastPage  /////////////////////////////////////
CMulticastPage& CMulticastPage::GetInstance()
{
    static CMulticastPage s_objMuticastPage;
    return s_objMuticastPage;
}

// 构造函数
CMulticastPage::CMulticastPage()
{
    m_iCurrentPort = 0;
    m_iMinRTPPort = 0;
    m_iMaxRTPPort = 0;
    m_iDTMFPayload = 0;
    m_iReceivePriority = 0;
    m_bPriorityActive = false;
    memset(m_strCodecName, 0, size_MIME);
}

CMulticastPage::~CMulticastPage()
{
    ClearPageList();
    ClearWaitList();
    paging_exit();
}

void CMulticastPage::InitInfo()
{
    // 去掉通话中的rtp包打印信息
    int iTrace = configParser_GetConfigInt(kszTraceLevel);
    TALK_INFO("Paging Trace Level %d", iTrace); // 初始化库信息
    paging_init();
    paging_set_debug_level(iTrace);
    // 绑定Paging来电消息处理
    paging_set_event_callback(PAGING_EVENT_ID_START, StartPageCallIn);
    paging_set_event_callback(PAGING_EVENT_ID_STOP, StopPageCallIn);

    // !!!消息注册
    SingleMsgReg(DSK_MSG_LISTEN_MULTICAST, OnMulticastPageMessage);
    SingleMsgReg(CONFIG_MSG_BCAST_CHANGED, OnMulticastCfgChange);
    SingleMsgReg(SYS_MSG_NOTIFY_NETWORK_STATUS, ::OnNetworkChange);

    // 读取端口

    m_iMinRTPPort = configParser_GetConfigInt(kszNetworkMinRTPPort);
    m_iCurrentPort = m_iMinRTPPort;
    m_iMaxRTPPort = configParser_GetConfigInt(kszNetworkMaxRTPPort);
    m_iDTMFPayload = configParser_GetConfigInt(kszDTMFPayload);

    // 初始化配置
    InitListenInfo();
}

bool CMulticastPage::BeMulticastIP(const yl::string& strIPPort)
{
    return commonUnits_BeMulticastIP(strIPPort);
}

void CMulticastPage::SetPageVoiceParam(CCallInfo* pCallInfo, PAGING_PARAM* pPage)
{
    if (NULL == pCallInfo)
    {
        return;
    }

    ipvp_speech_t& voiceparam = pCallInfo->GetIPVPParam();

    // 设置声音参数
    SetVoiceParam(&voiceparam, pPage);

    // xww test
    PrintVoice(voiceparam);

    // 设置完VPM的音频参数后设置组播的发送参数
    SetMultiPageParam(pCallInfo, pPage);
}

// 设置通话的Voice参数
bool CMulticastPage::SetVoiceParam(ipvp_speech_t* pVmParam, PAGING_PARAM* pPageParam)
{
    if (NULL == pVmParam)
    {
        return false;
    }

    // 先清空
    memset(pVmParam, 0, sizeof(ipvp_speech_t));

    // 获取话机IP
    yl::string strIP = "0.0.0.0";
    netGetWanIP(strIP);

    // 设置本地IP和端口
    inet_aton(strIP.c_str(), &pVmParam->audio.addr_rtp.src.sa4.sin_addr);
    int iRtpPort = GetRtpPort();
    pVmParam->audio.addr_rtp.src.sa4.sin_port = ntohs(iRtpPort);
    pVmParam->video.addr_rtp.src.sa4.sin_port = 0;
    // V80 VPM要求新设定参数
    pVmParam->audio.addr_rtp.af_type = AF_INET;
    pVmParam->audio.addr_rtp.src.sa4.sin_family = AF_INET;
    pVmParam->audio.addr_rtp.sa_len = sizeof(struct sockaddr_in);

    inet_aton(strIP.c_str(), &pVmParam->audio.addr_rtcp.src.sa4.sin_addr);
    pVmParam->audio.addr_rtcp.src.sa4.sin_port = ntohs(iRtpPort + 1);
    pVmParam->video.addr_rtcp.src.sa4.sin_port = 1;
    pVmParam->audio.addr_rtcp.af_type = AF_INET;
    pVmParam->audio.addr_rtcp.src.sa4.sin_family = AF_INET;
    pVmParam->audio.addr_rtcp.sa_len = sizeof(struct sockaddr_in);

    pVmParam->audio.addr_rtp.dst.sa4.sin_family = AF_INET;

    //pVmParam->audio.addr_rtcp.dst.sa4.sin_family = AF_INET;

    inet_aton("127.0.0.1", &pVmParam->audio.addr_rtcp.dst.sa4.sin_addr);

    // 设置对方的IP和端口
    inet_aton("127.0.0.1", &pVmParam->audio.addr_rtp.dst.sa4.sin_addr);
    iRtpPort = GetRtpPort();
    pVmParam->audio.addr_rtp.dst.sa4.sin_port = ntohs(iRtpPort);
    pVmParam->video.addr_rtp.dst.sa4.sin_port = 0;
    pVmParam->audio.addr_rtcp.dst.sa4.sin_family = AF_INET;
    pVmParam->audio.addr_rtcp.dst.sa4.sin_port = ntohs(iRtpPort + 1);
    pVmParam->video.addr_rtcp.dst.sa4.sin_port = 1;

    // 设置Codec信息
    if (pPageParam != NULL)
    {
        if (0 == strlen(pPageParam->codec_name))
        {
            strcpy(pPageParam->codec_name, GetCodecNameByID(pPageParam->payload_type));
        }

        strcpy(pVmParam->audio.codec_name, pPageParam->codec_name);
    }
    else
    {
        strcpy(pVmParam->audio.codec_name, m_strCodecName);
    }

    pVmParam->audio.codec_ptype =  GetCodecIDByName(pVmParam->audio.codec_name);
    pVmParam->audio.event_ptype = m_iDTMFPayload;

    // V80 VPM要求
    pVmParam->mode = IPVP_FL_VAM_AUDIO;
    pVmParam->audio.flag = IPVP_ADDR_FL_SENDRECV;
    pVmParam->audio.ptime = 20;

    return true;
}

bool CMulticastPage::SetMultiPageParam(CCallInfo* pCallInfo, PAGING_PARAM* pPageParam)
{
    if (pCallInfo == NULL)
    {
        return false;
    }

    PAGING_PARAM* pTmp = pCallInfo->m_pPageParam;
    if (NULL == pTmp)
    {
        pTmp = new PAGING_PARAM;
        chASSERT(pTmp != NULL);
        memset(pTmp, 0, sizeof(PAGING_PARAM));
    }

    if (pPageParam != NULL)
    {
        memcpy(pTmp, pPageParam, sizeof(PAGING_PARAM));

        // 传给VPM的端口号已经转换字节序，此处需要转换回来
        pTmp->local_udp_port = ntohs(pCallInfo->m_tVPMParam.audio.addr_rtp.src.sa4.sin_port);
        pTmp->local_bind_port = ntohs(pCallInfo->m_tVPMParam.audio.addr_rtp.dst.sa4.sin_port);
        pTmp->paging_channel = pPageParam->paging_channel;
    }
    else
    {
        pTmp->paging_in = 0;
        pTmp->payload_type = GetCodecIDByName(m_strCodecName);
        strcpy(pTmp->codec_name, m_strCodecName);

        char pStrIp[50] = "", pStrPort[10] = "";
        sscanf(pCallInfo->m_tRemoteInfo.sSIPName.c_str(), "%[^:]:%s", pStrIp, pStrPort);
        strcpy(pTmp->remote_ip_address, pStrIp);
        pTmp->remote_udp_port = atoi(pStrPort);

        // 传给VPM的端口号已经转换字节序，此处需要转换回来
        pTmp->local_udp_port = ntohs(pCallInfo->m_tVPMParam.audio.addr_rtp.dst.sa4.sin_port);
    }
    // 获取话机IP
    yl::string strIP = "127.0.0.1";
    netGetWanIP(strIP);
    strcpy(pTmp->local_ip_address, strIP.c_str());
    pCallInfo->m_pPageParam = pTmp;

    return true;
}

// 检测Paging的接收条件
// 小于0则不接收，等于0则等待
int CMulticastPage::CheckRecvCondition(int iPriority)
{
    SignalParams sigp;
    sigp.wParam.iValue = iPriority;
    sigp.lParam.iValue = m_iReceivePriority;
    sigp.tParam.iValue = m_bPriorityActive;
    if (modtklsn_emit(TALK_SIG_RECV_PAGE, sigp))
    {
        return sigp.uReply;
    }

    return RCR_OK;
}

bool CMulticastPage::CallPageOut(const char* cszDisplay, const char* cszNum, int iDsskey,
                                 const char* cszChannel/* = ""*/)
{
    if (cszNum == NULL
            || 0 == strlen(cszNum)
            || !BeMulticastIP(cszNum))
    {
        return false;
    }

    if (!netIsNetworkReady())
    {
        TALK_WARN("Paging CallOut Failed (network unavailable)");
        return false;
    }

    // 新建Session,呼出
    CSingleSession* pSession = _SessionManager.AllocSession();
    if (pSession == NULL)
    {
        return false;
    }

    CCallInfo* pCallInfo = pSession->GetCallInfo();
    pCallInfo->m_bHoldAvailable = true; // 默认是可以hold的
    pCallInfo->m_eCallType = CT_MULTICAST_OUT;
    pCallInfo->m_tRemoteInfo.sSIPName = cszNum;
    pCallInfo->m_tRemoteInfo.sDisplayName = (NULL == cszDisplay
                                            || 0 == strlen(cszDisplay)) ? cszNum : cszDisplay;
    pCallInfo->GetCallOutParam().iSponsorDsskeyID = iDsskey;

    // 设置组播参数
    SetPageVoiceParam(pCallInfo);

    if (NULL != pCallInfo->m_pPageParam)
    {
        yl::string strChannel = cszChannel;
        if (strChannel.empty() && iDsskey != -1)
        {
            strChannel = dsskey_GetDsskeyExtension(iDsskey);
        }

        if (commonUnits_IsValidPagingChannel(strChannel))
        {
            pCallInfo->m_pPageParam->paging_channel = atoi(strChannel.c_str());
        }
        else
        {
            pCallInfo->m_pPageParam->paging_channel = 0;
        }
    }

    pSession->EnterRoutine(ROUTINE_TALKING);

    // 判断是否调用paging
    if (!StartPagingTalk(pCallInfo->m_pPageParam))
    {
        TALK_WARN("StartPagingTalk Fail!");
    }

    // VPM
    pSession->GetRoutine()->StartTalk();

    voice_PlayTone(TONE_CALLWAIT2, 500);

    return true;
}

int CMulticastPage::GetRtpPort()
{
    int iRet = m_iCurrentPort;
    m_iCurrentPort += 2;
    if (m_iCurrentPort > m_iMaxRTPPort)
    {
        m_iCurrentPort = m_iMinRTPPort;
    }
    return iRet;
}

bool CMulticastPage::BreakOffCallInPage(PAGING_PARAM* param)
{
    if (NULL == param)
    {
        TALK_WARN("Not Multicast Paging Call In");
        return false;
    }

#if 0
    PrintPage(param);
#endif
    // 如果结束的是正在等待的Paging,则将该Paging从等待链表中移除
    RemoveWaitPage(param);

    param->paging_in = true;
    int iID = GetPriorityByIP(param->remote_ip_address, param->remote_udp_port, param->paging_channel);

    CSingleSession* pSession = _SessionManager.GetRelateSessionByCallID(iID);
    if (pSession == NULL)
    {
        TALK_WARN("ERROR CallID[%d]", iID);
        return false;
    }

    if (!pSession->BeMulticastPage())
    {
        TALK_WARN("Not Multicast Paging Call");
        return false;
    }

    CCallInfo* pCallInfo = pSession->GetCallInfo();
    if (NULL != pCallInfo)
    {
        pCallInfo->m_eStopPageType = SP_STOP;
    }

    // 退出
    pSession->ExitSession();
    return true;
}

bool CMulticastPage::ReceiveCallInPage(PAGING_PARAM* param)
{
    if (NULL == param)
    {
        TALK_WARN("Not Multicast Paging Call In");
        return false;
    }
    else
    {
        // 获取话机IP
        yl::string strIP = "0.0.0.0";
        netGetWanIP(strIP);
        if (param->request_ip_address == strIP)
        {
            TALK_WARN("Multicast Paging Call From Oneself");
            return false;
        }
    }

    param->paging_in = true;
    PrintPage(param);

    int iID = GetPriorityByIP(param->remote_ip_address, param->remote_udp_port, param->paging_channel);
    if (iID < 1 || iID > MAX_LISTENING_LIST_NUMBER)
    {
        TALK_WARN("CMulticastPage: Invalid Paging Priority [%d]", iID);
        return false;
    }

    // 检测条件
    // 小于0则不接收，等于1则等待
    int iCheckRet = CheckRecvCondition(iID);
    TALK_INFO("Check CheckRecvCondition [%d]", iCheckRet);
    if (iCheckRet != RCR_OK)
    {
        if (iCheckRet == RCR_WAIT)
        {
            AddWaitPage(param);
        }
        return false;
    }

    CSingleSession* pSession = _SessionManager.AllocSession(iID);
    if (pSession == NULL)
    {
        TALK_WARN("Call income priority(%d), but can't add it, Ignor", iID);
        return false;
    }

    // 满足以上条件则建立通话
    CCallInfo* pCallInfo = pSession->GetCallInfo();
    pCallInfo->m_eCallType = CT_MULTICAST_IN;
    pCallInfo->m_bHoldAvailable = true; // 默认是可以hold的

    yl::string strPage = commonAPI_FormatString(_T("%s:%d"), param->remote_ip_address,
                         param->remote_udp_port);
    pCallInfo->m_tRemoteInfo.sSIPName = strPage.c_str();
    // 获取显示名
    RECPage_Info* pInfo = GetPageById(iID);
    if (pInfo != NULL
            && !pInfo->strLabel.empty())
    {
        pCallInfo->m_tRemoteInfo.sDisplayName = pInfo->strLabel;
    }
    else
    {
        pCallInfo->m_tRemoteInfo.sDisplayName = strPage;
    }

    TALK_INFO("Call Income in priority[%d]", iID);
    SetPageVoiceParam(pCallInfo, param);

    PrintPage(pCallInfo->m_pPageParam);

    // 判断是否调用paging
    // StartPagingTalk(pCallInfo->m_pPageParam);

    // StartTalk(); 是否应该用BaseRoutine的函数
    // pCallInfo->m_bStartTalk = true;
    // voice_StartTalk(pCallInfo->GetCallID(), pCallInfo->m_tVPMParam);

    pSession->EnterRoutine(ROUTINE_TALKING);

    // 判断是否调用paging
    StartPagingTalk(pCallInfo->m_pPageParam);
    pSession->GetRoutine()->StartTalk();

    if (configParser_GetConfigInt(kszMulticastUseSpeaker) == 1)
    {
        voice_SwitchChannel(CHANNEL_HANDFREE);
    }

    //
    voice_PlayTone(TONE_CALLWAIT2, 500);

    return true;
}

void CMulticastPage::ClearPageList()
{
    while (m_pListPage.size() > 0)
    {
        list_page::iterator it = m_pListPage.begin();
        if (NULL != (*it))
        {
            delete (*it);
            (*it) = NULL;
        }
        m_pListPage.erase(it);
    }
}

void CMulticastPage::ClearWaitList()
{
    while (m_pListWait.size() > 0)
    {
        list_wait_page::iterator it = m_pListWait.begin();
        delete (*it);
        (*it) = NULL;
        m_pListWait.erase(it);
    }
}

void CMulticastPage::InitListenInfo()
{
    if (!netIsNetworkReady())
    {
        TALK_WARN("Network is unavailable");
        return;
    }

    if (GET_CONFIG_PROCESS_TYPE() == INI_FILE_PROCESS
            && !pathFileExist(szMulticastFile))
    {
        TALK_WARN("Paing file not exist");
        return;
    }

    strncpy(m_strCodecName, configParser_GetConfigString(kszPayloadType).c_str(), size_MIME);
    m_iReceivePriority = configParser_GetConfigInt(kszReceivePriority);
    m_iReceivePriority = (m_iReceivePriority >= 0
                          && m_iReceivePriority <= MAX_LISTENING_LIST_NUMBER) ? m_iReceivePriority : 0;
    m_bPriorityActive = configParser_GetConfigInt(kszReceiveActive);
    for (int i = 0; i < MAX_LISTENING_LIST_NUMBER; i++)
    {

        ChangeListen(i + 1);

    }
}

RECPage_Info* CMulticastPage::ExistInList(const char* pStrPage, int iChannel)
{
    if (pStrPage != NULL && m_pListPage.size() > 0)
    {
        list_page::iterator it = m_pListPage.begin();
        for (; it != m_pListPage.end(); ++it)
        {
            RECPage_Info* pInfo = (RECPage_Info*)((*it));
            if (pInfo->strValue == pStrPage
                    || 0 == strcmp(strstr(pInfo->strValue.c_str(), ":"), strstr(pStrPage, ":")))
            {
                if (pInfo->iChannel == iChannel)
                {
                    return pInfo;
                }
            }
        }
    }

    return NULL;
}

RECPage_Info* CMulticastPage::GetPageById(int iID)
{
    list_page::iterator it = m_pListPage.begin();
    for (; it != m_pListPage.end(); ++it)
    {
        RECPage_Info* pInfo = (RECPage_Info*)((*it));
        if (pInfo->iPriority == iID)
        {
            return pInfo;
        }
    }
    return NULL;
}

void CMulticastPage::ChangeListen(int iID)
{
    int iIndex = iID;

    yl::string strIPAddress = configParser_GetCfgItemStringValue(kszMulticastListenIP, iIndex).c_str();
    int iChannel = configParser_GetCfgItemIntValue(kszMulticastListenChannel, iIndex);
    CheckChannel(iChannel);

    // 去除多余的监听组播
    RECPage_Info* pInfo = GetPageById(iID);
    if (pInfo != NULL)
    {
        if (pInfo->strValue != strIPAddress
                || pInfo->iChannel != iChannel)
        {
            RemoveListenByID(pInfo);
        }
        else
        {
            pInfo->strLabel = configParser_GetCfgItemStringValue(kszMulticastListenLabel, iIndex).c_str();
        }
    }

    // 添加新的监听组播
    if (BeMulticastIP(strIPAddress))
    {
        pInfo = ExistInList(strIPAddress.c_str(), iChannel);
        if (NULL == pInfo)
        {
            RECPage_Info* pNewInfo = new RECPage_Info;
            chASSERT(pNewInfo != NULL);
            pNewInfo->iPriority = iID;
            pNewInfo->strValue = strIPAddress;
            pNewInfo->iChannel = iChannel;
            pNewInfo->strLabel = configParser_GetCfgItemStringValue(kszMulticastListenLabel, iIndex).c_str();

            m_pListPage.push_back(pNewInfo);

            // 加载监听
            PAGING_PARAM tPageInfo = GetPageInfo(pNewInfo);
            PrintPage(&tPageInfo);

            paging_add_listen(&tPageInfo);
        }
        else if (pInfo->iPriority > iID)
        {
            // 如果存在相同的端口号,但组播不同,则要更换组播
            if (pInfo->strValue != strIPAddress
                    || pInfo->iChannel != iChannel)
            {
                PAGING_PARAM tPageInfo = GetPageInfo(pInfo);
                TALK_INFO("paging_remove_listen of same port [%s] [%d]", pInfo->strValue.c_str(), pInfo->iChannel);
                PrintPage(&tPageInfo);
                paging_remove_listen(&tPageInfo);
                pInfo->strValue = strIPAddress;
                pInfo->iChannel = iChannel;
                tPageInfo = GetPageInfo(pInfo);
                TALK_INFO("paging_add_listen of same port [%s] [%d]", pInfo->strValue.c_str(), pInfo->iChannel);
                PrintPage(&tPageInfo);
                paging_add_listen(&tPageInfo);
            }
            pInfo->iPriority = iID;
            pInfo->strLabel = configParser_GetCfgItemStringValue(kszMulticastListenLabel, iIndex).c_str();
        }
    }
}

void CMulticastPage::RemoveListenByID(RECPage_Info* pInfo)
{
    list_page::iterator it = m_pListPage.begin();
    for (; it != m_pListPage.end(); ++it)
    {
        if (pInfo == (*it))
        {
            PAGING_PARAM tPageInfo = GetPageInfo(pInfo);
            PrintPage(&tPageInfo);
            paging_remove_listen(&tPageInfo);

            delete (*it);
            m_pListPage.erase(it);
            return;
        }
    }
}

PAGING_PARAM CMulticastPage::GetPageInfo(RECPage_Info* pInfo)
{
    PAGING_PARAM tPageInfo;
    memset(&tPageInfo, 0, sizeof(PAGING_PARAM));
    if (pInfo != NULL)
    {
        tPageInfo.paging_in = 1;
        char pStrIp[50] = "", pStrPort[10] = "";
        sscanf(pInfo->strValue.c_str(), "%[^:]:%s", pStrIp, pStrPort);
        strcpy(tPageInfo.remote_ip_address, pStrIp);
        tPageInfo.remote_udp_port = atoi(pStrPort);
        tPageInfo.paging_channel = pInfo->iChannel;
    }
    return tPageInfo;
}


int CMulticastPage::GetCodecIDByName(char* pCodec)
{
    if (0 == strcmp(pCodec, "PCMU"))
    {
        return 0;
    }
    else if (0 == strcmp(pCodec, "PCMA"))
    {
        return 8;
    }
    else if (0 == strcmp(pCodec, "G729"))
    {
        return 18;
    }
    else if (0 == strcmp(pCodec, "G722"))
    {
        return 9;
    }
    else if (0 == strcmp(pCodec, "G726-16"))
    {
        return 103;
    }
    else if (0 == strcmp(pCodec, "G726-24"))
    {
        return 104;
    }
    else if (0 == strcmp(pCodec, "G726-32"))
    {
        return 102;
    }
    else if (0 == strcmp(pCodec, "G726-40"))
    {
        return 105;
    }
    else if (0 == strcmp(pCodec, "G723_53"))
    {
        return 4;
    }
    else if (0 == strcmp(pCodec, "iLBC_13_3"))
    {
        return 106;
    }
    return 9;
}

const char* CMulticastPage::GetCodecNameByID(int iID)
{
    switch (iID)
    {
    case 0:
        return "PCMU";
    case 8:
        return "PCMA";
    case 18:
        return "G729";
    case 9:
        return "G722";
    case 103:
        return "G726-16";
    case 104:
        return "G726-24";
    case 102:
        return "G726-32";
    case 105:
        return "G726-40";
    case 4:
        return "G723_53";
    case 106:
        return "iLBC_13_3";
    default:
        return "G722";
    }
    return "G722";
}

int CMulticastPage::GetPriorityByIP(char* pStrIP, int iPort, int iChannel)
{
    if (pStrIP != NULL)
    {
        yl::string strPage = commonAPI_FormatString(_T("%s:%d"), pStrIP, iPort);
        list_page::iterator it = m_pListPage.begin();
        for (; it != m_pListPage.end(); ++it)
        {
            RECPage_Info* pInfo = (RECPage_Info*)((*it));
            if (pInfo != NULL
                    && pInfo->strValue == strPage
                    && pInfo->iChannel == iChannel)
            {
                return pInfo->iPriority;
            }
        }
    }
    return 0;
}

void CMulticastPage::PrintPage(PAGING_PARAM* pInfo)
{
    if (pInfo != NULL)
    {
        TALK_INFO("paging_in = %d,payload_type = %d,codec_name = %s", pInfo->paging_in, pInfo->payload_type,
                  pInfo->codec_name);
        TALK_INFO("local_ip_address = %s,local_udp_port = %d", pInfo->local_ip_address,
                  pInfo->local_udp_port);
        TALK_INFO("remoteIp = %s, udpPort = %d", pInfo->remote_ip_address, pInfo->remote_udp_port);
        TALK_INFO("request_ip_address = %s", pInfo->request_ip_address);
        TALK_INFO("paging_channel = %d", pInfo->paging_channel);
    }
}

// 打开或关闭已经处于通话中的Paging。
bool CMulticastPage::PagingTalkSwitch(int iCallID, bool bStart)
{
    CSingleSession* pSession = _SessionManager.GetRelateSessionByCallID(iCallID);
    if (pSession == NULL || !pSession->BeMulticastPage())
    {
        TALK_WARN("Not multicast paging session");
        return false;
    }

    // 呼出参数
    CCallInfo* pCallInfo = pSession->GetCallInfo();
    if (pCallInfo == NULL || NULL == pCallInfo->m_pPageParam)
    {
        TALK_WARN("The CallInfo of the paging session is invalid");
        return false;
    }

    // 开启通话
    if (bStart)
    {
        StartPagingTalk(pCallInfo->m_pPageParam);
        return true;
    }

    // 设置停Paging类型
    // STOPPAGE eStopType = pCallInfo->m_eStopPageType;
    if (pSession->BePageOut())
    {
        TALK_INFO("Stop Call Out Paging ! StopType =>> SP_STOP");
        pCallInfo->m_eStopPageType = SP_STOP;
    }

    StopPagingTalk(pCallInfo->m_pPageParam, pCallInfo->m_eStopPageType);

    // !!如果当前有处于通话的Paging,则需要从新开启该Paging
    _SessionManager.RestartOtherTalkPaging(pSession->GetSessionID());

    return true;
}

// 打开或关闭已经处理通话中的Paging
bool CMulticastPage::StartPagingTalk(PAGING_PARAM* param)
{
    if (param == NULL)
    {
        return false;
    }

    PrintPage(param);
    return (paging_start(param) == 0);
}

bool CMulticastPage::StopPagingTalk(PAGING_PARAM* param, STOPPAGE eType)
{
    if (param == NULL)
    {
        return false;
    }
    PrintPage(param);

    return (paging_stop(param, eType) == 0);
}

void CMulticastPage::AddWaitPage(PAGING_PARAM* param)
{
    if (param != NULL)
    {
        PAGING_PARAM* pNew = new PAGING_PARAM;
        chASSERT(pNew != NULL);
        memcpy(pNew, param, sizeof(PAGING_PARAM));
        m_pListWait.push_back(pNew);

        if (m_pListWait.size() > 1)
        {
            Sort(m_pListWait, ComparePriority);
        }
    }
}

void CMulticastPage::RemoveWaitPage(PAGING_PARAM* param)
{
    if (param != NULL)
    {
        list_wait_page::iterator it = m_pListWait.begin();
        for (; it != m_pListWait.end(); ++it)
        {
            PAGING_PARAM* pWait = (PAGING_PARAM*)((*it));
            if (param->remote_udp_port == pWait->remote_udp_port
                    && 0 == strcmp(param->remote_ip_address, pWait->remote_ip_address)
                    && param->paging_channel == pWait->paging_channel)
            {
                delete pWait;
                m_pListWait.erase(it);
                return;
            }
        }
    }
}

bool CMulticastPage::RestartWaitPage()
{
    TALK_WARN("Restart Wait Paging");
    int iWaitNum = m_pListWait.size();
    for (int i = 0; i < iWaitNum; i++)
    {
        list_wait_page::iterator it = m_pListWait.begin();
        PAGING_PARAM* pWait = (PAGING_PARAM*)((*it));
        m_pListWait.erase(it);
        ReceiveCallInPage(pWait);
        delete pWait;
    }
    return true;
}

// 网络状态改变消息处理
void CMulticastPage::OnNetworkChange(bool bConnectOk/* = true*/)
{
    TALK_INFO("CMulticastPage::OnNetworkChange bConnectOk=%d", bConnectOk);
    if (bConnectOk)
    {
        InitListenInfo();
    }
    else
    {
        // 去掉监控组播前，需要将监控组播正在进行的通话去掉
        HangUpAllRecPaging();
        RemoveAllListenPaging();
        ClearPageList();
    }
}

// 去掉所有的监听Paging
void CMulticastPage::RemoveAllListenPaging()
{
    list_page::iterator it = m_pListPage.begin();
    for (; it != m_pListPage.end(); ++it)
    {
        PAGING_PARAM tPageInfo = GetPageInfo((RECPage_Info*)((*it)));
        PrintPage(&tPageInfo);
        paging_remove_listen(&tPageInfo);
    }
}

// 退出所有监听的组播账号
void CMulticastPage::HangUpAllRecPaging()
{
    list_page::iterator it = m_pListPage.begin();
    for (; it != m_pListPage.end(); ++it)
    {
        PAGING_PARAM tPageInfo = GetPageInfo((RECPage_Info*)((*it)));
        BreakOffCallInPage(&tPageInfo);
    }
}

void CMulticastPage::CheckChannel(int& iChannel)
{
    if (iChannel < 0
            || iChannel > 30)
    {
        iChannel = 0;
    }
}

#endif //IF_FEATURE_PAGING
