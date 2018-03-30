#ifndef __LDAP_CONTACT_CONTROLER_H__
#define __LDAP_CONTACT_CONTROLER_H__

#include "directorycontroler.h"
#include "directorycommon.h"
#ifdef DIR_SUPPORT_SINGLE_UIHELPER
#include "diruihelper.h"
#else
class CLDAPUIHelper;
#endif


class CLDAPContactControler : public CDirectoryControler
{
public:
    CLDAPContactControler();
    virtual ~CLDAPContactControler();

public:
    static CLDAPContactControler * GetControler(CDirectoryControlerPtr & pControler);

public:
    virtual bool PreSearchContact();
    virtual bool LoadMenuData(int nDataIndex, int nDataCount);
    virtual bool LoadListData(int nDataIndex, int nDataCount);
    virtual bool LoadDetailData(int nDataIndex, int nDataCount);
    virtual bool OnAction(int nAction, int nDataIndex, int nId);
    virtual bool DialContact(int nContactId, int nAccountId = AUTO_ACCOUNT_IDENTIFY);
    virtual bool GetContactName(yl::string & strName);
#ifdef DIR_SUPPORT_SWITCH_DETAIL
    virtual void GetDetailInfo(int & nId, int & nIndex, int & nCount) const;
    virtual bool GetContactImage(int nId, yl::string & strImage) const;
    virtual bool SwitchContact(int & nId, int & nIndex);
#endif
#if IF_FEATURE_34162
    virtual bool ExecAction(int nAction, int nDataIndex, int nId);
#endif

    virtual LRESULT OnMessage(msgObject & refMessage);

    CLDAPUIHelper * GetLDAPUIHelper();

protected:
    // 根据index获取item
    bool GetDetailByIndex(int nIndex, LdapContactData & ldapContact);
    void OnCopyContact(int nAction, int nIndex);
    bool GetNumberAttr(LIST_ATTRIBUTE & lsAttr, LdapContactData ldapContact);

protected:
    LdapContactData m_ItemInfo;
    LIST_LDAP_CONTACT_DATA m_listItem;
#ifdef DIR_SUPPORT_SWITCH_DETAIL
    yl::string m_strLastSearchKey;
#endif
};

#endif
