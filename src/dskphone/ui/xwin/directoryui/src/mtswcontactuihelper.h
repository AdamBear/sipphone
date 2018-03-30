#ifndef __MTSW_CONTACT_UI_HELPER_H__
#define __MTSW_CONTACT_UI_HELPER_H__

#if IF_FEATURE_METASWITCH_DIRECTORY

#include "commonuihelper.h"
#include "imtswuihelper.h"
#include "mtswcontactlayout.h"

class CEditListItem;

class CMTSWContactUIHelper : public CCommonUIHelper, public IMTSWUIHelper
{
public:
    CMTSWContactUIHelper();
    virtual ~CMTSWContactUIHelper();

public:
    static CMTSWContactUIHelper* GetUIHelper(IDirUIHelperPtr & pDirUIHelper);
    static IMTSWUIHelper * GetMTSWUIHelper(IDirUIHelperPtr & pDirUIHelper);
    static void OnLoginResultCallBack(bool bLoginSucceed);

public:
    virtual bool GetMTSWContactListData(ContactMetaSwitchContactList& itemData);
    virtual void SetInputFocus(const char * pszAttr);
    virtual bool IsAddition();
    virtual void EnterMTSWLoginPage();

public:
    virtual void OnEnterDetail(int nAction);
    virtual int GetLisItemId(xListViewItem * pItem);
    virtual int GetListItemType(int nState, int nDataIndex);
public:
    virtual yl::string GetNumberAttribute(const yl::string & strName);

public:
    virtual bool GetOption(int nUIState, VEC_OPTION_ITEM_DATA & vecOption);
    virtual void GetMenuTitle(yl::string & strTitle);
    virtual bool GetNormalTitle(yl::string & strTitle, int nGroupId = knInvalidRId);
    virtual bool GetDetailTitle(yl::string & strTitle);
    virtual bool GetSoftKey(xListViewItem * pFocusItem, int nUIState,
                            CArraySoftKeyBarData & vecSoftKey);
    virtual int GetActionByUIAction(int nUIState, int nUIAction);
protected:
    virtual bool UpdateListItem(xListViewItem * pListItem, int nId, SmartPtr & pDetail);
    bool GetNumber(SmartPtr & pDetail, int nType, yl::string& strNumber);
    bool GetContactListNumber(SmartPtr & pDetail, yl::string& strNumber);
    void InitCopyLayout();
    void AddCombobxOption(const yl::string& strNumber1, const yl::string& strNumber2
                          , int nFocusIndex, CComboBoxListItem* pComboboxItem);
    bool IsMenuID(int nID);
    void GetMetaSwitchDisplayName(int nGroupId, const yl::string& strGroupName,
                                  yl::string& strDisplayName);

protected:
    CMTSWContactLayoutPtr m_pDetailLayout;
};

#endif

#endif
