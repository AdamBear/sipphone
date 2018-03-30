#if FEATURE_BROADTOUCH_UC

#include "btuccontactuihelper.h"
#include "btuccontactuihelper.h"
#include "btuccontactcontroler.h"
#include "btuclistitem.h"
#include "directorymgr.h"
#include "browseadapter.h"
#include "baseui/include/detaillistitem.h"
#include "baseui/include/menulistitem.h"
#include "baseui/include/t2xpicpath.h"
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"
#include "configiddefine/configiddefine.h"
#include "configparser/modconfigparser.h"

CBTUCUIHelper::CBTUCUIHelper()
    : CDirUIHelper(DIRECTORY_TYPE_BTUC)
{
}

CBTUCUIHelper::~CBTUCUIHelper()
{
}

CBTUCUIHelper* CBTUCUIHelper::GetUIHelper(CDirUIHelperPtr& pUIHelper)
{
    if (pUIHelper.IsEmpty() || DIRECTORY_TYPE_BTUC != pUIHelper->GetType())
    {
        return NULL;
    }

    return (CBTUCUIHelper *)pUIHelper.Get();
}

void CBTUCUIHelper::GetGroupDisplayName(int nGroupId, const yl::string& strGroupName,
                                        yl::string& strDisplayName)
{
    if (ALL_BUDDIES_ID == nGroupId)
    {
        strDisplayName = _LANG(TRID_BUDDIES);
    }
    else if (FAVOURITE_ID == nGroupId)
    {
        strDisplayName = _LANG(TRID_FAVOURITE);
    }
    else
    {
        strDisplayName = strGroupName;
    }
}

bool CBTUCUIHelper::GetBrowseAdjoinGroup(bool bPrevious, int& nGroupId, int& nGroupIndex)
{
    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    CBTUCContactControlerPtr pBTUCContact = CBTUCContactControler::GetControler(pControler);
    if (NULL == pBTUCContact)
    {
        return false;
    }

    return pBTUCContact->AdjustGroupIndex(bPrevious, nGroupId, nGroupIndex);
}

int CBTUCUIHelper::GetBrowsePageType(int nGroupId)
{
    if (knInvalidRId == nGroupId || kszRootId == nGroupId)
    {
        return BROWSER_PAGE_TYPE_MENU;
    }

#if IF_DIR_UI_SWITCH_GROUP_IN_BROWSE
    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    CBTUCContactControlerPtr pBTUCContact = CBTUCContactControler::GetControler(pControler);

    if (NULL != pBTUCContact && kszRootId == pBTUCContact->GetParGroupId(nGroupId))
    {
        return BROWSER_PAGE_TYPE_GROUP_SWITCH;
    }
#endif

    return BROWSER_PAGE_TYPE_NORMAL;
}

bool CBTUCUIHelper::GetOption(int nUIState, VEC_OPTION_ITEM_DATA& vecOption)
{
    return false;
}

int CBTUCUIHelper::GetListItemType(int nState, int nDataIndex)
{
    if (DIR_UI_STATE_CHOOSE_NUMBER == nState || DIR_UI_STATE_DETAIL == nState)
    {
        return CDetailListItem::GetDetailListItemType();
    }

    if (DIR_UI_STATE_BROWSER == nState && IsMenuPage())
    {
        return CMenuListItem::GetMenuItemType();
    }

    return CBTUCListItem::GetBTUCItemType();
}

bool CBTUCUIHelper::IsMenuPage()
{
    CDirectoryAdapterPtr pAdapter = g_DirectoryMgr.GetActiveAdapter();
    CBrowseAdapter* pBrowserAdapter = CBrowseAdapter::GetBrowserAdapter(pAdapter);
    if (NULL != pBrowserAdapter)
    {
        return pBrowserAdapter->IsMenuPage();
    }

    return false;
}

