#ifdef IF_FEATURE_DSSKEY

#ifndef _TALK_DSSKEY_DSSKEYEXPSWITCHACTION_H__
#define _TALK_DSSKEY_DSSKEYEXPSWITCHACTION_H__

#include "dsskeytalkbaseaction.h"

class CDsskeyExpSwitchAction
    : public IDsskeyImplAction
{
public:
    CDsskeyExpSwitchAction();
    virtual ~CDsskeyExpSwitchAction();


    //继承接口
    virtual bool OnKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData, void* pExtraData);

};


#endif
#endif // _TALK_DSSKEY_DSSKEYFWDACTION_H__


