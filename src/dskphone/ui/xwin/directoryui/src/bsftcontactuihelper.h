#ifndef __BSFT_CONTACT_UI_HELPER_H__
#define __BSFT_CONTACT_UI_HELPER_H__

#include "commonuihelper.h"
#include "ibsftcontactuihelper.h"


class CBSFTDetailLayout;
class CBSFTSearchLayout;

class CBSFTContactUIHelper : public CCommonUIHelper, public IBSFTContactUIHelper
{
public:
    CBSFTContactUIHelper();
    virtual ~CBSFTContactUIHelper();

    static CBSFTContactUIHelper * GetUIHelper(IDirUIHelperPtr & pDirUIHelper);
    static IBSFTContactUIHelper * GetBSFTUIHelper(IDirUIHelperPtr & pDirUIHelper);
    static bool GetGroupDisplayName(int nGroupId, yl::string & strGroupName);

// IBSFTContactUIHelper
public:
    virtual bool GetBSFTPersonalData(ContactBroadSoftPersonal & itemData);
    virtual bool GetBSFTPreciseSearchData(int & nGroupId, ContactBroadSoftItemData & itemData);
    virtual void SetInputFocus(const char* pszAttr);
    virtual void EnterPreciseSearch();
    virtual void OnPreciseSearchResult();
    virtual void OnEnterPreciseSearch();

public:
    virtual void FocusDefaultNumber();
    virtual bool GetTitlePageInfo(yl::string& strPageInfo);
    virtual int GetListItemType(int nState, int nDataIndex);
    virtual bool RefreshItem(int nState, xListViewItem * pItem);
    virtual bool GetOption(int nUIState, VEC_OPTION_ITEM_DATA & vecOption);
    virtual void GetMenuTitle(yl::string & strTitle);
    virtual bool GetNormalTitle(yl::string & strTitle, int nGroupId = knInvalidRId);
    virtual bool GetDetailTitle(yl::string & strTitle);
    virtual bool GetSoftKey(xListViewItem * pFocusItem, int nUIState,
                            CArraySoftKeyBarData & vecSoftKey);
    virtual int GetActionByKeyPress(int nUIState, int nKey);
    virtual int GetActionByUIAction(int nUIState, int nUIAction);

    virtual void OnEnterDetail(int nAction);
    virtual bool IsBackMenuPageOver(int nPageType, int nGroupID);

protected:
    virtual bool UpdateListItem(xListViewItem * pListItem, int nId, SmartPtr & pDetail);
    int GetBackAction();

protected:
    CBSFTDetailLayout * m_pDetailLayout;
    CBSFTSearchLayout * m_pSearchLayout;
};

#endif
