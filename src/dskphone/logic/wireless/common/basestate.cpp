///////////////////////////////////////////////////////////
//  CBaseState.cpp
//  Implementation of the Class CBaseState
//  Created on:      15-五月-2014 9:18:16
//  Original author: songk
///////////////////////////////////////////////////////////
#if defined(IF_SUPPORT_BLUETOOTH) || defined(IF_SUPPORT_WIFI)
#include "sessionbase.h"

CBaseState::CBaseState():
    m_iTimer((UINT) & m_iTimer),
    m_eState(SESSION_UNINIT_STATE),
    m_pSession(NULL),
    m_lDeviceHandle(-1)
{
}

CBaseState::~CBaseState()
{
}

bool CBaseState::SetTimer(bool bEnable, int iTimeOut)
{
    WIRE_INFO("%s timer id[%d]", bEnable ? "set" : "kill", m_iTimer);
    if (bEnable)
    {
        timerSetThreadTimer(m_iTimer, iTimeOut);
    }
    else
    {
        timerKillThreadTimer(m_iTimer);
    }

    return true;
}

bool CBaseState::ExitState(bool bEndConnect/* = false*/)
{
    if (NULL != m_pSession)
    {
        return m_pSession->ExitSession(bEndConnect);
    }

    return false;
}
#endif
