#ifndef __DLG_DIR_SEARCH_H__
#define __DLG_DIR_SEARCH_H__


#include "dlgdirectorybase.h"
#include "xwindow-kernel/include/xthreadtimer.h"
#include <baseui/include/baseuidefine.h>


class xEditTextView;

class CDlgDirSearch : public CDlgDirectoryBase, public xTimerHandler
{
public:
    CDlgDirSearch();
    virtual ~CDlgDirSearch();

public:
    static CBaseDialog * CreateInstance();

public:
    virtual void Init();
    virtual void Uninit();

    virtual void OnFocusChanged(bool bFocused);

public:
    virtual void loadChildrenElements(xml_node & node);

    virtual bool onKeyPressedEvent(int nKeyCode);

    virtual bool onFocusMoveEvent(xView * pNewFocus, xView * pOldFocus);
#if IF_HIDE_TITLE_BUT_EDIT
    virtual void RefreshTitle();
#endif

    void SetInitSearchKey(int nKeyCode);

    void slotOnIMEChanged(xSignal * sender, WPARAM wParam, LPARAM lParam, int nDataBytes, LPVOID pData);

protected:
    void OnEditKeyOkClick();
    void OnKeyWordChanged(xSignal * sender, WPARAM wParam, LPARAM lParam, int nDataBytes, LPVOID pData);
    void OnEditDelayTimerOut(xThreadTimer* timer, LPVOID pExtraData);

    void SearchContact(const yl::string & strKey);
    bool IsFocusEdit() const;
    bool ProcessEditKeyPress(int nKey);

    virtual bool OnSoftkeyAction(int iType, bool bLongPress);

protected:
    xThreadTimer m_tmTimer;
    xEditTextView * m_pEdit;
    bool m_bEditInit;
};

#endif
