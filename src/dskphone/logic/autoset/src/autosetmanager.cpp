#ifdef IF_FEATURE_AUTOTEST
#include "autoset_inc.h"

IMPLEMENT_GETINSTANCE(CAutoSetManager)

CAutoSetManager::CAutoSetManager(void)
{
    m_iAutoSetType = AUTOSET_NON;
    m_bFirstRunging = true;
    m_vecAutoSetStrategy.clear();
}

CAutoSetManager::~CAutoSetManager(void)
{
    Release();
}

void CAutoSetManager::Init()
{
    InitAutoSetObj();
    // 注册消息
    etl_RegisterMsgHandle(DSSKEY_RELOAD_FINISHED, DSSKEY_RELOAD_FINISHED,
                          &CAutoSetManager::OnAutoSetMessage);
    etl_RegisterMsgHandle(BLFLIST_DATA_UPDATE, BLFLIST_DATA_UPDATE, &CAutoSetManager::OnAutoSetMessage);
    etl_RegisterMsgHandle(ACCOUNT_STATUS_CHANGED, ACCOUNT_STATUS_CHANGED,
                          &CAutoSetManager::OnAutoSetMessage);
    etl_RegisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED,
                          &CAutoSetManager::OnAutoSetMessage);
    etl_RegisterMsgHandle(DSK_MSG_EXT_CHANGE, DSK_MSG_EXT_CHANGE, &CAutoSetManager::OnAutoSetMessage);
    etl_RegisterMsgHandle(TM_TIMER, TM_TIMER, &CAutoSetManager::OnAutoSetMessage);
#ifdef IF_BT_SUPPORT_PHONE
    etl_RegisterMsgHandle(BLUETOOTH_MSG_SETDSSKEY, BLUETOOTH_MSG_SETDSSKEY,
                          &CAutoSetManager::OnAutoSetMessage);
    etl_RegisterMsgHandle(BLUETOOTH_MSG_STATE_CHANGE, BLUETOOTH_MSG_STATE_CHANGE,
                          &CAutoSetManager::OnAutoSetMessage);
#endif //#ifdef IF_BT_SUPPORT_PHONE

#if FEATURE_BROADTOUCH_UC
    etl_RegisterMsgHandle(PRESENCE_MESSAGE_FAVORITE_CHANGE, PRESENCE_MESSAGE_FAVORITE_CHANGE,
                          &CAutoSetManager::OnAutoSetMessage);
#endif

#if IF_FEATURE_FAVORITE_DIR
    etl_RegisterMsgHandle(LOCAL_FAVORITE_CHANGED, LOCAL_FAVORITE_CHANGED,
                          &CAutoSetManager::OnAutoSetMessage);
#endif //IF_FEATURE_FAVORITE_DIR
}

