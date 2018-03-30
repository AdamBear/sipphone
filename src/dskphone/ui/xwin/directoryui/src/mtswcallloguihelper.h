#ifndef __MTSW_CALLLOG_UI_HELPER_H__
#define __MTSW_CALLLOG_UI_HELPER_H__

#if IF_FEATURE_METASWITCH_CALLLOG
#include "commonuihelper.h"
#include "imtswuihelper.h"

class CMTSWCallLogUIHelper : public CCommonUIHelper, public IMTSWCallLogUIHelper
{
public:
    CMTSWCallLogUIHelper();
    virtual ~CMTSWCallLogUIHelper();

    static CMTSWCallLogUIHelper* GetUIHelper(IDirUIHelperPtr& pUIHelper);
    static IMTSWCallLogUIHelper * GetMTSWCalllogUIHelper(IDirUIHelperPtr & pDirUIHelper);
    static void OnLoginResultCallBack(bool bLoginSucceed);

public:
    virtual void EnterMTSWLoginPage();
    virtual int GetBrowsePageType(int nGroupId);
    virtual int GetListItemType(int nState, int nDataIndex);
    virtual bool GetOption(int nUIState, VEC_OPTION_ITEM_DATA& vecOption);
    virtual void GetMenuTitle(yl::string& strTitle);
    virtual bool GetNormalTitle(yl::string& strTitle, int nGroupId = knInvalidRId);
    virtual bool GetSoftKey(xListViewItem* pFocusItem, int nUIState,
                            CArraySoftKeyBarData& vecSoftKey);
    virtual int GetActionByUIAction(int nUIState, int nUIAction);

    virtual bool ProcessDetailDial(int nUIState, int nAccountId = AUTO_ACCOUNT_IDENTIFY);
#if !IF_DIR_UI_SHOW_CALLLOG_GROUP_LIST
    virtual void CheckGroupID(int& nGroupID);
#endif

protected:
    virtual bool UpdateListItem(xListViewItem* pListItem, int nId, SmartPtr& pDetail);
};

#endif //#if IF_FEATURE_METASWITCH_CALLLOG

#endif
