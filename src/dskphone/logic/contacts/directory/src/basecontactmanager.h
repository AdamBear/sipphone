#ifndef __BASE_CONTACT_MANAGER_H__
#define __BASE_CONTACT_MANAGER_H__

#include "taskaction/taskactioncallbackbase.h"
#include <yllist.h>
#include <ylvector.h>
#include <ylhashmap.h>
#include "dirdefine.h"
#include "dsklog/log.h"
#include "contacts/directory/include/commondata.h"
#include "contacts/directory/include/directoryenumtypes.h"
#include "contacts/directory/include/directorystructure.h"

#define kszIDBegin kszFreeID//随机id开始，前面id预留特殊ID

const yl::string  kszEmptyString = "";

using namespace NS_TA;


typedef YLHashMap<int, BaseDirDataRecord *> MapContactInfo;

class CContactSearchAction;


class CBaseContactManger : public CTaskActionCallbackBase
{
public:
    CBaseContactManger();
    virtual~CBaseContactManger();

    //获取联系人列表数据
    virtual bool GetContactListByGroupId(ContactListData * pData, const int nGroupId);

    //本地搜索接口，异步搜索
    virtual bool LocalSearch(const yl::string & strSearchKey, const int nGroupId);

    //获取搜索结果数据，UI搜索搜索结果成功返回之后，调用该接口获取数据
    virtual bool GetSearchResultList(const yl::string & strSearchKey, ContactListData * pData);

    //获取联系人详情
    virtual SmartPtr GetDetailById(int nId);

    //通过ID获得父ID
    int GetParentIdById(int nId);

    // 获取最外层组的组名列表.
    // | refData | [out] 是buffer.
    // 如果获取成功则返回true, 否则返回false.
    virtual bool GetConfigMenuNameList(YLList<yl::string> & refData);

    //获取最外层组的id列表
    virtual bool GetConfigMenuIdList(VectorId & output);

    //号码匹配名字
    virtual bool GetContactNameByNumber(const yl::string & strNumber, yl::string & strName, bool bSync = true);

    //联系人是否有配置
    virtual bool IsDirAvailable();

    // 回调函数. 当一次异步读写完毕时, 会调用此函数.
    virtual void OnTaskActionCallback(CTaskAction * pTaskAction);

    //通过传入id获取对应数据显示名
    const yl::string & GetDisplayNameById(int nId);

    //通过ID获取排序字段
    virtual const yl::string & GetSortKeyById(int nId);

    //递归删除某个组下所有联系人数据
    virtual bool ClearContactByGroupID(int nGroupId, bool bClearPageID = false);

    void ClearChildIDListByGroupID(int nGroupID);

    //通过组ID获取其下联系人或者分组总数。
    // 不获取多级结构下所有数目
    virtual int GetChildListCount(int nGroupId);

    //通过组ID获取其下联系人总数
    int GetContactCount(int nGroupId);

    //使用组属性FileDownloadStatus才需使用该接口
    void CancelLoad(int nGroupId);

    //通过index获取Id，失败返回-1  ***Added By Yuyt! 2014/3/19
    virtual int GetContactIdByIndex(int index, int nGroup);

    virtual int GetContactIndexById(int nId, int nGroup);

    DIR_INHERIT_TYPE GetFirstChildTypeByGroupId(int nGroupId);

    bool GetFastIndexListByGroupId(int nGroupId, FastIndexList & refList);

    //通过组Id返回下载状态 Added by Yuyt, 2014/03/20.
    FileDownloadStatus GetDownLoadStatus(int nGroupId);

    DirGroupRecord * GetMenuDataById(int nId);

    bool IsAllLoadFinish(int nGroupId);

    int GetCompResult(int nLeftId, int nRightId);

    void SetContactLock();
    void SetContactUnLock();

    //设计异步线程接口
public:
    //bool SuperSearch(SuperSearchItemListData* pData);

    bool SuperSearch(const yl::string & strKey, YLVector<SuperDirData> & output,
                     StringMatchType eMatchType);

    bool IsSearchHit(int nId, const yl::string & strSearchKey, SearchMethod searchMethod,
                     StringMatchType eMatchType);

    bool GetSuperDataById(const SuperDirData & refInIterm, DirectorySearchData & refData,
                          bool bGetPhoto = true);

protected:

    virtual bool MySearch(int nGroupId, const yl::string & strKey, VectorId & output,
                          SearchMethod m_eSearchType);

    bool MySearchFromResult(const yl::string & strKey, VectorId & output, SearchMethod m_eSearchType);

protected:

    virtual bool MyInit();

    virtual void MyFinal();

    // 将数据拷贝到buffer中.
    // | pData | [out] 是buffer.
    // | pItemList | 为item链表.
    // 拷贝成功则返回true, 否则返回false.
    bool MyCopyList(ContactListData * pData, int nId);

    BaseDirDataRecord * GetDataById(int nId);

    BaseContactRecord * GetBaseContactByNumber(const yl::string & strNumber,
            const yl::string & strName = "", bool bSync = true);

    //删除记录，必须确保顺序一致，时间级别：n
    void RemoveRecord(const VectorId & vecRemoveId, int nParentId = knInvalidRId);

    //在pGroup的vecID里面删除vecRemoveId内容，必须确保顺序一致
    void RemoveIdFromGroup(const VectorId & vecRemoveId, DirGroupRecord * pGroup);

    void RemoveIdFromGroup(const VectorId & vecRemoveId, int nGroupId);

    //在pGroup的vecID里面添加nId内容，必须确保顺序一致，时间级别：n
    virtual DIRECTORY_RET_CODE AddIdToGroup(int nId, int nGroupId, bool IsSort = true,
                                            bool isOrder = true);

    //在map中添加一条记录，并返回其id
    int InsertMap(BaseDirDataRecord * pData, int nId = knInvalidRId);

    //to do syl
    //获取一个id
    int GetId();

    //重置id
    void ReSetId(int nPreBegin = kszIDBegin);

    virtual bool IsNeedShowAllGroup() = 0;

    void ClearMap();

    //清空所有联系人数据
    void ClearAllContactFromMap(int ExceptGroupId1 = knInvalidRId,
                                int ExceptGroupId2 = knInvalidRId);

    SmartPtr TransferToSmartPtr(BaseDirDataRecord * pDataOri);

    void AddIdToMenuShowList(int nGroupId);

    // 保存组名和组ID到临时文件中，以供网页使用
    void SaveGroupInfoToTempFile(const yl::string & strFileName, const yl::string & stdNodeName);

    //重置本地搜索关键字
    void ResetSearchKey();

    int AddContact(BaseContactRecord * pContact, int nGroupId, bool isSort = true, bool isOrder = true,
                   int nId = knInvalidRId, bool bTransT9 = true);

    void RemoveDataFromMap(int nId);
protected:

    MapContactInfo m_mapAllInfo;//存储所有联系人及组信息

    YLVector<int> m_vGroupId;//最外层组Id

    yl::string m_strSearchkey;

    int m_nSearchGroup;

    //id生成器
    int m_nId;

    bool m_bIsNeedAddToAllGroup;

    CContactSearchAction * m_pSearchAction;

    // 线程锁!
    chCriticalSection m_contactLock;

    DIR_TYPE m_eDirType;

    friend class CContactSearchAction;
};



#endif//__BASE_CONTACT_MANAGER_H__
