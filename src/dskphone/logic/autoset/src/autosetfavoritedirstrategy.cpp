#if IF_FEATURE_FAVORITE_DIR
#include "autoset_inc.h"

CAutoSetFavoriteDirStrategy::CAutoSetFavoriteDirStrategy(void)
{
    m_eAutoSetType = AUTOSET_FAVORITE_DIR;
    LoadConfig();
}

bool CAutoSetFavoriteDirStrategy::IsNeedToAutoSet(msgObject & msg)
{
    bool bEnable = false;
    if (msg.message == CONFIG_MSG_BCAST_CHANGED
            && msg.wParam == ST_AUTOSET_LOCAL_FAVORITE)
    {
        bEnable = OnSwitchChanged();
    }
    else if (msg.message == LOCAL_FAVORITE_CHANGED
             || CAutoSetBaseStrategy::IsNeedToAutoSet(msg))
    {
        bEnable = true;
    }

    return bEnable;
}

bool CAutoSetFavoriteDirStrategy::MakeAutoSetData(YLVector<autoset_dsskeydata_t> & vecAutoSetData)
{
    ContactListData refData;
    Dir_GetContactListByGroupId(&refData, kszFavoriteGroupId, LOCAL_DIR);
    int nMaxSize = refData.m_listId.size();
    DssKey_Type eType = GetBindDsskeyType();

    for (int i = 0; i < nMaxSize; i++)
    {
        int nContactId = refData.m_listId[i];
        SmartPtr pDetail = Dir_GetDetailById(nContactId, LOCAL_DIR);
        LocalDirContactRecord * pRecord = GetLocalContact(pDetail);
        if (pRecord != NULL)
        {
            autoset_dsskeydata_t tAutoSetData;
            tAutoSetData << *pRecord << eType;
            tAutoSetData << (pRecord->m_nLine < 0 ? acc_GetDefaultAccount() : pRecord->m_nLine);
            vecAutoSetData.push_back(tAutoSetData);
        }
    }
    return true;
}

// 加载配置
void CAutoSetFavoriteDirStrategy::LoadConfig()
{
    m_bAutoSetSwitch = LocalDir_IsFavoriteEnable();
}

bool CAutoSetFavoriteDirStrategy::IsEnable() const
{
    return m_bAutoSetSwitch;
}

bool CAutoSetFavoriteDirStrategy::OnSwitchChanged()
{
    bool bOldAutoSetSwitch = IsEnable();
    LoadConfig();
    bool bNewAutoSetSwitch = IsEnable();

    //只有从启用到不启用时清除所有自动设置的
    if (bOldAutoSetSwitch && !bNewAutoSetSwitch)
    {
        SetClearFlag(true);
        return true;
    }

    return bNewAutoSetSwitch;
}

LocalDirContactRecord * CAutoSetFavoriteDirStrategy::GetLocalContact(SmartPtr & pDetail)
{
    if (pDetail.IsEmpty() || DIR_TYPE_LOCAL_CONTACT != pDetail->GetType())
    {
        return NULL;
    }

    return (LocalDirContactRecord *)pDetail.Get();
}

AUTOSET_SEQUENCE CAutoSetFavoriteDirStrategy::GetSetDssKeySequence() const
{
    return (AUTOSET_SEQUENCE)configParser_GetConfigInt(kszFavoriteSequenceType);
}

#endif
