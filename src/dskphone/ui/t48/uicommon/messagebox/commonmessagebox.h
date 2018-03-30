#ifndef _COMMON_MESSAGEBOX_H
#define _COMMON_MESSAGEBOX_H

// #include <QDialog>
#include "messageboxbase.h"
// #include "uikernel/qlineeditutility.h"
#include "uikernel/quipushbutton.h"

class qUIPushButton;

class CCommonMessageBox : public CMessageBoxBase
{
    Q_OBJECT
public:
    explicit CCommonMessageBox(CMaskFrame * pMaskFrame = 0, bool bIsShowCountDown = false);
    ~CCommonMessageBox();

    void SetMessageBoxCommonInfo(const QString & lpszText, int timeTodie = -1, int iID = -1);

    void SetMessageBoxSpecialInfo(VecMsgBoxSoftkey & vecSoftkey);

    virtual bool ProcessKeyEvent(int nKeyCode, bool bPress);

    virtual void PrintMsgBoxInfo(bool bAdd);

    virtual bool IfClickOutExit()
    {
        return !m_bHaveButtons;
    }

private:
    //设置弹出框背景
    void SetMessageBoxBackground(const QString & strPic);

private Q_SLOTS:
    void OnCountTimeOut();

private:
    bool        m_bIsShowCountDown;
    int         m_nTimeRemain;
    QTimer   *  m_pRemainTimer;
    QString     m_RemainTimeHint;
    QLabel      m_LabText;

    bool        m_bHaveButtons;
};

#endif // COMMONMESSAGEBOX_H
