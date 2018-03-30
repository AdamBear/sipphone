#include "dsskey/include/idsskeyimplaction.h"

#ifdef IF_FEATURE_IDLESCREEN
class CDsskeySwitchAccountAction
    : public IDsskeyImplAction
{
public:
    CDsskeySwitchAccountAction(DssKey_Type eDsskeyType, bool bIncrement);

    //继承接口
    virtual bool OnKeyPress(int dsskeyDataID, const DssKeyLogicData & logicData, void * pExtraData);

private:
    bool m_bIncrement;
};

#endif // IF_FEATURE_IDLESCREEN
