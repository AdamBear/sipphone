#include "basecontactdata.h"
#include "commonunits/modcommonunits.h"
#include "modcommon.h"
#include "configiddefine.h"
#include "modconfigparser.h"

#if FEATURE_BROADTOUCH_UC

CBaseContactData::CBaseContactData()
{
    m_nId = 0;
    m_strLastSearchKey.clear();
    m_eType = PRESENCE_BASE;

    m_eSortType = SORT_BY_DEFOULT;

    ReSetGroupKey(m_lastGroupKey);
}


bool CBaseContactData::GetListByGroupId(ContactBuddyListData * pData, GroupKey & keyGroup,
                                        const yl::string & strKey/* = ""*/, AttrType eSearchType/* = ATTR_ALL*/)
{
    if (pData == NULL)
    {
        return false;
    }

    //是否搜索
    if (!strKey.empty())
    {
        return Search(pData, keyGroup, strKey, eSearchType);
    }

    int nFromIndex = pData->m_nIndexFrom;

    int nGetAmount = pData->m_nCountToGet;

    if (nFromIndex < 0 || nGetAmount < 0)
    {
        return false;
    }

    GroupItem * pGroup = GetGroupItemByID(keyGroup);

    if (pGroup == NULL)
    {
        return false;
    }

    pData->m_nTotalCount = pGroup->m_nIdList.size();

    if (nFromIndex == 0 && nGetAmount == 0)
    {
        nGetAmount = pData->m_nTotalCount;
    }

    int i = 0;

    for (CIterContactID iter = pGroup->m_nIdList.begin(); iter != pGroup->m_nIdList.end(); i++)
    {
        if (i < nFromIndex)
        {
            iter++;
            continue;
        }

        pData->m_list.push_back(*iter);
        if (--nGetAmount == 0)
        {
            break;
        }
        iter++;
    }

    return true;
}

int CBaseContactData::GetChildCountByGroupId(GroupKey & keyGroup)
{
    GroupItem * pGroup = GetGroupItemByID(keyGroup);

    if (pGroup != NULL)
    {
        return pGroup->m_nIdList.size();
    }

    return -1;
}

bool CBaseContactData::GetGroupList(GroupList & refList)
{
    if (m_GroupList.size() == 0)
    {
        return false;
    }

    for (GroupList::iterator iter = m_GroupList.begin(); iter != m_GroupList.end(); iter++)
    {
        GroupItem & Item = *iter;

        GroupItem newItem;
        newItem.m_strDisplayName = Item.m_strDisplayName;
        newItem.m_keyId = Item.m_keyId;
        refList.push_back(newItem);
    }

    return true;
}



void CBaseContactData::MyResetContactInfo()
{
    m_csSearch.Lock();

    yl::string strPath;
    for (IterMapPresenceInfo mapIter = m_mapPresenceInfo.begin(); mapIter != m_mapPresenceInfo.end();
            mapIter++)
    {
        PresenceInfo & refData = mapIter->second;

        if (refData.strAvatarInfo.empty())
        {
            continue;
        }

        strPath = TEMP_PATH + refData.strAvatarInfo;

        remove(strPath.c_str());

        APP_INFO("CUCContactData::GetImageByNumber strPath[%s]", strPath.c_str());
    }

    m_mapPresenceInfo.clear();

    m_mapKey.clear();
    m_mapGroupKey.clear();

    ReSetId();

    m_GroupList.clear();

    m_searchList.clear();
    m_csSearch.Unlock();

    MyResetSearchKey();
}

void CBaseContactData::MyResetSearchKey()
{
    m_csSearch.Lock();
    ReSetGroupKey(m_lastGroupKey);

    m_strLastSearchKey.clear();
    m_csSearch.Unlock();
}

