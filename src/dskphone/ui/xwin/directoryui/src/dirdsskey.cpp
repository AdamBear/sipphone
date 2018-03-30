#include "dirdsskey.h"
#include "directorycommon.h"
#include "bsftcontactcontroler.h"
#include "commondirctrl.h"
#include "moddirectoryui.h"
#include "modbtucui.h"
#include "presence/include/modpresence.h"
#include "setting_page_define.h"
#include "talkui/include/modtalkui.h"
#include <configiddefine.h>
#include <configparser/modconfigparser.h>
#include "talk/talklogic/include/modtalklogic.h"
#include "autoset/include/autoset_def.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "dsskey/include/dsskeymsgdefine.h"
#include "talk/talklogic/include/modtalklogic.h"


#if IF_FEATURE_METASWITCH
#include "contacts/directory/include/moddirinterface.h"
#endif


CDssKeyActionXMLGroup::CDssKeyActionXMLGroup()
    : IDsskeyImplAction(DT_XMLGROUP)
{

}

CDssKeyActionXMLGroup::~CDssKeyActionXMLGroup()
{

}

bool CDssKeyActionXMLGroup::OnKeyPress(int dsskeyDataID, const DssKeyLogicData & logicData,
                                       void * pExtraData)
{
    int nGroupId = CCommonDirCtrl::GetGroupIdByIndex(REMOTE_DIR, logicData.iLineID);

    if (knInvalidRId == nGroupId)
    {
        nGroupId = kszRootId;
    }

    return DM_EnterDirectoryUI(DIRECTORY_TYPE_REMOTE, nGroupId);
}

CDssKeyActionIdleCallList::CDssKeyActionIdleCallList()
    : IDsskeyImplAction(DT_IDLE_CALLLIST)
{

}

CDssKeyActionIdleCallList::~CDssKeyActionIdleCallList()
{

}

bool CDssKeyActionIdleCallList::OnKeyPress(int dsskeyDataID, const DssKeyLogicData & logicData,
        void * pExtraData)
{
#if IF_FEATURE_METASWITCH_CALLLOG
    if (MTSWLog_IsEnable())
    {
        //修改为可以通过Index控制，直接进入该历史记录
        if (!LocalLog_IsEnable() || 1 == logicData.iLineID)
        {
            DM_EnterDirectoryUI(CALL_LOG_TYPE_MTSW, knInvalidRId);
        }
        else
        {
            DM_EnterDirectoryUI(CALL_LOG_TYPE_LOCAL, knInvalidRId);
        }
    }
    else
#endif
    {
        if (BsftLog_IsEnable() && (1 == configParser_GetConfigInt(kszBWCallLogandDir)
                                   || 1 == logicData.iLineID))
        {
            DM_EnterDirectoryUI(CALL_LOG_TYPE_BROADSOFT, knInvalidRId);
        }
        else
        {
            DM_EnterDirectoryUI(CALL_LOG_TYPE_LOCAL, knInvalidRId);
        }

    }
    return true;
}

CDssKeyActionLDAP::CDssKeyActionLDAP()
    : IDsskeyImplAction(DT_LDAP)
{

}

CDssKeyActionLDAP::~CDssKeyActionLDAP()
{

}

bool CDssKeyActionLDAP::OnKeyPress(int dsskeyDataID, const DssKeyLogicData & logicData,
                                   void * pExtraData)
{
    return DM_EnterDirectoryUI(DIRECTORY_TYPE_LDAP, knInvalidRId);
}

CDssKeyActionLocalDir::CDssKeyActionLocalDir()
    : IDsskeyImplAction(DT_LOCALDIR)
{

}

CDssKeyActionLocalDir::~CDssKeyActionLocalDir()
{

}

bool CDssKeyActionLocalDir::OnKeyPress(int dsskeyDataID, const DssKeyLogicData & logicData,
                                       void * pExtraData)
{
    return DM_EnterDirectoryUI(DIRECTORY_TYPE_LOCAL, knInvalidRId);
}

CDssKeyActionBSDir::CDssKeyActionBSDir()
    : IDsskeyImplAction(DT_BSDIR)
{

}

CDssKeyActionBSDir::~CDssKeyActionBSDir()
{

}

