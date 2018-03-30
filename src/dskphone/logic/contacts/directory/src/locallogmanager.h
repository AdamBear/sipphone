#ifndef __LOCALLOG_MANAGER_H__
#define __LOCALLOG_MANAGER_H__
#include "basecontactmanager.h"
#include "account/include/modaccount.h"

struct LocalLogItemRecord;

class CLocalLogManager : public CBaseContactManger
{
public:
    static CLocalLogManager * GetInstance() DECLARE_ATTR;

    bool Init();

    //获取联系人列表数据
    virtual bool GetContactListByGroupId(ContactListData * pData, const int nGroupId);

    // 增加一条通话记录.
    // | refData | 是要添加的通话记录.
    // 添加成功返回记录的ID, 否则返回knInvalidRId.
    int AddLog(const LocalLogItemRecord & refData);

    // 删除一条或多条通话记录.必须确保顺序，否则会删除出错
    // | vecId | 是需通话记录的iD.nGroupId 组id
    // 删除成功返回true, 否则返回false.
    bool RemoveLog(VectorId & vecId, int nGroupId);

    //清空某类别下的历史记录
    bool ClearLogs(int nGroupId, int nAccountID = -1);


    //通过ID获取排序字段
    virtual const yl::string & GetSortKeyById(int nId);

    virtual SmartPtr GetDetailById(int nId);

    //获取上次呼出的号码和呼出号码的账号id
    bool GetLastOutCallNumAndAccountID(yl::string & strNumber, int & nAccountId);

    // 定时器
    bool OnTimer(unsigned int uTimerID) DECLARE_ATTR;

    // 保存历史记录到配置文件中
    bool SaveToFile(yl::string strPath = "");

    bool IsCloudCallLog(int nId);

    void Reload();

    // 通过手柄获取该手柄的历史记录
    // 主要是获取手柄的账号，通过账号获取历史记录
    bool GetCallLogByReqID(ContactListData * pData, const int nGroupId);

    bool ExportContactToFile(yl::string strFile);

#if IF_FEATURE_MUTI_CALLLOG
    int GetLastMissAccountID();
#endif

    //设计异步线程接口
public:
    //获取最近呼出最平凡的历史记录
    bool SearchCallLogRecentOutCall(SuperSearchItemListData * pData);

    bool ImportCalllog(yl::string strFilePath = "");

    void ClearNotifyListByID(const int nGroupId, REQUEST_ID eReqID = DEFAULT_ID);
protected:

    // 初始化组
    void InitGroup();

    //创建配置组节点
    void CreatConfigGroup(int nGroupId, yl::string strName, bool isShow);

    // 读取并解析历史记录保存文件
    bool ParseDataFromFile(yl::string strPath = "");

    //在pGroup的vecID里面添加nId内容，必须确保顺序一致，时间级别：n
    virtual DIRECTORY_RET_CODE AddIdToGroup(int nId, int nGroupId, bool IsSort = true,
                                            bool isOrder = true);

    // 设置改动标识
    void SetDirty(bool bDirty);

    // 发消息通知状态改变
    // | nType | 是通话类型, 为CALL_TYPE枚举值.
    void NotifyStatusChange(int nGroupId);

    // 清空未接来电列表或者转接来电列表
    void ClearNotifyList(int nGroupId, int nAccountID = -1);

    virtual bool IsNeedShowAllGroup();

private:
    //删除一条或者多条不保存的MissCall或者FWD历史记录数据
    bool RemoveNoSaveNotifyRecord(VectorId & vecId, int nGroupId);

    int ProcessMergeLog(const LocalLogItemRecord & refData);

private:
    CLocalLogManager();
    virtual ~CLocalLogManager();

private:
    // 是否有改动
    bool m_bIsDirty;

    // 定时器
    int m_iTimerID;

    // 不保存历史记录时的misscall id列表
    YLList<int> m_listNoSaveMissId;
    // 不保存历史记录时的FWD id列表
    YLList<int> m_listNoSaveFWDId;
    // 不保存历史记录时候呼出的历史记录
    LocalLogItemRecord m_stNoSavePlacedLog;
};

#define g_pLocalLogManager (CLocalLogManager::GetInstance())

#endif
