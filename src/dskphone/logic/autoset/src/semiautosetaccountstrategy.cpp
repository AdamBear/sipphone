#ifdef IF_FEATRUE_MANUAL_AUTOSET
#include "autoset_inc.h"

CSemiAutoSetAccountStrategy::CSemiAutoSetAccountStrategy(void)
    : CAutoSetAccountStrategy()
{
    m_eAutoSetType = AUTOSET_SEMI_ACCOUNT;
}

bool CSemiAutoSetAccountStrategy::MakeAutoSetData(YLVector<autoset_dsskeydata_t> & vecAutoSetData)
{
    YLList<int> list;
    DssKey_Type eType = GetBindDsskeyType();
    dsskey_getAllDsskeys(list, eType, true);

    for (int iAccountID = 0; iAccountID < acc_AccountNum(); ++iAccountID)
    {
        int iIndex = 0;
        int iNeedCount = GetAccLineKeyNumber(iAccountID);

        if (!acc_IsAccountEnable(iAccountID))
        {
            continue;
        }

        for (YLList<int>::iterator it = list.begin(); it != list.end(); ++it)
        {
            if (dsskey_GetDsskeyLine(*it, DSS_ATTR_NATIVE) == iAccountID)
            {
                // 优先使用已经有的
                if (++iIndex <= iNeedCount)
                {
                    m_listBindDsskeyID.push_back(*it);
                    continue;
                }

                break;
            }
        }

        // 不够需要凑出来
        while (++iIndex <= iNeedCount)
        {
            autoset_dsskeydata_t tAutoSetData;
            tAutoSetData.eDKType = eType;
            tAutoSetData.iLineIndex = iAccountID;
            vecAutoSetData.push_back(tAutoSetData);
        }
    }

    return true;
}

bool CSemiAutoSetAccountStrategy::IsEnable() const
{
    return GetAutoSwitch() >= AUTO_ACC_MT;
}

int CSemiAutoSetAccountStrategy::GetSetDssKeyFlag(int iDsskeyID) const
{
    return GetAutoSwitch() == AUTO_ACC_MT ? (DSSKEY_SET_AUTO_LABEL | (DSSKEY_SET_NORMAL & (~DSSKEY_SET_REFRESH))) : DSSKEY_SET_ONLY;
}

bool CSemiAutoSetAccountStrategy::AfterAutoSetProcess()
{
    m_listBindDsskeyID.clear();
    return CAutoSetAccountStrategy::AfterAutoSetProcess();
}

bool CSemiAutoSetAccountStrategy::IsDsskeyBind(int iDsskeyID) const
{
    return iDsskeyID != INVALID_DSSKEY_ID
           && yl::find(m_listBindDsskeyID.begin(), m_listBindDsskeyID.end(),
                       iDsskeyID) != m_listBindDsskeyID.end();
}

void CSemiAutoSetAccountStrategy::SetClearFlag(bool bFlag)
{
    /* TODO: 只有等于2的模式下才应该生效, 被关闭的时候需要清理autolabel */
    if (GetClearFlag() && !bFlag)
    {
        YLList<int> list;
        dsskey_getAllDsskeys(list, GetBindDsskeyType(), true);
        DssKeyLogicData tDsskeyData;

        for (YLList<int>::iterator it = list.begin(); it != list.end(); ++it)
        {
            yl::string strLabel = dsskey_GetDsskeyLabel(*it);
            yl::string strNativeLabel = dsskey_GetDsskeyLabel(*it, DSS_ATTR_NATIVE);

            if (strLabel != strNativeLabel)
            {
                dsskey_SetDsskeyLabel(*it, strNativeLabel);
                dsskey_refreshDSSKeyUI(*it);
            }
        }
    }

    CAutoSetBaseStrategy::SetClearFlag(bFlag);
}

#endif
