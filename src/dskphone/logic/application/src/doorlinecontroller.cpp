#include "app_inc.h"

#ifdef IF_FEATURE_ENTRANCE_GUARD

#define Default_Update_Time  5000  // 默认刷新时间5s

#define Max_Intervar  600.0f     // 最大间隔600s
#define Min_Intervar  0.299f     // 最小间隔300ms

#define kszLocalFilePrefix  TEMP_PATH "EntranceGuard_"

using namespace NS_TA;

extern CSessionProxy GetCurrentDoorLineSession();

IMPLEMENT_GETINSTANCE(CEntranceGuardLogic)

CEntranceGuardLogic::EntranceGuardInfo::EntranceGuardInfo(int iTraceId,
        const yl::string& strDoorlineDtmf, const yl::string& strstrDoorlineUrl)
    : bInLoading(false), bActived(false),
      iTryCount(0), bAutoFlow(false), strDTMF(strDoorlineDtmf), strUrl(strstrDoorlineUrl)
{
    strLocalFile = commonAPI_FormatString(kszLocalFilePrefix"%d", iTraceId);
}

CEntranceGuardLogic::EntranceGuardInfo::~EntranceGuardInfo()
{
    // 释放本地缓存图片资源
    if (pathFileExist(strLocalFile.c_str()))
    {
        remove(strLocalFile.c_str());
    }
}


CEntranceGuardLogic::CEntranceGuardLogic() : m_iFocusId(INVALID_SESSION_ID),
    m_iRefreshInterval(Default_Update_Time)
{
    OnConfigChange();

    etl_RegisterMsgHandle(TM_TIMER, TM_TIMER, CEntranceGuardLogic::OnMessage);
    etl_RegisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED,
                          CEntranceGuardLogic::OnMessage);
}

CEntranceGuardLogic::~CEntranceGuardLogic()
{
    etl_UnregisterMsgHandle(TM_TIMER, TM_TIMER, CEntranceGuardLogic::OnMessage);
    etl_UnregisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED,
                            CEntranceGuardLogic::OnMessage);
}

// 规则如下：
// 1. 有来电，取最后一路来电
// 2. 手动切换焦点之后，只显示当前通话(除非当前有来电)
// 3. 本地会议，只显示一路，第一路优先级高

// 优先级如下：
// 最后一路来电 > 来电 > 通话 > 普通通话

LRESULT CEntranceGuardLogic::OnMessage(msgObject& refObj)
{
    return g_refEntranceGuard.OnMessgeProcess(refObj);
}

bool CEntranceGuardLogic::OnMessgeProcess(msgObject& refObj)
{
    BOOL bHanlded = TRUE;
    switch (refObj.message)
    {
    case CONFIG_MSG_BCAST_CHANGED:
        {
            if (refObj.wParam == ST_DOORPHONE)
            {
                OnConfigChange();
            }
        }
        break;
    case TM_TIMER:
        {
            return OnTimer(refObj.wParam);
        }
        break;
    default:
        bHanlded = FALSE;
        break;
    }

    return bHanlded;
}

bool CEntranceGuardLogic::DealSessionDestory(int iTaskId)
{
    EDoorLineAction eAction = EDLA_DELETE;
    return OnAction(eAction, iTaskId);
}

bool CEntranceGuardLogic::DealTalkHide(int iTaskId, bool bHide)
{
    bool bRet = false;
    if (bHide)
    {
        bRet = OnAction(EDLA_PAUSE, iTaskId);
    }
    else
    {
        // 会议中，退出Sub session时，先设置焦点，再取消隐藏
        if (iTaskId == GetCurrentDoorLineSession())
        {
            bRet = OnAction(EDLA_START, iTaskId);
        }
    }
    return bRet;
}

bool CEntranceGuardLogic::DealRingingEnter(int iTaskId)
{
    return OnAction(EDLA_START, iTaskId);
}

bool CEntranceGuardLogic::DealFocusChanged(int iTaskId, bool bFocusIn)
{
    bool bRet = false;
    if (IsTaskIdValid(iTaskId))
    {
        EDoorLineAction eAction = bFocusIn ? EDLA_START : EDLA_PAUSE;
        bRet = OnAction(eAction, iTaskId);
    }
    return bRet;
}

bool CEntranceGuardLogic::DealDtmfFinshed(int iTaskId)
{
    bool  bRet = false;
    mapSessionInfoIt it = m_mapInfo.find(iTaskId);
    if (it != m_mapInfo.end())
    {
        if (it->second.bAutoFlow)
        {
            timerSetThreadTimer(TIMER_ID(it->first), 500);
        }
    }
    return bRet;
}

bool CEntranceGuardLogic::DealTalkEnter(int iTaskId)
{
    bool bRet = false;
    mapSessionInfoIt it = m_mapInfo.find(iTaskId);
    if (it != m_mapInfo.end())
    {
        if (it->second.bAutoFlow)
        {
            int iCallID = talklogic_GetCallIdBySessionID(iTaskId);

            bRet = talklogic_SendDtmf(iCallID, it->second.strDTMF, false);

            CSessionStateProxy proxy;
            if (!(proxy == iTaskId))
            {
                bRet = StartById(proxy);
            }
        }
    }
    return bRet;
}

