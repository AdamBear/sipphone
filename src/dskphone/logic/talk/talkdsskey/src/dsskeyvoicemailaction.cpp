#ifdef IF_FEATURE_DSSKEY
#include "talkdsskey_inc.h"

CDsskeyVoiceMailAction::CDsskeyVoiceMailAction()
    : CDsskeyBindAction(DT_VOICEMAIL)
{

}

bool CDsskeyVoiceMailAction::OnKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData,
                                        void* pExtraData)
{
#if IF_BUG_27050
    if (logicData.strValue.empty())
    {
        return AdapterBox_EnterConnectVoiceMailPage(acc_GetLastVMailAccountId());
    }
#endif
    return CallOutByDsskey(logicData);
}

bool CDsskeyVoiceMailAction::GetCallOutNumber(int dsskeyID, yl::string& strCallOutNumber,
        const DssKeyLogicData& logicData, void* pExtraData, int nExtType)
{
    yl::string strDial = logicData.strValue;
    if (strDial.empty())
    {
        strDial = configParser_GetCfgItemStringValue(
                      kszAccountVoiceMailNumber,
                      logicData.iLineIndex
                  );
    }

    strCallOutNumber = strDial;
    return !strDial.empty();
}

#endif  //IF_FEATURE_DSSKEY
