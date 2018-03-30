#ifndef _DLG_NEW_MESSAGE_H_
#define _DLG_NEW_MESSAGE_H_

#include "dlgsettingui.h"

class xEditTextView;

class CDlgNewMessage : public CDlgSettingUI
{
public:
    static CBaseDialog * CreateInstance();

public:
    CDlgNewMessage();
    virtual ~CDlgNewMessage();

public:
    virtual void Init();
    virtual void Uninit();
    // 弹出框回调处理
    virtual void MessageBoxCallBack(CMessageBoxBase * pMessageBox);
    virtual bool OnSoftkeyAction(int iType, bool bLongPress);
    virtual void loadChildrenElements(xml_node & node);
    virtual void OnReturnToIdle();
    virtual void RefreshTitle();
    virtual void RefreshTitle(const yl::string & strTitle);
    virtual void RefreshUI(bool bReLoadData = false);
    virtual bool onKeyPressedEvent(int key);
protected:
    void OnSoftKeyPress(xSignal * sender, WPARAM wParam, LPARAM lParam, int nDataBytes, LPVOID pData);
    void SetCommonSoftkey();
    void OnExitAction();
protected:
    xEditTextView   *   m_pEdit;
};

#endif //_DLG_NEW_MESSAGE_H_
