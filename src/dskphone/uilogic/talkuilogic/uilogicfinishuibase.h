#ifndef _UILOGIC_FINISH_UI_BASE_H
#define _UILOGIC_FINISH_UI_BASE_H

#include "finishprocessor.h"
#include "uilogicbasetalkui.h"

class CUILogicFinishUIBase: public CUILogicBaseTalkUI
{
public:
    CUILogicFinishUIBase(void);
    virtual ~CUILogicFinishUIBase(void);

public:
    //获取绑定的processor
    virtual CBaseTalkUIProcessor * GetBindUIProcessor();
    //绑定processor
    virtual void BindProcessor(CBaseTalkUIProcessor * pBindProcessor);


protected:
    //保存的processor
    CFinishProcessor * m_pProcessor;
};

#endif
