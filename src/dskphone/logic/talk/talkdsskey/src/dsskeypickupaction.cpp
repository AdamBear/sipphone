#ifdef IF_FEATURE_DSSKEY
#include "talkdsskey_inc.h"


CDsskeyPickupAction::CDsskeyPickupAction()
    : CDsskeyBindAction(DT_PICKUP)
{

}

bool CDsskeyPickupAction::OnKeyPress(int iDsskey, const DssKeyLogicData& logicData,
                                     void* pExtraData)
{
    CSessionStateProxy focus;
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
}

bool CDsskeyPickupAction::GetCallOutNumber(int dsskeyID, yl::string& strCallOutNumber,
        const DssKeyLogicData& logicData, void* pExtraData, int nExtType)
{
    return GetDPickupCallOutNumber(dsskeyID, strCallOutNumber, logicData.strValue,
                                   logicData.strExtension, logicData.iLineIndex);
}

bool CDsskeyPickupAction::GetCallOutParam(int iActionType, const CSessionStateProxy& proxy, const DssKeyLogicData& logicData, CallOut_Param& param)
{
    if (iActionType == DSSKEY_ACTION_MAKE_CALL)
    {
        param.eCallOutType = TCT_PICKUP;
    }

    return CDssTalkBaseAciton::GetCallOutParam(iActionType, proxy, logicData, param);
}

#endif  //IF_FEATURE_DSSKEY
