#ifndef __MOD_WARNING_CENTER_H__
#define __MOD_WARNING_CENTER_H__

#include <ylvector.h>

/***
 * @brief : 初始化
 * @return: 初始化结果, true/false
 */
bool WarningCenter_Init();

/***
 * @brief : 警告模式是否开启
 * @return: true/false  开关/关闭
 */
bool WarningCenter_IsWarningModeEnable();

/***
 * @brief : 获取警告列表
 * @param : |vecWarning| 警告列表
 * @return: 获取个数
 */
int  WarningCenter_GetWarning(YLVector<int>& vecWarning);

/***
 * @brief : 移除某种警告类型
 * @param : |iWarning| 目标警告类型
 * @notice: 当iWarning==0的时候, 表示移除所有
 */
void WarningCenter_RemoveWarning(int iWarning);

/***
 * @brief : 取消警告状态
 */
void WarningCenter_CancelWarning();

#endif // __MOD_WARNING_CENTER_H__
