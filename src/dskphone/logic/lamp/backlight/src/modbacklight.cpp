#include "modbacklight.h"
#include <ETLLib.hpp>
#include "backlight.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "account/include/modaccount.h"
#include "exp/include/modexp.h"
#include <interfacedefine.h>
#include "powersaving/include/powersavingmsgdefine.h"
#include "powersaving/include/modpowersaving.h"
#include "hotplug/commondefine.h"
#include "powerled/include/modpowerledlogic.h"
#include "blf/include/blflistmsgdefine.h"

LRESULT OnBacklightMessage(msgObject & refMessage);

/************************************************************************/
/* 接口  ： BackLight_Init()                                         */
/* 功能  ： 初始化背光灯类                                              */
/* 参数说明 ：无                                                        */
/* 返回值 ： 无                                                         */
/************************************************************************/
void backlight_Init()
{
    backlight_ResetContrast();
    BackLightManager::GetInstance();
    SingleMsgReg(CONFIG_MSG_BCAST_CHANGED, OnBacklightMessage);
    SingleMsgReg(ACCOUNT_STATUS_CHANGED, OnBacklightMessage);
    SingleMsgReg(DSK_MSG_CONTRAST_CHANGE, OnBacklightMessage);
    SingleMsgReg(DSK_MSG_EXT_CHANGE, OnBacklightMessage);
    SingleMsgReg(HOTPLUG_USB_DEVICER_STATE_CHANGE, OnBacklightMessage);
    SingleMsgReg(POWERSAVINGMEG_SCREEN_CLOSE, OnBacklightMessage);
    SingleMsgReg(POWERSAVINGMEG_SCREEN_AWAKE, OnBacklightMessage);
    SingleMsgReg(BLF_MESSAGE_FLUSH, OnBacklightMessage);
    SingleMsgReg(SYS_MSG_NOTIFY_PHY_STATUS, OnBacklightMessage);
    PowerSaving_Init();

}

/************************************************************************/
/* 接口  ： BackLight_UnInit()                                           */
/* 功能  ： 析构 背光灯类                                              */
/* 参数说明 ：无                                                        */
/* 返回值 ： 无                                                         */
/************************************************************************/
void backlight_UnInit()
{
    BackLightManager::ReleaseInstance();
    etl_UnregisterMsgHandle(ACCOUNT_STATUS_CHANGED, ACCOUNT_STATUS_CHANGED, OnBacklightMessage);
    etl_UnregisterMsgHandle(CONFIG_MSG_BCAST_CHANGED, CONFIG_MSG_BCAST_CHANGED, OnBacklightMessage);
    etl_UnregisterMsgHandle(DSK_MSG_CONTRAST_CHANGE, DSK_MSG_CONTRAST_CHANGE, OnBacklightMessage);
    etl_UnregisterMsgHandle(DSK_MSG_EXT_CHANGE, DSK_MSG_EXT_CHANGE, OnBacklightMessage);
    etl_UnregisterMsgHandle(HOTPLUG_USB_DEVICER_STATE_CHANGE, HOTPLUG_USB_DEVICER_STATE_CHANGE,
                            OnBacklightMessage);
    etl_UnregisterMsgHandle(POWERSAVINGMEG_SCREEN_CLOSE, POWERSAVINGMEG_SCREEN_CLOSE,
                            OnBacklightMessage);
    etl_UnregisterMsgHandle(POWERSAVINGMEG_SCREEN_AWAKE, POWERSAVINGMEG_SCREEN_AWAKE,
                            OnBacklightMessage);
    etl_UnregisterMsgHandle(BLF_MESSAGE_FLUSH, BLF_MESSAGE_FLUSH, OnBacklightMessage);
    etl_UnregisterMsgHandle(SYS_MSG_NOTIFY_PHY_STATUS, SYS_MSG_NOTIFY_PHY_STATUS, OnBacklightMessage);
}

/************************************************************************/
/* 接口  ： BackLight_RegisterEvent()                                    */
/* 功能  ： 注册、删除背光灯类型                                         */
/* 参数说明 ：iType 背光灯类型                                          */
/* 返回值 ：  bRegister  注册或者删除消息                               */
/* 返回值 ： 无                                                         */
/************************************************************************/
void backlight_RegisterEvent(BACK_LIGHT_TYPE eType, bool bRegister)
{
    //BACKLIGHT_INFO("backlight_RegisterEvent(%d, %d)", eType, bRegister);
    if (bRegister)
    {
        g_pBackLightManager->AddEvent(eType);
    }
    else
    {
        g_pBackLightManager->EarseEvent(eType);
    }
}

/************************************************************************/
/* 接口  ： BackLight_Reset()                                             */
/* 功能  ： 即点亮屏幕操作                                              */
/* 参数说明 ：无                                                        */
/* 返回值 ： 无                                                         */
/************************************************************************/
void backlight_Reset(bool bIsUserOperate)
{
    etl_NotifyApp(false, POWERSAVIGNMEG_NOTIFY_OPERATE, bIsUserOperate ? 1 : 0, 0);

    g_pBackLightManager->Reset();
}

/************************************************************************/
/* 接口  ： backlight_RefreshEvent()                                 */
/* 功能  ： 即点亮屏幕操作                                              */
/* 参数说明 ：无                                                        */
/* 返回值 ： 无                                                         */
/************************************************************************/
void backlight_RefreshEvent()
{
    etl_NotifyApp(false, POWERSAVIGNMEG_NOTIFY_OPERATE, 1, 0); // 主动操作

    g_pBackLightManager->RefreshEvent();
}

