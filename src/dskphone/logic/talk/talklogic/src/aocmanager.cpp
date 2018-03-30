#include "talklogic_inc.h"

IMPLEMENT_GETINSTANCE(CAocManager)

// 接收SIP下发的消息
static LRESULT OnMessage(msgObject& msg)
{
    switch (msg.message)
    {
    case TM_TIMER:
        {
            _AOCManager.OnTimer(msg.wParam);
        }
        break;
    case SIP_CALL_INFORMATION_UPDATE:
        {
            sip_call_infomation_header_t* pInfo = (sip_call_infomation_header_t*)msg.GetExtraData();

            if (NULL != pInfo && SIP_CALL_INFORMATION_DISPLAY_TEXT == pInfo->type)
            {
                _AOCManager.ProcessAocNotify(msg.wParam, msg.lParam, pInfo->payload);
            }
        }
        break;
    default:
        break;
    }

    return 1;
}

CAocManager::CAocManager(void)
{
}

CAocManager::~CAocManager(void)
{
    m_lstTalkInfo.clear();
}

// 初始化
void CAocManager::Init()
{
    SingleMsgReg(TM_TIMER, OnMessage);
    SingleMsgReg(SIP_CALL_INFORMATION_UPDATE, OnMessage);
}

// 是否显示AOC
bool CAocManager::IsAocShow(int iCallId)
{
    DataTalkInfo* pTalkData = GetTalkDurationInfoByCallID(iCallId);
    if (NULL == pTalkData)
    {
        return false;
    }

    return pTalkData->bShowAoc;
}

// 获取AOC信息
bool CAocManager::GetAocInfo(DataAoc& tAocData, int iCallId)
{
    DataTalkInfo* pTalkData = GetTalkDurationInfoByCallID(iCallId);
    if (NULL == pTalkData)
    {
        return false;
    }

    tAocData = pTalkData->tAocData;
    return true;
}

// 是否可用
bool CAocManager::IsAocEnable()
{
    return (0 != configParser_GetConfigInt(kszAOCEnable));
}

// 通话开启的处理
bool CAocManager::ProcessTalkStart(int iCallId)
{
    // 已存在, 不处理
    if (NULL != GetTalkDurationInfoByCallID(iCallId))
    {
        return false;
    }

    // 话路不存在, 不处理
    CBaseSession* pBaseSession = _SessionManager.GetSessionByCallID(iCallId);
    if (NULL == pBaseSession)
    {
        return false;
    }

    DataTalkInfo tTalkDurData;
    tTalkDurData.iCallId = iCallId;
    tTalkDurData.iTimeOut = 3000;
    tTalkDurData.bShowAoc = false;
    m_lstTalkInfo.push_back(tTalkDurData);
    return true;
}

// 清除AOC数据
void CAocManager::ClearAOCData(int iCallId)
{
    // 不存在, 不处理
    IterDataTalkInfo itr = m_lstTalkInfo.begin();
    for (; itr != m_lstTalkInfo.end(); ++itr)
    {
        if ((*itr).iCallId == iCallId)
        {
            timerKillThreadTimer((*itr).iTimeOut);
            m_lstTalkInfo.erase(itr);
            break ;
        }
    }
}

// 通话结束的处理
bool CAocManager::ProcessTalkFinish(int iCallId)
{
    // 不存在, 不处理
    DataTalkInfo* pTalkData = GetTalkDurationInfoByCallID(iCallId);
    if (NULL == pTalkData)
    {
        return false;
    }

    // 话路存在, 更新时长
    CBaseSession* pBaseSession = _SessionManager.GetSessionByCallID(iCallId);
    if (NULL != pBaseSession && NULL != pBaseSession->GetCallInfoAsSub())
    {
        pTalkData->tAocData.iTimeDuration = pBaseSession->GetCallInfoAsSub()->m_iCallDuration;
    }

    // 开启定时器
    timerSetThreadTimer((UINT)&pTalkData->iTimeOut, pTalkData->iTimeOut);
    return true;
}

// 处理定时器消息
bool CAocManager::OnTimer(UINT uTimerID)
{
    bool bHandle = false;
    IterDataTalkInfo itr = m_lstTalkInfo.begin();
    while (itr != m_lstTalkInfo.end())
    {
        if (uTimerID == (UINT) & (*itr).iTimeOut)
        {
            timerKillThreadTimer(uTimerID);
            itr = m_lstTalkInfo.erase(itr);
            bHandle = true;
            continue ;
        }

        ++itr;
    }

    return bHandle;
}

