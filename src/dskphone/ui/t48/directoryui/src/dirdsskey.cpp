#include "dirdsskey.h"
#include "directorycommon.h"
#include "bsftcontactcontroler.h"
#include "commondirctrl.h"
#include "modbtucui.h"
#include "moddirectoryui.h"
#if FEATURE_BROADTOUCH_UC
#include "presence/include/modpresence.h"
#endif
#include "setting/include/modmenu.h"
#include "setting_page_define.h"
#include "talkui/include/modtalkui.h"
#include <configiddefine.h>
#include <configparser/modconfigparser.h>
#include "talk/talklogic/include/modtalklogic.h"
#include "autoset/include/autoset_def.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "dsskey/include/dsskeymsgdefine.h"

#if IF_FEATURE_METASWITCH
#include "settingui/include/modsettingui.h"
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
        if (!LocalLog_IsEnable()
                || 1 == logicData.iLineID)
        {
            DM_EnterDirectoryUI(CALL_LOG_TYPE_MTSW, kszAllContactId);
        }
        else
        {
            DM_EnterDirectoryUI(CALL_LOG_TYPE_LOCAL, kszAllContactId);
        }
    }
    else
#endif
    {
        if (BsftLog_IsEnable() && (1 == configParser_GetConfigInt(kszBWCallLogandDir)
                                   || 1 == logicData.iLineID))
        {
            DM_EnterDirectoryUI(CALL_LOG_TYPE_BROADSOFT, kszAllContactId);
        }
        else
        {
            DM_EnterDirectoryUI(CALL_LOG_TYPE_LOCAL, kszAllContactId);
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
    return DM_EnterDirectoryUI(DIRECTORY_TYPE_LOCAL, kszAllContactId);
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
    return DM_EnterDirectoryUI(DIRECTORY_TYPE_BROADSOFT, kszAllContactId);
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
#if FEATURE_BROADTOUCH_UC
    return DM_EnterDirectoryUI(DIRECTORY_TYPE_BTUC, FAVOURITE_ID);
#else
    return false;
#endif
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
    if (pExtraData == NULL)
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
#if FEATURE_BROADTOUCH_UC
#ifdef IF_FEATURE_DIR_OPT
    return DM_EnterDirectoryUI(DIRECTORY_TYPE_BTUC, FAVOURITE_ID);
#else
    return DM_EnterDirectoryUI(DIRECTORY_TYPE_BTUC, ALL_BUDDIES_ID);
#endif
#else
    return false;
#endif
}

CDssKeyMyStatus::CDssKeyMyStatus()
    : IDsskeyImplAction(DT_MY_STATUS)
{

}

CDssKeyMyStatus::~CDssKeyMyStatus()
{

}

bool CDssKeyMyStatus::OnKeyPress(int dsskeyDataID, const DssKeyLogicData & logicData,
                                 void * pExtraData)
{
    if (1 == configParser_GetConfigInt(kszEnableChangePresenceStatus))
    {
        return Menu_EnterPageByType(MENU_SETTING_MY_STATUS);
    }

    return true;
}


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
    if (configParser_GetConfigInt(kszBroadsoftActive) == 1
            && configParser_GetConfigInt(kszBsftEnableCallLog) == 1
            && configParser_GetConfigInt(kszBWCallLogandDir) == 1)
    {
        Menu_EnterPageByType(MENU_PAGE_BROADSOFT_CALLLOG, kszPlacedLogId);
    }
    else
    {
        Menu_EnterPageByType(MENU_PAGE_LOCAL_CALLLOG, kszPlacedLogId);
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
    if (configParser_GetConfigInt(kszPlcmPhoneEnable) == 1)
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
    return DM_EnterDirectoryUI(DIRECTORY_TYPE_METASWICTH);
}
#endif
