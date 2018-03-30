#include "modadapterboxmock.h"

#include "modadapterbox.h"


AdapterBoxMock * g_pAdapterBoxMock = NULL;

bool AdapterBox_IsTestMode()
{
    return g_pAdapterBoxMock ? g_pAdapterBoxMock->IsTestMode() : false;
}
