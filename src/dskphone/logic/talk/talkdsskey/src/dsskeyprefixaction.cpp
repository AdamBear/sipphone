#ifdef IF_FEATURE_DSSKEY
#include "talkdsskey_inc.h"

CDsskeyPrefixAction::CDsskeyPrefixAction()
    : CDssTalkBaseAciton(DT_PREFIX)
{

}

bool CDsskeyPrefixAction::OnKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData,
                                     void* pExtraData)
{
    // 是否被处理
    bool bHandled = true;

    // Value是否为空
    if (logicData.strValue.empty())
    {
        return false;
    }

    // 没有话路时，则创建拨号界面，并附带号码
    if (!talklogic_SessionExist())
    {
        return EnterDialUI(logicData);
    }
    else
    {
        // 获取焦点Session
        CSessionStateProxy focus;
        if (!focus)
        {
            // 交给未绑定的Dsskey处理
            // 拨号界面，将附带号码赋给当前界面
            // 通话中，新起一路，附带号码
            // 来电fwd界面，替换号码
            bHandled = ProcessNoBindDsskey(focus, logicData);
        }
    }

    return bHandled;
}

bool CDsskeyPrefixAction::ProcessNoBindDsskey(const CSessionStateProxy& proxy, const DssKeyLogicData& logicData)
{
    ROUTINE_STATE eRoutine = proxy.GetInheritsRoutine();
    if (ROUTINE_DIALING == eRoutine
            || ROUTINE_RINGING == eRoutine)
    {
        CCallInfo* pCallInfo = proxy.GetCallInfo();
        if (pCallInfo == NULL)
        {
            return false;
        }

        pCallInfo->SetDialedNumber(logicData.strValue);
        return true;
    }
    else if (ROUTINE_AOC_FINISH == eRoutine
             || ROUTINE_PARK_NOTIFY == eRoutine
             || ROUTINE_TALKING == eRoutine
             || SESSION_BETRANSFERRED == proxy.GetSessionState())
    {
        return EnterDialUI(logicData);
    }

    return false;
}

bool CDsskeyPrefixAction::GetCallOutParam(int iActionType, const CSessionStateProxy& proxy, const DssKeyLogicData& logicData, CallOut_Param& param)
{
    int iAccountID = logicData.iLineIndex;
    if (iAccountID == INVALID_ACCOUNTID)
    {
        iAccountID = acc_GetUsableAccount();
    }

    param.iAccountID = iAccountID;
    param.eCallOutType = TCT_PREFIX;

    if (param.strNumber.empty())
    {
        param.strNumber = logicData.strValue;
    }

    return CDssTalkBaseAciton::GetCallOutParam(iActionType, proxy, logicData, param);
}

#endif  //IF_FEATURE_DSSKEY
