#ifndef __MAIN_STATION_H__
#define __MAIN_STATION_H__

#define IF_SCREEN_PLUGIN_DEBUG 1

#include "xwindow/xWindowManager.h"

#if !IF_SCREEN_PLUGIN_DEBUG && IF_SUPPORT_SCREEN_PLUGIN
#include "xwindow/screenloader.h"
#endif
#if IF_CUSTOM_SCREEN_DEVICE && defined(_WIN32)
#include "Win32ScreenDevice.h"
#endif


class CPhoneDev;
class CMainStation : public xWorkStation
{
public:
    static bool PreKeyHookBegin(int nPhoneKeyCode, bool bPress);
    static bool PreKeyHookEnd(int nPhoneKeyCode, bool bPress);

public:
    virtual BOOL AfterThreadLaunched();
    virtual void BeforeThreadShutdown();

#ifdef _WIN32
    void Init(LPWSTR lpCmdLine, int nCmdShow);
#if IF_CUSTOM_SCREEN_DEVICE
    CWin32ScreenDevice* GetScreenDevice();
#endif
#else
    void Init(int argc, char * argv[]);
#endif

private:
    void ParseRunCommand(int argc, char * argv[]);

#ifndef _WIN32
    BOOL OnHandlerMsgKit(msgObject &);
#endif

    BOOL OnReadKeyEvent(msgObject &);
    void OnSpecialKeyChange(int & nKey, bool & bPress);

    void InitModules();

    // 上电第一次各模块处理慢的问题
    void PreProcesser();
    // 设置适配函数
    void SetAdapterBox();

    static bool IsPowerSavingAllowThroughKey(int nKey);
    static bool OnGlobalKeyPress(int nKey);
    static void PlayKeyTone(int nKey);

public:
    CMainStation();
    ~CMainStation();

    static CMainStation & getInstance();

    virtual void dispatchKeypadEvent(int nKey, bool bPress);

protected:
#if !IF_SCREEN_PLUGIN_DEBUG && IF_SUPPORT_SCREEN_PLUGIN
    xScreenLoader       m_objScreenLoader;
#endif
#if IF_CUSTOM_SCREEN_DEVICE && defined(_WIN32)
    CWin32ScreenDevice* m_pScreenDevice;
#endif
    CPhoneDev     *     m_pDev;
};

#endif //__MAIN_STATION_H__
