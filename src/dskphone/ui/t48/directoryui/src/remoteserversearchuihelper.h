#ifndef __REMOTE_SERVER_SEARCH_UI_HELPER_H__
#define __REMOTE_SERVER_SEARCH_UI_HELPER_H__

#include "remoteuihelper.h"

class CRemoteServerSearchUIHelper : public CRemoteUIHelper
{
public:
    CRemoteServerSearchUIHelper();
    virtual ~CRemoteServerSearchUIHelper();

    static CRemoteServerSearchUIHelper * GetServerSearchUIHelper(CDirUIHelperPtr & pUIHelper);

public:
    virtual int GetActionByTitle(int nUIState, int nTitleAction);

    void RefreshServerSearchTitle(int nTotalCount);

protected:
    virtual bool RefreshListItemAmount(int nUIState, int nFromIndex, int nTotalCount);
};

#endif
