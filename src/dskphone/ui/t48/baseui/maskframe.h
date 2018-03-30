#ifndef __MASK_FRAME_H__
#define __MASK_FRAME_H__

#include <QWidget>

class CMaskFrame : public QWidget
{
    Q_OBJECT

public:
    CMaskFrame(QWidget * parent);
    ~CMaskFrame();

public:
    void ResetDown();
    void SetMaskRect(const QRect & rcMask);

    virtual bool eventFilter(QObject * pObject, QEvent * pEvent);

Q_SIGNALS:
    void click();

protected:
    virtual void paintEvent(QPaintEvent * pEvent);

private:
    bool m_bDown;
    QRect m_rcMask;
};

#endif // __MASK_FRAME_H__
