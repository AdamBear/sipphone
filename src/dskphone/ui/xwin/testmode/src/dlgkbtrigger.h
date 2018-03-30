#ifndef __DLG_KB_TRIGGER_H__
#define __DLG_KB_TRIGGER_H__

#ifdef _CP920

#include "uimanager/basedialog.h"


class xTextView;

class CDlgKeyTrigger : public xLinearLayout, public CBaseDialog
{
public:
    //取得实例
    static CBaseDialog * CreateInstance();

public:
    CDlgKeyTrigger();
    virtual ~CDlgKeyTrigger();

public:
    virtual void Init();
    virtual void Uninit();

public:
    virtual void loadChildrenElements(xml_node & node);

    virtual bool IsRejectReturnToIdle();

    virtual bool IsShowSoftkey();

public:
    virtual bool ProcessKeyEvent(int nKeyCode, bool bPress);

protected:
    xTextView * m_pTextView;
};

#endif //IF_KEYBOARD_AUTO_TEST
#endif //__DLG_KB_TRIGGER_H__
