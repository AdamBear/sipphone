#ifndef __PLCM_UI_HELPER_H__
#define __PLCM_UI_HELPER_H__

#include "commonuihelper.h"
#include "iplcmuihelper.h"


#if IF_BUG_PLCMDIR
class CPlcmUIHelper : public CCommonUIHelper, public IPLCMUIHelper
{
public:
    CPlcmUIHelper();
    virtual ~CPlcmUIHelper();

    static CPlcmUIHelper * GetUIHelper(IDirUIHelperPtr & pDirUIHelper);
    static IPLCMUIHelper * GetPLCMUIHelper(IDirUIHelperPtr & pDirUIHelper);

public:
    virtual int GetBrowsePageType(int nGroupId);
    virtual int GetListItemType(int nState, int nDataIndex);
    virtual bool GetNormalTitle(yl::string & strTitle, int nGroupId = knInvalidRId);
    virtual bool GetSoftKey(xListViewItem * pFocusItem, int nUIState,
                            CArraySoftKeyBarData & vecSoftKey);
    virtual bool GetOption(int nUIState, VEC_OPTION_ITEM_DATA & vecOption);
    virtual int GetActionByUIAction(int nUIState, int nUIAction);

public:
    virtual void OnDownLoadGroup(int nGroupID, bool bSuccess);
    virtual void OnFinishAllDownLoad(int nGroupID);

protected:
    virtual bool UpdateListItem(xListViewItem * pListItem, int nId, SmartPtr & pDetail);
};
#endif

#endif
