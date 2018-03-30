///////////////////////////////////////////////////////////
//  CBtState.h
//  Implementation of the Class CBtState
//  Created on:      2014年11月24日19:56:01
//  Original author: songk
///////////////////////////////////////////////////////////
#ifdef IF_SUPPORT_BLUETOOTH
#ifndef __BTSTATE_H__
#define __BTSTATE_H__
#include "../include/btdefines.h"
class CBtState : public CBaseState
{
public:
    CBtState(void)
    {
        m_lDeviceHandle = BT_DEVICE_HANDLE_INVAILED ;
        m_pSessionInfo = NULL;
    }

    virtual ~CBtState(void)
    {
    }

    BTSessionInfo * GetSessionInfo()
    {
        return m_pSessionInfo;
    }
    void SetSessionInfo(BTSessionInfo * pSessionInfo)
    {
        m_pSessionInfo = pSessionInfo;
    }
    void DeleteSessionInfo()
    {
        SAFE_DELETE(m_pSessionInfo);
    }
protected:
    BTSessionInfo * m_pSessionInfo;
};

#endif //__BTSTATE_H__
#endif // IF_SUPPORT_BLUETOOTH
