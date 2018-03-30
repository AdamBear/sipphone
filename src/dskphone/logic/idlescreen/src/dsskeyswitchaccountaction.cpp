#include "dsskeyswitchaccountaction.h"
#include "account/include/modaccount.h"

#ifdef IF_FEATURE_IDLESCREEN
CDsskeySwitchAccountAction::CDsskeySwitchAccountAction(DssKey_Type eDsskeyType, bool bIncrement)
    : IDsskeyImplAction(eDsskeyType), m_bIncrement(bIncrement)
{

}

bool CDsskeySwitchAccountAction::OnKeyPress(int dsskeyDataID, const DssKeyLogicData & logicData,
        void * pExtraData)
{
    acc_SwitchDefaultAccount(m_bIncrement);
    acc_SetDefaultAccount(acc_GetDefaultAccount(), true);

    return true;
}

#endif // IF_FEATURE_IDLESCREEN
