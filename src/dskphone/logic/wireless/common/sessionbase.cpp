///////////////////////////////////////////////////////////
//  CSessionBase.cpp
//  Implementation of the Class CSessionBase
//  Created on:      15-五月-2014 9:17:21
//  Original author: songk
///////////////////////////////////////////////////////////
#if defined(IF_SUPPORT_BLUETOOTH) || defined(IF_SUPPORT_WIFI)
#include "sessionbase.h"
#include "time.h"

long CSessionBase::m_lIncreasedID = 0;
long CSessionBase::m_lSessionCount = 0;

CSessionBase::CSessionBase()
{
    m_lSessionID = CSessionBase::GenerateSessionID();
    m_lSessionCount++;
    m_pState = NULL;
}

CSessionBase::~CSessionBase()
{
    if (m_lSessionCount > 0)
    {
        m_lSessionCount--;

        if (m_lSessionCount <= 0)
        {
            CSessionBase::ResetSessionID();
        }
    }
}

long CSessionBase::GenerateSessionID()
{
    time_t tRawTime;
    time(&tRawTime);

    return ((tRawTime) + (++ m_lIncreasedID) * 1000);
}

//回调消息处理
bool CSessionBase::ProcessCallbackEvent(msgObject & objMsg)
{
    if (NULL != m_pState)
    {
        return m_pState->ProcessCallbackEvent(objMsg);
    }
    return true;
}

//用户消息处理
bool CSessionBase::ProcessETLMsg(msgObject & objMsg)
{
    if (NULL != m_pState)
    {
        return m_pState->ProcessETLMsg(objMsg);
    }
    return true;
}

//处理事件操作
bool CSessionBase::ProcessOption(const WirelessSessionData & wsData)
{
    if (NULL != m_pState)
    {
        return m_pState->ProcessOption(wsData);
    }

    return false;
}

//定时器消息处理
bool CSessionBase::ProcessTimer(int iTimerID)
{
    if (NULL != m_pState)
    {
        return m_pState->ProcessTimer(iTimerID);
    }
    return false;
}
#endif
