#ifndef __DATA_DEFINE_H__
#define __DATA_DEFINE_H__

#include "ylstring.h"
#include "yllist.h"
#include "ylvector.h"
#include "contacts/directory/include/commondata.h"

#define LA_SEARCH_STR_LEN 20

#define ERROR_BUF_MAX_LEN 128

// LDAP元素数据
struct LdapContactData: public BaseContactRecord
{
#if IF_BUG_24324
    yl::string m_strCompany;
    yl::string m_strDepartment;
#endif

    LdapContactData()
    {
        m_eType = DIR_TYPE_LDAP;
        m_nParentId = kszAllContactId;
    }
};

typedef YLList<LdapContactData *> LdapItemList;

typedef LdapItemList::iterator LdapItemListIter;
typedef LdapItemList::const_iterator LdapItemListCIter;


typedef YLVector<LdapContactData *> vLdapItem;


struct LdapListData
{
    YLList<LdapContactData> m_listData;  // BroadSoft contact list.
    int m_nIndexFrom;  // Retrieve from which index.
    int m_nCountToGet;  // Count to retrieve.
    int m_nTotalCount;  // Total item count.
#if IF_BUG_31829
    int m_nTotalCountByName;   // 联系人名称总数量，不包含重复项
#endif

    LdapListData()
    {
        Reset();
    }

    void Reset()
    {
        m_listData.clear();
        m_nIndexFrom = 0;
        m_nCountToGet = 0;
        m_nTotalCount = 0;
#if IF_BUG_31829
        m_nTotalCountByName = 0;
#endif
    }
};



#endif
