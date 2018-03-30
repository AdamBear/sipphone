#include "contacts/directory/include/moddirinterface.h"
#include "supersearchmanager.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "searchaction.h"
#include "contacts/directory/include/directorymessagedefine.h"
#include "taskaction/modtaskaction.h"

#include "contacts/ldaplogic/include/modldap.h"
#include "commonunits/modcommonunits.h"
#include "configiddefine.h"
#include "configparser/modconfigparser.h"
#include "devicelib/phonedevice.h"
#define kszConfigureFile "super_search.xml"

// 以下为Xml结点名定义.
// 根节点.
#define kszNodeRoot "root_super_search"
// 各个配置结点.
#define kszNodeConfig "configuration"
// 各个配置结点.
#define kszDev "dev"
// 各个配置结点.
#define kszNodeItem "item"
// id名属性.
#define kszAttrIdName "id_name"
// 显示名属性.
#define kszAttrDisplay "display_name"
// 优先级属性.
#define kszAttrPriority "priority"
// 开启属性.
#define kszAttrEnable "enable"

#define kszSeparation "|"

// 计时器超时时间.
#define  knTimeoutInterval  300

LRESULT OnSuperSearchMessage(msgObject & refObj)
{
    LRESULT retVal = FALSE;
    switch (refObj.message)
    {
    case CONFIG_MSG_BCAST_CHANGED:
        // 超级搜索的配置文件改变了.
        if (refObj.wParam == ST_SUPERSEARCH)
        {
            SUPERSEARCH_INFO("SuperSearch config change!");
            // 初始化全局的
            g_refSuperSearchManager.LoadConfigFromFile();
            retVal = TRUE;
        }
        break;
    case LDAP_SRARCH_RESULT:
        {
            g_refSuperSearchManager.ProcessLdapMessage(refObj);
            retVal = TRUE;
        }
        break;
    default:
        retVal = FALSE;
        break;
    }
    return retVal;
}

SuperSearchManager & SuperSearchManager::GetInstance()
{
    static SuperSearchManager instance;
    return instance;
}

SuperSearchManager::SuperSearchManager()
{
    m_pSuperAction = NULL;
    m_eSearchType = SEARCH_BY_AUTO;
    m_strKey.clear();
}

SuperSearchManager::~SuperSearchManager()
{
    if (m_pSuperAction != NULL)
    {
        delete m_pSuperAction;
    }

}

void SuperSearchManager::Init()
{
    TaskAction_CreateSpecialThread(NS_TA::TA_REQUEST_SUPER_SEARCH,
                                   TA_THREAD_LOCAL_SEARCH, NULL);

    if (m_pSuperAction == NULL)
    {
        m_pSuperAction = new CSearchAction();
    }

    if (m_pSuperAction != NULL)
    {
        m_pSuperAction->SetAutoRelease(false);
        m_pSuperAction->SetSync(false);  // 设置异步.
        m_pSuperAction->SetCallback(this);  // 设置回调类.
    }
    LoadConfigFromFile();

#if IF_FEATURE_GBBOOK
    m_bClearSearchkey = false;
#endif

    // 注册消息.
    etl_RegisterMsgHandle(CONFIG_MSG_BCAST_CHANGED,
                          CONFIG_MSG_BCAST_CHANGED,
                          OnSuperSearchMessage);

    etl_RegisterMsgHandle(LDAP_SRARCH_RESULT,
                          LDAP_SRARCH_RESULT,
                          OnSuperSearchMessage);
}

bool SuperSearchManager::ReqSearch(const yl::string & strKey)
{
    SUPERSEARCH_INFO("ReqSearch(strKey = %s)", strKey.c_str());
    bool suc = false;

    // 杀掉计时器.
    if (m_objTimer.IsTimerRuning())
    {
        m_objTimer.KillTimer();
    }

    if (strKey.empty()
            && configParser_GetConfigInt(kszFeaturesRecentCallEnable) == 0)
    {
        return false;
    }

    // 使用超级搜索进行搜索.
    if (m_pSuperAction != NULL)
    {
        m_pSuperAction->SetNextSearchKey(strKey);  // 设置搜索关键字.
        // 开始搜索.
        suc = TaskAction_ExecAction(m_pSuperAction);
    }

    return suc;
}

