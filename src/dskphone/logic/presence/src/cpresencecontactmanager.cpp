#include "cpresencecontactmanager.h"

#include "etlmsghandler/modetlmsghandler.h"
#include "basecontactdata.h"
#include "uccontactdata.h"
#include "devicelib/networkserver.h"
#include "configparser/modconfigparser.h"
#include <configiddefine.h>
#include "dsskey/include/moddsskey.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "../include/modpresence.h"
#include "dsskey/include/dsskeymsgdefine.h"
#include "autoset/include/autoset_def.h"
#include "talk/talklogic/include/talkmsgdefine.h"

#if FEATURE_BROADTOUCH_UC

static Dsskey_Status PresenceStatus2DsskeyStatus(_PRESENCE_TYPE ePresenceType)
{
    Dsskey_Status iStatus = DS_UC_OFFLINE;
    switch (ePresenceType)
    {
    case _PRESENCE_TYPE_ONLINE:
        iStatus = DS_UC_ONLINE;
        break;
    case _PRESENCE_TYPE_CHATTY:
        iStatus = DS_UC_CHATTY;
        break;
    case _PRESENCE_TYPE_AWAY:
        iStatus = DS_UC_AWAY;
        break;
    case _PRESENCE_TYPE_EXTENDED_AWAY:
        iStatus = DS_UC_EXTENDED_AWAY;
        break;
    case _PRESENCE_TYPE_BUSY:
        iStatus = DS_UC_BUSY;
        break;
    case _PRESENCE_TYPE_OFFLINE:
        iStatus = DS_UC_OFFLINE;
        break;
    case _PRESENCE_TYPE_UNKNOWN:
        iStatus = DS_UC_UNKNOWN;
        break;
    default:
        break;
    }
    return iStatus;
}

static LRESULT PresenceProcessMessage(msgObject & objMessage)
{
    if (g_objPresenceManager.OnUCMessage(objMessage))
    {
        return true;
    }

    return false;
}


CPresenceContactManager & CPresenceContactManager::GetInstance()
{
    static CPresenceContactManager instace;

    return instace;
}

bool CPresenceContactManager::GetImageByNumber(yl::string & strPath, const yl::string & strNumber)
{
    if (g_pUCPresence != NULL)
    {
        return g_pUCPresence->GetImageByNumber(strPath, strNumber);
    }

    return false;
}


bool CPresenceContactManager::GetNameByNumber(yl::string & strName, const yl::string & strNumber,
        PresenceType eType, int nLine/* = 0*/)
{
    CBaseContactData * pContactData = GetCotactData(eType, nLine);

    if (pContactData != NULL)
    {
        return pContactData->GetNameByNumber(strName, strNumber);
    }

    return false;
}

LRESULT CPresenceContactManager::OnUCMessage(msgObject & refObj)
{
    APP_INFO("Presence ProcessNetworkMsg(%d)", refObj.message);
    switch (refObj.message)
    {
    case CONFIG_MSG_BCAST_CHANGED:
        {
            // UC账号信息修改，或者UC功能开启时需要重新更新信息
            if (refObj.wParam == ST_PRESENCE
                    || refObj.wParam == ST_UCACTIVE)
            {
                if (refObj.wParam == ST_PRESENCE
                        && refObj.lParam == XMPP_OPTIONS_SET_PROPERIES_MANUAL_FIELD)
                {
                    g_pUCPresence->SetXmppFeaturesManual();
                    return true;
                }

                // refresh all info
                g_pUCPresence->RefreshAllInfo();
                return true;
            }
            else if (ST_PRESENCE_CHANGE == refObj.wParam)
            {
                g_pUCPresence->SetAllowChangeStatus(configParser_GetConfigInt(kszEnableChangePresenceStatus));
                return true;
            }
        }
        break;
    case PRESENCE_MESSAGE_AVATAR_CHANGE:
        {
            yl::string strContactId = (char *)refObj.GetExtraData();
            if (g_pUCPresence->RefreshAvatar(strContactId))
            {
                g_pUCPresence->RefreshViewerPresence();
            }
            return true;
        }
        break;
    case PRESENCE_CONTACT_CHANGE:
        {
            char * pXml_stream = (char *)refObj.GetExtraData();
            RefreshType nRefreshType = REFRESH_NONE;
            if (pXml_stream != NULL && g_pUCPresence->RefreshRosterInfo(pXml_stream, nRefreshType))
            {
                g_pUCPresence->RefreshViewerPresence();
            }
            else if (REFRESH_SINGLE == nRefreshType)
            {
                if (g_pUCPresence->ReqAddressbookList())
                {
                    g_pUCPresence->RefreshViewerPresence();
                }
            }
            else if (REFRESH_ALL == nRefreshType)
            {
                if (!m_RefreshAllDelayTimer.IsTimerRuning())
                {
                    m_RefreshAllDelayTimer.SetTimer(2000, NULL, MK_MSG_ENTRY(this,
                                                    CPresenceContactManager::OnRefreshAllDelayTimer));
                }
            }
            return true;
        }
        break;
    case PRESENCE_PRESENCE_CHANGE:
        {
            char * pXml_stream = (char *)refObj.GetExtraData();
            if (pXml_stream != NULL && g_pUCPresence->ProcessPresenceStream(pXml_stream))
            {
                g_pUCPresence->RefreshViewerPresence();
            }
            return true;
        }
        break;
    case PRESENCE_PRESENCE_DISCONNECT:
        {
            if (m_InitTimer.IsTimerRuning())
            {
                return true;
            }

            m_InitTimer.SetTimer(5000, NULL, MK_MSG_ENTRY(this, CPresenceContactManager::OnTimeoutMessage));
        }
        break;
    default:
        break;
    }
    return false;
}

