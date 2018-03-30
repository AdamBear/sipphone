#include "directorycommon.h"
#ifndef DIR_SUPPORT_SINGLE_UIHELPER
#include "btucuihelper.h"
#endif
#include "btuccontactcontroler.h"
#include <configiddefine.h>
#include "configparser/modconfigparser.h"


CBTUCContactControler::CBTUCContactControler()
    : CDirectoryControler(DIRECTORY_TYPE_BTUC)
    , m_nContactId(0)
#ifdef DIR_SUPPORT_SWITCH_DETAIL
    , m_nDetailStatus(DIR_UI_STATE_NONE)
#endif
{
}

CBTUCContactControler::~CBTUCContactControler()
{
}

CBTUCContactControler * CBTUCContactControler::GetControler(CDirectoryControlerPtr & pControler)
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
            && configParser_GetConfigInt(kszBroadsoftActive) == 1);
}

CBTUCUIHelper * CBTUCContactControler::GetBTUCUIHelper()
{
#ifdef DIR_SUPPORT_SINGLE_UIHELPER
    return m_pUIHelper;
#else
    return CBTUCUIHelper::GetUIHelper(m_pUIHelper);
#endif
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

bool CBTUCContactControler::PreSearchContact()
{
    // BroadTouch UC只支持同步搜索
    return true;
}

bool CBTUCContactControler::LoadMenuData(int nDataIndex, int nDataCount)
{
    CBTUCUIHelper * pUIHelper = GetBTUCUIHelper();

    if (NULL == pUIHelper)
    {
        return false;
    }

#if FEATURE_BROADTOUCH_UC
    GroupList lsGroup;

    if (!Presence_GetGroupList(PRESENCE_UC, lsGroup) || lsGroup.size() <= 0)
    {
        return false;
    }

    pUIHelper->FillGroupList(lsGroup, nDataIndex, nDataCount);
#endif

    return true;
}

bool CBTUCContactControler::LoadListData(int nDataIndex, int nDataCount)
{
    CBTUCUIHelper * pUIHelper = GetBTUCUIHelper();

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

#if FEATURE_BROADTOUCH_UC
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

    pUIHelper->FillContactList(listContact);
#endif

    return true;
}

bool CBTUCContactControler::LoadDetailData(int nDataIndex, int nDataCount)
{
    CBTUCUIHelper * pUIHelper = GetBTUCUIHelper();

    if (NULL == pUIHelper)
    {
        return false;
    }

#if FEATURE_BROADTOUCH_UC
    PresenceInfo info;

    if (!GetDetail(m_nContactId, info))
    {
        return false;
    }

    pUIHelper->FillContact(info, nDataIndex, nDataCount);
#endif

    return true;
}

#if FEATURE_BROADTOUCH_UC
bool CBTUCContactControler::GetGroupList(GroupList & listGroup)
{
    return Presence_GetGroupList(PRESENCE_UC, listGroup);
}
#endif

int CBTUCContactControler::GetGroupIndexById(int nGroupId)
{
#if FEATURE_BROADTOUCH_UC
    GroupList lsGroup;

    if (!Presence_GetGroupList(PRESENCE_UC, lsGroup))
    {
        return -1;
    }

    GroupList::iterator iter = lsGroup.begin();

    for (int i = 0; iter != lsGroup.end(); iter++, ++i)
    {
        GroupItem & groupItem = *iter;

        if (groupItem.m_keyId == nGroupId)
        {
            return i;
        }
    }
#endif

    return -1;
}

#if FEATURE_BROADTOUCH_UC
bool CBTUCContactControler::GetDetail(int nId, PresenceInfo & info)
{
    info.m_nLocalId = nId;

    return Presence_GetContactDetail(info, PRESENCE_UC);
}
#endif

bool CBTUCContactControler::IsGroupIdValid(int nGroupId)
{
#if FEATURE_BROADTOUCH_UC
    GroupList lsGroup;

    if (!Presence_GetGroupList(PRESENCE_UC, lsGroup))
    {
        return false;
    }

    GroupList::iterator iter = lsGroup.begin();

    for (; iter != lsGroup.end(); iter++)
    {
        GroupItem & groupItem = *iter;

        if (groupItem.m_keyId == nGroupId)
        {
            return true;
        }
    }
#endif

    return false;
}

bool CBTUCContactControler::OnAction(int nAction, int nDataIndex, int nId)
{
    switch (nAction)
    {
    case DIR_ACTION_COMMON_DETAIL:
        {
            m_nContactId = nId;

#ifdef DIR_SUPPORT_SWITCH_DETAIL
            SetCurIndex(nDataIndex);
            m_nDetailStatus = GetState();
#endif

            CDirUIHelper::EnterUI(m_nType, DIR_UI_STATE_DETAIL, nAction, nId);
            return true;
        }
        break;
    case DIR_ACTION_COMMON_SELECT:
        {
            if (IsGroupIdValid(nId))
            {
                CDirUIHelper::EnterUI(m_nType, DIR_UI_STATE_BROWSER, nAction, nId);
            }
            else
            {
                DialContact(nId);
            }
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

        return CDirUIHelper::Dial(nAccountId, strName, lsNumber, this, NULL);
    }

    return false;
}

bool CBTUCContactControler::GetDialDetail(int nContactId, yl::string & strName,
        LIST_ATTRIBUTE & lsNum)
{
#if FEATURE_BROADTOUCH_UC
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
        Attribute & refData = *iter;

        if (ATTR_TYPE_NUMBER == refData.m_eType && !refData.m_strValue.empty())
        {
            stAttr.m_strName = refData.m_strName;
            stAttr.m_strValue = refData.m_strValue;
            lsNum.push_back(stAttr);
        }
    }
#endif

    return true;
}

bool CBTUCContactControler::GetContactName(yl::string & strName)
{
#if FEATURE_BROADTOUCH_UC
    PresenceInfo info;

    if (!GetDetail(m_nContactId, info))
    {
        return false;
    }

    strName = info.strContactName;
#endif

    return true;
}

#ifdef DIR_SUPPORT_SWITCH_DETAIL
void CBTUCContactControler::GetDetailInfo(int & nId, int & nIndex, int & nCount) const
{
    nId = m_nContactId;
    nIndex = GetCurIndex();
    nCount = GetTotalCount();
}

bool CBTUCContactControler::GetContactImage(int nId, yl::string & strImage) const
{
#if FEATURE_BROADTOUCH_UC
    PresenceInfo info;

    if (!GetDetail(nId, info))
    {
        return false;
    }

    if (!info.strAvatarInfo.empty())
    {
        strImage = TEMP_PATH + info.strAvatarInfo;
    }
    else
    {
        strImage = "";
    }
#endif

    return true;
}

bool CBTUCContactControler::SwitchContact(int & nId, int & nIndex)
{
    yl::string strSearchKey;

    if (DIR_UI_STATE_SEARCH == m_nDetailStatus)
    {
        strSearchKey = m_strSearchKey;

        if (strSearchKey.empty())
        {
            return false;
        }
    }

#if FEATURE_BROADTOUCH_UC
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
#endif

    return true;
}
#endif

int CBTUCContactControler::GetAdjoinGroupId(bool bPrevious, int & nIndex)
{
#if FEATURE_BROADTOUCH_UC
    GroupList listGroup;

    if (!Presence_GetGroupList(PRESENCE_UC, listGroup))
    {
        nIndex = 0;
        return knInvalidRId;
    }

    int nSize = listGroup.size();

    if (nSize <= 0)
    {
        nIndex = 0;
        return knInvalidRId;
    }

    GroupList::iterator iter = listGroup.begin();
    int nPreGroupId = knInvalidRId;
    int nNextGroupId = knInvalidRId;
    int nGroupId = GetGroupId();

    nIndex = -1;

    for (int i = 0; iter != listGroup.end(); iter++, ++i)
    {
        GroupItem & itemGroup = *iter;

        if (nGroupId == itemGroup.m_keyId)
        {
            nIndex = i;
            iter++;

            if (iter != listGroup.end())
            {
                nNextGroupId = (*iter).m_keyId;
            }

            break;
        }

        nPreGroupId = itemGroup.m_keyId;
    }

    if (-1 == nIndex)
    {
        nIndex = 0;

        GroupItem & itemGroup = listGroup.front();

        return itemGroup.m_keyId;
    }

    if (bPrevious)
    {
        nIndex -= 1;

        if (nIndex < 0)
        {
            nIndex = nSize - 1;

            GroupItem & itemGroup = listGroup.back();

            return itemGroup.m_keyId;
        }

        return nPreGroupId;
    }

    ++nIndex;

    if (nIndex >= nSize)
    {
        nIndex = 0;

        GroupItem & itemGroup = listGroup.front();

        return itemGroup.m_keyId;
    }

    return nNextGroupId;
#else
    return knInvalidRId;
#endif
}

LRESULT CBTUCContactControler::OnMessage(msgObject & refMessage)
{
    LRESULT retValue = FALSE;

    DIRUI_INFO("CBTUCContactControler::OnMessage(%d)", refMessage.message);

    if (BROADSOFT_BUDDYLIST_REFRESH_FINISHED == refMessage.message)
    {
        if (IsBrowserState() || IsSearchState())
        {
            CDirUIHelper::RefreshUI(false, false);
        }

        retValue = TRUE;
    }
    else if (BROADSOFT_BUDDYLIST_GROUP_CHANGE == refMessage.message)
    {
        CDirUIHelper::ExitAllUI(m_nType);
        retValue = TRUE;
    }
    else if (PRESENCE_MESSAGE_FAVORITE_CHANGE == refMessage.message && IsBrowserState()
             && FAVOURITE_ID == GetGroupId())
    {
        CDirUIHelper::RefreshUI(false, false);

        retValue = TRUE;
    }

    return retValue;
}
