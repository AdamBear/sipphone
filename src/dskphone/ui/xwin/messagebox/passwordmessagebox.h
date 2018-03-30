#ifndef __PASSWORD_MESSAGEBOX_H__
#define __PASSWORD_MESSAGEBOX_H__

#include "messageboxbase.h"


class xEditTextView;
class CPasswordMessageBox : public CMessageBoxBase
{
public:
    CPasswordMessageBox();
    virtual ~CPasswordMessageBox();

    static CMessageBoxBase * CreatePasswordMsgBox(const MSGBOX_INFO & stMsgInfo);

public:
    virtual void loadChildrenElements(xml_node & node);

    virtual bool ProcessKeyEvent(int nKeyCode, bool bPress);

    // 更新softkey数据
    virtual void UpdateSoftkeyData();

protected:
    virtual bool OnSoftkeyAction(int nType, bool bLongPress);

    void ShowErrorMessageBox(const yl::string & strNote);

	//退出并输出密码
    virtual void outputPassword();

public:
	yl::string getPassword()
    {
		return m_strPassword;
    }

protected:
    xEditTextView   *   m_pEditPwd;
    int                 m_nInputMinLimit;
    bool                m_bProcessOKBtn;

	yl::string m_strPassword;
};

#endif // __PASSWORD_MESSAGEBOX_H__