bool SuperSearchManager::GetContactListData(SuperSearchItemListData & itemListData,
        const yl::string & strKey)
{
    m_superCs.Lock();

    if (strKey.empty())
    {
        bool suc = false;
        if (configParser_GetConfigInt(kszFeaturesRecentCallEnable) != 0)
        {
            suc = GetRecentCallList(itemListData);
        }

        m_superCs.Unlock();
        return suc;
    }

    itemListData.m_nTotalCount = GetResultCount();

    int nGetCount = itemListData.m_nCountToGet;

    int nIndexFrom = itemListData.m_nIndexFrom;

    if (itemListData.m_nIndexFrom == 0 && itemListData.m_nCountToGet == 0)
    {
        nGetCount = itemListData.m_nTotalCount;
    }

    bool bFoundFirst = false;

    //SUPERSEARCH_INFO("SuperSearchManager::GetContactListData ldapsize[%d]nGetCount[%d]",m_vecLdapData.size(),nGetCount);

    DirectorySearchData IterData;
    for (ResultList::iterator iter = m_listResult.begin(); iter != m_listResult.end(); iter++)
    {
        if (nGetCount == 0)
        {
            break;
        }

        ResultData & refData = *iter;

        int nListSize = refData.m_listItem.size();

        if (!bFoundFirst && nIndexFrom >= nListSize)
        {
            nIndexFrom -= nListSize;
            continue;
        }

        for (int i = bFoundFirst ? 0 : nIndexFrom; i < nListSize;)
        {
            IterData.Reset();

            if (!Dir_GetSuperDataById(refData.m_listItem[i], IterData, refData.m_eType))
            {

                itemListData.m_nTotalCount--;

                refData.m_listItem.removeAt(i);

                nListSize--;

                continue;
            }

            itemListData.m_listContact.push_back(IterData);

            nIndexFrom--;
            if (--nGetCount == 0)
            {
                break;
            }
            i++;
        }

        bFoundFirst = true;
    }

    m_superCs.Unlock();

    return true;
}

bool SuperSearchManager::GetRecentCallList(SuperSearchItemListData & refData)
{
    refData.m_nTotalCount = m_listData.m_listContact.size();

    int nGetCount = refData.m_nCountToGet;

    if (refData.m_nIndexFrom == 0 && refData.m_nCountToGet == 0)
    {
        nGetCount = refData.m_nTotalCount;
    }
    int i = 0;

    for (YLList<DirectorySearchData>::iterator iter = m_listData.m_listContact.begin();
            iter != m_listData.m_listContact.end(); iter++)
    {
        if (i < refData.m_nIndexFrom)
        {
            i++;
            continue;
        }

        refData.m_listContact.push_back(*iter);

        if (--nGetCount == 0)
        {
            break;
        }
    }

    return true;
}

bool SuperSearchManager::SearchFromResult(const yl::string & strKey)
{
    SUPERSEARCH_INFO("SuperSearchManager::SearchFromResult");
    StringMatchType eSearchMatchType = Dir_GetSearchMatchType();
    m_superCs.Lock();
    for (ResultList::iterator iter = m_listResult.begin(); iter != m_listResult.end(); iter++)
    {
        ResultData & refData = *iter;

        if (refData.m_eType == LDAP_DIR)
        {
            refData.m_eStatus = SEARCH_READY;
            LDAP_ReqSearch(strKey);
            continue;
        }

        if (refData.m_eStatus != SEARCH_SUCC)
        {
            continue;
        }

        for (int i = refData.m_listItem.size() - 1; i > -1; i--)
        {
            if (!Dir_IsSearchHit(refData.m_listItem[i].m_nID, strKey, m_eSearchType,
                                 refData.m_eType, eSearchMatchType))
            {
                refData.m_listItem.removeAt(i);
            }
        }
    }

    m_superCs.Unlock();

    return true;
}

