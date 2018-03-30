#include "basecontactmanager.h"
#include "contactsearchaction.h"
#include "taskaction/modtaskaction.h"
#include "basemethod.h"
#include "../include/directorystructure.h"
#include "directorymessagedefine.h"

#include "etlmsghandler/modetlmsghandler.h"
#include "dircommonfunc.h"
#include "commonunits/modcommonunits.h"
#include "stable_sort.hpp"
#include "configiddefine.h"
#include "configparser/modconfigparser.h"

#include "localcontactmanager.h"
#include "contacts/ldaplogic/include/ldapdatadefine.h"
#include "regexp/modregexp.h"

#define MAX_NUMBER_LEN 128

bool CBaseContactManger::GetContactListByGroupId(ContactListData * pData, const int nGroupId)
{
    return MyCopyList(pData, nGroupId);
}

bool CBaseContactManger::GetSearchResultList(const yl::string & strSearchKey,
        ContactListData * pData)
{
    if (strSearchKey.compare(m_strSearchkey) != 0)
    {
        return false;
    }

    return MyCopyList(pData, kszSearchGroupId);
}

bool CBaseContactManger::LocalSearch(const yl::string & strSearchKey, const int nGroupId)
{
    DIR_INFO("CBaseContactManger::LocalSearch strKey[%s],nGroupId[%d]", strSearchKey.c_str(), nGroupId);
    if (m_pSearchAction == NULL)
    {
        return false;
    }

    m_pSearchAction->SetSearchGroupId(nGroupId);
    m_pSearchAction->SetSearchKey(strSearchKey);

    m_strSearchkey = strSearchKey;

    m_nSearchGroup = nGroupId;

    return TaskAction_ExecAction(m_pSearchAction);
}

CBaseContactManger::CBaseContactManger()
    : m_pSearchAction(NULL)
{
    m_eDirType = BASE_DIR;
    m_strSearchkey.clear();
    m_nId = kszIDBegin;
    m_bIsNeedAddToAllGroup = false;
    m_nSearchGroup = -1;

    m_pSearchAction = new CContactSearchAction(this);
}

CBaseContactManger::~CBaseContactManger()
{
    if (m_pSearchAction != NULL)
    {
        delete m_pSearchAction;
        m_pSearchAction = NULL;
    }

    MyFinal();
}

bool CBaseContactManger::MyInit()
{
    //创建All组
    DirGroupRecord * pAll = new DirGroupRecord;
    if (pAll != NULL)
    {
        pAll->m_strDisplayName = (m_eDirType == LOCAL_LOG ? kszGroupNameAllCalls : kszGroupNameAll);

        InsertMap(pAll, kszAllContactId);

        m_bIsNeedAddToAllGroup = true;

        if (IsNeedShowAllGroup())
        {
            AddIdToMenuShowList(kszAllContactId);
        }
    }

    //创建本地搜索组
    DirGroupRecord * pSearch = new DirGroupRecord;
    if (pSearch != NULL)
    {
        pSearch->m_strDisplayName = "Search Contact";

        InsertMap(pSearch, kszSearchGroupId);
    }

    return true;
}

void CBaseContactManger::MyFinal()
{
    ClearMap();
    m_contactLock.Lock();
    m_vGroupId.clear();
    ResetSearchKey();
    m_strSearchkey.clear();
    m_contactLock.Unlock();

    ReSetId();
}

void CBaseContactManger::ClearMap()
{
    m_contactLock.Lock();
    MapContactInfo::iterator iter = m_mapAllInfo.begin();
    while (iter != m_mapAllInfo.end())
    {
        BaseDirDataRecord * pData = iter->second;

        if (pData != NULL)
        {
            delete pData;
            pData = NULL;
        }
        iter++;
    }
    m_mapAllInfo.clear();
    m_contactLock.Unlock();
}

void CBaseContactManger::ClearAllContactFromMap(int ExceptGroupId1/* = knInvalidRId*/,
        int ExceptGroupId2/* = knInvalidRId*/)
{
    m_contactLock.Lock();
    //删除map中的联系人数据
    MapContactInfo::iterator iter = m_mapAllInfo.begin();
    while (iter != m_mapAllInfo.end())
    {
        BaseDirDataRecord * pData = iter->second;
        //具体联系人数据清除
        if (pData != NULL
                && pData->m_eInheritType == DIR_INHERIT_CONTACT)
        {
            if (pData->m_nParentId == ExceptGroupId1
                    || pData->m_nParentId == ExceptGroupId2)
            {
                iter++;
                continue;
            }
            delete pData;
            pData = NULL;

            iter = m_mapAllInfo.erase(iter);
        }
        else if (pData != NULL)
        {
            //联系人组数据清空其下联系人id表
            DirGroupRecord * pGroup = Base2DirGroup(pData);
            if (pGroup != NULL
                    && pGroup->m_nId != ExceptGroupId1
                    && pGroup->m_nId != ExceptGroupId2)
            {
                pGroup->m_vChildrenId.clear();
            }
            iter++;
        }
    }
    m_contactLock.Unlock();
}

