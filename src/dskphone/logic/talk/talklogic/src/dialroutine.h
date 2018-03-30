#ifndef __DIAL_ROUTINE_H__
#define __DIAL_ROUTINE_H__

// 拨号过程
class CDialRoutine : public CBaseDialRoutine
{
public:
    CDialRoutine();
    virtual ~CDialRoutine();

    // 处理ui发送过来的Action事件
    virtual bool OnActionEvent(DataUI2Logic* pActionData);

    // 获取对应session的状态
    virtual SESSION_STATE GetHostSessionState()
    {
        return SESSION_DIALING;
    }

    // 呼出当前已拨号码
    virtual bool CallOut(const yl::string& strNumber, yl::string strName = "",
                         int iCallOutTriggerMask = COT_DEFAULT,
                         bool bAudioOnly = true);


protected:

    ///////////////////begin:定时器设置和取消//////////////////////


    ///////////////////end:定时器设置和取消//////////////////////

    // 更改账号
    virtual void ChangeAccount(int iAccountID);
};

#endif //__DIAL_ROUTINE_H__
