#ifndef __MOD_VOLUMEBAR__
#define __MOD_VOLUMEBAR__

#include <ETLLib/ETLLib.hpp>
#include "xwindow-kernel/include/xsignal.h"

//设置音量条的可见性
void Volumebar_SetVolumeBarVisible(bool bVisible);

void Volumebar_ConnectVisibleChangeSlot(xSlotHandler * pHandler, slotMethod pMethod);
void Volumebar_ConnectVolumeChangeSlot(xSlotHandler * pHandler, slotMethod pMethod);

void Volumebar_DisconnectVisibleChangeSlot(xSlotHandler * pHandler, slotMethod pMethod);
void Volumebar_DisconnectVolumeChangeSlot(xSlotHandler * pHandler, slotMethod pMethod);


#endif // __MOD_VOLUMEBAR__
