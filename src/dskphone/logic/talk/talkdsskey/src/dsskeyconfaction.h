#ifdef IF_FEATURE_DSSKEY
#ifndef __DSSKEY_DSSKEYCONFACTION_H__
#define __DSSKEY_DSSKEYCONFACTION_H__

#include "dsskeytalkbaseaction.h"

class CDsskeyConfAction
    : public CDssTalkBaseAciton
{
public:
    CDsskeyConfAction();
    virtual ~CDsskeyConfAction();

    //继承接口
    virtual bool OnKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData, void* pExtraData);

    /* 未绑定的呼出 */
    virtual bool ProcessNoBindDsskey(const CSessionStateProxy& proxy, const DssKeyLogicData& logicData);

};


#endif


#endif  //IF_FEATURE_DSSKEY
