#ifndef QCHECKBOX33_H
#define QCHECKBOX33_H

#include <QWidget>

#include <QRadioButton>
class QGroupBox;

class QUIRadioButton : public QRadioButton
{
    Q_OBJECT
public:
    QUIRadioButton(QWidget * parent = 0);

    virtual ~QUIRadioButton();
public:
    // 鼠标放开事件响应函数
    void mousePressEvent(QMouseEvent * e);
    // 鼠标放开事件响应函数
    void mouseReleaseEvent(QMouseEvent * e);
protected:
    void paintEvent(QPaintEvent * pe);
private :
    bool m_bDown;
};

class qCheckBox : public QWidget
{
    Q_OBJECT

public:
    qCheckBox(QWidget * parent = NULL);
    ~qCheckBox();

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

private:
    QUIRadioButton * m_pRadioOn;
    QUIRadioButton * m_pRadioOff;
};

#endif // QCHECKBOX33_H
