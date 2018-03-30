#include "modscreensavers.h"
#include "screensaversuilogic/screensavershelp.h"
#include "screensaversuilogic/modscreensaversuilogic.h"
#include "screensaversuilogic/screensaversmanager.h"
#include "screensaversuilogic/screensavercontroller.h"
#include "screensavers/src/screensavers.h"
#if IF_FEATURE_XMLB_SCREENSAVER || IS_COLOR

void ScreenSavers_Init()
{
    UIlogic_ScreenSaversInit();

    g_pScreenSaversHelper->ScreenSaverHelper_Init();

    g_pScreenSaversHelper->SetScreenSavers(g_pDlgScreenSavers);

    g_ScreenSaverManger.setScreenSaverHelpeBase(g_pScreenSaversHelper);
}

void ScreenSavers_Uninit()
{
    UIlogic_ScreenSaversUninit();

    g_pScreenSaversHelper->ScreenSaverHelper_UnInit();
}

void ScreenSavers_XMBLoad()
{
    g_pDlgScreenSavers->OnXMLScreenSavers(true);
}

void ScreenSavers_Exit()
{
    g_ScreenSaverManger.OnExitScreenSavers();
}

#endif
