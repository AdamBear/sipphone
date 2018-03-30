#ifdef IF_FEATURE_DSSKEY
#include "talkdsskey_inc.h"


CDsskeyTalkAction::CDsskeyTalkAction(DssKey_Type eDsskeyType, TALK_ACTION_TYPE eTalkActionType)
    : IDsskeyImplAction(eDsskeyType), m_eTalkActionType(eTalkActionType)
{

}

bool CDsskeyTalkAction::OnKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData,
                                   void* pExtraData)
{
    return SendTalkAction();
}

bool CDsskeyTalkAction::SendTalkAction()
{
    // 获得焦点session
    CSessionStateProxy focus;
    return focus && talklogic_UIEvent(focus, m_eTalkActionType);
}

#endif  //IF_FEATURE_DSSKEY
