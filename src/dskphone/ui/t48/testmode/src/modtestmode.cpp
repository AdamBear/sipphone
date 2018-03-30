#include "modtestmode.h"
#include "testmode_common.h"
#include "uimanager/dialogfactory.h"
#include "testphonet4x.h"
#include "cdlgtestmode.h"
#include "uimanager/uimanager_inc.h"
#include "devicelib/networkserver.h"
#include "commonunits/commonunits_def.h"
#include "lamp/ledlamp//include/modledlamp.h"
#include "idle/phonestatemessagedefine.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "commonunits/modcommonunits.h"
#include "settingui/include/modsettingui.h"
#include "lamp/powersaving/include/modpowersaving.h"
#ifdef IF_USB_SUPPORT
#include "record/include/recordmessagedefine.h"
#endif
#include <devicelib/phonedevice.h>
#include "devicelib/phonedevice.h"
#ifndef WIN32
#include <qwindowsystem_qws.h>
#endif
#include <stdlib.h>
#include "voice/include/modvoice.h"
#include "exp/src/expdrive/deviceplugin/modexpscan.h"

TestMode_Type CurTestmode = TST_NONE;


// 处理USB连接消息
static LRESULT Testmode_ProcessMSG(msgObject & msg)
{
    TESTMODE_INFO("Testmode_ProcessMSG 0x[%x] 0x[%x]", \
                  msg.message, msg.wParam);

    CDlgTestMode * pDlgTestmode = qobject_cast<CDlgTestMode *>(UIManager_GetPrivateDlg("CDlgTestMode"));
    if (NULL == pDlgTestmode || pDlgTestmode != UIManager_GetTopWindow())
    {
        return 0;
    }
    switch (msg.message)
    {
#ifdef IF_USB_SUPPORT
    case RECORD_STORAGE_STATE_CHANGE:
        {
            if (NULL != g_pTestPhone)
            {
                if (msg.wParam == 0)
                {
                    g_pTestPhone->SetUSBState(USB_REMOVE);
                }
                else if (msg.wParam == 1)
                {
                    g_pTestPhone->SetUSBState(USB_INSERT);
                }
            }
        }
#else
    case PHONESTATE_TEST_USB:
        {
            if (NULL != g_pTestPhone)
            {
                g_pTestPhone->SetUSBState((USB_STATE)msg.wParam);
            }
        }
        break;
#endif
    default:
        break;
    }
    return 0;

}
void TestMode_EnterScreenCheck()
{
#ifndef WIN32
    qwsServer->closeKeyboard();
    system(PHONE_BIN_T48_SCREEN_CALIBRATION_PATH);
    qwsServer->closeMouse();
    qwsServer->openMouse();
    qwsServer->openKeyboard();
#endif
}
// 初始化
void testmode_Init()
{
    TESTMODE_INFO("testmode_Init ");
    REGISTER_DLG(CDlgTestMode);
#ifdef IF_USB_SUPPORT
    etl_RegisterMsgHandle(RECORD_STORAGE_STATE_CHANGE, RECORD_STORAGE_STATE_CHANGE, \
                          Testmode_ProcessMSG);
#else
    etl_RegisterMsgHandle(PHONESTATE_TEST_USB, PHONESTATE_TEST_USB, \
                          Testmode_ProcessMSG);
#endif
}

void testmode_Enter(int val)//lcm给别人调用开始测试模式
{
    TESTMODE_INFO("testmode_Enter : val(%d)", val);

    if (val == TST_VIEWSTATUS)
    {
        SettingUI_OpenTestModeStatus();
        return;
    }


    if (TST_NONE == CurTestmode)
    {
        //T48只支持三种测试类型，整机音频（#*02）以及普通话机（#*9），
        //新增半成品测试模式
        if (TST_FULLVOICE != val && TST_NORMAL != val && TST_PREBURNING != val
                && TST_SFP != val)
        {
            return;
        }
        CurTestmode = (TestMode_Type)val;
        CBaseDialog * pDlg = UIManager_CreatePrivateDlg((DLG_CDlgTestMode));
        if (NULL == pDlg)
        {
            return ;
        }
        // http://10.2.1.199/Bug.php?BugID=34706
        // 在测试模式开启时，才启动macd，退出测试模式再退出macd
        system(BOOT_PATH "bin/macd &");
        exp_SetExpTestMode(true);
        if (val == TST_FULLVOICE)
        {
            g_pTestPhone->TestFullVoiceInit();//lcm  整机音频测试
        }
        else if (val == TST_NORMAL)
        {
            g_pTestPhone->TestInitial();//lcm调用到的是基类的函数
            TestMode_EnterScreenCheck();

        }
        else if (TST_PREBURNING == val)
        {
            g_pTestPhone->TestPreBurningInit();
        }
        else if (val == TST_SFP)
        {
            g_pTestPhone->TestSFPInit();//lcm调用到的是基类的函数
        }
        else
        {
            return ;
        }
        UIManager_UpdateWnd(pDlg);//更新窗口
        TESTMODE_INFO("testmode_Enter : After TestInit CurTestmode(%d)", CurTestmode);
        // 进入测试模式 取消外部对灯的控制
        ledlamp_SwitchControl(false);

        // 进入测试模式禁止进入节电
        PowerSaving_SetAvoidPowerSaving(true);
    }
}

void testmode_Exit()
{
    TESTMODE_INFO("testmode_Exit : CurTestmode(%d)", CurTestmode);
    if (TST_NONE != CurTestmode)
    {
        CDlgTestMode * pTestDlg = qobject_cast<CDlgTestMode *>(UIManager_GetPrivateDlg(DLG_CDlgTestMode));
        if (NULL != pTestDlg)
        {
            UIManager_PutPrivateDlg(pTestDlg);
        }

        // http://10.2.1.199/Bug.php?BugID=34706
        // 在测试模式开启时，才启动macd，退出测试模式如果macd是以000000结尾，则不退出macd

        if (!commonUnit_IsMatchTestMode())
        {
            system("killall macd");
        }

        CTestPhoneT4X::GetInstance()->AfterEchoState();

        // 退出测试模式 恢复外部对灯的控制
        ledlamp_SwitchControl(true);

        // 退出测试模式恢复允许进节电
        PowerSaving_SetAvoidPowerSaving(false);

        CurTestmode = TST_NONE;
        exp_SetExpTestMode(false);
    }
}

bool testmode_IsTestMode()
{
    if (TST_NONE == CurTestmode)
    {
        return  false;
    }
    else
    {
        return true;
    }
}

