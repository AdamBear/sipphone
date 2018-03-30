#ifndef __LDAP_UI_HELPER_H__
#define __LDAP_UI_HELPER_H__

#include "diruihelper.h"
#include "threadtask/include/common.h"
#include "threadtask/include/ldapaction.h"

class CLDAPUIHelper : public CDirUIHelper
{
public:
    CLDAPUIHelper();
    virtual ~CLDAPUIHelper();

    static CLDAPUIHelper * GetUIHelper(CDirUIHelperPtr & pUIHelper);

public:
    bool FillLDAPContactList(LdapListData & listData);
    bool FillLDAPContact(LdapContactData & info, int nDataIndex, int nDataCount);
    virtual int GetActionByTitle(int nUIState, int nTitleAction);
    virtual void OnAttachFrameList();
    virtual void InitDetailTitle();
    void SetShowSearchTitle(bool bShowSearchTitle)
    {
        m_bShowSearchTitle = bShowSearchTitle;
    }
private:
    //bug:http://bugfree.yealink.com/Bug.php?BugID=140352
    //设置title数目为0时，可隐藏title
    bool m_bShowSearchTitle;
};

#endif
