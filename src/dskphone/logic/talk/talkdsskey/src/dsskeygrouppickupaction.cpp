#ifdef IF_FEATURE_DSSKEY
#include "talkdsskey_inc.h"

CDsskeyGroupPickupAction::CDsskeyGroupPickupAction()
    : CDsskeyBindAction(DT_GROUPPICKUP)
{

}

bool CDsskeyGroupPickupAction::OnKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData,
        void* pExtraData)
{
    yl::string callOutNumber;
    dsskey_GetCallOutNumber(dsskeyDataID, callOutNumber);
    if (callOutNumber.empty())
    {
        return false;
    }

    return CallOutByDsskey(logicData);
}

bool CDsskeyGroupPickupAction::GetCallOutNumber(int dsskeyDataID, yl::string& strCallOutNumber,
        const DssKeyLogicData& logicData, void* pExtraData, int nExtType)
{
    if (!logicData.strValue.empty())
    {
        strCallOutNumber = logicData.strValue;
    }
    else
    {
        strCallOutNumber = acc_GetPickupCodeByType(logicData.iLineIndex, PT_GROUP);
    }

    return !strCallOutNumber.empty();
}

bool CDsskeyGroupPickupAction::GetCallOutParam(int iActionType, const CSessionStateProxy& proxy, const DssKeyLogicData& logicData, CallOut_Param& param)
{
    if (iActionType == DSSKEY_ACTION_MAKE_CALL)
    {
        param.eCallOutType = TCT_PICKUP;
    }

    return CDssTalkBaseAciton::GetCallOutParam(iActionType, proxy, logicData, param);
}

#endif  //IF_FEATURE_DSSKEY