bool CBaseContactManger::ClearContactByGroupID(int nGroupId, bool bClearPageID/* = false*/)
{
    //清楚数据重置搜索关键字
    ResetSearchKey();

    if (nGroupId == kszAllContactId)
    {
        ClearAllContactFromMap();
        return true;
    }

    DirGroupRecord * pGroup = GetMenuDataById(nGroupId);

    if (pGroup == NULL)
    {
        return false;
    }

    //特殊处理Hotkey
    ContactRemoteMenu * pRemote = Base2RemoteMenu(GetMenuDataById(nGroupId));
    if (pRemote != NULL)
    {
        for (YLList<int>::iterator iter = pRemote->m_listHotKey.begin();
                iter != pRemote->m_listHotKey.end(); iter++)
        {
            int nId = *iter;

            ClearContactByGroupID(nId, bClearPageID);

            MapContactInfo::iterator it = m_mapAllInfo.find(nId);

            if (it != m_mapAllInfo.end())
            {
                delete it->second;
                m_mapAllInfo.erase(it);
            }
        }
        pRemote->m_listHotKey.clear();

        //清除下一页缓存数据
        if (pRemote->m_nNextPageId != knInvalidRId)
        {
            ClearContactByGroupID(pRemote->m_nNextPageId, bClearPageID);

            MapContactInfo::iterator it = m_mapAllInfo.find(pRemote->m_nNextPageId);

            if (it != m_mapAllInfo.end())
            {
                delete it->second;
                m_mapAllInfo.erase(it);
            }
        }

        //http://bugfree.yealink.com/Bug.php?BugID=71144
        if (bClearPageID)
        {
            pRemote->m_nNextPageId = knInvalidRId;
            pRemote->m_nprevPageId = knInvalidRId;
        }
    }

    int iDeleteCount = 0;

    for (int i = 0; i < pGroup->m_vChildrenId.size(); i++)
    {
        MapContactInfo::iterator iter = m_mapAllInfo.find(pGroup->m_vChildrenId[i]);

        if (iter == m_mapAllInfo.end())
        {
            continue;
        }

        BaseDirDataRecord * pBase = iter->second;

        //具体联系人数据清除
        if (pBase != NULL && pBase->m_eInheritType == DIR_INHERIT_CONTACT)
        {
            m_contactLock.Lock();
            delete pBase;
            pBase = NULL;

            iDeleteCount++;

            iter = m_mapAllInfo.erase(iter);
            m_contactLock.Unlock();
        }
        else if (pBase != NULL)
        {
            //联系人组数据清空其下联系人id表
            DirGroupRecord * pGroup = Base2DirGroup(pBase);
            if (pGroup != NULL)
            {
                //删除孩子
                ClearContactByGroupID(pGroup->m_nId);
            }
            //删除自己
            m_contactLock.Lock();
            delete pBase;
            pBase = NULL;
            iDeleteCount++;
            m_mapAllInfo.erase(iter);
            m_contactLock.Unlock();
        }
    }

    DIR_INFO("ClearContactByGroupID id[%d] num[%d]", nGroupId, iDeleteCount);

    RemoveIdFromGroup(pGroup->m_vChildrenId, kszAllContactId);

    m_contactLock.Lock();
    pGroup->m_vChildrenId.clear();
    m_contactLock.Unlock();

    return true;
}

void CBaseContactManger::ClearChildIDListByGroupID(int nGroupID)
{
    DirGroupRecord * pGroup = GetMenuDataById(nGroupID);
    if (pGroup != NULL)
    {
        m_contactLock.Lock();
        pGroup->m_vChildrenId.clear();
        m_contactLock.Unlock();
    }
}

const yl::string & CBaseContactManger::GetDisplayNameById(int nId)
{
    BaseDirDataRecord * pBase = GetDataById(nId);

    if (pBase != NULL)
    {
        return pBase->GetDisplayName();
    }

    return kszEmptyString;
}

bool CBaseContactManger::GetConfigMenuIdList(VectorId & output)
{
    if (m_vGroupId.size() == 0)
    {
        return false;
    }
    output = m_vGroupId;

    return true;
}

bool CBaseContactManger::GetConfigMenuNameList(YLList<yl::string> & refData)
{
    if (m_vGroupId.size() == 0)
    {
        return false;
    }

    for (int i = 0; i < m_vGroupId.size(); i++)
    {
        DirGroupRecord * pGroup = GetMenuDataById(m_vGroupId[i]);

        if (pGroup != NULL)
        {
            refData.push_back(pGroup->m_strDisplayName);
        }
    }

    return true;
}

SmartPtr CBaseContactManger::GetDetailById(int nId)
{
    MapContactInfo::iterator iter = m_mapAllInfo.find(nId);

    if (iter != m_mapAllInfo.end())
    {
        return TransferToSmartPtr(iter->second);
    }

#if DEBUG_CONTACT
    DIR_WARN("No find contact[%d]", nId);
#endif //DEBUG_CONTACT

    return SmartPtr();
}

bool CBaseContactManger::GetSuperDataById(const SuperDirData & refInIterm,
        DirectorySearchData & refData,
        bool bGetPhoto/* = true */)
{
    m_contactLock.Lock();
    BaseContactRecord * pContact = Base2BaseContact(GetDataById(refInIterm.m_nID));

    if (pContact != NULL)
    {
        refData.m_strName = pContact->GetDisplayName();
        refData.m_strNumber = pContact->GetNumberByIndex(refInIterm.m_nNumberIdex);

        if (DSK_PHONE_TYPE_COLOR == commonUnits_GetDskPhoneType())
        {
            if (bGetPhoto)
            {
                if (pContact->GetType() == DIR_TYPE_BT_CONTACT)
                {
                    refData.m_strPic = DEFULT_BT_IMAGE_PATH;
                }
                else if (pContact->GetType() == DIR_TYPE_LOCAL_CONTACT)
                {
                    const LocalDirContactRecord * pLocalContact = Base2LocalContact(pContact);

                    if (NULL != pLocalContact)
                    {
                        refData.m_strPic = pLocalContact->m_strDefaultPhoto;
                    }
                }
                else if (pContact->GetType() == DIR_TYPE_LOCALLOG_CALLLOG)
                {
                    refData.m_strPic = g_pLocalDirManager->GetContactImageByNumber(refData.m_strNumber);
                }
                if (!refData.m_strPic.empty()
                        && refData.m_strPic.find_first_of("/") == yl::string::npos)
                {
                    // 隐藏路径的格式则转换为全路径
                    commonUnits_Hide2Real(refData.m_strPic, PHFILE_TYPE_CONTACT_IMAGE);
                }
            }
        }



        m_contactLock.Unlock();
        if (m_eDirType == LOCAL_LOG && refData.m_strName.empty())
        {
            refData.m_strName = refData.m_strNumber;
        }
        refData.m_eDirType = m_eDirType;
        return !refData.m_strNumber.empty();
    }

    m_contactLock.Unlock();
    return false;
}

