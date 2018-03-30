#ifdef IF_FEATURE_DSSKEY
#ifndef __TALK_DSSKEY_DSSKEYPREFIXACTION_H__
#define __TALK_DSSKEY_DSSKEYPREFIXACTION_H__

#include "dsskeytalkbaseaction.h"

class CDsskeyPrefixAction
    : public CDssTalkBaseAciton
{
public:
    CDsskeyPrefixAction();

    //继承接口
    virtual bool OnKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData, void* pExtraData);

    virtual bool ProcessNoBindDsskey(const CSessionStateProxy& proxy, const DssKeyLogicData& logicData);

    /* 呼出参数处理 */
    virtual bool GetCallOutParam(int iActionType, const CSessionStateProxy& proxy, const DssKeyLogicData& logicData, CallOut_Param& param);

};

#endif // __TALK_DSSKEY_DSSKEYPREFIXACTION_H__

#endif  //IF_FEATURE_DSSKEY
