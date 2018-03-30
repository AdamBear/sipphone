#ifndef __AUTOSET_MANAGER_H__
#define __AUTOSET_MANAGER_H__
#ifdef IF_FEATURE_AUTOTEST

#include <singletonclassdefine.h>
#include <ylvector.h>
#include <ETLLib.hpp>
#include "autoset_def.h"

#define AUTOSET_EXTEND_LOG

#define AUTOSET_DELAY_UPDATE  (50)

// 提前声明
class CAutoSetBaseStrategy;
struct DssKeyLogicData;

typedef YLVector<CAutoSetBaseStrategy *>::iterator VECAUTOSETITER;

class CAutoSetManager
{
    DECLARE_SINGLETON_CLASS(CAutoSetManager)
public:
    //  初始化
    void Init();

    // 自动设置处理流程
    void AutoSetProcess(msgObject & msg, int iAutoSetType = AUTOSET_ALL);

    //获取是否允许设置
    bool IsAllowToSet(CAutoSetBaseStrategy * pStrategy, int iDsskeyID);

    // 依据类型查找绑定的策略
    CAutoSetBaseStrategy * GetStrategyByDsskeyType(DssKey_Type eDskType);

    // 遍历Dsskey模块 策略->模块
    void TraversalDssKeyModule(CAutoSetBaseStrategy * pStrategy, YLList<int> & listDsskey);
    // 分策略遍历Dsskey信息
    void TraversalDssKeyModule(CAutoSetBaseStrategy * pStrategy);
    // 分模块遍历Dsskey信息
    void TraversalDssKeyModule(AUTOSET_SEQUENCE eSequence, YLList<int> & listDsskey,
                               DSSKEY_MODULE_TYPE eModuleType, int nModuleIndex = 0, int iPageNum = -1);

    // 流程开始
    void AutoSetProcess();

    // 异步操作
    void DelayUpdate(UINT uType = AUTOSET_NON, int iTimer = AUTOSET_DELAY_UPDATE);

    // 定时器处理
    bool OnTimer(UINT uTimer);

private:
    // 消息处理函数
    static LRESULT OnAutoSetMessage(msgObject & msg);

    // 初始化自动设置类
    void InitAutoSetObj();
    // 根据类型创建AutoSetStrategy
    CAutoSetBaseStrategy * CreateAutoSetStrategy(AUTOSET_TYPE eType);

    // 获取自动设置数据
    bool GetAutoSetData(YLVector<autoset_dsskeydata_t> & vecAutoSetData, int & iListIndex,
                        autoset_dsskeydata_t & tAutoSetData);
    bool GetAutoSetData(int iDsskeyID, YLVector<autoset_dsskeydata_t> & vecAutoSetData,
                        autoset_dsskeydata_t & tAutoSetData);

    // 释放资源
    void Release();

    // 停止异步操作
    void StopAsync();

    // 按优先级排序
    static bool StrategyCompare(const CAutoSetBaseStrategy *l, const CAutoSetBaseStrategy * r);

    // 是否允许运行
    static bool IsAllowToRun();

private:
    // 自动设置策略对象
    YLVector<CAutoSetBaseStrategy *> m_vecAutoSetStrategy;
    // 通话结束后再去刷新标志
    UINT                m_iAutoSetType;
    // 是否上电第一次运行
    bool				m_bFirstRunging;
};

#define _AutoSetManager GET_SINGLETON_INSTANCE(CAutoSetManager)

#endif  // IF_FEATURE_AUTOTEST
#endif  //__AUTOSET_MANAGER_H__


