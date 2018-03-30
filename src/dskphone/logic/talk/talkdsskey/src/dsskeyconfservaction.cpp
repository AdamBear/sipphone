#ifdef IF_FEATURE_DSSKEY

#include "talkdsskey_inc.h"

#if IF_BUG_27306
CDsskeyConfServAction::CDsskeyConfServAction()
    : CDsskeyBindAction(DT_CONF_SERV)
{

}

CDsskeyConfServAction::~CDsskeyConfServAction()
{

}

bool CDsskeyConfServAction::OnKeyPress(int dsskeyDataID, const DssKeyLogicData& logicData,
                                       void* pExtraData)
{
    return CallOutByDsskey(logicData);
}

bool CDsskeyConfServAction::GetCallOutNumber(int dsskeyID, yl::string& strCallOutNumber,
        const DssKeyLogicData& logicData, void* pExtraData, int nExtType)
{
    strCallOutNumber.clear();

    strCallOutNumber = configParser_GetConfigString(kszConfServCode);

    return !strCallOutNumber.empty();
}

#endif
#endif
