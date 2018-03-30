#include "statusmanager.h"
#include "idlescreen_inc.h"

#ifdef IF_FEATURE_IDLESCREEN

//////////////////////////////////////////////////////////////////////////
// Class CStatusManager.
// 默认获取的状态数量.
static const int knDefRetrievedStatusCount = 5;
#define  IF_IDLE_SUPPORT_NOTIFY_LIMIT 1

CStatusManager & CStatusManager::GetInstance()
{
    static CStatusManager cStatusManager;
    return cStatusManager;
}


CStatusManager::CStatusManager()
    : m_isOn(false),
      m_NotifyTimer((UINT) & m_NotifyTimer),
      m_iNeedNotifyType(0),
      m_bTimerStatus(false)

{
    m_listStatus.clear();
    m_vecNotifyFunc.clear();

    etl_RegisterMsgHandle(TM_TIMER, TM_TIMER, &CStatusManager::OnTimerMsgProgress);
}

CStatusManager::~CStatusManager()
{
    timerKillThreadTimer(m_NotifyTimer);
    etl_UnregisterMsgHandle(TM_TIMER, TM_TIMER, &CStatusManager::OnTimerMsgProgress);
}

LRESULT CStatusManager::OnTimerMsgProgress(msgObject & msg)
{
    BOOL bRet = FALSE;
    if (msg.message == TM_TIMER)
    {
        bRet = _StatusManager.OnTimer(msg.wParam);
    }
    return bRet;
}

BOOL CStatusManager::OnTimer(UINT iTimerId)
{
    if (iTimerId == m_NotifyTimer)
    {
        timerKillThreadTimer(m_NotifyTimer);
        m_bTimerStatus = false;
        UpdateNotifyStatus();
        return TRUE;
    }
    return FALSE;
}

bool CStatusManager::IsTimerRuning()
{
    return (m_bTimerStatus == true);
}

bool CStatusManager::SetStatusChangeCallback(int nStstus, StatusChangeFunction funcCallback)
{
    StatusConfigItem * pConfigItem = GetConfigItem(nStstus);
    if (pConfigItem != NULL)
    {
        pConfigItem->funcChange = funcCallback;
    }

    return false;
}

CStatusManager::StatusConfigItem * CStatusManager::GetConfigItem(int nStatus)
{
    StatusConfigureMap::iterator iterConfig = m_mapStatusCfg.find(nStatus);
    if (iterConfig != m_mapStatusCfg.end())
    {
        return &iterConfig->second;
    }
    return NULL;
}


void CStatusManager::TurnNotify(bool bNotified /*= true*/)
{
    m_isOn = bNotified;

#ifndef IF_NOTIFICATION_CENTER_ON_IDLE
    if (!GetIsXwin())
    {
        // 赋值为最大值
        UpdateNotifyState(false, POPUP_WINDOW_TYPE_MAX, true);
    }
#endif

    if (bNotified == true)
    {
        // 开启之后立即更新一次.
        NotifyStatusChange();
    }
}


void CStatusManager::AddStatus(int nStatus, const yl::string & strHint /*= ""*/)
{
    COMMONUNITS_INFO("CStatusManager: add %d, %s", nStatus, strHint.c_str());
    // 获取配置项.
    StatusConfigItem * pCfgItem = GetConfigItem(nStatus);
    if (pCfgItem == NULL)
    {
        return;
    }

    bool bFlush = false;
    if (GetIsXwin())
    {
        bFlush = RemoveStatus(nStatus, true);

        // 再插入状态.
        IdleStatusItem stSI;
        stSI.m_nId = nStatus;
        stSI.m_strHint = strHint;

        // 首次增加时, 状态类型由注册时候决定
        VecNotifyTypePriority::iterator iter = pCfgItem->m_vecPriotify.begin();
        for (; iter != pCfgItem->m_vecPriotify.end(); ++iter)
        {
            NotifyTypePriority TypePriority = (*iter);
            stSI.m_vecTypePriority.push_back(TypePriority);
            m_iNeedNotifyType |= TypePriority.m_eNotifyType;
        }

        m_listStatus.push_back(stSI);
    }
#ifndef IF_NOTIFICATION_CENTER_ON_IDLE
    else
    {
        // 如果是固定状态组直接更改.
        if (pCfgItem->m_nLoc != 0)
        {
            AddFixedStatus(nStatus, strHint);
        }
        else
        {
            // 非固定状态, 首先删除状态,删除用于刷新，不进行状态通知.
            bFlush = RemoveStatus(nStatus, true);

            // 再插入状态.
            IdleStatusItem stSI;
            stSI.m_nId = nStatus;
            stSI.m_strHint = strHint;

            // 首次增加时, 是否弹出由配置决定.
            stSI.m_nPopupType = pCfgItem->m_nPopupType;

            // 用于强制刷新
            UpdateNotifyState(true, pCfgItem->m_nPopupType, bFlush);

            m_listStatus.push_front(stSI);
        }
    }
#endif // IF_NOTIFICATION_CENTER_ON_IDLE

    if (pCfgItem->funcChange != NULL)
    {
        pCfgItem->funcChange(bFlush ? STATUS_ACTION_UPDATE : STATUS_ACTION_ADD); //增加
    }
}


