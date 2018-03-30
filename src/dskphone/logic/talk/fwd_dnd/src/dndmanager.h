#ifndef __DND_MANAGER_H__
#define __DND_MANAGER_H__

#include <singletonclassdefine.h> // DECLARE_SINGLETON_CLASS GET_SINGLETON_INSTANCE IMPLEMENT_GETINSTANCE
#include <ETLLib.hpp> // msgObject
#include <ylvector.h>

#include "dndentity.h"

/** DND管理类 */
class CDndManager
{
    DECLARE_SINGLETON_CLASS(CDndManager)

public:
    /** DND初始化 */
    void Init();

    /** DND释放 */
    void Finalize();

    /**
     * DND功能开关是否启用
     *
     * @return  true: 启用; false: 禁用
     */
    bool IsActive() const;

    /**
     * 获取DND模式
     *
     * @return  当前DND模式
     */
    FWDDND_MODE GetMode() const;

    /**
     * 设置DND模式
     *
     * @param   eMode   需要设置的DND模式
     */
    void SetMode(FWDDND_MODE eMode);

    /**
     * 指定账号是否启用DND
     *
     * @param   iAccount    账号ID，FWDDND_ACCOUNT_GLOBAL(-1)表示全局模式
     *
     * @return  true: 启用; false: 禁用
     */
    bool IsEnable(int iAccount) const;

    /**
     * 设置指定账号的DND状态
     *
     * @param   iAccount    账号ID，FWDDND_ACCOUNT_GLOBAL(-1)表示全局模式
     * @param   bEnable     True to enable, false to disable
     * @param   eSource     设置来源
     *
     * @return  true: 设置成功; false: 设置失败
     */
    bool SetEnable(int iAccount, bool bEnable, DND_CONFIG_SOURCE_TYPE eSource);

    /**
     * 备份（所有账号）当前DND状态
     */
    void Backup();

    /**
     * 还原（所有账号）DND状态到上一次备份
     */
    void Restore();

private:
    /** DND实体联合体结构 */
    union DndEntityPtrUnion
    {
        void * pVoid;   // 指针
        CDndEntity * pGlobalEntity; // 全局模式DND实体
        YLVector<CDndEntity *> * pAccountEntityList; // 账号模式DND实体列表
    };

    FWDDND_MODE m_eMode;    // DND模式
    DndEntityPtrUnion m_unionEntities;  // DND实体（指针）联合体

    /**
     * 从配置中获取DND模式.
     *
     * @return  配置中存储的DND模式.
     */
    FWDDND_MODE GetModeFromConfig() const;

    /** 创建实体 */
    void CreateEntities();

    /** 释放实体 */
    void ReleaseEntities();

    /**
     * 获取指定账号DND实体
     *
     * @param   iAccount    账号ID，FWDDND_ACCOUNT_GLOBAL(-1)表示全局模式
     *
     * @return  DND实体指针。如果模式不正确或者找不到对应账号，则返回NULL。
     */
    CDndEntity * GetEntity(int iAccount) const;

    /** 消息处理函数，主要处理：配置修改消息(CONFIG_MSG_BCAST_CHANGED) */
    static LRESULT OnMessage(msgObject & msg);

    /** DND功能开关改变处理函数，实现功能开关改变时创建/释放当前模式的实体 */
    void OnActiveStatusChanged();

    /** DND模式改变处理函数，实现模式（全局/账号）改变时切换实体 */
    void OnModeChanged();

    /** 通知实体配置改变 */
    void NotifyEntitiesConfigChanged();
};

/** DND管理类单例 */
#define _DndManager GET_SINGLETON_INSTANCE(CDndManager)

#endif // !__DND_MANAGER_H__
