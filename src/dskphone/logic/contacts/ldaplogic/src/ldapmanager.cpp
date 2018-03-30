#include "ldapmanager.h"
#include "interfacedefine.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "ldapsearchaction.h"
#include "taskaction/modtaskaction.h"
#include "contacts/directory/include/directorymessagedefine.h"
#include "configiddefine.h"
#include "configparser/modconfigparser.h"
#include "modldap.h"
#include "contacts/directory/src/basemethod.h"
#include "commonunits/modcommonunits.h"

#include "ldap/yealink_ldap.h"

// 默认的LDAP名字.
#define  kszDefLdapName "LDAP"

CLdapSearchAction * TaskAction_CreateLdapSearchAction()
{
    // 创建CLdapAction实例.
    CLdapSearchAction * pAction = new CLdapSearchAction();
    return pAction;
}


LRESULT OnLDAPMessage(msgObject & refObj)
{
    return g_refLdapManager.ProcessMessage(refObj);
}

LdapManager & LdapManager::GetInstance()
{
    static LdapManager instance;
    return instance;
}

LdapManager::LdapManager()
{
    m_bNeedSort = false;
}

LdapManager::~LdapManager()
{
    MyFinal();
    commonUnits_RemoveSysLogFunc(LdapManager::SetLogLevel);
}


// 初始化
void LdapManager::Init()
{
    // 创建LDAP专用线程
    TaskAction_CreateSpecialThread(TA_REQUEST_LDAP, TA_THREAD_LDAP, NULL);

    //注册消息
    etl_RegisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED, OnLDAPMessage);

    yealink_ldap_config_init();

    yealink_ldap_log_init(YEALINK_LDAP_LOG_SYS, NULL);

    SetLogLevel(-1, commonUnits_GetLogLevel());

#if IF_BUG_24324
    yl::string strSep = ";";
    yealink_ldap_set_option(YEALINK_LDAP_OPTIONS_NAME_SEPARATE_SYMBOL, (char *)strSep.c_str());
#endif

    // 将ldap参数传给ldap底层库
    SetLdapOption();

    InitGroupData();

    commonUnits_AddSysLogFunc(LdapManager::SetLogLevel);
}

//设置日志等级
void LdapManager::SetLogLevel(int iModuleID, int iLogLevel)
{
    LDAP_INFO("set ldap log level[%d]", iLogLevel);
    if (iLogLevel >= 0 && iLogLevel < YEALINK_LDAP_LOG_DEBUG)
    {
        yealink_ldap_set_log_level((_yealink_ldap_log_level) iLogLevel);
    }
    else
    {
        yealink_ldap_set_log_level(YEALINK_LDAP_LOG_ERR);
    }
}

// 处理配置更改的消息
bool LdapManager::ProcessMessage(msgObject & refObj)
{
    bool bHandle = true;

    if (ST_LDAP == refObj.wParam)
    {
        LDAP_INFO("LdapManager::ProcessMessage LDAP config change!");

        m_stNowData.Reset();
        m_strSucKey.clear();

        etl_NotifyApp(false, LDAP_STATUS_CHANGED, 0, configParser_GetConfigInt(kszLDAPSwitch));

        // 重新加载配置文件,并设置到ldap底层库中
        SetLdapOption();
    }
    else
    {
        bHandle = false;
    }
    return bHandle;
}

