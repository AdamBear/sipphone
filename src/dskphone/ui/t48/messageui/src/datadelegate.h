#ifndef DATADELEGATE_H
#define DATADELEGATE_H

#include "settingui/src/settingitembase.h"

class DataDelegate
{
public:
    DataDelegate();
    ~DataDelegate();

    virtual bool CheckItemCanReused();

    virtual bool GetData(int iPage, int & iTotData, QWPVector * pVecWP = NULL);
    virtual bool SetData(void * pData = NULL);
};

#endif
