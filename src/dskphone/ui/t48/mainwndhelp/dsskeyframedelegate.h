#ifndef _DSSKEY_FRAME_PROXY_H_
#define _DSSKEY_FRAME_PROXY_H_

#include "mainwnd/mainwndwgtdelegatebase.h"
#include <QtCore>

class qDsskeyFrame;
class qDsskeyButton;

class CDsskeyFrameDelegate : public CMainWndWgtDelegateBase
{
    Q_OBJECT

public:
    CDsskeyFrameDelegate();
    virtual ~CDsskeyFrameDelegate();

    virtual void DrawWidget(CBaseDialog * pDialog);

    virtual QWidget * GetWidget()
    {
        return NULL;
    }

    virtual void SetParent(QWidget * pWgt);

    virtual void SetVisible(bool bVisible);

    virtual void Raise();

    virtual bool Operator(const QString & strAction, const QVariant & var);

private slots:
    // 切换Dsskey页面
    void OnDssKeyPageAction();

private:
    void OnDsskeyKeyClick(int iIndex);

private:
    QPointer<qDsskeyFrame>      m_pLeftDsskeyFrame;
    QPointer<qDsskeyFrame>      m_pRightDsskeyFrame;
    QPointer<qDsskeyButton>     m_pPageBtn;
};

#endif // _DSSKEY_FRAME_PROXY_H_