bool LdapManager::ReqSearch(const yl::string & strKey)
{
    if (!LDAP_IsEnable())
    {
        return false;
    }

    LDAP_INFO("Ldap Search lock");
    m_ldapCs.Lock();

    LDAP_INFO("LdapManager::ReqSearch [%s]", strKey.c_str());

    //和之前结果关键字相同则直接通知外部获取数据
    if ((m_strSucKey.compare(strKey) == 0 && !strKey.empty())
            || (strKey.empty() && configParser_GetConfigInt(kszLDAPAutoLoadContact) == 0))
    {
#if IF_FEATURE_34162
        NotifSearchResult(0, strKey);
#else
        NotifSearchResult(true, strKey);
#endif

        LDAP_INFO("Ldap Search Unlock");
        m_ldapCs.Unlock();
        return true;
    }

    if (m_stNowData.m_strNowKey.compare(strKey) == 0 && m_stNowData.m_eNowStatus == SEARCHING)
    {
        LDAP_INFO("Ldap Search Unlock");
        m_ldapCs.Unlock();
        return true;
    }
    // 新建LdapAction.
    else if (CLdapSearchAction * pLdapAction = TaskAction_CreateLdapSearchAction())
    {
        LDAP_INFO("Ldap model: create ldap instance 0x%x",
                  (unsigned int)pLdapAction);
        pLdapAction->SetSync(false);  // 设置异步.
        pLdapAction->SetCallback(this);  // 设置回调类.
        pLdapAction->SetRead(true);  // 设置搜索.
        pLdapAction->SetSearchKey(strKey);  // 设置搜索字符串.
        pLdapAction->SetSort(m_bNeedSort);  // 设置排序.

        // 设置搜索状态.
        m_stNowData.m_eNowStatus = SEARCHING;
        m_stNowData.m_strNowKey = strKey;

        LDAP_INFO("pLdapAction->GetSearchKey [%s], m_strNowKey [%s]",
                  pLdapAction->GetSearchKey().c_str(), m_stNowData.m_strNowKey.c_str());
        LDAP_INFO("Ldap Search Unlock");

        m_ldapCs.Unlock();
        return TaskAction_ExecAction(pLdapAction);
    }

    LDAP_INFO("Ldap Search Unlock");
    m_ldapCs.Unlock();
    return false;

}

bool LdapManager::GetContactList(LdapListData * pData, const yl::string & strKey)
{
    m_ldapCs.Lock();

    if (pData == NULL)
    {
        LDAP_INFO("searchData == NULL");
        m_ldapCs.Unlock();
        return false;
    }

    LDAP_INFO("LdapManager::GetContactList  strKey[%s][%s]", strKey.c_str(), m_strSucKey.c_str());
    if (strKey.compare(m_strSucKey) != 0)
    {
        m_ldapCs.Unlock();
        return false;
    }

#if IF_BUG_31829
    if (configParser_GetConfigInt(kszLDAPHintType) == 0)
    {
        pData->m_nTotalCountByName = GetContactCountsByName(eReqID);
    }
#endif

    DirGroupRecord * pGroup = GetMenuDataById(kszLDAPGroupID);
    if (pGroup == NULL)
    {
        m_ldapCs.Unlock();
        LDAP_INFO("LdapManager::GetContactList Group Is Null");
        return false;
    }
    pData->m_nTotalCount = pGroup->m_vChildrenId.size();

    if (pData->m_nIndexFrom == 0 && pData->m_nCountToGet == 0)
    {
        pData->m_nCountToGet = pData->m_nTotalCount;
    }

    int indexTo = pData->m_nIndexFrom + pData->m_nCountToGet;

    for (int i = pData->m_nIndexFrom; i < pGroup->m_vChildrenId.size() && i < indexTo ; i++)
    {
        LdapContactData * pItem = Base2LdapContact(GetDataById(pGroup->m_vChildrenId[i]));
        if (pItem != NULL)
        {
            pData->m_listData.push_back(*pItem);
        }
    }

    m_ldapCs.Unlock();
    return true;
}

#if IF_BUG_31829
int LdapManager::GetContactCountsByName()
{
    int iCounts = 0;
    // 遍历数组，计算不重名的name有多少个，该算法只支持数组按名称排序
    for (int i = 0; i < m_vLdapData.size(); i++)
    {
        LdapContactData * pItem = m_vLdapData[i];
        if (pItem != NULL)
        {
            for (int j = i + 1; j < m_vLdapData.size(); j++)
            {
                if ((m_vLdapData[j] != NULL) && (pItem->m_strName != m_vLdapData[j]->m_strName))
                {
                    iCounts++;
                    break;
                }
                else
                {
                    i = j;
                }
            }

            if (i == (m_vLdapData.size() - 1))
            {
                iCounts++;
            }
        }
    }

    return iCounts;
}
#endif