bool SuperSearchManager::MyReSearch(const yl::string & strKey)
{
    SUPERSEARCH_INFO("SuperSearchManager::MyReSearch strKey[%s]", strKey.c_str());

    bool isSucceed = false;
    StringMatchType eSearchMatchType = Dir_GetSearchMatchType();

    m_superCs.Lock();
    ClearSearchData();
    for (ResultList::iterator iter = m_listResult.begin();
            iter != m_listResult.end();
            iter++)
    {
        ResultData & refResult = *iter;
        SUPERSEARCH_INFO("Result iter: type=%d", refResult.m_eType);
        if (refResult.m_eType == LDAP_DIR)
        {
            isSucceed |= LDAP_ReqSearch(strKey);
        }
        else
        {
            if (refResult.m_eType == BLUETOOTH_DIR && !Dir_IsDirConfiged(BLUETOOTH_DIR))
            {
                continue;
            }
            isSucceed |= Dir_SuperSearch(strKey, refResult.m_listItem, refResult.m_eType,
                                         eSearchMatchType);
            MyFilterResult(refResult);
        }
    }

    m_mapFilter.clear();

    m_superCs.Unlock();
    return isSucceed;
}

void SuperSearchManager::MyFilterResult(ResultData & refResult)
{
    for (int i = refResult.m_listItem.size() - 1; i >= 0; i--)
    {
        SuperDirData & refItem = refResult.m_listItem[i];

        DirectorySearchData stData;

        // 这里只需要获取Name和Number，不需要获取Photo
        if (!Dir_GetSuperDataById(refItem, stData, refResult.m_eType, false))
        {
            continue;
        }

        if (!MyIsFilterExist(stData))
        {
            MyAddFilter(stData);
        }
        else
        {
            refResult.m_listItem.removeAt(i);
        }
    }  // for.

    refResult.m_eStatus = SEARCH_SUCC;
}


DIR_TYPE SuperSearchManager::GetTypeByString(const yl::string & strType)
{
    // 根据搜索类型, 决定搜索.
    if (strType == kszSearchTypeLocal)
    {
        return LOCAL_DIR;
    }
    else if (strType == kszSearchTypeRemote)
    {
        return REMOTE_DIR;
    }
    else if (strType == kszSearchTypeLdap)
    {
        return LDAP_DIR;
    }
    else if (strType == kszSearchTypeCalllog)
    {
        return LOCAL_LOG;
    }

    else if (strType == kszSearchTypeNetwork || strType == kszSearchTypeBst)
    {
        return BSFT_DIR;
    }

#if FEATURE_BROADTOUCH_UC
    else if (strType == kszSearchTypeUC)
    {
        return UC_DIR;
    }
#endif

#if IF_BUG_PLCMDIR
    else if (strType == kszSearchTypePlcm)
    {
        return PLCM_DIR;
    }
#endif
    else if (strType == kszSearchTypeCloud)
    {
        return CLOUD_DIR;
    }
#if IF_FEATURE_GBBOOK
    else if (strType == kszSearchTypeGB)
    {
        return GB_DIR;
    }
#endif
#if IF_FEATURE_METASWITCH_CALLLOG
    else if (strType == kszSearchTypeMTSWCalllog)
    {
        return MTSW_LOG;
    }
#endif
#if IF_FEATURE_METASWITCH_DIRECTORY
    else if (strType == kszSearchTypeMetaSwitch)
    {
        return MTSW_DIR;
    }
#endif
#if IF_BUG_39755
    else if (strType == kszSearchTypeBlueContact)
    {
        return BLUETOOTH_DIR;
    }
#endif

    return BASE_DIR;
}

void SuperSearchManager::LoadConfigFromFile()
{
    m_superCs.Lock();
    m_listResult.clear();

    if (m_pSuperAction != NULL)
    {
        m_pSuperAction->ResetLastKey();
    }

    // 加载xml配置文件.
    // 获取xml全路径.
    yl::string strFullPath(CONFIG_DATA_PATH);
    if (strFullPath.substr(strFullPath.length() - 1) != "/")
    {
        strFullPath.append("/");
    }
    strFullPath.append(kszConfigureFile);

    // Parse the xml file.
    xml_document doc;
    if (!doc.load_file(strFullPath.c_str()))
    {
        m_superCs.Unlock();
        return;
    }
    // Find root node.
    xml_node nodeRoot = doc.child(kszNodeRoot);
    if (!nodeRoot)
    {
        m_superCs.Unlock();
        return;
    }

    // 读取各个配置结点.
    ResultData stData;

#if !IF_BUG_39755
    //插入蓝牙搜索项
    stData.m_eType = BLUETOOTH_DIR;
    stData.m_nPriority = -1;//最高优先级
    m_listResult.push_back(stData);
#endif

    xml_node nodeChild = nodeRoot.child(kszNodeItem);

    for (; !nodeChild.empty(); nodeChild = nodeChild.next_sibling())
    {
        stData.Reset();

        bool bEnable = nodeChild.attribute(kszAttrEnable).as_int();

        if (!bEnable)
        {
            continue;
        }

        yl::string strType = nodeChild.attribute(kszAttrIdName).value();

        stData.m_eType = GetTypeByString(strType);

        if (stData.m_eType == BASE_DIR)
        {
            continue;
        }

        stData.m_nPriority = nodeChild.attribute(kszAttrPriority).as_int();

        m_listResult.push_back(stData);
    }

    // 对配置文件进行排序.
    Sort(m_listResult, ResultData::Compare);

    m_superCs.Unlock();
}

