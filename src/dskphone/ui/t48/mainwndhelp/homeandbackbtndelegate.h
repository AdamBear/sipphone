#ifndef _HOME_AND_BACK_BTN_PROXY_H_
#define _HOME_AND_BACK_BTN_PROXY_H_

#include "mainwnd/mainwndwgtdelegatebase.h"
#include <QtCore>

class qDsskeyFrame;
class qDsskeyButton;

class CHomeAndBackBtnDelegate : public CMainWndWgtDelegateBase
{
    Q_OBJECT

public:
    CHomeAndBackBtnDelegate();
    virtual ~CHomeAndBackBtnDelegate();

    virtual void DrawWidget(CBaseDialog * pDialog);

    virtual QWidget * GetWidget()
    {
        return NULL;
    }

    virtual void SetParent(QWidget * pWgt);

    virtual void SetVisible(bool bVisible);

    virtual void Raise();

private slots:
    // 响应点击BackBtn时候的操作
    void OnBackBtnClick();

    // 响应点击HomeBtn时候的操作
    void OnHomeBtnClick();

private:
    QPointer<qUIPushButton>     m_pBackBtn;
    QPointer<qUIPushButton>     m_pHomeBtn;
};

#endif
