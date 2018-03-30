#ifndef __TRANSPARENCY_MSG_HELPER_H__
#define __TRANSPARENCY_MSG_HELPER_H__

#include <QtGui>

class CTransChangeMsgHelper : public QObject
{
    Q_OBJECT

public:
    CTransChangeMsgHelper() {};
    virtual ~CTransChangeMsgHelper() {};

    void OnTransparencyChange();

Q_SIGNALS:
    void transparencyChange();
};

#define _TransChangeMsgHelper GetTransChangeMsgHelperInstance()
CTransChangeMsgHelper & GetTransChangeMsgHelperInstance();

#endif // #ifndef __TRANSPARENCY_MSG_HELPER_H__
