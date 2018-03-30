#include "commonunits_reboot.h"
#include "devicelib/phonedevice.h"
#include "lamp/ledlamp/src/factory.h"
#include <ETLLib.hpp>
#include "dsklog/dsklog.h"
#include "modcommonunits.h"
#include "service_system.h"
#include "lamp/powerled/include/modpowerledlogic.h"
#include "lamp/powersaving/include/modpowersaving.h"
#include "lamp/powersaving/include/powersavingmsgdefine.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "adapterbox/include/modadapterbox.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "configserver/configserverinc.h"
#include "voip/modvoip.h"

// 通话中重启的每次的检测时间
#define  REBOOT_IN_TALKING_CHECK_TIME  3000
#define  CALL_METHOD_MIN_TIME 1000

static UINT uRebootInTalkTimerID = 0;


static LRESULT ProcessMkitReboot(msgObject & msg)
{
    if (msg.message != MKIT_MSG_REBOOT)
    {
        return -1;
    }

    COMMONUNITS_INFO("recieve mkit_msg_reboot, reply now");

    msg.ReplyMessage(1);

    return 1;
}

static LRESULT OnRequesResetFactory(msgObject & msg)
{
    COMMONUNITS_INFO("LRESULT OnRequesResetFactory");

    int iResetType = RESET_OPTION_TYPE_NONE;

    // autop发起清除user data
    if (ATP_MSG_RESET_USER_CFG2_USER_DATA == msg.message)
    {
        iResetType = RESET_OPTION_USERDATA_ONLY;
    }
    // 网页请求reset类型
    else if (DSK_MSG_REQUES_RESET_FACTORY == msg.message)
    {
        iResetType = msg.wParam;
    }

    if (iResetType > RESET_OPTION_TYPE_NONE)
    {
        // 复用已有的处理流程
        AdapterBox_ResetFactory(iResetType);
    }

    return TRUE;
}

static LRESULT OnResetFactoryMsg(msgObject & msg)
{
    COMMONUNITS_INFO("SYSTEM_MSG_RESET_FACTORY");

    AdapterBox_ResetFactory(msg.lParam);

    return true;
}

static LRESULT OnRebootMsg(msgObject & msg)
{
    COMMONUNITS_INFO("SYS_MSG_DEVICE_REBOOT");

    if (talklogic_SessionExist())
    {
        SETTING_WARN("Warning : Talking now, end talk will reboot!");

#if IF_BUG_20000
        if (configParser_GetConfigInt(kszRebootInTalking) != 0)
        {
            talklogic_ExitAllSession();
            timerSetThreadTimer((UINT &)uRebootInTalkTimerID, 1);
        }
        else
#endif // IF_BUG_20000
        {
            timerSetThreadTimer((UINT &)uRebootInTalkTimerID, REBOOT_IN_TALKING_CHECK_TIME);
        }
        return false;
    }

    AdapterBox_Reboot(0);

    return true;
}

// 通话中重启定时器响应函数
static void OnRebootTimer()
{
    if (talklogic_SessionExist())
    {
        timerSetThreadTimer((UINT &)uRebootInTalkTimerID, REBOOT_IN_TALKING_CHECK_TIME);

        return;
    }

    COMMONUNITS_INFO("On Reboot time out");

    timerKillThreadTimer((UINT &)uRebootInTalkTimerID);

    msgPostMsgToThread(msgGetThreadByName(VP_PHONE_THREAD_NAME), DSK_MSG_LIGHT_STATUS_CHANGE, 1, 1);

    AdapterBox_Reboot(0);
}

static LRESULT OnUpdateFirware(msgObject & msg)
{
    COMMONUNITS_INFO("SYS_MSG_UPDATE_FIRWARE");

    // 关闭看门狗
    devicelib_CloseWatchDog();

    AdapterBox_UpdateFirware();
    // 先回复
    msg.ReplyMessage(1);

    return true;
}

static LRESULT OnUploadConfig(msgObject & msg)
{
    if (talklogic_SessionExist())
    {
        return 0;
    }
    // 关闭看门狗
    devicelib_CloseWatchDog();

    //停止SIP 服务
    modVoip_StopService();

    // 提示正在升级
    AdapterBox_UploadConfig();

    // 先回复
    msg.ReplyMessage(1);

    return 1;
}

