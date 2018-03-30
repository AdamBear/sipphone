#ifdef IF_FEATURE_DSSKEY
#ifndef _TALK_DSSKEY_DSSKEYFWDACTION_H__
#define _TALK_DSSKEY_DSSKEYFWDACTION_H__

#include "dsskeytalkbaseaction.h"

class CDsskeyFwdAction
    : public CDssTalkBaseAciton
{
public:
    CDsskeyFwdAction();
    virtual ~CDsskeyFwdAction();

    //继承接口
    virtual bool OnKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData, void* pExtraData);

    virtual bool OnConfigChange(int dsskeyID, void* pExtraData);

    /* 未绑定的呼出 */
    virtual bool ProcessNoBindDsskey(const CSessionStateProxy& proxy, const DssKeyLogicData& logicData);

protected:
    static LRESULT OnFwdStatusChangeMsg(msgObject& msg);

    static LRESULT OnDefaultAccountChangedMsg(msgObject& msg);

    static void UpdateAllDsskeys();

    static void UpdateDsskey(int dsskeyID);
};


#endif // _TALK_DSSKEY_DSSKEYFWDACTION_H__


#endif  //IF_FEATURE_DSSKEY
