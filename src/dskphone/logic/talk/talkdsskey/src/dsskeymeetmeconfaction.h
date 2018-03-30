#ifdef IF_FEATURE_DSSKEY
#ifndef __TALK_DSSKEY_DSSKEYMEETMECONFACTION_H__
#define __TALK_DSSKEY_DSSKEYMEETMECONFACTION_H__

#include "dsskeybindaction.h"

class CDsskeyMeetMeConfAction
    : public CDsskeyBindAction
{
public:
    CDsskeyMeetMeConfAction();

    virtual bool OnKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData, void* pExtraData);

    virtual bool ProcessNoBindDsskey(const CSessionStateProxy& proxy, const DssKeyLogicData& logicData);

    virtual bool ProcessFocusDsskey(const CSessionStateProxy& proxy, const DssKeyLogicData& logicData);
};


#endif // __TALK_DSSKEY_DSSKEYMEETMECONFACTION_H__
#endif  //IF_FEATURE_DSSKEY
