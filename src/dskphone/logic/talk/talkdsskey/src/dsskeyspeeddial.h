#ifdef IF_FEATURE_DSSKEY
#ifndef __TALK_DSSKEY_DSSKEYSPEEDDIALACTION_H__
#define __TALK_DSSKEY_DSSKEYSPEEDDIALACTION_H__

#include "dsskeybindaction.h"

class CDsskeySpeeddialAciton
    : public CDsskeyBindAction
{
public:
    CDsskeySpeeddialAciton()
        : CDsskeyBindAction(DT_SPEEDDIAL)
    {
    }


    virtual bool OnKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData, void* pExtraData);

    /* 呼出参数处理 */
    virtual bool GetCallOutParam(int iActionType, const CSessionStateProxy& proxy, const DssKeyLogicData& logicData, CallOut_Param& param);

    virtual bool ProcessNoBindDsskey(const CSessionStateProxy& proxy, const DssKeyLogicData& logicData);
};

#endif // __TALK_DSSKEY_DSSKEYDNDACTION_H__


#endif  //IF_FEATURE_DSSKEY
