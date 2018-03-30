#ifdef IF_FEATURE_DSSKEY
#include "talkdsskey_inc.h"

CDsskeyMeetMeConfAction::CDsskeyMeetMeConfAction()
    : CDsskeyBindAction(DT_MEET_ME_CONFERENCE)
{

}

bool CDsskeyMeetMeConfAction::OnKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData,
        void* pExtraData)
{
    if (!logicData.strValue.empty()
            && configParser_GetConfigInt(kszBroadsoftActive) != 0)
    {
        return CallOutByDsskey(logicData);
    }

    return false;
}

bool CDsskeyMeetMeConfAction::ProcessNoBindDsskey(const CSessionStateProxy& proxy, const DssKeyLogicData& logicData)
{
    if (proxy.IsInherits(ROUTINE_DIALING))
    {
        return DialByCurrent(proxy, logicData);
    }
    else if (proxy.IsInherits(ROUTINE_AOC_FINISH)
             || proxy.IsInherits(ROUTINE_PARK_NOTIFY)
             || proxy.IsInherits(ROUTINE_TALKING))
    {
        return CDsskeyBindAction::ProcessNoBindDsskey(proxy, logicData);
    }

    return false;
}

bool CDsskeyMeetMeConfAction::ProcessFocusDsskey(const CSessionStateProxy& proxy, const DssKeyLogicData& logicData)
{
    if (proxy.IsInherits(ROUTINE_DIALING)
            || proxy.IsInherits(ROUTINE_TALKING))
    {
        return CDsskeyBindAction::ProcessFocusDsskey(proxy, logicData);
    }
    else if (proxy.IsInherits(ROUTINE_AOC_FINISH)
             || proxy.IsInherits(ROUTINE_PARK_NOTIFY))
    {
        return ProcessCallOut(logicData);
    }

    return false;
}

#endif  //IF_FEATURE_DSSKEY
