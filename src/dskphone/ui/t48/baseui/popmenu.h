#ifndef CPOPMENU_H
#define CPOPMENU_H

#include <QtGui/QMenu>
#include <QtGui/QPixmap>
#include <QtCore/QMap>
#include <QtCore/QVector>
#include "uimanager/basedialog.h"

class CPopMenu : public QMenu
{
    Q_OBJECT

private:
    typedef QMap<QString, QAction *> ActionMap; //<action_name, action_pointer>
    typedef QVector<QString>    ActionSeq; //action的顺序

public:
    CPopMenu(QWidget * parent = 0);
    virtual ~CPopMenu();

public:
    void GetSoftKeyData(CArraySoftKeyBarData & sftKeyArr);

    bool AddAction(const QString & strTitle, const QString & strStrAction);

    bool RemoveAction(const QString & strStrAction);

    bool ClearActions();

    bool SetActionVisible(const QString & strStrAction, bool bVisible);

    void Connect(QObject * pReceiver, const char * szSlot);

    void SetFocusAction(const QString & strAction);

    void FocusFirstAction();

    virtual QSize sizeHint() const;

protected:
    virtual void showEvent(QShowEvent * e);

    virtual void hideEvent(QHideEvent * e);

    virtual void keyPressEvent(QKeyEvent * e);

protected:
    QAction * addAction(const QString & strTitle, const QString & strStrAction);

Q_SIGNALS:
    void ActionTrigger(const QString & strAction);
    void CanceleTriggered();
    void MenuHide();

private Q_SLOTS:
    void OnOkTriggered();
    void OnActionTriggered();
    void OnCancelTriggered();

private:
    //标识是否是为自动隐藏，不是因为选中action而触发的
    bool m_bAutoHide;
    ActionMap m_actionMap;
    ActionSeq  m_actionSeq;
};

#endif // CPOPMENU_H
