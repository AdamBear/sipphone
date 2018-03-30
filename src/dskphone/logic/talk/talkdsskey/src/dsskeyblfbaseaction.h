#ifdef IF_FEATURE_DSSKEY
#ifndef __TALK_DSSKEY_DSSKEYBLFBASEACTION_H__
#define __TALK_DSSKEY_DSSKEYBLFBASEACTION_H__

#include "dsskeybindaction.h"

class CDsskeyBlfBaseAction
    : public CDsskeyBindAction
{
public:
    CDsskeyBlfBaseAction(DssKey_Type dsskeyType);

    virtual ~CDsskeyBlfBaseAction() {};

    virtual bool OnKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData, void* pExtraData);

    virtual bool OnConfigChange(int dsskeyID,  void* pExtraData);

    virtual bool BeforeUpdateUI(int dsskeyDataID, DsskeyExpRefresh dsskeyRefresh = DSS_KEY_REFRESH_ALL);

    virtual bool IsIntercomMode();

    /* 呼出参数处理 */
    virtual bool GetCallOutParam(int iActionType, const CSessionStateProxy& proxy, const DssKeyLogicData& logicData, CallOut_Param& param);

    bool AddIntercomCode(yl::string& strNumber);

    virtual bool IsDTMFRelpalceTransfer(int iAccountID);

    virtual bool IsBLF()
    {
        return true;
    }

protected:

    int GetRemoteStatus(int iDsskey);

    bool ProcessBLFCustomAction(int iDsskey, const DssKeyLogicData& logicData);

    /* 未绑定的呼出 */
    virtual bool ProcessNoBindDsskey(const CSessionStateProxy& proxy, const DssKeyLogicData& logicData);

    bool ProcessBLF(const CSessionStateProxy& proxy, const DssKeyLogicData& logicData);

};


#endif // __TALK_DSSKEY_DSSKEYBLFBASEACTION_H__

#endif  //IF_FEATURE_DSSKEY
