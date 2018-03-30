#ifndef __BLF_CUSTOM_DATA_MANAGER_H__
#define __BLF_CUSTOM_DATA_MANAGER_H__

#include "customdata.h"
#include <yllist.h>
#include "dsskey/include/dsskey_enumtypes.h"
#include "talk/talklogic/include/uiandlogic_common.h"

class CBLFCustomDataManager
{
public:
    static CBLFCustomDataManager & GetInstance();

    // 加载配置文件
    bool LoadFromConfigFiles();

    // 获取优先级
    int GetPriority(Dsskey_Status eDssStatus);

    // 获取灯信息
    yl::string GetLedInfo(Dsskey_Status eDssStatus);

    // 获取操作类型/值
    eBLFOperateType GetOperationType(Dsskey_Status eDssStatus, SESSION_STATE eSessState);
    yl::string GetOperationValue(Dsskey_Status eDssStatus, SESSION_STATE eSessState);

    // 内部使用：配置更新防抖加载
    void StartUpdateTime();

    // 内部使用：处理定时器消息
    bool OnTimer(UINT uTimerID);

private:
    CBLFCustomDataManager();
    virtual ~CBLFCustomDataManager();

    // 查找自定义数据
    CBLFCustomData * FindCustomData(const yl::string & strStatus);

    // Map到字符串的映射
    yl::string MapMonitorStatus(Dsskey_Status eDssStatus);
    yl::string MapSessionStatus(SESSION_STATE eSessState);

private:
    // 防抖定时器
    int  m_iTimerLoad;

    YLList<CBLFCustomData> m_lstCustomData;
};

#define g_objBLFCustomDataManager (CBLFCustomDataManager::GetInstance())

#endif