void CPresenceContactManager::Init()
{
    APP_INFO("CPresenceContactManager::Init()");

    etl_RegisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED, PresenceProcessMessage);
    etl_RegisterMsgHandle(PRESENCE_MESSAGE_AVATAR_CHANGE, PRESENCE_MESSAGE_AVATAR_CHANGE,
                          PresenceProcessMessage);
    etl_RegisterMsgHandle(PRESENCE_CONTACT_CHANGE, PRESENCE_PRESENCE_DISCONNECT,
                          PresenceProcessMessage);
    etl_RegisterMsgHandle(DSK_MSG_EXT_CHANGE, DSK_MSG_EXT_CHANGE, PresenceProcessMessage);

    // for test
    //configParser_SetConfigInt(kszEnableUC,1);

    if (configParser_GetConfigInt(kszEnableUC) == 1
            && configParser_GetConfigInt(kszBroadsoftActive) == 1)
    {
        if (netIsNetworkReady())
        {
            g_pUCPresence->RefreshAllInfo();
        }
        else
        {
            m_InitTimer.SetTimer(5000, NULL, MK_MSG_ENTRY(this, CPresenceContactManager::OnTimeoutMessage));
        }
    }
}

CPresenceContactManager::CPresenceContactManager()
{
}

BOOL CPresenceContactManager::OnTimeoutMessage(msgObject & refObj)
{
    if (netIsNetworkReady())
    {
        if (m_InitTimer.IsTimerRuning())
        {
            //关闭超时计时器
            m_InitTimer.KillTimer();
        }

        g_pUCPresence->RefreshAllInfo();
    }
    return TRUE;
}

CPresenceContactManager::~CPresenceContactManager()
{
    etl_UnregisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED, PresenceProcessMessage);
    etl_UnregisterMsgHandle(PRESENCE_MESSAGE_AVATAR_CHANGE, PRESENCE_MESSAGE_AVATAR_CHANGE,
                            PresenceProcessMessage);
    etl_UnregisterMsgHandle(PRESENCE_CONTACT_CHANGE, PRESENCE_PRESENCE_DISCONNECT,
                            PresenceProcessMessage);

    delete g_pUCPresence;
}

CBaseContactData * CPresenceContactManager::GetCotactData(PresenceType eType, int nline,
        bool IsCreat)
{
    if (eType == PRESENCE_UC)
    {
        return g_pUCPresence;
    }
    return NULL;
}

bool CPresenceContactManager::GetListByGroupId(ContactBuddyListData * pData, GroupKey & keyGroup,
        int nLine, const yl::string & strKey /* = */, AttrType eSearchType/* = ATTR_ALL*/)
{
    if (pData == NULL)
    {
        return false;
    }

    CBaseContactData * pContactData = GetCotactData(pData->m_eType, nLine);

    if (pContactData != NULL)
    {
        return pContactData->GetListByGroupId(pData, keyGroup, strKey, eSearchType);
    }

    return false;
}

bool CPresenceContactManager::GetGroupList(PresenceType eType, GroupList & refList, int nLine)
{
    CBaseContactData * pContactData = GetCotactData(eType, nLine);

    if (pContactData != NULL)
    {
        return pContactData->GetGroupList(refList);
    }

    return false;

}

int CPresenceContactManager::GetChildCountByGroupId(GroupKey & keyGroup, PresenceType eType,
        int nLine /* = 0 */)
{
    CBaseContactData * pContactData = GetCotactData(eType, nLine);

    if (pContactData != NULL)
    {
        return pContactData->GetChildCountByGroupId(keyGroup);
    }

    return -1;
}

bool CPresenceContactManager::ReqChangPresence(_PRESENCE_TYPE eType)
{
    return g_pUCPresence->ReqChangPresence(eType);
}

bool CPresenceContactManager::GetContactDetail(PresenceInfo & refData, PresenceType eType,
        int nLine /* = 0 */)
{
    CBaseContactData * pData = GetCotactData(eType);

    if (pData != NULL)
    {
        return pData->GetContactDetail(refData);
    }
    return false;
}

bool CPresenceContactManager::IsAllowChangePresenceStatus()
{
    return g_pUCPresence->IsAllowChangeStatus();
}

BOOL CPresenceContactManager::OnRefreshAllDelayTimer(msgObject & refObj)
{
    m_RefreshAllDelayTimer.KillTimer();

    if (talklogic_SessionExist())
    {
        m_RefreshAllDelayTimer.SetTimer(5000, NULL, MK_MSG_ENTRY(this,
                                        CPresenceContactManager::OnRefreshAllDelayTimer));
        return TRUE;
    }

    if (g_pUCPresence->ReqAddressbookList(true))
    {
        g_pUCPresence->RefreshViewerPresence();
    }
    return TRUE;
}

#endif //FEATURE_BROADTOUCH_UC

