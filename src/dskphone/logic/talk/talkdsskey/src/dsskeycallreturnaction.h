#ifdef IF_FEATURE_DSSKEY
#ifndef __TALK_DSSKEY_DSSKEYCALLRETURNACTION_H__
#define __TALK_DSSKEY_DSSKEYCALLRETURNACTION_H__

#include "dsskeytalkbaseaction.h"
#include "talk/talklogic/include/modtalklogic.h"

class CDsskeyCallReturnAciton
    : public CDssTalkBaseAciton
{
public:
    CDsskeyCallReturnAciton()
        : CDssTalkBaseAciton(DT_CALLRETURN)
    {
    }

    virtual bool OnKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData, void* pExtraData)
    {
        // 按键处理
        talklogic_CallReturn();
        return true;
    }
};



#endif // __TALK_DSSKEY_DSSKEYCALLRETURNACTION_H__


#endif  //IF_FEATURE_DSSKEY