bool CBaseContactData::Search(ContactBuddyListData * pData, GroupKey & keyGroup,
                              const yl::string & strKey, AttrType eSearchType/* = ATTR_ALL*/)
{
    m_csSearch.Lock();
    if (m_lastGroupKey != keyGroup)
    {
        m_lastGroupKey = keyGroup;
        m_strLastSearchKey = "";
    }

    //strKey = CommonUnits_ToKeyString(strKey);
    //是否在上次搜索基础上搜索

    if (commonAPI_StringMatch(strKey, m_strLastSearchKey, false) && !m_strLastSearchKey.empty())
    {
        IterPresenceInfo iter = m_searchList.begin();
        while (iter != m_searchList.end() && m_strLastSearchKey.size() != strKey.size())
        {

            IterMapPresenceInfo mapItem = MyFindInfoById(*iter);

            if (mapItem == m_mapPresenceInfo.end())
            {
                iter = m_searchList.erase(iter);
                continue;
            }
            PresenceInfo & refData = mapItem->second;

            if (!IsSearchHit(refData, strKey, eSearchType))
            {
                iter = m_searchList.erase(iter);
            }
            else
            {
                iter++;
            }
        }
    }
    else
    {
        m_searchList.clear();

        GroupItem * pGItem = GetGroupItemByID(keyGroup);

        if (pGItem == NULL)
        {
            m_csSearch.Unlock();
            return false;
        }

        for (CIterContactID it = pGItem->m_nIdList.begin(); it != pGItem->m_nIdList.end(); it++)
        {
            IterMapPresenceInfo iter = MyFindInfoById(*it);


            if (iter != m_mapPresenceInfo.end())
            {
                PresenceInfo & refData = iter->second;

                if (IsSearchHit(refData, strKey, eSearchType))
                {
                    m_searchList.push_back(refData.m_nLocalId);
                }
            }
        }
    }

    //记录搜索关键字
    m_strLastSearchKey = strKey;

    // 获取起始下标.
    int nIndexFrom = pData->m_nIndexFrom;
    //获取获取联系人个数
    int nCountGet = pData->m_nCountToGet;

    //记录联系人总数
    pData->m_nTotalCount = m_searchList.size();

    int i = 0;

    //更具UI显示要求返回数据
    for (CIterPresenceInfo iter = m_searchList.begin(); iter != m_searchList.end(); iter++)
    {
        if (i++ < nIndexFrom)
        {
            continue;
        }
        //stPreInfo = *iter;
        pData->m_list.push_back(*iter);

        if (--nCountGet == 0)
        {
            m_csSearch.Unlock();
            return true;
        }
    }
    m_csSearch.Unlock();
    return true;
}

bool CBaseContactData::SuperSearch(const yl::string & strKey, YLVector<SuperDirData> & output)
{
    APP_INFO("CBaseContactData::SuperSearch,strKey=[%s]", strKey.c_str());
    m_csSearch.Lock();

    int nId = ALL_BUDDIES_ID;

    GroupItem * pGItem = GetGroupItemByID(nId);

    if (pGItem == NULL)
    {
        m_csSearch.Unlock();
        return false;
    }

    SuperDirData itemData;

    for (CIterContactID it = pGItem->m_nIdList.begin(); it != pGItem->m_nIdList.end(); it++)
    {
        IterMapPresenceInfo iter = MyFindInfoById(*it);

        if (iter == m_mapPresenceInfo.end())
        {
            continue;
        }

        PresenceInfo & refData = iter->second;

        if (IsSearchHit(refData, strKey, ATTR_TYPE_NAME_AND_NUM))
        {
            itemData.m_nID = iter->first;
            int j = 0;

            for (YLList<Attribute>::iterator it = refData.m_listAttribute.begin();
                    it != refData.m_listAttribute.end(); it++)
            {
                Attribute & refData = *it;
                if (refData.m_eType == ATTR_TYPE_NUMBER)
                {
                    if (!refData.m_strValue.empty())
                    {
                        itemData.m_nNumberIdex = j;
                        output.push_back(itemData);
                    }
                    j++;
                }
            }
        }

    }
    m_csSearch.Unlock();

    return true;
}

