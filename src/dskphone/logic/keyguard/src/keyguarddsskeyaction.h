#ifdef IF_FEATURE_DSSKEY

#ifndef _KEYGUARD_DSSKEY_ACTION_
#define _KEYGUARD_DSSKEY_ACTION_
#include "dsskey/include/idsskeyimplaction.h"
#include "dsskey/include/dsskey_enumtypes.h"
#include "../include/modkeyguard.h"
#include "dsskey/include/dsskeystructure.h"
#include "adapterbox/include/modadapterbox.h"

class CDssKeyKeyGuard : public IDsskeyImplAction
{
public:
    CDssKeyKeyGuard(): IDsskeyImplAction(DT_KEYPAD_LOCK)
    {

    }
    virtual ~CDssKeyKeyGuard()
    {

    }

    virtual bool OnKeyPress(int dsskeyDataID, const DssKeyLogicData & logicData, void * pExtraData)
    {
        if (keyGuard_IsEnable()
                && AdapterBox_IsInIdle())
        {
            keyGuard_Lock(true);
        }
        return true;
    }
};

#endif
#endif