bool LdapManager::GetContactInfoByKey(const yl::string & strKey, LdapContactData & stData)
{
    m_ldapCs.Lock();

    if (strKey.compare(m_strSucKey) != 0)
    {
        m_ldapCs.Unlock();
        return false;
    }

    bool suc = false;

    DirGroupRecord * pGroup = GetMenuDataById(kszLDAPGroupID);
    if (pGroup == NULL)
    {
        m_ldapCs.Unlock();
        return false;
    }
    for (int i = 0; i < pGroup->m_vChildrenId.size(); i++)
    {
        LdapContactData * pItem = Base2LdapContact(GetDataById(pGroup->m_vChildrenId[i]));
        if (pItem != NULL)
        {
            YLList<stNumber>::ListIterator iter = pItem->m_listNumber.begin();
            for (; iter != pItem->m_listNumber.end(); ++iter)
            {
                stNumber & stNum = *iter;
                if (stNum.strValue.compare(strKey) == 0)
                {
                    stData = *pItem;
                    suc = true;
                    break;
                }
            }
        }
    }
    m_ldapCs.Unlock();
    return suc;
}

void LdapManager::ClearData()
{
    LDAP_INFO("LdapManager::ClearData() begin");

    ClearContactByGroupID(kszLDAPGroupID);
    ReSetId();

    LDAP_INFO("LdapManager::ClearData() end");
}

void LdapManager::OnTaskActionCallback(NS_TA::CTaskAction * pTaskAction)
{
    LDAP_INFO("LdapManager::OnTaskActionCallback");

    if (pTaskAction == NULL || pTaskAction->GetReqType() != NS_TA::TA_REQUEST_LDAP)
    {
        LDAP_INFO("CLdapModel::OnTaskActionCallback None ldap acton");
        return;
    }

    CLdapSearchAction * pLdapAction = static_cast<CLdapSearchAction *>(pTaskAction);
    if (pLdapAction == NULL)
    {
        LDAP_INFO("CLdapModel::OnTaskActionCallback None ldap acton");
        return;
    }

    int nRetCode = -1;
#if IF_FEATURE_34162
    if (pLdapAction->GetTaskActionErrorType() == NS_TA::TA_LDAP_FAILED_TO_BIND_SERVER)
    {
        nRetCode = pLdapAction->GetErrorCode();
    }
#endif

    //关键字变化，数据过时，无需操作目前数据
    if (m_stNowData.m_strNowKey.compare(pLdapAction->GetSearchKey()) != 0)
    {
        return;
    }

    m_ldapCs.Lock();
    ClearData();
    m_strSucKey.clear();

#if !IF_FEATURE_34162
    bool suc = false;
#endif
    // 如果搜索成功.
    if (pLdapAction->IsOperationSucceed())
    {
#if IF_FEATURE_34162
        nRetCode = 0;
#else
        suc = true;
#endif
        LdapItemList & refListData = pLdapAction->GetDataList();

        for (LdapItemListIter iter = refListData.begin(); iter != refListData.end(); iter++)
        {
            LdapContactData * pData = *iter;

            if (pData != NULL)
            {
                // LDAP不需要转换成T9
                AddContact(pData, kszLDAPGroupID, false, true, knInvalidRId, false);
            }
        }

        m_strSucKey = pLdapAction->GetSearchKey();
        m_stNowData.m_eNowStatus = SEARCH_SUCCEED;

        pLdapAction->SetClearDataFlag(false);
    }
    else
    {
        m_strSucKey.clear();
        m_stNowData.Reset();
        m_stNowData.m_eNowStatus = SEARCH_FAIL;
    }

    m_ldapCs.Unlock();

#if IF_FEATURE_34162
    LDAP_INFO("LdapManager::OnTaskActionCallback Notif result[%#02x]", nRetCode);

    NotifSearchResult(nRetCode, pLdapAction->GetSearchKey().c_str());

#else
    LDAP_INFO("LdapManager::OnTaskActionCallback Notif result[%d]", suc);

    NotifSearchResult(suc, pLdapAction->GetSearchKey().c_str());
#endif
}

