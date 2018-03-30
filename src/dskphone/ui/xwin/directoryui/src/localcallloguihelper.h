#ifndef __LOCAL_CALLLOG_UI_HELPER_H__
#define __LOCAL_CALLLOG_UI_HELPER_H__

#include "commonuihelper.h"


class CLocalCallLogUIHelper : public CCommonUIHelper
{
public:
    CLocalCallLogUIHelper();
    virtual ~CLocalCallLogUIHelper();

    static CLocalCallLogUIHelper * GetUIHelper(IDirUIHelperPtr & pDirUIHelper);

public:
    virtual int GetBrowsePageType(int nGroupId);
    virtual int GetListItemType(int nState, int nDataIndex);

#if IF_DIR_UI_SHOW_CALLLOG_GROUP_LIST
    virtual void GetMenuTitle(yl::string & strTitle);
#endif
    virtual bool GetDetailTitle(yl::string & strTitle);
    virtual bool GetSoftKey(xListViewItem * pFocusItem, int nUIState,
                            CArraySoftKeyBarData & vecSoftKey);
    virtual bool GetOption(int nUIState, VEC_OPTION_ITEM_DATA & vecOption);
    virtual int GetActionByKeyPress(int nUIState, int nKey);
    virtual int GetActionByUIAction(int nUIState, int nUIAction);
    virtual int GetLisItemId(xListViewItem * pItem);
    virtual bool ProcessDetailDial(int nUIState, int nAccountId = AUTO_ACCOUNT_IDENTIFY);
#if !IF_DIR_UI_SHOW_CALLLOG_GROUP_LIST
    virtual void CheckGroupID(int& nGroupID);
#endif

protected:
    virtual bool UpdateListItem(xListViewItem * pListItem, int nId, SmartPtr & pDetail);
};

#endif
