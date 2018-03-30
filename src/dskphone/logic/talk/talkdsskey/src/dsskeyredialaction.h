#ifdef IF_FEATURE_DSSKEY
#ifndef __TALK_DSSKEY_DSSKEYREDIALACTION_H__
#define __TALK_DSSKEY_DSSKEYREDIALACTION_H__

#include "dsskey/include/idsskeyimplaction.h"
#include "dsskey/include/moddsskey.h"
#include "dsskey/include/dsskeystructure.h"
#include "dsskey/include/dsskey_def.h"
#include "talk/talklogic/include/modtalklogic.h"

class CDsskeyRedialAciton: public IDsskeyImplAction
{
public:
    CDsskeyRedialAciton()
        : IDsskeyImplAction(DT_REDIAL)
    {
    }

    virtual ~CDsskeyRedialAciton()
    {
    }

    virtual bool OnKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData, void* pExtraData)
    {
        talklogic_Redial();
        return true;
    }
};



#endif // __TALK_DSSKEY_DSSKEYREDIALACTION_H__

#endif