int CBaseContactManger::GetParentIdById(int nId)
{
    BaseDirDataRecord * pRet = GetDataById(nId);
    if (pRet != NULL)
    {
        return pRet->m_nParentId;
    }
    return -1;
}

bool CBaseContactManger::IsDirAvailable()
{
    if (IsNeedShowAllGroup() && m_eDirType != LOCAL_DIR)
    {
        return m_vGroupId.size() > 1;
    }
    return m_vGroupId.size() > 0;
}

bool CBaseContactManger::GetContactNameByNumber(const yl::string & strNumber, yl::string & strName, bool bSync /* = true*/)
{
    const BaseContactRecord * pContact = GetBaseContactByNumber(strNumber, strName, bSync);

    if (pContact == NULL)
    {
        return false;
    }

    strName = pContact->m_strDisplayName;

    return true;
}

bool CBaseContactManger::MyCopyList(ContactListData * pData, int nId)
{
    if (pData == NULL || pData->m_nIndexFrom < 0)
    {
        return false;
    }

    VectorId * pVlist = NULL;

    VectorId vecOutput;
    if (nId == kszRootId)
    {
        // 获取所有对应根节点下的组ID,通过接口获取
        // Broadsoft历史记录的组ID和账号状态有关系
        GetConfigMenuIdList(vecOutput);
        pVlist = &vecOutput;
    }
    else
    {
        DirGroupRecord * pGroup = GetMenuDataById(nId);

        if (pGroup == NULL)
        {
            return false;
        }

        pVlist = &(pGroup->m_vChildrenId);
    }

    if (pVlist != NULL)
    {
        pData->m_nTotalCount = pVlist->size();

        DIR_INFO("GetContactList pData->m_nTotalCount[%d]", pData->m_nTotalCount);
        int nCount = pData->m_nCountToGet;
        if (pData->m_nIndexFrom == 0 && pData->m_nCountToGet == 0)
        {
            nCount = pData->m_nTotalCount;
        }

        for (int i = pData->m_nIndexFrom; i < pVlist->size(); i++)
        {
            if (nCount <= 0)
            {
                break;
            }

            pData->m_listId.push_back((*pVlist)[i]);
            nCount--;
        }

        return true;
    }

    return false;
}

BaseDirDataRecord * CBaseContactManger::GetDataById(int nId)
{
    MapContactInfo::iterator iter = m_mapAllInfo.find(nId);

    if (iter != m_mapAllInfo.end())
    {
        return iter->second;
    }
    return NULL;
}

DirGroupRecord * CBaseContactManger::GetMenuDataById(int nId)
{
    MapContactInfo::iterator iter = m_mapAllInfo.find(nId);

    if (iter != m_mapAllInfo.end())
    {
        return Base2DirGroup(iter->second);
    }

    return NULL;
}


int CBaseContactManger::InsertMap(BaseDirDataRecord * pData, int nID/* = knInvalidRId*/)
{
    if (nID == knInvalidRId)
    {
        nID = GetId();
    }

    pData->m_nId = nID;

    //to do syl 失败？
    m_mapAllInfo.put(nID, pData);

    return nID;
}

void CBaseContactManger::RemoveRecord(const VectorId & vecRemoveId, int nParentId)
{
    RemoveIdFromGroup(vecRemoveId, nParentId);

    if (nParentId != kszAllContactId
            && nParentId != kszBlackListGroupId)
    {
        //在真实组中删除，还需删除all组中的id
        RemoveIdFromGroup(vecRemoveId, kszAllContactId);
    }

    for (int i = 0; i < vecRemoveId.size(); i++)
    {
        int nId = vecRemoveId[i];
        MapContactInfo::iterator iter = m_mapAllInfo.find(nId);

        if (iter != m_mapAllInfo.end())
        {
            BaseDirDataRecord * pData = iter->second;

            if (pData != NULL)
            {
                if (pData->m_nParentId != nParentId
                        && nParentId == kszAllContactId)
                {
                    //在All组中删除，还需删除联系人真实组的id
                    VectorId vSecdId;
                    vSecdId.push_back(nId);
                    RemoveIdFromGroup(vSecdId, pData->m_nParentId);
                }
            }

            m_contactLock.Lock();
            if (pData != NULL)
            {
                delete pData;
            }
            m_mapAllInfo.erase(iter);
            m_contactLock.Unlock();
        }
    }
}

void CBaseContactManger::RemoveIdFromGroup(const VectorId & vecRemoveId, DirGroupRecord * pGroup)
{
    if (pGroup == NULL)
    {
        return;
    }
    m_contactLock.Lock();

#if 1
    int i = vecRemoveId.size() - 1;
    int j = pGroup->m_vChildrenId.size() - 1;
    for (; i >= 0 && j >= 0;)
    {
        if (pGroup->m_vChildrenId[j] == vecRemoveId[i])
        {
            pGroup->m_vChildrenId.removeAt(j);
            i--;
        }
        j--;
    }
#else
    int i = 0;
    int j = 0;
    for (; i < vecRemoveId.size() && j < pGroup->m_vChildrenId.size();)
    {
        if (pGroup->m_vChildrenId[j] == vecRemoveId[i])
        {
            pGroup->m_vChildrenId.removeAt(j);
            i++;
        }
        else
        {
            j++;
        }
    }
#endif
    m_contactLock.Unlock();
}

void CBaseContactManger::RemoveIdFromGroup(const VectorId & vecRemoveId, int nGroupId)
{
    DirGroupRecord * pGroup = GetMenuDataById(nGroupId);

    if (pGroup != NULL)
    {
        RemoveIdFromGroup(vecRemoveId, pGroup);
    }
}

