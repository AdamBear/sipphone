#ifdef IF_FEATURE_DSSKEY

#ifndef __DSSKEY_DSSKEYCONFSERVACTION_H__
#define __DSSKEY_DSSKEYCONFSERVACTION_H__

#include "dsskeybindaction.h"

#if IF_BUG_27306
class CDsskeyConfServAction : public CDsskeyBindAction
{
public:
    CDsskeyConfServAction();
    virtual ~CDsskeyConfServAction();

    //继承接口
    virtual bool OnKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData, void* pExtraData);

    // 获取呼出号码
    virtual  bool GetCallOutNumber(int dsskeyID, yl::string& strCallOutNumber,
                                   const DssKeyLogicData& logicData, void* pExtraData, int nExtType);
};

#endif // IF_BUG_27306
#endif
#endif

