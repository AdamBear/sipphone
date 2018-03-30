#ifndef _UILOGIC_TALKING_UI_BASE_H
#define _UILOGIC_TALKING_UI_BASE_H

#include "talkingprocessor.h"
#include "uilogicbasetalkui.h"

class CUILogicTalkingUIBase: public CUILogicBaseTalkUI
{
public:
    CUILogicTalkingUIBase(void);
    virtual ~CUILogicTalkingUIBase(void);

public:
    //获取绑定的processor
    virtual CBaseTalkUIProcessor * GetBindUIProcessor();
    //绑定processor
    virtual void BindProcessor(CBaseTalkUIProcessor * pBindProcessor);

public:

    virtual void ShowSecurityBox(int nAccount) = 0;

    virtual void ShowACDDispCodeMenu(int nCallID) = 0;

    virtual void ShowEmergencyMenu(int nCallID) = 0;

    //处理Pool的softkey
    virtual void DealPool() = 0;

protected:
    //保存的processor
    CTalkingProcessor * m_pProcessor;
};

#endif