DIRECTORY_RET_CODE CBaseContactManger::AddIdToGroup(int nId, int nGroupId, bool IsSort /* = true */,
        bool isOrder /* = true */)
{
    DirGroupRecord * pGroup = GetMenuDataById(nGroupId);
    if (pGroup == NULL)
    {
        return DIR_RET_GROUP_UNEXISTED;
    }

    if (!IsSort)
    {
        pGroup->m_vChildrenId.push_back(nId);

        return DIR_RET_SUCC;
    }

    if (pGroup->m_vChildrenId.size() == 0)
    {
        pGroup->m_vChildrenId.push_back(nId);
        return DIR_RET_SUCC;
    }

    int nLeft = 0;

    int nRingt = pGroup->m_vChildrenId.size() - 1;

    while (nLeft <= nRingt)
    {
        int nMid = (nLeft + nRingt) / 2;

        int nCmp = 0;

        if (m_eDirType == LOCAL_LOG)
        {
            nCmp = atoi(GetSortKeyById(pGroup->m_vChildrenId[nMid]).c_str()) - atoi(GetSortKeyById(
                        nId).c_str());
        }
        else
        {
            nCmp = GetCompResult(pGroup->m_vChildrenId[nMid], nId);
        }

        if ((nCmp > 0 && isOrder) || (nCmp < 0 && !isOrder))
        {
            nRingt = nMid - 1;
        }
        else if (nCmp == 0 && nGroupId != kszAllContactId && nGroupId != kszServerSearchGroupId
                 && m_eDirType != BSFT_DIR && m_eDirType != LOCAL_LOG)
        {
            return DIR_RET_CONTACT_EXIST;
        }
        else
        {
            nLeft = nMid + 1;
        }
    }

    int nIndex = nLeft > nRingt ? nLeft : nRingt;

    if (nIndex == pGroup->m_vChildrenId.size())
    {
        pGroup->m_vChildrenId.push_back(nId);
    }
    else
    {
        pGroup->m_vChildrenId.insertAt(nIndex, nId);
    }

    return DIR_RET_SUCC;
}

int CBaseContactManger::GetId()
{
    return ++m_nId;
}

void CBaseContactManger::ReSetId(int nPreBegin)
{
    m_nId = nPreBegin;
}

BaseContactRecord * CBaseContactManger::GetBaseContactByNumber(const yl::string & strNumber,
        const yl::string & strName/* = ""*/, bool bSync /* = true*/)
{
    if (strNumber.empty())
    {
        return NULL;
    }

#if IF_BUG_38979
    //来电异步执行正则表达式匹配
    yl::string strContactPattern;
    yl::string strIncomePattern;
    yl::string strContactWith;
    yl::string strIncomeWith;

    char szReplaced[MAX_NUMBER_LEN] = { 0 };
    int nReplaceRet = -1;
    HREGEXP stIncomeHergexp = 0;
    HREGEXP stContactHergexp = 0;

    if (!bSync)
    {
        //来电异步执行正则表达式匹配
        strContactPattern = configParser_GetConfigString(kszContactReplacePattern);
        strIncomePattern = configParser_GetConfigString(kszIncomeCallRelacePattern);
        strContactWith = configParser_GetConfigString(kszContactReplaceWith);
        strIncomeWith = configParser_GetConfigString(kszIncomeCallRelaceWith);

        if (!strIncomePattern.empty())
        {
            stIncomeHergexp = regExp_Alloc(strIncomePattern.c_str());
        }
        if (!strContactPattern.empty())
        {
            stContactHergexp = regExp_Alloc(strContactPattern.c_str());
        }
    }
#endif

    MapContactInfo::iterator iter = m_mapAllInfo.begin();

    for (; iter != m_mapAllInfo.end(); iter++)
    {
        BaseDirDataRecord * pBase = iter->second;

        if (pBase == NULL)
        {
            continue;
        }

        BaseContactRecord * pContact = Base2BaseContact(pBase);
        if (pContact != NULL)
        {
            for (ListNumber::iterator it = pContact->m_listNumber.begin(); it != pContact->m_listNumber.end();
                    it++)
            {
                stNumber & stNum = *it;

                yl::string  strIncomingNumber = strNumber;
                yl::string  strContactNumber = stNum.strValue;

                bool bHandle = false;
#if IF_BUG_38979
                if (!strIncomePattern.empty())
                {
                    memset(szReplaced, 0, MAX_NUMBER_LEN);
                    nReplaceRet = regExp_ReplaceEx(stIncomeHergexp, strIncomingNumber.c_str(),
                                                   strIncomeWith.c_str(), szReplaced, MAX_NUMBER_LEN);
                    if (nReplaceRet == 0)
                    {
                        strIncomingNumber = szReplaced;
                        bHandle = true;
                    }
                }

                if (!strContactPattern.empty())
                {
                    memset(szReplaced, 0, MAX_NUMBER_LEN);
                    nReplaceRet = regExp_ReplaceEx(stContactHergexp, strContactNumber.c_str(),
                                                   strContactWith.c_str(), szReplaced, MAX_NUMBER_LEN);
                    if (nReplaceRet == 0)
                    {
                        strContactNumber = szReplaced;
                        bHandle = true;
                    }
                }
#endif
#if IF_BUG_36497
                if (!bHandle
                        && strContactNumber.substr(0, 1) == "+"
                        && strIncomingNumber.substr(0, 1) == "0")
                {
                    int post1 = stNum.strValue.find("-");
                    if (post1 == yl::string::npos)
                    {
                        post1 = 0;
                    }
                    strIncomingNumber = strNumber.substr(1);
                    strContactNumber = stNum.strValue.substr(post1 + 1);
                    // 去除字符串里面的'-'号, 0592-123456 --> 0592123456
                    strContactNumber = commonAPI_replaceSubStringByTag(strContactNumber, "-", "");
                }
#endif
                if (strIncomingNumber.compare(strContactNumber) == 0
                        && (strName.empty()
                            || pContact->GetDisplayName().compare(strName) == 0))
                {
#if IF_BUG_38979
                    if (!bSync)
                    {
                        regExp_Release(stIncomeHergexp);
                        regExp_Release(stContactHergexp);
                    }
#endif
                    return pContact;
                }
            }
        }
    }

#if IF_BUG_38979
    if (!bSync)
    {
        regExp_Release(stIncomeHergexp);
        regExp_Release(stContactHergexp);
    }
#endif
    return NULL;
}

