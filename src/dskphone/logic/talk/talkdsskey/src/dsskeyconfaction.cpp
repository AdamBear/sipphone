#ifdef IF_FEATURE_DSSKEY
#include "talkdsskey_inc.h"

CDsskeyConfAction::CDsskeyConfAction()
    : CDssTalkBaseAciton(DT_CONF)
{

}

CDsskeyConfAction::~CDsskeyConfAction()
{

}

bool CDsskeyConfAction::OnKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData,
                                   void* pExtraData)
{
    // 如果当前只有一路话路且处于通话状态，则新起话路，并在对方接起后自动建立会议
    // 号码不为空则呼出该号码

    CSessionStateProxy focus;

    if (!logicData.strValue.empty())
    {
        if (!focus || focus.IsHide())
        {
            if (ProcessNoBindDsskey(focus, logicData))
            {
                return true;
            }
        }

        ProcessCallOut(logicData);
        return true;
    }

    // 号码空则还是当做Conf Hardkey处理
    if (!focus)
    {
        return false;
    }

    if (DSK_PHONE_TYPE_BLACK == commonUnits_GetDskPhoneType())
    {
        if (focus.IsAllowConfWithAnother())
        {
            AdapterBox_EnterConfWith(focus);
            return true;
        }
    }

    // 调用接口进入Conference界面
    return talklogic_UIEvent(focus, TALK_ACTION_CONF);
}

bool CDsskeyConfAction::ProcessNoBindDsskey(const CSessionStateProxy& proxy, const DssKeyLogicData& logicData)
{
    if (proxy.IsInherits(ROUTINE_PRETRAN))
    {
        return SendTalkEvent(proxy, logicData, DSSKEY_ACTION_TRAN_DIRECT);
    }
    else if (proxy.IsInherits(ROUTINE_DIALING)
             || proxy.IsInherits(ROUTINE_TALKING)
             || proxy.IsInherits(SESSION_LOCALCONF))
    {
        return SendTalkEvent(proxy, logicData, DSSKEY_ACTION_CONF_TO);
    }

    return false;
}

#endif  //IF_FEATURE_DSSKEY
