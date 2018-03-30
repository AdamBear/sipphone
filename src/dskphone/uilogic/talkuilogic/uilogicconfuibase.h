#ifndef _UILOGIC_CONF_UI_BASE_H
#define _UILOGIC_CONF_UI_BASE_H

#include "uilogicbasetalkui.h"
#include "confprocessor.h"

class CUILogicConfUIBase: public CUILogicBaseTalkUI
{
public:
    CUILogicConfUIBase(void);
    virtual ~CUILogicConfUIBase(void);

public:
    //获取绑定的processor
    virtual CBaseTalkUIProcessor * GetBindUIProcessor();
    //绑定processor
    virtual void BindProcessor(CBaseTalkUIProcessor * pBindProcessor);

public:
    virtual int GetFocusCallID() = 0;

    virtual void ShowSecurityBox(int nAccount) = 0;

    virtual void ShowACDDispCodeMenu(int nCallID) = 0;

    virtual void ShowEmergencyMenu(int nCallID) = 0;

protected:
    //保存的processor
    CConfProcessor * m_pProcessor;
};

#endif