bool CBaseContactManger::IsSearchHit(int nId, const yl::string & strSearchKey,
                                     SearchMethod searchMethod,
                                     StringMatchType eMatchType)
{
    m_contactLock.Lock();
    BaseContactRecord * pContact = Base2BaseContact(GetDataById(nId));
    if (pContact != NULL)
    {
        bool bRec = pContact->SearchHit(strSearchKey, searchMethod, eMatchType) != HIT_TYPE_NONE;

        m_contactLock.Unlock();
        return bRec;
    }

    m_contactLock.Unlock();
    return false;
}

bool CBaseContactManger::SuperSearch(const yl::string & strKey, YLVector<SuperDirData> & output,
                                     StringMatchType eMatchType)
{

    DIR_INFO("CBaseContactManger::SuperSearch lock");
    m_contactLock.Lock();
    DirGroupRecord * pAllGroup = GetMenuDataById(kszAllContactId);

    if (pAllGroup == NULL)
    {
        m_contactLock.Unlock();
        return false;
    }

    SearchMethod searchMethod = commonAPI_IsDigital(strKey) ? SEARCH_BY_T9 : SEARCH_BY_NAME;

    SuperDirData itemData;

    for (int i = 0; i < pAllGroup->m_vChildrenId.size(); i++)
    {
        BaseDirDataRecord * pBase = GetDataById(pAllGroup->m_vChildrenId[i]);

        BaseContactRecord * pContact = Base2BaseContact(pBase);

        if (pBase == NULL || pContact == NULL)
        {
            continue;
        }

#if IF_BUG_27560
        // 隐藏的历史记录不被搜索到
        if (pContact->m_bHideSipName)
        {
            continue;
        }
#endif

        if (pContact->SearchHit(strKey, searchMethod, eMatchType) != HIT_TYPE_NONE)
        {
            itemData.m_nID = pAllGroup->m_vChildrenId[i];
            int j = 0;

            for (ListNumber::iterator it = pContact->m_listNumber.begin(); it != pContact->m_listNumber.end();
                    it++)
            {
                stNumber & stNum = *it;

                if (!stNum.strValue.empty())
                {
                    itemData.m_nNumberIdex = j;
                    output.push_back(itemData);
                }

                j++;
            }
        }
    }


    if (m_eDirType == REMOTE_DIR)
    {
        yl::stable_sort(output.begin(), output.end(), RemoteIdCmp);
    }
    DIR_INFO("CBaseContactManger::SuperSearch result size [%d]", output.size());
    DIR_INFO("CBaseContactManger::SuperSearch UnLock");
    m_contactLock.Unlock();

    return true;
}

bool CBaseContactManger::MySearch(int nGroupId, const yl::string & strKey, VectorId & output,
                                  SearchMethod m_eSearchType)
{
    DIR_INFO("CBaseContactManger::MySearch lock");
    m_contactLock.Lock();
    DirGroupRecord * pGroup = GetMenuDataById(nGroupId);
    StringMatchType eMatchtype = CONTAIN_MATCH;

    if (1 == configParser_GetConfigInt(kszDirSearchMatchtype))
    {
        eMatchtype = LEAD_MATCH;
    }

    if (pGroup == NULL)
    {
        m_contactLock.Unlock();
        return false;
    }

    for (int i = 0; i < pGroup->m_vChildrenId.size(); i++)
    {
        BaseDirDataRecord * pData = GetDataById(pGroup->m_vChildrenId[i]);

        if (pData == NULL)
        {
            continue;
        }

        if (pData->m_eInheritType == DIR_INHERIT_CONTACT)
        {
            if (pData->SearchHit(strKey, m_eSearchType, eMatchtype) != HIT_TYPE_NONE)
            {
                output.push_back(pGroup->m_vChildrenId[i]);
            }
        }
        else
        {
            m_contactLock.Unlock();
            MySearch(pGroup->m_vChildrenId[i], strKey, output, m_eSearchType);
            m_contactLock.Lock();
        }
    }

    DIR_INFO("CBaseContactManger::MySearch UnLock");
    m_contactLock.Unlock();

    return true;
}

bool CBaseContactManger::MySearchFromResult(const yl::string & strKey, VectorId & output,
        SearchMethod m_eSearchType)
{
    VectorId outpotCopy;
    StringMatchType eMatchtype = CONTAIN_MATCH;

    if (1 == configParser_GetConfigInt(kszDirSearchMatchtype))
    {
        eMatchtype = LEAD_MATCH;
    }

    DIR_INFO("CBaseContactManger::MySearchFromResult lock");
    m_contactLock.Lock();
    for (int i = 0; i < output.size(); i++)
    {
        BaseDirDataRecord * pData = GetDataById(output[i]);

        if (pData == NULL)
        {
            continue;
        }

        if (pData->SearchHit(strKey, m_eSearchType, eMatchtype) != HIT_TYPE_NONE)
        {
            outpotCopy.push_back(output[i]);
        }
    }
    DIR_INFO("CBaseContactManger::MySearchFromResult UnLock");
    m_contactLock.Unlock();

    output = outpotCopy;

    return true;
}

