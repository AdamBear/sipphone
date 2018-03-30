#ifndef _DLG_PING_AND_TRACEOUT_H_
#define _DLG_PING_AND_TRACEOUT_H_

#include "dlgsettingui.h"

class xScrollBar;
class CDlgPingAndTraceOut : public CDlgSettingUI
{
public:
    static CBaseDialog * CreateInstance();

public:
    CDlgPingAndTraceOut();
    virtual ~CDlgPingAndTraceOut();

public:
    virtual void loadChildrenElements(xml_node & node);
    virtual void RefreshUI(bool bReLoadData = false);
protected:
    bool IsScrollBypageDelegate();
protected:
    xScrollBar * m_pScroll;
};

#endif //_DLG_PING_AND_TRACEOUT_H_
