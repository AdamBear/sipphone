#ifndef _PSTN_IDLE_HANDLE_H_
#define _PSTN_IDLE_HANDLE_H_

#include "baseidlehandle.h"
#include "ylvector.h"

#ifdef IF_FEATURE_PSTN
class CPstnHandle : public CBaseIdleHandle
{
public:
    CPstnHandle(int nType = PS_STATE_PSTN_ACCOUNT_NOTE);
    virtual ~CPstnHandle();
    static LRESULT ProcessPstnAccountMsg(msgObject & objMsg);

public:
    // 获取弹框信息
    virtual bool GetPopupBoxInfo(PopupBoxData & popData);
    // 按键处理
    virtual bool HandleKeyProcess(SoftKey_TYPE eSoftkey, PHONE_KEY_CODE eKeyCode);

    const VEC_ID & GetPstnIdList();
    void AddPstnIdToList(int nAccountId);
    void RemovePstnIdFromList(int nAccountId);
    void ClearPstnIdList();
private:
    VEC_ID m_vecAddPstnId;
};
#endif

#endif //end of _PSTN_IDLE_HANDLE_H_

