#ifndef __BASECONTACTDATA_H__
#define __BASECONTACTDATA_H__
//#include "Test.h"
#include "ETLLib.hpp"
#include "presence_inc.h"

#if FEATURE_BROADTOUCH_UC


const yl::string kszEmptyString = "";

class CBaseContactData
{
public:
    //外部获取Group联系人的接口
    virtual bool GetListByGroupId(ContactBuddyListData * pData, GroupKey & keyGroup,
                                  const yl::string & strKey = "", AttrType eSearchType = ATTR_TYPE_ALL);

    virtual int GetChildCountByGroupId(GroupKey & keyGroup);

    //通过id获取联系人详情
    //refData.m_nLocal 中指定id
    bool GetContactDetail(PresenceInfo & refData);

    _PRESENCE_TYPE GetPresenceById(int nId);

    ////处理sip消息接口
    //virtual bool ProcessMessage(msgObject & refObj) = 0;

    //获取组链表显示
    virtual bool GetGroupList(GroupList & refList);

    bool Search(ContactBuddyListData * pData, GroupKey & keyGroup, const yl::string & strKey,
                AttrType eSearchType = ATTR_TYPE_ALL);

    bool SuperSearch(const yl::string & strKey, YLVector<SuperDirData> & output);

    bool GetSuperDataById(const SuperDirData & refInIterm, DirectorySearchData & refData);

    virtual const yl::string & GetSortKeyByKey(GroupKey & strId);

    eSortType GetSortType()
    {
        return m_eSortType;
    }

    virtual bool GetNameByNumber(yl::string & strName, const yl::string & strNumber);

    bool IsSearchHit(int nId,
                     const yl::string & strKey);

public:
    CBaseContactData();

protected:

    //重置联系人相关数据
    void MyResetContactInfo();

    //重置搜索相关关键字
    void MyResetSearchKey();

    //搜索是否命中
    virtual bool IsSearchHit(PresenceInfo & refData,
                             const yl::string & strKey,
                             AttrType eSearchType = ATTR_TYPE_ALL,
                             StringMatchType eMatchType = CONTAIN_MATCH);

    //通过组id返回组信息
    GroupItem * GetGroupItemByID(GroupKey & strId);

    //通过联系人关键字返回联系人信息
    IterMapPresenceInfo MyFindInfoByKey(const yl::string & strContactId);

    IterMapPresenceInfo MyFindInfoById(int id);

    //获取一个id
    int GetId();

    //重置id，从1开始
    void ReSetId(int nPreBegin = 0);

    void InsertToMap(PresenceInfo & refData);

    void RemoveFromMapByKey(yl::string & strKey);

    void MySort(GroupItem & refData);

    void MyInsert(ListContactID & refList, GroupKey & stKey);

protected:
    MapPresenceInfo   m_mapPresenceInfo;// 记录联系人和其状态

    MapKeyToId m_mapKey;//服务器关键字到本地id的map

    MapKeyToId m_mapGroupKey;

    GroupList m_GroupList;//Favorite联系人链表，只存储Jid

    ListPresenceInfo m_searchList;

    yl::string m_strLastSearchKey;

    GroupKey m_lastGroupKey;

    //超级搜索导致的多线程锁
    chCriticalSection m_csSearch;

    PresenceType m_eType;

    eSortType m_eSortType;

private:
    //id生成器
    int m_nId;
};

#endif//FEATURE_BROADTOUCH_UC


#endif
