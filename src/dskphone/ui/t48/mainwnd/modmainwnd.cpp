#include "modmainwnd.h"

#include "mainwnd.h"
#include "uimanager/moduimanager.h"


void MainWnd_UpdateSoftkey()
{
    _MainWND.DrawSoftkey(UIManager_GetTopWindow());
}

void MainWnd_ShowVolumeBar(bool bShow)
{
    _MainWND.SetVolumeBarVisible(bShow);
}
