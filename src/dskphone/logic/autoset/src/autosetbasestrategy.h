#ifndef __AUTOSET_BASE_STRATEGY_H__
#define __AUTOSET_BASE_STRATEGY_H__

#ifdef IF_FEATURE_AUTOTEST

#include <ylvector.h>
#include <ETLLib.hpp>
#include <dsklog/log.h>
#include "autoset_def.h"
#include "service_config.h"

// 提前声明
struct DssKeyLogicData;

class CAutoSetBaseStrategy
{
public:
    CAutoSetBaseStrategy(void);

    virtual ~CAutoSetBaseStrategy(void);

public:

    // 依据填充的数据构造dsskey模块需要的数据
    virtual bool GetDssKeyData(autoset_dsskeydata_t & tAutoSetData, DssKeyLogicData & tDsskeyData,
                               int iDssKeyID, bool bHaveAutoSetData = true);

    // 策略刷新dsskey状态
    virtual bool UpdateDssKeyStatus(int iDssKeyID, const autoset_dsskeydata_t & tAutoSetData,
                                    bool bHaveData = true);

    // 消息处理入口
    virtual bool IsNeedToAutoSet(msgObject & msg);

    // dsskey是否允许被重置
    virtual bool IsAllowToSet(int iDssKeyID);

public:
    /* 策略运行前的准备操作 */

    // 构造数据
    virtual bool MakeAutoSetData(YLVector<autoset_dsskeydata_t> & vecAutoSetData)
    {
        return false;
    }

    // 固有流程处理
    virtual bool InherentProcess(msgObject & msg) const
    {
        return false;
    }

public:
    /* 策略运行完之后的清理操作 */

    // 策略运行完的附加操作
    virtual bool AfterAutoSetProcess();

    // 保存前的操作
    virtual bool PreSaveToFile(YLList<int> & listDsskey);

public:
    /* 其它策略查询接口 */

    // 其它策略查询是否可以被占用
    virtual bool IsAllowToSetByType(CAutoSetBaseStrategy * pStrategy, int iDsskeyID);

    // 低优先级策略查询
    virtual bool OnLowerPriorityStrategyQuery(int iDsskeyID) const
    {
        return false;
    }

    // 高优先级策略查询
    virtual bool OnHigherPriorityStrategyQuery(int iDsskeyID) const;

    // 自身策略查询
    virtual bool OnSelfStrategyQuery(int iDsskeyID) const
    {
        return true;
    }

public:
    /* 查询策略内部状态 */

    // 设置清除标志
    virtual void SetClearFlag(bool bFlag)
    {
        m_bClearFlag = bFlag;
    }

    // 获取清除标志
    bool GetClearFlag() const
    {
        return m_bClearFlag;
    }

    // 策略是否被动失效
    virtual bool IsDisablePassive() const
    {
        return false;
    }

    // 是否使能
    virtual bool IsEnable() const
    {
        return false;
    }

    // 获取dsskey模块保存标志位
    virtual int GetSetDssKeyFlag(int iDsskeyID) const
    {
        return DSSKEY_SET_ONLY;
    }

    // 获取策略对应的dsskey遍历顺序
    virtual AUTOSET_SEQUENCE GetSetDssKeySequence() const
    {
        return POSITIVE_SEQUENCE;
    }

    // 获取策略类型枚举值
    AUTOSET_TYPE GetAutoSetType() const
    {
        return m_eAutoSetType;
    }

    // 是否绑定
    virtual bool IsDsskeyBind(int iDsskeyID) const
    {
        return false;
    }

    /* 获取策略名称，用于排序使用 */
    virtual LPCSTR GetAutoSetName() const
    {
        return NULL;
    }

    /* 设置策略优先级 */
    void SetPriority(int value)
    {
        m_iPriority = value;
    }

    /* 获取策略优先级 */
    int GetPriority() const
    {
        return m_iPriority;
    }

    /* 是否是自动模式，区分手动模式 */
    virtual bool IsAutoSet() const
    {
        return IsEnable();
    }

    /* 是否只允许存在一个key */
    virtual bool IsSingleKey() const
    {
        return false;
    }

public:
    /* 优先级比较函数 */

    // 策略优先级比较函数
    int Compare(CAutoSetBaseStrategy * pStrategy) const;

    // 策略优先级比较函数
    int Compare(DssKey_Type eType) const;

public:
    /* 策略事件响应 */

    // dsskey重新加载
    virtual bool OnDsskeyReload(const dsskey_change_info_t & data)
    {
        return false;
    }

    // 策略绑定的Dsskey类型
    virtual DssKey_Type GetBindDsskeyType() const
    {
        return DT_NA;
    };

protected:
    // 通过dsskey类型判断是否需要触发策略
    bool IsNeedAutoSetImp(DssKey_Type eNewType, DssKey_Type eOldType);

protected:

    YLList<int>         m_listFlushDsskeyId; // 修改过的dsskey链表
    AUTOSET_TYPE        m_eAutoSetType;      // 自动设置策略类型
    // 清除标志，该标志代表清除所有自动设置的dsskey
    bool                m_bClearFlag;
    int                 m_iPriority;         // 优先级
};

#endif  //IF_FEATURE_AUTOTEST
#endif  //__AUTOSET_BASE_STRATEGY_H__


