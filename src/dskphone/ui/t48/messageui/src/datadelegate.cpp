#include "datadelegate.h"

DataDelegate::DataDelegate()
{
    //
}

DataDelegate::~DataDelegate()
{
    //
}

bool DataDelegate::CheckItemCanReused()
{
    return true;
}

bool DataDelegate::GetData(int iPage, int & iTotData, QWPVector * pVecWP /*= NULL*/)
{
    return true;
}

bool DataDelegate::SetData(void * pData /*= NULL*/)
{
    return true;
}
