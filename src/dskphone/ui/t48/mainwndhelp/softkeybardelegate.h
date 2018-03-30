#ifndef _SOFTKEY_BAR_PROXY_H_
#define _SOFTKEY_BAR_PROXY_H_

#include "mainwnd/mainwndwgtdelegatebase.h"
#include <QtCore>

Q_DECLARE_METATYPE(QWidget *);

class qSoftKeyBar;
class CSoftKeyBarDelegate : public CMainWndWgtDelegateBase
{
    Q_OBJECT

public:
    CSoftKeyBarDelegate();
    virtual ~CSoftKeyBarDelegate();

    virtual void DrawWidget(CBaseDialog * pDialog);

    virtual QWidget * GetWidget();

    virtual void SetParent(QWidget * pWgt);

    virtual void SetVisible(bool bVisible);

    virtual void Raise();

    virtual bool Operator(const QString & strAction, const QVariant & var);

    virtual bool ProcessKeyEvent(QObject * pObject, QKeyEvent * pEvent);

    bool IsIMESoftkey(const CSoftKeyBarData& SoftKeyBarData);

private:
    // 根据传入的控件指针设置IME的soft信息
    void DisplayIMESoftkey(QWidget * pFocusWidget);

    // 根据传入的控件指针设置下一个IME的soft信息
    void SwitchNextIMEName(QWidget * pFocusWidget);

    void InsertEDKSoftkeyData(CArraySoftKeyBarData & arrSoftKeyBarData);

    // 设置所有softkeyBar数据
    void RefreshSoftKeyBar(const CArraySoftKeyBarData & arrSoftKeyBarData);

    // 设置某个softkey数据
    void RefreshSoftKeyBar(int iIndex, const CSoftKeyBarData & objSoftkeyBarData);

private Q_SLOTS:
    void OnSoftKeyAction(qSoftButton * pBtn);

private:
    QPointer<qSoftKeyBar> m_pSoftkeyBar;

    CArraySoftKeyBarData m_arrSoftKeyBarData;

};

#endif // _SOFTKEY_BAR_PROXY_H_
