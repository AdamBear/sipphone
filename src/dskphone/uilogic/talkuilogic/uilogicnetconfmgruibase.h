#ifndef _UILOGIC_NETCONF_MGR_UI_BASE_H
#define _UILOGIC_NETCONF_MGR_UI_BASE_H

#include "netconfmgrprocessor.h"
#include "uilogicbasetalkui.h"

class CUILogicNetConfMgrUIBase: public CUILogicBaseTalkUI
{
public:
    CUILogicNetConfMgrUIBase(void);
    virtual ~CUILogicNetConfMgrUIBase(void);

public:
    //获取绑定的processor
    virtual CBaseTalkUIProcessor * GetBindUIProcessor();
    //绑定processor
    virtual void BindProcessor(CBaseTalkUIProcessor * pBindProcessor);

public:
    //获取焦点话路callid
    virtual int GetFocusCallID() = 0;

    virtual void ShowRemoveMemberBox(yl::string strName) = 0;

protected:
    //保存的processor
    CNetConfMgrProcessor * m_pProcessor;
};

#endif