bool CBaseContactData::IsSearchHit(int nId, const yl::string & strKey)
{
    m_csSearch.Lock();
    bool bRec = false;
    IterMapPresenceInfo iter = MyFindInfoById(nId);

    if (iter != m_mapPresenceInfo.end())
    {
        bRec = IsSearchHit(iter->second, strKey, ATTR_TYPE_NAME_AND_NUM);
    }

    m_csSearch.Unlock();

    return bRec;
}

bool CBaseContactData::IsSearchHit(PresenceInfo & refData,
                                   const yl::string & strKey,
                                   AttrType eSearchType/* = ATTR_ALL*/,
                                   StringMatchType eMatchType /*= CONTAIN_MATCH*/)
{
    if ((eSearchType == ATTR_TYPE_ALL || eSearchType == ATTR_TYPE_NAME_AND_NUM)
            && (commonAPI_StringMatch(commonAPI_transT9String(refData.strContactName), strKey, false,
                                      eMatchType)
                || commonAPI_StringMatch(refData.strContactName, strKey, false, eMatchType)
                || commonAPI_StringMatch(refData.strContactID, strKey, false, eMatchType)))
    {
        //固定属性命中
        APP_INFO("IsSearchHit static [%s][%s][%s][%d]", refData.strContactName.c_str(), refData.strContactID.c_str(), strKey.c_str(), eMatchType);
        return true;
    }
    else
    {
        for (YLList<Attribute>::const_iterator citer = refData.m_listAttribute.begin();
                citer != refData.m_listAttribute.end(); citer++)
        {
            const Attribute & attr = *citer;

            if (eSearchType != ATTR_TYPE_ALL && attr.m_eType != eSearchType
                    && !(eSearchType == ATTR_TYPE_NAME_AND_NUM && attr.m_eType == ATTR_TYPE_NUMBER))
            {
                continue;
            }

            if (commonAPI_StringMatch(attr.m_strValue, strKey, false, eMatchType))
            {
                APP_INFO("IsSearchHit dynamic [%s][%s][%d][%d]", attr.m_strValue.c_str(), strKey.c_str(), attr.m_eType, eMatchType);
                //动态属性命中
                return true;
            }
        }
    }

    return false;
}

IterMapPresenceInfo CBaseContactData::MyFindInfoByKey(const yl::string & strContactId)
{
    MapKeyToId::iterator iter = m_mapKey.find(strContactId);

    if (iter != m_mapKey.end())
    {
        return m_mapPresenceInfo.find(iter->second);
    }

    return m_mapPresenceInfo.end();
}

IterMapPresenceInfo CBaseContactData::MyFindInfoById(int id)
{
    return m_mapPresenceInfo.find(id);
}

int CBaseContactData::GetId()
{
    return ++m_nId;
}

void CBaseContactData::ReSetId(int nPreBegin)
{
    m_nId = nPreBegin;
}

bool CBaseContactData::GetContactDetail(PresenceInfo & refData)
{
    IterMapPresenceInfo iter = MyFindInfoById(refData.m_nLocalId);
    if (iter != m_mapPresenceInfo.end())
    {
        refData = iter->second;
        return true;
    }

    return false;
}

_PRESENCE_TYPE CBaseContactData::GetPresenceById(int nId)
{
    IterMapPresenceInfo iter = MyFindInfoById(nId);
    if (iter != m_mapPresenceInfo.end())
    {
        return iter->second.ePresenceType;
    }

    return _PRESENCE_TYPE_UNKNOWN;
}

