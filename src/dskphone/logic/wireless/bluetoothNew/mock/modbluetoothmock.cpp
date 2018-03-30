#include "modbluetoothmock.h"

#include "modbluetooth.h"


BlueToothMock * g_pBlueToothMock = NULL;

void Bluetooth_HSStopRing()
{
    if (g_pBlueToothMock)
    {
        g_pBlueToothMock->HSStopRing();
    }
}
