#ifdef IF_FEATURE_DSSKEY
#include "talkdsskey_inc.h"

void talk_dsskey_init()
{
    new CDsskeyLineAction();                    //DT_LINE
    new CDsskeyConfAction();                    //DT_CONF
    new CDsskeyFwdAction();                     //DT_FWD
    new CDsskeyDNDAciton();                     //DT_DND
    new CDsskeyTransAction();                   //DT_TRAN
    new CDsskeyPickupAction();                  //DT_PICKUP
    new CDsskeyCallReturnAciton();              //DT_CALLRETURN
    new CDsskeySpeeddialAciton();               //DT_SPEEDDIAL
    new CDsskeyGroupPickupAction();             //DT_GROUPPICKUP
#if IF_FEATURE_PAGING
    new CDsskeyPagingAction();                  //DT_PAGING
#endif
    new CDsskeyRecordAction(DT_RECORD);         //DT_RECORD
    new CDsskeyRecordAction(DT_URLRECORD);      //DT_URLRECORD
#ifdef IF_FEATURE_C5_SYS_VOICE_RECORD
    new CDsskeyRecordAction(DT_VOICERECORD);
#endif
    new CDsskeyPrefixAction();                  //DT_PREFIX
    new CDsskeyRetrieveParkAction();            //DT_BSFT_RETRIEVE_PARK
    new CDsskeyCustomAction();                  //DT_CUSTOM
    new CDsskeyVoiceMailAction();               //DT_VOICEMAIL
    new CDsskeyCallparkAction();                //DT_CALLPARK

    //Dsskey Send Talk Action
    new CDsskeyTalkAction(DT_GROUPLISTEN, TALK_ACTION_GROUP_LISTEN); //DT_GROUPLISTEN
    new CDsskeyTalkAction(DT_PUBLICHOLD, TALK_ACTION_HOLD_PUBLIC); //DT_PUBLICHOLD
    new CDsskeyTalkAction(DT_PRIVATEHOLD, TALK_ACTION_HOLD_PRIVATE); //DT_PRIVATEHOLD
    new CDsskeyTalkAction(DT_HOLD, TALK_ACTION_HOLD_SWITCH);    //DT_HOLD
    new CDsskeyExpSwitchAction();

    new CDsskeyBlfAction(DT_BLF);                     //DT_BLF
#ifdef IF_FEATURE_GENBAND
    new CDsskeyBlfGroupAction(DT_BLFGROUP);
#endif
    new CDsskeyBlfListAction();                 //DT_BLFLIST
    new CDsskeyIntercomAction();                //DT_INTERCOM
    new CDsskeyMeetMeConfAction();              //DT_MEET_ME_CONFERENCE
    new CDsskeyNAAction();                      //DT_NA

#ifdef IF_BT_SUPPORT_PHONE
    new CDsskeyBTBindAction();                  //DT_BTLINE
    //通知中心Dsskey
    new CDsskeyCommonAction(DT_BLUETOOTH);
#endif

#ifdef _T49
#ifdef IF_SUPPORT_WIFI
    new CDsskeyCommonAction(DT_WIFI);
#endif // IF_SUPPORT_WIFI

    new CDsskeyCommonAction(DT_CAMERA);
    new CDsskeyCommonAction(DT_EXT_DISPLAY);
#endif // _T49

#ifdef IF_FEATURE_GETBALANCE
    new CDsskeyGetBalanceAction();              //DT_GET_BALANCE
#endif

#if IF_FEATURE_EDK
    new CDsskeyEdkAction();                 //DT_CUSTOM_KEY
#endif

#if IF_BUG_27029
    new CDsskeyRedialAciton();                     //DT_REDIAL
#endif

#if IF_BUG_30958
    new CDsskeyTipTelBtnAciton();
#endif

#if IF_BUG_30965
    new CDsskeyUnParkAction();              //DT_UNPARK
#endif

#if IF_FEATURE_SRTP
    new CDsskeySRTPAction();
#endif

#ifdef IF_SUPPORT_VIDEO
    new CDsskey3rdVMRAciton();
    new CDsskeyVideoConfAction();
#endif

#if IF_FEATURE_METASWITCH_CALLPARK
    new CDsskeyMTSWCallparkAction();            //DT_MTSW_CALLPARK
#endif
}

CSessionProxy talk_GetSessionByDsskey(int iDsskeyID)
{
    return _DsskeyBindManager.GetSessionByDsskey(iDsskeyID, dsskey_GetDsskeyType(iDsskeyID) == DT_LINE);
}

#endif  //IF_FEATURE_DSSKEY
