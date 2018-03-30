#ifndef __COMMON_UI_HELPER_H__
#define __COMMON_UI_HELPER_H__

#include "diruihelper.h"


class CCommonUIHelper : public CDirUIHelper
{
public:
    explicit CCommonUIHelper(int nType);
    virtual ~CCommonUIHelper();

public:
    virtual int GetBrowsePageType(int nGroupId);
    virtual bool GetBrowseAdjoinGroup(bool bPrevious, int & nGroupId, int & nGroupIndex);

    virtual int GetListItemType(int nState, int nDataIndex);
    virtual bool RefreshItem(int nState, xListViewItem * pItem);

    virtual bool GetGroupTitleList(int & nGroupIndex, VEC_GROUP_TITLE & vecTitle);
    virtual bool GetNormalTitle(yl::string & strTitle, int nGroupId = knInvalidRId);
    virtual bool GetDetailTitle(yl::string & strTitle);
    virtual int GetLisItemId(xListViewItem * pItem);
    virtual bool IsFocusItemCanDial(xListViewItem * pItem);

    virtual int GetActionByItem(int nItemAction, int nItemId);

protected:
    virtual bool UpdateListItem(xListViewItem * pListItem, int nId, SmartPtr & pDetail)   = 0;
    bool UpdateCallLogItem(xListViewItem * pItem, int nId, int nCallType,
                           const yl::string & strName, const yl::string & strNumber,
                           const yl::string & strBornTick);

    int GetCallLogActionByKey(int nUIState, int nKey);
    int GetCallLogId(xListViewItem * pItem);

#if IF_FEATURE_CALLLIST_SHOW_METHOD
    bool GetCallLogDisplayInfo(yl::string & strText, const yl::string & strName,
                               const yl::string & strNumber, const yl::string & strSipServer);
#endif
    void GetCallLogTextToShow(yl::string& strDstName, yl::string& strDstNumber,
                              const yl::string& strSrcName, const yl::string& strSrcNumber);

    bool IsInRootGroup();
#if IS_COLOR
    bool IsCallLogShowNameAndNumber(int nDataIndex);
#endif
};

#endif
