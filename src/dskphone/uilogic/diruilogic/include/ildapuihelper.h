#ifndef __I_LDAP_UI_HELPER_H__
#define __I_LDAP_UI_HELPER_H__

#include <diruilogic/include/diruilogicdefine.h>

#if DIR_UI_SUPPORT_LOAD_DATA_LIST
#include "contacts/ldaplogic/include/modldap.h"

class ILDAPUIHelper
{
public:
    ILDAPUIHelper();
    virtual ~ILDAPUIHelper();

public:
    virtual bool FillLDAPContactList(LdapListData & listData)        = 0;
};
#endif

#endif
