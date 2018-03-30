#ifndef __TALK_AYNC_LDAP_HELPER_HEADER_H__
#define __TALK_AYNC_LDAP_HELPER_HEADER_H__

#include "talkasync_data.h"
#include "yllist.h"
#include "singletonclassdefine.h"

using namespace NS_TA;

// 来电LDAP查找联系人助手
class CHelperLDAPInTalk
{
    DECLARE_SINGLETON_CLASS(CHelperLDAPInTalk)
    typedef YLList<LDAPSearchInfo*> LST_LDAP_INFO;
    typedef LST_LDAP_INFO::iterator LST_INFO_IT;

public:
    bool OnLdapCallback(const yl::string& strKey);

    // 设置搜索的CallIInfo
    void SetSearchCallInfo(LDAPSearchInfo* pSearchInfo);

    // 通过CalllID释放，考虑到可能存在消息移除，没有回调导致内存泄露
    void ClearByCallId(int iCallID);

private:
    // 保存LDAP搜索信息
    LST_LDAP_INFO  m_listLDAPInfo;
};

// 获取全局唯一的对象
#define _HelperLDAPInTalk GET_SINGLETON_INSTANCE(CHelperLDAPInTalk)

#endif
