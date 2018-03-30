#ifndef MSGDATADELEGATE_H
#define MSGDATADELEGATE_H

#include "datadelegate.h"
#include <QWidget>

namespace
{
#define PAGE_MAX_NUM        7
}

class MsgDataDelegate : public QObject, public DataDelegate
{
public:
    MsgDataDelegate();
    virtual ~MsgDataDelegate();

    virtual bool CheckItemCanReused();

    virtual bool GetData(int iPage, int & iTotData, QWPVector * pVecWP = NULL);
    virtual bool SetData(void * pData = NULL);
};

#endif
