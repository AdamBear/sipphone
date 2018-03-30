#ifndef _UILOGIC_CONFMANAGER_UI_BASE_H
#define _UILOGIC_CONFMANAGER_UI_BASE_H

#include "uilogicbasetalkui.h"
#include "confmanagerprocessor.h"

class CUILogicConfManagerUIBase: public CUILogicBaseTalkUI
{
public:
    CUILogicConfManagerUIBase(void);
    virtual ~CUILogicConfManagerUIBase(void);

public:
    //获取绑定的processor
    virtual CBaseTalkUIProcessor * GetBindUIProcessor();
    //绑定processor
    virtual void BindProcessor(CBaseTalkUIProcessor * pBindProcessor);

public:
    //获取焦点话路callid
    virtual int GetFocusCallID() = 0;

    virtual void ShowACDDispCodeMenu(int nCallID) = 0;

    virtual void ShowEmergencyMenu(int nCallID) = 0;

protected:
    //保存的processor
    CConfManagerProcessor * m_pProcessor;
};


#endif