bool CStatusManager::RemoveStatus(int nStatus, bool bFlush/* = false*/)
{
    COMMONUNITS_INFO("CStatusManager: remove %d", nStatus);
    StatusConfigItem * configItem = GetConfigItem(nStatus);
    if (configItem == NULL)
    {
        return false;
    }

    if (GetIsXwin())
    {
        // 删除状态
        NonFixedStatusList::iterator iterItem = m_listStatus.begin();
        while (iterItem != m_listStatus.end())
        {
            IdleStatusItem & statusItem = (*iterItem);
            if (nStatus == statusItem.m_nId)
            {
                VecNotifyTypePriority::iterator iter = statusItem.m_vecTypePriority.begin();
                for (; iter != statusItem.m_vecTypePriority.end(); ++iter)
                {
                    NotifyTypePriority TypePriority = (*iter);
                    m_iNeedNotifyType |= TypePriority.m_eNotifyType;
                }
                m_listStatus.erase(iterItem);
                //状态变化回调, 如果只是刷新状态则不通知
                if (!bFlush
                        && configItem->funcChange != NULL)
                {
                    configItem->funcChange(STATUS_ACTION_DELETE);
                }
                return true;
            }
            ++iterItem;
        }

        return false;
    }

#ifndef IF_NOTIFICATION_CENTER_ON_IDLE
    if (configItem->m_nLoc != 0) // 删除固定位置状态
    {
        if (configItem->m_nLoc <= m_vecFixedStatus.size())
        {
            if (m_vecFixedStatus[configItem->m_nLoc - 1].m_nId != PS_STATE_NONE)
            {
                m_vecFixedStatus[configItem->m_nLoc - 1].m_nId = PS_STATE_NONE;
                //TODO. 需要通过返回值来判断是否刷新
                UpdateNotifyState(false, m_vecFixedStatus[configItem->m_nLoc - 1].m_nPopupType);

                //状态变化回调
                if (!bFlush
                        && configItem->funcChange != NULL)
                {
                    configItem->funcChange(STATUS_ACTION_DELETE);
                }
                return true;
            }
        }
    }
    else
    {
        // 删除非固定状态
        NonFixedStatusList::iterator iterItem = m_listStatus.begin();
        while (iterItem != m_listStatus.end())
        {
            if (nStatus == (*iterItem).m_nId)
            {
                //TODO. 需要通过返回值来判断是否刷新
                UpdateNotifyState(false, (*iterItem).m_nPopupType);

                m_listStatus.erase(iterItem);
                //状态变化回调, 如果只是刷新状态则不通知
                if (!bFlush
                        && configItem->funcChange != NULL)
                {
                    configItem->funcChange(STATUS_ACTION_DELETE);
                }
                return true;
            }
            ++iterItem;
        }
    }
#endif

    return false;
}


bool CStatusManager::RegisterStatus(int nStatus, const StatusConfigItem & sConfig)
{
    //加入map中
    m_mapStatusCfg.insert(nStatus, sConfig);

#ifndef IF_NOTIFICATION_CENTER_ON_IDLE
    if (!GetIsXwin() && sConfig.m_nLoc != 0) //提前分配固定组内存
    {
        ExpandYLVector(m_vecFixedStatus, sConfig.m_nLoc);
    }
#endif

    return m_mapStatusCfg.count(nStatus) != 0;
}

bool CStatusManager::NotifyStatusChange()
{
    UpdateNotifyStatus();

    return true;
}

