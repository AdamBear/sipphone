#ifndef __MODTASKACTION_H__
#define __MODTASKACTION_H__

#include "taskaction.h"
#ifdef __cplusplus
#define TA_EXPORT_C extern "C"
#else
#define TA_EXPORT_C
#endif

namespace NS_TA
{
class CTaskAction;
}
using NS_TA::CTaskAction;

/***
 * @brief : TaskAction模块初始化
 * @param : |uCommonDefaultCount| 通用线程池常驻线程个数
 *        : |uSpecialMaxCount| 专用线程最大个数
 * @return: true/false 初始化成功/失败
 * @notice: 只允许单次初始化
 */
TASKACTION_DLL_DECLARRE bool TaskAction_Init(UINT uCommonDefaultCount, UINT uSpecialMaxCount);

/***
 * @brief : TaskAction设置通用线程池(Common Thread Pool)额外参数
 * @param : |uCommonMaxCount| 通用线程池线程最大数量
 *        : |uTaskThreshold| 通用任务务堆积阈值
 *        : |uKeepAliveTime(ms)| 所有线程闲置超时时间(>=500ms), 为0则不回收
 * @return: true/false 设置额外参数成功/失败
 * @notice: 1. 线程扩展条件: 任务堆积超过设置阈值, 即不断扩展线程数量协助处理
 *          2. 线程回收条件: 通用线程数量 > 常驻数量 && 所有线程闲置时间超时
 */
TASKACTION_DLL_DECLARRE bool TaskAction_SetCommonTPExtraParam(UINT uCommonMaxCount,
        UINT uTaskThreshold, UINT uKeepAliveTime);

/***
 * @brief : 创建专用线程
 * @param : |uReqType| 专用线程映射的请求类型
 *        : |strName| 线程名
 *        : |MsgCallBackFun| 消息回调函数
 * @return: true/false 创建成功/失败
 * @notice: 1. 线程名存在的线程直接进行复用
 *          2. MsgCallBackFun == NULL, 线程只支持task方式, 否则只支持消息方式
 */
TASKACTION_DLL_DECLARRE bool TaskAction_CreateSpecialThread(UINT uReqType,
        const yl::string & strName, NS_TA::typeMsgCallBackFun MsgCallBackFun);

/***
 * @brief : 创建专用线程
 * @param : |uReqType| 专用线程映射的请求类型, 可忽略
 *        : |strName| 线程名
 *        : |threadMethod| 线程函数
 *        : |pData| 线程函数参数
 * @return: true/false 创建成功/失败
 * @notice: 不支持复用
 */
TASKACTION_DLL_DECLARRE bool TaskAction_CreateSpecialThreadByMethod(LPTHREAD_MAIN threadMethod,
        const yl::string & strName, void * pData = NULL,
        UINT uReqType = NS_TA::TA_REQUEST_NONE);

/***
 * @brief : 销毁专用线程
 *          如果正在运行任务, 也会尝试销毁
 * @param : |uReqType| 专用线程映射的请求类型
 * @return: true/false 销毁成功/失败
 */
TASKACTION_DLL_DECLARRE bool TaskAction_DestroySpecialThread(UINT uReqType);

/***
 * @brief : Action执行入口
 * @param : |pAction| 需要执行的action
 * @return: true/false 执行操作成功/失败
 * @notice: 执行成功则生命周期由taskaction控制, 否则taskaction不关心
 */
TASKACTION_DLL_DECLARRE bool TaskAction_ExecAction(CTaskAction * pAction);

/***
 * @brief : Action释放入口
 * @param : |pAction| 需要释放的action
 * @notice: 1. 释放action数据和内存, 可能存在一定延时
 *          2. 需要确保调用接口后, 不再使用该action
 *          3. 需要确保action不存在auto release
 */
TASKACTION_DLL_DECLARRE bool TaskAction_ReleaseAction(CTaskAction * pAction);

/***
 * @brief : 移除所有未执行且类型为uReqType的action
 * @param : |uReqType| 移除的目标类型
 * @return: true/false action移除成功/失败
 * @notice: 建议少用, 需要确保类型为uReqtype的action都不存在auto release
 */
TASKACTION_DLL_DECLARRE bool TaskAction_ReleaseActionByReqType(UINT uReqType);

/***
 * @brief : taskaction dump信息
 * @param : |uDumpType| dump信息操作类型
 * @return: true/false 成功/失败
 */
TA_EXPORT_C bool TaskAction_DumpInfo(UINT uDumpType);

#endif  // __MODTASKACTION_H__
