#ifndef _TEST_STATUS_ICON_H_
#define _TEST_STATUS_ICON_H_

#include <QObject>
#include <string>
#include "common/common_data_define.h"
#include <ylstring.h>
#include <ETLLib.hpp>
#include "idlescreen/include/modidlescreen.h"

class QTimer;

class TTestStatusIcon : public QObject
{
    Q_OBJECT

public:
    TTestStatusIcon();
    ~TTestStatusIcon();

    void StatrTest();

    void StopTest();

private slots:
    void OnTimeOut();

private:
    YLList<IdleStatusItem>      m_listStatusItem;
    QTimer           *          m_pTimer;
    std::string                 m_strText;
};

#define _TestStatusIcon GetTestStatusIcon()
TTestStatusIcon & GetTestStatusIcon();
#endif // _TEST_STATUS_ICON_H_