bool CStatusManager::UpdateNotifyStatus()
{
    if (GetIsXwin())
    {
        //通知UI更新状态信息
        if (m_isOn)
        {
            unsigned iNeedNotifyType = m_iNeedNotifyType;
            m_iNeedNotifyType = 0; //清空通知类型
            for (int iIndex = 0; iIndex < m_vecNotifyFunc.size(); ++iIndex)
            {
                if ((m_vecNotifyFunc[iIndex].m_fnNotifyfunc != NULL)
                        && (m_vecNotifyFunc[iIndex].m_eNotifyType & iNeedNotifyType))
                {
                    YLList<IdleStatusItem>  StatusItemList = GetNotifyStateList(m_vecNotifyFunc[iIndex].m_eNotifyType);
                    m_vecNotifyFunc[iIndex].m_fnNotifyfunc(StatusItemList);
                }
            }
            //清空通知类型
        }

        return true;
    }

#ifndef IF_NOTIFICATION_CENTER_ON_IDLE

    IDLE_DEBUG("CStatusManager: Notify Status Changed m_isOn=[%d] [%d]", m_isOn, m_iNeedNotifyType);
#if IF_FEATURE_XMLB_SCREENSAVER
    if (m_iNeedNotifyType == POPUP_WINDOW_POPUP
            || m_iNeedNotifyType == POPUP_WINDOW_TEXT_NOTIFY)
    {
        //弹框类型则通知黑白屏退出xml屏保
        msgPostMsgToThread(msgGetThreadByName(VP_PHONE_THREAD_NAME), IDLE_MSG_EXIT_SCREENSAVER, 0, 0);
    }
#endif
    if (!m_isOn)
    {
        return false;
    }
#if IF_IDLE_SUPPORT_NOTIFY_LIMIT
    if (m_iNeedNotifyType != 0)
#endif // IF_IDLE_SUPPORT_NOTIFY_LIMIT
    {
        for (int iIndex = 0; iIndex < m_vecNotifyFunc.size(); ++iIndex)
        {
            if (NULL == m_vecNotifyFunc[iIndex].m_fnNotifyfunc)
            {
                continue;
            }

            m_vecNotifyFunc[iIndex].m_fnNotifyfunc(
                GetTopStateList(m_vecNotifyFunc[iIndex].m_iCount)
            );
        }
    }

    UpdateNotifyState(false, 0, true);
#endif // IF_NOTIFICATION_CENTER_ON_IDLE

    return true;
}

bool CStatusManager::ChangeHint(int nStatus, const yl::string & strNewHint,
                                unsigned int uNotifyType/*= SNT_ICON*/)
{
    if (GetIsXwin())
    {
        AddStatus(nStatus, strNewHint);

        unsigned int iClearFlag = 0;

        for (int i = SNT_NONE; i < SNT_END; ++i)
        {
            unsigned int iFlag = (0x01 << i);

            if (iFlag & uNotifyType)
            {
                continue;
            }

            iClearFlag |= iFlag;
        }

        ClearNotifyTypeFlag(nStatus, iClearFlag);

        NotifyStatusChange();

        return true;
    }

#ifndef IF_NOTIFICATION_CENTER_ON_IDLE
    IdleStatusItem * pstItem = GetStatusItem(nStatus);
    if (pstItem != NULL)
    {
        pstItem->m_strHint = strNewHint;
        UpdateNotifyState(true, nStatus);
        return true;
    }
#endif // IF_NOTIFICATION_CENTER_ON_IDLE

    return false;
}

IdleStatusItem* CStatusManager::GetStatusItem(int nStatus)
{
    StatusConfigureMap::iterator iterStatusCfg = m_mapStatusCfg.find(nStatus);
    if (iterStatusCfg == m_mapStatusCfg.end())
    {
        return NULL;
    }

#ifndef IF_NOTIFICATION_CENTER_ON_IDLE
    if (!GetIsXwin())
    {
        //直接返回固定状态组
        if (iterStatusCfg->second.m_nLoc != 0)
        {
            return &m_vecFixedStatus[iterStatusCfg->second.m_nLoc - 1];
        }

        return NULL;
    }
#endif // IF_NOTIFICATION_CENTER_ON_IDLE

    for (NonFixedStatusList::iterator iterStatusItem = m_listStatus.begin();
            iterStatusItem != m_listStatus.end(); ++iterStatusItem)
    {
        if ((*iterStatusItem).m_nId == nStatus)
        {
            return &(*iterStatusItem);
        }
    }

    return NULL;
}

