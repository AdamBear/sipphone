#include "dirctrlfactory.h"
#include "localcontactcontroler.h"
#include "remotecontactcontroler.h"
#include "remoteserversearchcontroler.h"
#include "ldapcontactcontroler.h"
#include "bsftcontactcontroler.h"
#include "bluetoothcontactcontroler.h"
#include "localcalllogcontroler.h"
#include "bsftcalllogcontroler.h"
#ifdef IF_SUPPORT_BLUETOOTH_CONTACT
#include "bluetoothcontactcontroler.h"
#endif
#if IF_BUG_PLCMDIR
#include "plcmcontactcontroler.h"
#endif
#if IF_FEATURE_GBBOOK
#include "genbandcontactcontroler.h"
#endif
#if FEATURE_BROADTOUCH_UC
#include "btuccontactcontroler.h"
#endif
#if IF_FEATURE_METASWITCH_DIRECTORY
#include "mtswcontactcontroler.h"
#endif
#if IF_FEATURE_METASWITCH_CALLLOG
#include "mtswcalllogcontroler.h"
#endif

IMPLEMENT_GETINSTANCE(CDirCtrlFactory)

CDirCtrlFactory::CDirCtrlFactory()
{
}

CDirCtrlFactory::~CDirCtrlFactory()
{
    Clear();
}

CDirectoryControlerPtr CDirCtrlFactory::GetControler(int nType)
{
    CDirectoryControlerPtr pControler = FindControler(nType);

    if (!pControler.IsEmpty())
    {
        return pControler;
    }

    switch (nType)
    {
    case DIRECTORY_TYPE_LOCAL:
        {
            pControler = CDirectoryControlerPtr(new CLocalContactControler);
            DIRUI_DEBUG("new CLocalContactControler:%p", pControler.Get());
        }
        break;
    case DIRECTORY_TYPE_REMOTE:
        {
            pControler = CDirectoryControlerPtr(new CRemoteContactControler);
            DIRUI_DEBUG("new CRemoteContactControler:%p", pControler.Get());
        }
        break;
    case DIRECTORY_TYPE_LDAP:
        {
            pControler = CDirectoryControlerPtr(new CLDAPContactControler);
            DIRUI_DEBUG("new CLDAPContactControler:%p", pControler.Get());
        }
        break;
    case DIRECTORY_TYPE_BROADSOFT:
        {
            pControler = CDirectoryControlerPtr(new CBSFTContactControler);
            DIRUI_DEBUG("new CBSFTContactControler:%p", pControler.Get());
        }
        break;
#if FEATURE_BROADTOUCH_UC
    case DIRECTORY_TYPE_BTUC:
        {
            pControler = CDirectoryControlerPtr(new CBTUCContactControler);
            DIRUI_DEBUG("new CBTUCContactControler:%p", pControler.Get());
        }
        break;
#endif
#ifdef IF_SUPPORT_BLUETOOTH_CONTACT
    case DIRECTORY_TYPE_BLUETOOTH:
        {
            pControler = CDirectoryControlerPtr(new CBluetoothContactControler);
            DIRUI_DEBUG("new CBluetoothContactControler:%p", pControler.Get());
        }
        break;
#endif
#if IF_BUG_PLCMDIR
    case DIRECTORY_TYPE_PLCM:
        {
            pControler = CDirectoryControlerPtr(new CPlcmContactControler);
            DIRUI_DEBUG("new CPlcmContactControler:%p", pControler.Get());
        }
        break;
#endif
    case DIRECTORY_TYPE_REMOTE_SEARCH:
        {
            pControler = CDirectoryControlerPtr(new CRemoteServerSearchControler);
            DIRUI_DEBUG("new CRemoteServerSearchControler:%p", pControler.Get());
        }
        break;
#if IF_FEATURE_GBBOOK
    case DIRECTORY_TYPE_GENBAND:
        {
            pControler = CDirectoryControlerPtr(new CGenbandContactControler);
            DIRUI_DEBUG("new CGenbandContactControler:%p", pControler.Get());
        }
        break;
#endif
#if IF_FEATURE_METASWITCH_DIRECTORY
    case DIRECTORY_TYPE_METASWICTH:
        {
            pControler = CDirectoryControlerPtr(new CMTSWContactControler);
            DIRUI_DEBUG("new CMTSWContactControler:%p", pControler.Get());
        }
        break;
#endif
    case CALL_LOG_TYPE_LOCAL:
        {
            pControler = CDirectoryControlerPtr(new CLocalCallLogControler);
            DIRUI_DEBUG("new CLocalCallLogControler:%p", pControler.Get());
        }
        break;
    case CALL_LOG_TYPE_BROADSOFT:
        {
            pControler = CDirectoryControlerPtr(new CBSFTCallLogControler);
            DIRUI_DEBUG("new CBSFTCallLogControler:%p", pControler.Get());
        }
        break;
#if IF_FEATURE_METASWITCH_CALLLOG
    case CALL_LOG_TYPE_MTSW:
        {
            pControler = CDirectoryControlerPtr(new CMTSWCallLogControler);
            DIRUI_DEBUG("new CMTSWCallLogControler:%p", pControler.Get());
        }
        break;
#endif
    default:
        {
            DIRUI_ERR("CDirCtrlFactory::GetControler(%d) invalid type", nType);
        }
        break;
    }

    if (!pControler.IsEmpty())
    {
        m_mapDirCtrler[nType] = pControler;
    }

    return pControler;
}

CDirectoryControlerPtr CDirCtrlFactory::FindControler(int nType)
{
    MAP_DIR_CTRLER::iterator iter = m_mapDirCtrler.find(nType);

    if (iter != m_mapDirCtrler.end())
    {
        return iter->second;
    }

    return CDirectoryControlerPtr(NULL);
}

void CDirCtrlFactory::DeleteControler(int nType)
{
    MAP_DIR_CTRLER::iterator iter = m_mapDirCtrler.find(nType);

    if (iter != m_mapDirCtrler.end())
    {
        m_mapDirCtrler.erase(iter);
    }
}

void CDirCtrlFactory::Clear()
{
    m_mapDirCtrler.clear();
}

bool CDirCtrlFactory::IsControlerValid(int nType, CDirectoryControlerPtr pControler)
{
    if (pControler.IsEmpty())
    {
        return false;
    }

    if (pControler->GetType() == nType)
    {
        return true;
    }

    return false;
}
