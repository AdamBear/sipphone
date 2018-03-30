#ifdef USE_LDAP_IN_THREADTASKASST___

#ifndef TASKACTION_LDAPACTION_H_
#define TASKACTION_LDAPACTION_H_

#include "taskaction/taskaction.h"
#include <ylstl/yllist.h>

#ifndef IF_CUT_LDAP
using  namespace NS_TA;

// LDAP链表, 保存各个LDAP数据.
typedef YLList<LDAPITEMINFO> LdapItemList;


typedef LdapItemList::iterator LdapItemListIter;
typedef LdapItemList::const_iterator LdapItemListCIter;

// CLdapAction类, 用于读取LDAP.
class CLdapAction : public CTaskAction
{
public:
    CLdapAction();
    virtual ~CLdapAction();

    // 重写
    virtual bool Exec();

    // 获取绑定结果: 是否已经绑定
    inline bool GetBindResult() const
    {
        return m_hasBound;
    }

    // 设置搜索条件.
    inline void SetNumber(const yl::string & strNumber)
    {
        m_strNumber = strNumber;
    }
    // 获取搜索条件.
    inline yl::string GetNumber() const
    {
        return m_strNumber;
    }

    // 设置是否排序.
    inline void SetSort(bool bSort)
    {
        m_isSort = bSort;
    }

    // 获取查询结果链表.
    inline LdapItemList & GetDataList()
    {
        return m_listLdapItem;
    }

    // 设置是否仅查询一个.
    inline void SetOnlyOne(bool bOnlyOne)
    {
        m_isOnlyOne = bOnlyOne;
    }

    // 设置起始下标.
    inline void SetFromIndex(int nIndexFrom);
    // 获取起始下标.
    inline int GetFromIndex() const;

    // 设置获取多少个.
    inline void SetCountToGet(int nCountToGet);
    // 获取设置的获取数量.
    inline int GetCountToGet() const;

    // 设置结果总数.
    // | nCount | 是要设置的结果总数.
    inline void SetTotalCount(int nCount);
    // 获取结果总数.
    // 返回结果总数
    inline int GetTotalCount() const;

private:
    // 初始化, 即绑定LDAP服务器..
    bool Init();

    // 结束, 即取消LDAP服务器的绑定, 并做一些清除操作.
    bool Final();

    // 读取LDAP.
    bool Read();

    // 写入LDAP.
    bool Write();

    // 绑定LDAP服务器.
    // 如果绑定成功则返回true, 否则返回false.
    bool MyBind();
    // 取消LDAP服务器的绑定.
    // 如果取消成功则返回true, 否则返回false.
    bool MyUnBind();

    void * m_pLdapId; // LDAP绑定时, 服务器分配的id.
    LdapItemList m_listLdapItem;  // LDAP数据结果链表.
    yl::string m_strNumber;  // 请求查询的号码, 用于服务器查找.
    bool m_isSort;  // 是否排序.
    bool m_hasBound;  // 是否已经绑定服务器.
    bool m_isOnlyOne;  // 是否只有一个.
    int m_nMessageId;  // message id.
    int m_nIndexFrom;  // 起始下标.
    int m_nCountToGet;  // 获取多少个.
    int m_nTotalCount;  // 总共获取到的数量.
};  // class CLdapAction

inline void CLdapAction::SetFromIndex(int nIndexFrom)
{
    if (m_nIndexFrom != nIndexFrom)
    {
        m_nIndexFrom = nIndexFrom;
    }
}

inline int CLdapAction::GetFromIndex() const
{
    return m_nIndexFrom;
}

inline void CLdapAction::SetCountToGet(int nCountToGet)
{
    if (m_nCountToGet != nCountToGet)
    {
        m_nCountToGet = nCountToGet;
    }
}

inline int CLdapAction::GetCountToGet() const
{
    return m_nCountToGet;
}

inline void CLdapAction::SetTotalCount(int nCount)
{
    if (m_nTotalCount != nCount)
    {
        m_nTotalCount = nCount;
    }
}

inline int CLdapAction::GetTotalCount() const
{
    return m_nTotalCount;
}



#endif   // IF_CUT_LDAP
#endif  // TASKACTION_LDAPACTION_H_




#endif //USE_LDAP_IN_THREADTASKASST___

