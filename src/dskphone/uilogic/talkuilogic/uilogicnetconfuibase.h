#ifndef _UILOGIC_NETCONF_UI_BASE_H
#define _UILOGIC_NETCONF_UI_BASE_H

#include "netconfprocessor.h"
#include "uilogictalkinguibase.h"

class CUILogicNetConfUIBase : public CUILogicTalkingUIBase
{
public:
    CUILogicNetConfUIBase(void);
    virtual ~CUILogicNetConfUIBase(void);

public:
    //获取绑定的processor
    virtual CBaseTalkUIProcessor * GetBindUIProcessor();
    //绑定processor
    virtual void BindProcessor(CBaseTalkUIProcessor * pBindProcessor);

protected:
    //保存的processor
    CNetConfProcessor * m_pProcessor;
};

#endif
