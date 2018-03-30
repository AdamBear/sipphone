#ifdef IF_FEATURE_DSSKEY
#ifndef _TALK_DSSKEY_DSSKEYRECORDACTION_H__
#define _TALK_DSSKEY_DSSKEYRECORDACTION_H__

#include "dsskeybindaction.h"

class CDsskeyRecordAction
#ifdef IF_FEATURE_C5_SYS_VOICE_RECORD
    : public CDsskeyBindAction
#else
    : public CDssTalkBaseAciton
#endif
{
public:
    CDsskeyRecordAction(DssKey_Type eDsskeyType);

    //继承接口
    virtual bool OnKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData, void* pExtraData);
};


#endif // _TALK_DSSKEY_DSSKEYRECORDACTION_H__
#endif  //IF_FEATURE_DSSKEY
