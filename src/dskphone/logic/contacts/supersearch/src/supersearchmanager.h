#ifndef __SUPERSEARHC_MANAGER_H__
#define __SUPERSEARHC_MANAGER_H__

#include "taskaction/taskactioncallbackbase.h"
#include "supersearchdatadefine.h"
#include "ylhashmap.h"

class CSearchAction;

typedef YLList<ResultData> ResultList;

typedef YLHashMap<yl::string, int> FilterMap;

class SuperSearchManager : public NS_TA::CTaskActionCallbackBase
{
public:

    // 创建单键.
    static SuperSearchManager & GetInstance();

    bool ReqSearch(const yl::string & strKey);

    bool GetContactListData(SuperSearchItemListData & itemListData, const yl::string & strKey);

    //模块初始化函数
    void Init();

    virtual void OnTaskActionCallback(NS_TA::CTaskAction * pTaskAction);

    void ProcessLdapMessage(msgObject & refObj);

    void LoadConfigFromFile();

    // 处理计时器到时消息.
    BOOL MyDealTimeoutMessage(msgObject & refObj);

#if IF_FEATURE_GBBOOK
    bool IsClearSearchKey();

    void SetIsClearSearchKey(bool bClearSearchKey);
#endif

//涉及异步
public:

    void SetSearchType(SearchMethod eSearchType);

    bool SearchFromResult(const yl::string & strKeye);

    bool MyReSearch(const yl::string & strKey);

    void SetLdapResult(const yl::string & strLdapKey, const yl::string & strKey);

    bool SearchRecentCallOut();

//涉及异步
protected:

    void ClearSearchData();

    void MyFilterResult(ResultData & refResult);

    // 增加一个过滤.
    // | refData | 是要增加的过滤.
    // 增加成功则返回true, 否则返回false.
    void MyAddFilter(DirectorySearchData & refData);
    // 判断过滤是否存在.
    // | refData | 是要查询的过滤.
    // 如果过滤存在则返回true, 否则返回false.
    bool MyIsFilterExist(DirectorySearchData & refData);

protected:

    bool GetRecentCallList(SuperSearchItemListData & refData);

    DIR_TYPE GetTypeByString(const yl::string & strType);

    // 根据类型获取结果.
    // | strType | 是搜索类型.
    // 如果获取成功则返回结果指针, 否则返回NULL.
    ResultData * MyFindResultByType(DIR_TYPE eType);

    void NotifSearchSucResult(const yl::string & strKey);

    void MyCheckResult(const yl::string & strKey);

    //获取目前结果个数
    int GetResultCount();

    SuperSearchManager();

    ~SuperSearchManager();

private:

    CSearchAction * m_pSuperAction; // 超级搜索的Action.

    ResultList m_listResult;  // 结果列表.

    //YLVector<DirectorySearchData> m_vecLdapData;

    SearchMethod m_eSearchType;

    FilterMap m_mapFilter;  // 过滤map.

    //记录最近呼出记录
    SuperSearchItemListData m_listData;

    mkThreadTimer m_objTimer;  // 计时器.

#if IF_FEATURE_GBBOOK
    bool m_bClearSearchkey;
#endif

    yl::string m_strKey;

    chCriticalSection m_superCs;
};

#define g_refSuperSearchManager SuperSearchManager::GetInstance()

#endif

