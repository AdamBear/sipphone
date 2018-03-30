#ifndef __FWD_ENTITY_H__
#define __FWD_ENTITY_H__

#include <ylstring.h>
#include <ylhashmap.h>

#include "fwddndtypes.h"


/** FWD实体，用于管理账号模式中一个账号的FWD信息，或者全局模式中的全局FWD信息 */
class CFwdEntity
{
public:
    /**
     * 构造函数
     *
     * @param   iAccount    FWD实体对应的账号ID，全局模式为FWDDND_ACCOUNT_GLOBAL(-1)
     */
    CFwdEntity(int iAccount);
    ~CFwdEntity();

    /** 重新加载配置文件 */
    void ReloadConfig();

    /**
     * 获取账号ID
     *
     * @return  账号ID，全局模式返回FWDDND_ACCOUNT_GLOBAL(-1)
     */
    int GetAccountId() const;

    /**
     * 获取FWD信息
     *
     * @param       eType       FWD类型
     * @param [out] pstrTarget  转移的目标号码，指针为空时不获取该字段
     * @param [out] pRingDelay  响铃时间，获取无应答转移时有效，指针为空时不获取该字段
     *
     * @return  指定类型的FWD是否启用，未启用时strTarget和iRingDelay为配置值但不生效
     */
    bool GetInfo(FWD_TYPE eType, yl::string * pstrTarget = NULL,
                 int * pRingDelay = NULL) const;

    /**
     * 设置FWD信息
     *
     * @param       eType       FWD类型
     * @param       bEnabled    是否启用FWD
     * @param [in]  pstrTarget  (Optional) 转移的目标号码，指针为空时不设置该字段
     * @param [in]  pRingDelay  (Optional) 响铃时间，设置无应答转移时有效，指针为空时不设置该字段
     * @param       eSource     (Optional) 设置来源
     * @param       bSetLastSavedType   (Optional) 是否设置最后一次修改配置时启用的FWD类型
     *
     * @return  true: 设置成功; false: 设置失败
     */
    bool SetInfo(FWD_TYPE eType, bool bEnabled,
                 const yl::string * pstrTarget = NULL, int * pRingDelay = NULL,
                 FWD_CONFIG_SOURCE_TYPE eSource = FWD_CFG_SRC_TYPE_COMMON,
                 bool bSetLastSavedType = true);

    /**
     * 同时设置多个FWD开关
     *
     * @param       eType       FWD类型，支持同时指定多个FWD类型
     * @param       bEnabled    是否启用FWD
     * @param       eSource     (Optional) 设置来源
     *
     * @return  true: 设置成功; false: 设置失败
     */
    bool SetMultiInfo(int eType, bool bEnabled,
                      FWD_CONFIG_SOURCE_TYPE eSource = FWD_CFG_SRC_TYPE_COMMON);

    /**
     * 还原备份的FWD信息，并关闭FWD
     *
     * @param   eType   FWD类型
     *
     * @return  true: 还原成功; false: 还原失败
     */
    bool RestoreInfo(FWD_TYPE eType);

    /**
     * 获取最后一次修改配置时启用的FWD类型
     *
     * @return  最后一次修改配置时启用的FWD类型
     */
    int GetLastSavedType() const;

protected:
    const int m_iAccount;   // 账号ID
    bool m_bAlwaysFwdEnable;    // 无条件转移是否启用
    yl::string m_strAlwaysFwdTarget;    // 无条件转移目标号码
    bool m_bBusyFwdEnable;  // 遇忙转移是否启用
    yl::string m_strBusyFwdTarget;  // 遇忙转移目标号码
    bool m_bNoAnswerFwdEnable;  // 无应答转移是否启用
    yl::string m_strNoAnswerFwdTarget;  // 无应答转移目标号码
    int m_iRingDelay;   // The ring delay

    /**
     * 判断指定FWD类型是否开启
     *
     * @param   eType   FWD类型
     *
     * @return  是否开启FWD
     */
    bool IsEnable(FWD_TYPE eType) const;

    /**
     * 设置指定FWD类型是否开启
     *
     * @param   eType   FWD类型
     * @param   bEnable 是否开启FWD
     */
    void SetEnable(FWD_TYPE eType, bool bEnable);

    /**
     * 获取指定FWD类型的转移目标号码
     *
     * @param   eType   FWD类型
     *
     * @return  转移目标号码
     */
    const yl::string GetTarget(FWD_TYPE eType) const;

    /**
     * 获取指定FWD类型的转移目标号码
     *
     * @param   eType       FWD类型
     * @param   strTarget   转移目标号码
     */
    void SetTarget(FWD_TYPE eType, const yl::string & strTarget);

    /**
     * 判断指定FWD类型在配置文件中是否开启
     *
     * @param   eType   FWD类型
     *
     * @return  配置文件中是否开启FWD
     */
    bool IsEnableInConfig(FWD_TYPE eType) const;

    /**
     * 获取指定FWD类型在配置文件中的转移目标号码
     *
     * @param   eType   FWD类型
     *
     * @return  配置文件中的转移目标号码
     */
    yl::string GetTargetInConfig(FWD_TYPE eType) const;

    /**
     * 获取配置文件中的（无应答转移）响铃时间
     *
     * @return  响铃时间，单位：s
     */
    int GetRingDelayInConfig() const;

    /**
     * 保存指定FWD类型的配置
     *
     * @param   eType   FWD类型
     */
    void SaveInfo(FWD_TYPE eType, FWD_CONFIG_SOURCE_TYPE eSource) const;

    /**
     * 备份指定FWD类型的配置
     *
     * @param   eType   FWD类型
     */
    void BackupInfo(FWD_TYPE eType) const;

    /**
     * 通知FWD状态改变
     *
     * @param   eType   FWD类型
     * @param   eSource FWD状态改变来源
     */
    void NotifyChange(FWD_TYPE eType, FWD_CONFIG_SOURCE_TYPE eSource);

    /**
     * 设置最后一次修改配置时启用的FWD类型
     */
    void SetLastSavedType() const;
};

#endif // !__FWD_ENTITY_H__
