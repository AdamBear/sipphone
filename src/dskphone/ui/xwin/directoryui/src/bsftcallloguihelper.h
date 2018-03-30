#ifndef __BSFT_CALLLOG_UI_HELPER_H__
#define __BSFT_CALLLOG_UI_HELPER_H__

#include "commonuihelper.h"


class CBSFTCallLogUIHelper : public CCommonUIHelper
{
public:
    CBSFTCallLogUIHelper();
    virtual ~CBSFTCallLogUIHelper();

    static CBSFTCallLogUIHelper * GetUIHelper(IDirUIHelperPtr & pDirUIHelper);

public:
    virtual int GetBrowsePageType(int nGroupId);
    virtual int GetListItemType(int nState, int nDataIndex);
    virtual bool GetOption(int nUIState, VEC_OPTION_ITEM_DATA & vecOption);
#if IF_DIR_UI_SHOW_CALLLOG_GROUP_LIST || IF_FEATURE_MUTI_CALLLOG
    virtual void GetMenuTitle(yl::string & strTitle);
#endif
    virtual bool GetSoftKey(xListViewItem * pFocusItem, int nUIState,
                            CArraySoftKeyBarData & vecSoftKey);
    virtual int GetActionByKeyPress(int nUIState, int nKey);
    virtual int GetActionByUIAction(int nUIState, int nUIAction);
    virtual int GetLisItemId(xListViewItem * pItem);
    virtual bool ProcessDetailDial(int nUIState, int nAccountId = AUTO_ACCOUNT_IDENTIFY);
    virtual void CheckGroupID(int& nGroupID);

protected:
    virtual bool UpdateListItem(xListViewItem * pListItem, int nId, SmartPtr & pDetail);

#if DIR_SUPPORT_NET_CALLLOG_DELETE
    bool IsCallLogDeleteable();
#endif
#if IF_FEATURE_MUTI_CALLLOG
    int GetFocusId();
#endif
};

#endif
