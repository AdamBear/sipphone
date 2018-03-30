#ifdef IF_FEATURE_DSSKEY
#ifndef __TALK_DSSKEY_DSSKEYTRANSACTION_H__
#define __TALK_DSSKEY_DSSKEYTRANSACTION_H__

#include "dsskeytalkbaseaction.h"

class CDsskeyTransAction
    : public CDssTalkBaseAciton
{
public:
    CDsskeyTransAction(void);

    //继承接口
    virtual bool OnKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData, void* pExtraData);
};



#endif // __TALK_DSSKEY_DSSKEYTRANSACTION_H__


#endif  //_DECT
