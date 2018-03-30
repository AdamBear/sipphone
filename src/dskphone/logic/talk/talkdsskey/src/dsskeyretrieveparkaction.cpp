#ifdef IF_FEATURE_DSSKEY
#include "talkdsskey_inc.h"

bool CDsskeyRetrieveParkAction::OnKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData, void* pExtraData)
{
    const yl::string& strNumber = logicData.strExtension.empty() ? logicData.strValue : logicData.strExtension;
    SignalParams sigp(INVALID_SESSION_ID, strNumber);
    sigp.wParam.iValue = logicData.iLineIndex;
    modtklsn_emit_sync(TALK_SIG_SESSION_RETRIEVEPARK, sigp);
    return true;
}

#endif  //IF_FEATURE_DSSKEY
