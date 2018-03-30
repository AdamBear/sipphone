#ifdef IF_FEATURE_GETBALANCE
#include "talkdsskey_inc.h"

CDsskeyGetBalanceAction::CDsskeyGetBalanceAction()
    : CDsskeyBindAction(DT_GET_BALANCE)
{

}

bool CDsskeyGetBalanceAction::OnKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData,
        void* pExtraData)
{
    // 非Idle界面不允许获取账号余额信息
    if (talklogic_SessionExist()
            || acc_IsLoadingBalance())
    {
        DSSKEY_WARN("bIsLoadingBalance:%d", acc_IsLoadingBalance());
        return FALSE;
    }

    acc_SetNeedGetBalanceAccount(logicData.iLineIndex, true);

    return true;
}
#endif
