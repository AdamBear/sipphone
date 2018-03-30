#ifndef __Q_COMBOBOX_LABEL_H__
#define __Q_COMBOBOX_LABEL_H__

#include "config.h"

#ifdef IF_FEATURE_COMBOBOXLABEL

#include "qcheckbox.h"

class qComboBoxLabel : public QWidget
{
    Q_OBJECT

public:
    qComboBoxLabel(QWidget * parent = 0);
    virtual ~qComboBoxLabel();

    void SetLabel(const yl::string & strLabel);

    void SetData(bool bFlag);
    bool GetData() const;
    void SetContent(const QString & strTextOn, const QString & strTextOff);

    QRadioButton * GetRadioButtonOn() const;
protected:
    virtual void focusInEvent(QFocusEvent * event);
Q_SIGNALS:
    void checkChanged(bool bChecked);
private slots:
    void OnSelectChange();

protected:
    virtual void paintEvent(QPaintEvent * pEvent);

private:
    int       m_fOffset;
    QString   m_strLabel;

    QUIRadioButton * m_pRadioOn;
    QUIRadioButton * m_pRadioOff;
};

#endif

#endif // __Q_COMBOBOX_LABEL_H__
