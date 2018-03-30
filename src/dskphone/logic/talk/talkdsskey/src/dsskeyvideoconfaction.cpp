#ifdef IF_FEATURE_DSSKEY

#include "talkdsskey_inc.h"

#ifdef IF_SUPPORT_VIDEO

CDsskeyVideoConfAction::CDsskeyVideoConfAction()
    : CDssTalkBaseAciton(DT_VIDEO_CONF)
{

}

CDsskeyVideoConfAction::~CDsskeyVideoConfAction()
{

}

bool CDsskeyVideoConfAction::OnKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData,
                                        void* pExtraData)
{
    DSSKEY_INFO("video conf on key press");
    // 如果当前只有一路话路且处于通话状态，则新起话路，并在对方接起后自动建立会议
    // 号码不为空则呼出该号码
    CSessionStateProxy focus;

    if (!logicData.strValue.empty())
    {
        if (ProcessNoBindDsskey(focus, logicData))
        {
            return true;
        }

        // video conf 支持选择号码
        ProcessCallOut(logicData);
        return true;
    }

    // 号码空则还是当做Conf Hardkey处理
    return talklogic_UIEvent(focus, TALK_ACTION_CONF);
}

bool CDsskeyVideoConfAction::GetCallOutNumber(int dsskeyID, yl::string& strCallOutNumber,
        const DssKeyLogicData& logicData, void* pExtraData, int nExtType)
{
    DSSKEY_INFO("GetCallOutNumber va[%s]", logicData.strValue.c_str());
    strCallOutNumber = logicData.strValue;
    return true;
}

bool CDsskeyVideoConfAction::ProcessNoBindDsskey(const CSessionStateProxy& proxy, const DssKeyLogicData& logicData)
{
    if (proxy.IsInherits(ROUTINE_TALKING)
            || proxy.IsInherits(ROUTINE_DIALING)
            || proxy.IsInherits(SESSION_LOCALCONF))
    {
        return SendTalkEvent(proxy, logicData, DSSKEY_ACTION_CONF_TO);
    }

    return false;
}

bool CDsskeyVideoConfAction::GetCallOutParam(int iActionType, const CSessionStateProxy& proxy, const DssKeyLogicData& logicData, CallOut_Param& param)
{
    if (iActionType == DSSKEY_ACTION_MAKE_CALL)
    {
        param.bAudioOnly = false;
    }

    return CDssTalkBaseAciton::GetCallOutParam(iActionType, proxy, logicData, param);
}

#endif
#endif
