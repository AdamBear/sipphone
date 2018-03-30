#ifndef __TALK_DSSKEY_DSSKEYGETBALANCE_H__
#define __TALK_DSSKEY_DSSKEYGETBALANCE_H__

#ifdef IF_FEATURE_GETBALANCE
#include "dsskeybindaction.h"

class CDsskeyGetBalanceAction
    : public CDsskeyBindAction
{
public:
    CDsskeyGetBalanceAction();

    //继承接口
    virtual bool OnKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData, void* pExtraData);

};


#endif
#endif // __TALK_DSSKEY_DSSKEYGETBALANCE_H__
