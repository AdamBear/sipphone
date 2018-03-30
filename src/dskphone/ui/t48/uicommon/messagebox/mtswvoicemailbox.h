#ifndef MTSWVOICEMAILBOX_H
#define MTSWVOICEMAILBOX_H
#include "config.h"
#if IF_FEATURE_METASWITCH_VOICEMAIL
#include <QDialog>
#include "messageboxbase.h"
#include "uikernel/qlineeditutility.h"
#include "baseui/framelist.h"

class qUIPushButton;
class QLabel;

class CMTSWVoiceMailBox : public CMessageBoxBase
{
    Q_OBJECT

public:
    explicit CMTSWVoiceMailBox(CMaskFrame * pMaskFrame = 0);
    ~CMTSWVoiceMailBox();
    //设置弹窗tile,时间
    void SetMessageBoxInfo(const QString & lpszText = "", int timeTodie = -1);
    //设置已读未读
    void SetRead(bool bRead)
    {
        m_bRead = bRead;
    }
    //设置是否添加到metaswitch联系人
    void SetAddToNetwork(bool bAdd)
    {
        m_bAddToNetwork = bAdd;
    }

    virtual void PrintMsgBoxInfo(bool bAdd) {};

    virtual bool IfClickOutExit()
    {
        return true;
    }

private:
    //设置弹出框背景
    void SetMessageBoxBackground(const QString & strPic);

    virtual bool ProcessKeyEvent(int nKeyCode, bool bPress);

protected slots:
    //响应点击Delete事件
    void OnBtnDeleteClick();
    //响应点击CallBack事件
    void OnBtnCallBackClick();
    //响应点击Mark事件
    void OnBtnMarkClick();
    //响应点击AddToLocal事件
    void OnBtnAddToLocalClick();
    //响应点击AddToNetwork事件
    void OnBtnAddToNetworkClick();

private:
    qUIPushButton * m_pBtnCallBack;
    qUIPushButton * m_pBtnMark;
    qUIPushButton * m_pBtnAddToLocal;
    qUIPushButton * m_pBtnAddToNetwork;
    qUIPushButton * m_pBtnDelete;
    QLabel    *    m_pLabelTitle;
    bool           m_bRead;
    bool           m_bAddToNetwork;
};

#endif
#endif
