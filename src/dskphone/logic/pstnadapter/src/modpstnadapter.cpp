#include "modpstnadapter.h"

#include <configiddefine.h>
#include <configparser/modconfigparser.h>

#include "pstnadaptermanager.h"


#ifdef IF_FEATURE_PSTN

/**
 * @brief PSTN适配模块初始化
 */
void pstnadapter_Init()
{
#if !WIN32
    if (configParser_GetConfigInt(kszPSTNEnable) == 1)
#endif // !WIN32
    {
        _PstnAdapterManager.Launch();
    }
}

/**
 * @brief PSTN适配模块释放
 */
void pstnadapter_Uninit()
{
#if !WIN32
    if (configParser_GetConfigInt(kszPSTNEnable) == 1)
#endif // !WIN32
    {
        _PstnAdapterManager.Destroy();
    }
}

/**
 * @brief 获取当前PSTN Box设备数量
 *
 * @return 当前PSTN Box设备数量
 */
int pstnadapter_GetDeviceCount()
{
    return _PstnAdapterManager.GetDeviceCount();
}

/**
 * @brief 获取当前PSTN线路数量
 *
 * @return 当前PSTN线路数量
 */
int pstnadapter_GetLineCount()
{
    return _PstnAdapterManager.GetLineCount();
}

/**
 * @brief 设置是否启用
 *
 * @param iDeviceId 设备ID
 * @param bEnable true=启用, false=禁用
 */
void pstnadapter_SetEnable(int iDeviceId, bool bEnable)
{
    _PstnAdapterManager.SetEnable(iDeviceId, bEnable);
}

/**
 * @brief 摘机/挂机
 *
 * @param iDeviceId 设备ID
 * @param bOffHook true=摘机, false=挂机
 *
 * @return 操作是否成功
 */
bool pstnadapter_SetHook(int iDeviceId, bool bOffHook)
{
    return _PstnAdapterManager.SetHook(iDeviceId, bOffHook);
}

/**
 * @brief 设置静音/取消静音
 *
 * @param iDeviceId 设备ID
 * @param bMute true=静音, false=取消静音
 *
 * @return 操作是否成功
 */
bool pstnadapter_SetMute(int iDeviceId, bool bMute)
{
    return _PstnAdapterManager.SetMute(iDeviceId, bMute);
}

/**
 * @brief 通话保持/取回
 *
 * @param iDeviceId 设备ID
 * @param bHold true=保持, false=取回
 *
 * @return 操作是否成功
 */
bool pstnadapter_SetHold(int iDeviceId, bool bHold)
{
    return _PstnAdapterManager.SetHold(iDeviceId, bHold);
}

/**
 * @brief 拍叉簧
 *
 * @param iDeviceId 设备ID
 *
 * @return 操作是否成功
 */
bool pstnadapter_Flash(int iDeviceId)
{
	return _PstnAdapterManager.Flash(iDeviceId);
}

/**
* @brief 当前是否处于Flash（拍叉簧）状态
*
* @param iDeviceId 设备ID
*
* @return 当前是否处于Flash状态
*/
bool pstnadapter_IsFlashing(int iDeviceId)
{
	return _PstnAdapterManager.IsFlashing(iDeviceId);
}

/**
 * @brief 设置免打扰状态
 *
 * @param iDeviceId 设备ID
 * @param bDND true=免打扰, false=退出免打扰
 *
 * @return 操作是否成功
 */
bool pstnadapter_SetDND(int iDeviceId, bool bDND)
{
    return _PstnAdapterManager.SetDND(iDeviceId, bDND);
}

/**
 * @brief 拨号
 *
 * @param iDeviceId 设备ID
 * @param strNumber 拨打号码
 * @param iCallId 呼叫ID
 *
 * @return 操作是否成功
 */
bool pstnadapter_Dial(int iDeviceId, const yl::string & strNumber, int iCallId)
{
    return _PstnAdapterManager.Dial(iDeviceId, strNumber, iCallId);
}

/**
 * @brief 获取线路ID
 * @details 根据设备ID获取线路ID，用于VPM调用
 *
 * @param iDeviceId 设备ID
 * @return 线路ID
 */
int pstnadapter_GetLineId(int iDeviceId)
{
    return _PstnAdapterManager.GetLineId(iDeviceId);
}

bool pstnadapter_IsAvailable(int iDeviceId)
{
    return _PstnAdapterManager.IsAvailable(iDeviceId);
}

#endif // IF_FEATURE_PSTN
