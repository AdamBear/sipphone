#include "btuccontactcontroler.h"

#if FEATURE_BROADTOUCH_UC
#include <configiddefine.h>
#include "configparser/modconfigparser.h"
#include "diruiadapter.h"
#include "presence/include/modpresence.h"
#include "translateiddefine.h"
#include "uilogiccommon/include/uilogiclanghelper.h"

CBTUCContactControler::CBTUCContactControler()
    : CDirectoryControler(DIRECTORY_TYPE_BTUC)
    , m_nContactId(0)
{
}

CBTUCContactControler::~CBTUCContactControler()
{
}

CBTUCContactControler* CBTUCContactControler::GetControler(CDirectoryControlerPtr& pControler)
{
    if (pControler.IsEmpty() || DIRECTORY_TYPE_BTUC != pControler->GetType())
    {
        return NULL;
    }

    return (CBTUCContactControler *)pControler.Get();
}

bool CBTUCContactControler::IsConfigure()
{
    return (1 == configParser_GetConfigInt(kszEnableUC)
            && 1 == configParser_GetConfigInt(kszBroadsoftActive));
}

int CBTUCContactControler::GetParGroupId(int nGroupId)
{
    return kszRootId;
}

void CBTUCContactControler::SetContactId(int nContactId)
{
    m_nContactId = nContactId;
}

int CBTUCContactControler::GetContactId() const
{
    return m_nContactId;
}

int CBTUCContactControler::GetContactAmount()
{
    int nCount = 0;


    GroupKey keyGroup = GetGroupId();

    if (IsSearchState())
    {
        const yl::string& strSearchKey = GetSearchKey();

        if (!strSearchKey.empty())
        {
            ContactBuddyListData listContact;

            listContact.m_eType = PRESENCE_UC;
            listContact.m_nIndexFrom = 0;
            listContact.m_nCountToGet = 1;

            if (Presence_GetListByGroupId(&listContact, keyGroup, 0, strSearchKey))
            {
                nCount = listContact.m_nTotalCount;
            }
        }
    }
    else
    {
        nCount = Presence_GetChildCountByGroupId(keyGroup, PRESENCE_UC);
    }

    return nCount;
}

int CBTUCContactControler::GetContactIdByIndex(int nDataIndex)
{
    int nContactId = knInvalidRId;

    yl::string strSearchKey;

#ifdef DIR_SUPPORT_SWITCH_DETAIL
    if (IsSearchState() || (IsDetailState() && DIR_UI_STATE_SEARCH == m_nDetailStatus))
#else
    if (IsSearchState())
#endif
    {
#ifdef DIR_SUPPORT_SWITCH_DETAIL
        strSearchKey = m_strSearchKey;
#endif

        if (strSearchKey.empty())
        {
            return nContactId;
        }
    }

    ContactBuddyListData listContact;
    GroupKey keyGroup = m_nGroupId;

    listContact.m_eType = PRESENCE_UC;
    listContact.m_nIndexFrom = nDataIndex;
    listContact.m_nCountToGet = 1;

    if (!Presence_GetListByGroupId(&listContact, keyGroup, 0, strSearchKey))
    {
        DIRUI_ERR("Presence_GetListByGroupId(Key:%s, Group:%d) fail.", strSearchKey.c_str(), keyGroup);
        return nContactId;
    }

    if (listContact.m_list.size() <= 0)
    {
        return nContactId;
    }

    nContactId = listContact.m_list.front();

    return nContactId;
}

bool CBTUCContactControler::PreSearchContact()
{
    // BroadTouch UC只支持同步搜索
    return true;
}

#if DIR_UI_FEATURE_LOAD_DATA_LIST
bool CBTUCContactControler::LoadMenuData(int nDataIndex, int nDataCount)
{
    IBTUCUIHelper* pUIHelper = g_DirUIAdapter.GetBTUCUIHelper(m_pUIHelper);

    if (NULL == pUIHelper)
    {
        return false;
    }

    GroupList lsGroup;

    if (!Presence_GetGroupList(PRESENCE_UC, lsGroup) || lsGroup.size() <= 0)
    {
        return false;
    }

    pUIHelper->FillGroupList(lsGroup, nDataIndex, nDataCount);

    return true;
}

