#include "screenloader.h"
#include "dsklog/log.h"
#include <dlfcn.h>


#if IF_SUPPORT_SCREEN_PLUGIN
namespace
{
#define SCREEN_PLUGIN_SECTION "Library"
#define SCREEN_PLUGIN_KEY "Name"
#define SYMBOL_CREATE_SCREEN "CreateScreen"
#define SYMBOL_RELEASE_SCREEN "ReleaseScreen"
}

xScreenLoader::xScreenLoader()
    : m_hLibScreen(NULL)
{

}

xScreenLoader::~xScreenLoader()
{

}

bool xScreenLoader::Init(const char* pszConfigFile)
{
    if (NULL == pszConfigFile)
    {
        return false;
    }

    if (NULL != m_hLibScreen)
    {
        UnInit();
    }

    chIniFileA file(pszConfigFile);

    phraseConstTextA textSection(SCREEN_PLUGIN_SECTION);
    phraseConstTextA textKey(SCREEN_PLUGIN_KEY);
    yl::string strLibrary = file.GetTextValue(textSection, textKey, "").c_str();

    if (strLibrary.empty())
    {
        XWINDOW_ERR("Screen Config File Error.Path=%s", pszConfigFile);
        return false;
    }

    strLibrary = "lib" + strLibrary + ".so";

    m_hLibScreen = dlopen(strLibrary.c_str(), (RTLD_NOW | RTLD_LOCAL));

    if (NULL == m_hLibScreen)
    {
        XWINDOW_ERR("Screen Plugin Load Fail.Path=%s", strLibrary.c_str());
        return false;
    }

    return true;
}

bool xScreenLoader::UnInit()
{
    if (NULL == m_hLibScreen)
    {
        return false;
    }

    dlclose(m_hLibScreen);
    m_hLibScreen = NULL;

    return true;
}

FUN_CREATE_SCREEN xScreenLoader::GetFunCreateScreen()
{
    if (NULL == m_hLibScreen)
    {
        return NULL;
    }

    return (FUN_CREATE_SCREEN)dlsym(m_hLibScreen, SYMBOL_CREATE_SCREEN);
}

FUN_RELEASE_SCREEN xScreenLoader::GetFunReleaseScreen()
{
    if (NULL == m_hLibScreen)
    {
        return NULL;
    }

    return (FUN_RELEASE_SCREEN)dlsym(m_hLibScreen, SYMBOL_RELEASE_SCREEN);
}
#endif