static void DoResetShell(int eOption)
{
    switch (eOption)
    {
    case RESET_OPTION_CONFIG_ONLY:

        // 只恢复用户配置
        // 执行恢复出厂设置的脚本
        commonUnits_System(PHONE_SCRIPTS_PATH "factory.sh -resetconfig &");

        break;
    case RESET_OPTION_USERDATA_ONLY:

        // 只恢复用户数据
        // 执行恢复出厂设置的脚本
        commonUnits_System(PHONE_SCRIPTS_PATH "factory.sh -resetuserdata &");

        break;

    case RESET_OPTION_USERDATA_USERCFG:

        // 恢复用户数据和用户配置

        if (CFG_SERVER_PROCESS == GET_CONFIG_PROCESS_TYPE())
        {
            // 此处使用超时、autop处理完成后，会直接退出
            msgSendMsgToThreadTimeoutEx(msgGetThreadByName(VP_AP_THREAD_NAME), ATP_MSG_REQ_CLEAR_CFG, 0, 1, 0, NULL, 5 * 1000);
        }
        else
        {
            msgSendMsgToThread(msgGetThreadByName(CONFIG_SERVER_THREAD_NAME), CSM_CLEAR_USERDATA, 0, 0);
        }

        break;

    default:

        // 完全恢复
        // 执行恢复出厂设置的脚本
        commonUnits_System(PHONE_SCRIPTS_PATH "factory.sh -all &");

        break;
    }
}

static LRESULT OnTimerProcess(msgObject & msg)
{
    if (msg.wParam == (UINT &)uRebootInTalkTimerID)
    {
        OnRebootTimer();
    }
    return TRUE;
}

void commonUnitsReboot_Init()
{
    SingleMsgReg(DSK_MSG_REQUES_RESET_FACTORY, OnRequesResetFactory);
    SingleMsgReg(ATP_MSG_RESET_USER_CFG2_USER_DATA, OnRequesResetFactory);
    SingleMsgReg(MKIT_MSG_REBOOT, ProcessMkitReboot);
    SingleMsgReg(TM_TIMER, OnTimerProcess);
    SingleMsgReg(SYS_MSG_RESET_FACTORY, OnResetFactoryMsg);
    SingleMsgReg(SYS_MSG_DEVICE_REBOOT, OnRebootMsg);
    SingleMsgReg(SYS_MSG_UPDATE_FIRWARE, OnUpdateFirware);
    SingleMsgReg(DSK_MSG_UPLOAD_CONFIG, OnUploadConfig);
}

void commonUnits_ResetToFatory(int eOption/* = RESET_OPTION_ALL */)
{
    //测试模式会去设置状态
    //只有测试模式OK了才能进行恢复出厂
    if (PT_T49 == devicelib_GetPhoneType()
            && commUnits_GetResetFactoryState() != FACSTATE_ENABLE
       )
    {
        COMMONUNITS_WARN("test result not ok, can not reset to factory");
        return;
    }

    COMMONUNITS_INFO("Reset Factory [%d]", eOption);

    powerled_FlushPowerLed();

    // 开启背光灯
    msgPostMsgToThread(msgGetThreadByName(VP_PHONE_THREAD_NAME), DSK_MSG_LIGHT_STATUS_CHANGE, 1, 1);

    // 关闭看门狗
    devicelib_CloseWatchDog();

    etlSleep(1000);

    DoResetShell(eOption);

    if (eOption != RESET_OPTION_USERDATA_USERCFG)
    {
        etlSleep(500 * 1000);
    }
    else
    {
        devicelib_startWatchDog();
    }

}

// 重启
void commonUnits_Reboot(int iDelayTime/* = 4*/)
{
    COMMONUNITS_INFO("Ready to reboot");

#if LINUX_SYSTEM
    // 看门狗重启时,需先将所有的灯灭掉,否则如果有闪灯操作,ioctl相当于喂狗,造成重启不成功
    _AllLightOff();
#endif // LINUX_SYSTEM

    devicelib_RebootByWatchDog();

    commonUnits_SetSystemMaintenance(true);
}