#ifndef IF_NOTIFICATION_CENTER_ON_IDLE
void CStatusManager::UpdateNotifyState(bool bAdd, int iStatus, bool bForce/* = false*/)
{
    if (bForce)
    {
        m_iNeedNotifyType = iStatus;
        return;
    }

    if (bAdd)
    {
        m_iNeedNotifyType += iStatus;
    }
    else
    {
        m_iNeedNotifyType -= iStatus;
    }
}

YLList<IdleStatusItem> CStatusManager::GetTopStateList(int iCount /*= -1*/)
{
    YLList<IdleStatusItem> listStatus;
    int nMaxStateCount = 0;
    int nCounter = 0;

    if (iCount == -1) //iCount为-1时获取所有状态
    {
        nMaxStateCount = m_vecFixedStatus.size() + m_listStatus.size();
    }
    else
    {
        nMaxStateCount = iCount;
    }
    //获取固定状态
    for (int i = 0;
            i < (int)m_vecFixedStatus.size() && nCounter < nMaxStateCount;
            ++i)
    {
        if (m_vecFixedStatus[i].m_nId != PS_STATE_NONE)
        {
            listStatus.push_back(m_vecFixedStatus[i]);
        }
    }

    // 获取其他状态.
    NonFixedStatusList::iterator citer = m_listStatus.begin();
    for (; citer != m_listStatus.end() && nCounter < nMaxStateCount; ++citer)
    {
        const IdleStatusItem & refItem = (*citer);
        listStatus.push_back(refItem);
    }

    return listStatus;
}

void CStatusManager::ClearPopupFlag(int nStatus)
{
    // 查找状态项.
    IdleStatusItem * pItem = GetStatusItem(nStatus);
    if (pItem != NULL)
    {
        pItem->m_nPopupType = POPUP_WINDOW_NO_POPUP;
#if IF_BUG_11498
        if (nStatus == PS_STATE_MISSCALL)
        {
            powerled_FlushPowerLed();
        }
#endif
    }
}

void CStatusManager::SetPopupStatus(int nStatus, int nPopupStatus)
{
    if (nPopupStatus <= POPUP_WINDOW_TYPE_NONE
            || nPopupStatus >= POPUP_WINDOW_TYPE_MAX)
    {
        return;
    }

    IDLE_INFO("Status manager: SetPopupStatus(%d)->Status(%d)",
              nStatus, nPopupStatus);
    IdleStatusItem * pItem = GetStatusItem(nStatus);
    if (pItem != NULL)
    {
        pItem->m_nPopupType = nPopupStatus;
    }
}

int CStatusManager::GetPopupStatus(int nStatus)
{
    // 获取状态的弹窗状态.
    const IdleStatusItem * pItem = GetStatusItem(nStatus);
    return pItem != NULL ? pItem->m_nPopupType : POPUP_WINDOW_TYPE_NONE;
}

YLVector<int> CStatusManager::GetStatusToPopup()
{
    YLVector<int> vecStatus;
    //获取固定状态
    for (int i = 0;
            i < (int)m_vecFixedStatus.size();
            ++i)
    {
        if (m_vecFixedStatus[i].m_nId != PS_STATE_NONE
                && m_vecFixedStatus[i].m_nPopupType == POPUP_WINDOW_POPUP)
        {
            vecStatus.push_back(m_vecFixedStatus[i].m_nId);
        }
    }  // for.

    // 获取其他状态.
    NonFixedStatusList::iterator citer = m_listStatus.begin();
    for (; citer != m_listStatus.end(); ++citer)
    {
        const IdleStatusItem & refItem = (*citer);
        if (refItem.m_nPopupType == POPUP_WINDOW_POPUP)
        {
            vecStatus.push_back(refItem.m_nId);
        }
    }  // for.
    return vecStatus;
}

bool CStatusManager::RegisterStatus(int nStatus, PopupWindowType ePopupStatus, int nLoc)
{
    //组成StatusConfigItem
    StatusConfigItem cfgItem;
    cfgItem.m_nLoc = nLoc;
    cfgItem.m_nPopupType = ePopupStatus;
    return RegisterStatus(nStatus, cfgItem);
}

