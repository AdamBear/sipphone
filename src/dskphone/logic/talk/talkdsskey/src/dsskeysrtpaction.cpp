#ifdef IF_FEATURE_DSSKEY
#ifdef IF_FEATURE_SRTP
#include "talkdsskey_inc.h"

CDsskeySRTPAction::CDsskeySRTPAction()
    : CDsskeyBindAction(DT_SRTP)
{

}

bool CDsskeySRTPAction::OnKeyPress(int dsskeyDataID, const DssKeyLogicData & logicData,
                                   void * pExtraData)
{
    if (talklogic_SessionExist())
    {
        return false;
    }

    YLList<int> listAccounts;
    listAccounts = acc_GetAvailableAccountList();
    int iSRTPStatus = SRTP_COMPLUSORY;
    if (logicData.iStatus == DS_SRTP_COMPLUSORY)
    {
        iSRTPStatus = SRTP_OPTIONAL;
    }

    for (YLList<int>::iterator itBeg = listAccounts.begin(); itBeg != listAccounts.end(); ++itBeg)
    {
        int iAccount = *itBeg;

        if (configParser_GetCfgItemIntValue(kszAccountSrtpEncryption, iAccount) != iSRTPStatus)
        {
            configParser_SetCfgItemIntValue(kszAccountSrtpEncryption, iAccount, iSRTPStatus);
            msgBroadpostThreadMsg(CONFIG_MSG_BCAST_CHANGED, ST_SIPLINE, iAccount);
        }
    }

    return true;
}

bool CDsskeySRTPAction::OnConfigChange(int dsskeyID,  void * pExtraData)
{
    dsskey_UpdateSRTPStatus();
    return true;
}


#endif
#endif
