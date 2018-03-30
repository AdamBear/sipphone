#ifdef IF_FEATURE_DSSKEY
#if IF_FEATURE_PAGING
#ifndef __TALK_DSSKEY_DSSKEYPAGINGACTION_H__
#define __TALK_DSSKEY_DSSKEYPAGINGACTION_H__

#include "dsskeybindaction.h"

class CDsskeyPagingAction
    : public CDsskeyBindAction
{
public:
    CDsskeyPagingAction();

    //继承接口
    virtual bool OnKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData, void* pExtraData);
#if IF_BUG_27033
    // 获取呼出号码
    virtual bool GetCallOutNumber(int dsskeyID, yl::string& strCallOutNumber,
                                  const DssKeyLogicData& logicData, void* pExtraData, int nExtType = 0);
#endif

    /* 未绑定的呼出 */
    virtual bool ProcessNoBindDsskey(const CSessionStateProxy& proxy, const DssKeyLogicData& logicData);

};


#endif // __TALK_DSSKEY_DSSKEYPAGINGACTION_H__

#endif //IF_FEATURE_PAGING
#endif  //IF_FEATURE_DSSKEY