bool CBaseContactData::GetSuperDataById(const SuperDirData & refInIterm,
                                        DirectorySearchData & refData)
{
    m_csSearch.Lock();
    IterMapPresenceInfo iter = MyFindInfoById(refInIterm.m_nID);

    if (iter != m_mapPresenceInfo.end())
    {
        PresenceInfo & IterData = iter->second;

        refData.m_strName = IterData.strContactName;

        Attribute stNub;

        if (!IterData.GetAttributeValue(refInIterm.m_nNumberIdex, ATTR_TYPE_NUMBER, stNub))
        {
            m_csSearch.Unlock();
            return false;
        }

        refData.m_strNumber = stNub.m_strValue;
        if (!IterData.strAvatarInfo.empty())
        {
            refData.m_strPic = TEMP_PATH + IterData.strAvatarInfo;
        }
        refData.m_eStatus = IterData.ePresenceType;

        m_csSearch.Unlock();
        return !refData.m_strNumber.empty();
    }

    m_csSearch.Unlock();
    return false;
}

void CBaseContactData::InsertToMap(PresenceInfo & refData)
{
    m_csSearch.Lock();
    refData.m_nLocalId = GetId();

    m_mapKey.put(refData.strContactID, refData.m_nLocalId);

    m_mapPresenceInfo.put(refData.m_nLocalId, refData);
    m_csSearch.Unlock();
}

void CBaseContactData::RemoveFromMapByKey(yl::string & strKey)
{
    m_csSearch.Lock();
    MapKeyToId::iterator iter = m_mapKey.find(strKey);

    if (iter != m_mapKey.end())
    {
        IterMapPresenceInfo it = m_mapPresenceInfo.find(iter->second);

        if (it != m_mapPresenceInfo.end())
        {
            m_mapPresenceInfo.erase(it);
        }

        m_mapKey.erase(iter);
    }
    m_csSearch.Unlock();
}

GroupItem * CBaseContactData::GetGroupItemByID(GroupKey & strId)
{
    for (GroupList::iterator iter = m_GroupList.begin(); iter != m_GroupList.end(); iter++)
    {
        GroupItem & refItem = *iter;

        if (refItem.m_keyId == strId)
        {
            return &refItem;
        }
    }

    return NULL;
}

const yl::string & CBaseContactData::GetSortKeyByKey(GroupKey & strId)
{
    IterMapPresenceInfo Iter = MyFindInfoById(strId);

    if (Iter != m_mapPresenceInfo.end())
    {
        switch (m_eSortType)
        {
        case SORT_BY_NAME :
            {
                return Iter->second.strContactName;
            }
            break;
        default:
            {
                return Iter->second.GetSortKey();
            }
            break;
        }
    }

    return kszEmptyString;
}

void CBaseContactData::MySort(GroupItem & refData)
{
    Sort(refData.m_nIdList, CompareItem);
}

void CBaseContactData::MyInsert(ListContactID & refList, GroupKey & stKey)
{
    ListContactID::iterator iter = refList.begin();

    const yl::string strrSortKey = GetSortKeyByKey(stKey);

    bool bFound = false;

    for (; iter != refList.end(); iter++)
    {
        const yl::string & strlSortKey = GetSortKeyByKey(*iter);

        if (strcasecmp(strlSortKey.c_str(), strrSortKey.c_str()) > 0)
        {
            bFound = true;
            break;
        }
    }

    if (bFound)
    {
        refList.insert(iter, stKey);
    }
    else
    {
        refList.push_back(stKey);
    }
}

bool CBaseContactData::GetNameByNumber(yl::string & strName, const yl::string & strNumber)
{
    StringMatchType eMachType = TOTAL_TAIL_MATCH;
    int nUcTailMatchNum = configParser_GetConfigInt(kszUcTailMatchNumber);
    //使用完全匹配
    if (nUcTailMatchNum == 0)
    {
        eMachType = TOTAL_MATH;
    }
    else if (strNumber.size() < nUcTailMatchNum)
    {
        return false;
    }

    for (IterMapPresenceInfo mapIter = m_mapPresenceInfo.begin(); mapIter != m_mapPresenceInfo.end();
            mapIter++)
    {
        PresenceInfo & refData = mapIter->second;

        if (IsSearchHit(refData, strNumber, ATTR_TYPE_NUMBER, eMachType))
        {
            strName = refData.strContactName;
            return true;
        }
    }

    return false;
}

#endif //FEATURE_BROADTOUCH_UC