// 处理AocNotify
bool CAocManager::ProcessAocNotify(int iAccId, int iCallId, const char* lpInfo)
{
    if (NULL == lpInfo)
    {
        return false;
    }

    if (!IsAocEnable())
    {
        return false;
    }

    // 缓存数据已被清除, 不处理
    DataTalkInfo* pTalkDuration = GetTalkDurationInfoByCallID(iCallId);
    if (NULL == pTalkDuration)
    {
        return false;
    }

    // 解析数据
    if (!ParserAocInfo(lpInfo, pTalkDuration->tAocData))
    {
        return false;
    }

    pTalkDuration->bShowAoc = true;

    // 具体业务处理
    CBaseSession* pSession = _SessionManager.GetSessionByCallID(iCallId);
    if (NULL == pSession)
    {
        timerKillThreadTimer((UINT)&pTalkDuration->iTimeOut);
        return EnterAocFinishRoutine(iAccId, iCallId, pTalkDuration->tAocData);
    }

    if (SESSION_TALKING == pSession->GetSessionState()
            || SESSION_AOC_TALKING == pSession->GetSessionState())
    {
        //pTalkDuration->tAocData.iTimeDuration = pSession->GetTalkDurationSec();
        return true;
    }

    return false;
}

// 解析Aoc
bool CAocManager::ParserAocInfo(const yl::string& strInfo, DataAoc& tAocData)
{
    xml_document doc;
    xml_parse_result docRet = doc.load_buffer(strInfo.c_str(), strInfo.size());

    // 解析失败
    if (status_ok != docRet.status)
    {
        return false;
    }

    // 获取节点Aoc
    xml_node nodeAoc = doc.child("aoc");
    if (nodeAoc.empty())
    {
        return false;
    }

    // 处理AOC-D或AOC-E的情况
    xml_node nodeAocD = nodeAoc.child("aoc-d");
    xml_node nodeAocE = nodeAoc.child("aoc-e");
    if (nodeAocD.empty() && nodeAocE.empty())
    {
        return false;
    }

    xml_node nodeInfo = nodeAocD.empty() ? nodeAocE : nodeAocD;

    // 解析各个字段
    nodeInfo = nodeInfo.first_child();
    while (!nodeInfo.empty())
    {
        const char* lpszName = nodeInfo.name();
        if (strstr(lpszName, "charging-info") != NULL)
        {
            tAocData.strSpendType = nodeInfo.text().get();
        }
        else if (strstr(lpszName, "billing-id") != NULL)
        {
            tAocData.strBillingMode = nodeInfo.text().get();
        }
        else if (strstr(lpszName, "recorded-charges") != NULL)
        {
            // 获取recorded-currency-units节点
            xml_node nodeUnits = nodeInfo.child("recorded-currency-units");
            if (nodeUnits.empty())
            {
                nodeInfo = nodeInfo.next_sibling();
                continue ;
            }

            xml_node nodeId = nodeUnits.child("currency-id");
            if (!nodeId.empty())
            {
                tAocData.strMonetaryUnit = nodeId.text().get();
            }

            xml_node nodeAmount = nodeUnits.child("currency-amount");
            if (!nodeAmount.empty())
            {
                tAocData.strBill = nodeAmount.text().get();
            }
        }

        nodeInfo = nodeInfo.next_sibling();
    }

    // 容错处理
    if (tAocData.strSpendType.empty())
    {
        tAocData.strSpendType = "total";
    }

    return (!tAocData.strBill.empty() && !tAocData.strMonetaryUnit.empty());
}

// 进入AocFinishRoutine
bool CAocManager::EnterAocFinishRoutine(int iAccId, int iCallId, const DataAoc& tAocData)
{
    // 账号不可用, 不处理
    if (!acc_IsAccountIDValid(iAccId))
    {
        return false;
    }

    // 网络异常, 不处理
    if (!netIsNetworkReady())
    {
        return false;
    }

    // 加入Session
    CSingleSession* pSession = _SessionManager.AllocSession(iCallId);
    if (pSession == NULL)
    {
        return false;
    }

    // 满足以上条件则建立通话
    CCallInfo* pCallInfo = pSession->GetCallInfo();
    pCallInfo->m_eCallType = CT_AOC_NOTIFY;
    pCallInfo->SetAccountID(iAccId);

    TALK_INFO("ROUTINE_AOC_FINISH(%#x), Ringing", iCallId);
    pSession->EnterRoutine(ROUTINE_AOC_FINISH);
    return true;
}

// 通过CallId找通话信息
DataTalkInfo* CAocManager::GetTalkDurationInfoByCallID(int iCallId)
{
    IterDataTalkInfo itr = m_lstTalkInfo.begin();
    for (; itr != m_lstTalkInfo.end(); ++itr)
    {
        if ((*itr).iCallId == iCallId)
        {
            return &(*itr);
        }
    }

    return NULL;
}
