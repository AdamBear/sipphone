#ifndef __BROADSOFT_CONTACT_MANAGER__
#define __BROADSOFT_CONTACT_MANAGER__
#include "basecontactmanager.h"

struct ContactBroadSoftItemData;

class CBroadSoftContactManager : public CBaseContactManger
{
    typedef YLHashMap<int, ContactBroadSoftPersonal> EditMap;

public:
    static CBroadSoftContactManager * GetInstance() DECLARE_ATTR;

    //请求下载联系人
    bool ReqLoadContact(int nGroupId);

    bool ReqNextPageContact(int nGroupId, bool bSearch = false);

    //请求超级搜索联系人
    DIRECTORY_RET_CODE ReqSearchContact(int nGroupId, ContactBroadSoftItemData & refSearchData);

    //预加载函数，初始化,nGroup可以指定预加载组
    bool PreLoad(int nGroupId = knInvalidRId, bool bLoad = true);

    // 回调函数. 当一次异步读写完毕时, 会调用此函数.
    virtual void OnTaskActionCallback(CTaskAction * pTaskAction);

    //新建联系人，加入到Perosnal联系人中
    //|refData|是加入到联系人的信息
    //返回新增联系人ID,失败返回错误类型
    //nId 不为-1则指定id
    int AddContact2Personal(ContactBroadSoftPersonal & refData, int nId = knInvalidRId);

    // 新建联系人，加入到 All Contact 组
    int AddContact2AllContact(ContactGeneralItemData & refData);

    // 删除personal联系人
    DIRECTORY_RET_CODE DeletePersonalContact(VectorId & vecRemoveId,
            BSFT_SEARCH_TYPE eType = SEARCH_NONE);

    //新建联系人，加入到Perosnal联系人中
    //|refData|是加入到联系人的信息
    //返回失败信息，成功返回0
    DIRECTORY_RET_CODE EditPersonalContact(int nId, ContactBroadSoftPersonal & refData,
                                           BSFT_SEARCH_TYPE eType = SEARCH_NONE);

    //IsClearLocal为true，清空勾选设置。
    void ClearCustomList(bool IsClearLocal = false);

    //定时更新，UI点击更新
    //nGroupId未指定更新某一地址簿，默认值更新所有
    DIRECTORY_RET_CODE Update(int nGroupId = kszAllContactId);

    // 处理定时器消息.
    // | refMessage | 是要处理的消息.
    // 如果处理成功则返回TRUE, 否则返回FALSE.
    virtual bool OnTimeout(unsigned int uTimerID, bool bInTalk) DECLARE_ATTR;

    //设置自动更新定时器
    void SetAutoUpdateTimer();

    void StopAutoUpdateTimer();

    //号码匹配名字
    virtual bool GetContactNameByNumber(const yl::string & strNumber, yl::string & strName, bool bSync = true);

    //对所有数据进行排序
    void SortAllData();

    void ClearContactInfo(int nGroupID);

    bool HasNextPage(int nGroupID);

protected:
    // 初始化.
    // 如果初始化成功则返回true, 否则返回false.
    virtual bool MyInit();

    //加载成功则返回true, 否则返回false.
    bool MyCreatGroupsFromConfig();

    //创建配置组节点
    void CreatConfigGroup(int nGroupId, const char * lpszConfigName, yl::string strName,
                          const yl::string & strServer, const yl::string & strDefaultName = "");

    virtual bool IsNeedShowAllGroup();

    // 启动下载.
    // | strUrl | 是文件在服务器的地址, 也是map的key.
    // | isSync | 同步还是异步.
    // 如果下载成功则返回FILE_DOWNLOAD_OK, 否则返回错误码
    int MyExecDownload(int GroupId,
                       bool isSearch = false,
                       bool isPreLoad = false,
                       const yl::string & strStartIndex = "",
                       const yl::string & strCountPerPage = "");

