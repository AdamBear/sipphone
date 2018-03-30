#ifndef __LDAP_UI_HELPER_H__
#define __LDAP_UI_HELPER_H__

#include "diruihelper.h"
#include "taskaction/common.h"
#ifndef _WIN32
#warning
#endif
//#include "taskaction/ldapaction.h"

class CLDAPUIHelper : public CDirUIHelper
{
public:
    CLDAPUIHelper();
    virtual ~CLDAPUIHelper();

    static CLDAPUIHelper * GetUIHelper(IDirUIHelperPtr & pDirUIHelper);

public:
    virtual bool RefreshItem(int nState, xListViewItem * pItem);
    virtual void GetSearchTitle(yl::string & strTitle);
    virtual bool GetDetailTitle(yl::string & strTitle);
    virtual bool GetSoftKey(xListViewItem * pFocusItem, int nUIState,
                            CArraySoftKeyBarData & vecSoftKey);
    virtual int GetActionByUIAction(int nUIState, int nUIAction);

    virtual int GetActionByItem(int nItemAction, int nItemId);
};

#endif
