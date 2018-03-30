#ifndef _DSSKEY_DATA_MANAGER_HEADER_
#define _DSSKEY_DATA_MANAGER_HEADER_

#include <ylhashmap.h>
#include <ylvector.h>
#include <yllist.h>
#include "../include/dsskey_enumtypes.h"
#include "../include/dsskeystructure.h"
#include "dsskey_common.h"
#include "../include/dsskey_def.h"

class CDSSkeyData;
class CDSSKeyDataModule;

class DsskeyModuleInfo
{
public:
    int first;
    CDSSKeyDataModule * second;

    DsskeyModuleInfo()
    {
        second = NULL;
        first = -1;
    }
};

typedef YLList<DsskeyModuleInfo *> MapModules;
typedef MapModules::iterator MapModuleItem;

class CDSSKeyDataEntryManager
{
    friend class CBlfListProcessor;

public:
    //typedef YLHashMap<int, CDSSKeyDataModule *> MapModules;

    CDSSKeyDataEntryManager(void);
    ~CDSSKeyDataEntryManager(void);

    void clear();
public:
    // 初始化
    void Init();

    static CDSSKeyDataEntryManager & GetInstance() DECLARE_ATTR;

    //根据dsskeyID获取dsskey数据
    CDSSkeyData * GetDsskeyDataByID(int dsskeyID);

    int GetModulesSize();

    //
    CDSSKeyDataModule * GetModuleByDsskeyID(int dsskeyID);

    // 根据类型获取CDSSKeyDataModule
    CDSSKeyDataModule * GetDssKeyModule(DSSKEY_MODULE_TYPE eType, int iEXPIndex = 0);

    void GetModulesList(YLList<int> & listOutput);

    //根据模块ID，获取模块指针
    CDSSKeyDataModule * GetModuleByModuleID(int moduleID);

    MapModuleItem FindModule(int iModuleID);

    //初始化所有DSSKey管理模块
    bool InitModules();

    //获取指定模块的DssKey
    void ResetModuleDsskeyData(DSSKEY_MODULE_TYPE eType, int iEXPIndex = 0);

    void RemoveModule(DSSKEY_MODULE_TYPE eType, int iEXPIndex = 0);

    //bool GetDsskeyDatasByModuleAndType(int moduleID, DssKey_Type dsskeyType, YLVector<CDSSkeyData *> &vecOutput);

    //bool GetDSSKeyLogicData(int dsskeyID, DssKeyLogicData &dataOutput);

    //bool GetDSSKeyLogicDataList(int moduleID, int beginIndex, int iNum,   YLList<DssKeyLogicData>& listData);

    //bool SaveModuleToIni(int moduleID);

    //获取Dsskey总数
    int GetAllDsskeyNum();

    //获取模块中Dsskey个数
    int GetModuleDsskeyNum(DSSKEY_MODULE_TYPE eType, int iEXPIndex = 0);

    bool GetAllDsskeys(YLList<int> & listOutput, DssKey_Type eType = DT_NONE, bool bNative = false);

    bool GetAllDsskeyByType(YLList<int> & listOutput, DSSKEY_MODULE_TYPE eType);

    CDSSkeyData * GetDsskeyInfoByKeyIndex(int iKeyIndex);

    // 对方状态更新的消息
    void ProcessRemoteStatus(msgObject & msg);

    // 使用给定的状态数组更新CDssKeyModule上的dsskey状态
    void UpdateRemoteStatusByModule(CDSSKeyDataModule * pModule, char * lpStatus, int iNum);

    // 配置改变消息处理
    bool OnConfigChange(msgObject & msg);

    // 处理定时器消息
    bool OnTimer(UINT uTimerID) DECLARE_ATTR;

    // line状态改变消息处理
    bool OnLineStateChange(msgObject & msg);

#ifdef IF_SUPPORT_BLUETOOTH
    //处理btline 状态改变消息
    bool OnBluetoothStateChange(msgObject & msg);

