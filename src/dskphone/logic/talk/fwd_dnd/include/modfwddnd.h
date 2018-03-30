#ifndef __MOD_FWD_DND_H__
#define __MOD_FWD_DND_H__

#include <ylstring.h>
#include "fwddndtypes.h"

/** FWD-DND模块初始化 */
void fwddnd_Init();

/**
 * DND功能开关是否启用
 *
 * @return  true: 启用; false: 禁用
 */
bool dnd_IsActive();

/**
 * 获取DND模式
 *
 * @return  当前DND模式
 */
FWDDND_MODE dnd_GetMode();

/**
 * 设置DND模式
 *
 * @param [in]  eMode   需要设置的DND模式
 */
void dnd_SetMode(FWDDND_MODE eMode);

/**
 * 指定账号是否启用DND
 *
 * @param [in]  iAccount    账号ID，FWDDND_ACCOUNT_GLOBAL(-1)表示全局模式
 *
 * @return  true: 启用; false: 禁用
 */
bool dnd_IsEnable(int iAccount);

/**
 * 设置指定账号的DND状态
 *
 * @param [in]  iAccount    账号ID，FWDDND_ACCOUNT_GLOBAL(-1)表示全局模式
 * @param       bEnable     True to enable, false to disable
 * @param [in]  eSource     (Optional) 设置来源
 *
 * @return  true: 设置成功; false: 设置失败
 */
bool dnd_SetEnable(int iAccount, bool bEnable,
                   DND_CONFIG_SOURCE_TYPE eSource = DND_CFG_SRC_TYPE_COMMON);

/**
 * 备份（所有账号）当前DND状态
 */
void dnd_Backup();

/**
 * 还原（所有账号）DND状态到上一次备份
 */
void dnd_Restore();

/**
 * FWD功能开关是否启用
 *
 * @return  true: 启用; false: 禁用
 */
bool fwd_IsActive();

/**
 * 获取FWD模式
 *
 * @return  当前FWD模式
 */
FWDDND_MODE fwd_GetMode();

/**
 * 设置FWD模式
 *
 * @param [in]  eMode   需要设置的FWD模式
 */
void fwd_SetMode(FWDDND_MODE eMode);

/**
 * 获取FWD信息
 *
 * @param [in]  iAccount    账号ID，FWDDND_ACCOUNT_GLOBAL(-1)表示全局模式
 * @param [in]  eType       FWD类型
 * @param [out] pstrTarget  转移的目标号码，指针为空时不获取该字段
 * @param [out] pRingDelay  响铃时间，获取无应答转移时有效，指针为空时不获取该字段
 *
 * @return  指定账号对应类型的FWD是否启用，未启用时strTarget和iRingDelay为配置值但不生效
 */
bool fwd_GetInfo(int iAccount, FWD_TYPE eType, yl::string * pstrTarget = NULL,
                 int * pRingDelay = NULL);

/**
 * 设置FWD信息
 *
 * @param [in]  iAccount    账号ID，FWDDND_ACCOUNT_GLOBAL(-1)表示全局模式
 * @param [in]  eType       FWD类型
 * @param [in]  bEnabled    是否启用FWD
 * @param [in]  pstrTarget  (Optional) 转移的目标号码，指针为空时不设置该字段
 * @param [in]  pRingDelay  (Optional) 响铃时间，设置无应答转移时有效，指针为空时不设置该字段
 * @param [in]  eSource     (Optional) 设置来源
 *
 * @return  true: 设置成功; false: 设置失败
 */
bool fwd_SetInfo(int iAccount, FWD_TYPE eType, bool bEnabled,
                 const yl::string * pstrTarget = NULL, int * pRingDelay = NULL,
                 FWD_CONFIG_SOURCE_TYPE eSource = FWD_CFG_SRC_TYPE_COMMON);

/**
 * 同时设置多个FWD开关
 *
 * @param [in]  iAccount    账号ID，FWDDND_ACCOUNT_GLOBAL(-1)表示全局模式
 * @param [in]  eType       FWD类型，支持同时指定多个FWD类型
 * @param [in]  bEnabled    是否启用FWD
 * @param [in]  eSource     (Optional) 设置来源
 *
 * @return  true: 设置成功; false: 设置失败
 */
bool fwd_SetMultiInfo(int iAccount, int eType, bool bEnabled,
                      FWD_CONFIG_SOURCE_TYPE eSource = FWD_CFG_SRC_TYPE_COMMON);

/**
 * 还原备份的FWD信息，并关闭FWD
 *
 * @param [in]  iAccount    账号ID，FWDDND_ACCOUNT_GLOBAL(-1)表示全局模式
 * @param [in]  eType       FWD类型
 *
 * @return  true: 还原成功; false: 还原失败
 */
bool fwd_RestoreInfo(int iAccount, FWD_TYPE eType);

/**
 * 判断是否允许转移到指定号码，主要进行国际转移开关限制
 *
 * @param [in]  strTarget   转移的目标号码
 *
 * @return  是否允许转移到指定号码
 */
bool fwd_IsAllowForwardTo(const yl::string & strTarget);

/**
 * 获取全局模式最后一次修改配置时启用的FWD类型
 *
 * @return  最后一次修改配置时启用的FWD类型
 */
int fwd_GetLastSavedType();

#endif // !__MOD_FWD_DND_H__
