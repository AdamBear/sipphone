#include "dirdsskeymgr.h"


IMPLEMENT_GETINSTANCE(CDirDssKeyMgr)

CDirDssKeyMgr::CDirDssKeyMgr()
{
}

CDirDssKeyMgr::~CDirDssKeyMgr()
{
    UnInit();
}

void CDirDssKeyMgr::Init()
{
    if (!m_mapDssKey.empty())
    {
        return;
    }

    CreateDssKeyXmlGroup();
    CreateDssKeyIdleCallList();
    CreateDssKeyLDAP();
    CreateDssKeyLocalDir();
    CreateDssKeyBSDir();
    CreateDssKeyLocalGroup();
    CreateDssKeyBSGroup();
    CreateDssKeyIdleRemote();
    CreateDssKeyFavorites();
    CreateDssKeyNetworkFavorite();
    CreateDssKeyUCFavorite();
    CreateDssKeyBuddies();
    CreateDssKeyStatus();
    CreateDsskeyRedial();
#if IF_BUG_14856
    CreateDssKeyMissedCall();
#endif
#if IF_BUG_PLCMDIR
    CreateDssKeyPlcmDir();
#endif
#if IF_FEATURE_METASWITCH_DIRECTORY
    CreateDssKeyMTSWDir();
#endif
}

void CDirDssKeyMgr::UnInit()
{
    MAP_DSS_KEY::iterator iter = m_mapDssKey.begin();

    for (; iter != m_mapDssKey.end(); ++iter)
    {
        IDsskeyImplActionPtr pAction = iter->second;

        if (NULL == pAction)
        {
            continue;
        }

        delete pAction;
    }

    m_mapDssKey.clear();
}

bool CDirDssKeyMgr::IsDirDssKey(int nDssKeyType)
{
    MAP_DSS_KEY::iterator iter = m_mapDssKey.find(nDssKeyType);

    return (iter != m_mapDssKey.end());
}

void CDirDssKeyMgr::CreateDssKeyXmlGroup()
{
    CDssKeyActionXMLGroup * pAction = new CDssKeyActionXMLGroup;

    if (NULL == pAction)
    {
        return;
    }

    m_mapDssKey[DT_XMLGROUP] = pAction;
}

void CDirDssKeyMgr::CreateDssKeyIdleCallList()
{
    CDssKeyActionIdleCallList * pAction = new CDssKeyActionIdleCallList;

    if (NULL == pAction)
    {
        return;
    }

    m_mapDssKey[DT_IDLE_CALLLIST] = pAction;
}

void CDirDssKeyMgr::CreateDssKeyLDAP()
{
    CDssKeyActionLDAP * pAction = new CDssKeyActionLDAP;

    if (NULL == pAction)
    {
        return;
    }

    m_mapDssKey[DT_LDAP] = pAction;
}

void CDirDssKeyMgr::CreateDssKeyLocalDir()
{
    CDssKeyActionLocalDir * pAction = new CDssKeyActionLocalDir;

    if (NULL == pAction)
    {
        return;
    }

    m_mapDssKey[DT_LOCALDIR] = pAction;
}

void CDirDssKeyMgr::CreateDssKeyBSDir()
{
    CDssKeyActionBSDir * pAction = new CDssKeyActionBSDir;

    if (NULL == pAction)
    {
        return;
    }

    m_mapDssKey[DT_BSDIR] = pAction;
}

void CDirDssKeyMgr::CreateDssKeyLocalGroup()
{
    CDssKeyActionLocalGroup * pAction = new CDssKeyActionLocalGroup;

    if (NULL == pAction)
    {
        return;
    }

    m_mapDssKey[DT_LOCALGROUP] = pAction;
}

void CDirDssKeyMgr::CreateDssKeyBSGroup()
{
    CDssKeyActionBSGroup * pAction = new CDssKeyActionBSGroup;

    if (NULL == pAction)
    {
        return;
    }

    m_mapDssKey[DT_BSGROUP] = pAction;
}

void CDirDssKeyMgr::CreateDssKeyIdleRemote()
{
    CDssKeyActionIdleRemote * pAction = new CDssKeyActionIdleRemote;

    if (NULL == pAction)
    {
        return;
    }

    m_mapDssKey[DT_IDLE_REMOTE] = pAction;
}

void CDirDssKeyMgr::CreateDssKeyFavorites()
{
    CDssKeyActionFavrites * pAction = new CDssKeyActionFavrites(DT_FAVORITES);

    if (NULL == pAction)
    {
        return;
    }

    m_mapDssKey[DT_FAVORITES] = pAction;

    CDssKeyActionFavrites * pIdleContactsAction = new CDssKeyActionFavrites(DT_IDLE_CONTACTS);

    if (NULL == pIdleContactsAction)
    {
        return;
    }

    m_mapDssKey[DT_IDLE_CONTACTS] = pIdleContactsAction;
}

void CDirDssKeyMgr::CreateDssKeyNetworkFavorite()
{
    CDssKeyActionNetworkFavorite * pAction = new CDssKeyActionNetworkFavorite;

    if (NULL == pAction)
    {
        return;
    }

    m_mapDssKey[DT_NETWORK_FAVORITE] = pAction;
}

void CDirDssKeyMgr::CreateDssKeyUCFavorite()
{
    CDssKeyUCFavorite * pAction = new CDssKeyUCFavorite;

    if (NULL == pAction)
    {
        return;
    }

    m_mapDssKey[DT_UC_FAVORITE] = pAction;
}

void CDirDssKeyMgr::CreateDssKeyBuddies()
{
    CDssKeyBuddies * pAction = new CDssKeyBuddies;

    if (NULL == pAction)
    {
        return;
    }

    m_mapDssKey[DT_BUDDIES] = pAction;
}

void CDirDssKeyMgr::CreateDsskeyRedial()
{
    CDsskeyRedialAciton * pAction = new CDsskeyRedialAciton;

    if (NULL == pAction)
    {
        return;
    }

    m_mapDssKey[DT_REDIAL] = pAction;
}

void CDirDssKeyMgr::CreateDssKeyStatus()
{
    CDssKeyMyStatus * pAction = new CDssKeyMyStatus;

    if (NULL == pAction)
    {
        return;
    }

    m_mapDssKey[DT_MY_STATUS] = pAction;
}

#if IF_BUG_14856
void CDirDssKeyMgr::CreateDssKeyMissedCall()
{
    CDssKeyActionMissedCall * pAction = new CDssKeyActionMissedCall;

    if (NULL == pAction)
    {
        return;
    }

    m_mapDssKey[DT_MISSED_CALL] = pAction;
}
#endif

#if IF_BUG_PLCMDIR
void CDirDssKeyMgr::CreateDssKeyPlcmDir()
{
    CDssKeyActionPlcmDir * pAction = new CDssKeyActionPlcmDir;

    if (NULL == pAction)
    {
        return;
    }

    m_mapDssKey[DT_PLCM_PHONEBOOK] = pAction;
}
#endif

#if IF_FEATURE_METASWITCH_DIRECTORY
void CDirDssKeyMgr::CreateDssKeyMTSWDir()
{
    CDssKeyActionMTSWDir * pAction = new CDssKeyActionMTSWDir;

    if (NULL == pAction)
    {
        return;
    }

    m_mapDssKey[DT_MTSWDIR] = pAction;
}
#endif
