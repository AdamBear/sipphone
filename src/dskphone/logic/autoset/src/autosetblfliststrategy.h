#ifndef __AUTOSET_BLFLIST_STRATEGY_H__
#define __AUTOSET_BLFLIST_STRATEGY_H__
#ifdef IF_FEATURE_AUTOTEST

#ifdef IF_FEATURE_BLFLIST

#include "autosetbasestrategy.h"
#include "blf/include/modblf.h"

class CAutoSetBlfListStrategy
    : public CAutoSetBaseStrategy
{
public:
    CAutoSetBlfListStrategy(void);
public:
    virtual ~CAutoSetBlfListStrategy(void);

public:
    // 判断自动设置的开关是否有开启或者改动的dsskey是否有影响到blflist
    virtual bool IsNeedToAutoSet(msgObject & msg);
    // 生成自动设置数据
    virtual bool MakeAutoSetData(YLVector<autoset_dsskeydata_t> & vecAutoSetData);
    // 当前的dsskey类型能否被设置
    virtual bool IsAllowToSet(int iDssKeyID);
    // 设置当前dsskey所对应的远端状态
    virtual bool UpdateDssKeyStatus(int iDssKeyID, const autoset_dsskeydata_t & tAutoSetData,
                                    bool bHaveData);
    virtual bool InherentProcess(msgObject & msg) const;

    virtual bool GetDssKeyData(autoset_dsskeydata_t & tAutoSetData, DssKeyLogicData & tDsskeyData,
                               int iDssKeyID, bool bHaveAutoSetData = true);

    virtual bool OnHigherPriorityStrategyQuery(int iDsskeyID) const;

    virtual bool AfterAutoSetProcess();

    virtual AUTOSET_SEQUENCE GetSetDssKeySequence() const;

    virtual DssKey_Type GetBindDsskeyType() const
    {
        return DT_BLFLIST;
    }

    virtual bool IsEnable() const;

    // dsskey重新加载
    virtual bool OnDsskeyReload(const dsskey_change_info_t & data);

    virtual LPCSTR GetAutoSetName() const
    {
        return AUTOSET_STRATEGY_NAME_BLFLIST;
    }

    /* 是否是自动模式，区分手动模式 */
    virtual bool IsAutoSet() const
    {
        return m_bEnableAutoSetBlflist;
    }

private:
    // 重新加载配置
    void LoadConfig();
    // 自动设置账号的开关
    bool IsEnableAutoSetBlfList() const;

    bool GetManualSetDsskeyData(int iIndex, YLVector<AccountMoitorData> & refVector,
                                autoset_dsskeydata_t & tAutoSetData);

    // 自动设置blflist开关
    bool m_bEnableAutoSetBlflist;
#if IF_BUG_32812
    // 自定义设置账号的开关
    bool IsEnableCustomSetBlfList() const;

    bool GetManualSetDsskeyData(int iDssKeyID, YLVector<autoset_dsskeydata_t> & vecAutoSetData);
    // 更新Dsskey的BLFList数据
    void UpdateBLFListData(int iDssKeyID);
    // 获取自动填充dsskey
    bool GetAutoSetDataByDsskeyID(int iDssKeyID, autoset_dsskeydata_t & tAutoSetData);

    // 自定义设置blflist开关
    bool m_bEnableCustomSetBlflist;
#endif

};

#endif //IF_FEATURE_BLFLIST

#endif  // IF_FEATURE_AUTOTEST
#endif  //__AUTOSET_BLFLIST_STRATEGY_H__

