#ifndef _MESSAGE_BOX_CALL_BACK_H_
#define _MESSAGE_BOX_CALL_BACK_H_

class CMessageBoxBase;

class CMsgBoxCallBackAction
{
public:
    CMsgBoxCallBackAction();
    virtual ~CMsgBoxCallBackAction();

public:
    virtual void MessageBoxCallBack(CMessageBoxBase * pMessageBox) = 0;
};

// 弹出框消失时的回调函数
typedef void (*MSGBOX_CALLBACK_FUN)(CMessageBoxBase *);

// 弹出框消失时候回调函数的包装类
class CMsgBoxCBFunWrapper : public CMsgBoxCallBackAction
{
public:
    explicit CMsgBoxCBFunWrapper(MSGBOX_CALLBACK_FUN  pCallBackFun);
    virtual ~CMsgBoxCBFunWrapper();

    virtual void MessageBoxCallBack(CMessageBoxBase * pMessageBox);

private:
    MSGBOX_CALLBACK_FUN     m_pCallBackFun;
};

#define WRAPPER_MSGBOX_CBFUN(fun)   (new CMsgBoxCBFunWrapper(fun))

#endif // _MESSAGE_BOX_CALL_BACK_H_
