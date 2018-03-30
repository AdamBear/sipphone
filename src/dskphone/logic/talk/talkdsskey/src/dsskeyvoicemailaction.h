#ifdef IF_FEATURE_DSSKEY
#ifndef __TALK_DSSKEY_DSSKEYVOICEMAILACTION_H__
#define __TALK_DSSKEY_DSSKEYVOICEMAILACTION_H__

#include "dsskeybindaction.h"


class CDsskeyVoiceMailAction
    : public CDsskeyBindAction
{
public:
    CDsskeyVoiceMailAction();

    //继承接口
    virtual bool OnKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData, void* pExtraData);

    // 获取呼出号码
    virtual bool GetCallOutNumber(int dsskeyID, yl::string& strCallOutNumber,
                                  const DssKeyLogicData& logicData, void* pExtraData, int nExtType = 0);
};



#endif // __TALK_DSSKEY_DSSKEYVOICEMAILACTION_H__
#endif  //IF_FEATURE_DSSKEY
