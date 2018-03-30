#ifndef _UILOGIC_RING_UI_BASE_H
#define _UILOGIC_RING_UI_BASE_H

#include "ringprocessor.h"
#include "uilogicbasetalkui.h"

class CUILogicRingUIBase: public CUILogicBaseTalkUI
{
public:
    CUILogicRingUIBase(void);
    virtual ~CUILogicRingUIBase(void);

public:
    //获取绑定的processor
    virtual CBaseTalkUIProcessor * GetBindUIProcessor();
    //绑定processor
    virtual void BindProcessor(CBaseTalkUIProcessor * pBindProcessor);

protected:
    //保存的processor
    CRingProcessor * m_pProcessor;
};

#endif
