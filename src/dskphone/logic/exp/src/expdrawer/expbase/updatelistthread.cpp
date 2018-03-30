#include "updatelistthread.h"
CUpdateListThread::~CUpdateListThread()
{

}

CUpdateListThread::CUpdateListThread()
    : m_pEXPManager(NULL)
{
}

CUpdateListThread & CUpdateListThread::GetInstance()
{
    static CUpdateListThread instance;
    return instance;
}

bool CUpdateListThread::OnMessage(msgObject& objMessage)
{
    // 过滤消息
    if (objMessage.message != EXP_MESSAGE_UPDATE_KEY)
    {
        return false;
    }

    if (_ExpUpdateListThread.m_pEXPManager != NULL)
    {
        _ExpUpdateListThread.m_pEXPManager->UpdateList();
    }

    return false;
}
