#ifdef IF_FEATURE_DSSKEY
#ifndef _TALK_DSSKEY_DSSKEYBLFLISTACTION_H__
#define _TALK_DSSKEY_DSSKEYBLFLISTACTION_H__

#include "dsskeyblfaction.h"

class CDsskeyBlfListAction
    : public CDsskeyBlfBaseAction
{
public:
    CDsskeyBlfListAction();

    virtual bool OnKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData, void* pExtraData);

    // 获取呼出号码
    virtual bool GetCallOutNumber(int dsskeyID, yl::string& strCallOutNumber,
                                  const DssKeyLogicData& logicData, void* pExtraData, int nExtType = 0);

    virtual bool OnDsskeyTypeModify(int dsskeyID, DssKey_Type oldType, DssKey_Type newType,
                                    void* pExtraData);

    virtual bool OnConfigChange(int dsskeyID,  void* pExtraData);

    /* 呼出参数处理 */
    virtual bool GetCallOutParam(int iActionType, const CSessionStateProxy& proxy, const DssKeyLogicData& logicData, CallOut_Param& param);

    virtual const yl::string& GetNumber(const DssKeyLogicData& logicData)
    {
        return logicData.strExtension.empty() ? logicData.strValue : logicData.strExtension;
    }
};


#endif // _TALK_DSSKEY_DSSKEYBLFLISTACTION_H__

#endif  //IF_FEATURE_DSSKEY
