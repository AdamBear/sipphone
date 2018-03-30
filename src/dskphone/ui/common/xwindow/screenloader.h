#ifndef __SCREEN_LOADER_H__
#define __SCREEN_LOADER_H__

#include <xwindow/xscreendefine.h>
#include "xGraphic.h"


#if IF_SUPPORT_SCREEN_PLUGIN
class xScreenLoader
{
public:
    xScreenLoader();
    ~xScreenLoader();

public:
    bool Init(const char* pszConfigFile);
    bool UnInit();

    FUN_CREATE_SCREEN GetFunCreateScreen();
    FUN_RELEASE_SCREEN GetFunReleaseScreen();

protected:
    void* m_hLibScreen;
};
#endif

#endif
