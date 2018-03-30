#ifdef IF_FEATURE_DSSKEY
#ifndef dsskeygrouppickupaction_h__
#define dsskeygrouppickupaction_h__

#include "dsskeybindaction.h"

class CDsskeyGroupPickupAction
    : public CDsskeyBindAction
{
public:
    CDsskeyGroupPickupAction();

    //继承接口
    virtual bool OnKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData, void* pExtraData);

    // 获取呼出号码
    virtual bool GetCallOutNumber(int dsskeyDataID, yl::string& strCallOutNumber,
                                  const DssKeyLogicData& logicData, void* pExtraData, int nExtType = 0);

    bool GetCallOutParam(int iActionType, const CSessionStateProxy& proxy, const DssKeyLogicData& logicData, CallOut_Param& param);

};

#endif // dsskeygrouppickupaction_h__
#endif  //IF_FEATURE_DSSKEY