LRESULT CAutoSetManager::OnAutoSetMessage(msgObject & msg)
{
    //重启过程中，不进行autoset，防止配置被重写
    if (commonUnits_IsSystemMaintenanceStatus())
    {
        COMMONUNITS_INFO("system maintenance, do not process AutoSet.");
        return -1;
    }

    int iAutoSetType = AUTOSET_NON;
    switch (msg.message)
    {
    case DSSKEY_RELOAD_FINISHED:
        {
            iAutoSetType = AUTOSET_ALL;
        }
        break;
    case BLFLIST_DATA_UPDATE:
        {
            iAutoSetType = AUTOSET_BLFLIST;
        }
        break;
    case ACCOUNT_STATUS_CHANGED:
        {
            // 账号最终状态为注册上或disable或注册失败才去设置
            if (acc_IsInFinalRegisterState(msg.wParam)
                    || acc_IsDisableStatus(msg.wParam))
            {
                iAutoSetType = AUTOSET_ALL;
            }
        }
        break;
    case CONFIG_MSG_BCAST_CHANGED:
        {
            if (msg.wParam == ST_AUTOSET_LINEKEY
                    || msg.wParam == ST_AUTO_BLFLIST
                    || msg.wParam == ST_AUTO_FAVORITE
#if IF_FEATURE_FAVORITE_DIR
                    || msg.wParam == ST_AUTOSET_LOCAL_FAVORITE
#endif //IF_FEATURE_FAVORITE_DIR
               )
            {
                iAutoSetType = AUTOSET_ALL;
            }
            // web只修改账号label时，不会广播账号配置变更，只会广播默认账号变更
            else if (msg.wParam == ST_DEFAULT_ACCOUNT)
            {
                iAutoSetType = AUTOSET_ACCOUNT | AUTOSET_SEMI_ACCOUNT;
            }
            else
            {
                return 0;
            }
        }
        break;
    // 扩展台拔插消息处理
    case DSK_MSG_EXT_CHANGE:
        {
            iAutoSetType = AUTOSET_ALL;
        }
        break;
#ifdef IF_BT_SUPPORT_PHONE
    case BLUETOOTH_MSG_SETDSSKEY:
    case BLUETOOTH_MSG_STATE_CHANGE:
        {
            iAutoSetType = AUTOSET_ALL;
        }
        break;
#endif //#ifdef IF_BT_SUPPORT_PHONE
#if FEATURE_BROADTOUCH_UC
    case PRESENCE_MESSAGE_FAVORITE_CHANGE:
        {
            iAutoSetType = AUTOSET_UCFAVORITE;
        }
        break;
#endif
#if IF_FEATURE_FAVORITE_DIR
    case LOCAL_FAVORITE_CHANGED:
        {
            iAutoSetType = AUTOSET_FAVORITE_DIR;
        }
        break;
#endif //IF_FEATURE_FAVORITE_DIR
    case TM_TIMER:
        {
            return _AutoSetManager.OnTimer(msg.wParam);
        }
        break;
    default:
        return -1;
    }

    DSSKEY_INFO("CAutoSetManager::OnAutoSetMessage : [message:%d-%d-%d][eAutoSetType:%d] ",
                msg.message, msg.wParam, msg.lParam, iAutoSetType);
    _AutoSetManager.AutoSetProcess(msg, iAutoSetType);

    return 0;
}

void CAutoSetManager::AutoSetProcess(msgObject & msg, int iAutoSetType/* = AUTOSET_ALL*/)
{
    bool bAllowRun = IsAllowToRun();
    CAutoSetBaseStrategy * pTopStrategy = NULL;

    // 根据优先级顺序生成设置链表
    for (VECAUTOSETITER it = m_vecAutoSetStrategy.begin(); it != m_vecAutoSetStrategy.end();)
    {
        CAutoSetBaseStrategy * pStrategy = *(it++);
        if (NULL == pStrategy)
        {
            continue;
        }

        int iCmpResult = pStrategy->Compare(pTopStrategy);
        AUTOSET_TYPE eAutoSetType = pStrategy->GetAutoSetType();

        if ((eAutoSetType & iAutoSetType) || iCmpResult < 0)
        {
            // 判断自动设置的条件
            if (pStrategy->IsNeedToAutoSet(msg)
                    || (iCmpResult < 0 && pStrategy->IsEnable()))
            {
                // 不允许运行先执行固有流程
                if (!bAllowRun)
                {
                    pStrategy->InherentProcess(msg);
                }

                DelayUpdate(eAutoSetType);

                // 记住最高优先级
                if (pTopStrategy == NULL && pStrategy->IsEnable())
                {
                    pTopStrategy = pStrategy;
                }
            }
        }
    }

    if (!bAllowRun)
    {
        StopAsync();
    }
}

CAutoSetBaseStrategy * CAutoSetManager::GetStrategyByDsskeyType(DssKey_Type eDskType)
{
    for (VECAUTOSETITER it = m_vecAutoSetStrategy.begin(); it != m_vecAutoSetStrategy.end();)
    {
        CAutoSetBaseStrategy * pStrategy = *(it++);
        if (NULL == pStrategy
                || pStrategy->IsDisablePassive())
        {
            continue;
        }

        if (eDskType == pStrategy->GetBindDsskeyType())
        {
            return pStrategy;
        }
    }

    return NULL;
}

