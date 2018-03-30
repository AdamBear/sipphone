#ifndef __AUTOSET_BT_STRATEGY_H__
#define __AUTOSET_BT_STRATEGY_H__

#include "autosetbasestrategy.h"

#ifdef IF_BT_SUPPORT_PHONE
class CAutoSetBTStrategy
    : public CAutoSetBaseStrategy
{
public:
    CAutoSetBTStrategy(void);
public:
    virtual ~CAutoSetBTStrategy(void) {}

public:
    // 判断是否需要设置BT
    virtual bool IsNeedToAutoSet(msgObject & msg);

    // 设置当前dsskey所对应的远端状态
    virtual bool UpdateDssKeyStatus(int iDssKeyID, const autoset_dsskeydata_t & tAutoSetData,
                                    bool bHaveData);

    // 依据填充的数据构造dsskey模块需要的数据
    virtual bool GetDssKeyData(autoset_dsskeydata_t & tAutoSetData, DssKeyLogicData & tDsskeyData,
                               int iDssKeyID, bool bHaveAutoSetData = true);

    virtual DssKey_Type GetBindDsskeyType() const
    {
        return DT_BTLINE;
    }

    virtual bool IsEnable() const
    {
        return true;
    }

    // 高优先级策略查询
    virtual bool OnHigherPriorityStrategyQuery(int iDsskeyID) const;

    // 低优先级策略查询
    virtual bool OnLowerPriorityStrategyQuery(int iDsskeyID) const;

    // 获取dsskey模块保存标志位
    virtual int GetSetDssKeyFlag(int iDsskeyID) const;

    // dsskey重新加载
    virtual bool OnDsskeyReload(const dsskey_change_info_t & data);

    // 获取绑定的dsskey位置
    int GetBindDsskeyID() const;

    /* 是否是自动模式，区分手动模式 */
    virtual bool IsAutoSet() const
    {
        return false;
    }

    /* 是否只允许存在一个key */
    virtual bool IsSingleKey() const
    {
        return true;
    }

private:
    // 蓝牙连接状态
    bool m_bBTActive;
    // 绑定的dsskey
    int  m_iBindDsskeyID;
};

#endif
#endif  //__AUTOSET_BT_STRATEGY_H__


