#include "modtestmode.h"
#include "uimanager/moduimanager.h"
#include "testphone.h"
#include "test_include.h"
#include "commonunits/commonunits_def.h"
#include "commonunits/modcommonunits.h"
#include "devicelib/networkserver.h"
#include "voice/include/modvoice.h"
#include "lamp/ledlamp//include/modledlamp.h"
#include "lamp/powersaving/include/modpowersaving.h"
#include "dsklog/log.h"
#include "xwindow/xWindowManager.h"
#include "cdlgtestmode.h"
#include "dlgkbtrigger.h"

void testmode_Init()
{
    REGISTER_DLG(CDlgTestMode);

#ifdef _CP920
    REGISTER_DLG(CDlgKeyTrigger);
#endif
}

bool testmode_IsTestMode()
{
    return (NULL != UIManager_GetPrivateDlg(DLG_CDlgTestMode)
#ifdef _CP920
            || NULL != UIManager_GetPrivateDlg(DLG_KB_TRIGGER)
#endif
           );
}

void testmode_Enter(int iTestType)
{
    if (testmode_IsTestMode())
    {
        return ;
    }

    TESTMODE_INFO("testmode_Enter : iTestType(%d)", iTestType);

    // 禁止节电模式
    PowerSaving_SetAvoidPowerSaving(true);

    // 进入测试模式 取消外部对灯的控制
    ledlamp_SwitchControl(false);

#ifdef _CP920
    if (TST_ERROR_TRIGGER == iTestType)
    {
        UIManager_CreatePrivateDlg(DLG_KB_TRIGGER);
        return ;
    }
#endif

    // 获取窗口的指针
    CBaseDialog * pDlgBase = UIManager_CreatePrivateDlg(DLG_CDlgTestMode);
    CDlgTestMode * pTestDlg = static_cast<CDlgTestMode *>(pDlgBase);

    if (NULL != pTestDlg)
    {
        pTestDlg->InitTestPhone(iTestType);
    }

    TESTMODE_INFO("Before killall macd!");
    commonUnits_System("killall macd");
    commonUnits_System("nice -n 19 /boot/bin/macd &");
    TESTMODE_INFO("After nice -n 19 /boot/bin/macd &");
}

void testmode_Exit()
{
    if (!testmode_IsTestMode())
    {
        return ;
    }

    PowerSaving_SetAvoidPowerSaving(false);

    // 退出测试模式 恢复外部对灯的控制
    ledlamp_SwitchControl(true);

    exp_SetExpTestMode(false);

    voice_SwitchChannel(CHANNEL_AUTO);

    UIManager_PutPrivateDlg(DLG_CDlgTestMode);

#ifdef _CP920
    UIManager_PutPrivateDlg(DLG_KB_TRIGGER);
#endif

    if (!commonUnit_IsMatchTestMode())
    {
        commonUnits_System("killall macd");
    }
}
