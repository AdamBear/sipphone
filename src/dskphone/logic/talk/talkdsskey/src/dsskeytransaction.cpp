#ifdef IF_FEATURE_DSSKEY
#include "talkdsskey_inc.h"

CDsskeyTransAction::CDsskeyTransAction(void)
    : CDssTalkBaseAciton(DT_TRAN)
{

}

bool CDsskeyTransAction::OnKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData,
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

    if (!logicData.strValue.empty()
            && ProcessNoBindDsskey(focus, logicData))
    {
        // Talk模块须提供外部发起TRAN的接口
        return true;
    }

    // 调用接口进入Transfer界面
    return talklogic_UIEvent(focus, TALK_ACTION_TRAN);
}

#endif  //IF_FEATURE_DSSKEY
