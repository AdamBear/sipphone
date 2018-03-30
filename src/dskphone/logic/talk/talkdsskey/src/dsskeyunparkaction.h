#ifdef IF_FEATURE_DSSKEY
#ifndef __TALK_DSSKEY_DSSKEYUNPARKACTION_H__
#define __TALK_DSSKEY_DSSKEYUNPARKACTION_H__

#include "dsskeybindaction.h"
#if IF_BUG_30965
class CDsskeyUnParkAction
    : public CDsskeyBindAction
{
public:
    CDsskeyUnParkAction();
    virtual ~CDsskeyUnParkAction();
    //继承接口
    virtual bool OnKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData, void* pExtraData);
    // 获取呼出号码
    virtual bool GetCallOutNumber(int dsskeyID, yl::string& strCallOutNumber,
                                  const DssKeyLogicData& logicData, void* pExtraData, int nExtType = 0);

    virtual bool ProcessUnFocusDsskey(const CSessionStateProxy& proxy, const DssKeyLogicData& logicData);
};
#endif //#if IF_BUG_30965

#endif // __TALK_DSSKEY_DSSKEYUNPARKACTION_H__

#endif
