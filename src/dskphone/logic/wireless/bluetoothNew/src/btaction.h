#ifdef IF_SUPPORT_BLUETOOTH

#ifndef __BT_ACTION_H__
#define __BT_ACTION_H__

#include "bluetooth_inc.h"
#include "taskaction/taskaction.h"

class CBTAction : public NS_TA::CTaskAction
{
public:
    CBTAction();
    ~CBTAction();

    /***
     * @brief : 纯虚函数, 执行业务
     */
    virtual bool Exec();

    /***
     * @brief : Exec过程中产生的资源回收
     * @param : |bCallBackSucceed| 是否回调成功
     * @notice: 涉及异步, 只能操作自身资源
     */
    virtual bool ReleaseData(bool bCallBackSucceed);

    void SetBtExecParam(const BTTaskParam& sParam);

    BTTaskParam* GetBtExecParam()
    {
        return m_pBTExecParam;
    }

    void SetTaskType(BT_TASK_TYPE eType)
    {
        m_eTaskType = eType;
    }

    BT_TASK_TYPE GetTaskType()
    {
        return m_eTaskType;
    }

    void SetRelateHandle(long lHandle)
    {
        m_lHandle = lHandle;
    }

    long GetRelateHandle()
    {
        return m_lHandle;
    }

    int GetTaskResult()
    {
        return m_iResult;
    }

    /***
     * @brief : 将任务类型转换为string
     */
    yl::string GetTaskName(BT_TASK_TYPE eType);

private:

    bool ExecHeadsetEvent();

private:
    long                m_lHandle;
    int                 m_iResult;
    BT_TASK_TYPE        m_eTaskType;
    BTTaskParam*        m_pBTExecParam;
};

#endif // __BT_ACTION_H__
#endif // IF_SUPPORT_BLUETOOTH
