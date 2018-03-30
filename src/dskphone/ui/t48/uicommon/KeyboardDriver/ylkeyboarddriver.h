#ifndef YL_KEYBOARD_DRIVER_H
#define YL_KEYBOARD_DRIVER_H


#include <QtGui/qkbd_qws.h>


class QYealinkKbPrivate;

class QYealinkInputKeyboardHandler : public QWSKeyboardHandler
{
public:
    QYealinkInputKeyboardHandler(const QString &);
    virtual ~QYealinkInputKeyboardHandler();

    virtual bool filterInputEvent(quint16 & input_code, qint32 & input_value);

private:
    QYealinkKbPrivate * d;
};


#endif // YL_KEYBOARD_DRIVER_H