bool CBTUCUIHelper::RefreshItem(int nState, xListViewItem* pItem)
{
    if (NULL == pItem)
    {
        return false;
    }

    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    CBTUCContactControlerPtr pBTUCContact = CBTUCContactControler::GetControler(pControler);
    if (NULL == pBTUCContact)
    {
        return false;
    }

    CMenuListItem* pMenuItem = CMenuListItem::GetMenuListItem(pItem);
    if (NULL != pMenuItem)
    {
        yl::string strDispname = "";
        int nGroupId = -1;
        if (pBTUCContact->GetGroupDataByIndex(nGroupId, strDispname, pItem->GetDataIndex()))
        {
            UpdateMenuItem(pMenuItem, nGroupId, strDispname, true);
            return true;
        }

        return false;
    }

    CBTUCListItem* pBTUCItem = CBTUCListItem::GetBTUCListItem(pItem);
    if (NULL != pBTUCItem)
    {
        int nContactId = pBTUCContact->GetContactIdByDataIndex(pItem->GetDataIndex());
        if (-1 == nContactId)
        {
            return false;
        }

        PresenceInfo info;
        if (!CBTUCContactControler::GetDetail(nContactId, info))
        {
            return false;
        }

        UpdateBTUCContactItem(pItem, nContactId, info);

        return true;
    }

    return CDirUIHelper::RefreshItem(nState, pItem);
}

void CBTUCUIHelper::GetMenuTitle(yl::string& strTitle)
{
    strTitle = _LANG(TRID_BUDDIES);
}

bool CBTUCUIHelper::GetGroupTitleList(int& nGroupIndex, VEC_GROUP_TITLE& listTitle)
{
    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    CBTUCContactControlerPtr pBTUCContact = CBTUCContactControler::GetControler(pControler);

    if (NULL == pBTUCContact)
    {
        return false;
    }

    GroupList listGroup;

    if (!pBTUCContact->GetGroupList(listGroup))
    {
        return false;
    }

    int nCurGroupId = pBTUCContact->GetGroupId();
    GroupList::iterator iter = listGroup.begin();

    for (int i = 0; iter != listGroup.end(); ++i, ++iter)
    {
        GroupItem& itemGroup = *iter;
        yl::string strGroupName;

        GetGroupDisplayName(itemGroup.m_keyId, itemGroup.m_strDisplayName, strGroupName);

        listTitle.push_back(strGroupName);

        if (itemGroup.m_keyId == nCurGroupId)
        {
            nGroupIndex = i;
        }
    }

    return true;
}

bool CBTUCUIHelper::GetDetailTitle(yl::string& strTitle)
{
    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    CBTUCContactControler* pBTUCContact = CBTUCContactControler::GetControler(pControler);

    if (NULL == pBTUCContact)
    {
        return false;
    }

    yl::string strContactName;

    if (!pBTUCContact->GetContactShowName(strContactName))
    {
        return false;
    }

    strTitle = strContactName;

    return true;
}

bool CBTUCUIHelper::GetSoftKey(xListViewItem* pFocusItem, int nUIState,
                               CArraySoftKeyBarData& objSoftKey)
{
    if (objSoftKey.size() < 4)
    {
        return false;
    }

    if (NULL == pFocusItem)
    {
        return true;
    }

    if ((DIR_UI_STATE_CHOOSE_NUMBER == nUIState || DIR_UI_STATE_DETAIL == nUIState)
            && CDetailListItem::GetDetailListItemType() == pFocusItem->GetType())
    {
        CDetailListItemPtr pDetailItem = (CDetailListItemPtr)pFocusItem;
        const yl::string strData = pDetailItem->GetItemStringData();

        if (DIR_ATTR_NUMBER == strData)
        {
            objSoftKey[3].m_strSoftkeyTitle = _LANG(TRID_SEND);
            objSoftKey[3].m_iType = ST_SEND;
        }

        return true;
    }

    if (DIR_UI_STATE_BROWSER == nUIState && CMenuListItem::GetMenuItemType() == pFocusItem->GetType())
    {
        objSoftKey[2].m_strSoftkeyTitle = _LANG(TRID_SEARCH);
        objSoftKey[2].m_iType = ST_SEARCH;

        objSoftKey[3].m_strSoftkeyTitle = _LANG(TRID_ENTER);
        objSoftKey[3].m_iType = ST_ENTER;

        return true;
    }

    objSoftKey[2].m_strSoftkeyTitle = _LANG(TRID_VIEW);
    objSoftKey[2].m_iType = ST_VIEW;

    objSoftKey[3].m_strSoftkeyTitle = _LANG(TRID_SEND);
    objSoftKey[3].m_iType = ST_SEND;

    return true;
}

