#ifdef IF_FEATURE_AUTOTEST
#ifdef IF_FEATURE_BLFLIST
#include "autoset_inc.h"

CAutoSetBlfListStrategy::CAutoSetBlfListStrategy(void)
{
    m_eAutoSetType = AUTOSET_BLFLIST;
    LoadConfig();
}

CAutoSetBlfListStrategy::~CAutoSetBlfListStrategy(void)
{
}

bool CAutoSetBlfListStrategy::IsNeedToAutoSet(msgObject & msg)
{
    bool bEnable = false;

    // 总有一种策略处于激活状态
    if (CAutoSetBaseStrategy::IsNeedToAutoSet(msg))
    {
        bEnable = true;
    }
    else if (msg.message == BLFLIST_DATA_UPDATE)
    {
        bEnable = true;
    }
    else if (CONFIG_MSG_BCAST_CHANGED == msg.message
             && ST_AUTO_BLFLIST == msg.wParam)
    {
        LoadConfig();
        bEnable = true;
    }

    return bEnable;
}

// 从blflist模块获取数据
bool CAutoSetBlfListStrategy::MakeAutoSetData(YLVector<autoset_dsskeydata_t> & vecAutoSetData)
{
    // 从blflist模块获取数据
    YLVector<AccountMoitorData> refVector;
    blf_GetBlfListData(refVector);
    DSSKEY_INFO("Get Blflist data [%d] auto[%d]", refVector.size(), m_bEnableAutoSetBlflist);
    DssKey_Type eType = GetBindDsskeyType();

    if (IsEnableAutoSetBlfList())
    {
        for (int i = 0; i < refVector.size(); ++i)
        {
            AccountMoitorData objData = refVector[i];
            DSSKEY_INFO("Get Blflist data [Account:%d][Blflist Size:%d]", objData.m_nAccount,
                        objData.m_listData.size());
            for (YLList<BLFBaseData *>::ListIterator iter = objData.m_listData.begin();
                    iter != objData.m_listData.end(); ++iter)
            {
                BLFBaseData * pData = *iter;

                if (pData != NULL)
                {
                    autoset_dsskeydata_t tAutoSetData;
                    tAutoSetData << *pData << eType << objData.m_nAccount;
                    vecAutoSetData.push_back(tAutoSetData);
                }
            }
        }
    }
#if IF_BUG_32812
    else if (IsEnableCustomSetBlfList())
    {
        YLList<int> listBlflistDsskeys;
        dsskey_GetDsskeysByType(listBlflistDsskeys, eType, GetSetDssKeySequence() != NEGATIVE_SEQUENCE);
        for (YLList<int>::ListIterator itr = listBlflistDsskeys.begin();
                itr != listBlflistDsskeys.end(); ++itr)
        {
            int iDssKeyID = *itr;

            GetManualSetDsskeyData(iDssKeyID, vecAutoSetData);
        }
    }
#endif
    else
    {
        YLList<int> listBlflistDsskeys;
        dsskey_GetDsskeysByType(listBlflistDsskeys, eType, GetSetDssKeySequence() != NEGATIVE_SEQUENCE);
        int iAccountNum = acc_AccountNum();
        int * arrAccount = new int[iAccountNum];
        memset(arrAccount, 0, sizeof(int) * iAccountNum);

        for (YLList<int>::ListIterator itr = listBlflistDsskeys.begin();
                itr != listBlflistDsskeys.end(); ++itr)
        {
            int iAccountId = dsskey_GetDsskeyLine(*itr);
            if (iAccountId < 0 || iAccountId > iAccountNum - 1)
            {
                continue;
            }

            autoset_dsskeydata_t tAutoSetData;
            tAutoSetData << eType << iAccountId;
            GetManualSetDsskeyData(++arrAccount[iAccountId], refVector, tAutoSetData);

            vecAutoSetData.push_back(tAutoSetData);
        }
        delete [] arrAccount;
    }
    return true;
}