bool CStatusManager::AddFixedStatus(int nStatus, const yl::string & strHint)
{
    StatusConfigItem * configItem = GetConfigItem(nStatus);
    if (configItem == NULL)
    {
        return false;
    }

    bool bFlush = m_vecFixedStatus[configItem->m_nLoc - 1].m_nId != PS_STATE_NONE;

    m_vecFixedStatus[configItem->m_nLoc - 1].m_nId = nStatus;
    m_vecFixedStatus[configItem->m_nLoc - 1].m_nPopupType = configItem->m_nPopupType;
    m_vecFixedStatus[configItem->m_nLoc - 1].m_strHint = strHint;

    UpdateNotifyState(true, configItem->m_nPopupType);

    if (configItem->funcChange != NULL)
    {
        configItem->funcChange(bFlush ? STATUS_ACTION_UPDATE : STATUS_ACTION_ADD);
    }

    return true;
}

bool CStatusManager::SetNotifyFunByCount(StatusNotifyFunction func, int nCount)
{
    StatusNotifyInfo newNofify;
    newNofify.m_fnNotifyfunc = func;
    newNofify.m_iCount = nCount;
    m_vecNotifyFunc.push_back(newNofify);
    return true;
}

#endif // IF_NOTIFICATION_CENTER_ON_IDLE

bool CStatusManager::IsNotifyTypeInStatusItem(IdleStatusItem & statusItem,
        StateNotifyType eNotifyType)
{
    bool bRet = false;
    VecNotifyTypePriority::iterator VecIter = statusItem.m_vecTypePriority.begin();
    for (; VecIter != statusItem.m_vecTypePriority.end(); ++VecIter)
    {
        NotifyTypePriority & TypePriority = (*VecIter);
        if (TypePriority.m_eNotifyType == eNotifyType)
        {
            bRet = true;
            break;
        }
    }
    return bRet;
}

int CStatusManager::GetStatusItemPriority(IdleStatusItem & statusItem, StateNotifyType eNotifyType)
{
    int iPriority = -1;
    VecNotifyTypePriority::iterator VecIter = statusItem.m_vecTypePriority.begin();
    for (; VecIter != statusItem.m_vecTypePriority.end(); ++VecIter)
    {
        NotifyTypePriority & TypePriority = (*VecIter);
        if (TypePriority.m_eNotifyType == eNotifyType)
        {
            iPriority = TypePriority.m_Priority;
            break;
        }
    }
    return iPriority;
}

void CStatusManager::ClearNotifyTypeFlag(VecNotifyTypePriority & vecNotify,
        unsigned int iNotifyType)
{
    if (iNotifyType == 0)
    {
        //http://10.2.1.199/Bug.php?BugID=95091
        //vecNotify.clear();
        return;
    }

    int iIndex = 0;
    while (iIndex < vecNotify.size())
    {
        if (vecNotify[iIndex].m_eNotifyType & iNotifyType)
        {
            vecNotify.removeAt(iIndex);
            continue;
        }
        iIndex++;
    }
}

YLList<IdleStatusItem> CStatusManager::GetNotifyStateList(StateNotifyType eNotifyType)
{
    YLList<IdleStatusItem> IdleStatusList;

    NonFixedStatusList::iterator citer = m_listStatus.begin();
    for (; citer != m_listStatus.end(); ++citer)
    {
        IdleStatusItem & refItem = (*citer);
        //IdleStatusItem中是否存在需要类型
        if (!IsNotifyTypeInStatusItem(refItem, eNotifyType))
        {
            continue;
        }

        if (0 == IdleStatusList.size())
        {
            //第一个直接插入
            IdleStatusList.push_back(refItem);
        }
        else
        {
            int iCurrentPriority = GetStatusItemPriority(refItem, eNotifyType);
            YLList<IdleStatusItem>::iterator IterList = IdleStatusList.begin();
            for (; IterList != IdleStatusList.end(); ++IterList)
            {
                IdleStatusItem & rItem = (*IterList);
                //判断优先级
                int iPrePriority = GetStatusItemPriority(rItem, eNotifyType);
                if (iCurrentPriority >= iPrePriority)
                {
                    //前方插入即可
                    IdleStatusList.insert(IterList, refItem);
                    break;
                }
            }//找不到比他大
            if (IterList == IdleStatusList.end())
            {
                IdleStatusList.push_back(refItem);
            }
        }
    }  // for.

    return IdleStatusList;
}

