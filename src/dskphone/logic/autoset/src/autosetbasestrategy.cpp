#ifdef IF_FEATURE_AUTOTEST
#include "autoset_inc.h"

CAutoSetBaseStrategy::CAutoSetBaseStrategy(void)
{
    m_bClearFlag = false;
    m_listFlushDsskeyId.clear();
    m_iPriority = 0;
}

CAutoSetBaseStrategy::~CAutoSetBaseStrategy(void)
{
}

bool CAutoSetBaseStrategy::AfterAutoSetProcess()
{
    // 刷新所有变更过的dsskey
    for (YLList<int>::ListIterator iter = m_listFlushDsskeyId.begin();
            iter != m_listFlushDsskeyId.end(); ++iter)
    {
        dsskey_refreshDSSKeyUI(*iter);
    }

    m_listFlushDsskeyId.clear();
    SetClearFlag(false);
    return true;
}

bool CAutoSetBaseStrategy::PreSaveToFile(YLList<int> & listDsskey)
{
    // 去重复操作，合并刷新操作
    Sort(m_listFlushDsskeyId, yl::less<int>());
    YLList<int>::iterator it =
        yl::unique(m_listFlushDsskeyId.begin(), m_listFlushDsskeyId.end());

    while (it != m_listFlushDsskeyId.end())
    {
        it = m_listFlushDsskeyId.erase(it);
    }

    return true;
}

bool CAutoSetBaseStrategy::IsAllowToSet(int iDssKeyID)
{
    // 查询其它策略关于该dsskey是否可重置
    return _AutoSetManager.IsAllowToSet(this, iDssKeyID);
}

bool CAutoSetBaseStrategy::IsAllowToSetByType(CAutoSetBaseStrategy * pStrategy, int iDsskeyID)
{
    // 查询者策略
    if (pStrategy == NULL)
    {
        return false;
    }

    // 查询者优先级更高，交给当前策略决定
    int iCmpResult = Compare(pStrategy);

    if (iCmpResult < 0)
    {
        return OnHigherPriorityStrategyQuery(iDsskeyID);
    }
    // 当前策略查询自身
    else if (iCmpResult == 0)
    {
        return OnSelfStrategyQuery(iDsskeyID);
    }
    // 低优先级策略查询
    else
    {
        return OnLowerPriorityStrategyQuery(iDsskeyID);
    }
}

bool CAutoSetBaseStrategy::GetDssKeyData(autoset_dsskeydata_t & tAutoSetData,
        DssKeyLogicData & tDsskeyData, int iDssKeyID, bool bHaveAutoSetData /*= true*/)
{
    dsskey_GetData(iDssKeyID, tDsskeyData);

    // 有设置数据
    if (bHaveAutoSetData)
    {
        tAutoSetData >> tDsskeyData;
        return true;
    }
    else
    {
        // 没有产生数据的情况下，多余的dsskey直接重置掉
        if (GetBindDsskeyType() == tDsskeyData.eDKType)
        {
            if (!IsSingleKey() && IsEnable() && !IsAutoSet())
            {
                tDsskeyData.clear_value();
            }
            else
            {
                tDsskeyData.reset();
            }

            return true;
        }
    }

    return false;
}

bool CAutoSetBaseStrategy::IsNeedToAutoSet(msgObject & msg)
{
    if (msg.message == DSSKEY_RELOAD_FINISHED)
    {
        bool bRet = false;
        // dsskey模块发送过来的数据
        int iChangeNum = msg.GetExtraSize() / sizeof(dsskey_change_info_t);
        dsskey_change_info_t * pChangeData = (dsskey_change_info_t *)msg.GetExtraData();

        if (iChangeNum == 0 || pChangeData == NULL)
        {
            bRet |= IsNeedAutoSetImp((DssKey_Type)msg.wParam, (DssKey_Type)msg.lParam);
        }
        else
        {
            for (int i = 0; i < iChangeNum; ++i)
            {
                // dsskey重新加载
                bRet |= OnDsskeyReload(pChangeData[i]);
                bRet |= IsNeedAutoSetImp(pChangeData[i].eNewType, pChangeData[i].eOldType);
            }
        }

        return bRet && IsEnable();
    }
    else if (msg.message == DSK_MSG_EXT_CHANGE)
    {
        // exp 变更需要处理
        return IsEnable();
    }
    else if (msg.message == ACCOUNT_STATUS_CHANGED)
    {
        // 由于账号改变账号可能会有dssskey被动隐藏，所以也需要重排序
        return IsAutoSet();
    }

    return false;
}

bool CAutoSetBaseStrategy::IsNeedAutoSetImp(DssKey_Type eNewType, DssKey_Type eOldType)
{
    DssKey_Type eType = CAutoSetBaseStrategy::GetBindDsskeyType();

    if (eOldType == eNewType && eNewType == eType)
    {
        return false;
    }

    /* 其它策略优先级比当前策略高的时候，当前策略被迫运行*/
    return eType == eNewType || Compare(eOldType) <= 0 || Compare(eNewType) <= 0;
}

int CAutoSetBaseStrategy::Compare(CAutoSetBaseStrategy * pStrategy) const
{
    if (pStrategy == NULL)
    {
        /* 理论上非autoset类型优先级高，实际处理时当做优先级低 */
        return 1;
    }

    // 值越小优先级越大
    int iValue = pStrategy->GetPriority() - GetPriority();
    return ((iValue == 0) ? 0 : (iValue < 0) ? -1 : 1);
}

int CAutoSetBaseStrategy::Compare(DssKey_Type eType) const
{
    // 类型相同，相等
    if (eType == GetBindDsskeyType())
    {
        return 0;
    }
    // 基类NA相同，则优先级最低
    else if (eType == CAutoSetBaseStrategy::GetBindDsskeyType())
    {
        return 1;
    }

    // 依据类型获取绑定的策略，判断策略优先级
    return Compare(_AutoSetManager.GetStrategyByDsskeyType(eType));
}

bool CAutoSetBaseStrategy::OnHigherPriorityStrategyQuery(int iDsskeyID) const
{
    // 是否需要抢占
    return IsEnable();
    // 已经分配的优先使用此规则
    // return dsskey_GetDsskeyType(iDsskeyID, DSS_ATTR_NATIVE) != GetBindDsskeyType();
}

bool CAutoSetBaseStrategy::UpdateDssKeyStatus(int iDssKeyID,
        const autoset_dsskeydata_t & tAutoSetData, bool bHaveData /*= true*/)
{
    m_listFlushDsskeyId.push_back(iDssKeyID);
    return true;
}

#endif  // IF_FEATURE_AUTOTEST
