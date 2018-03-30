#ifndef _UILOGIC_CONNECT_UI_BASE_H
#define _UILOGIC_CONNECT_UI_BASE_H

#include "connectprocessor.h"
#include "uilogicbasetalkui.h"

class CUILogicConnectUIBase: public CUILogicBaseTalkUI
{
public:
    CUILogicConnectUIBase(void);
    virtual ~CUILogicConnectUIBase(void);

public:
    //获取绑定的processor
    virtual CBaseTalkUIProcessor * GetBindUIProcessor();
    //绑定processor
    virtual void BindProcessor(CBaseTalkUIProcessor * pBindProcessor);

protected:
    //保存的processor
    CConnectProcessor * m_pProcessor;
};

#endif
