#include "diruihelperfactory.h"
#include "bsftcallloguihelper.h"
#include "bsftcontactuihelper.h"
#include "ldapuihelper.h"
#include "btucuihelper.h"
#include "localcallloguihelper.h"
#include "localcontactuihelper.h"
#include "bluetoothuihelper.h"
#include "remoteuihelper.h"
#include "remoteserversearchuihelper.h"
#if IF_BUG_PLCMDIR
#include "plcmuihelper.h"
#endif
#if IF_FEATURE_GBBOOK
#include "genbandcontactuihelper.h"
#endif


#if IF_FEATURE_METASWITCH_CALLLOG
#include "mtswcallloguihelper.h"
#endif
#if IF_FEATURE_METASWITCH_DIRECTORY
#include "mtswcontactuihelper.h"
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

CDirUIHelperPtr CDirUIHelperFactory::FindDirUIHelper(int nType)
{
    MAP_DIR_UI_HELPER::iterator iter = m_mapDirUIHelper.find(nType);

    if (iter != m_mapDirUIHelper.end())
    {
        return iter->second;
    }

    return CDirUIHelperPtr(NULL);
}

CDirUIHelperPtr CDirUIHelperFactory::GetDirUIHelper(int nType)
{
    CDirUIHelperPtr pUIHelper = FindDirUIHelper(nType);

    if (!pUIHelper.IsEmpty())
    {
        return pUIHelper;
    }

    switch (nType)
    {
    case DIRECTORY_TYPE_LOCAL:
        {
            pUIHelper = CDirUIHelperPtr(new CLocalContactUIHelper);
            DIRUI_INFO("new CLocalContactUIHelper:%p", pUIHelper.Get());
        }
        break;
    case DIRECTORY_TYPE_REMOTE:
        {
            pUIHelper = CDirUIHelperPtr(new CRemoteUIHelper);
            DIRUI_INFO("new CRemoteUIHelper:%p", pUIHelper.Get());
        }
        break;
    case DIRECTORY_TYPE_LDAP:
        {
            pUIHelper = CDirUIHelperPtr(new CLDAPUIHelper);
            DIRUI_INFO("new CLDAPUIHelper:%p", pUIHelper.Get());
        }
        break;
    case DIRECTORY_TYPE_BROADSOFT:
        {
            pUIHelper = CDirUIHelperPtr(new CBSFTContactUIHelper);
            DIRUI_INFO("new CBSFTContactUIHelper:%p", pUIHelper.Get());
        }
        break;
    case DIRECTORY_TYPE_BTUC:
        {
            pUIHelper = CDirUIHelperPtr(new CBTUCUIHelper);
            DIRUI_INFO("new CBTUCUIHelper:%p", pUIHelper.Get());
        }
        break;
    case DIRECTORY_TYPE_BLUETOOTH:
        {
            pUIHelper = CDirUIHelperPtr(new CBluetoothUIHelper);
            DIRUI_INFO("new CBluetoothUIHelper:%p", pUIHelper.Get());
        }
        break;
#if IF_BUG_PLCMDIR
    case DIRECTORY_TYPE_PLCM:
        {
            pUIHelper = CDirUIHelperPtr(new CPlcmUIHelper);
            DIRUI_INFO("new CPlcmUIHelper:%p", pUIHelper.Get());
        }
        break;
#endif
#if IF_FEATURE_GBBOOK
    case DIRECTORY_TYPE_GENBAND:
        {
            pUIHelper = CDirUIHelperPtr(new CGenbandContactUIHelper);
            DIRUI_INFO("new CGenbandContactUIHelper:%p", pUIHelper.Get());
        }
        break;
#endif
    case CALL_LOG_TYPE_LOCAL:
        {
            pUIHelper = CDirUIHelperPtr(new CLocalCallLogUIHelper);
            DIRUI_INFO("new CLocalCallLogUIHelper:%p", pUIHelper.Get());
        }
        break;
    case CALL_LOG_TYPE_BROADSOFT:
        {
            pUIHelper = CDirUIHelperPtr(new CBSFTCallLogUIHelper);
            DIRUI_INFO("new CBSFTCallLogUIHelper:%p", pUIHelper.Get());
        }
        break;
    case DIRECTORY_TYPE_REMOTE_SEARCH:
        {
            pUIHelper = CDirUIHelperPtr(new CRemoteServerSearchUIHelper);
            DIRUI_INFO("new CRemoteServerSearchUIHelper:%p", pUIHelper.Get());
        }
        break;
#if IF_FEATURE_METASWITCH_CALLLOG
    case CALL_LOG_TYPE_MTSW:
        {
            pUIHelper = CDirUIHelperPtr(new CMTSWCallLogUIHelper);
            DIRUI_INFO("new CMTSWCallLogUIHelper:%p", pUIHelper.Get());
        }
        break;
#endif
#if IF_FEATURE_METASWITCH_DIRECTORY
    case DIRECTORY_TYPE_METASWICTH:
        {
            pUIHelper = CDirUIHelperPtr(new CMTSWContactUIHelper);
            DIRUI_INFO("new CMTSWContactUIHelper:%p", pUIHelper.Get());
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
