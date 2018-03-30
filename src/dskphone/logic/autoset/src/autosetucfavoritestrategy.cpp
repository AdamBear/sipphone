#ifdef IF_FEATURE_AUTOTEST

#if FEATURE_BROADTOUCH_UC
#include "autoset_inc.h"

CAutoSetUCFavoriteStrategy::CAutoSetUCFavoriteStrategy(void)
{
    m_eAutoSetType = AUTOSET_UCFAVORITE;
    LoadConfig();
}

CAutoSetUCFavoriteStrategy::~CAutoSetUCFavoriteStrategy(void)
{
}

bool CAutoSetUCFavoriteStrategy::IsNeedToAutoSet(msgObject & msg)
{
    bool bNeedAuto = false;

    if (CAutoSetBaseStrategy::IsNeedToAutoSet(msg)
            || msg.message == PRESENCE_MESSAGE_FAVORITE_CHANGE)
    {
        bNeedAuto = true;
    }
    else if (CONFIG_MSG_BCAST_CHANGED == msg.message
             && ST_AUTO_FAVORITE == msg.wParam)
    {
        LoadConfig();
        bNeedAuto = true;
    }

    return bNeedAuto;
}

// 生成自动设置数据
bool CAutoSetUCFavoriteStrategy::MakeAutoSetData(YLVector<autoset_dsskeydata_t> & vecAutoSetData)
{
    ContactBuddyListData ucData;
    if (configParser_GetConfigInt(kszEnableUC) != 0
            && configParser_GetConfigInt(kszBroadsoftActive) != 0)
    {
        GroupKey key = FAVOURITE_ID;
        Presence_GetListByGroupId(&ucData, key, 0);
    }

    PresenceInfo refData;
    char szLocalId[128] = {0};
    yl::string strNumber;
    autoset_dsskeydata_t tAutoSetData;

    IterPresenceInfo itr = ucData.m_list.begin();
    for (; itr != ucData.m_list.end(); itr++)
    {
        refData.m_nLocalId = (*itr);
        Presence_GetContactDetail(refData, PRESENCE_UC);

        // 转换ID为字符串
        memset(szLocalId, 0, sizeof(szLocalId));
        sprintf(szLocalId, "%d", refData.m_nLocalId);

        tAutoSetData.eDKType = GetBindDsskeyType();
        tAutoSetData.strValue = szLocalId;
        tAutoSetData.strLabel = refData.strContactName.empty() ?
                                refData.GetAttributeValue(ATTR_TYPE_NUMBER) : refData.strContactName;
        tAutoSetData.strExtension = refData.strAvatarInfo;
        tAutoSetData.iStatus = refData.ePresenceType;
        tAutoSetData.iLineIndex = 0;
        vecAutoSetData.push_back(tAutoSetData);
    }

    return true;
}

// 设置当前dsskey所对应的远端状态
bool CAutoSetUCFavoriteStrategy::UpdateDssKeyStatus(int iDssKeyID,
        const autoset_dsskeydata_t & tAutoSetData, bool bHaveData)
{
    if (bHaveData)
    {
        dsskey_SetDsskeyStatus(iDssKeyID,
                               (Dsskey_Status)Presence_MapStatus2DsskeyStatus((_PRESENCE_TYPE)tAutoSetData.iStatus));
    }

    return CAutoSetBaseStrategy::UpdateDssKeyStatus(iDssKeyID, tAutoSetData, bHaveData);
}

void CAutoSetUCFavoriteStrategy::LoadConfig()
{
    m_bEnableAuto = configParser_GetConfigInt(kszEnableAutoFavorite) != 0;
}

bool CAutoSetUCFavoriteStrategy::IsAllowToSet(int iDssKeyID)
{
    // 如果是手动设置的话只有当类型相同时才能被设置
    if (!m_bEnableAuto)
    {
        return dsskey_GetDsskeyType(iDssKeyID, DSS_ATTR_NATIVE) == GetBindDsskeyType();
    }

    return CAutoSetBaseStrategy::IsAllowToSet(iDssKeyID);
}

bool CAutoSetUCFavoriteStrategy::OnHigherPriorityStrategyQuery(int iDsskeyID) const
{
    return m_bEnableAuto;
}

AUTOSET_SEQUENCE CAutoSetUCFavoriteStrategy::GetSetDssKeySequence() const
{
    return (AUTOSET_SEQUENCE)configParser_GetConfigInt(kszUCFavoriteSequenceType);
}

#endif // FEATURE_BROADTOUCH_UC
#endif