yl::string LdapManager::GetName() const
{
    // 获取LDAP名.
    yl::string strName(configParser_GetConfigString(kszLDAPName));
    //k空不在返回默认名字，由UI实现支持翻译
    return strName;
}

#if IF_FEATURE_34162
void LdapManager::NotifSearchResult(int nRetCode, const yl::string & strKey)
{
    //同时发给自己
    //code 和ldap返回值一致，0表示成功，其他值表示错误码
    etl_NotifyAppEx(false, LDAP_SRARCH_RESULT, 0, nRetCode, strKey.size() + 1, strKey.c_str());
}
#else
void LdapManager::NotifSearchResult(bool isSuc, const yl::string & strKey)
{
    //同时发给自己
    etl_NotifyAppEx(false, LDAP_SRARCH_RESULT, isSuc, 0, strKey.size() + 1, strKey.c_str());
}
#endif

void LdapManager::SetLdapOption()
{
    // 获取Ldap各个配置.

    m_bNeedSort =
        configParser_GetConfigInt(kszLDAPSort) == 1;

    yl::string strNameFilter =
        configParser_GetConfigString(kszLDAPNameFilter);
    yl::string strNumberFilter =
        configParser_GetConfigString(kszLDAPNumberFilter);
    yl::string strServer =
        configParser_GetConfigString(kszLDAPhost);
    yl::string strBase =
        configParser_GetConfigString(kszLDAPbase);

    yl::string strUser =
        configParser_GetConfigString(kszLDAPuser);
    yl::string strPassword =
        configParser_GetConfigString(kszLDAPpswd);
    yl::string strNameAttr =
        configParser_GetConfigString(kszLDAPNameAttr);
    yl::string strNumberAttr =
        configParser_GetConfigString(kszLDAPNumbAttr);
    yl::string strDisplayName =
        configParser_GetConfigString(kszLDAPDisplayName);
    int nPort =
        configParser_GetConfigInt(kszLDAPport);
    int nVersion =
        configParser_GetConfigInt(kszLDAPversion);
    int nMaxHits =
        configParser_GetConfigInt(kszLDAPMaxHits);

    int nTLSMode = configParser_GetConfigInt(kszLDAPTLSMode);

    int nTimeOut = configParser_GetConfigInt(kszLDAPConnectTimeOut);

    yl::string strDefaultFilter =
        configParser_GetConfigString(kszLDAPDefaultFilter);

    int nEnableNameFilterFirst = configParser_GetConfigInt(kszLDAPEnableNameFilterFirst);

    int nSearchType = configParser_GetConfigInt(kszLdapSearchType);

    yl::string strSSLCipherList = configParser_GetConfigString(kszHttpsSSLCipherList);

    if (strDefaultFilter.empty())
    {
        strDefaultFilter = nEnableNameFilterFirst == 1 ? strNameFilter : strNumberFilter;
    }

    int iMd5Enable = 0;

    //加载到LADP库中
    //PORT
    if (yealink_ldap_set_option(YEALINK_LDAP_OPTIONS_PORT, &(nPort)))
    {
        LDAP_INFO("set YEALINK_LDAP_OPTIONS_PORT failed.");
    }
    //VERSION
    if (yealink_ldap_set_option(YEALINK_LDAP_OPTIONS_VERSION, &(nVersion)))
    {
        LDAP_INFO("set YEALINK_LDAP_OPTIONS_VERSION failed.");
    }
    //MAXHITS
    if (yealink_ldap_set_option(YEALINK_LDAP_OPTIONS_MAXHITS, &(nMaxHits)))
    {
        LDAP_INFO("set YEALINK_LDAP_OPTIONS_MAXHITS failed.");
    }
    //MD5_ENABLE
    if (yealink_ldap_set_option(YEALINK_LDAP_OPTIONS_MD5_ENABLE, &iMd5Enable))
    {
        LDAP_INFO("set YEALINK_LDAP_OPTIONS_MD5_ENABLE failed.");
    }
    //HOST
    if (yealink_ldap_set_option(YEALINK_LDAP_OPTIONS_HOST, (char *)(strServer.c_str())))
    {
        LDAP_INFO("set YEALINK_LDAP_OPTIONS_HOST failed.");
    }
    //USER
    if (yealink_ldap_set_option(YEALINK_LDAP_OPTIONS_USER, (char *)(strUser.c_str())))
    {
        LDAP_INFO("set YEALINK_LDAP_OPTIONS_USER failed.");
    }
    //PASSWORD
    if (yealink_ldap_set_option(YEALINK_LDAP_OPTIONS_PASSWORD, (char *)(strPassword.c_str())))
    {
        LDAP_INFO("set YEALINK_LDAP_OPTIONS_PASSWORD failed.");
    }
    //BASE
    if (yealink_ldap_set_option(YEALINK_LDAP_OPTIONS_BASE, (char *)(strBase.c_str())))
    {
        LDAP_INFO("set YEALINK_LDAP_OPTIONS_BASE failed.");
    }
    //DN
    if (yealink_ldap_set_option(YEALINK_LDAP_OPTIONS_DN, (char *)(strUser.c_str())))
    {
        LDAP_INFO("set YEALINK_LDAP_OPTIONS_DN failed.");
    }
    //HttpsSSLCipherList
    if (yealink_ldap_set_option(YEALINK_LDAP_OPTIONS_TLS_CIPHER_LIST, (char *)(strSSLCipherList.c_str())))
    {
        LDAP_INFO("set YEALINK_LDAP_OPTIONS_TLS_CIPHER_LIST failed.");
    }
    //NAME_FILTER
    if (yealink_ldap_set_option(YEALINK_LDAP_OPTIONS_NAME_FILTER, (char *)(strNameFilter.c_str())))
    {
        LDAP_INFO("set YEALINK_LDAP_OPTIONS_NAME_FILTER failed.");
    }
    //NUMBER_FILTER
    if (yealink_ldap_set_option(YEALINK_LDAP_OPTIONS_NUMBER_FILTER, (char *)(strNumberFilter.c_str())))
    {
        LDAP_INFO("set YEALINK_LDAP_OPTIONS_NUMBER_FILTER failed.");
    }

    //DEFAULT_FILTER
    if (yealink_ldap_set_option(YEALINK_LDAP_OPTIONS_DEFAULT_SEARCH_FILTER,
                                (char *)(strDefaultFilter.c_str())))
    {
        LDAP_INFO("set YEALINK_LDAP_OPTIONS_DEFAULT_SEARCH_FILTER failed.");
    }

    //NAME_ATTR
    if (yealink_ldap_set_option(YEALINK_LDAP_OPTIONS_NAME_ATTR, (char *)(strNameAttr.c_str())))
    {
        LDAP_INFO("set YEALINK_LDAP_OPTIONS_NAME_ATTR failed.");
    }
    //NUMBER_ATTR
    if (yealink_ldap_set_option(YEALINK_LDAP_OPTIONS_NUMBER_ATTR, (char *)(strNumberAttr.c_str())))
    {
        LDAP_INFO("set YEALINK_LDAP_OPTIONS_NUMBER_ATTR failed.");
    }
    //DISPALY_NAME
    if (yealink_ldap_set_option(YEALINK_LDAP_OPTIONS_DISPALY_NAME, (char *)(strDisplayName.c_str())))
    {
        LDAP_INFO("set YEALINK_LDAP_OPTIONS_DISPALY_NAME failed.");
    }

    // LDAP 使用模式
    if (yealink_ldap_set_option(YEALINK_LDAP_OPTIONS_TLS_MODE, &nTLSMode))
    {
        LDAP_WARN("set YEALINK_LDAP_OPTIONS_TLS_MODE ERR!");
    }

    char bufCert[128] = {0};
    strcpy(bufCert, CONFIG_PATH"/certs/ca");
    // ca 存放目录s
    if (yealink_ldap_set_option(YEALINK_LDAP_OPTIONS_TLS_CA_DIR, (void *)bufCert))
    {
        LDAP_WARN("set YEALINK_LDAP_OPTIONS_TLS_CA_DIR ERR!");
    }

    // 证书路径
    if (yealink_ldap_set_option(YEALINK_LDAP_OPTIONS_TLS_CERT_FILE, (void *)szDeviceCertFile))
    {
        LDAP_WARN("set YEALINK_LDAP_OPTIONS_TLS_CERT_FILE ERR!");
    }

    unsigned int use_ipv6 = configParser_GetConfigInt(kszNetworkIPAddressMode);
    if (yealink_ldap_set_option(YEALINK_LDAP_OPTIONS_IPADDR_MODE, &use_ipv6))
    {
        LDAP_WARN("set YEALINK_LDAP_OPTIONS_IPADDR_MODE ERR!");
    }

    // 超时时间
    if (yealink_ldap_set_option(YEALINK_LDAP_OPTIONS_NETWORK_TIMEOUT, &nTimeOut))
    {
        LDAP_WARN("set YEALINK_LDAP_OPTIONS_NETWORK_TIMEOUT ERR!");
    }

    //搜索方式
    if (yealink_ldap_set_option(YEALINK_LDAP_OPTIONS_SEARCH_TYPE, &nSearchType))
    {
        LDAP_WARN("set YEALINK_LDAP_OPTIONS_SEARCH_TYPE ERR!");
    }

}