//是否允许设置
bool CAutoSetManager::IsAllowToSet(CAutoSetBaseStrategy * pStrategy, int iDsskeyID)
{
    if (pStrategy == NULL)
    {
        return false;
    }

    DssKey_Type eTargetDssKeyType = dsskey_GetDsskeyType(iDsskeyID);
    if (DT_NA == eTargetDssKeyType)
    {
        return true;
    }

    CAutoSetBaseStrategy * pTempStrategy = GetStrategyByDsskeyType(eTargetDssKeyType);
    if (NULL == pTempStrategy)
    {
        return false;
    }

    bool bAllow = pTempStrategy->IsAllowToSetByType(pStrategy, iDsskeyID);

    DSSKEY_INFO("from[%d] to[%d] query dsskey[%d] type[%d] allow[%d]", pStrategy->GetAutoSetType(),
                pTempStrategy->GetAutoSetType(), iDsskeyID, eTargetDssKeyType, bAllow);

    return bAllow;
}

void CAutoSetManager::InitAutoSetObj()
{
    // 优先级顺序
    AUTOSET_TYPE arr_Priority[] = { AUTOSET_ACCOUNT, AUTOSET_SEMI_ACCOUNT, AUTOSET_BT, AUTOSET_BLFLIST, AUTOSET_UCFAVORITE, AUTOSET_FAVORITE_DIR };
    CAutoSetBaseStrategy * pStrategy = NULL;
    for (int i = 0; i < sizeof(arr_Priority) / sizeof(AUTOSET_TYPE); i++)
    {
        pStrategy = CreateAutoSetStrategy(arr_Priority[i]);
        if (pStrategy != NULL)
        {
            // 目前只有 blflist、uc、favorite 才支持优先级排序，先将其优先级跳到最低
            pStrategy->SetPriority(arr_Priority[i] < AUTOSET_BLFLIST ? i : i + 20);
            m_vecAutoSetStrategy.push_back(pStrategy);
        }
    }

    // 动态优先级排序
    yl::string strSequence = configParser_GetConfigString(kszStrategySequence);
    YLVector<yl::string> vec;
    if (!ParseString(vec, strSequence, ", "))
    {
        return;
    }

    for (int i = 0; i < vec.size(); ++i)
    {
        const yl::string & strID = vec[i];

        YLVector<CAutoSetBaseStrategy *>::iterator it = m_vecAutoSetStrategy.begin();
        for (; it != m_vecAutoSetStrategy.end(); ++it)
        {
            pStrategy = *it;

            /* 1. 支持优先级排序，出现在配置中的理性按配置顺序确定优先级
             * 2. 没有出现的类型优先级默认最低，按默认优先级
             */

            LPCSTR lpName = pStrategy->GetAutoSetName();
            if (lpName == NULL)
            {
                continue;
            }

            if (strcasecmp(strID.c_str(), lpName) == 0)
            {
                // 出现在配置中提高优先级
                pStrategy->SetPriority(i + 10);
            }
        }
    }

    Sort(m_vecAutoSetStrategy, StrategyCompare);

#if IF_FEATURE_EXTEND_LOG
    YLVector<CAutoSetBaseStrategy *>::iterator it = m_vecAutoSetStrategy.begin();
    for (; it != m_vecAutoSetStrategy.end(); ++it)
    {
        pStrategy = *it;
        DSSKEY_INFO("type[%d] name[%s] priority[%d]", pStrategy->GetAutoSetType(), pStrategy->GetAutoSetName(), pStrategy->GetPriority());
    }
#endif
}

