#ifdef IF_FEATURE_DSSKEY
#ifndef __TALK_DSSKEY_DSSKEYBLFACTION_H__
#define __TALK_DSSKEY_DSSKEYBLFACTION_H__

#include "dsskeyblfbaseaction.h"

class CDsskeyBlfAction
    : public CDsskeyBlfBaseAction
{
public:
    CDsskeyBlfAction(DssKey_Type dsskeyType);

    virtual bool OnKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData, void* pExtraData);

    // 获取呼出号码
    virtual bool GetCallOutNumber(int dsskeyID, yl::string& strCallOutNumber,
                                  const DssKeyLogicData& logicData, void* pExtraData, int nExtType = 0);

    virtual bool OnDsskeyTypeModify(int dsskeyID, DssKey_Type oldType, DssKey_Type newType,
                                    void* pExtraData);

    /* 呼出参数处理 */
    virtual bool GetCallOutParam(int iActionType, const CSessionStateProxy& proxy, const DssKeyLogicData& logicData, CallOut_Param& param);
};

#ifdef IF_FEATURE_GENBAND
class CDsskeyBlfGroupAction
    : public CDsskeyBlfAction
{
public:
    CDsskeyBlfGroupAction(DssKey_Type dsskeyType);

    virtual bool OnKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData, void* pExtraData);

    virtual bool OnDsskeyTypeModify(int dsskeyID, DssKey_Type oldType, DssKey_Type newType,
                                    void* pExtraData);

    virtual bool GetCallOutNumber(int dsskeyID, yl::string& strCallOutNumber,
                                  const DssKeyLogicData& logicData, void* pExtraData, int nExtType = 0);

    /* 未绑定的呼出 */
    virtual bool ProcessNoBindDsskey(const CSessionStateProxy& proxy, const DssKeyLogicData& logicData);

};
#endif

#endif  //IF_FEATURE_DSSKEY
#endif // __TALK_DSSKEY_DSSKEYBLFACTION_H__

