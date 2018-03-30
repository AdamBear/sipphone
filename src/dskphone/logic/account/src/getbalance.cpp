#ifdef IF_FEATURE_GETBALANCE
#include "account_inc.h"
#include "getbalance.h"
#include "xmlparser/xmlparser.hpp"
#include "sipaccount.h"
#include "accountmanager.h"
#include "taskaction/modtaskaction.h"
#include "threadtask/include/modthreadtask.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "account/include/modaccount.h"
#include "adapterbox/include/modadapterbox.h"

CAccountBalanceMng & GetAccountBalanceMng()
{
    static CAccountBalanceMng objAccountBalanceMng;
    return objAccountBalanceMng;
}

CAccountBalanceMng::CAccountBalanceMng()
{
    m_iBalanceID = -1;
    m_bIsLoadingBalance = false;
    m_bCanceled = false;
    memset(&m_BalanceInfo, 0, sizeof(Balance_Info_t));
}

CAccountBalanceMng::~CAccountBalanceMng()
{

}

// 下载账号余额信息
bool CAccountBalanceMng::LoadBalance(int iAccount)
{
    // 未指定账号ID或账号ID是IP直播账号,则不处理
    if (-1 == iAccount || m_bIsLoadingBalance)
    {
        return false;
    }

    // 根据账号ID获取账号对象指针
    CSipAccount * pAccount = dynamic_cast<CSipAccount *>(_AccountManager.GetAccountByAccountId(
                                 iAccount));

    // 可以获取账号,该账号允许获取余额信息且获取余额的URL非空,才会去获取账号余额
    if (pAccount != NULL
            && pAccount->GetBalanceSwtich()
            && (pAccount->IsAccountUsable() || LS_SYSTEM_DEFAULT_ACCOUNT == pAccount->GetAccountState())
            && !pAccount->GetBalanceLink().empty()
            && !pAccount->IsDirectIPAccount())
    {
        yl::string strURL = pAccount->GetBalanceLink();

        AddLinkHidePart(strURL);

        // 纠正URL头域
        if (strncmp(strURL.c_str(), "http://", 7) != 0
                && strncmp(strURL.c_str(), "https://", 8) != 0)
        {
            strURL = "http://" + strURL;
        }

        // 替换URL中的Session ID参数
        if (strstr(strURL.c_str(), RPB_SESSION_ID) != NULL)
        {
            strURL.replace(RPB_SESSION_ID, "");
        }

        // 替换URL中的Sipu ID参数
        if (strstr(strURL.c_str(), RPB_SIPUID) != NULL)
        {
            yl::string strSipuId = pAccount->GetUserName() + "@" + pAccount->GetServerName();
            strURL.replace(RPB_SIPUID, strSipuId);
        }

        // 替换URL中的Password参数
        if (strstr(strURL.c_str(), RPB_PWD) != NULL)
        {
            strURL.replace(RPB_PWD, pAccount->GetPassword());
        }

        //send http msg to server
        // 直接去服务器上下载文件, 不关心结果.
        CNetworkFileAction * pAction = TaskAction_CreateNetworkFileAction(szXmlBalance, strURL.c_str());
        if (pAction != NULL)
        {
            pAction->SetSync(false);  // 设置异步.
            pAction->SetRead(true);  // 设置下载.
            pAction->SetCallback(this);  // 设置回调.
            pAction->SetExtraParam(iAccount); // 设置账号id
            // 开始下载.
            bool ret = TaskAction_ExecAction(pAction);

            m_bCanceled = false;
            m_bIsLoadingBalance = ret;

            return ret;
        }
    }

    return false;
}

// 解析包含账号余额信息的下载xml文件
void CAccountBalanceMng::ParseBalance(yl::string strFile)
{
    // 文件名为空或不存在,则不处理
    if (strFile.empty()
            || !pathFileExist(strFile.c_str()))
    {
        return;
    }

    // 解析xml文件并获取主节点指针
    xml_document xmlBalance;
    xmlBalance.load_file(strFile.c_str());
    xml_node nodeXmlNode = xmlBalance.child("protocol");

    // 不符合xml规范,则返回
    if (nodeXmlNode.empty())
    {
        return;
    }

    // 获取并存储显示名
    yl::string strValue = nodeXmlNode.attribute("namespace").value();
    strncpy(m_BalanceInfo.szName, strValue.c_str(), MAX_BALANCE_LEN - 1);

    // 获取账号余额查询的回复信息
    nodeXmlNode = nodeXmlNode.child("response");
    if (!nodeXmlNode.empty())
    {
        // 获取账号余额查询结果
        xml_node nodeXmlChild = nodeXmlNode.child("result");
        if (!nodeXmlChild.empty())
        {
            strValue = "";
            strValue = nodeXmlChild.text().get();
            strValue.trim_both("\r \n");
            m_BalanceInfo.iResult = atoi(strValue.c_str());
        }

        // 根据账号余额返回结果解析返回信息
        if (m_BalanceInfo.iResult)
        {
            // 获取错误查询的信息
            nodeXmlChild = nodeXmlNode.child("comment");
            if (!nodeXmlChild.empty())
            {
                strValue = nodeXmlChild.text().get();
                strValue.trim_both("\r \n");
                strncpy(m_BalanceInfo.szBalance, strValue.c_str(), MAX_BALANCE_LEN - 1);
            }
        }
        else
        {
            // 获取余额信息
            nodeXmlChild = nodeXmlNode.child("balance");
            if (!nodeXmlChild.empty())
            {
                // 获取并设置账号
                strValue = nodeXmlChild.attribute("mbaccount").value();
                strValue.trim_both("\r \n");
                strncpy(m_BalanceInfo.szAccount, strValue.c_str(), MAX_BALANCE_LEN - 1);

                // 获取余额信息
                strValue = nodeXmlChild.attribute("value").value();
                strValue = strValue + " " + nodeXmlChild.attribute("currency").value();
                strValue.trim_both("\r \n");
                strncpy(m_BalanceInfo.szBalance, strValue.c_str(), MAX_BALANCE_LEN - 1);
            }
        }
    }
}