bool CEntranceGuardLogic::SetAutoAnswer(int iTaskId, bool bAuto/* = false*/)
{
    mapSessionInfoIt it = m_mapInfo.find(iTaskId);
    if (it != m_mapInfo.end())
    {
        if (bAuto)
        {
            OnAction(EDLA_PAUSE, iTaskId);
        }

        it->second.bAutoFlow = bAuto;

        return true;
    }

    return false;
}

bool CEntranceGuardLogic::IsAutoAnswer(int iTaskId)
{
    mapSessionInfoIt it = m_mapInfo.find(iTaskId);
    if (it != m_mapInfo.end())
    {
        return it->second.bAutoFlow;
    }

    return false;
}

void CEntranceGuardLogic::OnTaskActionCallback(CTaskAction* pTaskAction)
{
    if (pTaskAction != NULL)
    {
        TaskActionErrorType eErrorType = pTaskAction->GetTaskActionErrorType();

        CNetworkFileAction* pAction = static_cast<CNetworkFileAction*>(pTaskAction);
        if (pAction == NULL)
        {
            return;
        }

        int iTaskId = pAction->GetExtraParam();
        mapSessionInfoIt it = m_mapInfo.find(iTaskId);
        if (it != m_mapInfo.end())
        {
            APP_INFO("CEntranceGuardLogic::OnTaskActionCallback eError=[%d] task=[%d] focus=[%d] active=[%d]",
                     eErrorType, iTaskId, m_iFocusId, it->second.bActived);

            it->second.bInLoading = false;

            if (iTaskId == m_iFocusId && it->second.bActived)
            {
                if (eErrorType == TA_NO_ERROR)
                {
                    it->second.iTryCount = 0;
                    EntranceGuardMessageDefine MsgId = ENTRANCE_GUARD_MESSAGE_REFRESH;
                    yl::string& strFile = it->second.strLocalFile;

                    etl_NotifyAppEx(false, MsgId, m_iFocusId, eErrorType, strFile.size() + 1, strFile.c_str());
                }
                else
                {
                    ++(it->second.iTryCount);
                    NotifyFailMsg(iTaskId, eErrorType);
                }
            }
            else
            {
                StopById(iTaskId);
            }
        }
    }
}

bool CEntranceGuardLogic::OnTimer(UINT uTimerId)
{
    for (CEntranceGuardLogic::mapSessionInfoIt it = m_mapInfo.begin(); it != m_mapInfo.end(); ++it)
    {
        if (TIMER_ID(it->first) == uTimerId)
        {
            if (it->second.bAutoFlow)
            {
                talklogic_ExitSession(talklogic_GetCallIdBySessionID(it->first));
            }
            else if (it->second.Failed())
            {
                StopById(it->first);
            }
            else
            {
                ExeLoad(*it);
            }

            return true;
        }
    }

    return false;
}

bool CEntranceGuardLogic::ExeLoad(const mapValueType& mapItem)
{
    const item_value_type& item = mapItem.second;
    const int iTaskId = mapItem.first;

    if (item.strUrl.empty())
    {
        APP_WARN("CEntranceGuardLogic::ExeLoad url empty");
        return false;
    }
    else if (item.bInLoading)
    {
        APP_WARN("CEntranceGuardLogic::ExeLoad no need to exe load");
        return true;
    }

    APP_INFO("CEntranceGuardLogic::ExeLoad load task=[%d]", iTaskId);

    CNetworkFileAction* pAction =
        TaskAction_CreateNetworkFileAction(item.strLocalFile.c_str(), item.strUrl.c_str());
    if (pAction != NULL)
    {
        pAction->SetCheckUrl(false);
        pAction->SetSync(false);
        pAction->SetRead(true);
        pAction->SetCallback(this);
        pAction->SetExtraParam(iTaskId);
    }

    return TaskAction_ExecAction(pAction);
}

bool CEntranceGuardLogic::StartById(int iTaskId)
{
    if (iTaskId == INVALID_SESSION_ID)
    {
        return false;
    }

    APP_INFO("CEntranceGuardLogic::StartById task=[%d] focus=[%d]", iTaskId, m_iFocusId);
    // 先停掉当前Task
    if (m_iFocusId != iTaskId)
    {
        StopById(m_iFocusId);
    }

    mapSessionInfoIt it = m_mapInfo.find(iTaskId);
    if (it != m_mapInfo.end())
    {
        APP_INFO("CEntranceGuardLogic::Start task=[%d] active=[%d] try=[%d]",
                 it->first, it->second.bActived, it->second.iTryCount);

        // 已经开始了或者已经失败了
        if (it->second.bActived || it->second.Failed())
        {
            return false;
        }

        if (ExeLoad(*it))
        {
            it->second.bActived = true;
            it->second.bInLoading = true;
            m_iFocusId = it->first;

            timerSetThreadTimer(TIMER_ID(it->first), m_iRefreshInterval);

            if (it->second.iTryCount == 0)
            {
                etl_NotifyApp(false, ENTRANCE_GUARD_MESSAGE_START, it->first, 0);
            }

            return true;
        }
    }

    return false;
}