bool CDssKeyActionBSDir::OnKeyPress(int dsskeyDataID, const DssKeyLogicData & logicData,
                                    void * pExtraData)
{
#ifdef IF_FEATURE_DIR_OPT
    return DM_EnterDirectoryUI(DIRECTORY_TYPE_BROADSOFT, kszAllContactId);
#else
    return DM_EnterDirectoryUI(DIRECTORY_TYPE_BROADSOFT, knInvalidRId);
#endif
}

CDssKeyActionLocalGroup::CDssKeyActionLocalGroup()
    : IDsskeyImplAction(DT_LOCALGROUP)
{

}

CDssKeyActionLocalGroup::~CDssKeyActionLocalGroup()
{

}

bool CDssKeyActionLocalGroup::OnKeyPress(int dsskeyDataID, const DssKeyLogicData & logicData,
        void * pExtraData)
{
    int nGroupId = CCommonDirCtrl::GetGroupIdByIndex(LOCAL_DIR, logicData.iLineID);

    return DM_EnterDirectoryUI(DIRECTORY_TYPE_LOCAL, nGroupId);
}

CDssKeyActionBSGroup::CDssKeyActionBSGroup()
    : IDsskeyImplAction(DT_BSGROUP)
{

}

CDssKeyActionBSGroup::~CDssKeyActionBSGroup()
{

}

bool CDssKeyActionBSGroup::OnKeyPress(int dsskeyDataID, const DssKeyLogicData & logicData,
                                      void * pExtraData)
{
    int nGroupId = CBSFTContactControler::GetGroupIdInAllGroup(logicData.iLineID);

    return DM_EnterDirectoryUI(DIRECTORY_TYPE_BROADSOFT, nGroupId);
}

CDssKeyActionIdleRemote::CDssKeyActionIdleRemote()
    : IDsskeyImplAction(DT_IDLE_REMOTE)
{

}

CDssKeyActionIdleRemote::~CDssKeyActionIdleRemote()
{

}

bool CDssKeyActionIdleRemote::OnKeyPress(int dsskeyDataID, const DssKeyLogicData & logicData,
        void * pExtraData)
{
    return DM_EnterDirectoryUI(DIRECTORY_TYPE_REMOTE, kszRootId);
}

CDssKeyActionFavrites::CDssKeyActionFavrites(DssKey_Type type)
    : IDsskeyImplAction(type)
{

}

CDssKeyActionFavrites::~CDssKeyActionFavrites()
{

}

bool CDssKeyActionFavrites::OnKeyPress(int dsskeyDataID, const DssKeyLogicData & logicData,
                                       void * pExtraData)
{
    return talkui_OpenPool(true);
}

#if FEATURE_BROADTOUCH_UC
CDssKeyActionNetworkFavorite::CDssKeyActionNetworkFavorite()
    : IDsskeyImplAction(DT_NETWORK_FAVORITE)
{

}

CDssKeyActionNetworkFavorite::~CDssKeyActionNetworkFavorite()
{

}

bool CDssKeyActionNetworkFavorite::OnKeyPress(int dsskeyDataID, const DssKeyLogicData & logicData,
        void * pExtraData)
{
    return DM_EnterDirectoryUI(DIRECTORY_TYPE_BTUC, FAVOURITE_ID);
}

CDssKeyUCFavorite::CDssKeyUCFavorite()
    : IDsskeyImplAction(DT_UC_FAVORITE)
{

}

CDssKeyUCFavorite::~CDssKeyUCFavorite()
{

}

bool CDssKeyUCFavorite::OnKeyPress(int dsskeyDataID, const DssKeyLogicData & logicData,
                                   void * pExtraData)
{
    int nContactId = atoi(logicData.strValue.c_str());

    return BTUC_DialFavorite(nContactId, logicData.iLineID);
}

bool CDssKeyUCFavorite::OnDsskeyTypeModify(int dsskeyID, DssKey_Type oldType, DssKey_Type newType,
        void * pExtraData)
{
    if (NULL == pExtraData)
    {
        dsskey_change_info_t tChangeDsskeyInfo;
        tChangeDsskeyInfo.eNewType = newType;
        tChangeDsskeyInfo.eOldType = oldType;
        tChangeDsskeyInfo.iDsskeyID = dsskeyID;

        etl_NotifyAppEx(FALSE, DSSKEY_RELOAD_FINISHED, newType, oldType, sizeof(dsskey_change_info_t),
                        &tChangeDsskeyInfo);
    }

    return true;
}

