#ifdef IF_FEATURE_DSSKEY
#ifndef __TALKLOGIC_DSSKEY_DSSKEYBINDACTION_H__
#define __TALKLOGIC_DSSKEY_DSSKEYBINDACTION_H__

#include "dsskeytalkbaseaction.h"

class CDsskeyBindAction
    : public CDssTalkBaseAciton
{
public:
    CDsskeyBindAction(DssKey_Type eDsskeyType);
    virtual ~CDsskeyBindAction();

    //继承接口
    virtual bool OnKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData, void* pExtraData) = 0;

    virtual bool OnKeyLongPress(int dsskeyDataID, const DssKeyLogicData& logicData, void* pExtraData);

    virtual bool OnBindedAction(int dsskeyDataID, const DssKeyLogicData& logicData, void* pExtraData);

    virtual bool OnDebinded(int dsskeyDataID, const DssKeyLogicData& logicData, void* pExtraData);

    virtual bool OnTimer(int dsskeyDataID, UINT timerID, const DssKeyLogicData& logicData,
                         void* pExtraData);

    virtual bool IsBindType();

    //virtual void SetTimer(int dsskeyDataID,UINT uTimeInterval, const DssKeyLogicData &logicData, void *pExtraData);

    // 获取呼出号码
    virtual bool GetCallOutNumber(int dsskeyID, yl::string& strCallOutNumber,
                                  const DssKeyLogicData& logicData, void* pExtraData, int nExtType = 0);

    virtual bool AfterDebind(int dsskeyDataID, const DssKeyLogicData& logicData, void* pExtraData);

    virtual bool OnDsskeyTypeModify(int dsskeyID, DssKey_Type oldType, DssKey_Type newType,
                                    void* pExtraData);

    virtual bool OnConfigChange(int dsskeyID,  void* pExtraData);

    /* 需要绑定的dssskey在未绑定的时候处理 */
    virtual bool ProcessNoBindDsskey(const CSessionStateProxy& proxy, const DssKeyLogicData& logicData);

    /* 已经绑定的焦点通话处理 */
    virtual bool ProcessFocusDsskey(const CSessionStateProxy& proxy, const DssKeyLogicData& logicData);

    /* 已经绑定的非焦点通话处理 */
    virtual bool ProcessUnFocusDsskey(const CSessionStateProxy& proxy, const DssKeyLogicData& logicData);

    /* 默认的呼出操作，依据当前通话状态，和绑定关系处理 */
    virtual bool CallOutByDsskey(const DssKeyLogicData& logicData);

    /* 呼出Line（BTLine呼出） */
    bool CallOutByLine(const DssKeyLogicData& logicData);

protected:

#if IF_FEATRUE_PTT
    bool ExitSessionByDsskey(int dsskeyDataID);
#endif
};



#endif //__TALKLOGIC_DSSKEY_DSSKEYBINDACTION_H__
#endif  //IF_FEATURE_DSSKEY