// 释放资源
void CAutoSetManager::Release()
{
    for (VECAUTOSETITER it = m_vecAutoSetStrategy.begin(); it != m_vecAutoSetStrategy.end();)
    {
        CAutoSetBaseStrategy * pStrategy = *(it++);
        delete pStrategy;
        pStrategy = NULL;
    }
    m_vecAutoSetStrategy.clear();
}

CAutoSetBaseStrategy * CAutoSetManager::CreateAutoSetStrategy(AUTOSET_TYPE eType)
{
    CAutoSetBaseStrategy * pStrategy = NULL;
    switch (eType)
    {
    case AUTOSET_ACCOUNT:
        pStrategy = new CAutoSetAccountStrategy;
        break;
#ifdef IF_BT_SUPPORT_PHONE
    case AUTOSET_BT:
        pStrategy = new CAutoSetBTStrategy;
        break;
#endif
#ifdef IF_FEATURE_BLFLIST
    case AUTOSET_BLFLIST:
        pStrategy = new CAutoSetBlfListStrategy;
        break;
#endif
#if FEATURE_BROADTOUCH_UC
    case AUTOSET_UCFAVORITE:
        pStrategy = new CAutoSetUCFavoriteStrategy;
        break;
#endif

#ifdef IF_FEATRUE_MANUAL_AUTOSET
    case AUTOSET_SEMI_ACCOUNT:
        pStrategy = new CSemiAutoSetAccountStrategy;
        break;
#endif

#if IF_FEATURE_FAVORITE_DIR
    case AUTOSET_FAVORITE_DIR:
        pStrategy = new CAutoSetFavoriteDirStrategy;
        break;
#endif //IF_FEATURE_FAVORITE_DIR

    default:
        break;
    }

    return pStrategy;
}

