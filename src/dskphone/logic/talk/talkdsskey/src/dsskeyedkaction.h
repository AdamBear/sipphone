#ifdef IF_FEATURE_DSSKEY

#ifndef __TALKLOGIC_DSSKEY_DSSKEYMACROACTION_H__
#define __TALKLOGIC_DSSKEY_DSSKEYMACROACTION_H__

#include "dsskeytalkbaseaction.h"

class CDsskeyEdkAction
    : public CDssTalkBaseAciton
{
public:
    CDsskeyEdkAction();
    virtual ~CDsskeyEdkAction();

    //继承接口
    virtual bool OnKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData, void* pExtraData);


};

#endif

#endif //__TALKLOGIC_DSSKEY_DSSKEYMACROACTION_H__
