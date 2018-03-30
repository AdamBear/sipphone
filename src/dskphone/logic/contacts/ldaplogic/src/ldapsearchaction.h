#ifndef TASKACTION_LDAPACTION_H_
#define TASKACTION_LDAPACTION_H_

#include "taskaction/taskaction.h"
#include "ldapdatadefine.h"

// LDAP链表, 保存各个LDAP数据.

// CLdapAction类, 用于读取LDAP.
class CLdapSearchAction : public NS_TA::CTaskAction
{
public:
    CLdapSearchAction();
    virtual ~CLdapSearchAction();

    // 重写
    virtual bool Exec();

    /***
     * @brief : Exec过程中产生的资源回收
     * @param : |bCallBackSucceed| 是否回调成功
     * @notice: 涉及异步, 只能操作自身资源
     */
    virtual bool ReleaseData(bool bCallBackSucceed)
    {
        return false;
    }

    // 设置搜索条件.
    inline void SetSearchKey(const yl::string & strNumber)
    {
        m_strKey = strNumber;
    }
    // 获取搜索条件.
    inline yl::string GetSearchKey() const
    {
        return m_strKey;
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

    void SetClearDataFlag(bool bClear);

    //获取bind的Error code
    int GetErrorCode()
    {
        return m_nErrorCode;
    }

private:

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
    yl::string m_strKey;  // 请求查询的号码, 用于服务器查找.
    bool m_isSort;  // 是否排序.
    bool m_hasBound;  // 是否已经绑定服务器.
    int m_nMessageId;  // message id.

    bool m_bClear;

    int m_nErrorCode; //bind返回error code
};  // class CLdapAction

#endif  // TASKACTION_LDAPACTION_H_