/************************************************************************/
/* 接口  ： backlight_setLight()                                     */
/* 功能  ： 设置屏幕亮度操作,只有在关闭背光灯事件的情况下设置才生效     */
/* 参数说明 ：无                                                        */
/* 返回值 ： 无                                                         */
/************************************************************************/
void backlight_SetBacklightLevel(int iLightLevel)
{
    if (!g_pBackLightManager->IsOpenBackLight())// 判断背光灯亮度是否关闭
    {
        g_pBackLightManager->SetBacklightLevel(iLightLevel);
        g_pBackLightManager->SetEXPBacklightLevel(iLightLevel);
    }
}

/************************************************************************/
/* 接口  ： backlight_OpenBacklight()                                    */
/* 功能  ： 开启关闭背光灯                                              */
/* 参数说明 ：bOpen 是否开启                                             */
/* 返回值 ： 无                                                         */
/************************************************************************/
void backlight_OpenBacklightEvent(bool bOpen)
{
    g_pBackLightManager->OpenBackLightEvent(bOpen);
}

/************************************************************************/
/* 接口  ： backlight_ShowBacklightUI                                    */
/* 功能  ： 是否在背光灯设置界面                                        */
/* 参数说明 ：bShowBKUI 是否背光灯设置界面                              */
/* 返回值 ： 无                                                         */
/************************************************************************/
void backlight_ShowBacklightUI(bool bShowBKUI)
{
    g_pBackLightManager->ShowBacklightUI(bShowBKUI);
}

LRESULT OnBacklightMessage(msgObject & refMessage)
{
    switch (refMessage.message)
    {
    case  CONFIG_MSG_BCAST_CHANGED:
    {
        if (refMessage.wParam == ST_BACK_LIGHT)
        {
            // 刷新事件
            g_pBackLightManager->RefreshEvent();
        }
    }
    break;
    //http://10.2.1.199/Bug.php?BugID=90555
    //case ACCOUNT_STATUS_CHANGED:
    //    if (acc_IsAccountIDValid(acc_GetUsableAccount()))
    //    {
    //        //TO DO:所有账号其中有注册上的情况处理!!!
    //        //Account Well，注销屏保的Account事件
    //        //ledlamp_PostEvent(LAMP_EVENTTYPE_FAILACCOUNT, false);
    //        backlight_RegisterEvent(BACK_LIGHT_ACCOUNT, false);
    //    }
    //    else
    //    {
    //        //TO DO:所有账号均未注册上的情况处理!!!
    //        //Account Fail，注册屏保的Account事件
    //        //ledlamp_PostEvent(LAMP_EVENTTYPE_FAILACCOUNT, true);
    //        backlight_RegisterEvent(BACK_LIGHT_ACCOUNT, true);
    //    }
    //    break;
    case DSK_MSG_CONTRAST_CHANGE:
    {
        backlight_ResetContrast(refMessage.wParam);
    }
    break;
    case DSK_MSG_EXT_CHANGE:
    {
        // 刷新扩展台背光灯
        g_pBackLightManager->UpdateExpLight();
    }
    break;
    case HOTPLUG_USB_DEVICER_STATE_CHANGE:
    {
        backlight_Reset(true);
    }
    break;
    case POWERSAVINGMEG_SCREEN_AWAKE:
    case POWERSAVINGMEG_SCREEN_CLOSE:
    {
        if (POWERSAVINGMEG_SCREEN_AWAKE == refMessage.message)
        {
            g_pBackLightManager->Reset();
        }
        else if (POWERSAVINGMEG_SCREEN_CLOSE == refMessage.message)
        {
            if (g_pBackLightManager->IsOpenBackLight())
            {
                g_pBackLightManager->SetBacklightLevel(refMessage.lParam);
                g_pBackLightManager->SetEXPBacklightLevel(refMessage.lParam);
            }
        }
            // 先设置状态再刷新power 灯  http://10.2.1.199/Bug.php?BugID=108211
            if (DSK_PHONE_TYPE_COLOR == commonUnits_GetDskPhoneType())
            {

                powerled_FlushPowerLed();

            }

        }
    break;
    case BLF_MESSAGE_FLUSH:
    {
        if (configParser_GetConfigInt(kszBLFActiveBackLight) == 1)
        {
            backlight_Reset(false);
        }
    }
    break;
    case SYS_MSG_NOTIFY_PHY_STATUS:
    {
        backlight_Reset(true);
    }
    break;
    default:
        break;
    }

    return 0;
}

void backlight_ResetContrast(int iLevel)
{
    if (iLevel <= 0)
    {
        iLevel = configParser_GetConfigInt(kszContrastLevel);
    }
    //液晶对比度
    devicelib_SetLcdContrast(iLevel);

    //EXP对比度
    for (int i = 0; i < exp_GetEXPNum(); ++i)
    {
		exp_SetExpContrast(i, iLevel * 10);
    }
}

void backlight_SetPreviewLevel(bool bPreview)
{

    g_pBackLightManager->SetPreviewMode(bPreview);

}

int backlight_GetLastLightLevel()
{

    return g_pBackLightManager->GetLastLightLevel();

}

int backlight_GetCurrenLightLevel()
{

    return g_pBackLightManager->GetLightLevel();

}

int backlight_UpdateExpLight()
{
    g_pBackLightManager->UpdateExpLight();
    return 0;
}

void backlight_ResetExpLightShake()
{
    g_pBackLightManager->ResetExpLightShake();
}
