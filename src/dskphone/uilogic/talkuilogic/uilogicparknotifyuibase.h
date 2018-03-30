#ifndef _UILOGIC_PARKNOTIFY_UI_BASE_H
#define _UILOGIC_PARKNOTIFY_UI_BASE_H

#include "parknotifyprocessor.h"
#include "uilogicbasetalkui.h"

class CUILogicParkNotifyUIBase: public CUILogicBaseTalkUI
{
public:
    CUILogicParkNotifyUIBase(void);
    virtual ~CUILogicParkNotifyUIBase(void);

public:
    //获取绑定的processor
    virtual CBaseTalkUIProcessor * GetBindUIProcessor();
    //绑定processor
    virtual void BindProcessor(CBaseTalkUIProcessor * pBindProcessor);


protected:
    //保存的processor
    CParkNotifyProcessor * m_pProcessor;
};

#endif
