#ifndef __TALK_ASYNC_DATA_HEADER_H__
#define __TALK_ASYNC_DATA_HEADER_H__

#include "ylstring.h"

enum TALK_RING_LOAD_STATUS
{
    TALK_RING_LS_UNKOWN = 0,
    TALK_RING_LS_LOADING,
    TALK_RING_LS_LOADED,
    TALK_RING_LS_FAILED,
};

// LDAP请求时的话机状态
enum Send_Status
{
    LDAP_NON = -1,

    // DAIL 的搜索类型
    LDAP_DIAL_FIRST = 0,
    LDAP_PREDIAL,
    LDAP_DIAL,
    LDAP_PRETRANS,
    LDAP_PRECONF,
    LDAP_CALLIN_FWD,
    LDAP_DIAL_LAST,

    // CALLIN 的搜索类型
    LDAP_CALLIN_FIRST,
    LDAP_CALLIN,
    LDAP_CALLIN_LAST,

    LDAP_CONTACT,       // 此状态是指在联系人
};

// LDAP搜索的信息
struct LDAPSearchInfo
{
    int iCallID;          // 对应的CallID
    bool m_bIsSecend;
    yl::string strSearchNum;   // 搜索的号码

    LDAPSearchInfo()
    {
        iCallID = -1;
        strSearchNum.clear();
        m_bIsSecend = false;
    }
};

#endif
