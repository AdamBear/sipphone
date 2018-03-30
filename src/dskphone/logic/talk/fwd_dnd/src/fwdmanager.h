#ifndef __FWD_MANAGER_H__
#define __FWD_MANAGER_H__

#include <singletonclassdefine.h> // DECLARE_SINGLETON_CLASS GET_SINGLETON_INSTANCE IMPLEMENT_GETINSTANCE
#include <ETLLib.hpp> // msgObject
#include <ylvector.h>

#include "fwdentity.h"

/** FWD管理类 */
class CFwdManager
{
    DECLARE_SINGLETON_CLASS(CFwdManager)

public:
    /** FWD初始化 */
    void Init();

    /** FWD释放 */
    void Finalize();

    /**
     * FWD功能开关是否启用.
     *
     * @return  true: 启用; false: 禁用
     */
    bool IsActive() const;

    /**
     * 获取FWD模式
     *
     * @return  当前FWD模式
     */
    FWDDND_MODE GetMode() const;

    /**
     * 设置FWD模式
     *
     * @param   eMode   需要设置的FWD模式
     */
    void SetMode(FWDDND_MODE eMode);

    /**
     * 获取FWD信息
     *
     * @param       iAccount    账号ID，FWDDND_ACCOUNT_GLOBAL(-1)表示全局模式
     * @param       eType       FWD类型
     * @param [out] pstrTarget  转移的目标号码，指针为空时不获取该字段
     * @param [out] pRingDelay  响铃时间，获取无应答转移时有效，指针为空时不获取该字段
     *
     * @return  指定账号对应类型的FWD是否启用，未启用时strTarget和iRingDelay为配置值但不生效
     */
    bool GetInfo(int iAccount, FWD_TYPE eType, yl::string * pstrTarget = NULL,
                 int * pRingDelay = NULL) const;

    /**
     * 设置FWD信息
     *
     * @param       iAccount    账号ID，FWDDND_ACCOUNT_GLOBAL(-1)表示全局模式
     * @param       eType       FWD类型
     * @param       bEnabled    是否启用FWD
     * @param [in]  pstrTarget  (Optional) 转移的目标号码，指针为空时不设置该字段
     * @param [in]  pRingDelay  (Optional) 响铃时间，设置无应答转移时有效，指针为空时不设置该字段
     * @param       eSource     (Optional) 设置来源
     *
     * @return  true: 设置成功; false: 设置失败
     */
    bool SetInfo(int iAccount, FWD_TYPE eType, bool bEnabled,
                 const yl::string * pstrTarget = NULL, int * pRingDelay = NULL,
                 FWD_CONFIG_SOURCE_TYPE eSource = FWD_CFG_SRC_TYPE_COMMON);

    /**
     * 同时设置多个FWD开关
     *
     * @param       iAccount    账号ID，FWDDND_ACCOUNT_GLOBAL(-1)表示全局模式
     * @param       eType       FWD类型，支持同时指定多个FWD类型
     * @param       bEnabled    是否启用FWD
     * @param       eSource     (Optional) 设置来源
     *
     * @return  true: 设置成功; false: 设置失败
     */
    bool SetMultiInfo(int iAccount, int eType, bool bEnabled,
                      FWD_CONFIG_SOURCE_TYPE eSource = FWD_CFG_SRC_TYPE_COMMON);

    /**
     * 还原备份的FWD信息，并关闭FWD
     *
     * @param   iAccount    账号ID，FWDDND_ACCOUNT_GLOBAL(-1)表示全局模式
     * @param   eType       FWD类型
     *
     * @return  true: 还原成功; false: 还原失败
     */
    bool RestoreInfo(int iAccount, FWD_TYPE eType);

    /**
     * 判断是否允许转移到指定号码 主要进行国际转移开关限制
     *
     * @param   strTarget   转移的目标号码
     *
     * @return  是否允许转移到指定号码
     */
    static bool IsAllowForwardTo(const yl::string & strTarget);

    /**
     * 获取全局模式最后一次修改配置时启用的FWD类型
     *
     * @return  最后一次修改配置时启用的FWD类型
     */
    int GetLastSavedType() const;

private:
    /** FWD实体联合体结构 */
    union FwdEntityPtrUnion
    {
        void * pVoid;   // 指针
        CFwdEntity * pGlobalEntity; // 全局模式FWD实体
        YLVector<CFwdEntity *> * pAccountEntityList; // 账号模式FWD实体列表
    };

    FWDDND_MODE m_eMode;    // FWD模式
    FwdEntityPtrUnion m_unionEntities;  // FWD实体（指针）联合体

    /**
     * 从配置中获取FWD模式
     *
     * @return  配置中存储的FWD模式
     */
    FWDDND_MODE GetModeFromConfig() const;

    /** 创建实体 */
    void CreateEntities();

    /** 释放实体 */
    void ReleaseEntities();

    /**
     * 获取指定账号FWD实体
     *
     * @param   iAccount    账号ID，FWDDND_ACCOUNT_GLOBAL(-1)表示全局模式
     *
     * @return  FWD实体指针。如果模式不正确或者找不到对应账号，则返回NULL
     */
    CFwdEntity * GetEntity(int iAccount) const;

    /** 消息处理函数，主要处理：配置修改消息(CONFIG_MSG_BCAST_CHANGED) */
    static LRESULT OnMessage(msgObject & msg);

    /** FWD功能开关改变处理函数 实现功能开关改变时创建/释放当前模式的实体 */
    void OnActiveStatusChanged();

    /** FWD模式改变处理函数 实现模式（全局/账号）改变时切换实体 */
    void OnModeChanged();

    /** 通知实体配置改变 */
    void NotifyEntitiesConfigChanged();
};

/** FWD管理类单例 */
#define _FwdManager GET_SINGLETON_INSTANCE(CFwdManager)

#endif // !__FWD_MANAGER_H__