void CBaseContactManger::OnTaskActionCallback(CTaskAction * pTaskAction)
{
    if (pTaskAction->GetReqType() != TA_REQUEST_LOCAL_CONTACT_SEARCH)
    {
        return;
    }

    // 返回结果的类型为请求网络文件.
    CContactSearchAction * pSearchAction =
        static_cast<CContactSearchAction *>(pTaskAction);
    if (pSearchAction == NULL)
    {
        return;
    }

    if (!pSearchAction->m_bIsNeedNotif)
    {
        return;
    }

    bool result = false;

    if (m_strSearchkey.compare(pSearchAction->GetSearchKey()) != 0)
    {
        return;
    }

    DirGroupRecord * pSearchGroup = GetMenuDataById(kszSearchGroupId);

    if (pSearchGroup != NULL)
    {
        if (pTaskAction->IsOperationSucceed())
        {
            result = true;
            pSearchGroup->m_vChildrenId = pSearchAction->GetSearchResult();

            if (m_eDirType == REMOTE_DIR)
            {
                yl::stable_sort(pSearchGroup->m_vChildrenId.begin(), pSearchGroup->m_vChildrenId.end(),
                                RemoteContactIdCmp);
            }
        }
        else
        {
            ResetSearchKey();
        }
    }

    // 发送消息.
    etl_NotifyAppEx(false, LOCAL_SEARCH_RESULT, m_eDirType, result,
                    (int)m_strSearchkey.length() + 1, m_strSearchkey.c_str());

}

const yl::string & CBaseContactManger::GetSortKeyById(int nId)
{
    BaseDirDataRecord * pBase = GetDataById(nId);

    if (pBase != NULL)
    {
        return pBase->GetDisplayName();
    }

    return kszEmptyString;
}

void CBaseContactManger::AddIdToMenuShowList(int nGroupId)
{
    m_vGroupId.push_back(nGroupId);
}

// 保存组名和组ID到临时文件中，以供网页使用
void CBaseContactManger::SaveGroupInfoToTempFile(const yl::string & strFileName,
        const yl::string & stdNodeName)
{
#if 0
    // web新框架已经不再使用临时文件访问联系人了。暂时去除
    // 设置Xml文件头
    xml_document docSave;
    xml_node nodeDecl = docSave.prepend_child(node_declaration);
    nodeDecl.append_attribute("version", "1.0");
    nodeDecl.append_attribute("encoding", "ISO-8859-1");

    xml_node nodeRoot = docSave.append_child(stdNodeName.c_str());

    //保存固定组
    for (int i = 0; i < m_vGroupId.size(); i++)
    {
        DirGroupRecord * pGroup = GetMenuDataById(m_vGroupId[i]);

        if (pGroup != NULL)
        {
            xml_node nodeGroup = nodeRoot.append_child("Group");
            nodeGroup.append_attribute("name", pGroup->m_strDisplayName.c_str());
            nodeGroup.append_attribute("id", m_vGroupId[i]);

        }
    }

    //BSFT需要可配custom联系人子集
    if (m_eDirType == BSFT_DIR)
    {
        DirGroupRecord * pCustom = GetMenuDataById(kszCustomId);

        if (pCustom != NULL)
        {
            //保存固定组
            for (int i = 0; i < pCustom->m_vChildrenId.size(); i++)
            {
                DirGroupRecord * pGroup = GetMenuDataById(pCustom->m_vChildrenId[i]);
                if (pGroup != NULL)
                {
                    xml_node nodeGroup = nodeRoot.append_child("Group");
                    nodeGroup.append_attribute("name", pGroup->m_strDisplayName.c_str());
                    nodeGroup.append_attribute("id", pCustom->m_vChildrenId[i]);
                }
            }
        }
    }

    yl::string strPath(g_strTempPath);
    strPath.append(strFileName.c_str());
    docSave.save_file(strPath.c_str());
#endif
}

void CBaseContactManger::ResetSearchKey()
{
    if (m_pSearchAction != NULL)
    {
        m_pSearchAction->ReSet();
    }
}

int CBaseContactManger::GetChildListCount(int nGroupId)
{
    DirGroupRecord * pGroup = GetMenuDataById(nGroupId);
    if (pGroup != NULL)
    {
        return pGroup->m_vChildrenId.size();
    }
    return 0;
}

int CBaseContactManger::GetContactCount(int nGroupId)
{
    int nCount = 0;
    DirGroupRecord * pGroup = GetMenuDataById(nGroupId);
    if (pGroup != NULL)
    {
        for (int i = 0; i < pGroup->m_vChildrenId.size(); i++)
        {
            BaseDirDataRecord * pBase = GetDataById(pGroup->m_vChildrenId[i]);
            if (pBase != NULL && pBase->m_eInheritType == DIR_INHERIT_CONTACT)
            {
                nCount += pGroup->m_vChildrenId.size();
                break;
            }
            else if (pBase != NULL)
            {
                nCount += GetContactCount(pGroup->m_vChildrenId[i]);
            }
        }
    }
    return nCount;
}

