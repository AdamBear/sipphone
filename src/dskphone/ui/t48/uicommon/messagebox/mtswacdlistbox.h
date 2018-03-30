#ifndef MTSWACDLISTBOX_H
#define MTSWACDLISTBOX_H

#include "config.h"

#if IF_FEATURE_METASWITCH
#include <QDialog>
#include "messageboxbase.h"
#include "uikernel/qlineeditutility.h"
#include "baseui/framelist.h"
#include "modmessagebox.h"

class qUIPushButton;
class QLabel;

class CMTSWAcdListBox : public CMessageBoxBase
{
    Q_OBJECT

public:
    explicit CMTSWAcdListBox(CMaskFrame * pMaskFrame = 0);
    ~CMTSWAcdListBox();

    // 初始化数据
    void SetMessageBoxInfo(const QString & lpszText = "", int timeTodie = -1);

    //设置是否已登录
    void SetLoggedBtnMode(EMTSWAcdListShowLoggedMode eShowLoggedMode)
    {
        m_eShowLoggedMode = eShowLoggedMode;
    }

    virtual void PrintMsgBoxInfo(bool bAdd) {};


private:
    //设置弹出框背景
    void SetMessageBoxBackground(const QString & strPic);

    //virtual bool ProcessKeyEvent(int nKeyCode, bool bPress);

protected slots:
    //响应点击Login/Logout事件
    void OnBtnLoginClick();
    //响应点击Detail事件
    void OnBtnDetailClick();
    //响应点击Member事件
    void OnBtnMemberClick();

    //响应点击Cancel事件
    void OnBtnCancelClick();

private:
    QLabel           *          m_pLabelTitle;
    qUIPushButton       *       m_pBtnLogin;
    qUIPushButton       *       m_pBtnDetail;
    qUIPushButton       *       m_pBtnMember;

    qUIPushButton * m_pBtnCancel;

    EMTSWAcdListShowLoggedMode  m_eShowLoggedMode;
};

#endif
#endif
