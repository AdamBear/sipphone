#include "modvolumebar.h"
#include "uimanager_header.h"
#include "uimanager_inc.h"

#include "autovolumebar.h"

void Volumebar_SetVolumeBarVisible(bool bVisible)
{
    _UIManager.SetVolumeBarVisible(bVisible);
}

void Volumebar_ConnectVisibleChangeSlot(xSlotHandler * pHandler, slotMethod pMethod)
{
    CAutoVolumeBar * pVolumebar = _UIManager.GetVolumeBar();
    if (NULL == pVolumebar)
    {
        return;
    }

    pVolumebar->ConnectVisibleChangeSlot(pHandler, pMethod);
}

void Volumebar_ConnectVolumeChangeSlot(xSlotHandler * pHandler, slotMethod pMethod)
{
    CAutoVolumeBar * pVolumebar = _UIManager.GetVolumeBar();
    if (NULL == pVolumebar)
    {
        return;
    }

    pVolumebar->ConnectVolumeChangeSlot(pHandler, pMethod);
}

void Volumebar_DisconnectVisibleChangeSlot(xSlotHandler * pHandler, slotMethod pMethod)
{
    CAutoVolumeBar * pVolumebar = _UIManager.GetVolumeBar();
    if (NULL == pVolumebar)
    {
        return;
    }

    pVolumebar->DisconnectVisibleChangeSlot(pHandler, pMethod);
}

void Volumebar_DisconnectVolumeChangeSlot(xSlotHandler * pHandler, slotMethod pMethod)
{
    CAutoVolumeBar * pVolumebar = _UIManager.GetVolumeBar();
    if (NULL == pVolumebar)
    {
        return;
    }

    pVolumebar->DisconnectVolumeChangeSlot(pHandler, pMethod);
}
