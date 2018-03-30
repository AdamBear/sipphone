#ifdef IF_FEATURE_DSSKEY
#include "talkdsskey_inc.h"


CDsskeyRecordAction::CDsskeyRecordAction(DssKey_Type eDsskeyType)
#ifdef IF_FEATURE_C5_SYS_VOICE_RECORD
    : CDsskeyBindAction(eDsskeyType)
#else
    : CDssTalkBaseAciton(eDsskeyType)
#endif
{

}

bool CDsskeyRecordAction::OnKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData,
                                     void* pExtraData)
{
    //URLRECORD复用
    if ((logicData.eDKType == DT_URLRECORD || logicData.eDKType == DT_VOICERECORD)
            && logicData.strValue.empty())
    {
        return false;
    }

#ifdef IF_FEATURE_C5_SYS_VOICE_RECORD
    if (logicData.eDKType == DT_VOICERECORD)
    {
        return CallOutByDsskey(logicData);
    }
#endif

    CSessionStateProxy focus;
    return talklogic_UIEvent(focus, TALK_ACTION_RECORD_RESULT, dsskeyDataID, logicData.eDKType);
}

#endif  //IF_FEATURE_DSSKEY
