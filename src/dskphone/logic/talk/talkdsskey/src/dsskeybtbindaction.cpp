#ifdef IF_FEATURE_DSSKEY
#include "talkdsskey_inc.h"

#ifdef IF_BT_SUPPORT_PHONE
CDsskeyBTBindAction::CDsskeyBTBindAction()
    : CDsskeyBindAction(DT_BTLINE)
{

}

CDsskeyBTBindAction::~CDsskeyBTBindAction()
{
}

bool CDsskeyBTBindAction::OnKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData,
                                     void* pExtraData)
{
    //未连接时,打开自动连接定时器
    if (!Bluetooth_IsMobileConnected())
    {
        Bluetooth_SwitchAutoConnect(true);
        return true;
    }

    return CallOutByLine(logicData);
}

bool CDsskeyBTBindAction::OnDsskeyTypeModify(int dsskeyID, DssKey_Type oldType, DssKey_Type newType,
        void* pExtraData)
{
    if (pExtraData == NULL)
    {
        dsskey_change_info_t tChangeDssKeyInfo;
        tChangeDssKeyInfo.eNewType = newType;
        tChangeDssKeyInfo.iDsskeyID = dsskeyID;
        tChangeDssKeyInfo.eOldType = oldType;
        etl_NotifyAppEx(FALSE, DSSKEY_RELOAD_FINISHED, newType, oldType, sizeof(dsskey_change_info_t),
                        &tChangeDssKeyInfo);
    }

    return true;
}

bool CDsskeyBTBindAction::OnDebinded(int dsskeyDataID, const DssKeyLogicData& logicData,
                                     void* pExtraData)
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

    dsskey_SetDsskeyStatus(dsskeyDataID, eStatus);
    dsskey_refreshDSSKeyUI(dsskeyDataID);
    return true;
}

bool CDsskeyBTBindAction::ProcessNoBindDsskey(const CSessionStateProxy& proxy, const DssKeyLogicData& logicData)
{
    if (proxy.IsInherits(SESSION_LOCALCONF)
            || proxy.IsInherits(SESSION_TALKING))
    {
        return EnterDialUI(logicData);
    }
    else if (proxy.IsInherits(SESSION_BTDIALING)
             || proxy.IsInherits(SESSION_BTFINISHED))
    {
        return DialByCurrent(proxy, logicData);
    }

    return false;
}

bool CDsskeyBTBindAction::ProcessFocusDsskey(const CSessionStateProxy& proxy, const DssKeyLogicData& logicData)
{
    if (proxy.IsInherits(ROUTINE_BTRINGING))
    {
        return talklogic_UIEvent(proxy, TALK_ACTION_ANSWER);
    }
    else if (proxy.IsInherits(ROUTINE_BTTALKING))
    {
        return EnterDialUI(logicData);
    }
    else if (proxy.IsInherits(SESSION_BTFINISHED)
             || proxy.IsInherits(SESSION_BTDIALING)
             || proxy.IsInherits(SESSION_BTCONNECTING))
    {
        return proxy.ExitSession();
    }

    return false;
}

#endif
#endif