void CAutoSetManager::TraversalDssKeyModule(CAutoSetBaseStrategy * pStrategy)
{
    if (pStrategy == NULL)
    {
        return;
    }

    // 获取Dsskey数据
    YLList<int> listDsskey;
    TraversalDssKeyModule(pStrategy, listDsskey);

    int iIndex = 0;
    YLVector<autoset_dsskeydata_t> vecAutoSetData;
    pStrategy->MakeAutoSetData(vecAutoSetData);

#ifdef AUTOSET_EXTEND_LOG
    for (YLVector<autoset_dsskeydata_t>::iterator it = vecAutoSetData.begin();
            it != vecAutoSetData.end(); ++it)
    {
        DSSKEY_INFO("id[%d] type[%d] line[%d] lab[%s] val[%s] ext[%s] stat[%d] key[%s]", it->iDssKeyID,
                    it->eDKType, it->iLineIndex, it->strLabel.c_str(), it->strValue.c_str(), it->strExtension.c_str(),
                    it->iStatus, it->strKey.c_str());
    }
#endif

    YLList<int>::ListIterator it = listDsskey.begin();
    while (it != listDsskey.end())
    {
        int iDsskeyID = *it;
        // 允许被设置
        if (pStrategy->IsAllowToSet(iDsskeyID))
        {
            // 根据链表位置获取设置的数据
            autoset_dsskeydata_t tAutoSetData;
            bool bHaveData = false;

            if (pStrategy->IsDsskeyBind(iDsskeyID))
            {
                bHaveData = GetAutoSetData(iDsskeyID, vecAutoSetData, tAutoSetData);
                // 没有数据的情形下，跳过该dsskey
                if (!bHaveData)
                {
                    ++it;
                    continue;
                }
            }
            else
            {
                bHaveData = GetAutoSetData(vecAutoSetData, iIndex, tAutoSetData);
                // 该数据存在绑定的情况下直接跳过该数据
                if (bHaveData && pStrategy->IsDsskeyBind(tAutoSetData.iDssKeyID))
                {
                    continue;
                }
            }

            // 根据链表中获取的数据去生成dsskeydata，保存在tDsskeyData中
            DssKeyLogicData tDsskeyData;
            bool bRet = pStrategy->GetDssKeyData(tAutoSetData, tDsskeyData, iDsskeyID, bHaveData);

            if (bRet)
            {
#ifdef AUTOSET_EXTEND_LOG
                DssKeyLogicData tDsskeyOldData;
                dsskey_GetData(iDsskeyID, tDsskeyOldData);
#endif
                int iFlags = pStrategy->GetSetDssKeyFlag(iDsskeyID);
                // 把该dsskey数据存入内存
                dsskey_SetDsskeyData(iDsskeyID, tDsskeyData, iFlags);
                // 更新dsskey各自类型所对应的远端状态，并刷新ui
                pStrategy->UpdateDssKeyStatus(iDsskeyID, tAutoSetData, bHaveData);

#ifdef AUTOSET_EXTEND_LOG
                // autoset修改的dsskey的所有信息，方便记录和排查
                DSSKEY_INFO("dsskey[%d][%d][%d][%d] index[%d] status[%d][%d] [%d][%d][%d] type[%d]>[%d] line[%d]>[%d] lab[%s]>[%s] value[%s]>[%s] ext[%s]>[%s] flag[%d][%d][%d]",
                            iDsskeyID, GetModuleType(iDsskeyID),
                            GetModuleIndex(iDsskeyID), GetDsskeyIndex(iDsskeyID), iIndex,
                            (tDsskeyOldData.iLineIndex >= 0 ? acc_GetState(tDsskeyOldData.iLineIndex) : -1),
                            (tDsskeyData.iLineIndex >= 0 ? acc_GetState(tDsskeyData.iLineIndex) : -1),
                            tAutoSetData.iStatus, tDsskeyOldData.iStatus, tDsskeyData.iStatus,
                            tDsskeyOldData.eDKType, tDsskeyData.eDKType,
                            tDsskeyOldData.iLineIndex, tDsskeyData.iLineIndex,
                            tDsskeyOldData.strLabel.c_str(), tDsskeyData.strLabel.c_str(),
                            tDsskeyOldData.strValue.c_str(), tDsskeyData.strValue.c_str(),
                            tDsskeyOldData.strExtension.c_str(), tDsskeyData.strExtension.c_str(),
                            pStrategy->GetAutoSetType(), iFlags, bHaveData);
#endif
            }
        }

        ++it;
    }

    // 执行保存动作
    pStrategy->PreSaveToFile(listDsskey);
}

