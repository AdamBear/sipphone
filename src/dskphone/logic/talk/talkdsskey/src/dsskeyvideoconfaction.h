#ifdef IF_FEATURE_DSSKEY

#ifndef __DSSKEY_DSSKEYVIDEOCONFACTION_H__
#define __DSSKEY_DSSKEYVIDEOCONFACTION_H__

#ifdef IF_SUPPORT_VIDEO

#include "dsskeytalkbaseaction.h"

class CDsskeyVideoConfAction
    : public CDssTalkBaseAciton
{
public:
    CDsskeyVideoConfAction();
    virtual ~CDsskeyVideoConfAction();

    //继承接口
    virtual bool OnKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData, void* pExtraData);

    // 获取呼出号码
    virtual bool GetCallOutNumber(int dsskeyID, yl::string& strCallOutNumber,
                                  const DssKeyLogicData& logicData, void* pExtraData, int nExtType = 0);

    virtual bool ProcessNoBindDsskey(const CSessionStateProxy& proxy, const DssKeyLogicData& logicData);

    bool GetCallOutParam(int iActionType, const CSessionStateProxy& proxy, const DssKeyLogicData& logicData, CallOut_Param& param);

};

#endif

#endif //__DSSKEY_DSSKEYVIDEOCONFACTION_H__

#endif
