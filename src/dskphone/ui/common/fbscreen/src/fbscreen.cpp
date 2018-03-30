#include "fbscreen.h"
#include "xframebufferscreen.h"

static xFrameBufferScreen* g_pScreen = NULL;

#ifdef __cplusplus
extern "C" {
#endif

#if IF_CUSTOM_SCREEN_DEVICE
FB_SCREEN_EXPORT xScreen* CreateCustomScreen(IScreenDevice* pDevice)
{
    if (NULL != g_pScreen)
    {
        return g_pScreen;
    }

    g_pScreen = new xFrameBufferScreen;

    if (NULL != g_pScreen)
    {
        g_pScreen->SetScreenDevice(pDevice);
    }

    return g_pScreen;
}
#endif

FB_SCREEN_EXPORT xScreen* CreateScreen()
{
    if (NULL != g_pScreen)
    {
        return g_pScreen;
    }

    g_pScreen = new xFrameBufferScreen;

    return g_pScreen;
}

FB_SCREEN_EXPORT bool ReleaseScreen(xScreen* pScreen)
{
    if (NULL == g_pScreen || pScreen != g_pScreen)
    {
        return false;
    }

    delete g_pScreen;

    g_pScreen = NULL;

    return true;
}

#ifdef __cplusplus
};
#endif