    //执行删除联系人信息，本地删除，服务器删除
    bool MyExecDelete(int nId);

    //上传本地联系人文件到服务器
    bool MyExecPost(const yl::string & strUrl);

    void ResetCustomListStatus();

    //把链表中的信息保存到文件
    //strUrl为私有联系人全URl
    void SaveToFile(ContactBroadSoftPersonal & refData);

    // 从Xml文件中读取联系人.
    // | GroupId | 为id.
    // | strFilePath | 为文件全路径.
    // 读取成功则返回true, 否则返回false.
    bool MyLoadDataFromFile(int GroupId,
                            const yl::string & strFilePath, bool isSearch, bool & bReqNest);

    //解析Custom联系人服务器配置了那些子联系人组
    bool MyLoadCustomListFromFile(yl::string & strCustomList,
                                  const yl::string & strFilePath);

    // 读取个人联系人的Xml文件.
    // | strKey | 为map中的key.
    // | pElem | 为Xml的根节点.
    // 读取成功则返回true, 否则返回false.
    bool MyLoadPersonalXml(int GroupId,
                           xml_node * pElem, bool isSearch);
    // 解析组联系人的Xml文件.
    // | strKey | 为map中的key.
    // | pElem | 为Xml的根节点.
    // 解析成功则返回true, 否则返回false.
    bool MyLoadGroupXml(int GroupId,
                        xml_node * pElem, bool isSearch);

    // 解析Common联系人的Xml文件
    bool MyLoadCommonXml(int GroupId,
                         xml_node * pElem, bool isSearch);

    // 解析Enterprise联系人的Xml文件
    bool MyLoadEnterpriseXml(int GroupId,
                             xml_node * pElem, bool isSearch);

    bool MyLoadCustomXml(int GroupId,
                         xml_node * pElem, bool isSearch);

    void InsertOneContact(BaseContactRecord * pAdd, ContactBroadSoftMenu * pGroup, bool isSearch);

    //分页请求
    bool ReqNextPage(int GroupId, const xml_node & node, bool isSearch);

    bool ReqNextPage(int nGroupID, bool isSearch = false);

    void MyInsertSubCustomDir(const yl::string & strCustomList);

    //处理预加载成功
    void ProcessPreLoadSuc();

    bool AddParameters(int GroupId, ContactBroadSoftItemData & refSearchData);

    //在Url上添加一个搜索参数
    void AddUrlParameter(yl::string & strUrl, const yl::string & strName, const yl::string & strValue,
                         bool isPreMatch = false, bool isCaseSensitive = true);

    //设置搜索参数模式
    void SetSearchCriteriaModeOr(yl::string & strUrl, bool isOn);

    //是否为Personal联系人组
    inline bool IsPersonal(int GroupId);

    inline bool IsCommonDir(int GroupId);

    bool IsCustomSubDir(int GroupId);

    bool IsOnlySearchEnterprise();

    //服务器搜索前置处理，如返回false不在继续
    bool PreSearchProcess(int nId, ContactBroadSoftItemData & refSearchData);

    yl::string UrlEncode(const yl::string & strName);

    void ResetDisplayName(int nGroupID);

    void MySort(int nGroupId);

    bool ParsePageInfo(int GroupId, const xml_node& node, bool bIsSearch = false);
private:
    CBroadSoftContactManager();
    virtual ~CBroadSoftContactManager();

private:
    bool m_bIsOnlySearchEnterPrise;

    int m_nFileIndex;//为0-5，一共提供6个文件夹，防止联系人下载冲突覆盖

    yl::string m_strUser;//XSI userName

    yl::string m_strPassword;//XSI password

    yl::string m_strPort;//XSI port

    yl::string m_strServer;

    int m_iTimerID;

    int m_iInitTimer;

    EditMap m_editMap;

    ContactBroadSoftItemData m_refLastSearchData;
};

#define g_pBroadSoftDirManager (CBroadSoftContactManager::GetInstance())

#endif