bool CBTUCContactControler::LoadListData(int nDataIndex, int nDataCount)
{
    IBTUCUIHelper* pBTUCUIHelper = g_DirUIAdapter.GetBTUCUIHelper(m_pUIHelper);
    IDirUIHelper* pUIHelper = m_pUIHelper.Get();

    if (NULL == pUIHelper)
    {
        return false;
    }

    yl::string strSearchKey;

    if (IsSearchState())
    {
        strSearchKey = GetSearchKey();

        if (strSearchKey.empty())
        {
            return false;
        }

#ifdef DIR_SUPPORT_SWITCH_DETAIL
        m_strSearchKey = strSearchKey;
#endif
    }

    ContactBuddyListData listContact;
    GroupKey keyGroup = GetGroupId();

    listContact.m_eType = PRESENCE_UC;
    listContact.m_nIndexFrom = nDataIndex;
    listContact.m_nCountToGet = nDataCount;

    if (!Presence_GetListByGroupId(&listContact, keyGroup, 0, strSearchKey))
    {
        DIRUI_ERR("Presence_GetListByGroupId(Key:%s, Group:%d) fail.", strSearchKey.c_str(), keyGroup);
        return false;
    }

    if (pUIHelper->AdjustListData(listContact.m_nTotalCount, nDataIndex, nDataCount))
    {
        DIRUI_INFO("AdjustListData when search, DataIndex:%d DataCount:%d", nDataIndex, nDataCount);

        listContact.m_list.clear();
        listContact.m_nCountToGet = nDataCount;
        listContact.m_nIndexFrom = nDataIndex;

        if (!Presence_GetListByGroupId(&listContact, keyGroup, 0, strSearchKey))
        {
            DIRUI_ERR("Presence_GetListByGroupId(Key:%s, Group:%d) fail.", strSearchKey.c_str(), keyGroup);
            return false;
        }
    }

#ifdef DIR_SUPPORT_SWITCH_DETAIL
    SetTotalCount(listContact.m_nTotalCount);
#endif

    pBTUCUIHelper->FillContactList(listContact);

    return true;
}
#endif

bool CBTUCContactControler::GetGroupList(GroupList& listGroup)
{
    return Presence_GetGroupList(PRESENCE_UC, listGroup);
}

bool CBTUCContactControler::GetGroupDataByIndex(int &nGroupID, yl::string& strName, int nDataIndex)
{
    GroupList lsGroup;

    if (!Presence_GetGroupList(PRESENCE_UC, lsGroup))
    {
        return false;
    }

    GroupList::iterator iter = lsGroup.begin();

    for (int i = 0; iter != lsGroup.end(); iter++, ++i)
    {
        if (i == nDataIndex)
        {
            GroupItem& groupItem = *iter;
            nGroupID = groupItem.m_keyId;
            strName = groupItem.m_strDisplayName;
            return true;
        }
    }

    return false;
}

int CBTUCContactControler::GetGroupIndexById(int nGroupId)
{
    GroupList lsGroup;

    if (!Presence_GetGroupList(PRESENCE_UC, lsGroup))
    {
        return -1;
    }

    return GetGroupIndexById(nGroupId, lsGroup);
}

int CBTUCContactControler::GetGroupIndexById(int nGroupId, GroupList& lsGroup)
{
    GroupList::iterator iter = lsGroup.begin();

    for (int i = 0; iter != lsGroup.end(); iter++, ++i)
    {
        GroupItem& groupItem = *iter;

        if (groupItem.m_keyId == nGroupId)
        {
            return i;
        }
    }

    return -1;
}

bool CBTUCContactControler::AdjustGroupIndex(bool bPrevious, int& nGroupId, int& nGroupIndex)
{
    GroupList lsGroup;
    if (!Presence_GetGroupList(PRESENCE_UC, lsGroup))
    {
        return false;
    }

    int nSize = lsGroup.size();
    if (nSize <= 1)
    {
        return false;
    }

    int nCurGroup = GetGroupId();
    nGroupIndex = GetGroupIndexById(nCurGroup, lsGroup);
    if (-1 == nGroupIndex)
    {
        return false;
    }

    if (bPrevious)
    {
        nGroupIndex--;
    }
    else
    {
        nGroupIndex++;
    }

    if (nGroupIndex < 0)
    {
        nGroupIndex = nSize - 1;
    }
    else if (nGroupIndex >= nSize)
    {
        nGroupIndex = 0;
    }

    GroupList::iterator iter = lsGroup.begin();
    for (int i = 0; iter != lsGroup.end(); iter++, ++i)
    {
        if (i == nGroupIndex)
        {
            GroupItem& groupItem = *iter;
            nGroupId = groupItem.m_keyId;
            return true;
        }
    }

    return false;
}

bool CBTUCContactControler::GetDetail(int nId, PresenceInfo& info)
{
    info.m_nLocalId = nId;

    return Presence_GetContactDetail(info, PRESENCE_UC);
}