// 显示账号余额下载提示
void CAccountBalanceMng::ShowLoadingBalance()
{
    etl_NotifyApp(TRUE, GET_BALANCE_MESSAGE_LOADING, m_iBalanceID, 0);
}

void CAccountBalanceMng::CancelLoadBalance()
{
    ACCOUNT_INFO("CAccountBalanceMng::CancelLoadBalance");
    m_bIsLoadingBalance = false;
    m_bCanceled = true;
}

// 处理余额信息
bool CAccountBalanceMng::ProcessBalanceResult(bool bSuccess, int iAccount)
{
    if (!talklogic_SessionExist())
    {
        ACCOUNT_WARN("Load balance bSuccess=[%d], iResult=[%d]", bSuccess, m_BalanceInfo.iResult != 0);
        if (bSuccess)
        {
            // 如果账号余额信息没有给出显示名,则用默认的显示名
            if (0 == strlen(m_BalanceInfo.szName))
            {
                // 返回错误信息的默认显示名是“Warning”,正确信息的默认显示名是账号的显示名
                if (m_BalanceInfo.iResult)
                {
                    strncpy(m_BalanceInfo.szName, TRID_WARNING, MAX_BALANCE_LEN - 1);
                }
                else
                {
                    yl::string strName = "";
                    CSipAccount * pAccount = dynamic_cast<CSipAccount *>(_AccountManager.GetAccountByAccountId(
                                                 iAccount));
                    if (pAccount != NULL)
                    {
                        // 显示名首选账号的Label Name,如果为空,则用显示名,如果还是为空,则用User Name
                        strName = acc_GetAccountShowText(iAccount).c_str();
                    }

                    // 如果账号显示名为空,则用公共显示名
                    if (strName.empty())
                    {
                        strName = TRID_CHARGE;
                    }

                    strncpy(m_BalanceInfo.szName, strName.c_str(), MAX_BALANCE_LEN - 1);
                }
            }

            etl_NotifyApp(false, GET_BALANCE_MESSAGE_GET_RESULT, iAccount, m_BalanceInfo.iResult == 0);
        }
        else
        {
            etl_NotifyApp(false, GET_BALANCE_MESSAGE_LOAD_ERROR, iAccount, 0);
        }

        return bSuccess;
    }
    else
    {
        // 有通话清除提示框
        etl_NotifyApp(false, GET_BALANCE_MESSAGE_CLEAN_TIPS, iAccount, 0);
    }

    return false;
}

// 添加账号余额信息http查询连接的隐藏部分
void CAccountBalanceMng::AddLinkHidePart(yl::string & strLink)
{
    // 字符串为空,则不需要添加隐藏部分
    if (strLink.empty())
    {
        return;
    }

    // 如果已经有询问条件,则不需要添加隐藏部分
    if (strstr(strLink.c_str(), "?") != NULL)
    {
        return;
    }

    // 如果最后一个字符不是‘/’,则要加上
    int iLen = strLink.GetLength();
    if (strLink[iLen - 1] != '/')
    {
        strLink += "/";
    }

    // 添加余额http连接隐藏部分
    strLink += "sip_balance?sipuid=$sipuid&password=$password";
}

void CAccountBalanceMng::OnTaskActionCallback(CTaskAction * pTaskAction)
{
    if (pTaskAction == NULL
            || pTaskAction->GetReqType() != NS_TA::TA_REQUEST_NETWORK_FILE)
    {
        return;
    }

    // 回调后删除
    CNetworkFileAction * pAction = static_cast<CNetworkFileAction *>(pTaskAction);

    m_bIsLoadingBalance = false;

    if (!m_bCanceled)
    {
        // 已经取消直接返回
        bool bSuccess = pTaskAction->IsOperationSucceed();
        if (bSuccess)
        {
            memset(&m_BalanceInfo, 0, sizeof(Balance_Info_t));
            // 解析得到所需的信息
            ParseBalance(pAction->GetFilePath());
        }

        ProcessBalanceResult(bSuccess, pAction->GetExtraParam());
    }
}
#endif  // IF_FEATURE_GETBALANCE


