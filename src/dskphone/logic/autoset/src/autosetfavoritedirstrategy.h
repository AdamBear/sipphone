#ifndef __AUTOSET_FAVORITE_DIR_STRATEGY_H__
#define __AUTOSET_FAVORITE_DIR_STRATEGY_H__

#include "autosetbasestrategy.h"
#include "contacts/directory/include/commondata.h"

#if IF_FEATURE_FAVORITE_DIR
struct LocalDirContactRecord;

class CAutoSetFavoriteDirStrategy
    : public CAutoSetBaseStrategy
{
public:
    CAutoSetFavoriteDirStrategy(void);

    virtual ~CAutoSetFavoriteDirStrategy(void) {}

public:
    // 判断自动设置的开关是否有开启或者改动的dsskey是否有影响到linekey
    virtual bool IsNeedToAutoSet(msgObject & msg);
    // 生成自动设置数据
    virtual bool MakeAutoSetData(YLVector<autoset_dsskeydata_t> & vecAutoSetData);
    // 判断开关
    virtual bool IsEnable() const;

    // 策略绑定的Dsskey类型
    virtual DssKey_Type GetBindDsskeyType() const
    {
        return DT_SPEEDDIAL;
    };

    // 获取策略对应的dsskey遍历顺序
    virtual AUTOSET_SEQUENCE GetSetDssKeySequence() const;

    virtual LPCSTR GetAutoSetName() const
    {
        return AUTOSET_STRATEGY_NAME_FAVORITE;
    }

protected:
    // 加载配置
    void LoadConfig();
    // 开关变更
    bool OnSwitchChanged();

    static LocalDirContactRecord * GetLocalContact(SmartPtr & pDetail);

private:

    bool m_bAutoSetSwitch;
};
#endif
#endif  //__AUTOSET_FAVORITE_DIR_STRATEGY_H__
