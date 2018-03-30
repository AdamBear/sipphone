#ifndef __MSG_KIT_HANDLER_H__
#define __MSG_KIT_HANDLER_H__

#include <ETLLib/ETLLib.hpp>


class CMsgKitHandler : public CMKThread
{
    // 从主线程MsgQueue取到消息后的回调函数列表
    BEGIN_MSG_MAP(CMsgKitHandler)
    CHAIN_MSG_ON_METHOD(OnMessage)
    CHAIN_MSG_TO_BASE(CMKThread)
    END_MSG_MAP()

public:
    CMsgKitHandler();
    virtual ~CMsgKitHandler();

    static CMsgKitHandler& getInstance();

public:
    bool InitMsgKit();
    void UnInitMsgKit();

    void OnMsgKitFDActive();

#ifdef _WIN32
    virtual bool IsRegisterCallBack()
    {
        return false;
    }
#else
    int GetMKFD() const
    {
        return m_nMKFD;
    }
#endif

protected:
    // 回调函数，遍历消息函数表
    BOOL OnMessage(msgObject& objMessage);
#ifndef _WIN32
    BOOL OnTimeOut(msgObject& refObj);

protected:
    int m_nMKFD;
    mkThreadTimer m_objTimer;
#endif
};

#define g_MsgKitHandler CMsgKitHandler::getInstance()

#endif