// 拷贝一份数据并存为知道指针对象给UI
SmartPtr CBaseContactManger::TransferToSmartPtr(BaseDirDataRecord * pDataOri)
{
    if (NULL == pDataOri)
    {
        return SmartPtr();
    }

    DIR_DATA_ENTRY_TYPE eType = pDataOri->GetType();
    BaseDirDataRecord * pData = CreateDataEntry(eType);
    if (NULL == pData)
    {
        return SmartPtr();
    }

    switch (eType)
    {
    case DIR_TYPE_BT_CONTACT:
    case DIR_TYPE_LOCAL_CONTACT://本地联系人
        *((LocalDirContactRecord *)pData) = *((LocalDirContactRecord *)pDataOri);
        break;

    case DIR_TYPE_GROUP: //组
        *((DirGroupRecord *)pData) = *((DirGroupRecord *)pDataOri);
        break;

    case DIR_TYPE_LOCALLOG_CALLLOG: //本地历史记录
        *((LocalLogItemRecord *)pData) = *((LocalLogItemRecord *)pDataOri);
        break;

    case DIR_TYPE_BSFT_LOG_GROUP: // BSFT联系人和历史记录组
        *((BSFTLogGroupRecord *)pData) = *((BSFTLogGroupRecord *)pDataOri);
        break;

    case DIR_TYPE_BSFT_CALLLOG: // BSFT历史记录
        *((BSFTCallLogRecord *)pData) = *((BSFTCallLogRecord *)pDataOri);
        break;

    case DIR_TYPE_BROADSOFT_MENU://BraodSoft联系人组
        {
            // 给外部模块的Detial数据，只需要复制UI需要的数据即可
            // 全部数据复制效率低下
            //*((ContactBroadSoftMenu *)pData) = *((ContactBroadSoftMenu *)pDataOri);
            ContactBroadSoftMenu * pMenu = Base2BsftMenu(pData);
            ContactBroadSoftMenu * pOriMenu = Base2BsftMenu(pDataOri);
            if (pMenu != NULL && pOriMenu != NULL)
            {
                pMenu->CopyShowUIData(pOriMenu);
            }
        }
        break;

    case DIR_TYPE_BROADSOFT_ITEM_COMMON:
        *((ContactBroadSoftCommon *)pData) = *((ContactBroadSoftCommon *)pDataOri);
        break;

    case DIR_TYPE_BROADSOFT_ITEM_CUSTOM:
        *((ContactBroadSoftCustom *)pData) = *((ContactBroadSoftCustom *)pDataOri);
        break;

    case DIR_TYPE_BROADSOFT_ITEM_PERSONAL:
        *((ContactBroadSoftPersonal *)pData) = *((ContactBroadSoftPersonal *)pDataOri);
        break;

    case DIR_TYPE_BROADSOFT_ITEM_GROUP:
        *((ContactBroadSoftGroup *)pData) = *((ContactBroadSoftGroup *)pDataOri);
        break;

    case  DIR_TYPE_BROADSOFT_ITEM_ENTERPRISE:
        *((ContactBroadSoftEnterprise *)pData) = *((ContactBroadSoftEnterprise *)pDataOri);
        break;

    case DIR_TYPE_REMOTE_MENU:
        *((ContactRemoteMenu *)pData) = *((ContactRemoteMenu *)pDataOri);
        break;

    case DIR_TYPE_REMOTE_UNIT:
        *((ContactRemoteUnit *)pData) = *((ContactRemoteUnit *)pDataOri);
        break;
    case DIR_TYPE_REMOTE_HOTKEY:
        *((ContactRemoteHotKey *)pData) = *((ContactRemoteHotKey *)pDataOri);
        break;
    case DIR_TYPE_CLOUD_ITEM_ENTERPRISE:
        *((ContactCloudEnterprise *)pData) = *((ContactCloudEnterprise *)pDataOri);
        break;
#if IF_FEATURE_GBBOOK
    case DIR_TYPE_GB_MENU:
        *((ContactGBMenu *)pData) = *((ContactGBMenu *)pDataOri);
        break;

    case DIR_TYPE_GB_UNIT:
        *((ContactGBBookUnit *)pData) = *((ContactGBBookUnit *)pDataOri);
        break;
#endif
#if IF_FEATURE_METASWITCH_CALLLOG
    case DIR_TYPE_MTSW_LOG_GROUP: // MTSW历史记录组
        *((MTSWLogGroupRecord *)pData) = *((MTSWLogGroupRecord *)pDataOri);
        break;

    case DIR_TYPE_MTSW_CALLLOG: // MTSW历史记录
        *((MTSWCallLogRecord *)pData) = *((MTSWCallLogRecord *)pDataOri);
        break;
#endif
#if IF_FEATURE_METASWITCH_DIRECTORY
    case DIR_TYPE_METASWITCH_MENU:
        *((ContactMetaSwitchMenu *)pData) = *((ContactMetaSwitchMenu *)pDataOri);
        break;

    case DIR_TYPE_METASWITCH_ITEM_CONTACTLIST:
        *((ContactMetaSwitchContactList *)pData) = *((ContactMetaSwitchContactList *)pDataOri);
        break;

    case DIR_TYPE_METASWITCH_ITEM_EXTENTION:
        *((ContactMetaSwitchExtention *)pData) = *((ContactMetaSwitchExtention *)pDataOri);
        break;

    case DIR_TYPE_METASWITCH_ITEM_MLHG:
        *((ContactMetaSwitchMLHG *)pData) = *((ContactMetaSwitchMLHG *)pDataOri);
        break;
#endif
    case DIR_TYPE_LDAP:
        *((LdapContactData *)pData) = *((LdapContactData *)pDataOri);
        break;
    default://默认返回空
        break;
    }

    return SmartPtr(pData);
}

void CBaseContactManger::RemoveDataFromMap(int nId)
{
    MapContactInfo::iterator iter = m_mapAllInfo.find(nId);

    if (iter != m_mapAllInfo.end())
    {
        BaseDirDataRecord * pData = iter->second;

        m_contactLock.Lock();
        if (pData != NULL)
        {
            delete pData;
        }
        m_mapAllInfo.erase(iter);
        m_contactLock.Unlock();
    }
}

int CBaseContactManger::AddContact(BaseContactRecord * pContact, int nGroupId,
                                   bool isSort /* = true */, bool isOrder /* = true */, int nId /* = knInvalidRId */, bool bTransT9 /*= true*/)
{
    if (pContact->m_strNameIndex.empty() && bTransT9)
    {
        pContact->m_strNameIndex = commonAPI_transT9String(pContact->m_strDisplayName);
    }

    nId = InsertMap(pContact, nId);

    int nRet = AddIdToGroup(nId, nGroupId, isSort, isOrder);

    if (nRet != DIR_RET_SUCC)
    {
        MapContactInfo::iterator iter = m_mapAllInfo.find(nId);

        if (iter != m_mapAllInfo.end())
        {
            if (iter->second != NULL)
            {
                delete iter->second;
            }
            m_mapAllInfo.erase(iter);
        }

        return nRet;
    }

    if (m_bIsNeedAddToAllGroup
            && nGroupId != kszAllContactId
            && nGroupId != kszBlackListGroupId)
    {
        AddIdToGroup(nId, kszAllContactId, isSort, isOrder);
    }

    return nId;
}

