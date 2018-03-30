#ifdef IF_FEATURE_AUTOTEST

#ifdef IF_BT_SUPPORT_PHONE
#include "autoset_inc.h"

CAutoSetBTStrategy::CAutoSetBTStrategy(void)
    : m_bBTActive(configParser_GetConfigInt(kszBluetoothSupportTalk)), m_iBindDsskeyID(INVALID_DSSKEY_ID)
{
    m_eAutoSetType = AUTOSET_BT;
}

bool CAutoSetBTStrategy::IsNeedToAutoSet(msgObject & msg)
{
    if (CAutoSetBaseStrategy::IsNeedToAutoSet(msg))
    {
    }
    else if (BLUETOOTH_MSG_SETDSSKEY == msg.message)
    {
		DSSKEY_INFO("BLUETOOTH_MSG_SETDSSKEY [%d][%d]", msg.wParam, msg.lParam);
        m_bBTActive = (msg.wParam == 1);
    }
	else if (CONFIG_MSG_BCAST_CHANGED == msg.message)
	{
		return false;
	}

    return true;
}

bool CAutoSetBTStrategy::GetDssKeyData(autoset_dsskeydata_t & tAutoSetData,
                                       DssKeyLogicData & tDsskeyData, int iDssKeyID, bool bHaveAutoSetData /*= true*/)
{
	DssKey_Type eDKType = GetBindDsskeyType();
    // 开启的情况下设置一个数据
    if (m_bBTActive && (iDssKeyID == m_iBindDsskeyID || m_iBindDsskeyID == INVALID_DSSKEY_ID))
    {
        dsskey_GetData(iDssKeyID, tDsskeyData);

		if (tDsskeyData.eDKType != eDKType)
		{
			tDsskeyData.reset();
			tDsskeyData.eDKType = eDKType;
		}

        m_iBindDsskeyID = iDssKeyID;
        return true;
    }
	else if(!m_bBTActive)
	{
		if (dsskey_GetDsskeyType(iDssKeyID) == eDKType)
		{
			dsskey_GetData(iDssKeyID, tDsskeyData);
			return true;
		}		
	}

    return CAutoSetBaseStrategy::GetDssKeyData(tAutoSetData, tDsskeyData, iDssKeyID, bHaveAutoSetData);
}

bool CAutoSetBTStrategy::UpdateDssKeyStatus(int iDssKeyID,
        const autoset_dsskeydata_t & tAutoSetData, bool bHaveData)
{
    //需要判断状态
    /*
        1、未连接时
        2、已连接
        3、存在话路
    */
    if (GetBindDsskeyType() == dsskey_GetDsskeyType(iDssKeyID))
    {
        Dsskey_Status eStatus = DS_BTLINE_OFFLINE;

        if (talklogic_IsBtSessionExist())
        {
            eStatus = DS_BTLINE_USING;
        }
        else if (Bluetooth_IsMobileConnected())
        {
            eStatus = DS_BTLINE_ONLINE;
        }

        dsskey_SetDsskeyStatus(iDssKeyID, eStatus);
    }

    return CAutoSetBaseStrategy::UpdateDssKeyStatus(iDssKeyID, tAutoSetData, bHaveData);
}

bool CAutoSetBTStrategy::OnHigherPriorityStrategyQuery(int iDsskeyID) const
{
    // 未开启、未绑定
    return !m_bBTActive || iDsskeyID != m_iBindDsskeyID;
}

int CAutoSetBTStrategy::GetBindDsskeyID() const
{
    YLList<int> list;
    dsskey_getAllDsskeys(list, GetBindDsskeyType(), true);
    if (list.begin() == list.end())
    {
        dsskey_getAllDsskeys(list, GetBindDsskeyType(), false);
    }

    YLList<int>::ListIterator it = list.begin();
    if (it != list.end())
    {
        DSSKEY_INFO("BTStrategy bind to [%d]", *it);
        return *it;
    }

    return INVALID_DSSKEY_ID;
}

int CAutoSetBTStrategy::GetSetDssKeyFlag(int iDsskeyID) const
{
    return DSSKEY_SET_NOTIFY | CAutoSetBaseStrategy::GetSetDssKeyFlag(iDsskeyID);
}

bool CAutoSetBTStrategy::OnLowerPriorityStrategyQuery(int iDsskeyID) const
{
    return !m_bBTActive;
}

bool CAutoSetBTStrategy::OnDsskeyReload(const dsskey_change_info_t & data)
{
    DssKey_Type eType = GetBindDsskeyType();
    if (data.eOldType == eType || data.eNewType == eType)
    {
        m_iBindDsskeyID = GetBindDsskeyID();
        return true;
    }

    return false;
}
#endif
#endif

