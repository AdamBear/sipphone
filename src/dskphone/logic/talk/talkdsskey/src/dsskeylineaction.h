#ifdef IF_FEATURE_DSSKEY
#ifndef __TALKLOGIC_DSSKEY_DSSKEYLINEACTION_H__
#define __TALKLOGIC_DSSKEY_DSSKEYLINEACTION_H__

#include "dsskeybindaction.h"

class CDsskeyLineAction
    : public CDsskeyBindAction
{
public:
    CDsskeyLineAction();
    virtual ~CDsskeyLineAction();

    //继承接口
    virtual bool OnKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData, void* pExtraData);

    virtual bool AfterDebind(int dsskeyDataID, const DssKeyLogicData& logicData, void* pExtraData);

    virtual bool OnDsskeyTypeModify(int dsskeyID, DssKey_Type oldType, DssKey_Type newType,
                                    void* pExtraData);

    virtual bool OnConfigChange(int dsskeyID,  void* pExtraData);

    static LRESULT OnFwdDndStatusChangeMsg(msgObject& msg);

    virtual bool ProcessNoBindDsskey(const CSessionStateProxy& proxy, const DssKeyLogicData& logicData);

    virtual bool ProcessFocusDsskey(const CSessionStateProxy& proxy, const DssKeyLogicData& logicData);

    virtual bool ProcessUnFocusDsskey(const CSessionStateProxy& proxy, const DssKeyLogicData& logicData);

private:
    // 账号类型是BroadSoft SCA的按键处理
    bool ProcessBLAKeyPress(int iAccountID, int iDsskeyID, const DssKeyLogicData& logicData);

    void UpdateLineInfo(const DssKeyLogicData& logicData);
    void CorrectRealBindAccount(const DssKeyLogicData& logicData);
    void UpdateLineDsskeyStatus(const DssKeyLogicData& logicData);
    void UpdateLineStatus(const DssKeyLogicData& logicData, LINE_STATE eLineState);

    int GetBlaStatus(int iDsskeyID);

    bool ProcessBeforeScaCallout(const DssKeyLogicData& logicData);

    /* 呼出参数处理 */
    virtual bool GetCallOutParam(int iActionType, const CSessionStateProxy& proxy, const DssKeyLogicData& logicData, CallOut_Param& param);

};

#endif //__TALKLOGIC_DSSKEY_DSSKEYLINEACTION_H__



#endif  //IF_FEATURE_DSSKEY
