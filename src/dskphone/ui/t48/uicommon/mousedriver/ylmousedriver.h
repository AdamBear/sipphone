#ifndef YLMOUSEDRIVER_H
#define YLMOUSEDRIVER_H


#include <QtGui/qmouse_qws.h>


class QYealinkTouchScreenHandlerPrivate;

class QYealinkInputMouseHandler : public QWSCalibratedMouseHandler
{
    friend class QYealinkTouchScreenHandlerPrivate;
public:
    explicit QYealinkInputMouseHandler(const QString & = QString(),
                                       const QString & = QString());
    ~QYealinkInputMouseHandler();

    void suspend();
    void resume();

private:
    QYealinkTouchScreenHandlerPrivate * d;

};


#endif // YLMOUSEDRIVER_H
