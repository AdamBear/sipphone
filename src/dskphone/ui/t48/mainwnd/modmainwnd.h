#ifndef _MOD_MAIN_WND_H_
#define _MOD_MAIN_WND_H_

#include "uimanager/uimanager_header.h"
#include "uimanager/uimanager_common.h"

void MainWnd_UpdateSoftkey();

//该接口用于显示出音量条，并且如果当前界面不允许显示，则不会显示
void MainWnd_ShowVolumeBar(bool bShow);

#endif // _MOD_MAIN_WND_H_