void CBaseContactManger::CancelLoad(int nGroupId)
{
    //All 组不能够调用该接口，导致一直无法变成OK
    if (nGroupId == kszAllContactId)
    {
        return;
    }

    DirGroupRecord * pGroup = GetMenuDataById(nGroupId);
    if (pGroup != NULL)
    {
        pGroup->m_eStatus = FILE_DOWNLOAD_CANCELED;
    }
}

//Added By Yuyt! 2014/3/19
int CBaseContactManger::GetContactIdByIndex(int index, int nGroup)
{
    DirGroupRecord * pGroup = GetMenuDataById(nGroup);
    if (pGroup != NULL)
    {
        if (index >= 0 && index < pGroup->m_vChildrenId.size())
        {
            return pGroup->m_vChildrenId[index];
        }
    }
    return -1;
}

int CBaseContactManger::GetContactIndexById(int nId, int nGroup)
{
    DirGroupRecord * pGroup = GetMenuDataById(nGroup);
    if (pGroup != NULL)
    {
        for (int pos = 0; pos < pGroup->m_vChildrenId.size(); ++pos)
        {
            if (pGroup->m_vChildrenId[pos] == nId)
            {
                return pos;
            }
        }
    }

    return 0;
}

//Added By Yuyt, 2014/3/20
FileDownloadStatus CBaseContactManger::GetDownLoadStatus(int nGroupId)
{
    DirGroupRecord * pGroup = GetMenuDataById(nGroupId);

    if (pGroup != NULL)
    {
        return pGroup->m_eStatus;
    }

    return FILE_DOWNLOAD_STATUS_NONE;
}

bool CBaseContactManger::IsAllLoadFinish(int nGroupId)
{
    if (nGroupId == kszAllContactId)
    {
        for (int i = 0; i < m_vGroupId.size(); i++)
        {
            if (m_vGroupId[i] == kszAllContactId)
            {
                continue;
            }

            if (!IsAllLoadFinish(m_vGroupId[i]))
            {
                return false;
            }
        }
    }
    else
    {
        DirGroupRecord * pGroup = GetMenuDataById(nGroupId);
        if (pGroup != NULL)
        {
            if (pGroup->m_eStatus == FILE_DOWNLOADING || pGroup->m_bReqNext)
            {
                return false;
            }
            for (int i = 0; i < pGroup->m_vChildrenId.size(); i++)
            {
                BaseDirDataRecord * pData = GetDataById(pGroup->m_vChildrenId[i]);

                if (pData != NULL && pData->m_eInheritType == DIR_INHERIT_GROUP)
                {
                    if (!IsAllLoadFinish(pGroup->m_vChildrenId[i]))
                    {
                        return false;
                    }
                }
            }
        }
    }

    return true;
}

DIR_INHERIT_TYPE CBaseContactManger::GetFirstChildTypeByGroupId(int nGroupId)
{

    DirGroupRecord * pGroup = GetMenuDataById(nGroupId);

    if (pGroup == NULL || pGroup->m_vChildrenId.size() == 0)
    {
        return DIR_INHERIT_BASE;
    }

    BaseDirDataRecord * pData = GetDataById(pGroup->m_vChildrenId[0]);

    if (pData == NULL)
    {
        return DIR_INHERIT_BASE;
    }

    return pData->m_eInheritType;
}


bool CBaseContactManger::GetFastIndexListByGroupId(int nGroupId, FastIndexList & refList)
{
    if (configParser_GetConfigInt(kszNotLocalDirSortEnable) != 1 && m_eDirType != LOCAL_DIR)
    {
        return false;
    }

    DirGroupRecord * pGroup = GetMenuDataById(nGroupId);

    if (pGroup == NULL)
    {
        return false;
    }

    char cKey = '#' - 1;

    FastIndex sFastIndex;


    for (int i = 0; i < pGroup->m_vChildrenId.size(); i++)
    {
        BaseDirDataRecord * pData = GetDataById(pGroup->m_vChildrenId[i]);

        if (pData == NULL)
        {
            DIR_WARN(" empty Contact Data!");
            return false;
        }

        int nValue = GetFirstCharSortValue(pData->GetDisplayName());

        if (nValue > cKey)
        {
            sFastIndex.m_ckey = nValue;
            sFastIndex.m_nIdex = i;

            refList.push_back(sFastIndex);

            cKey = nValue;
        }
    }

    return true;
}

int CBaseContactManger::GetCompResult(int nLeftId, int nRightId)
{
    const yl::string & strLeft = GetSortKeyById(nLeftId);
    const yl::string & strRight = GetSortKeyById(nRightId);

    int nLeft = GetFirstCharSortValue(strLeft);
    int nRight = GetFirstCharSortValue(strRight);

    if (nLeftId == kszViewMoreId)
    {
        return 1;
    }
    if (nRightId ==  kszViewMoreId)
    {
        return -1;
    }

    if (nLeft == nRight)
    {
        return strcasecmp(strLeft.c_str(), strRight.c_str());
    }
    else
    {
        return nLeft - nRight;
    }
}

void CBaseContactManger::SetContactLock()
{
    DIR_INFO("CBaseContactManger::SetContactLock Lock");
    m_contactLock.Lock();
}

void CBaseContactManger::SetContactUnLock()
{
    DIR_INFO("CBaseContactManger::SetContactUnLock UnLock");
    m_contactLock.Unlock();
}


