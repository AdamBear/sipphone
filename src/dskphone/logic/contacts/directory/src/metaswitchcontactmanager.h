#if IF_FEATURE_METASWITCH_DIRECTORY
#ifndef __METASWITCH_CONTACT_MANAGER__
#define __METASWITCH_CONTACT_MANAGER__

#include "basecontactmanager.h"
#include "metaswitch/mtswcontrol/include/mtswactioncmdbase.h"


struct ContactMetaSwitchItemData;


class CMetaSwitchContactManager : public CBaseContactManger, public CMTSWActionCmdBase
{
public:
    static CMetaSwitchContactManager * GetInstance() DECLARE_ATTR;

    //请求下载联系人
    bool ReqLoadContact(int nGroupId);

    //预加载函数，初始化,可以指定预加载组
    void PreLoad();

    //新建联系人，加入到Metaswitch联系人中
    //|refData|是加入到联系人的信息
    //返回新增联系人ID,失败返回错误类型
    //nId 不为-1则指定id
    int AddContact2ContactList(ContactMetaSwitchContactList & refData);

    // 删除Metaswitch联系人
    DIRECTORY_RET_CODE DeleteContactListContact(VectorId & vecRemoveId);

    //新建联系人，加入到Metaswitch联系人中
    //|refData|是加入到联系人的信息
    //返回失败信息，成功返回0
    DIRECTORY_RET_CODE EditContactListContact(int nId, ContactMetaSwitchContactList & refData);

    DIRECTORY_RET_CODE ClearContact(int nGroupId);


    //定时更新，UI点击更新
    //nGroupId未指定更新某一地址簿，默认值更新所有
    DIRECTORY_RET_CODE Update(int nGroupId = kszAllContactId);

    // 处理定时器消息.
    // | refMessage | 是要处理的消息.
    // 如果处理成功则返回TRUE, 否则返回FALSE.
    virtual bool OnTimeout(unsigned int uTimerID, bool bInTalk) DECLARE_ATTR;

    //号码匹配名字
    virtual bool GetContactNameByNumber(const yl::string & strNumber, yl::string & strName, bool bSync = true);

    /*上传下载错误回调函数*/
    virtual void OnRequestError(NS_TA::TaskActionErrorType eErrCode, int nType, int nId);

    /*上传下载成功回调函数*/
    virtual bool OnLoadSucProcess(CMTSWAction * pAction);

    void SortContactList();

    SORT_TYPE GetCurrentSortType();

    bool CompareMtswContact(int leftId, int rightId);

    //设置自动更新定时器
    void SetAutoUpdateTimer();

    void KillUpdateTimer();

    int IsContactNameExist(const yl::string & strName, int nGroupId, int nExceptId = knInvalidRId);

protected:

    //在pGroup的vecID里面添加nId内容，必须确保顺序一致，时间级别：n
    virtual DIRECTORY_RET_CODE AddIdToGroup(int nId, int nGroupId, bool IsSort = true,
                                            bool isOrder = true);

    virtual bool MySearch(int nGroupId, const yl::string & strKey, VectorId & output,
                          SearchMethod m_eSearchType);

    // 启动下载.
    // 如果下载成功则返回FILE_DOWNLOAD_OK, 否则返回错误码
    int LoadAllContact();

    bool PraseData(cJSON * pOjbParse, int nId);

    void ParseContact(cJSON * pOjbParse);

    void ParseGroup(cJSON * pOjbParse);

    void ParseExtention(cJSON * pOjbParse);

    void ParseMLHG(cJSON * pOjbParse);

    void ParseMLHGGroup(cJSON * pOjbParse, int nId);

    char * GetDeleteJsonString(VectorId & vecId);

#if 0 //test
    int * getTimeID()
    {
        return &m_iTimerID;
    }
#endif

protected:
    // 初始化.
    // 如果初始化成功则返回true, 否则返回false.
    virtual bool MyInit();

    //加载成功则返回true, 否则返回false.
    bool MyCreatGroupsFromConfig();

    //创建配置组节点
    void CreatConfigGroup(int nGroupId, const char * lpszConfigName);

    virtual bool IsNeedShowAllGroup();

    void SortGroup(int nGroupId);

    void SortContactByCurrentSortType();


private:
    CMetaSwitchContactManager();
    virtual ~CMetaSwitchContactManager();

private:
    SORT_TYPE m_eSortType;

    int m_iTimerID;

    YLHashMap<yl::string, int> m_mapUidToId;
};

#define g_pMetaSwitchDirManager (CMetaSwitchContactManager::GetInstance())

#endif //__METASWITCH_CONTACT_MANAGER__

#endif // IF_FEATURE_METASWITCH_DIRECTORY