bool CAutoSetBlfListStrategy::GetManualSetDsskeyData(int iIndex,
        YLVector<AccountMoitorData> & refVector, autoset_dsskeydata_t & tAutoSetData)
{
    for (int i = 0; i < refVector.size(); ++i)
    {
        if (tAutoSetData.iLineIndex != refVector[i].m_nAccount)
        {
            continue;
        }

        AccountMoitorData & objData = refVector[i];
        int iListIndex = 1;
        for (YLList<BLFBaseData *>::ListIterator iter = objData.m_listData.begin();
                iter != objData.m_listData.end(); ++iter, ++iListIndex)
        {
            BLFBaseData * pData = *iter;

            // 找到该账号的第几个
            if (pData != NULL && iListIndex == iIndex)
            {
                tAutoSetData << *pData;
                return true;
            }
        }
    }

    return false;
}

// 判断能被设置成blflist的条件
bool CAutoSetBlfListStrategy::IsAllowToSet(int iDssKeyID)
{
    // 如果是手动设置的话只有当原来的dsskey为blflist类型才能被设置
    if (!IsEnableAutoSetBlfList())
    {
        return dsskey_GetDsskeyType(iDssKeyID) == GetBindDsskeyType();
    }

    return CAutoSetBaseStrategy::IsAllowToSet(iDssKeyID);
}

bool CAutoSetBlfListStrategy::IsEnableAutoSetBlfList() const
{
    return m_bEnableAutoSetBlflist;
}

bool CAutoSetBlfListStrategy::GetDssKeyData(autoset_dsskeydata_t & tAutoSetData,
        DssKeyLogicData & tDsskeyData, int iDssKeyID, bool bHaveAutoSetData/* = true*/)
{
    bool bChanged = false;
    dsskey_GetData(iDssKeyID, tDsskeyData);
    if (bHaveAutoSetData)
    {
        // 当前dsskey为blflist，先比较有没有变化，有变化才重新设置
        if (tDsskeyData.eDKType == GetBindDsskeyType())
        {
            // 手动模式
            if (tDsskeyData.iLineIndex != tAutoSetData.iLineIndex
                    && !IsEnableAutoSetBlfList()
#if IF_BUG_32812
                    && !IsEnableCustomSetBlfList()
#endif
               )
            {
                return false;
            }
        }

        tAutoSetData >> tDsskeyData;
        bChanged = true;
    }
    else
    {
        // 后面为多余的blflist，重置掉
        if (tDsskeyData.eDKType == GetBindDsskeyType())
        {
#if IF_BUG_32812
            if (IsEnableCustomSetBlfList())
            {
                tDsskeyData.strExtension.clear();
            }
            else
#endif
                if (!IsEnableAutoSetBlfList())
                {
                    tDsskeyData.strExtension.clear();
                    tDsskeyData.strLabel.clear();
                    tDsskeyData.strValue.clear();
                }
                else
                {
                    tDsskeyData.reset();
                }

            bChanged = true;
        }
    }

    return bChanged;
}

bool CAutoSetBlfListStrategy::InherentProcess(msgObject & msg) const
{
    if (BLFLIST_DATA_UPDATE != msg.message)
    {
        return false;
    }

    // 找到所有的BLFList
    YLList<int> listDsskey;
    dsskey_getAllDsskeys(listDsskey, GetBindDsskeyType());

    int iStatus;
    yl::string strKey;

    // 将目前已有的Dsskey绑定相应的数据
    // http://10.2.1.199/Bug.php?BugID=92656
    YLList<int>::ListIterator itr = listDsskey.begin();
    for (; itr != listDsskey.end(); ++itr)
    {
        int iDsskeyId = (*itr);
        int iLine = dsskey_GetDsskeyLine(iDsskeyId);

        if (iLine == msg.wParam)
        {
            blf_SetDsskeyId(iDsskeyId, iLine, GetBLFkeyByDsskeyId(iDsskeyId));
        }
    }

    return true;
}

bool CAutoSetBlfListStrategy::UpdateDssKeyStatus(int iDssKeyID,
        const autoset_dsskeydata_t & tAutoSetData, bool bHaveData)
{
    // 设置blflist远端状态，并刷新ui
    if (bHaveData)
    {
        blf_SetDsskeyId(iDssKeyID, tAutoSetData.iLineIndex, tAutoSetData.strKey);
        dsskey_SetDsskeyStatus(iDssKeyID, (Dsskey_Status)tAutoSetData.iStatus);
    }

    return CAutoSetBaseStrategy::UpdateDssKeyStatus(iDssKeyID, tAutoSetData, bHaveData);
}

