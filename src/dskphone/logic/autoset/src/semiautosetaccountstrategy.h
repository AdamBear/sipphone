#ifndef __SEMIAUTOSET_ACCOUNT_STRATEGY_H__
#define __SEMIAUTOSET_ACCOUNT_STRATEGY_H__

#include "autosetaccountstrategy.h"

#ifdef IF_FEATRUE_MANUAL_AUTOSET
// 手自一体autoset方案，针对的是SCA 账号，Telstra需求
class CSemiAutoSetAccountStrategy
    : public CAutoSetAccountStrategy
{
public:
    CSemiAutoSetAccountStrategy(void);
    virtual ~CSemiAutoSetAccountStrategy(void) {}

    // 生成自动设置数据
    virtual bool MakeAutoSetData(YLVector<autoset_dsskeydata_t> & vecAutoSetData);

    // 开关状态
    virtual bool IsEnable() const;

    // 获取dsskey设置标志
    virtual int GetSetDssKeyFlag(int iDsskeyID) const;

    // 策略运行完的附加操作
    virtual bool AfterAutoSetProcess();

    // 是否绑定
    virtual bool IsDsskeyBind(int iDsskeyID) const;

	// 设置清除标志
	virtual void SetClearFlag(bool bFlag);
private:
    // 绑定的dsskey集合
    YLList<int> m_listBindDsskeyID;
};

#endif

#endif // __SEMIAUTOSET_ACCOUNT_STRATEGY_H__
