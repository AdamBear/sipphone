#ifdef IF_FEATURE_DSSKEY
#if IF_FEATURE_PAGING
#include "talkdsskey_inc.h"

CDsskeyPagingAction::CDsskeyPagingAction()
    : CDsskeyBindAction(DT_PAGING)
{

}

bool CDsskeyPagingAction::OnKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData,
                                     void* pExtraData)
{
    CSessionStateProxy focus;

#if IF_BUG_27033
    if (!focus && logicData.strValue.empty())
    {
        // 没有通话存在时
        return EnterDialUI(logicData);
    }
    else
    {
        // 使用Dsskey呼出
        return CallOutByDsskey(logicData);
    }
#else
    if (!commonUnits_BeMulticastIP(logicData.strValue))
    {
        DSSKEY_ERR("Invalid Multicat IP !");
        return false;
    }

    if (!focus)
    {
        // 无焦点通话呼出
        return talklogic_CallPagingOut(
                   logicData.strLabel.c_str(),
                   logicData.strValue.c_str(),
                   dsskeyDataID,
                   logicData.strExtension.c_str());
    }

    // 参考T2X，有Paging呼出，且焦点焦点Session与当前绑定Session不同(即使为NULL)，则什么也不做
    if (talklogic_BeHavePageOutCall()
            && focus != _DsskeyBindManager.GetSessionByDsskey(dsskeyDataID))
    {
        return false;
    }

    return CallOutByDsskey(logicData);
#endif
}

bool CDsskeyPagingAction::ProcessNoBindDsskey(const CSessionStateProxy& proxy, const DssKeyLogicData& logicData)
{
    if (proxy.IsInherits(ROUTINE_PRETRAN))
    {
        return false;
    }

    ROUTINE_STATE eRoutine = proxy.GetInheritsRoutine();
#if IF_BUG_27033
    if (ROUTINE_DIALING == eRoutine)
    {
        proxy.ExitSession();
        return ProcessCallOut(logicData);
    }
    else if (ROUTINE_TALKING == eRoutine)
    {
        return ProcessCallOut(logicData);
    }
    else if (ROUTINE_FINISHED == eRoutine
             || proxy.IsTalkSession())
#else
    if (ROUTINE_DIALING == eRoutine
            || ROUTINE_FINISHED == eRoutine
            || ROUTINE_RINGING == eRoutine
            || proxy.IsTalkSession())
#endif
    {
        return talklogic_CallPagingOut(logicData.strLabel.c_str(), logicData.strValue.c_str(),
                                       logicData.iDssKeyID, logicData.strExtension.c_str());
    }

    return CDsskeyBindAction::ProcessNoBindDsskey(proxy, logicData);
}

#if IF_BUG_27033
bool CDsskeyPagingAction::GetCallOutNumber(int dsskeyID, yl::string& strCallOutNumber,
        const DssKeyLogicData& logicData, void* pExtraData, int nExtType)
{
    if (logicData.strValue.empty())
    {
        return false;
    }
    strCallOutNumber = logicData.strValue;
    return !strCallOutNumber.empty();
}
#endif

#endif //IF_FEATURE_PAGING
#endif  //IF_FEATURE_DSSKEY