ResultData * SuperSearchManager::MyFindResultByType(DIR_TYPE eType)
{
    for (ResultList::iterator iter = m_listResult.begin();
            iter != m_listResult.end();
            iter++)
    {
        ResultData & refResult = *iter;

        if (refResult.m_eType == eType)
        {
            return &refResult;
        }
    }  // for.

    return NULL;
}

void SuperSearchManager::ClearSearchData()
{
    for (ResultList::iterator iter = m_listResult.begin();
            iter != m_listResult.end();
            iter++)
    {
        ResultData & refResult = *iter;

        refResult.m_listItem.clear();

        refResult.m_eStatus = SEARCH_READY;
    }
}

void SuperSearchManager::SetSearchType(SearchMethod eSearchType)
{
    m_eSearchType = eSearchType;
}

void SuperSearchManager::MyAddFilter(DirectorySearchData & refData)
{
    yl::string strkey = refData.m_strName;
    strkey.append(kszSeparation).append(refData.m_strNumber);
    // 如果不存在过滤, 则增加.
    m_mapFilter.put(strkey, 1);
}

bool SuperSearchManager::MyIsFilterExist(DirectorySearchData & refData)
{
    yl::string strkey = refData.m_strName;
    strkey.append(kszSeparation).append(refData.m_strNumber);
    // 查找, 如果存在则返回true, 否则返回false.
    return m_mapFilter.find(strkey) != m_mapFilter.end();
}

bool SuperSearchManager::SearchRecentCallOut()
{
    m_superCs.Lock();
    m_listData.Reset();

    //只获取10条
    m_listData.m_nCountToGet = 10;

    bool suc = LocalLog_SearchCallLogRecentOutCall(&m_listData);

    m_superCs.Unlock();

    return suc;
}

void SuperSearchManager::OnTaskActionCallback(NS_TA::CTaskAction * pTaskAction)
{
    SUPERSEARCH_INFO("SuperSearchManager  OnTaskActionCallback");
    if (pTaskAction == NULL || pTaskAction->GetReqType() != NS_TA::TA_REQUEST_SUPER_SEARCH)
    {
        return;
    }

#if IF_FEATURE_GBBOOK
    m_bClearSearchkey = false;
#endif

    CSearchAction * pSearchAction = static_cast<CSearchAction *>(pTaskAction);
    if (pSearchAction == NULL)
    {
        SUPERSEARCH_INFO("SuperSearchManager::OnTaskActionCallback None SearchAction");
        return;
    }

    if (pSearchAction->IsProcessLdapData())
    {
        SUPERSEARCH_INFO("IsProcessLdapData");
        pSearchAction->SetLdapDataProcess(false);

        ResultData * pResult = MyFindResultByType(LDAP_DIR);

        if (pResult != NULL)
        {
            pResult->m_eStatus = SEARCH_SUCC;
        }

        //ldap需要处理数据故一定在其后，故直接发送通知消息
        NotifSearchSucResult(pSearchAction->GetSearchKey());
    }
    else if (pSearchAction->IsOperationSucceed())
    {
        SUPERSEARCH_INFO("OperationSucceed");
        if (pSearchAction->GetSearchKey().empty())
        {
            // http://10.2.1.199/Bug.php?BugID=85238 发送的lParam参数要为1，否则UI当作失败处理，不显示搜索结果
            NotifSearchSucResult(pSearchAction->GetSearchKey());
        }
        else
        {
            MyCheckResult(pSearchAction->GetSearchKey());
        }
    }
    else
    {
        SUPERSEARCH_INFO("SuperSearch fail!");
        //noting to do
    }
}

