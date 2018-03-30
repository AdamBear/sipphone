#include "zero_common.h"
#include "modzero.h"
#include "zerocontroller.h"
#include "autopcontrol/include/modautopcontrol.h"
#include "idlescreen/include/modidlescreen.h"
#include "dsklog/log.h"

#if IF_BUG_30885
#include "zerosptouchui/include/modzerosptouch.h"
#endif

// 进入Zero的入口函数
void zero_Enter(bool bZeroLanuched/* = false*/)
{
    if (Autop_IsUpdating())
    {
        SETTINGUI_INFO("Zero is already launched");
        return;
    }

    g_pZeroController->SetZeroLaunched(bZeroLanuched);
    g_pZeroController->Enter();
    Autop_StartByZero(bZeroLanuched);
}

bool zero_EnterRPS()
{
    bool bLaunch = Autop_GetUpdateType() == AUT_RPS;
    SETTINGUI_INFO("SettingUI_EnterRPS bLaunch[%d]", bLaunch);
    // 正在执行其他autop过程不执行RPS
    if (!bLaunch
            && Autop_IsUpdating())
    {
        SETTINGUI_INFO("RPS is no launched and autop is updating.");
        return false;
    }

    if (!bLaunch)
    {
        Autop_Start(AUT_RPS);
    }

    // 如果已经处于RPS界面的话已经检查过网络了,不需要重新进网络检测界面
    if ((bLaunch && idleScreen_IsNetworkAvailable())
            || zero_CheckingNetwork())
    {
        Autop_StartRPSConnect();
    }
    return TRUE;
}

bool zero_CheckingNetwork()
{
    return g_pZeroController->CheckingNetwork();
}

// 退出Zero
void zero_Exit(bool bExitAutoP/* = true*/)
{
    if (bExitAutoP)
    {
        g_pZeroController->Exit();
    }

    CZeroController::ReleaseInstance();
}

// 根据当前页面与softkey操作,进入下一个页面
void zero_EnterNextStage(const yl::string & strCurrentPageAction,
                         const yl::string & strSoftkeyAction)
{
    g_pZeroController->EnterNextStage(strCurrentPageAction, strSoftkeyAction);
}

void zero_SetRedirectData(const yl::string strUserName, const yl::string strPassword)
{
    g_pZeroController->SetRedirectData(strUserName, strPassword);
}

#if IF_BUG_30885
bool zero_BeginAutop()
{
    // 检查网络
    ZeroSPTouch_OpenDlg(ZERO_NETWORK_CHECK_PAGE, true);
    if (!Autop_WaitingForNetwork())
    {
        // 显示网络不可用界面
        ZeroSPTouch_OpenDlg(ZERO_NETWORK_FAIL_PAGE, true);
        return false;
    }
    else
    {
        // 进行autop
        if (!g_pZeroController->BeginAutop())
        {
            ZeroSPTouch_OpenDlg(ZERO_UPDATEFAIL_PAGE);
            return false;
        }
    }

    return true;
}
#endif

bool zero_IsZeroStart()
{
    return AUT_ZERO == Autop_GetUpdateType();
}

bool zero_EnterRPSWaitting()
{
#if IF_FEATURE_POWER_ON_RPS
    SETTINGUI_INFO("SettingUI_EnterRPSWaitting");
    if (!Autop_IsLaunchRPS())
    {
        SETTINGUI_INFO("Not Need To Launch RPS");
        return false;
    }

    if (Autop_IsUpdating())
    {
        SETTINGUI_WARN("Autop is updating, RPS is not allow");
        return false;
    }

    Autop_Start(AUT_RPS);

    if (zero_CheckingNetwork())
    {
        g_pZeroController->EnterPage(ZERO_REDIRECTOR_NOTE);
    }
#endif
    return true;
}
