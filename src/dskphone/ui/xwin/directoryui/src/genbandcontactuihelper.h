#ifndef __GENBAND_CONTACT_UI_HELPER_H__
#define __GENBAND_CONTACT_UI_HELPER_H__

#if IF_FEATURE_GBBOOK

#include "commonuihelper.h"
#include "igenbandcontactuihelper.h"
#include "genbandsearchlayout.h"
#include "genbanddetaillayout.h"

class CGenbandContactUIHelper : public CCommonUIHelper, public IGenbandContactUIHelper
{
public:
    CGenbandContactUIHelper();
    virtual ~CGenbandContactUIHelper();

    static CGenbandContactUIHelper* GetUIHelper(IDirUIHelperPtr& pUIHelper);
    static IGenbandContactUIHelper* GetGenbandUIHelper(IDirUIHelperPtr& pUIHelper);

public: // IGenbandContactUIHelper
    virtual void EnterPreciseSearch();
    virtual void OnPreciseSearchResult() {}
    virtual void OnEnterPreciseSearch();
    virtual bool GetGABPreciseSearchData(yl::string & strKey);
    virtual void OnEnterManageGroup(int nDirType, int nID);
    virtual bool GetGenbandContactData(ContactGBBookUnit& itemData);
    virtual bool GetGenbandGroupData(yl::string& strGroupName);
    virtual void SetInputFocus(const char * pszAttr);
    virtual void GetGenbandNumber(LIST_ATTRIBUTE& lsAttr, ContactGBBookUnit* pContact);
    virtual void OnChooseAccountResult(int nAccountID);
public:
    virtual int GetBrowsePageType(int nGroupId);
    virtual int GetListItemType(int nState, int nDataIndex);
    virtual bool GetOption(int nUIState, VEC_OPTION_ITEM_DATA& vecOption);
    virtual bool GetNormalTitle(yl::string& strTitle, int nGroupId = knInvalidRId);
    virtual bool GetDetailTitle(yl::string& strTitle);
    virtual bool GetSoftKey(xListViewItem* pFocusItem, int nUIState,
                            CArraySoftKeyBarData& vecSoftKey);
    virtual int GetActionByUIAction(int nUIState, int nUIAction);
    virtual int GetActionByKeyPress(int nUIState, int nKey);
    virtual void OnEnterDetail(int nAction);

protected:
    void AddGroupOption();
    virtual bool UpdateListItem(xListViewItem* pListItem, int nId, SmartPtr& pDetail);
public:
    virtual void ShowSearchModeOption(int nUIState);
protected:
    bool GetSearchModeOption(int nUIState, VEC_OPTION_ITEM_DATA& vecOption);
    int GetSearchTypeIndex();
    void GetItemDataByName(CEditListItem* pEditItem, const yl::string& strName, ContactGBBookUnit& itemData);
    void GetGroupDisplayName(int nGroupId, const yl::string & strGroupName, yl::string & strDisplayName);
    bool GetTitleBySearchMode(int nSearchMode, yl::string& strTitle);
    void GetNumberByName(ContactGBBookUnit* pContact, LIST_ATTRIBUTE & lsAttr,
                         const yl::string& strName, const yl::string& strDispName);
protected:
    CGenbandSearchLayout* m_pSearchLayout;
    CGenbandDetailLayout* m_pDetailLayout;
};
#endif //IF_FEATURE_GBBOOK
#endif //__GENBAND_CONTACT_UI_HELPER_H__
