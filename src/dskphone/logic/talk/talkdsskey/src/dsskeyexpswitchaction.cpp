#ifdef IF_FEATURE_DSSKEY

#include "talkdsskey_inc.h"

CDsskeyExpSwitchAction::CDsskeyExpSwitchAction()
    : IDsskeyImplAction(DT_EXP_SWITCH)
{

}

CDsskeyExpSwitchAction::~CDsskeyExpSwitchAction()
{

}

bool CDsskeyExpSwitchAction::OnKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData,
                                        void* pExtraData)
{
    exp_Exp39Switch(dsskey_GetModuleIndex(dsskeyDataID));
    return true;
}

#endif
