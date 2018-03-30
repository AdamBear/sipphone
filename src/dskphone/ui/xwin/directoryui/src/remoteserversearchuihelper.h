#ifndef __REMOTE_SERVER_SEARCH_UI_HELPER_H__
#define __REMOTE_SERVER_SEARCH_UI_HELPER_H__

#include "remoteuihelper.h"


class CRemoteServerSearchUIHelper : public CRemoteUIHelper
{
public:
    CRemoteServerSearchUIHelper();
    virtual ~CRemoteServerSearchUIHelper();

    static CRemoteServerSearchUIHelper * GetServerSearchUIHelper(IDirUIHelperPtr & pDirUIHelper);

// IRemoteUIHelper
public:
    virtual void OnFinishDownLoad(int nGroupID, bool bSuccess);

public:
    virtual int GetBrowsePageType(int nGroupId);
    virtual bool GetSoftKey(xListViewItem * pFocusItem, int nUIState,
                            CArraySoftKeyBarData & vecSoftKey);
    virtual int GetActionByUIAction(int nUIState, int nUIAction);
};

#endif
