#ifndef __WARNING_CENTER_H__
#define __WARNING_CENTER_H__

#include <ylvector.h>
#include <ylstring.h>
#include "dskthread/msgobject.h"
#include "singletonclassdefine.h"
#include "account/include/account_datadefine.h"

class CWarningManager
{
    DECLARE_SINGLETON_CLASS(CWarningManager)

public:
    /***
     * @brief : 初始化
     */
    void Init();

    /***
     * @brief : 注销
     */
    void UnInit();

    /***
     * @brief : 检查警告信息
     */
    void CheckWarningInfo();

    /***
     * @brief : 检查网络状态
     */
    bool CheckNetwork();

    /***
     * @brief : 检查账号状态
     */
    bool CheckAccount(int iAccountID = INVALID_ACCOUNTID);

    /***
     * @brief : 检查autop状态
     */
    bool CheckAutop(msgObject& msg);

    /***
     * @brief : 检查话机用户密码状态
     */
    bool CheckPhonePSWD();

    /***
     * @brief : 获取警告列表
     * @param : |vecWarning| 警告列表
     * @return: 获取个数
     */
    int  GetWarningList(YLVector<int>& vecWarning);

    bool OnMessage(msgObject& msg);

    /***
     * @brief : 移除警告
     * @param : |iWarning|
     * @notice: 当iWarning==0的时候, 表示移除所有
     */
    void RemoveWarning(int iWarning);

    /***
     * @brief : 处理警告模式变更
     * @param : |bEnable| 警告模式开关
     * @notice: true/false  开/关
     */
    void ProcessWraningModeChange();

    /***
     * @brief : 清楚警告相关信息
     */
    void ClearWarningInfo();

    void SetWarningStatus(bool bStatus);

    bool GetWarningStatus();

    /***
     * @brief : 设置警告模式
     */
    void SetWraningModeEnable(bool bEnable)
    {
        m_bEnable = bEnable;
    }

    /***
     * @brief : 警告模式是否开启
     */
    bool IsWarningModeEnable()
    {
        return m_bEnable;
    }

private:
    /***
     * @brief : 新增警告
     * @param : |iWarning| 目标警告类型
     */
    void AddWarning(int iWarning);

private:
    bool              m_bEnable;
    bool              m_bStatus;
    YLVector<int>     m_vecWarning;         // 警告列表
};
#define _WarningManager     CWarningManager::GetInstance()

#endif __WARNING_CENTER_H__
