#ifndef __TASKACTION_H__
#define __TASKACTION_H__

#include "common.h"

namespace NS_TA
{

class CTaskActionCallbackBase;
class TASKACTION_DLL_DECLARRE CTaskAction //: public chObject
{
public:
    CTaskAction();
    virtual ~CTaskAction();

    /***
     * @brief : 纯虚函数, 执行业务
     */
    virtual bool Exec() = 0;

    /***
     * @brief : Exec过程中产生的资源回收
     * @param : |bCallBackSucceed| 是否回调成功
     * @notice: 涉及异步, 只能操作自身资源
     */
    virtual bool ReleaseData(bool bCallBackSucceed) = 0;

    /***
     * @brief : 设置回调类指针
     * @param : |pTaskActionCallbackBase| 回调类指针
     */
    void SetCallback(CTaskActionCallbackBase* pTaskActionCallbackBase);

    /***
     * @brief : 获取回调类指针
     */
    CTaskActionCallbackBase* GetCallback();

    /***
     * @brief : 获取请求类型
     */
    TaskActionRequestType GetReqType() const;

    /***
     * @brief : 操作是否成功
     */
    bool IsOperationSucceed() const;

    /***
     * @brief : 设置错误代码
     * @param : |eType| 错误类型
     */
    void SetTaskActionErrorType(TaskActionErrorType eType);

    /***
     * @brief : 获取错误代码
     */
    TaskActionErrorType GetTaskActionErrorType() const;

    /***
     * @brief : 设置读或写
     */
    void SetRead(bool bRead);

    /***
     * @brief : 是否读操作
     */
    bool IsRead() const;

    /***
     * @brief : 设置同步还是异步
     */
    void SetSync(bool bSync);

    /***
     * @brief : 是否同步
     */
    bool IsSync() const;

    /***
     * @brief : 设置任务是否需要覆盖
     */
    void SetCoverTask(bool bCoverTask);

    /***
     * @brief : 任务是否需要覆盖
     */
    bool IsCoverTask() const;

    /***
     * @brief : 设置任务是否需要时序
     */
    void SetNeedQueue(bool bIsNeedQueue);

    /***
     * @brief : 任务是否需要时序
     */
    bool IsNeedQueue() const;

    /***
     * @brief : 设置action是否自动释放
     * @notice: release前, 会将等待队列中相同action都移除
     */
    void SetAutoRelease(bool bAutoRelease);

    /***
     * @brief : action是否释放
     */
    bool IsAutoRelease();

    /***
     * @brief : 设置优先级
     */
    void SetPriority(TaskActionPriority ePriority);

    /***
     * @brief : 获取优先级
     */
    TaskActionPriority GetPriority() const;

    /**************** 以下接口, 禁止外部使用 ****************/

    /***
     * @brief : 设置Action是否用来释放
     */
    void SetReleaseFlag(bool bEnable);

    /***
     * @brief : Action是否用来释放
     * @return: true/false
     */
    bool GetReleaseFlag();

    /****************************************************/

protected:
    TaskActionRequestType       m_eTARequestType;           // 请求类型

private:
    CTaskActionCallbackBase*    m_pTaskActionCallbackBase;  // 回调类指针
    TaskActionErrorType         m_eTAErrorType;             // 错误代码
    bool                        m_bRead;                    // 读或写
    bool                        m_bSync;                    // 同步还是异步
    bool                        m_bCoverTask;               // 是否覆盖队列任务
    bool                        m_bIsNeedQueue;             // 是否需要时序
    bool                        m_bAutoRelease;             // Action是否自动释放
    bool                        m_bReleaseFlag;             // Action是否用来释放
    TaskActionPriority          m_ePriority;                // 优先级
}; // class TaskAction

} // namespace NS_TA

#endif  // __TASKACTION_H__
