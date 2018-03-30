#ifndef __DIR_UI_HELPER_H__
#define __DIR_UI_HELPER_H__

#include "directorycommon.h"
#include "idiruihelper.h"
#include "directoryuicommon.h"
#include "baseui/include/baseuicommon.h"
#include "baseui/include/t2xpicpath.h"
#include "uimanager/dialogviewcontroller.h"

class xListViewItem;
class xListView;
class CMenuListItem;

class CDirUIHelper : public IDirUIHelper
{
public:
    explicit CDirUIHelper(int nType);
    virtual ~CDirUIHelper();

    static CDirUIHelper * GetDirUIHelper(IDirUIHelperPtr pUIHelper);

    // 进入新界面
    static void EnterUI(int nDirType, int nUIState, int nAction, int nId);
    // 退出当前界面
    static void ExitCurrentUI(int nUIState, bool bReloadData = true);
    // 退出某一类型联系人所有界面
    static void ExitAllUI(int nDirType);
    // 回退到根节点
    static void ReturnRootGroup(int nDirType, bool bCheckCondition = false);
    // 检测当前状态是否正确
    static bool CheckState(int nCheckState, int nCurState);

    // 拔号
    static bool DialContact(int nAccountId, const yl::string & strName, LIST_ATTRIBUTE & lsAttr,
                            int nDialAction = DIR_DIAL_ACTION_NONE);
    static bool DialNumber(int nAccountId, const yl::string & strName, const yl::string & strNumber,
                           int nDialAction = DIR_DIAL_ACTION_NONE);

    static void SetDialAccountId(int nID);
    static int GetDialAccountId();
    static void ResetDialAccountId();
    static yl::string GetDialFocusNumber();
    static void ResetDialFocusNumber();

public:
    void Init();
    virtual void UnInit();

    void SetListView(xListView * pListView);

    int GetType() const;

    void SetShowTitle(bool bShowTitle);
    bool IsShowTitle();

    virtual bool GetTitlePageInfo(yl::string& strPageInfo);

// IDirUIHelper
public:
    virtual const yl::string & GetSearchKey();

    virtual void RefreshUI(int nType, int nParam);
    virtual void RefreshUIAfterDeleted(bool bBackUI = false);

public:
    virtual void FocusDefaultNumber();

    virtual int GetBrowsePageType(int nGroupId);
    virtual bool GetBrowseAdjoinGroup(bool bPrevious, int & nGroupId, int & nGroupIndex);
    virtual int GetListItemType(int nState, int nDataIndex);
    virtual bool RefreshItem(int nState, xListViewItem* pItem);
    virtual void GetMenuTitle(yl::string & strTitle);
    virtual bool GetGroupTitleList(int & nGroupIndex, VEC_GROUP_TITLE & vecTitle);
    virtual bool GetNormalTitle(yl::string & strTitle, int nGroupId = knInvalidRId);
    virtual void GetSearchTitle(yl::string & strTitle);
    virtual bool GetDetailTitle(yl::string & strTitle);
    virtual bool GetSoftKey(xListViewItem* pFocusItem, int nUIState,
                            CArraySoftKeyBarData & vecSoftKey)  = 0;
    virtual bool GetOption(int nUIState, VEC_OPTION_ITEM_DATA & vecOption);
    virtual int GetActionByKeyPress(int nUIState, int nKey);
    virtual int GetActionByUIAction(int nUIState, int nUIAction);
    virtual int GetLisItemId(xListViewItem* pItem);

    virtual bool ProcessLineKey(int nKey, int nUIState);
    virtual bool ProcessDetailDial(int nUIState, int nAccountId = AUTO_ACCOUNT_IDENTIFY);
    virtual bool IsFocusItemCanDial(xListViewItem* pItem);

    virtual void OnEnterDetail(int nAction);

    virtual int GetActionByItem(int nItemAction, int nItemId);

    virtual void CheckGroupID(int& nGroupID);

    virtual bool IsBackMenuPageOver(int nPageType, int nGroupID);

protected:
    int GetAdjoinGroupId(VEC_CONTACT_ID & vecGroupId, bool bPrevious, int nGroupId, int & nIndex);

    int GetContactId(xListViewItem* pItem);

    void PushDetailOption(ST_OPTION_ITEM_DATA & stOption, VEC_OPTION_ITEM_DATA & vecOption);

    bool GetContactSoftKey(xListViewItem* pFocusItem, int nUIState,
                           int nGroupId, CArraySoftKeyBarData & vecSoftKey);

    int GetContactCopyAction(int nUIAction);

    bool UpdateMenuItem(CMenuListItem* pMenuItem, int nId, const yl::string & strMenu,
                        bool bShowIndex);
    bool UpdateContactItem(xListViewItem* pItem, int nId, const yl::string & strName,
                           const yl::string & strNumber, const yl::string& strPhoto = PIC_CONTACT_NORMAL);
    bool CheckFocusSearchNum(bool bCheckState = true);

    virtual yl::string GetFocusContactNumber();
    void FocusFirstDetailNumber();

protected:
    int m_nType;
    bool m_bShowTitle;
    xListView* m_pListView;

    static int s_nBackUIPageIndex;
    static int s_nBackUIFocusIndex;

    // 记录号码选择界面使用的AccountId;
    static int s_nDialAccountId;

    // 记录号码选择界面使用的焦点Number
    static yl::string s_strDialFocusNumber;
};

#ifndef CDirUIHelperPtr
typedef YLSmartPtr<CDirUIHelper> CDirUIHelperPtr;
#endif

#endif