#if IF_FEATURE_34162
// 重新设置LDAP配置
void LdapManager::ReSetLdapOption()
{
    SetLdapOption();
}
#endif

void LdapManager::InitGroupData()
{
    //必须先初始化基类初始化函数
    CBaseContactManger::MyInit();

    DirGroupRecord * pGroup = new DirGroupRecord;
    if (pGroup != NULL)
    {
        InsertMap(pGroup, kszLDAPGroupID);
    }

}
//获取超级搜索列表数据
bool LdapManager::GetContactSuperSearchList(YLVector<SuperDirData> & output,
        const yl::string & strKey)
{
    LDAP_INFO("LdapManager::GetContactIdList strKey[%s]", strKey.c_str());
    m_ldapCs.Lock();

    if (strKey.compare(m_strSucKey) != 0)
    {
        LDAP_INFO("LdapManager::GetContactIdList SucKey = %s", m_strSucKey.c_str());
        m_ldapCs.Unlock();
        return false;
    }

    DirGroupRecord * pGroup = GetMenuDataById(kszLDAPGroupID);
    if (pGroup == NULL)
    {
        LDAP_ERR("Group is Null Group %d", kszLDAPGroupID);
        m_ldapCs.Unlock();
        return false;
    }

    if (pGroup->m_vChildrenId.size() <= 0)
    {
        LDAP_INFO("LdapManager::GetContactIdList Search Result Is Null");
        m_ldapCs.Unlock();
        return false;
    }

    SuperDirData itemData;

    for (int i = 0; i < pGroup->m_vChildrenId.size(); i++)
    {
        BaseDirDataRecord * pBase = GetDataById(pGroup->m_vChildrenId[i]);
        BaseContactRecord * pContact = Base2BaseContact(pBase);

        if (pBase == NULL || pContact == NULL)
        {
            continue;
        }

        itemData.m_nID = pGroup->m_vChildrenId[i];
        int j = 0;

        for (ListNumber::iterator it = pContact->m_listNumber.begin(); it != pContact->m_listNumber.end();
                it++)
        {
            stNumber & stNum = *it;

            if (!stNum.strValue.empty())
            {
                itemData.m_nNumberIdex = j;
                output.push_back(itemData);
            }

            j++;
        }
    }

    m_ldapCs.Unlock();
    return true;
}