void CAutoSetManager::TraversalDssKeyModule(CAutoSetBaseStrategy * pStrategy,
        YLList<int> & listDsskey)
{
    if (pStrategy == NULL)
    {
        return;
    }

    DSSKEY_MODULE_TYPE eModuleType = exp_GetEXPModuleType();
    int iExpModuleNum = exp_GetEXPNum();

    // 根据设置顺序进行设置
    AUTOSET_SEQUENCE eSequence = pStrategy->GetSetDssKeySequence();
    if (eSequence == POSITIVE_SEQUENCE)
    {
        TraversalDssKeyModule(eSequence, listDsskey, DMT_LINEKEY_MODULE);
        TraversalDssKeyModule(eSequence, listDsskey, DMT_MEMKEY_MODULE);

        for (int i = 0; i < iExpModuleNum; i++)
        {
            TraversalDssKeyModule(eSequence, listDsskey, eModuleType, i);
        }
    }
    else if (eSequence == FIRSTPAGE_SEQUENCE)
    {
        /* 顺序为：linekey 第1页 memeky 第1页
        exp0 exp1... expN 第1页 exp0 exp1... expN 第2页
        linekey memkey 第2,3,N页 */

        int iPageNum = 0;
        TraversalDssKeyModule(eSequence, listDsskey, DMT_LINEKEY_MODULE, 0, iPageNum);
        TraversalDssKeyModule(eSequence, listDsskey, DMT_MEMKEY_MODULE, 0, iPageNum);

        int iMaxPageNum = dsskey_GetPageCountByMoudle(eModuleType);
        for (; iPageNum < iMaxPageNum; iPageNum++)
        {
            for (int i = 0; i < iExpModuleNum; i++)
            {
                TraversalDssKeyModule(eSequence, listDsskey, eModuleType, i, iPageNum);
            }
        }

        iPageNum = 1;
        int iMaxPageLineNum = dsskey_GetPageCountByMoudle(DMT_LINEKEY_MODULE);
        int iMaxPageMemNum = dsskey_GetPageCountByMoudle(DMT_MEMKEY_MODULE);
        iMaxPageNum = chMAX(iMaxPageLineNum, iMaxPageMemNum);

        for (; iPageNum < iMaxPageNum; iPageNum++)
        {
            TraversalDssKeyModule(eSequence, listDsskey, DMT_LINEKEY_MODULE, 0, iPageNum);
            TraversalDssKeyModule(eSequence, listDsskey, DMT_MEMKEY_MODULE, 0, iPageNum);
        }
    }
    else if (eSequence == EXP_FIRSTPATE_SEQUENCE)
    {
        /* 顺序为 exp1 第一页 → exp2 第一页。。。。。 →
        exp1 第二页 → exp2 第二页。。。。→ line key */
        int iPageNum = 0;

        //分布扩展台
        int iMaxPageNum = dsskey_GetPageCountByMoudle(eModuleType);
        for (; iPageNum < iMaxPageNum; iPageNum++)
        {
            for (int i = 0; i < iExpModuleNum; i++)
            {
                TraversalDssKeyModule(eSequence, listDsskey, eModuleType, i, iPageNum);
            }
        }

        TraversalDssKeyModule(eSequence, listDsskey, DMT_LINEKEY_MODULE);
        TraversalDssKeyModule(eSequence, listDsskey, DMT_MEMKEY_MODULE);
    }
    else
    {
        for (int i = 0; i < iExpModuleNum; i++)
        {
            TraversalDssKeyModule(eSequence, listDsskey, eModuleType, i);
        }

        TraversalDssKeyModule(eSequence, listDsskey, DMT_MEMKEY_MODULE);
        TraversalDssKeyModule(eSequence, listDsskey, DMT_LINEKEY_MODULE);
    }

    DSSKEY_INFO("sequence[%d] module[%d] count[%d] size[%d]",
                eSequence, eModuleType, iExpModuleNum, listDsskey.size());
}

void CAutoSetManager::TraversalDssKeyModule(AUTOSET_SEQUENCE eSequence, YLList<int> & listDsskey,
        DSSKEY_MODULE_TYPE eModuleType, int nModuleIndex/* = 0*/, int iPageNum /*= -1*/)
{
    if (iPageNum >= 0 && eSequence >= FIRSTPAGE_SEQUENCE)
    {
        dsskey_GetPageDsskeys(listDsskey, eModuleType, nModuleIndex, iPageNum);
    }
    else
    {
        dsskey_GetAllDsskeyDataByModule(listDsskey, eModuleType, nModuleIndex);
    }
}

bool CAutoSetManager::GetAutoSetData(YLVector<autoset_dsskeydata_t> & vecAutoSetData,
                                     int & iListIndex, autoset_dsskeydata_t & tAutoSetData)
{
    if (iListIndex >= vecAutoSetData.size()
            || iListIndex < 0)
    {
        return false;
    }

    tAutoSetData = vecAutoSetData[iListIndex++];
    return true;
}

bool CAutoSetManager::GetAutoSetData(int iDsskeyID, YLVector<autoset_dsskeydata_t> & vecAutoSetData,
                                     autoset_dsskeydata_t & tAutoSetData)
{
    // 查找绑定的dsskey
    YLVector<autoset_dsskeydata_t>::iterator it = vecAutoSetData.begin();
    for (; it != vecAutoSetData.end(); ++it)
    {
        // 没有绑定的情况下，不用再遍历了
        if (it->iDssKeyID == INVALID_DSSKEY_ID)
        {
            break;
        }
        else if (it->iDssKeyID == iDsskeyID)
        {
            tAutoSetData = *it;
            return true;
        }
    }

    return false;
}

