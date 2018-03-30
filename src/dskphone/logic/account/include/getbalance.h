#ifndef ACCOUNT_GET_BALANCE_H
#define ACCOUNT_GET_BALANCE_H

#ifdef IF_FEATURE_GETBALANCE
#include "taskaction/taskactioncallbackbase.h"
#include "threadtask/include/networkfileaction.h"
#include "talk/talklogic/include/uiandlogic_common.h"

using NS_TA::CTaskAction;
using NS_TA::CTaskActionCallbackBase;

// 对获取账号余额的URL进行变量替换
#define RPB_SESSION_ID  "$session_id"       // 当前通话的标示变量,暂时未用,可替换成空
#define RPB_SIPUID      "$sipuid"           // 账号显示名和服务器,如: ivanoff@mangosip.ru
#define RPB_PWD         "$password"         // 账号密码

// 弹窗时间，超时退出时间
enum BALANCE_MSG_SHOW_TIME
{
    BALANCE_MSG_BOX_SHOW_NORMAL  = 5 * 1000,  // 5s弹出框
    BALANCE_MSG_BOX_SHOW_LOADING = 180 * 1000, // 180s下载提示框
    BALANCE_MSG_BOX_DELAY_SHOW   = 10,        // 延时10毫秒显示，防止弹出框阻塞
    BALANCE_MSG_DELAY_TO_GET_BALANCE = 1 * 1000
};

class CAccountBalanceMng : public CTaskActionCallbackBase
{
public:
    CAccountBalanceMng();
    ~CAccountBalanceMng();

    // 下载账号余额信息
    bool LoadBalance(int iAccount);
    // 显示账号余额下载提示
    void ShowLoadingBalance();
    // 查询账号余额的账号ID
    int GetBalanceID()
    {
        return m_iBalanceID;
    }
    // 设置账号余额的账号ID
    void SetBalanceID(int iID)
    {
        m_iBalanceID = iID;
    }
    // 是否正在下载余额信息
    bool IsLoadingBalance()
    {
        return m_bIsLoadingBalance;
    }
    // 是否取消了下载余额信息
    bool IsCancelGetBalance()
    {
        return m_bCanceled;
    }

    void GetBalanceInfo(Balance_Info_t & tInfo)
    {
        memcpy(&tInfo, &m_BalanceInfo, sizeof(Balance_Info_t));
    }

    void CancelLoadBalance();

private:
    // 解析包含账号余额信息的下载xml文件
    void ParseBalance(yl::string strFile);
    // 添加账号余额信息http查询连接的隐藏部分
    void AddLinkHidePart(yl::string & strLink);
    // 处理余额信息
    bool ProcessBalanceResult(bool bSuccess, int iAccount);

    virtual void OnTaskActionCallback(CTaskAction * pTaskAction);

private:
    int                 m_iBalanceID;           // 需要查询账号余额的账号ID
    bool                m_bIsLoadingBalance;    // 是否正在下载余额信息
    Balance_Info_t      m_BalanceInfo;          // 存储获取的余额相关信息
    bool                m_bCanceled;
};

CAccountBalanceMng & GetAccountBalanceMng();
#define _AccBalanceManager GetAccountBalanceMng()

#endif  // IF_FEATURE_GETBALANCE
#endif  // ACCOUNT_GET_BALANCE_H
