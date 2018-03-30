#ifndef __BLFBASE_MANAGER_H__
#define __BLFBASE_MANAGER_H__

#include <ETLLib.hpp>
#include "ylvector.h"
#include "blfcommon.h"
#include "ylhashmap.h"
#include "taskaction/taskactioncallbackbase.h"


using namespace NS_TA;

typedef YLHashMap<yl::string, BLFBaseData *> BLFDataMap;



class CBLFBaseManager : public CTaskActionCallbackBase
{

public:
    // 获取Pickup显示的内容
    void GetPickUpShowContentByKey(YLList<yl::string> & listMenuData, YLVector<int> & listDsskeyID,
                                   const yl::string & strKey);

    // 通过DsskeyID获取DialogInfo
    bool GetDialogInfoByDsskeyId(DialgInfo & refData, int nDsskeyId, int index = 0);

    // 刷新BLF&BLFList的Dsskey
    static void FlushDsskeyUI(YLList<int> & lstFlushDsskey);

    static bool FlushDsskeyUI(const int iDsskeyId, DsskeyExpRefresh dsskeyRefresh = DSS_KEY_REFRESH_ALL);
#if IF_BUG_32812
    bool GetBLFAutoSetDataBykey(const int iDsskeyId, autoset_dsskeydata_t & tAutoSetDssKeyData);
#endif
    yl::string GetPickUpDisplayName(const yl::string & strName, const yl::string & strNumber);
#if IF_FEATURE_SHOW_BLF_CALLID
    bool GetMenuList(int nDsskeyId, YLList<yl::string>& listMenu, YLList<int>& listStatus);

    bool ShowBLFMenu(int nDsskeyId);

    bool GetDialogCallType(const yl::string& strKey, DialgInfo* pDialog);
#endif
protected:
    // 获取某个监控方的数据
    BLFBaseData * GetBLFDataBykey(const yl::string & strKey);

    // 回调函数. 当一次异步读写完毕时, 会调用此函数.
    virtual void OnTaskActionCallback(CTaskAction * pTaskAction);

    // 定时器消息
    BOOL OnBLFBaseTimer(msgObject & refObj);

protected:
    // 解析回调处理函数
    virtual void ProcessTaskActionCallBack(CBLFPraseAction * pAction) = 0;

    // 根据账号清除所有数据
    bool ClearAllData(int nAccount);

    // 清除某个账号所有监控方的状态
    void ClearDialogInfoByAccId(int nAccount);

    // 移除监控方
    void RemoveIterByKey(const yl::string & strKey, int nAccount, int nDsskeyId = -1,
                         bool bListRemove = true);

    // 增加监控方
    bool AddIter(BLFBaseData * pData, int nAccount);

    // 处理状态改变
    bool ProcessStatusChange(BLFBaseData * pData, int iDsskeyID = -1);

    // 更新监控方的数据
    void UpdateDialog(BLFBaseData * pData, BLFBaseData * pOldData);

    // 刷新Dsskey
    void FlushDsskey(BLFBaseData * pData, int nDsskeyId);
    void FlushDsskey(BLFBaseData * pData);
    bool SetFlushDsskeyUITimer();

    // 记录到列表中, 为删除时服务
    void AddKeyToList(const yl::string & strKey, int nAccount);

    // 亮灯模式改变，刷新灯状态
    void FlushALLDsskeyLight();

protected:

    CBLFBaseManager();

    virtual ~CBLFBaseManager();

protected:
    //一个账号对于于一个监控方列表
    YLVector<ListMonitor> m_vBlfAccoutData;

    //监控方关键字到监控方信息映射
    BLFDataMap m_mapData;

    YLList<int> m_listFlushDsskeyID;

    mkThreadTimer m_FlushDsskeyTimer;


    eBLFMangaerType m_eManagerType;
};


#endif