void CAutoSetManager::DelayUpdate(UINT uType/* = AUTOSET_NON*/,
                                  int iTimer/* = AUTOSET_DELAY_UPDATE*/)
{
    m_iAutoSetType |= uType;

    if (m_iAutoSetType != AUTOSET_NON)
    {
        // 上电时，设置时间为比较长，避免频繁运行
        if (m_bFirstRunging)
        {
            iTimer = 1000;
        }

        timerSetThreadTimer(TIMER_ID(m_iAutoSetType), iTimer);
        DSSKEY_INFO("DelayUpdate [%d] [%d] [%d]", uType, m_iAutoSetType, iTimer);
    }
}

bool CAutoSetManager::OnTimer(UINT uTimer)
{
    if (uTimer == TIMER_ID(m_iAutoSetType))
    {
        // Autop 没有执行
        if (Autop_IsUpdating(true))
        {
            DSSKEY_INFO("autoset delay for autop runing");
            DelayUpdate(m_iAutoSetType, AUTOSET_DELAY_UPDATE * 10);
        }
        else if (IsAllowToRun())
        {
            m_bFirstRunging = false;

            StopAsync();
            AutoSetProcess();
        }

        return true;
    }

    return false;
}

void CAutoSetManager::AutoSetProcess()
{
    if (m_iAutoSetType == AUTOSET_NON)
    {
        return;
    }

    // 根据优先级顺序进行自动设置
    for (VECAUTOSETITER it = m_vecAutoSetStrategy.begin(); it != m_vecAutoSetStrategy.end();)
    {
        CAutoSetBaseStrategy * pStrategy = *(it++);
        if (NULL == pStrategy)
        {
            continue;
        }

        AUTOSET_TYPE eAutoSetType = pStrategy->GetAutoSetType();
        if (eAutoSetType & m_iAutoSetType)
        {
            /* 没开启，非析构流程 */
            if (!pStrategy->IsEnable()
                    && !pStrategy->GetClearFlag())
            {
                continue;
            }

            // 新增还原流程，直接析构掉内存临时文件
            if (pStrategy->GetClearFlag()
                    && (pStrategy->GetSetDssKeyFlag(INVALID_DSSKEY_ID) & DSSKEY_SET_ONLY))
            {
                DSSKEY_INFO("release type %d", pStrategy->GetAutoSetType());
                bool bRet = dsskey_ReloadDSSKeyData(pStrategy->GetBindDsskeyType());
                pStrategy->SetClearFlag(false);
                if (bRet)
                {
                    // 类型恢复，需要重新跑autoset
                    return;
                }

                continue;
            }

            DSSKEY_INFO("AutoSet [ Type:%d] Begin", eAutoSetType);
            // 遍历dsskey模块，自动设置保存数据并刷新ui
            TraversalDssKeyModule(pStrategy);
            // 自动设置完成后的处理，blflist刷新callpickupmenu界面、账号(SCA)刷新灯状态
            pStrategy->AfterAutoSetProcess();
            DSSKEY_INFO("AutoSet [ Type:%d] End", eAutoSetType);
        }
    }

    m_iAutoSetType = AUTOSET_NON;
}

void CAutoSetManager::StopAsync()
{
    timerKillThreadTimer(TIMER_ID(m_iAutoSetType));
}

bool CAutoSetManager::IsAllowToRun()
{
    return !talklogic_SessionExist();
}

bool CAutoSetManager::StrategyCompare(const CAutoSetBaseStrategy *l, const CAutoSetBaseStrategy * r)
{
    if (l == r || l == NULL)
    {
        return false;
    }

    if (r == NULL)
    {
        return true;
    }

    return l->GetPriority() < r->GetPriority();
}

#endif  // IF_FEATURE_AUTOTEST
