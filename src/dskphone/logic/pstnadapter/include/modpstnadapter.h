#ifndef __MOD_PSTN_ADAPTER_H__
#define __MOD_PSTN_ADAPTER_H__

#ifdef IF_FEATURE_PSTN

#if WIN32
#include "win32_adapter.h"
#else
#include <ylstring.h>
#endif // WIN32

#include "pstnadaptermessagedefine.h"

/**
 * @brief PSTN适配模块初始化
 */
void pstnadapter_Init();

/**
 * @brief PSTN适配模块释放
 */
void pstnadapter_Uninit();

/**
 * @brief 获取当前PSTN Box设备数量
 *
 * @return 当前PSTN Box设备数量
 */
int pstnadapter_GetDeviceCount();

/**
 * @brief 获取当前PSTN线路数量
 *
 * @return 当前PSTN线路数量
 */
int pstnadapter_GetLineCount();

/**
 * @brief 设置是否启用
 *
 * @param iDeviceId 设备ID
 * @param bEnable true=启用, false=禁用
 */
void pstnadapter_SetEnable(int iDeviceId, bool bEnable);

/**
 * @brief 摘机/挂机
 *
 * @param iDeviceId 设备ID
 * @param bOffHook true=摘机, false=挂机
 *
 * @return 操作是否成功
 */
bool pstnadapter_SetHook(int iDeviceId, bool bOffHook);

/**
 * @brief 拍叉簧
 *
 * @param iDeviceId 设备ID
 *
 * @return 操作是否成功
 */
bool pstnadapter_Flash(int iDeviceId);

/**
 * @brief 当前是否处于Flash（拍叉簧）状态
 *
 * @param iDeviceId 设备ID
 *
 * @return 当前是否处于Flash状态
 */
bool pstnadapter_IsFlashing(int iDeviceId);

/**
 * @brief 设置静音/取消静音
 *
 * @param iDeviceId 设备ID
 * @param bMute true=静音, false=取消静音
 *
 * @return 操作是否成功
 */
bool pstnadapter_SetMute(int iDeviceId, bool bMute);

/**
 * @brief 通话保持/取回
 *
 * @param iDeviceId 设备ID
 * @param bHold true=保持, false=取回
 *
 * @return 操作是否成功
 */
bool pstnadapter_SetHold(int iDeviceId, bool bHold);

/**
 * @brief 设置免打扰状态
 *
 * @param iDeviceId 设备ID
 * @param bDND true=免打扰, false=退出免打扰
 *
 * @return 操作是否成功
 */
bool pstnadapter_SetDND(int iDeviceId, bool bDND);

/**
 * @brief 拨号
 *
 * @param iDeviceId 设备ID
 * @param strNumber 拨打号码
 * @param iCallId 呼叫ID
 *
 * @return 操作是否成功
 */
bool pstnadapter_Dial(int iDeviceId, const yl::string & strNumber, int iCallId);

/**
 * @brief 获取线路ID
 * @details 根据设备ID获取线路ID，用于VPM调用
 *
 * @param iDeviceId 设备ID
 * @return 线路ID
 */
int pstnadapter_GetLineId(int iDeviceId);

/**
 * @brief 判断PSTN线路是否可用（可以用于呼出）
 * @details 判断PSTN线路状态是否为空闲
 *
 * @param iDeviceId 设备ID
 * @return PSTN线路是否可用
 */
bool pstnadapter_IsAvailable(int iDeviceId);

#endif // IF_FEATURE_PSTN

#endif // __MOD_PSTN_ADAPTER_H__