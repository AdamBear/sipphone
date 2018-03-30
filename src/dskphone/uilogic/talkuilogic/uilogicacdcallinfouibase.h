#ifndef _UILOGIC_ACDCALLINFO_UIBASE_H
#define _UILOGIC_ACDCALLINFO_UIBASE_H

#include "uilogicbasetalkui.h"
#include "acdcallinfoprocessor.h"

class CUILogicACDCallInfoUIBase: public CUILogicBaseTalkUI
{
public:
    CUILogicACDCallInfoUIBase(void);
    virtual ~CUILogicACDCallInfoUIBase(void);

public:
    //获取绑定的processor
    virtual CBaseTalkUIProcessor * GetBindUIProcessor();
    //绑定processor
    virtual void BindProcessor(CBaseTalkUIProcessor * pBindProcessor);

protected:
    //保存的processor
    CACDCallInfoProcessor * m_pProcessor;

};

#endif
