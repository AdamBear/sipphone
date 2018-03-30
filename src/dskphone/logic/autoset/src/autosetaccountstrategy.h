#ifndef __AUTOSET_ACCOUNT_STRATEGY_H__
#define __AUTOSET_ACCOUNT_STRATEGY_H__
#ifdef IF_FEATURE_AUTOTEST

#include "autosetbasestrategy.h"

class CAutoSetAccountStrategy
    : public CAutoSetBaseStrategy
{
public:
    CAutoSetAccountStrategy(void);
public:
    virtual ~CAutoSetAccountStrategy(void) {}

public:
    // 判断自动设置的开关是否有开启或者改动的dsskey是否有影响到linekey
    virtual bool IsNeedToAutoSet(msgObject & msg);
    // 生成自动设置数据
    virtual bool MakeAutoSetData(YLVector<autoset_dsskeydata_t> & vecAutoSetData);
    // 设置当前dsskey所对应的远端状态
    virtual bool UpdateDssKeyStatus(int iDssKeyID, const autoset_dsskeydata_t & tAutoSetData,
                                    bool bHaveData);

    virtual bool IsDisablePassive() const;

    virtual bool PreSaveToFile(YLList<int> & listDsskey);

    virtual DssKey_Type GetBindDsskeyType() const
    {
        return DT_LINE;
    }

    // dsskey本身是否允许被占用
    virtual bool OnLowerPriorityStrategyQuery(int iDsskeyID) const;

    // dsskey重新加载
    virtual bool OnDsskeyReload(const dsskey_change_info_t & data);

protected:
    // 自动设置账号的开关
    virtual bool IsEnable() const;
    // 加载配置
    void LoadConfig();
    // 开关变更
    bool OnSwitchChanged();
    // 期望的Dsskey个数
    int GetAccLineKeyNumber(int iAccount) const;
    // 同步Label
    bool SyncLinekeyLabel(YLList<int> & listDsskey, autoset_dsskeydata_t & tData, int iAccountID);
    // 为上层提供接口
    AUTOSET_ACCOUNT_TYPE GetAutoSwitch() const
    {
        return m_eAutoSetSwitch;
    }

private:
    // 自动设置开关
    AUTOSET_ACCOUNT_TYPE            m_eAutoSetSwitch;
};

#endif  // IF_FEATURE_AUTOTEST


#endif  //__AUTOSET_ACCOUNT_STRATEGY_H__