int CBTUCUIHelper::GetActionByUIAction(int nUIState, int nUIAction)
{
    if (ST_SEND == nUIAction)
    {
        return DIR_ACTION_COMMON_SEND;
    }
    else if (ST_ENTER == nUIAction)
    {
        return DIR_ACTION_COMMON_SELECT;
    }
    else if (ST_VIEW == nUIAction)
    {
        return DIR_ACTION_COMMON_DETAIL;
    }
    else if (ST_BLIND_TRANSFER == nUIAction)
    {
        return DIR_ACTION_COMMON_BTRANSFER;
    }

    return DIR_ACTION_NONE;
}

int CBTUCUIHelper::GetLisItemId(xListViewItem* pItem)
{
    CMenuListItem* pMenuItem = CMenuListItem::GetMenuListItem(pItem);
    if (NULL != pMenuItem)
    {
        return pMenuItem->GetId();
    }

    return GetContactId(pItem);
}

int CBTUCUIHelper::GetContactId(xListViewItem* pItem)
{
    CBTUCListItemPtr pBTUCItem = CBTUCListItem::GetBTUCListItem(pItem);
    if (NULL != pItem)
    {
        return pBTUCItem->GetId();
    }
    return knInvalidRId;
}

bool CBTUCUIHelper::IsFocusItemCanDial(xListViewItem* pItem)
{
    CBTUCListItemPtr pBTUCItem = CBTUCListItem::GetBTUCListItem(pItem);
    if (NULL != pBTUCItem)
    {
        return true;
    }

    return CDirUIHelper::IsFocusItemCanDial(pItem);
}

bool CBTUCUIHelper::UpdateBTUCContactItem(xListViewItem* pItem, int nId, PresenceInfo& info)
{
    CBTUCListItem* pBTUCItem = CBTUCListItem::GetBTUCListItem(pItem);
    if (NULL == pBTUCItem)
    {
        return false;
    }

    pBTUCItem->SetName(info.strContactName);
    pBTUCItem->SetSignature(info.m_strTitle);
    yl::string strPresence = GetPresenceIcon(info.ePresenceType);
    pBTUCItem->SetPresence(strPresence);
    pBTUCItem->SetPhoto(info.strAvatarInfo);
    pBTUCItem->SetId(info.m_nLocalId);

    return true;
}

yl::string CBTUCUIHelper::GetPresenceIcon(int nPresence)
{
    bool bNewIconMode = (1 == configParser_GetConfigInt(kszPresenceIconMode));
    switch (nPresence)
    {
    case _PRESENCE_TYPE_ONLINE:
        return bNewIconMode ? PIC_ICON_PRESENCE_NEW_ONLINE : PIC_ICON_PRESENCE_ONLINE;
        break;
    case _PRESENCE_TYPE_AWAY:
        return bNewIconMode ? PIC_ICON_PRESENCE_NEW_AWAY : PIC_ICON_PRESENCE_AWAY;
        break;
    case _PRESENCE_TYPE_BUSY:
        return bNewIconMode ? PIC_ICON_PRESENCE_NEW_BUSY : PIC_ICON_PRESENCE_BUSY;
        break;
    case _PRESENCE_TYPE_OFFLINE:
        return bNewIconMode ? PIC_ICON_PRESENCE_NEW_OFFLINE : PIC_ICON_PRESENCE_OFFLINE;
        break;
    default:
        break;
    }

    return bNewIconMode ? PIC_ICON_PRESENCE_NEW_UNKNOWN : PIC_ICON_PRESENCE_UNKNOWN;
}

#endif
