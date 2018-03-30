#ifdef IF_FEATURE_DSSKEY
#ifndef __TALK_DSSKEY_DSSKEYRETRIEVEPARKACTION_H__
#define __TALK_DSSKEY_DSSKEYRETRIEVEPARKACTION_H__

#include "dsskey/include/idsskeyimplaction.h"
#include "dsskey/include/moddsskey.h"
#include "dsskey/include/dsskeystructure.h"
#include "dsskey/include/dsskey_def.h"
#include "talk/talklogic/include/modtalklogic.h"

class CDsskeyRetrieveParkAction
    : IDsskeyImplAction
{
public:
    CDsskeyRetrieveParkAction()
        : IDsskeyImplAction(DT_BSFT_RETRIEVE_PARK)
    {
    };

    //继承接口
    virtual bool OnKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData, void* pExtraData);
};

#endif // __TALK_DSSKEY_DSSKEYRETRIEVEPARKACTION_H__
#endif  //IF_FEATURE_DSSKEY
