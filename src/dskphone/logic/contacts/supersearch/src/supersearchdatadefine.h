#ifndef __SUOERSEARCH_DATA_DEFINE_H__
#define __SUOERSEARCH_DATA_DEFINE_H__

#include "contacts/directory/include/commondata.h"


// 搜索的id名定义.
// 本地联系人搜索.
#define  kszSearchTypeLocal "local_directory_search"
// 远程联系人搜索.
#define kszSearchTypeRemote "remote_directory_search"
// LDAP联系人搜索.
#define kszSearchTypeLdap "ldap_search"
// 历史记录搜索.
#define kszSearchTypeCalllog "calllog_search"
//BroadSoft联系人搜索
#define kszSearchTypeNetwork "Network_directory_search"

#define kszSearchTypeBst "BroadSoft_directory_search"

#if FEATURE_BROADTOUCH_UC
#define kszSearchTypeUC "BroadSoft_UC_search"
#endif

#if IF_BUG_PLCMDIR
#define kszSearchTypePlcm "plcm_directory_search"
#endif

#if IF_FEATURE_GBBOOK
#define kszSearchTypeGB "genband_directory_search"
#endif


#if IF_FEATURE_METASWITCH_CALLLOG
//Metaswitch 历史记录搜索
#define kszSearchTypeMTSWCalllog "MetaSwitch_calllog_search"
#endif
#define kszSearchTypeCloud "Cloud_directory_search"

#if IF_FEATURE_METASWITCH_DIRECTORY
//metaswitch联系人搜索
#define kszSearchTypeMetaSwitch "MetaSwitch_directory_search"
#endif

#if IF_BUG_39755
#define kszSearchTypeBlueContact "mobile_directory_search"
#endif

// 搜索状态.
enum SearchStatus
{
    SEARCH_INVALID = -1,
    SEARCH_SUCC = 0,  // 搜索完毕.
    SEARCH_READY = 2,  // 准备搜索.
    SEARCHING = 3,  // 搜索中.
    SEARCH_FAILED = 4,  // 搜索失败.
};



typedef YLVector<SuperDirData> ItemList;

// 搜索结果结构体.
struct ResultData
{
    ItemList m_listItem;  // 搜索结果链表.
    DIR_TYPE m_eType;  // 搜索类型id.
    SearchStatus m_eStatus;  // 搜索结果状态.

    int m_nPriority;

    // 比较大小.
    static bool Compare(const ResultData & lhs, const ResultData & rhs)
    {
        return lhs.m_nPriority < rhs.m_nPriority;
    }

    ResultData()
    {
        Reset();
    }

    void Reset()
    {
        m_nPriority = -1;
        m_listItem.clear();
        m_eType = BASE_DIR;
        m_eStatus = SEARCH_INVALID;
    }
};

enum SearchExeType
{
    EXE_NONE,
    EXE_CONTACT_SEARCH,
    EXE_LDAP_DATA_GET,
};


#endif