bool CEntranceGuardLogic::StopById(int iTaskId, bool bRemoveTask/* = false*/)
{
    if (iTaskId == INVALID_SESSION_ID)
    {
        return false;
    }

    APP_INFO("CEntranceGuardLogic::StopById task=[%d] remove=[%d]", iTaskId, bRemoveTask);

    bool bRet = false;
    mapSessionInfoIt it = m_mapInfo.find(iTaskId);
    if (it != m_mapInfo.end())
    {
        // 焦点Task被停掉，需要将焦点设为非法值
        if (m_iFocusId == it->first)
        {
            m_iFocusId = INVALID_SESSION_ID;
        }

        if (it->second.bActived)
        {
            EntranceGuardMessageDefine MsgId = bRemoveTask
                                               ? ENTRANCE_GUARD_MESSAGE_STOP : ENTRANCE_GUARD_MESSAGE_PAUSE;

            etl_NotifyApp(false, MsgId, it->first, 0);
        }

        if (it->second.bActived || bRemoveTask)
        {
            timerKillThreadTimer(TIMER_ID(it->first));
            it->second.bActived = false;
        }

        if (bRemoveTask)
        {
            m_mapInfo.erase(it);
        }

        bRet = true;
    }

    if (bRemoveTask)
    {
        // 停止通话后，手动查找下一路需要显示的
        int iNeedActiveTask = GetCurrentDoorLineSession();
        APP_INFO("CEntranceGuardLogic::StopById need to active=[%d]", iNeedActiveTask);

        if (iTaskId != m_iFocusId)
        {
            StartById(iNeedActiveTask);
        }
    }

    return bRet;
}

bool CEntranceGuardLogic::NotifyFailMsg(int iTaskId, TaskActionErrorType eErrorType)
{
    mapSessionInfoIt it = m_mapInfo.find(iTaskId);
    if (it != m_mapInfo.end())
    {
        if (it->second.Failed())
        {
            // 3s 之后自动退出失败提示
            timerSetThreadTimer(TIMER_ID(it->first), 3 * 1000);
        }
        else
        {
            EntranceGuardMessageDefine MsgId = TA_HTTP_ERR408 == eErrorType
                                               ? ENTRANCE_GUARD_MESSAGE_TIME_OUT : ENTRANCE_GUARD_MESSAGE_ERROR;

            etl_NotifyApp(false, MsgId, iTaskId, eErrorType);
        }

        return true;
    }

    return false;
}

bool CEntranceGuardLogic::CreateTask(int iTaskId, const yl::string& strDtmf,
                                     const yl::string& strUrl)
{
    APP_INFO("CEntranceGuardLogic::CreateTask task=[%d] dtmf=[%s] url=[%s]", iTaskId, strDtmf.c_str(),
             strUrl.c_str());
    if (strUrl.empty())
    {
        return false;
    }

    if (!IsTaskIdValid(iTaskId))
    {
        m_mapInfo[iTaskId] = item_value_type(iTaskId, strDtmf, strUrl);
        return true;
    }

    return false;
}

bool CEntranceGuardLogic::OnAction(EDoorLineAction eAction, int iTaskId)
{
    bool bHanled = true;
    switch (eAction)
    {
    case EDLA_START:
        bHanled = StartById(iTaskId);
        break;
    case EDLA_DELETE:
    case EDLA_PAUSE:
        bHanled = StopById(iTaskId, eAction == EDLA_DELETE);
        break;
    default:
        bHanled = false;
        break;
    }

    return bHanled;
}

yl::string CEntranceGuardLogic::GetDTMFString(int iTaskId)
{
    mapSessionInfoIt it = m_mapInfo.find(iTaskId);
    if (it != m_mapInfo.end())
    {
        return it->second.strDTMF;
    }

    return yl::string();
}

bool CEntranceGuardLogic::IsTaskIdValid(int iTaskId)
{
    return m_mapInfo.find(iTaskId) != m_mapInfo.end();
}

void CEntranceGuardLogic::OnConfigChange()
{
    double dIntervar = GetConfigIntervar();

    m_iRefreshInterval = (dIntervar > Min_Intervar
                          && dIntervar < Max_Intervar) ? (int)(dIntervar * 1000) : Default_Update_Time;

    APP_INFO("CEntranceGuardLogic::OnConfigChange update interval=[%d]", m_iRefreshInterval);
}

bool CEntranceGuardLogic::IsEnable()
{
    return GetConfigIntervar() >= Min_Intervar;
}

double CEntranceGuardLogic::GetConfigIntervar()
{
    return strtod(configParser_GetConfigString(kszCameraURLRefreshInterval).c_str(), 0);
}


#endif
