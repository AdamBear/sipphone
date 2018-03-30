#ifndef __WIFI_ACTION_H__
#define __WIFI_ACTION_H__

#include "wireless/wifi/include/wifidefines.h"
#include "taskaction/taskaction.h"

class CWifiAction : public NS_TA::CTaskAction
{
public:
    CWifiAction();
    virtual ~CWifiAction();

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

    /***
     * @brief : 设置wifi执行参数
     * @param : |sParam| 执行所需参数
     */
    void SetWifiExecParam(const WiFiTaskParam& sParam);

    /***
     * @brief : 获取wifi执行参数
     */
    WiFiTaskParam* GetWifiExecParam()
    {
        return m_pWifiExecParam;
    }

    /***
     * @brief : 设置任务类型
     */
    void SetTaskType(WIFI_TASK_TYPE eType)
    {
        m_eTaskType = eType;
    }

    /***
     * @brief : 获取任务类型
     */
    WIFI_TASK_TYPE GetTaskType()
    {
        return m_eTaskType;
    }

    /***
     * @brief : 将任务类型转换为string
     */
    yl::string GetTaskInfo(WIFI_TASK_TYPE eType);

private:
    WiFiTaskParam*          m_pWifiExecParam;
    WIFI_TASK_TYPE          m_eTaskType;
};

#endif // __WIFI_ACTION_H__
