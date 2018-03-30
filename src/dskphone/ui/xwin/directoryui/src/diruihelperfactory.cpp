#include "diruihelperfactory.h"
#include "bsftcallloguihelper.h"
#include "bsftcontactuihelper.h"
#include "ldapuihelper.h"
#include "localcallloguihelper.h"
#include "localcontactuihelper.h"
#include "remoteuihelper.h"
#include "remoteserversearchuihelper.h"
#ifdef IF_SUPPORT_BLUETOOTH_CONTACT
#include "bluetoothcontactuihelp.h"
#endif
#if IF_BUG_PLCMDIR
#include "plcmuihelper.h"
#endif
#if IF_FEATURE_GBBOOK
#include "genbandcontactuihelper.h"
#endif
#if FEATURE_BROADTOUCH_UC
#include "btuccontactuihelper.h"
#endif
#if IF_FEATURE_METASWITCH_DIRECTORY
#include "mtswcontactuihelper.h"
#endif
#if IF_FEATURE_METASWITCH_CALLLOG
#include "mtswcallloguihelper.h"
#endif

CDirUIHelperFactory::CDirUIHelperFactory()
{
}

CDirUIHelperFactory::~CDirUIHelperFactory()
{
    Clear();
}

CDirUIHelperFactory & CDirUIHelperFactory::GetInstance()
{
    static CDirUIHelperFactory objFactory;

    return objFactory;
}

IDirUIHelperPtr CDirUIHelperFactory::FindDirUIHelper(int nType)
{
    MAP_DIR_UI_HELPER::iterator iter = m_mapDirUIHelper.find(nType);

    if (iter != m_mapDirUIHelper.end())
    {
        return iter->second;
    }

    return IDirUIHelperPtr(NULL);
}

IDirUIHelperPtr CDirUIHelperFactory::GetDirUIHelper(int nType)
{
    IDirUIHelperPtr pUIHelper = FindDirUIHelper(nType);

    if (!pUIHelper.IsEmpty())
    {
        return pUIHelper;
    }

    switch (nType)
    {
    case DIRECTORY_TYPE_LOCAL:
        {
            pUIHelper = IDirUIHelperPtr(new CLocalContactUIHelper);
            DIRUI_DEBUG("new CLocalContactUIHelper:%p", pUIHelper.Get());
        }
        break;
    case DIRECTORY_TYPE_REMOTE:
        {
            pUIHelper = IDirUIHelperPtr(new CRemoteUIHelper);
            DIRUI_DEBUG("new CRemoteUIHelper:%p", pUIHelper.Get());
        }
        break;
    case DIRECTORY_TYPE_LDAP:
        {
            pUIHelper = IDirUIHelperPtr(new CLDAPUIHelper);
            DIRUI_DEBUG("new CLDAPUIHelper:%p", pUIHelper.Get());
        }
        break;
    case DIRECTORY_TYPE_BROADSOFT:
        {
            pUIHelper = IDirUIHelperPtr(new CBSFTContactUIHelper);
            DIRUI_DEBUG("new CBSFTContactUIHelper:%p", pUIHelper.Get());
        }
        break;
#if FEATURE_BROADTOUCH_UC
    case DIRECTORY_TYPE_BTUC:
        {
            pUIHelper = IDirUIHelperPtr(new CBTUCUIHelper);
            DIRUI_DEBUG("new CBTUCUIHelper:%p", pUIHelper.Get());
        }
        break;
#endif
#ifdef IF_SUPPORT_BLUETOOTH_CONTACT
    case DIRECTORY_TYPE_BLUETOOTH:
        {
            pUIHelper = IDirUIHelperPtr(new CBluetoothUIHelper);
            DIRUI_DEBUG("new CBluetoothUIHelper:%p", pUIHelper.Get());
        }
        break;
#endif
#if IF_BUG_PLCMDIR
    case DIRECTORY_TYPE_PLCM:
        {
            pUIHelper = IDirUIHelperPtr(new CPlcmUIHelper);
            DIRUI_DEBUG("new CPlcmUIHelper:%p", pUIHelper.Get());
        }
        break;
#endif
    case DIRECTORY_TYPE_REMOTE_SEARCH:
        {
            pUIHelper = IDirUIHelperPtr(new CRemoteServerSearchUIHelper);
            DIRUI_DEBUG("new CRemoteServerSearchUIHelper:%p", pUIHelper.Get());
        }
        break;
#if IF_FEATURE_GBBOOK
    case DIRECTORY_TYPE_GENBAND:
        {
            pUIHelper = IDirUIHelperPtr(new CGenbandContactUIHelper);
            DIRUI_DEBUG("new CGenbandContactUIHelper:%p", pUIHelper.Get());
        }
        break;
#endif
#if IF_FEATURE_METASWITCH_DIRECTORY
    case DIRECTORY_TYPE_METASWICTH:
        {
            pUIHelper = IDirUIHelperPtr(new CMTSWContactUIHelper);
            DIRUI_DEBUG("new CMTSWContactUIHelper:%p", pUIHelper.Get());
        }
        break;
#endif
    case CALL_LOG_TYPE_LOCAL:
        {
            pUIHelper = IDirUIHelperPtr(new CLocalCallLogUIHelper);
            DIRUI_DEBUG("new CLocalCallLogUIHelper:%p", pUIHelper.Get());
        }
        break;
    case CALL_LOG_TYPE_BROADSOFT:
        {
            pUIHelper = IDirUIHelperPtr(new CBSFTCallLogUIHelper);
            DIRUI_DEBUG("new CBSFTCallLogUIHelper:%p", pUIHelper.Get());
        }
        break;
#if IF_FEATURE_METASWITCH_CALLLOG
    case CALL_LOG_TYPE_MTSW:
        {
            pUIHelper = IDirUIHelperPtr(new CMTSWCallLogUIHelper);
            DIRUI_DEBUG("new CMTSWCallLogUIHelper:%p", pUIHelper.Get());
        }
        break;
#endif
    default:
        break;
    }

    if (!pUIHelper.IsEmpty())
    {
        m_mapDirUIHelper[nType] = pUIHelper;
    }

    return pUIHelper;
}

void CDirUIHelperFactory::DeleteDirUIHelper(int nType)
{
    MAP_DIR_UI_HELPER::iterator iter = m_mapDirUIHelper.find(nType);

    if (iter != m_mapDirUIHelper.end())
    {
        m_mapDirUIHelper.erase(iter);
    }
}

void CDirUIHelperFactory::Clear()
{
    m_mapDirUIHelper.clear();
}