CDssKeyBuddies::CDssKeyBuddies()
    : IDsskeyImplAction(DT_BUDDIES)
{

}

CDssKeyBuddies::~CDssKeyBuddies()
{

}

bool CDssKeyBuddies::OnKeyPress(int dsskeyDataID, const DssKeyLogicData & logicData,
                                void * pExtraData)
{
#ifdef IF_FEATURE_DIR_OPT
    return DM_EnterDirectoryUI(DIRECTORY_TYPE_BTUC, FAVOURITE_ID);
#else
    return DM_EnterDirectoryUI(DIRECTORY_TYPE_BTUC, ALL_BUDDIES_ID);
#endif
}
#endif

// DT_REDIAL
CDsskeyRedialAciton::CDsskeyRedialAciton()
    : IDsskeyImplAction(DT_REDIAL)
{

}

CDsskeyRedialAciton::~CDsskeyRedialAciton()
{

}

bool CDsskeyRedialAciton::OnKeyPress(int dsskeyDataID, const DssKeyLogicData & logicData,
                                     void * pExtraData)
{
#if IF_BUG_27501
    if (1 == configParser_GetConfigInt(kszBroadsoftActive)
            && 1 == configParser_GetConfigInt(kszBsftEnableCallLog)
            && 1 == configParser_GetConfigInt(kszBWCallLogandDir))
    {
        DM_EnterDirectoryUI(CALL_LOG_TYPE_BROADSOFT, kszPlacedLogId);
    }
    else
    {
        DM_EnterDirectoryUI(CALL_LOG_TYPE_LOCAL, kszPlacedLogId);
    }
#else
    talklogic_Redial();
#endif
    return TRUE;
}

#if IF_BUG_14856
CDssKeyActionMissedCall::CDssKeyActionMissedCall()
    : IDsskeyImplAction(DT_MISSED_CALL)
{

}

CDssKeyActionMissedCall::~CDssKeyActionMissedCall()
{

}

bool CDssKeyActionMissedCall::OnKeyPress(int dsskeyDataID, const DssKeyLogicData & logicData,
        void * pExtraData)
{
    if (BsftLog_IsEnable() && 1 == configParser_GetConfigInt(kszBWCallLogandDir))
    {
        return DM_EnterDirectoryUI(CALL_LOG_TYPE_BROADSOFT, kszMissLogId);
    }
    else
    {
        return DM_EnterDirectoryUI(CALL_LOG_TYPE_LOCAL, kszMissLogId);
    }
}
#endif

#if IF_BUG_PLCMDIR
CDssKeyActionPlcmDir::CDssKeyActionPlcmDir()
    : IDsskeyImplAction(DT_PLCM_PHONEBOOK)
{

}

CDssKeyActionPlcmDir::~CDssKeyActionPlcmDir()
{

}

bool CDssKeyActionPlcmDir::OnKeyPress(int dsskeyDataID, const DssKeyLogicData & logicData,
                                      void * pExtraData)
{
    if (1 == configParser_GetConfigInt(kszPlcmPhoneEnable))
    {
        return DM_EnterDirectoryUI(DIRECTORY_TYPE_PLCM, kszPlcmDirGroupId);
    }

    return false;
}
#endif

#if IF_FEATURE_METASWITCH_DIRECTORY
CDssKeyActionMTSWDir::CDssKeyActionMTSWDir()
    : IDsskeyImplAction(DT_MTSWDIR)
{

}

CDssKeyActionMTSWDir::~CDssKeyActionMTSWDir()
{

}

bool CDssKeyActionMTSWDir::OnKeyPress(int dsskeyDataID, const DssKeyLogicData & logicData,
                                      void * pExtraData)
{
    return DM_EnterDirectoryUI(DIRECTORY_TYPE_METASWICTH, knInvalidRId);
}
#endif

#ifdef IF_FEATURE_PSTN
CDssKeyActionLocalFavorite::CDssKeyActionLocalFavorite()
    : IDsskeyImplAction(DT_LOCAL_FAVORITE)
{

}

CDssKeyActionLocalFavorite::~CDssKeyActionLocalFavorite()
{

}

bool CDssKeyActionLocalFavorite::OnKeyPress(int dsskeyDataID, const DssKeyLogicData & logicData,
        void * pExtraData)
{
    return DM_EnterDirectoryUI(DIRECTORY_TYPE_LOCAL, kszFavoriteGroupId);
}
#endif
