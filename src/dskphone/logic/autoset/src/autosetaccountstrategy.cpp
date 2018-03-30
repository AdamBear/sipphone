#ifdef IF_FEATURE_AUTOTEST
#include "autoset_inc.h"

CAutoSetAccountStrategy::CAutoSetAccountStrategy(void)
{
    m_eAutoSetType = AUTOSET_ACCOUNT;
    LoadConfig();
}

bool CAutoSetAccountStrategy::IsNeedToAutoSet(msgObject & msg)
{
    bool bEnable = false;

    if (msg.message == ACCOUNT_STATUS_CHANGED)
    {
        int iAccountID = msg.wParam;
        /* 账号注册上之后（注册上前被其它低优先策略抢占）即使本策略被禁用也要能够抢占回来 */
        if (m_eAutoSetType == AUTOSET_ACCOUNT
                && !IsEnable()
                && acc_IsAccountAvailable(iAccountID))
        {
            if (dsskey_CountsDsskeyByAccID(iAccountID, true) > dsskey_CountsDsskeyByAccID(iAccountID))
            {
                bEnable = true;
                SetClearFlag(bEnable);
                return true;
            }
        }
    }
    
    if (CAutoSetBaseStrategy::IsNeedToAutoSet(msg))
    {
        bEnable = true;
    }
    else if (CONFIG_MSG_BCAST_CHANGED == msg.message
                 && msg.wParam == ST_DEFAULT_ACCOUNT)
    {
        bEnable = IsEnable();
    }
    else if (CONFIG_MSG_BCAST_CHANGED == msg.message
             && ST_AUTOSET_LINEKEY == msg.wParam)
    {
        bEnable = OnSwitchChanged();
    }

    return bEnable;
}

bool CAutoSetAccountStrategy::MakeAutoSetData(YLVector<autoset_dsskeydata_t> & vecAutoSetData)
{
    YLList<int> list;
    DssKey_Type eType = GetBindDsskeyType();
    dsskey_getAllDsskeys(list, eType, true);

    for (int iAccount = 0; iAccount < acc_AccountNum(); iAccount++)
    {
        if (!acc_IsInFinalRegisterState(iAccount))
        {
            continue;
        }

        int iNum = GetAccLineKeyNumber(iAccount);

        for (int i = 0; i < iNum; ++i)
        {
            autoset_dsskeydata_t tAutoSetData;
            tAutoSetData.eDKType = eType;
            tAutoSetData.iLineIndex = iAccount;
            SyncLinekeyLabel(list, tAutoSetData, iAccount);
            vecAutoSetData.push_back(tAutoSetData);
        }
    }

#ifdef IF_FEATURE_H323
    // H323也支持自动配置，并且只有一个账号 //http://10.2.1.199/Bug.php?BugID=94699
    if (IsEnable()
            && acc_IsAccountAvailable(H323ACCOUNTID))
    {
        autoset_dsskeydata_t tAutoSetData;
        tAutoSetData.eDKType = eType;
        tAutoSetData.iLineIndex = H323ACCOUNTID;
        vecAutoSetData.push_back(tAutoSetData);
    }

#endif //IF_FEATURE_H323

    return true;
}

// 加载配置
void CAutoSetAccountStrategy::LoadConfig()
{
    m_eAutoSetSwitch = (AUTOSET_ACCOUNT_TYPE)configParser_GetConfigInt(kszAutoLinekeysSwitch);
}

bool CAutoSetAccountStrategy::UpdateDssKeyStatus(int iDssKeyID,
        const autoset_dsskeydata_t & tAutoSetData, bool bHaveData)
{
    // 设置账号注册状态
    if (bHaveData)
    {
        dsskey_UpdateLineStatus(iDssKeyID);
    }

    return CAutoSetBaseStrategy::UpdateDssKeyStatus(iDssKeyID, tAutoSetData, bHaveData);
}

bool CAutoSetAccountStrategy::OnLowerPriorityStrategyQuery(int iDsskeyID) const
{
    // 账号未注册，允许低优先级策略抢占
    int iLineID = dsskey_GetDsskeyLine(iDsskeyID);
    return acc_IsDisableStatus(iLineID);
}

bool CAutoSetAccountStrategy::IsEnable() const
{
    return m_eAutoSetSwitch == AUTO_ACC_AT;
}

