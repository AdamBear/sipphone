#ifdef IF_FEATURE_DSSKEY
#ifndef __TALK_DSSKEY_DSSKEYPICKUPACTION_H__
#define __TALK_DSSKEY_DSSKEYPICKUPACTION_H__

#include "dsskeybindaction.h"

class CDsskeyPickupAction
    : public CDsskeyBindAction
{
public:
    CDsskeyPickupAction();

    //继承接口
    virtual bool OnKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData, void* pExtraData);

    // 获取呼出号码
    virtual bool GetCallOutNumber(int dsskeyDataID, yl::string& strCallOutNumber,
                                  const DssKeyLogicData& logicData, void* pExtraData, int nExtType = 0);
    /* 呼出参数处理 */
    virtual bool GetCallOutParam(int iActionType, const CSessionStateProxy& proxy, const DssKeyLogicData& logicData, CallOut_Param& param);

};



#endif // __TALK_DSSKEY_DSSKEYPICKUPACTION_H__
#endif  //IF_FEATURE_DSSKEY
