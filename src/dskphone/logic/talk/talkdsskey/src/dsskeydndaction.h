#ifdef IF_FEATURE_DSSKEY
#ifndef __TALK_DSSKEY_DSSKEYDNDACTION_H__
#define __TALK_DSSKEY_DSSKEYDNDACTION_H__

#include "dsskeytalkbaseaction.h"

class CDsskeyDNDAciton
    : public CDssTalkBaseAciton
{
public:
    CDsskeyDNDAciton();

    virtual ~CDsskeyDNDAciton();

    virtual bool OnKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData, void* pExtraData);

    virtual bool OnConfigChange(int dsskeyID,  void* pExtraData);

protected:
    static LRESULT OnDndStatusChangeMsg(msgObject& msg);

    static LRESULT OnDefaultAccountChangedMsg(msgObject& msg);

    static void UpdateDsskey();
};

#endif // __TALK_DSSKEY_DSSKEYDNDACTION_H__


#endif  //IF_FEATURE_DSSKEY