bool CBTUCContactControler::IsGroupIdValid(int nGroupId)
{
    GroupList lsGroup;

    if (!Presence_GetGroupList(PRESENCE_UC, lsGroup))
    {
        return false;
    }

    GroupList::iterator iter = lsGroup.begin();

    for (; iter != lsGroup.end(); iter++)
    {
        GroupItem& groupItem = *iter;

        if (groupItem.m_keyId == nGroupId)
        {
            return true;
        }
    }

    return false;
}

bool CBTUCContactControler::OnAction(int nAction, int nDataIndex, int nId)
{
    switch (nAction)
    {
    case DIR_ACTION_COMMON_DETAIL:
        {
            m_nContactId = nId;

            m_nDetailStatus = GetState();
#ifdef DIR_SUPPORT_SWITCH_DETAIL
            SetCurIndex(nDataIndex);
#endif
            PrepareDetailData();

            g_DirUIAdapter.EnterUI(m_nType, DIR_UI_STATE_DETAIL, nAction, nId);
            return true;
        }
        break;
    case DIR_ACTION_COMMON_SELECT:
        {
            g_DirUIAdapter.EnterUI(m_nType, DIR_UI_STATE_BROWSER, nAction, nId);

            return true;
        }
        break;
    case DIR_ACTION_COMMON_SEND:
        {
            DialContact(nId);

            return true;
        }
        break;
    default:
        break;
    }

    return false;
}

bool CBTUCContactControler::DialContact(int nContactId, int nAccountId/* = AUTO_ACCOUNT_IDENTIFY*/)
{
    LIST_ATTRIBUTE lsNumber;
    yl::string strName;

    if (GetDialDetail(nContactId, strName, lsNumber))
    {
        m_nContactId = nContactId;

        return Dial(nAccountId, strName, lsNumber, GetDialAction());
    }

    return false;
}

bool CBTUCContactControler::GetDialDetail(int nContactId, yl::string& strName,
        LIST_ATTRIBUTE& lsNum)
{
    PresenceInfo info;

    if (!GetDetail(nContactId, info))
    {
        return false;
    }

    strName = info.strContactName;

    YLList<Attribute>::iterator iter = info.m_listAttribute.begin();
    ST_CONTACT_ATTRIBUTE stAttr;

    for (; iter != info.m_listAttribute.end(); iter++)
    {
        Attribute& refData = *iter;

        if (ATTR_TYPE_NUMBER == refData.m_eType && !refData.m_strValue.empty())
        {
            stAttr.m_strName = refData.m_strName;
            stAttr.m_strValue = refData.m_strValue;
            lsNum.push_back(stAttr);
        }
    }

    return true;
}

bool CBTUCContactControler::GetContactShowName(yl::string& strName)
{
    PresenceInfo info;

    if (!GetDetail(m_nContactId, info))
    {
        return false;
    }

    strName = info.strContactName;

    return true;
}

#ifdef DIR_SUPPORT_SWITCH_DETAIL
void CBTUCContactControler::GetDetailInfo(int& nId, int& nIndex, int& nCount) const
{
    nId = m_nContactId;
    nIndex = GetCurIndex();
    nCount = GetTotalCount();
}

bool CBTUCContactControler::GetContactImage(int nId, yl::string& strImage) const
{
    PresenceInfo info;

    if (!GetDetail(nId, info))
    {
        return false;
    }

    if (!info.strAvatarInfo.empty())
    {
        strImage = "/tmp/" + info.strAvatarInfo;
    }
    else
    {
        strImage = "";
    }

    return true;
}

bool CBTUCContactControler::SwitchContact(int& nId, int& nIndex)
{
    yl::string strSearchKey;

    if (IsSearchState() || (IsDetailState() && DIR_UI_STATE_SEARCH == m_nDetailStatus))
    {
#ifdef DIR_SUPPORT_SWITCH_DETAIL
        strSearchKey = m_strSearchKey;
#endif

        if (strSearchKey.empty())
        {
            return false;
        }
    }

    ContactBuddyListData listContact;
    GroupKey keyGroup = m_nGroupId;

    listContact.m_eType = PRESENCE_UC;
    listContact.m_nIndexFrom = nIndex;
    listContact.m_nCountToGet = 1;

    if (!Presence_GetListByGroupId(&listContact, keyGroup, 0, strSearchKey))
    {
        DIRUI_ERR("Presence_GetListByGroupId(Key:%s, Group:%d) fail.", strSearchKey.c_str(), keyGroup);
        return false;
    }

    if (listContact.m_list.size() <= 0)
    {
        return false;
    }

    nId = listContact.m_list.front();
    m_nContactId = nId;
    s_nCurIndex = nIndex;

    return true;
}
#endif

