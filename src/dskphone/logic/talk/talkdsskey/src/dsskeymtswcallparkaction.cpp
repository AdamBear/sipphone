#ifdef IF_FEATURE_DSSKEY

#if IF_FEATURE_METASWITCH_CALLPARK
#include "talkdsskey_inc.h"

CDsskeyMTSWCallparkAction::CDsskeyMTSWCallparkAction(void)
    : CDssTalkBaseAciton(DT_MTSW_CALLPARK)
{

}

CDsskeyMTSWCallparkAction::~CDsskeyMTSWCallparkAction(void)
{

}

bool CDsskeyMTSWCallparkAction::OnKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData,
        void* pExtraData)
{
    // 如果不存在session
    if (!talklogic_SessionExist())
    {
        return false;
    }

    CSessionStateProxy focus;
    if (!focus)
    {
        return false;
    }

    if (!logicData.strValue.empty())
    {
        // Talk模块须提供外部发起TRAN的接口
        if (ProcessNoBindDsskey(focus, logicData))
        {
            return true;
        }
    }

    // 调用接口进入Transfer界面
    return talklogic_UIEvent(focus, TALK_ACTION_TRAN);
}

bool CDsskeyMTSWCallparkAction::ProcessNoBindDsskey(const CSessionStateProxy& proxy, const DssKeyLogicData& logicData)
{
    if (proxy.IsAllowTran()
            && proxy.IsInPreTrans())
    {
        return false;
    }

    CallOut_Param param;
    GetCallOutParam(DSSKEY_ACTION_MAKE_CALL, proxy, logicData, param);
    return proxy.AttendedTransfer(param);
}

#endif

#endif
