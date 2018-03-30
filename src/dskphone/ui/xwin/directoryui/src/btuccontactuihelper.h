#ifndef __BTUC_UI_HELPER_H__
#define __BTUC_UI_HELPER_H__

#if FEATURE_BROADTOUCH_UC

#include "diruihelper.h"
#include "presence/include/modpresence.h"


class CBTUCUIHelper : public CDirUIHelper
{
public:
    CBTUCUIHelper();
    virtual ~CBTUCUIHelper();

    static CBTUCUIHelper* GetUIHelper(CDirUIHelperPtr& pUIHelper);
    static void GetGroupDisplayName(int nGroupId, const yl::string& strGroupName,
                                    yl::string& strDisplayName);

public:
    virtual bool GetBrowseAdjoinGroup(bool bPrevious, int& nGroupId, int& nGroupIndex);
    virtual int GetBrowsePageType(int nGroupId);
    virtual bool GetOption(int nUIState, VEC_OPTION_ITEM_DATA& vecOption);
    virtual int GetListItemType(int nState, int nDataIndex);
    virtual bool RefreshItem(int nState, xListViewItem * pItem);
    virtual void GetMenuTitle(yl::string& strTitle);
    virtual bool GetGroupTitleList(int& nGroupIndex, VEC_GROUP_TITLE& listTitle);
    virtual bool GetDetailTitle(yl::string& strTitle);
    virtual bool GetSoftKey(xListViewItem* pFocusItem, int nUIState, CArraySoftKeyBarData& vecSoftKey);
    virtual int GetActionByUIAction(int nUIState, int nUIAction);
    virtual int GetLisItemId(xListViewItem * pItem);
    virtual int GetContactId(xListViewItem* pItem);
    virtual bool IsFocusItemCanDial(xListViewItem* pItem);
protected:
    bool UpdateBTUCContactItem(xListViewItem * pItem, int nId, PresenceInfo& info);
    bool IsMenuPage();
    yl::string GetPresenceIcon(int nPresence);
};
typedef CBTUCUIHelper * CBTUCUIHelperPtr;

#endif // FEATURE_BROADTOUCH_UC
#endif //__BTUC_UI_HELPER_H__

