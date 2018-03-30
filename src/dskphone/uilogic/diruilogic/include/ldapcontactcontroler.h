#ifndef __LDAP_CONTACT_CONTROLER_H__
#define __LDAP_CONTACT_CONTROLER_H__

#include "directorycontroler.h"
#include "ildapuihelper.h"
#include "contacts/ldaplogic/include/modldap.h"


class CLDAPContactControler : public CDirectoryControler
{
public:
    CLDAPContactControler();
    virtual ~CLDAPContactControler();

public:
    static CLDAPContactControler * GetControler(CDirectoryControlerPtr & pControler);

public:
    virtual bool IsEmptyKeySearch() const;

    virtual bool PreSearchContact();
#if DIR_UI_SUPPORT_LOAD_DATA_LIST
    virtual bool LoadMenuData(int nDataIndex, int nDataCount);
    virtual bool LoadListData(int nDataIndex, int nDataCount);
#endif
    virtual bool OnAction(int nAction, int nDataIndex, int nId);
    virtual bool DialContact(int nContactId, int nAccountId = AUTO_ACCOUNT_IDENTIFY);
    virtual bool GetContactShowName(yl::string & strName);
#ifdef DIR_SUPPORT_SWITCH_DETAIL
    virtual void GetDetailInfo(int & nId, int & nIndex, int & nCount) const;
    virtual bool GetContactImage(int nId, yl::string & strImage) const;
    virtual bool SwitchContact(int & nId, int & nIndex);
#endif
    virtual bool ExecAction(int nAction, int nDataIndex, int nId);

    virtual int GetBrowseItemAmount();
    virtual int GetSearchItemAmount();

    virtual LRESULT OnMessage(msgObject & refMessage);

    int GetContactAmount();
    bool GetDetailByIndex(int nIndex, LdapContactData & ldapContact);

protected:
    void OnCopyContact(int nIndex, int nAction, int nGroupId);
    void UpdateDetailData(LdapContactData & ldapContact);

protected:
    yl::string m_strLastSearchKey;
};

#endif
