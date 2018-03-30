#ifndef __REMOTE_UI_HELPER_H__
#define __REMOTE_UI_HELPER_H__

#include "commonuihelper.h"
#include "iremoteuihelper.h"

struct ContactRemoteUnit;
struct ContactRemoteMenu;

class CRemoteUIHelper : public CCommonUIHelper, public IRemoteUIHelper
{
public:
    CRemoteUIHelper();
    virtual ~CRemoteUIHelper();

    static CRemoteUIHelper * GetUIHelper(IDirUIHelperPtr & pDirUIHelper);
    static IRemoteUIHelper * GetRemoteUIHelper(IDirUIHelperPtr & pDirUIHelper);

// IRemoteUIHelper
public:
    virtual void OnFinishDownLoad(int nGroupID, bool bSuccess);
    virtual void OnFinishAllDownLoad(int nGroupID);

public:
    virtual int GetListItemType(int nState, int nDataIndex);
    virtual void GetMenuTitle(yl::string & strTitle);
    virtual bool GetNormalTitle(yl::string & strTitle, int nGroupId = knInvalidRId);
    virtual bool GetSoftKey(xListViewItem * pFocusItem, int nUIState,
                            CArraySoftKeyBarData & vecSoftKey);
    virtual int GetActionByUIAction(int nUIState, int nUIAction);

protected:
    virtual bool UpdateListItem(xListViewItem * pListItem, int nId, SmartPtr & pDetail);
};

#endif
