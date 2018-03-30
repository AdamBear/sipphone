/************************************************************************
* FileName  : QDSKApplication.h (header file)
* Purpose   :
* Date      : 2012/09/01 11:04:56
************************************************************************/

#include "expdefine.h"

#if (not defined _USE_SKIA) &&(not defined _XWIN)

#ifndef _QDSKAPPLICATION_H_
#define _QDSKAPPLICATION_H_

#include "ETLLib.hpp"
#include <QtGui/QApplication>
#include <QTimer>
#include <QSocketNotifier>

class QDSKApplication : public QApplication, public CMKThread
{
    Q_OBJECT
public:

    QDSKApplication(int & argc, char ** argv);
    virtual ~QDSKApplication();

#ifdef WIN32
    virtual bool IsRegisterCallBack()
    {
        return false;
    }
#endif

public Q_SLOTS:
    void OnMsgKitFDActive(int socket);
    void OnTimeOut();

protected:
    void InitMsgKit();
    void UnInitMsgKit();
    void ReleaseNotifier();

private:
    QSocketNotifier * m_pMKNotifier;

    QTimer * m_pTimer;
};

#endif

#endif
