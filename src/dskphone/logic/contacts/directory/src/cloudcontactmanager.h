#ifndef __CLOUD_CONTACT_MANAGER__
#define __CLOUD_CONTACT_MANAGER__
#include "basecontactmanager.h"
#include "account/include/account_datadefine.h"
#include "commonunits/commonunits_def.h"

#ifdef IF_FEATURE_CLOUD
struct ContactBroadSoftItemData;

class CCloudContactManager : public CBaseContactManger
{
public:
    static CCloudContactManager * GetInstance() DECLARE_ATTR;

    bool PreLoad();

    void Init()
    {
        return;
    }

    bool Init(int nAccountID);

    void UnInit(bool bRefresh = false);

    //请求下载联系人
    bool ReqLoadContact(int nGroupId);

    bool ReqMd5File();

    int GetCloudAccountId()
    {
        return m_nCloudAccountID;
    }

    // 回调函数. 当一次异步读写完毕时, 会调用此函数.
    virtual void OnTaskActionCallback(CTaskAction * pTaskAction);

    //定时更新，UI点击更新
    //nGroupId未指定更新某一地址簿，默认值更新所有
    DIRECTORY_RET_CODE Update(int nGroupId = kszAllContactId);

    // 处理定时器消息.
    // | refMessage | 是要处理的消息.
    // 如果处理成功则返回TRUE, 否则返回FALSE.
    virtual bool OnTimeout(unsigned int uTimerID, bool bInTalk) DECLARE_ATTR;

    //设置自动更新定时器
    void SetAutoUpdateTimer(bool bEnable = true);

    //联系人是否有配置
    virtual bool IsDirAvailable();

protected:

    // 初始化.
    // 如果初始化成功则返回true, 否则返回false.
    virtual bool MyInit();

    virtual DIRECTORY_RET_CODE AddIdToGroup(int nId, int nGroupId, bool IsSort = true,
                                            bool isOrder = true);

    //加载成功则返回true, 否则返回false.
    bool MyCreatGroupsFromConfig();

    //创建配置组节点
    void CreatConfigGroup(int nGroupId, yl::string strName, const yl::string & strServer);

    virtual bool IsNeedShowAllGroup();

    // 启动下载.
    // | strUrl | 是文件在服务器的地址, 也是map的key.
    // | isSync | 同步还是异步.
    // 如果下载成功则返回FILE_DOWNLOAD_OK, 否则返回错误码
    int MyExecDownload(int GroupId,
                       CLOUD_OPERATION_TYPE eType = CLOUD_GET_CONTACTLIST,
                       const yl::string & strStartIndex = "",
                       const yl::string & strCountPerPage = "");


    // 从Xml文件中读取联系人.
    // | GroupId | 为id.
    // | strFilePath | 为文件全路径.
    // 读取成功则返回true, 否则返回false.
    bool MyLoadDataFromFile(int GroupId,
                            const yl::string & strFilePath, bool & bReqNest);

    bool MyLoadMD5FromFile(const yl::string & strFilePath, yl::string & strMd5);


    // 解析Enterprise联系人的Xml文件
    bool MyLoadEnterpriseXml(int GroupId, xml_node * pElem);

    void InsertOneContact(BaseContactRecord * pAdd, ContactBroadSoftMenu * pGroup);

    //分页请求
    bool ReqNextPage(int GroupId, const xml_node & node);

    //在Url上添加一个搜索参数
    void AddUrlParameter(yl::string & strUrl, const yl::string & strName, const yl::string & strValue,
                         bool isPreMatch = false, bool isCaseSensitive = true);

private:
    CCloudContactManager();
    virtual ~CCloudContactManager();

private:
    int m_nFileIndex;//为0-5，一共提供6个文件夹，防止联系人下载冲突覆盖

    yl::string m_strUser;//XSI userName

    yl::string m_strPassword;//XSI password

    yl::string m_strPort;//XSI port

    yl::string m_strServer;

    yl::string m_strMd5;

    yl::string m_strEnterPriseNum;

    int m_nCloudAccountID;

    int m_iTimerID;
};

#define g_pCloudDirManager (CCloudContactManager::GetInstance())
#endif //#ifdef IF_FEATURE_CLOUD
#endif
