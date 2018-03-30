#ifndef __LOCAL_CONTACT_UI_HELPER_H__
#define __LOCAL_CONTACT_UI_HELPER_H__

#include "commonuihelper.h"
#include "ilocalcontactuihelper.h"
#include <directorydefine.h>

class CLocalDetailLayout;

class CLocalContactUIHelper : public CCommonUIHelper, public ILocalContactUIHelper
{
public:
    CLocalContactUIHelper();
    virtual ~CLocalContactUIHelper();

    static CLocalContactUIHelper * GetUIHelper(IDirUIHelperPtr & pDirUIHelper);
    static ILocalContactUIHelper * GetLocalContactUIHelper(IDirUIHelperPtr & pDirUIHelper);
    static bool GetGroupDisplayName(int nGroupId, yl::string & strGroupName);

public:
    virtual void UnInit();

// ILocalContactUIHelper
public:
    virtual bool GetLocalGroupData(ContactGroupData & groupData);
    virtual bool GetLocalContactData(int & nGroupId, ContactGeneralItemData & itemData);
#if IF_FEATURE_FAVORITE_DIR
    virtual bool GetLocalContactFavorite(yl::string &strFavoriteIndex);
    virtual void PopSureEditMsgBox(const char * pszTitle, const char * pszMsg, int nAction, int nId);
#endif
    virtual void SetInputFocus(const char * pszAttr);

public:
    virtual int GetBrowsePageType(int nGroupId);
    virtual int GetListItemType(int nState, int nDataIndex);
#if IF_DIR_SUPPORT_FAVORITE_ITEM
    virtual int GetLisItemId(xListViewItem * pItem);
#endif
    virtual bool GetOption(int nUIState, VEC_OPTION_ITEM_DATA & vecOption);
    virtual void GetMenuTitle(yl::string & strTitle);
    virtual bool GetNormalTitle(yl::string & strTitle, int nGroupId = knInvalidRId);
    virtual bool GetDetailTitle(yl::string & strTitle);
    virtual bool GetSoftKey(xListViewItem * pFocusItem, int nUIState,
                            CArraySoftKeyBarData & vecSoftKey);
    virtual int GetActionByKeyPress(int nUIState, int nKey);
    virtual int GetActionByUIAction(int nUIState, int nUIAction);

    virtual void OnEnterDetail(int nAction);

#ifdef PHOTO_SHOW
public:
    void GetDetailLayoutPhoto(yl::string & strPhoto);
    void SetDetailLayoutPhoto(yl::string & strPhoto);
#endif

protected:
    virtual bool UpdateListItem(xListViewItem * pListItem, int nId, SmartPtr & pDetail);

    void InitCopyLayout(int nSrcDirType, const yl::string & strName, VEC_STRING & vecNumber);

    bool GetGroupOption(int nUIState, int nCurGroupId, int nFocusId, VEC_OPTION_ITEM_DATA & vecOption);

    void ReleaseLayout();

    bool GetDetailSoftKey(xListViewItem * pFocusItem, int nUIState, CArraySoftKeyBarData & vecSoftKey);
    bool GetBrowseSoftKey(int nCurGroupId, int nFocusId, bool bEdit, CArraySoftKeyBarData & vecSoftKey);
#if IF_DIR_SUPPORT_FAVORITE_ITEM
    bool UpdateFavoriteContactItem(xListViewItem* pItem, int nId, const yl::string & strName,
                                   const yl::string & strNumber, int nFavoriteIndex);
    virtual yl::string GetFocusContactNumber();
#endif
protected:
    CLocalDetailLayout * m_pDetailLayout;
};

#endif
