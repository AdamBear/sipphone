#ifdef IF_FEATURE_DSSKEY
#ifndef __TALK_DSSKEY_DSSKEYINTERCOMACTION_H__
#define __TALK_DSSKEY_DSSKEYINTERCOMACTION_H__

#include "dsskeyblfbaseaction.h"

class CDsskeyIntercomAction
    : public CDsskeyBlfBaseAction
{
public:
    CDsskeyIntercomAction();
    virtual ~CDsskeyIntercomAction();

    //继承接口
    virtual bool OnKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData, void* pExtraData);

    // 获取呼出号码
    virtual bool GetCallOutNumber(int dsskeyID, yl::string& strCallOutNumber,
                                  const DssKeyLogicData& logicData, void* pExtraData, int nExtType = 0);

    virtual bool OnDsskeyTypeModify(int dsskeyID, DssKey_Type oldType, DssKey_Type newType,
                                    void* pExtraData);

    virtual bool OnConfigChange(int dsskeyID,  void* pExtraData);

#if IF_FEATRUE_PTT
    virtual bool OnKeyRelease(int dsskeyDataID, const DssKeyLogicData& logicData, void* pExtraData);
#endif

    virtual bool IsIntercomMode()
    {
        return true;
    }
};


#endif //__TALK_DSSKEY_DSSKEYINTERCOMACTION_H__
#endif  //IF_FEATURE_DSSKEY