bool CAutoSetAccountStrategy::OnSwitchChanged()
{
    bool bOldAutoSetSwitch = IsEnable();
    LoadConfig();
    bool bNewAutoSetSwitch = IsEnable();

    DSSKEY_INFO("OnSwitchChanged type=[%d] switch=[%d] old=[%d] new=[%d]", m_eAutoSetType,
                m_eAutoSetSwitch, bOldAutoSetSwitch, bNewAutoSetSwitch);

    if (bOldAutoSetSwitch && !bNewAutoSetSwitch)
    {
        // 单独的策略切换则不需要
        bool bRun = m_eAutoSetSwitch == AUTO_ACC_NONE
                    || (m_eAutoSetSwitch > AUTO_ACC_NONE && IsDisablePassive());
        SetClearFlag(bRun);
        return bRun;
    }

    return bNewAutoSetSwitch;
}

bool CAutoSetAccountStrategy::IsDisablePassive() const
{
    return m_eAutoSetSwitch > AUTO_ACC_NONE && !IsEnable();
}

int CAutoSetAccountStrategy::GetAccLineKeyNumber(int iAccount) const
{
    // 该账号需要的Linekey个数
    return configParser_GetCfgItemIntValue(kszAccountSCALineKeyNumber, iAccount);
}

bool CAutoSetAccountStrategy::SyncLinekeyLabel(YLList<int> & listDsskey,
        autoset_dsskeydata_t & tData, int iAccountID)
{
    for (YLList<int>::iterator it = listDsskey.begin(); it != listDsskey.end(); ++it)
    {
        int iDsskeyID = *it;

        if (dsskey_GetDsskeyLine(iDsskeyID, DSS_ATTR_NATIVE) == iAccountID)
        {
            // 直接删除，后续减少遍历次数
            listDsskey.erase(it);
            tData.strLabel = dsskey_GetDsskeyLabel(iDsskeyID, DSS_ATTR_NATIVE);
            return true;
        }
    }

    return false;
}

bool CAutoSetAccountStrategy::PreSaveToFile(YLList<int> & listDsskey)
{
    /* 保存到文件之后需要单独 刷新一遍 AutoLabel
    WARRING: AutoLabel 数据不需要保存，只是临时文件
    TODO:: 后续改成读写临时文件之后不需要做这一遍操作，可以在Autoset主流程里面完成 */
    int iAccountNum = acc_AccountNum();
    int * pAccountNumIndex = new int[iAccountNum];
    memset(pAccountNumIndex, 0, iAccountNum * sizeof(int));
    DssKeyLogicData tDsskeyData;

    for (YLList<int>::iterator it = listDsskey.begin(); it != listDsskey.end(); ++it)
    {
        int iDsskeyID = *it;
        int iLineID = dsskey_GetDsskeyLine(iDsskeyID);

        if (iLineID < 0
                || iLineID >= iAccountNum)
        {
            continue;
        }

        if (!acc_IsAccountEnable(iLineID)
                || dsskey_GetDsskeyType(iDsskeyID) != GetBindDsskeyType()
                || !acc_IsAutoLabelEnable(iLineID))
        {
            continue;
        }

        dsskey_GetData(iDsskeyID, tDsskeyData);

        // 写文件的策略，如果当前label等于配置值，不需要执行autolabel流程
        if (GetSetDssKeyFlag(iDsskeyID) & DSSKEY_SET_WRITE)
        {
            if (!tDsskeyData.strLabel.empty()
                    && tDsskeyData.strLabel == dsskey_GetDsskeyLabel(iDsskeyID, DSS_ATTR_NATIVE))
            {
                continue;
            }

            tDsskeyData.strLabel.clear();
        }

        if (acc_GetCustomLabel(iLineID, pAccountNumIndex[iLineID]++, tDsskeyData.strLabel))
        {
            dsskey_SetDsskeyData(iDsskeyID, tDsskeyData, GetSetDssKeyFlag(iDsskeyID));
            // 不保存但是需要刷新
            m_listFlushDsskeyId.push_back(iDsskeyID);
        }
    }

    delete [] pAccountNumIndex;
    return CAutoSetBaseStrategy::PreSaveToFile(listDsskey);
}

bool CAutoSetAccountStrategy::OnDsskeyReload(const dsskey_change_info_t & data)
{
    // BT需要特殊处理，由于其绑定属性，优先级会比Line还高
    return data.eOldType == DT_BTLINE || data.eNewType == DT_BTLINE;
}

#endif // IF_FEATURE_AUTOTEST

