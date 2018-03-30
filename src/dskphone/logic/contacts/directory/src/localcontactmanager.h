#ifndef __LOCAL_CONTACT_MANAGER__
#define __LOCAL_CONTACT_MANAGER__
#include "basecontactmanager.h"

struct ContactGroupData;

struct ContactGeneralItemData;

class CLocalContactManager : public CBaseContactManger
{
public:

    static CLocalContactManager * GetInstance() DECLARE_ATTR;

    void Init();

    //号码匹配名字
    virtual bool GetContactNameByNumber(const yl::string & strNumber, yl::string & strName, bool bSync  = true);

    const yl::string & GetContactNameByNumber(const yl::string & strNumber);

    const yl::string & GetContactImageByNumber(const yl::string & strNumber,
            const yl::string & strName = "");

    // 将联系人和组导出到指定路径.
    // | strPath | 是指定的导出路径.
    // 如果导出成功则返回true, 否则返回false.
    bool ExportContactAndGroup(yl::string strPath = "");

    //添加一个组
    //成功返回id,失败返回失败类型DIRECTORY_RET_CODE
    int AddGroup(const ContactGroupData & groupData, bool bImPort = false);

    //更新组
    int UpdateGroup(int groupID, const ContactGroupData & groupData);

    //删除组
    bool RemoveGroup(int groupID, bool isOutClear = false);

    //清空组
    void ClearGroup();

    //添加一条联系人，nGroupId为组Id，bSetDirty是否保存联系人到文件，返回联系人Id值或错误码
    int AddLocalContact(const ContactGeneralItemData & refContactData, int nGroupId, bool bSave = true);

    //批量添加联系人，返回成功添加个数，或则错误码
    int AddLocalContactList(YLList<ContactGeneralItemData> & refList, int nGroupId);

    //vecRemoveId为删除联系人id列，nParentId为组Id，可为All组ID
    bool RemoveContacts(const VectorId & vecRemoveId, int nParentId);

    //清空某组下所有联系人,All组清空除黑名单外所有联系人
    void ClearContact(int nGroupId);

    //更新联系人，refContactData为更新后联系人数据，nGroupId为组ID，nId为联系人ID，返回联系人Id值或错误码
    int UpdateContact(const ContactGeneralItemData & refContactData, int nGroupId, int nId,
                      bool bSaveToFile = true);

#if IF_FEATURE_BACKUP_DIRECTORY
    bool IsLocalDirSyncEnable();

    void RemoevFileFromPath(yl::string & strPath);
    bool GetUploadPath(yl::string & strPath);

    // 回调函数. 当一次异步读写完毕时, 会调用此函数.
    virtual void OnTaskActionCallback(CTaskAction * pTaskAction);

    bool UploadBackupDir();

    bool IsNetWorkChangeNeedUpload()
    {
        return bNetConNeedUpload;
    }
#endif //IF_FEATURE_BACKUP_DIRECTORY

#if IF_FEATURE_FAVORITE_DIR
    bool IsFavoriteEnable();

    bool IsContactInFavorite(const int & nContactID);

    bool IsFavoriteIndexExist(const int & nIndex, int nContactID = 0);

    int CreatFavoriteIndex();

    int GetFavoriteMaxIndex();
#endif //IF_FEATURE_FAVORITE_DIR

    //对所有数据进行排序
    void SortAllData();

    void ReloadContact();

    bool GetDivertNumberByNumber(const yl::string & strNum, yl::string & strDivertNum);

    //是否是黑名单号码
    bool IsBlackContactNumber(const yl::string & strNum);

    // 设置联系人文件有修改的标志,1为修改联系人信息，2为修改组，3为二者都修改
    void SetContactUpdateMark(int iUpdateType);

    int GetContactUpdateMark()
    {
        return m_iContactUpdateMark;
    }

    int GetGroupIdByGroupName(const yl::string & strName);

    bool AfterImport(bool bNeedUpload = true);

    bool BeforeImport(bool isAppend);

    const yl::string & GetRingByNumber(const yl::string & strNum);

    DIRECTORY_RET_CODE MoveToGroup(YLVector<int> & vecContactID, int nGroupID, int nGroupFromId);

#if IF_FEATURE_FAVORITE_DIR
    DIRECTORY_RET_CODE CopyContactToFavorite(int nContactID, int nFavoriteIndex = 0);

    bool RemoveContactsIdFromFavorite(const VectorId& vecRemoveId, int nParentID);

    bool IsUpdateContactNeedSetLineKey(LocalDirContactRecord * pContact,
                                       const ContactGeneralItemData & refContactData);
#endif

public:

    void InsertToNumMap(const yl::string & strNumber, const yl::string & strName);

    int IsContactNameExist(const yl::string & strName, int nGroupId, int nExceptId = knInvalidRId);

protected:

    virtual bool MyInit();

    void InitBeforeLoadData();

    //解析组
    bool ParseGroup(const yl::string & strFilePath);

    //解析联系人
    bool ParseContact(const yl::string & strFilePath);

    // 创建contact_group_list.xml
    // |path| 文件保存路径
    // 成功返回true，否则false
    bool CreateContactGroupList(const yl::string & path);

    bool IsGroupNameUsable(const yl::string & strGroupName, int nExceptId = knInvalidRId);

    virtual bool IsNeedShowAllGroup();

    //联系人数量是否超过限制
    bool EnableToInsert(int groupID, int nAdd = 1);

    virtual void MySort(int nGroupId);

    //把联系人数据写到文件
    void SaveContactToFile(bool bNeedUpload = true);

    //把组数据写到文件
    void SaveGroupToFile(bool bNeedUpload = true);

    //构建组文件根节点
    void CreatRootGroupNode(xml_node & rootGroup);

    //构建联系人文件根节点
    void CreatRootContactNode(xml_node & rootContact);

    //通过Id删除号码到名字map中的内容
    void RemoveFromNumMapById(int nId);

#if IF_FEATURE_FAVORITE_DIR
    void NotifyFavoriteChange();

    //在pGroup的vecID里面添加nId内容，必须确保顺序一致，时间级别：n
    virtual DIRECTORY_RET_CODE AddIdToGroup(int nId, int nGroupId, bool IsSort = true,
                                            bool isOrder = true);
#endif


private:
    CLocalContactManager(void);

    virtual ~CLocalContactManager(void);

private:

    IdChangeMap m_MapIdChange;//组名到Id的转换map

    MapNumberToName m_MapNumToName;

    int m_iContactUpdateMark;

#if IF_FEATURE_BACKUP_DIRECTORY
    bool bNetConNeedUpload;

    int m_nFileIndex;
#endif
};

#define g_pLocalDirManager (CLocalContactManager::GetInstance())

#endif
