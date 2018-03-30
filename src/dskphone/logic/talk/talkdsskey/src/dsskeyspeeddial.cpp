#ifdef IF_FEATURE_DSSKEY

#include "talkdsskey_inc.h"

bool CDsskeySpeeddialAciton::OnKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData, void* pExtraData)
{
    //http://bugfree.yealink.com/Bug.php?BugID=120208
    //如果是在conf拨号界面按blf/blflist/speeddial则将value输入
    CSessionStateProxy focus;
    if (focus.GetRoutineState() == ROUTINE_DIALING)
    {
        CCallInfo* pCallInfo = focus.GetCallInfo();
        if (pCallInfo != NULL && focus.GetDialUIType() == DIAL_UI_CONFERENCE)
        {
            pCallInfo->SetDialedNumber(pCallInfo->GetDialText() + logicData.strValue);
            return true;
        }
    }

    if (!logicData.strValue.empty())
    {
        return CallOutByDsskey(logicData);
    }
#if IF_FEATURE_FAVORITE_DIR
    else
    {
        etl_NotifyApp(false, IDLE_MSG_NUMBER_EMPTY, 0, 0);
    }
#endif //IF_FEATURE_FAVORITE_DIR

    return false;
}

bool CDsskeySpeeddialAciton::GetCallOutParam(int iActionType, const CSessionStateProxy& proxy, const DssKeyLogicData& logicData, CallOut_Param& param)
{
    CDssTalkBaseAciton::GetCallOutParam(iActionType, proxy, logicData, param);

    switch (iActionType)
    {
    case DSSKEY_ACTION_CALLOUT:
        {
            GetLigaturesText(proxy, logicData, param);
        }
        break;
    }

    return true;
}

bool CDsskeySpeeddialAciton::ProcessNoBindDsskey(const CSessionStateProxy& proxy, const DssKeyLogicData& logicData)
{
    if (proxy.IsInherits(ROUTINE_RETRIEVE_CALLPARK))
    {
        return OnActionSend(proxy, logicData.strValue);
    }
    else if (CDssTalkBaseAciton::ProcessNoBindDsskey(proxy, logicData))
    {
        return true;
    }

    return CDsskeyBindAction::ProcessNoBindDsskey(proxy, logicData);
}

#endif
