#ifndef __PRETRAN_ROUTINE_H__
#define __PRETRAN_ROUTINE_H__

// Transfer拨号过程
class CPreTranRoutine : public CDialRoutine
{
public:
    CPreTranRoutine();
    virtual ~CPreTranRoutine();

    // 状态
    virtual ROUTINE_STATE GetRoutineState() const
    {
        return ROUTINE_PRETRAN;
    }
    // 名称
    virtual LPCSTR GetName() const
    {
        return "PreTrans";
    }

    // 处理ui发送过来的Action事件
    virtual bool OnActionEvent(DataUI2Logic* pActionData);
    // 获取对应session的状态
    virtual SESSION_STATE GetHostSessionState()
    {
        return SESSION_PRETRAN;
    }

    // 呼出当前已拨号码
    virtual bool CallOut(const yl::string& strNumber, yl::string strName = "",
                         int iCallOutTriggerMask = COT_DEFAULT,
                         bool bAudioOnly = true);

private:
    // 盲转，输入号码后按Tran键实现盲转
    bool BlindTransfer(const yl::string& strNumber = "");
    // 对tran事件的处理
    bool ProcessTranAction(int iTargetCallID, const char* lpszNumber);
    // 根据转移的类型进行相应操作
    bool TransferByMode(const yl::string& strNumber, const yl::string& strName = "",
                        int iCallOutTriggerMask = COT_DEFAULT, DSSKEY_TRANS_TYPE eDsskeyTranType = DTT_NEW_CALL);
};

#endif //__PRETRAN_ROUTINE_H__
