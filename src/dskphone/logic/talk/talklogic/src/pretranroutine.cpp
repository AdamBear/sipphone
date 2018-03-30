#include "talklogic_inc.h"

CPreTranRoutine::CPreTranRoutine()
{
}

CPreTranRoutine::~CPreTranRoutine()
{

}

// 处理ui发送过来的Action事件
bool CPreTranRoutine::OnActionEvent(DataUI2Logic* pActionData)
{
    if (pActionData == NULL)
    {
        return false;
    }

    bool bHandled = false;
    switch (pActionData->eAction)
    {
    case TALK_ACTION_TRAN:
        {
            bHandled = ProcessTranAction(pActionData->wActionParam, m_pCallInfo->GetDialText().c_str());
        }
        break;
    case TALK_ACTION_REDIAL:
        {
            // Tran拨号界面不处理Redial
            bHandled = true;
        }
        break;
    default:
        bHandled = CDialRoutine::OnActionEvent(pActionData);
        break;
    }

    return bHandled;
}

// 盲转，输入号码后按Tran键实现盲转
bool CPreTranRoutine::BlindTransfer(const yl::string& strNumber/* = ""*/)
{
    // 如果输入参数的号码为空则以当前界面已拨号码为Tran Target
    yl::string strTranNumber = strNumber;
    if (strNumber.empty())
    {
        strTranNumber = m_pCallInfo->GetDialText();
    }

    if (strTranNumber.empty())
    {
        return false;
    }

    // 获取发起Tran的Session及routine
    CSingleSession* pSession = m_pCallSession->GetSponsorSession(SPONSOR_TRAN);
    if (NULL == pSession)
    {
        return false;
    }

    if (pSession->IsInherits(ROUTINE_TALKING))
    {
        pSession->Redirectcall(strTranNumber.c_str());
        // 退出当前Session
        m_pCallSession->ExitSession();
    }

    return true;
}

// 对tran事件的处理
bool CPreTranRoutine::ProcessTranAction(int iTargetCallID, const char* lpszNumber)
{
    bool bHandled = false;
    if (iTargetCallID == 0)
    {
        // callid为0表示非穿梭
        yl::string strNumber = m_pCallInfo->GetDialText();

        // 用UI传过来的号码Tran
        if (lpszNumber != NULL)
        {
            strNumber = lpszNumber;
        }

        bHandled = BlindTransfer(strNumber);
    }
    else
    {
        // 已有多路通话的情况下穿梭
        CSingleSession* pTranTargetSession = _SessionManager.GetRelateSessionByCallID(iTargetCallID);
        if (pTranTargetSession != NULL)
        {
            CSingleSession* pTranSponsor = m_pCallSession->GetSponsorSession(SPONSOR_TRAN);
            if (pTranSponsor != NULL)
            {
                // 取消当前Session和sponsor的关系
                m_pCallSession->SetSponsorSession(SPONSOR_TRAN, NULL);
                // 将要Tran的目标session设置为当前Session Sponsor的Follow
                pTranTargetSession->SetSponsorSession(SPONSOR_TRAN, pTranSponsor);

                // 先退出当前Session bugid：16039
                // 原因：TranCurrentSession处理后当前routine会跳转到dialroutine，当前routine成员m_pCallSession已被delete
                m_pCallSession->ExitSession();

                // 后实现穿梭
                pTranSponsor->TranCurrentSession();

                bHandled = true;
            }
        }
    }

    return bHandled;
}

// 根据转移的类型进行相应操作
bool CPreTranRoutine::TransferByMode(const yl::string& strNumber,
                                     const yl::string& strName/* = ""*/,
                                     int iCallOutTriggerMask/* = COT_DEFAULT*/, DSSKEY_TRANS_TYPE eDsskeyTranType/* = DTT_NEW_CALL */)
{
    TALK_INFO("[CPreTranRoutine::TransferByMode: Type = [%d], Number = [%s] !",
              eDsskeyTranType, strNumber.c_str());

    // 咨询或半咨询或按功能呼出
    if (DTT_NEW_CALL == eDsskeyTranType
            || DTT_CALL_OUT == eDsskeyTranType
#if IF_BUG_30961
            || DTT_FON_CALL_OUT == eDsskeyTranType
#endif
            || DTT_AUTO == eDsskeyTranType
       )
    {
        return CDialRoutine::CallOut(strNumber, strName, iCallOutTriggerMask);
    }
    //直接忙转
    else if (DTT_BLIND_TRANSFER <= eDsskeyTranType)
    {
        //sip缺陷，盲转不能过快，否则引起sip消息处理序列异常 -- 移植T3X
        etlSleep(500);

        return BlindTransfer(strNumber);
    }

    return false;
}

bool CPreTranRoutine::CallOut(const yl::string& strNumber, yl::string strName /*= ""*/,
                              int iCallOutTriggerMask /*= COT_DEFAULT*/, bool bAudioOnly /*= true*/)
{
    ////非自动呼出直接基类呼出
    if (iCallOutTriggerMask != COT_DIALNOW
            && iCallOutTriggerMask != COT_DIGITMAP)
    {
        return CDialRoutine::CallOut(strNumber, strName, iCallOutTriggerMask, bAudioOnly);
    }

    // Dial Plan - Dial Now：根据配置确定是咨询转还是盲转
    enum DIAL_PLAN_TRANSFER_MODE
    {
        DPTM_CONSULT_TRANSFER = 0, // 咨询转
        DPTM_BLIND_TRANSFER = 1 // 盲转
    };
    DIAL_PLAN_TRANSFER_MODE mode =
        (DIAL_PLAN_TRANSFER_MODE)configParser_GetConfigInt(kszDialPlanTransferMode);
    DSSKEY_TRANS_TYPE transType = DTT_CALL_OUT;
    if (DPTM_BLIND_TRANSFER == mode && iCallOutTriggerMask == COT_DIALNOW)
    {
        transType = DTT_BLIND_TRANSFER;
    }

    return TransferByMode(strNumber, strName, iCallOutTriggerMask, transType);
}
