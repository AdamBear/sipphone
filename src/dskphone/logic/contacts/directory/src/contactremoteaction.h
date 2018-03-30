#ifndef DIRECTORY_CONTACTREMOTEACTION_H_
#define DIRECTORY_CONTACTREMOTEACTION_H_

#include "ETLLib.hpp"
#include "threadtask/include/networkfileaction.h"
#include "commondata.h"

typedef YLList<BaseDirDataRecord *> ListData;

typedef struct ContactGroupInfo
{
    yl::string m_strDisplayName;  // Group display name.
    int m_ID;  // Group ID
} ContactGroupInfo;

// Class CContactRemoteAction, 远程文件下载与解析类.
class CContactRemoteAction : public CNetworkFileAction
{
public:
    // | strLocalFilePath | 是远程文件将要存放在本地的路径.
    // | strRemoteFilePath | 是远程文件的地址.
    CContactRemoteAction(const yl::string & strLocalFilePath,
                         const yl::string & strRemoteFilePath);
    virtual ~CContactRemoteAction();

    // Override.
    virtual bool Exec();

    /***
     * @brief : Exec过程中产生的资源回收
     * @param : |bCallBackSucceed| 是否回调成功
     * @notice: 涉及异步, 只能操作自身资源
     */
    virtual bool ReleaseData(bool bCallBackSucceed)
    {
        return false;
    }

    // 解析下载完成的文件.
    // | strFileLocal | 是文件在本地的保存路径.
    // | strUrl | 是远程文件地址.
    // 如果解析成功则返回true, 否则返回false.
    bool ParseDownloadedFile(const yl::string & strFileLocal);

    //操作失败才需释放，由外部管理
    void ReleaseData();

    void SetGroupId(int nId);

    int GetGroupId()
    {
        return m_nGroupId;
    }

    ListData & GetListData()
    {
        return m_listData;
    }

    const yl::string & GetTitle()
    {
        return m_strTitle;
    }

    ContactRemoteMenu * GetNextMenuPageGroup()
    {
        return m_pNextMenu;
    }

    bool ParseLocalGroup(xml_node GRoot, YLVector<ContactGroupInfo> & vecGroup);

    bool ParseLocalContact(xml_node CRoot, YLVector<ContactGroupInfo> & vecGroup);

    void SetNeedReleaseData(bool bRelease);

private:
    //Menu式文件解析
    bool MenuParse(xml_node & NodeRoot);

    //具体联系人式文件解析
    bool UnitParse(xml_node & NodeRoot);

    //混合一次式文件解析
    bool MixParse(xml_node & NodeRoot);

#if IF_BUG_PLCMDIR
    bool PrasePlcmDir(xml_document & docParse);
#endif

    //本地联系人格式文件的解析
    bool LocalParse(xml_node & docParse);

    //解析本地联系人是判断组是否存在 返回组ID
    int FindLocalGroup(YLVector<ContactGroupInfo> & vecGroup, yl::string & strGroupName);
private:

    ListData m_listData;

    ContactRemoteMenu * m_pNextMenu;

    int m_nGroupId;

    yl::string m_strTitle;

    bool m_bNeedReleaseData;
};


#endif  // DIRECTORY_CONTACTREMOTEACTION_H_
