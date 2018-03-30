#ifndef _TITLE_BAR_PROXY_H_
#define _TITLE_BAR_PROXY_H_

#include "mainwnd/mainwndwgtdelegatebase.h"
#include <QtCore>

class CTitleBar;
class CTitleBarDelegate : public CMainWndWgtDelegateBase
{
    Q_OBJECT

public:
    CTitleBarDelegate();
    virtual ~CTitleBarDelegate();

    virtual void DrawWidget(CBaseDialog * pDialog);

    virtual QWidget * GetWidget();

    virtual void SetParent(QWidget * pWgt);

    virtual void SetVisible(bool bVisible);

    virtual void Raise();

    virtual bool Operator(const QString & strAction, const QVariant & var);

private slots:
    void OnTitleBarSelectAccount(int nAccountID);

private:
    QPointer<CTitleBar> m_pTitleBar;
};

#endif // _TITLE_BAR_PROXY_H_