void SuperSearchManager::ProcessLdapMessage(msgObject & refObj)
{
    SUPERSEARCH_INFO("SuperSearchManager::ProcessLdapMessage,lparm[%d]", refObj.lParam);
    yl::string strKey = (char *)refObj.GetExtraData();

    if (m_pSuperAction == NULL)
    {
        return;
    }

    ResultData * pResult = MyFindResultByType(LDAP_DIR);

    if (pResult == NULL)
    {
        return;
    }

#if IF_FEATURE_34162
    if (refObj.lParam == 0 && m_pSuperAction->GetSearchKey() == strKey)
#else
    if (refObj.lParam != 0 && m_pSuperAction->GetSearchKey() == strKey)
#endif
    {
        m_pSuperAction->SetLdapDataProcess(true, strKey);

        APP_INFO("SuperSearchManager::ProcessLdapMessage get ldap data");

        TaskAction_ExecAction(m_pSuperAction);
    }
    else
    {
        //pResult->m_eStatus = SEARCH_FAILED;
    }
}

void SuperSearchManager::SetLdapResult(const yl::string & strLdapKey, const yl::string & strKey)
{
    SUPERSEARCH_INFO("SuperSearchManager::SetLdapResult");

    ResultData * pResult = MyFindResultByType(LDAP_DIR);
    if (pResult == NULL)
    {
        return;
    }
    m_superCs.Lock();

    // clear ldap before data
    pResult->m_listItem.clear();
    // 获取 LDAP 数据
    LDAP_GetContactSuperSearchList(pResult->m_listItem, strLdapKey);

    m_superCs.Unlock();
}

void SuperSearchManager::NotifSearchSucResult(const yl::string & strKey)
{
    // 杀掉计时器.
    if (m_objTimer.IsTimerRuning())
    {
        m_objTimer.KillTimer();
    }

    SUPERSEARCH_INFO("SuperSearchManager::NotifSearchSucResult key[%s]", strKey.c_str());

    etl_NotifyAppEx(false, SUPER_SEARCH_RESULT, 0, 1, strKey.size() + 1, strKey.c_str());
}

void SuperSearchManager::MyCheckResult(const yl::string & strKey)
{
    for (ResultList::iterator iter = m_listResult.begin();
            iter != m_listResult.end();
            iter++)
    {
        ResultData & refResult = *iter;
        if (refResult.m_eStatus == SEARCH_READY)
        {
            if (!m_objTimer.IsTimerRuning())
            {
                //如部分没完成则启动超时计时器
                m_objTimer.SetTimer(knTimeoutInterval, NULL,
                                    MK_MSG_ENTRY(this, SuperSearchManager::MyDealTimeoutMessage));

                APP_INFO("SuperSearchManager::MyCheckResult type[%d]->set timer", refResult.m_eType);
            }

            m_strKey = strKey;
            return;
        }
    }

    //如全部搜索完毕，则直接发送成功消息
    NotifSearchSucResult(strKey);
}

BOOL SuperSearchManager::MyDealTimeoutMessage(msgObject & refObj)
{
    SUPERSEARCH_INFO("SuperSearchManager::MyDealTimeoutMessage");
    // 杀掉计时器.
    if (m_objTimer.IsTimerRuning())
    {
        m_objTimer.KillTimer();
    }

    NotifSearchSucResult(m_strKey);

    return TRUE;
}

int SuperSearchManager::GetResultCount()
{
    int nCount = 0;
    for (ResultList::iterator iter = m_listResult.begin();
            iter != m_listResult.end();
            iter++)
    {
        ResultData & refResult = *iter;

        int nListCount = refResult.m_listItem.size();

        nCount += nListCount;
    }

    return nCount;
}

#if IF_FEATURE_GBBOOK
bool SuperSearchManager::IsClearSearchKey()
{
    bool bIsClearSearch;
    m_superCs.Lock();
    bIsClearSearch = m_bClearSearchkey;
    m_superCs.Unlock();
    return bIsClearSearch;
}

void SuperSearchManager::SetIsClearSearchKey(bool bClearSearchKey)
{
    m_superCs.Lock();
    m_bClearSearchkey = bClearSearchKey;
    m_superCs.Unlock();
}
#endif

