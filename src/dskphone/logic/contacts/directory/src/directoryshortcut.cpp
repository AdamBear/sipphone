#include "directoryshortcut.h"
#include "configiddefine.h"
#include "configparser/modconfigparser.h"
#include "commondata.h"
//#include "define/common.h"
#include "remotecontactmanager.h"
#include "dirdefine.h"
#include "directorystructure.h"
#include "contacts/ldaplogic/include/modldap.h"

#if IF_FEATURE_METASWITCH
#include "contacts/directory/include/moddirinterface.h"
#endif
#if IF_BUG_39755
#include "contacts/directory/src/bluetoothcontactmanager.h"
#endif


//////////////////////////////////////////////////////////////////////////
// Class CDirectoryShortcut.

CDirectoryShortcut * CDirectoryShortcut::GetInstance()
{
    static CDirectoryShortcut locallogProcessor;
    return &locallogProcessor;
}

bool CDirectoryShortcut::Init()
{
    // 加载配置文件.
    return ReloadConfigure();
}

CDirectoryShortcut::CDirectoryShortcut()
{
}

CDirectoryShortcut::~CDirectoryShortcut()
{

}


bool CDirectoryShortcut::GetShortcutList(DirShortcutListData * pData)
{
    if (pData == NULL)
    {
        return false;
    }

    bool isBDOn = configParser_GetConfigInt(kszBroadsoftActive) == 1
                  && configParser_GetConfigInt(kszXSISwitch) == 1;

    // 获取起止下标.
    int nIndexFrom = pData->m_nIndexFrom;
    int nCountToGet = pData->m_nCountToGet;

    if (nIndexFrom < 0)
    {
        nIndexFrom = 0;
    }
    if (nIndexFrom == 0 && nCountToGet <= 0)
    {
        nCountToGet = m_listData.size();
    }

    int nAmount = 0;
    for (YLList<DirectoryShortcutData>::iterator iter = m_listData.begin() ; iter != m_listData.end()
            && nAmount < nCountToGet; iter++)
    {
        // 获取指定数量的配置项.
        DirShortcutData stDSData;

        DirectoryShortcutData & refShortcutData = *iter;


        if (!isBDOn
                && (refShortcutData.m_strIdName.compare("broadsoftdirectory") == 0
                    || refShortcutData.m_strIdName.compare("networkcalllog") == 0))
        {
            continue;
        }

        if (refShortcutData.m_strIdName.compare("networkcalllog") == 0
                && configParser_GetConfigInt(kszBsftEnableCallLog) == 0)
        {
            continue;
        }

        if (refShortcutData.m_strIdName.compare("broadsoftdirectory") == 0
                && configParser_GetConfigInt(kszBsftEnableDirectory) == 0)
        {
            continue;
        }

        // 如果LDAP开关没开启, 则不显示LDAP项.
        if (refShortcutData.m_strIdName.compare("ldap") == 0)
        {
            if (LDAP_IsEnable())
            {
                yl::string strLdapName = LDAP_GetName();
                if (!strLdapName.empty())
                {
                    refShortcutData.m_strDisplayName = strLdapName;
                }
            }
            else
            {
                continue;
            }
        }

        // 如果没配置远程地址簿, 则不显示远程地址簿项.
        if (refShortcutData.m_strIdName.compare("remotedirectory") == 0
                && !g_pRemoteDirManager->IsDirAvailable())
        {
            continue;
        }

#if IF_BUG_PLCMDIR
        if (refShortcutData.m_strIdName.compare("plcmdirectory") == 0
                && configParser_GetConfigInt(kszPlcmPhoneEnable) != 1)
        {
            continue;
        }
#endif

#if IF_FEATURE_GBBOOK
        if (refShortcutData.m_strIdName.compare("pabdirectory") == 0
                && (configParser_GetConfigInt(kszGbSoupEnable) == 0
                    || configParser_GetConfigInt(kszGbPabBookEnable) == 0))
        {
            continue;
        }

        if (refShortcutData.m_strIdName.compare("gabdirectory") == 0
                && (configParser_GetConfigInt(kszGbSoupEnable) == 0
                    || configParser_GetConfigInt(kszGbGabBookEnable) == 0))
        {
            continue;
        }
#endif

        if (refShortcutData.m_strIdName.compare("metaswitchcontacts") == 0
#if IF_FEATURE_METASWITCH_DIRECTORY
                && !MtswDir_IsEnable()
#endif
           )
        {
            continue;
        }

        if (refShortcutData.m_strIdName.compare("metaswitchcalllog") == 0
#if IF_FEATURE_METASWITCH_CALLLOG
                && !MTSWLog_IsEnable()
#endif
           )
        {
            continue;
        }

#if IF_FEATURE_METASWITCH_CALLLOG
        if (refShortcutData.m_strIdName.compare("history") == 0
                && !LocalLog_IsEnable())
        {
            continue;
        }
#endif

        if (refShortcutData.m_strIdName.compare("uc_buddies") == 0
                && (configParser_GetConfigInt(kszBroadsoftActive) == 0
                    || configParser_GetConfigInt(kszEnableUC) == 0))
        {
            continue;
        }

#if IF_BUG_39755
        if (refShortcutData.m_strIdName.compare("mobilecontant") == 0
#ifdef IF_BT_SUPPORT_PHONE
                && !g_pBlueToothDirManager->IsDirAvailable()
#endif
           )
        {
            continue;
        }
#endif

        stDSData.m_strIdName = refShortcutData.m_strIdName;
        stDSData.m_strDisplayName = refShortcutData.m_strDisplayName;
        pData->m_listData.push_back(stDSData);
        nAmount ++;
    }

    pData->m_nTotalCount = nAmount;

    return true;
}



bool CDirectoryShortcut::ReloadConfigure()
{
    m_listData.clear();

    // 加载.
    return LoadConfigure();
}


bool CDirectoryShortcut::LoadConfigure()
{
    static const char kszConfigureName[] = "favorite_setting.xml";  // 配置文件名.
    static const char kszFavoriteRootNode[] = "root_favorite_set";  // 根节点名.
    static const char kszFavoriteItemNode[] = "item";  // Item节点.

    yl::string strFullPath(g_strConfigPath);

    if (strFullPath.substr(strFullPath.length() - 1) != "/")
    {
        strFullPath.append("/");
    }
    strFullPath.append(kszConfigureName);

    xml_document doc;
    doc.load_file(strFullPath.c_str());

    // 读取根节点.
    xml_node rootNode = doc.child(kszFavoriteRootNode);
    if (!rootNode)
    {
        return false;
    }

    // 读取各个配置结点.
    for (xml_node node = rootNode.child(kszFavoriteItemNode);
            node;
            node = node.next_sibling(kszFavoriteItemNode))
    {
        DirectoryShortcutData item;
        //解析
        item.ParseFromXMLNode(node);

        if (item.m_isEnable == 0)
        {
            continue;
        }
        if (item.m_nPriority == 0xFFFF)
        {
            continue;
        }
        m_listData.push_back(item);
    }  // for.

    Sort(m_listData, DirectoryShortcutData::Compare);

    return true;
}




