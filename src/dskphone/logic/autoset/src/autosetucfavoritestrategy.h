#ifndef __AUTOSET_UCFAVORITE_STRATEGY_H__
#define __AUTOSET_UCFAVORITE_STRATEGY_H__

#ifdef IF_FEATURE_AUTOTEST
#if FEATURE_BROADTOUCH_UC
#include "autosetbasestrategy.h"

class CAutoSetUCFavoriteStrategy
    : public CAutoSetBaseStrategy
{
public:
    CAutoSetUCFavoriteStrategy(void);
public:
    virtual ~CAutoSetUCFavoriteStrategy(void);

public:
    // 判断自动设置的开关是否有开启或者改动的dsskey是否有影响到
    virtual bool IsNeedToAutoSet(msgObject & msg);

    // 生成自动设置数据
    virtual bool MakeAutoSetData(YLVector<autoset_dsskeydata_t> & vecAutoSetData);

    // 设置当前dsskey所对应的远端状态
    virtual bool UpdateDssKeyStatus(int iDssKeyID, const autoset_dsskeydata_t & tAutoSetData,
                                    bool bHaveData);

    virtual DssKey_Type GetBindDsskeyType() const
    {
        return DT_UC_FAVORITE;
    }
    // 自动设置的开关
    virtual bool IsEnable() const
    {
        return true;
    }

    virtual bool OnHigherPriorityStrategyQuery(int iDsskeyID) const;

    // 当前的dsskey类型能否被设置
    virtual bool IsAllowToSet(int iDssKeyID);

    // 获取策略对应的dsskey遍历顺序
    virtual AUTOSET_SEQUENCE GetSetDssKeySequence() const;

    virtual LPCSTR GetAutoSetName() const
    {
        return AUTOSET_STRATEGY_NAME_UC_FAVORITE;
    }

    /* 是否是自动模式，区分手动模式 */
    virtual bool IsAutoSet() const
    {
        return m_bEnableAuto;
    }

private:
    void LoadConfig();

private:
    bool                      m_bEnableAuto; // 是否开启自动设置
};

#endif // FEATURE_BROADTOUCH_UC
#endif  //__AUTOSET_UCFAVORITE_STRATEGY_H__
#endif


