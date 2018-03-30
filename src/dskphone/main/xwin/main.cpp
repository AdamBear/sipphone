#ifndef _WIN32 // win32代码在Win32Main.cpp中
#include "mainstation.h"
#include "interfacedefine.h"
#include "commonunits/modcommonunits.h"


int main(int argc, char * argv[])
{
    comminUnits_DeleteInitInfofile();
    comminUnits_WriteInitInfo("InitTime_LibLoadSucceed = 1");

    CMainStation& objStation = CMainStation::getInstance();

    objStation.Init(argc, argv);

    objStation.m_strThreadName = VP_PHONE_THREAD_NAME;
    objStation.AttachCurrentThread();

    int nResult = objStation.Run();

    objStation.DetachThread();

    return nResult;
}

#endif //#ifndef _WIN32
