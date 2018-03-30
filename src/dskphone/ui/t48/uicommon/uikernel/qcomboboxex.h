#ifndef __Q_COMBOBOX_EX_H__
#define __Q_COMBOBOX_EX_H__

#include <QComboBox>
#include "scrolltext.h"

class qComboBoxEx : public QComboBox
{
    Q_OBJECT

public:
    qComboBoxEx(QWidget * parent = 0);
    virtual ~qComboBoxEx();

protected:
    virtual void focusInEvent(QFocusEvent * pEvent);
    virtual void focusOutEvent(QFocusEvent * pEvent);
    virtual void paintEvent(QPaintEvent * pEvent);
    virtual void resizeEvent(QResizeEvent * pEvent);

    bool IsFocus();
    int GetCurrentTextWidth();

protected:
    QRect m_rcText;

    SCROLL_TEXT_DECLARATION(CurrentText)

public Q_SLOTS:
    void OnSelecteChanged(int nIndex);
    void ScrollCurrentText();
};

#endif // __Q_COMBOBOX_EX_H__
