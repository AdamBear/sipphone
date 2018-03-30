#ifdef IF_FEATURE_DSSKEY
#include "talkdsskey_inc.h"

#if IF_BUG_30965

CDsskeyUnParkAction::CDsskeyUnParkAction()
    : CDsskeyBindAction(DT_UNPARK)
{

}

CDsskeyUnParkAction::~CDsskeyUnParkAction()
{

}

bool CDsskeyUnParkAction::OnKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData,
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

bool CDsskeyUnParkAction::GetCallOutNumber(int dsskeyID, yl::string& strCallOutNumber,
        const DssKeyLogicData& logicData, void* pExtraData, int nExtType)
{
    if (logicData.strValue.empty())
    {
        return false;
    }
    strCallOutNumber = logicData.strValue;
    return !strCallOutNumber.empty();
}

bool CDsskeyUnParkAction::ProcessUnFocusDsskey(const CSessionStateProxy& proxy, const DssKeyLogicData& logicData)
{
    if (proxy.IsInherits(ROUTINE_TALKING))
    {
        return false;
    }

    return CDsskeyBindAction::ProcessUnFocusDsskey(proxy, logicData);
}

#endif //#if IF_BUG_30965
#endif