    //通知中心Dsskey 状态变化
    bool OnNotifyDsskeyStateChange(msgObject & msg);
#endif

#ifdef _T49
    //查询通知中心dsskey模块状态
    void QueryNotifyModuleStatus();
#endif
    // 获取某类型的Dsskey列表
    void GetDsskeyListByType(YLVector<CDSSkeyData *> & vecDsskey, DssKey_Type eType);

    // 获取Dsskey数据
    bool GetDataForUI(int iDssKeyID, DssKeyLogicData & sData);

    //获取指定数量的DSSKey数据
    bool GetDataForUI(DSSKEY_MODULE_TYPE eType, int iBeginKey, int iNum,
                      YLList<DssKeyLogicData> & listData, int iEXPIndex = -1);

    // 更新DSSkey状态
    bool UpdateStatus(DssKey_Type eType, Dsskey_Status eStatus, const yl::string& strValue = "");
    bool UpdateStatus(CDSSkeyData * pData, Dsskey_Status eStatus, const yl::string& strValue = "");
    // 根据Fwd/Dnd的改变，更新linekey的显示
    void UpdateLinekeyLabel(int iAccount = -1, DSSKEY_MODULE_TYPE eType = DMT_LINEKEY_MODULE,
                            int iModuleIndex = 0);

    VOID ProcessCallParkNotify(int iAccountID, bool bOn);

    void GetAllDsskeyByAccID(YLList<int> & listOutput, int accID);

    // 更新Dsskey灯
    void RefreshDsskeyStatusByType(DssKey_Type eType);

    // 根据账号重置对方的状态
    void ResetRemoteStatusByAccount(int iLineId);

    // 计算指定账号设置个数
    int CountsDsskeyByAccID(int iAccountID, bool bNative = false);
    // 根据Dsskey获取Dsskey的BlaIndex
    bool GetBLAIndexByDsskey(int & iIndex, int iDsskeyID, int iAccountID);
    // 根据BlaIndex获取Dsskeyid
    int GetDsskeyByBLAIndex(int iIndex, int iAccountID);

    // 移除某模块前的处理
    void BeforeRemoveModule(DSSKEY_MODULE_TYPE eType, int iEXPIndex = 0);

#if IF_BUG_31595
    // 更新Dsskey
    void ShowXMLBrowserInfo(yl::string strXMLinfo);
#endif

#if IF_FEATRUE_PTT
    //判断dsskey是否处于长按状态
    bool IsDsskeyInLongPress(int iDsskeyID);
    //添加长按
    bool AddLongPress(int iDsskeyID);
    //释放长按
    bool RemoveLongPress(int iDsskeyID);
    //是否处于长按过程中
    bool IsExistLongPress();
#endif

#ifdef _T49
    Dsskey_Status GetCameraDSStatus(int iStatus);
    Dsskey_Status GetExtDisplayStatus();
#endif

    // 保存Dsskey信息到指定文件中
    bool SaveDsskeyInfoToFile(const yl::string & strFile, int iFileType, int nModuleID);

    // 重新加载变更的dssskey
    bool ReloadModifyDSSKeyData(DssKey_Type eType = DT_NONE);

    // 修改
    void ModifyNativeData(int iDssKeyID);

    // 释放单个数据
    void FreeNativeData(int iDssKeyID);

private:
#ifdef _T49
    //处理通话中 通知中心dsskey逻辑
    bool ProcessNotifyDsskeyInTalking(msgObject & objMsg);
#endif

    void UpdateKeysInfo(DSSKEY_MODULE_TYPE eType, char * pKeysCode, int iEXPIndex = 0);

public:
    char        m_cRemoteStatusArray[MAX_DSSANDEXP_KEYS_NUM]; // 记录当前对方的状态

private:
    MapModules  m_mapDataModules;
    bool        m_bIsNetNormal;     // 网络是否正常

#if IF_FEATRUE_PTT
    YLList<int> m_listLongPressDsskeyID;
#endif

    YLList<int> m_listModify;
};

#define _DsskeyManager CDSSKeyDataEntryManager::GetInstance()
#endif