int CBTUCContactControler::GetBrowseItemAmount()
{
    GroupKey keyGroup = GetGroupId();
    if (knInvalidRId == keyGroup)
    {
        GroupList refList;
        if (Presence_GetGroupList(PRESENCE_UC, refList))
        {
            return refList.size();
        }

        return 0;
    }

    return Presence_GetChildCountByGroupId(keyGroup, PRESENCE_UC);
}

int CBTUCContactControler::GetSearchItemAmount()
{
    GroupKey keyGroup = GetGroupId();
    const yl::string& strSearchKey = GetSearchKey();

    if (!strSearchKey.empty())
    {
        ContactBuddyListData listContact;

        listContact.m_eType = PRESENCE_UC;
        listContact.m_nIndexFrom = 0;
        listContact.m_nCountToGet = 1;

        if (Presence_GetListByGroupId(&listContact, keyGroup, 0, strSearchKey))
        {
            return listContact.m_nTotalCount;
        }
    }

    return 0;
}

void CBTUCContactControler::PrepareDetailData()
{
    m_vecDetail.clear();

    PresenceInfo info;

    if (!GetDetail(m_nContactId, info))
    {
        DIRUI_ERR("CBTUCContactControler::PrepareDetailData, Getetail Fail, Id:%s", m_nContactId);
        return;
    }

    ST_DETAIL_DATA stDetail;
    stDetail.nType = DETAIL_DATA_TYPE_NONE;
    stDetail.stAttr.m_strName = _UILOGIC_LANG_TRAN(TRID_USER_ID);
    stDetail.stAttr.m_strValue = info.strContactID;
    m_vecDetail.push_back(stDetail);

    stDetail.stAttr.m_strName = _UILOGIC_LANG_TRAN(TRID_NAME);
    stDetail.stAttr.m_strValue = info.strContactName;
    m_vecDetail.push_back(stDetail);

    YLList<Attribute>::iterator iter = info.m_listAttribute.begin();
    for (; iter != info.m_listAttribute.end(); iter++)
    {
        Attribute& attr = *iter;

        if (ATTR_TYPE_NUMBER == attr.m_eType)
        {
            stDetail.nType = DETAIL_DATA_TYPE_NUMBER;
        }
        else
        {
            stDetail.nType = DETAIL_DATA_TYPE_NONE;
        }

        stDetail.stAttr.m_strName = _UILOGIC_LANG_TRAN(attr.m_strName.c_str());
        stDetail.stAttr.m_strValue = attr.m_strValue;
        m_vecDetail.push_back(stDetail);
    }
}

int CBTUCContactControler::GetContactIdByDataIndex(int nDataIndex)
{
    GroupKey keyGroup = GetGroupId();
    yl::string strSearchKey = "";
    if (IsSearchState())
    {
        strSearchKey = GetSearchKey();
    }

    ContactBuddyListData listContact;
    listContact.m_eType = PRESENCE_UC;
    listContact.m_nIndexFrom = nDataIndex;
    listContact.m_nCountToGet = 1;

    if (Presence_GetListByGroupId(&listContact, keyGroup, 0, strSearchKey))
    {
        return listContact.m_list.back();
    }

    return -1;
}

LRESULT CBTUCContactControler::OnMessage(msgObject& refMessage)
{
    LRESULT retValue = FALSE;
    IDirUIHelperPtr pDirUIHelper = m_pUIHelper;
    IDirUIHelper* pUIHelper = pDirUIHelper.Get();

    if (NULL == pUIHelper)
    {
        return FALSE;
    }

    if (BROADSOFT_BUDDYLIST_REFRESH_FINISHED == refMessage.message)
    {
        if (IsBrowseState() || IsSearchState())
        {
            pUIHelper->RefreshUI(DIR_UI_REFRESH_TYPE_LIST_VIEW | DIR_UI_REFRESH_TYPE_TITLE,
                                 DIR_UI_REFRESH_PARAM_NONE);
        }

        retValue = TRUE;
    }
    else if (BROADSOFT_BUDDYLIST_GROUP_CHANGE == refMessage.message)
    {
        g_DirUIAdapter.ExitAllUI(m_nType);
        retValue = TRUE;
    }
    else if (PRESENCE_MESSAGE_FAVORITE_CHANGE == refMessage.message && IsBrowseState()
             && FAVOURITE_ID == GetGroupId())
    {
        pUIHelper->RefreshUI(DIR_UI_REFRESH_TYPE_LIST_VIEW | DIR_UI_REFRESH_TYPE_TITLE,
                             DIR_UI_REFRESH_PARAM_NONE);

        retValue = TRUE;
    }

    return retValue;
}
#endif
