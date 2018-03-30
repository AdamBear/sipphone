#ifndef __DND_ENTITY_H__
#define __DND_ENTITY_H__

#include "fwddndtypes.h"

/** DND实体，用于管理账号模式中一个账号的DND状态，或者全局模式中的全局DND状态 */
class CDndEntity
{
public:
    /**
     * 构造函数
     *
     * @param   iAccount    DND实体对应的账号ID，全局模式为FWDDND_ACCOUNT_GLOBAL(-1)
     */
    CDndEntity(int iAccount);
    ~CDndEntity();

    /** 重新加载配置文件 */
    void ReloadConfig();

    /**
     * 获取账号ID
     *
     * @return  账号ID，全局模式返回FWDDND_ACCOUNT_GLOBAL(-1)
     */
    int GetAccountId() const;

    /**
     * 判断DND状态是否开启
     *
     * @return  DND是否开启
     */
    bool IsEnable() const;

    /**
     * 设置DND状态
     *
     * @param   bEnable DND是否开启
     * @param   eSource 设置来源
     *
     * @return  设置是否成功
     */
    bool SetEnable(bool bEnable, DND_CONFIG_SOURCE_TYPE eSource);

    /**
     * 备份DND状态
     */
    void Backup();

    /**
     * 还原DND状态
     */
    void Restore();

protected:
    const int m_iAccount;   // 账号ID
    bool m_bEnable; // DND是否开启
    bool m_bEnableBackup; // DND状态备份

    /**
     * 判断DND配置文件中的状态是否开启
     *
     * @return  配置文件中的DND状态是否开启
     */
    bool IsEnableInConfig() const;

    /**
     * 通知DND状态改变
     *
     * @param   eSource DND状态改变来源
     */
    void NotifyChange(DND_CONFIG_SOURCE_TYPE eSource);
};

#endif // !__DND_ENTITY_H__