void CStatusManager::ClearNotifyTypeFlag(int nStatus, unsigned int iNotifyType)
{
    // 查找状态项.
    IdleStatusItem * pItem = GetStatusItem(nStatus);
    if (pItem != NULL)
    {
        ClearNotifyTypeFlag(pItem->m_vecTypePriority, iNotifyType);
    }
}
void CStatusManager::ClearCfgStatusFlag(int nStatus, unsigned int iNotifyType)
{
    StatusConfigItem * pCfgItem = GetConfigItem(nStatus);
    if (pCfgItem != NULL)
    {
        ClearNotifyTypeFlag(pCfgItem->m_vecPriotify, iNotifyType);
    }
}

bool CStatusManager::SetNotifyType(int nStatus, StateNotifyType eNotifyType, int iPriority)
{
    bool  bRet = false;
    COMMONUNITS_INFO("Status manager: SetPopupStatus(%d)->Status(%d)", nStatus, eNotifyType);
    StatusConfigItem * pItem = GetConfigItem(nStatus);
    if (pItem != NULL)
    {
        VecNotifyTypePriority::iterator VecIter = pItem->m_vecPriotify.begin();
        for (; VecIter != pItem->m_vecPriotify.end(); ++VecIter)
        {
            NotifyTypePriority & TypePriority = (*VecIter);
            if (TypePriority.m_eNotifyType == eNotifyType)
            {
                TypePriority.m_Priority = iPriority;
                bRet = true;
                break;
            }
        }
        //原来没有这个类型则添加新类型
        if (VecIter == pItem->m_vecPriotify.end())
        {
            NotifyTypePriority nPriority;
            nPriority.m_eNotifyType = eNotifyType;
            nPriority.m_Priority = iPriority;
            pItem->m_vecPriotify.push_back(nPriority);
            bRet = true;
        }
    }
    return bRet;
}

unsigned int CStatusManager::GetNotifyType(int nStatus)
{
    // 获取状态的通知类型
    IdleStatusItem * pItem = GetStatusItem(nStatus);
    int iNotifyType = SNT_NONE;
    if (pItem != NULL)
    {
        VecNotifyTypePriority::iterator VecIter = pItem->m_vecTypePriority.begin();
        for (; VecIter != pItem->m_vecTypePriority.end(); ++VecIter)
        {
            NotifyTypePriority & TypePriority = (*VecIter);
            iNotifyType |= TypePriority.m_eNotifyType;
        }
    }
    return iNotifyType;
}

bool CStatusManager::RegisterStatus(int nStatus, StateNotifyType eNotifyType, int iPriority)
{
    bool bRet = false;
    //如果注册过则设置成功
    bRet = SetNotifyType(nStatus, eNotifyType, iPriority);
    if (!bRet)
    {
        //设置失败，未注册
        NotifyTypePriority TypePriority;
        TypePriority.m_eNotifyType = eNotifyType;
        TypePriority.m_Priority = iPriority;
        StatusConfigItem cfgItem;
        cfgItem.m_vecPriotify.push_back(TypePriority);
        bRet = RegisterStatus(nStatus, cfgItem);
    }
    return bRet;
}

bool CStatusManager::SetNotifyFunction(StatusNotifyFunction func,  StateNotifyType eNotifyType)
{
    StatusNotifyInfo newNofify;
    newNofify.m_fnNotifyfunc = func;
    newNofify.m_eNotifyType = eNotifyType;
    m_vecNotifyFunc.push_back(newNofify);
    return true;
}

#if IS_CODE_TEST
void CStatusManager::reset()
{
    m_mapStatusCfg.clear();
    m_vecFixedStatus.clear();
    m_listStatus.clear();  // 可变状态列表.
}
#endif

#ifdef IF_FEATURE_PSTN
void CStatusManager::GetAddPstnIdList(YLVector<int> & vecID)
{
    vecID = m_listAddPstnId;
}

void CStatusManager::AddPstnIdToList(int nAccountId)
{
    m_listAddPstnId.push_back(nAccountId);
}

void CStatusManager::RemovePstnIdFromList(int nAccountId)
{
    for (int i = 0; i < m_listAddPstnId.size(); ++i)
    {
        if (m_listAddPstnId[i] == nAccountId)
        {
            m_listAddPstnId.removeAt(i);
        }
    }
}

void CStatusManager::ClearPstnIdList()
{
    m_listAddPstnId.clear();
}
#endif // IF_FEATURE_PSTN

#endif // IF_FEATURE_IDLESCREEN