void CAutoSetBlfListStrategy::LoadConfig()
{
    m_bEnableAutoSetBlflist = configParser_GetConfigInt(kszAutoBlfListEnable) != 0;
#if IF_BUG_32812
    m_bEnableCustomSetBlflist = configParser_GetConfigInt(kszBLFListCustomSet) != 0;
#endif
}

bool CAutoSetBlfListStrategy::OnHigherPriorityStrategyQuery(int iDsskeyID) const
{
    return IsEnableAutoSetBlfList();
}

bool CAutoSetBlfListStrategy::AfterAutoSetProcess()
{
    blf_RefreshDsskey(m_listFlushDsskeyId);
    m_listFlushDsskeyId.clear();
    return true;
}

AUTOSET_SEQUENCE CAutoSetBlfListStrategy::GetSetDssKeySequence() const
{
    return (AUTOSET_SEQUENCE)configParser_GetConfigInt(kszAutoBlfListSequenceType);
}

bool CAutoSetBlfListStrategy::IsEnable() const
{
#if IF_BUG_32812
    // 非custom模式都认为使能
    return IsEnableAutoSetBlfList() || !IsEnableCustomSetBlfList();
#else
    // 手动模式总是开启
    return true;
#endif
}

bool CAutoSetBlfListStrategy::OnDsskeyReload(const dsskey_change_info_t & data)
{
    DssKey_Type eType = GetBindDsskeyType();
    bool bRet = false;

    if (data.eOldType == eType)
    {
        blf_UnBindDsskeyId(data.iDsskeyID, data.iOldLineID);
        bRet = true;
    }
#if IF_BUG_32812
    if (data.eNewType == eType)
    {
        if (IsEnableCustomSetBlfList())
        {
            UpdateBLFListData(data.iDsskeyID);
            AfterAutoSetProcess();
        }
    }
#endif

    return bRet;
}

#if IF_BUG_32812
bool CAutoSetBlfListStrategy::IsEnableCustomSetBlfList() const
{
    return m_bEnableCustomSetBlflist;
}

bool CAutoSetBlfListStrategy::GetManualSetDsskeyData(int iDssKeyID,
        YLVector<autoset_dsskeydata_t> & vecAutoSetData)
{
    autoset_dsskeydata_t tAutoSetData;
    if (GetAutoSetDataByDsskeyID(iDssKeyID, tAutoSetData))
    {
        vecAutoSetData.push_back(tAutoSetData);
        return true;
    }

    return false;
}

// 更新Dsskey的BLFList数据
void CAutoSetBlfListStrategy::UpdateBLFListData(int iDssKeyID)
{
    autoset_dsskeydata_t tAutoSetData;
    if (!GetAutoSetDataByDsskeyID(iDssKeyID, tAutoSetData))
    {
        return;
    }

    // 根据链表中获取的数据去生成dsskeydata，保存在tDsskeyData中
    DssKeyLogicData tDsskeyData;
    bool bRet = GetDssKeyData(tAutoSetData, tDsskeyData, iDssKeyID);

    if (bRet)
    {
        // 把该dsskey数据存入内存
        dsskey_SetDsskeyData(iDssKeyID, tDsskeyData, GetSetDssKeyFlag(iDssKeyID));
        // 更新dsskey各自类型所对应的远端状态，并刷新ui
        UpdateDssKeyStatus(iDssKeyID, tAutoSetData, true);
    }
}

bool CAutoSetBlfListStrategy::GetAutoSetDataByDsskeyID(int iDssKeyID,
        autoset_dsskeydata_t & tAutoSetData)
{
    int iAccountId = dsskey_GetDsskeyLine(iDssKeyID);
    tAutoSetData << GetBindDsskeyType() << iAccountId;

    return blf_IsAccountHaveBLFListData(iAccountId)
           && blf_GetBLFListAutoSetDataBykey(iDssKeyID, tAutoSetData);
}
#endif

#endif //IF_FEATURE_BLFLIST
#endif //IF_FEATURE_AUTOTEST
