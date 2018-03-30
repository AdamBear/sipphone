#include "commonunits_inc.h"
#include "backtrace.h"
#include <commonapi/stringhelperapi.h>
#include <commonapi/systemhelperapi.h>
#include "resource/modresource.h"
#include "dsklog/log.h"
#include "dsklog/dsklog.h"
#include "lamp/powerled/include/modpowerledlogic.h"
#include "devicelib/phonedevice.h"
//#include "setting/include/modsetting.h"
#include "exp/include/exp_struct.h"
#include "lamp/ledlamp/src/factory.h"
#include "adapterbox/include/modadapterbox.h"
#include "deletemanager.h"
#include <ETLLib.hpp>
#include "etlmsghandler/modetlmsghandler.h"
#include "talk/fwd_dnd/include/modfwddnd.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "extended/actionuri_url/include/modactionuri_url.h"
#include "account/include/modaccount.h"

#include "voice/include/modvoice.h"
#include "regexp/modregexp.h"
#include "mysystem.h"
#include <xmlparser/xmlparser.hpp>
#include "lamp/powersaving/include/modpowersaving.h"
#include "lamp/powersaving/include/powersavingmsgdefine.h"
#include "commonunits/t2xpath.h"
#include "talk/talklogic/include/commonfunctions.h"
#include "autopcontrol/include/modautopcontrol.h"
#include "wireless/bluetoothNew/include/modbluetooth.h"

#ifdef IF_SUPPORT_WIFI
#include "wireless/wifi/include/modwifi.h"
#endif
#include "configserver/configserverinc.h"
#include "ETLLib/base64.h"
#include "ETLLib/aestable.h"

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "commonunits_reboot.h"
#ifdef _DECT
#include "hsupdate/include/modhsupdate.h"
#include "cmdprocessor/include/modcmdprocessor.h"
#endif //_DECT

//#include "json/cjson.h"
#include "contacts/directory/include/moddirinterface.h"
#include "ylstl/ylalgorithm.h"
#include "service_h323_interface.h"
#include <winimp/winimp.h>
#include "msgkit/mkhelper.h"
#include "taskaction/modtaskaction.h"
#include "service_config.h"

#include "exp/include/modexp.h"
#include "record/include/modrecord.h"

#define INIT_INFO_PATH TEMP_PATH "phoneboot"

// 各种函数声明
// 处理更新配置消息
LRESULT ProcessUpdateCfgMsg(msgObject & msg);

// 获取话机信息,包括固件版本号,FWD等信息
LRESULT OnGetPhoneInfo(msgObject & msg);

// 处理更改话机配置消息
LRESULT OnSetPhoneCfgMsg(msgObject & msg);

// 延迟恢复出厂定时器响应函数
static void OnResetTimer();

// 获取话机信息,包括固件版本号,FWD等信息
static LRESULT OnDumpPhoneInfo(msgObject & msg);

static void UpdateConfigTimer();

//void SetSystemMaintenanceStatus(bool bOn);

struct CommonFeaturesValue
{
    CommonFeaturesValue()
    {
        bIsUpdateConfig = false;
        wUpdateLparam = 0;
        bSystemMaintenance = false;
    }

    bool    bIsUpdateConfig;            // 正在更新配置标志
    bool    bSystemMaintenance;
    WPARAM  wUpdateLparam;              // 保存sip发过来的lparam参数,转发给autop
};

static CommonFeaturesValue g_tGlobalValue;
static YLList<FuncSetSysLog> g_listCallBackFunc;

// 处理配置更改消息
static LRESULT OnConfigChangeMSG(msgObject & msg)
{
    switch (msg.wParam)
    {
    case ST_NETWORK:
        {
            AdapterBox_Reboot(0);
        }
        break;
    case ST_SYSLOG:
        {
            // 从配置中读取log等级
            // run_cmd_t  run_cmd;
            // memset(&run_cmd, 0, sizeof(run_cmd_t));

            int iLevel = commonUnits_GetLogLevel();
            dsklog_setlog_level(iLevel);
            // char strLevel[32] = {0};
            // snprintf(strLevel, sizeof(strLevel) - 1, "ANY =%d", iLevel);
            // run_cmd.loglevel_str = strLevel;
            // run_cmd_log_apply(&run_cmd);


            //循环设置各模块的日志
            for (YLList<FuncSetSysLog>::iterator itBeg = g_listCallBackFunc.begin();
                    itBeg != g_listCallBackFunc.end();
                    ++itBeg)
            {
                FuncSetSysLog pFunc = *itBeg;
                if (NULL == pFunc)
                {
                    continue;
                }
                printf("set log level [%p]\n", pFunc);
                pFunc(-1, iLevel);
            }
        }
        break;
    case ST_IME:
        {
            // autop更新输入法
            COMMONUNITS_INFO("Reload IME");
            AdapterBox_ReloadIME();
        }
        break;
#ifdef IF_SUPPORT_VIDEO
    case ST_CAMERA:
        {
            int iValue = modVoip_IsSupportVideo() ? 1 : 0;
            modVoip_SetOption(SIP_SERVICE_OPT_VIDEO_CAP, (void *)&iValue, sizeof(int), true);
#ifdef IF_FEATURE_H323
            modVoip_SetOption(H323_SERVICE_OPT_VIDEO_CAP, (void *)&iValue, sizeof(int), false);
#endif //IF_FEATURE_H323

            // todo
            //给sip发消息
            msgBroadpostThreadMsg(0x10000, 2, 0);
        }
        break;
#endif //IF_SUPPORT_VIDEO
    case ST_DEBUG_TIMER:
        {
            mkDumpTimerInfo();
            etl_DumpMsgMap(msg.lParam);
            yl::string strInfo;
            dskthread_DumpThreadInfo(strInfo);
        }
        break;
    case ST_DEBUG_TASKACTION:
        {
            TaskAction_DumpInfo(msg.lParam);
        }
        break;
    default:
        break;
    }

    return 0;
}

// 处理定时器消息
static LRESULT ProcessTimerMsg(msgObject & msg)
{
    if (msg.wParam == (UINT)UpdateConfigTimer)
    {
        UpdateConfigTimer();
        return 1;
    }
    else if (msg.wParam == (UINT)OnResetTimer)
    {
        OnResetTimer();
        return 1;
    }

    return 0;
}

// 话机状态查询
static LRESULT OnPhoneStatusMsg(msgObject & msg)
{
    bool bHandle = true;
    if (msg.message == DSK_MSG_GET_DONGLE_STATUS)
    {
        int iRet = 0;

#ifdef IF_SUPPORT_BLUETOOTH
        if (2 == msg.wParam)
        {
            iRet = Bluetooth_GetDeviceState() != DS_NO_DONGLE;
        }
#endif

#ifdef IF_SUPPORT_WIFI
        if (1 == msg.wParam)
        {
            iRet = WIFI_IsDongleVaild();
        }
#endif

#if ((defined IF_SUPPORT_BLUETOOTH) && (defined IF_BT_SUPPORT_PHONE))
        if (3 == msg.wParam)
        {
            iRet = Bluetooth_IsMobileConnected();
        }
        else if (4 == msg.wParam)
        {
            iRet = Dir_IsDirConfiged(BLUETOOTH_DIR);
        }
#endif

        msg.ReplyMessage(iRet);
        return iRet;
    }
    else if (msg.message == DSK_MSG_GET_PHONE_STATUS)
    {
        int iRet = 0;

#ifdef _DECT
        switch (PHONE_STATE(msg.wParam))
        {
        case PS_REGISTER_MODE:
            iRet = CMD_IsRegisterModeOn() ? 1 : 0;
            break;
#if IF_FEATURE_PAGING
        case PS_PAGING:
            iRet = CMD_IsPagingOn() ? 1 : 0;
            break;
#endif
        case PS_HS_UPDATE:
            iRet = HSUpdate_IsUpdating();
            break;
        case PS_DSKPHONE_RUNUP:
            iRet = 1;
            break;
        }
#endif
        msg.ReplyMessage(iRet);
        return iRet;
    }
    else
    {
        bHandle = false;
    }


    return bHandle;
}

static LRESULT OnUpdatePhoneStatusMsg(msgObject & msg)
{
    BOOL bHandle = TRUE;
    switch (msg.message)
    {
    case MKIT_MSG_SYSLOG:
        {
            int log_type = HIBYTE(HIWORD(msg.wParam));
            int module_id = LOBYTE(HIWORD(msg.wParam));
            int log_level = HIBYTE(LOWORD(msg.wParam));

            COMMONUNITS_INFO("get MKIT_MSG_SYSLOG, type %d, module id %d, log_level %d",
                             log_type, module_id, log_level);

            if (log_type != -1
                    && NULL != msg.GetExtraData())
            {
                LOG_OPEN(log_type, (const char *) msg.GetExtraData());
            }

            if (log_level != DBG_LOG_INV)
            {
                LOG_SET(module_id, log_level);
            }

            //循环设置各模块的日志
            for (YLList<FuncSetSysLog>::iterator itBeg = g_listCallBackFunc.begin();
                    itBeg != g_listCallBackFunc.end();
                    ++itBeg)
            {
                FuncSetSysLog pFunc = *itBeg;
                if (NULL == pFunc)
                {
                    continue;
                }
                pFunc(module_id, log_level);
            }
        }
        break;
    default:
        bHandle = FALSE;
        break;
    }

    return bHandle;
}

// 延迟恢复出厂定时器响应函数
void OnResetTimer()
{
    if (talklogic_SessionExist())
    {
        timerSetThreadTimer(UINT(OnResetTimer), 3000);
        return;
    }
    SETTING_INFO("On Reset time out");
    timerKillThreadTimer(UINT(OnResetTimer));

    etl_NotifyApp(true, SYS_MSG_RESET_FACTORY, 0, RESET_OPTION_ALL);
}

// 处理sip notify的恢复出厂操作
LRESULT OnResetFactoryAfterCallEnd(msgObject & msg)
{
    // sip 发起恢复出厂
    if (SIP_CMD_RESET_PHONE != msg.message)
    {
        return 0;
    }

    if (talklogic_SessionExist())
    {
        SETTING_WARN("Warning : Talking now, end talk will reset factory!");

        timerSetThreadTimer(UINT(OnResetTimer), 3000);
        return 0;
    }

    COMMONUNITS_INFO("LRESULT OnResetFactoryAfterCallEnd");

    // 复用已有的处理流程
    return etl_NotifyApp(true, SYS_MSG_RESET_FACTORY, 0, RESET_OPTION_ALL);
}

// 初始化各公用单元
VOID commonUnits_Init()
{
    //启用异常Dump
#if ENABLE_BACKTRACE
    commonUnits_EnableMiniDump();
#endif

    if (PT_T49 != devicelib_GetPhoneType())
    {
        //上电时将时间设置方法强制设为网络取时
        //etlSetProfileInt(szInitFile, "PhoneSetting", "Manual_Time", 0);
        configParser_SetConfigInt(kszNetManualTime, 0, CONFIG_LEVEL_PHONE);

    }

    // 缓存网络信息,防止网页改变网络信息但未生效造成的问题
    // http://192.168.1.99/Bug.php?BugID=14011
    // CacheNetWorkInfo();

    modVoip_init();

    SingleMsgReg(CONFIG_MSG_BCAST_CHANGED, OnConfigChangeMSG);
    SingleMsgReg(TM_TIMER, ProcessTimerMsg);
    // 从miscFeatures移动到commonUnits，需要全机型支持
    SingleMsgReg(SYS_MSG_UPDATE_CONFIG, ProcessUpdateCfgMsg);
    SingleMsgReg(DSK_MSG_GETPHONE_INFO, OnGetPhoneInfo);
    SingleMsgReg(DSK_MSG_SETPHONE_CFG, OnSetPhoneCfgMsg);
    SingleMsgReg(DSK_MSG_GET_DONGLE_STATUS, OnPhoneStatusMsg);
    SingleMsgReg(DSK_MSG_GET_PHONE_STATUS, OnPhoneStatusMsg);
    SingleMsgReg(SIP_CMD_RESET_PHONE, OnResetFactoryAfterCallEnd);
    SingleMsgReg(DSK_MSG_QUERY_INFO_DIAG, OnDumpPhoneInfo);
    SingleMsgReg(MKIT_MSG_SYSLOG, OnUpdatePhoneStatusMsg);

    commonUnitsReboot_Init();

    //初始化时预读几个配置文件
    configParser_GetConfigString(kszEnableSilenceMode);
    configParser_GetConfigString(kszPhoneOEM);
    configParser_GetConfigString(kszFeaturesNoAnswerForwardTarget);
    configParser_GetConfigString(kszNetworkWanStaticMask);
}

yl::string commonUnit_SearchRingPath(const yl::string & strRingFileName,
                                     const yl::string & strDefaultFileName /*= Ring1.wav*/)
{
    // 1.全路径情况直接返回
    if (strRingFileName.find_first_of("/") != yl::string::npos
            && pathFileExist(strRingFileName.c_str()))
    {
        return strRingFileName;
    }

    // 2.隐藏路径,转换为全路径
    if (strRingFileName.find_first_of(":") != yl::string::npos
            || commonAPI_StringConvert(strRingFileName, STRING_CONVERT_TO_LOWER) == "default"
            || commonAPI_StringConvert(strRingFileName, STRING_CONVERT_TO_LOWER) == "common")
    {
        // 保存铃声文件为Resource:Ring1.wav格式
        yl::string strTmp = strRingFileName;

        commonUnits_Hide2Real(strTmp, PHFILE_TYPE_RING_FILE);
        if (!strTmp.empty() && pathFileExist(strTmp.c_str()))
        {
            return strTmp;
        }
    }

    //3.先在自定义铃声目录寻找，不存在则到固定铃声目录去寻找
    //4.如果仍然不存在，则使用默认铃声路径，指定的文件名
    LPCSTR lpszFileName = pathGetFileNamePos(strRingFileName.c_str());
    chPathNameA strRingFile(USER_RING_FILE_PATH, lpszFileName);

    if (!pathFileExist(strRingFile))
    {
        // 先查紧急资源目录
        chPathNameA strEmergencyFile(szResEmergencyPath, lpszFileName);
        if (pathFileExist((LPCSTR) strEmergencyFile))
        {
            return (LPCTSTR) strEmergencyFile;
        }

        // 查默认铃声资源目录
        chPathNameA strFixRingFile(res_getPathByName(RESOURCE_PATH_RING).c_str(), lpszFileName);

        //如果仍然不存在，则使用默认铃声资源目录下指定的文件
        if (!pathFileExist((LPCSTR) strFixRingFile))
        {
            strFixRingFile.ReplaceFileName(strDefaultFileName.c_str());
        }

        strRingFile = strFixRingFile;
    }
    return strRingFile;
}

// 获取默认铃声路径
VOID commonUnits_GetDefaultRing(yl::string & strRingPath)
{
    // 存放当前设置的铃声类型
    yl::string strRing = configParser_GetConfigString(kszRingtype);

    if (strRing.empty())
    {
        return ;
    }

    strRingPath = commonUnit_SearchRingPath(strRing);
}

// 是否*、#呼出键
BOOL commonUnits_IsSendDigitKey(int iKey)
{
    // 0--Disable，1--#，2--*
    int iSendKey = configParser_GetConfigInt(kszKeyAsSend);
    if (iSendKey == 1 && iKey == PHONE_KEY_POUND
            || iSendKey == 2 && iKey == PHONE_KEY_STAR)
    {
        return TRUE;
    }

    return FALSE;
}

// 发送模拟按键
int commonUnits_SendSimulateKey(UINT uMessage, int iKey, bool bExp /*= false*/)
{
    WPARAM wParam = bExp ? 1 : 0;

#ifndef _DECT
    if (uMessage == DEV_MSG_FUNKEY_EVENT
            && voice_ProcessChannelKeyAccess(iKey))
    {
        return 1;
    }
#endif

    DSK_TARGET_T hThread = 0;
    if (uMessage == DEV_MSG_FUNKEY_EVENT || uMessage == DEV_MSG_DIGITKEY_EVENT)
    {
        // exp走黑白屏的流程
        if (!bExp)
        {
            AdapterBox_SendKey(iKey, true);
            AdapterBox_SendKey(iKey, false);
            return 1;
        }

        // 获取活动进程
        hThread = msgGetThreadByName(VP_PHONE_THREAD_NAME);
    }

    // 发送消息
    if (hThread != 0)
    {
        // 打开背光灯
        msgPostMsgToThread(msgGetThreadByName(VP_PHONE_THREAD_NAME), DSK_MSG_LIGHT_STATUS_CHANGE, 0, 0);

        msgPostMsgToThread(msgGetThreadByName(VP_PHONE_THREAD_NAME), POWERSAVIGNMEG_NOTIFY_OPERATE, 1, 0);

        msgPostMsgToThread(hThread, uMessage, wParam, iKey);

        if (uMessage == DEV_MSG_FUNKEY_EVENT || uMessage == DEV_MSG_DIGITKEY_EVENT)
        {
            // 发送功能键弹起的消息
            msgPostMsgToThread(hThread, DEV_MSG_KEY_RELEASED, 0, 0);
        }
    }

    return 1;
}

yl::string commonUnits_Hide2Real(yl::string & strPath, PHFileType eType)
{
    if (eType > PHFILE_TYPE_NONE && eType < PHFILE_TYPE_MAX)
    {
        res_toRealPath(strPath, eType);
    }
    return strPath;
}

yl::string commonUnits_Real2Hide(yl::string & strPath, PHFileType eType)
{
    if (eType > PHFILE_TYPE_NONE && eType < PHFILE_TYPE_MAX)
    {
        res_toHidePath(strPath, eType);
    }
    return strPath;
}

// 进入老化测试模式化的特征码
#define ENTER_PREBURNING_CODE "#*7"
// 进入半成品测试模式的特征码
#define ENTER_SFPTEST_CODE      "#*159"
//CP920进入手动按键测试的特征码
#define ENTER_TEST_CODE_MANAUAL_KEY_CP920 "#*7"
// 进入普通测试模式的特征码
#define ENTER_TEST_CODE      "#*9"
// 进入查看话机状态的特征码
#define ENTER_TEST_CODE_VIEWSTATUS     "#*03"
// 进入整机免提音质测试模式的特征码
#define ENTER_TEST_CODE_FULLVOICE      "#*02"
//查看机器测试状态模式的特征码
#define ENTER_TEST_CODE_CHECKFLAG      "#*08"
// CP进入老化测试模式化的特征码
#define ENTER_PREBURNING_CODE_CP "#*01"
// 进入待机误触测试模式
#define ENTER_ERROR_TRIGGER_CODE "#*8"
// 进入Wifi测试模式的特征码
#define ENTER_TEST_CODE_WIFI            "#*07"

#define ENTER_TEST_CODE_AUDIODEVICE    "#*01"

// 判断是否是测试模式前缀
bool commonUnits_IsTestModePrefix(yl::string strNumber)
{
    //禁止通过预拨号界面进入测试模式
    if (configParser_GetConfigInt(kszForbiddenTestMode) != 0)
    {
        return false;
    }

    bool bRet = false;
    int iLength = strNumber.size();
    if (iLength > strlen(ENTER_TEST_CODE_FULLVOICE)
            || iLength == 0)
    {
        return bRet;
    }

    bRet = (strncmp(strNumber.c_str(), ENTER_TEST_CODE, iLength) == 0);

    if (!bRet)
    {
        bRet = (strncmp(strNumber.c_str(), ENTER_TEST_CODE_FULLVOICE, iLength) == 0);
    }

    if (!bRet && (DEVICELIB_IS_T48(devicelib_GetPhoneType())))
    {
        bRet = (strncmp(strNumber.c_str(), ENTER_PREBURNING_CODE, iLength) == 0);
    }

    if (!bRet && (DEVICELIB_IS_CP(devicelib_GetPhoneType())))
    {
        bRet = (strncmp(strNumber.c_str(), ENTER_PREBURNING_CODE_CP, iLength) == 0);
    }

    if (!bRet && (devicelib_GetPhoneType() == PT_CP920))
    {
        bRet = (strncmp(strNumber.c_str(), ENTER_TEST_CODE_MANAUAL_KEY_CP920, iLength) == 0);
    }

    if (!bRet)
    {
        bRet = (strncmp(strNumber.c_str(), ENTER_TEST_CODE_AUDIODEVICE, iLength) == 0);
    }

    if (!bRet && (devicelib_GetPhoneType() == PT_CP920))
    {
        bRet = (strncmp(strNumber.c_str(), ENTER_ERROR_TRIGGER_CODE, iLength) == 0);
    }

    if (!bRet && !DEVICELIB_IS_CP(devicelib_GetPhoneType()))
    {
        bRet = (strncmp(strNumber.c_str(), ENTER_SFPTEST_CODE, iLength) == 0);
    }

    return bRet;
}

// 获取测试模式类型
int commonUnits_GetTestModeType(yl::string strNumber)
{
    //禁止通过预拨号界面进入测试模式
    if (configParser_GetConfigInt(kszForbiddenTestMode) != 0)
    {
        return -1;
    }
    int iRet = -1;
    int iLength = strNumber.size();
    if (iLength == 0)
    {
        return iRet;
    }

    if (ENTER_TEST_CODE == strNumber)
    {
        // 普通测试模式
        iRet = TST_NORMAL;
    }
    else if (ENTER_TEST_CODE_FULLVOICE == strNumber)
    {
        // 整机免提音质测试模式
        iRet = TST_FULLVOICE;
    }
    else if (ENTER_TEST_CODE_VIEWSTATUS == strNumber)
    {
        // 查看测试模式状态
        iRet =  TST_VIEWSTATUS;
    }
    else if (ENTER_PREBURNING_CODE == strNumber
             && (DEVICELIB_IS_T48(devicelib_GetPhoneType())))
    {
        // 老化模式测试模式
        iRet = TST_PREBURNING;
    }
    else if (ENTER_TEST_CODE_CHECKFLAG == strNumber
             && (PT_T49 == devicelib_GetPhoneType() || PT_CP920 == devicelib_GetPhoneType()))
    {
        //查看机器测试状态模式的特征码
        iRet = TST_CHECKFLAG;
    }
    else if (ENTER_PREBURNING_CODE_CP == strNumber
             && (DEVICELIB_IS_CP(devicelib_GetPhoneType())))
    {
        // 老化模式测试模式
        iRet = TST_PREBURNING;
    }
    else if (ENTER_TEST_CODE_AUDIODEVICE == strNumber
             && PT_T49 == devicelib_GetPhoneType())
    {
        iRet = TST_AUDIODEVICE;
    }
    else if (ENTER_SFPTEST_CODE == strNumber
             && !DEVICELIB_IS_CP(devicelib_GetPhoneType()))
    {
        // 半成品测试模式
        iRet = TST_SFP;
    }
    else if (ENTER_ERROR_TRIGGER_CODE == strNumber
             && PT_CP920 == devicelib_GetPhoneType())
    {
        iRet = TST_ERROR_TRIGGER;
    }
    else if (ENTER_TEST_CODE_MANAUAL_KEY_CP920 == strNumber
             && PT_CP920 == devicelib_GetPhoneType())
    {
        iRet = TST_MANAUAL_KEY;
    }
    else if (ENTER_TEST_CODE_WIFI == strNumber
             && PT_CP920 == devicelib_GetPhoneType())
    {
        iRet = TST_WIFI;
    }

    return iRet;
}

///////////////接收Update消息////////////////
LRESULT ProcessUpdateCfgMsg(msgObject & msg)
{
    if (SYS_MSG_UPDATE_CONFIG != msg.message)
    {
        return 0;
    }

    APPLICATION_INFO("OnUpdateMsg wparam[%d] lparam[%d]", msg.wParam, msg.lParam);
    //更新消息转给XmlBrowser线程
    //  msgPostMsgToThread(msgGetThreadByName(VP_BROWSER_THREAD_NAME), SYS_MSG_UPDATE_CONFIG, 0, 0);

    // 清空当前所有通话
    if (talklogic_SessionExist())
    {
        // 配置项控制在通话中可以重启
        if (configParser_GetConfigInt(kszRebootInTalking) != 0)
        {
            talklogic_ExitAllSession();
        }

        g_tGlobalValue.bIsUpdateConfig = true;
        timerSetThreadTimer((UINT)UpdateConfigTimer, 3000);

        //Elkotek SOL T28 话机接收Broadsoft重启消息的机制改变和默认Feature Key Synchronisation为enabled
        //http://10.2.1.199/Bug.php?BugID=12702
        g_tGlobalValue.wUpdateLparam = msg.lParam;
        return 0;
    }
    else
    {
        //if(configParser_GetConfigInt(kszAutoprovisionPromptEnable) != 0)
        {
            /*
            0 - 触发AutoP更新
            1 - 触发TR069回连
            2 - 触发AutoP更新后再触发TR069回连
            3 - 触发TR069回连后再触发XML Action URL
            */
            int iMode = configParser_GetConfigInt(kszCheckSyncMode);
            if (iMode == 0
                    || iMode == 2)
            {
                etl_NotifyApp(false, COMMONUNITS_MESSAGE_CHECKSYNC_UPDATECONFIG, 0, 0);
            }
        }

        //Elkotek SOL T28 话机接收Broadsoft重启消息的机制改变和默认Feature Key Synchronisation为enabled
        //http://10.2.1.199/Bug.php?BugID=12702
        // 通知AutoProvision进程开始配置更新
#if IF_BUG_30385
        // 标示GUI发送check-sync给autop
        Autop_NotifyAutoServer(false, ATP_MSG_REQ_START_BY_DEFAULT, 1,
                               msg.lParam);
#else
        Autop_NotifyAutoServer(false, ATP_MSG_REQ_START_BY_DEFAULT, 0,
                               msg.lParam);
#endif
        return 1;
    }

    return 1;
}

void UpdateConfigTimer()
{
    if (!g_tGlobalValue.bIsUpdateConfig)
    {
        // 被外部停止更新配置
        timerKillThreadTimer((UINT)UpdateConfigTimer);
        return;
    }

    if (talklogic_SessionExist())
    {
        // 不杀定时器,等下次再检查
        return;
    }

    timerKillThreadTimer((UINT)UpdateConfigTimer);

    msgObject msg;
    msg.message = SYS_MSG_UPDATE_CONFIG;
    //Elkotek SOL T28 话机接收Broadsoft重启消息的机制改变和默认Feature Key Synchronisation为enabled
    //http://10.2.1.199/Bug.php?BugID=12702
    msg.lParam = g_tGlobalValue.wUpdateLparam;
    msg.wParam = 0;
    ProcessUpdateCfgMsg(msg);
}

///////////////获取话机信息////////////////
int CreatePhoneInfo(LPCSTR lpParam)
{
    if (NULL == lpParam || strlen(lpParam) == 0)
    {
        return 0;
    }

    // accounts=1&dnd=1&fw=1
    int nNeedAccount = 0;
    int nNeedDND = 0;
    int nNeedFw = 0;
    LPCSTR lpCfg = NULL;

    // 获取需要返回的状态配置
    lpCfg = strstr(lpParam, "accounts=");
    if (lpCfg != NULL)
    {
        sscanf(lpCfg, "accounts=%d", &nNeedAccount);
    }

    lpCfg = strstr(lpParam, "dnd=");
    if (lpCfg != NULL)
    {
        sscanf(lpCfg, "dnd=%d", &nNeedDND);
    }

    lpCfg = strstr(lpParam, "fw=");
    if (lpCfg != NULL)
    {
        sscanf(lpCfg, "fw=%d", &nNeedFw);
    }

    // 把需要的状态写到文件中
    yl::string strPhonecfg = "[Status]\nFirmwareVersion=";
    char szFirmware[32] = {0};
    yl::string strFirmware = "";

    if (0 == sys_get_sw_version(szFirmware, 32))
    {
        strFirmware = szFirmware;
    }

    strPhonecfg = strPhonecfg + strFirmware.c_str() + "\n";

    if (nNeedAccount != 0)
    {
        acc_GetAccountInfo(strPhonecfg);
    }

    if (nNeedDND != 0)
    {
        strPhonecfg += yl::string("[DND]\n") +
                       "State=" + (dnd_IsEnable(acc_GetDefaultAccount()) ? "On" : "Off") + "\n";
    }

    if (nNeedFw != 0)
    {
        yl::string strTarget;
        bool bEnable = false;
        //  [Forward_always]
        //  State=On
        //  Target=phonenumber
        bEnable = fwd_GetInfo(acc_GetDefaultAccount(), FWD_TYPE_ALWAYS, &strTarget);
        strPhonecfg += yl::string("[Forward_always]\n") +
                       "State=" + (bEnable ? "On" : "Off") + "\n";
        strPhonecfg += "Target=" + strTarget + "\n";
        //  [Forward_busy]
        //  State=Off
        //  Target=phonenumber
        bEnable = fwd_GetInfo(acc_GetDefaultAccount(), FWD_TYPE_BUSY, &strTarget);
        strPhonecfg += yl::string("[Forward_busy]\n") +
                       "State=" + (bEnable ? "On" : "Off") + "\n";
        strPhonecfg += "Target=" + strTarget + "\n";
        //  [Forward_noanswer]
        //  Ringtimes=2
        //  State=Off
        //  Target=phonenumber
        int iRingTimes = 0;
        bEnable = fwd_GetInfo(acc_GetDefaultAccount(), FWD_TYPE_ALWAYS, &strTarget, &iRingTimes);
        char lpRingtimes[64] = "";
        int iRingDuration = configParser_GetConfigInt(kszNoAnswerForwardRingDuration);
        if (iRingDuration <= 0)
        {
            iRingDuration = 6;
        }
        sprintf(lpRingtimes, "Ringtimes=%d\n", iRingTimes * iRingDuration);
        strPhonecfg += yl::string("[Forward_noanswer]\n") + lpRingtimes +
                       "State=" + (bEnable ? "On" : "Off") + "\n";
        strPhonecfg += "Target=" + strTarget + "\n";
    }

    if (!strPhonecfg.empty())
    {
        etlWriteBinFile(TEMP_PATH "phonecfg.ini", strPhonecfg.c_str(), strPhonecfg.size());
        return 1;
    }

    return 0;
}

// 获取话机信息
LRESULT OnGetPhoneInfo(msgObject & msg)
{
    LPCSTR lpCmdMsg = (LPCSTR)msg.GetExtraData();
    int iRet = CreatePhoneInfo(lpCmdMsg);
    msg.ReplyMessage(iRet);
    return iRet;
}

///////////////更改话机配置////////////////
LRESULT OnSetPhoneCfgMsg(msgObject & msg)
{
    // 权限控制
    if (!accessCtr_Process(msg))
    {
        return 1;
    }
    // 获取配置字符串
    LPCSTR lpCfg = (LPCSTR)msg.GetExtraData();
    if (NULL == lpCfg || strlen(lpCfg) == 0)
    {
        return 0;
    }
    return commonUnits_SetForwardCfg(lpCfg);
}

// 设置FWD配置信息
int commonUnits_SetForwardCfg(const char * cszConfig)
{
    COMMONUNITS_INFO("DSK_MSG_SETPHONE_CFG [%s]", cszConfig);
    // 获取账号
    int iAccount = FWDDND_ACCOUNT_GLOBAL;
    commonUnits_GetIntValue(cszConfig, "account", iAccount);
    // 判断当前是否账号模式(Custom模式)
    if (fwd_GetMode() == FWDDND_MODE_ACCOUNT && !acc_IsAccountIDValid(iAccount))
    {
        COMMONUNITS_WARN("SetPhoneCfg Account info error!");
        return 0;
    }
    // 设置转移配置信息
    int iEnable = 0;
    char szTarget[128] = { 0 };
    yl::string strTarget;
    // 设置无条件转移
    bool bGetEnableVaule = commonUnits_GetIntValue(cszConfig, "Forward_always_enable", iEnable);
    bool bGetTargetVaule = commonUnits_GetStringValue(cszConfig, "Forward_always_target", szTarget);
    if (bGetEnableVaule || bGetTargetVaule)
    {
        strTarget = szTarget;
        fwd_SetInfo(iAccount, FWD_TYPE_ALWAYS,
                    bGetEnableVaule ? iEnable : fwd_GetInfo(iAccount, FWD_TYPE_ALWAYS),
                    bGetTargetVaule ? &strTarget : NULL);
    }
    // 设置遇忙转移
    bGetEnableVaule = commonUnits_GetIntValue(cszConfig, "Forward_busy_enable", iEnable);
    bGetTargetVaule = commonUnits_GetStringValue(cszConfig, "Forward_busy_target", szTarget);
    if (bGetEnableVaule || bGetTargetVaule)
    {
        strTarget = szTarget;
        fwd_SetInfo(iAccount, FWD_TYPE_BUSY,
                    bGetEnableVaule ? iEnable : fwd_GetInfo(iAccount, FWD_TYPE_BUSY),
                    bGetTargetVaule ? &strTarget : NULL);
    }
    // 设置无应答转移
    bGetEnableVaule = commonUnits_GetIntValue(cszConfig, "Forward_noanswer_enable", iEnable);
    bGetTargetVaule = commonUnits_GetStringValue(cszConfig, "Forward_noanswer_target", szTarget);
    int iRingTimes = 0;
    bool bGetRingTimes = commonUnits_GetIntValue(cszConfig, "Forward_noanswer_ringtimes", iRingTimes);
    int RingDuration = configParser_GetConfigInt(kszNoAnswerForwardRingDuration);
    if (RingDuration <= 0)
    {
        RingDuration = 6;
    }
    if (bGetRingTimes
            && iRingTimes > 0
            && iRingTimes <= TOTAL_RING_INTERVAL * RingDuration)
    {
        iRingTimes = (iRingTimes + RingDuration - 1) / RingDuration;
        COMMONUNITS_INFO("iRingTimes: %d", iRingTimes);
    }
    if (bGetEnableVaule || bGetTargetVaule || bGetRingTimes)
    {
        strTarget = szTarget;
        fwd_SetInfo(iAccount, FWD_TYPE_NO_ANSWER,
                    bGetEnableVaule ? iEnable : fwd_GetInfo(iAccount, FWD_TYPE_NO_ANSWER),
                    bGetTargetVaule ? &strTarget : NULL,
                    bGetRingTimes ? &iRingTimes : NULL);
    }
    return 1;
}


void commonUnits_EnableMiniDump()
{
    catch_sig();
}

Factory_State commUnits_GetResetFactoryState()
{
    yl::string strFlag = "testmode";
    yl::string strValue = commUnits_GetUserEnvString(strFlag);

    if (strValue.size() <= 0)
    {
        return FACSTATE_NONE;
    }

    return ((strValue[strValue.size() - 1] - '0') == 1) ? FACSTATE_ENABLE : FACSTATE_DISABLE;
}

bool commUnits_SetResetFactoryEnable(bool bEnable)
{
    yl::string strCmd = commonAPI_FormatString(BOOT_PATH "bin/userenv -s testmode -v %d",
                        bEnable ? 1 : 0);

    commonUnits_System(strCmd.c_str());

    return (bEnable ? (commUnits_GetResetFactoryState() == FACSTATE_ENABLE) :
            (commUnits_GetResetFactoryState() == FACSTATE_DISABLE));
}

yl::string commUnits_GetUserEnvString(const yl::string & strFlag)
{
    yl::string strPath = commonAPI_FormatString("%s %s", BOOT_PATH "bin/userenv -g", strFlag.c_str());
    FILE * pFile = popen(strPath.c_str(), "r");

    yl::string strValue = "";
    if (NULL == pFile)
    {
        return strValue;
    }

    //读取命令输出
    char szResult[64] = {0};
    if (NULL == fgets(szResult, sizeof(szResult), pFile)
            || strcmp("", szResult) == 0)
    {
        pclose(pFile);
        return strValue;
    }

    pclose(pFile);

    strValue = szResult;

    //去掉换行符
    strValue.trim_both("\n");

    COMMONUNITS_INFO("%s = %s", strPath.c_str(), strValue.c_str());

    return strValue;
}

yl::string char2hex(char dec)
{
    chASSERT(dec != '\0');
    char dig1 = (dec & 0xF0) >> 4;
    char dig2 = (dec & 0x0F);
    if (dig1 <= 9)
    {
        dig1 += 48;    // 0,48 in ascii
    }
    if (10 <= dig1 && dig1 <= 15)
    {
        dig1 += 65 - 10;    // A,65 in ascii
    }
    if (dig2 <= 9)
    {
        dig2 += 48;
    }
    if (10 <= dig2 && dig2 <= 15)
    {
        dig2 += 65 - 10;
    }

    yl::string r = "";
    r += dig1;
    r += dig2;
    return r;
}

void commonUnits_EncodeURI(yl::string & strURL)
{
    yl::string szEscaped = "";
    int iMax = strURL.size();
    chASSERT(iMax > 0);
    for (int i = 0; i < iMax; i++)
    {
        // 不用转换的特殊字符：~!@#$&*()=:/,;?'_-.
        if (('0' <= strURL[i] && strURL[i] <= '9') ||  //0-9
                ('a' <= strURL[i] && strURL[i] <= 'z') ||  //abc...xyz
                ('A' <= strURL[i] && strURL[i] <= 'Z') || //ABC...XYZ
                (strURL[i] == '~' || strURL[i] == '!' || strURL[i] == '*' || strURL[i] == '('
                 || strURL[i] == ')' || strURL[i] == '\'' || strURL[i] == '@' || strURL[i] == '#'
                 || strURL[i] == '$' || strURL[i] == '&' || strURL[i] == '=' || strURL[i] == ':'
                 || strURL[i] == '/' || strURL[i] == ',' || strURL[i] == ';' || strURL[i] == '?'
                 || strURL[i] == '-' || strURL[i] == '_' || strURL[i] == '.'
                 /*|| strURL[i] == '+'*/) // 客户要求对输入的"+"编码
           )
        {
            szEscaped += strURL[i];
        }
        else
        {
            szEscaped += "%";
            szEscaped += char2hex(strURL[i]);//converts char 255 to string "ff"
        }
    }
    strURL = szEscaped;
}

void commonUnits_EncodeAutopURI(yl::string & strURL)
{
    if (strURL.empty())
    {
        return;
    }
    yl::string szEscaped = "";
    int iMax = strURL.GetLength();
    if (iMax <= 0)
    {
        return;
    }
    for (int i = 0; i < iMax; i++)
    {
        // 不用转换的字符：0-9,abc...xyz,ABC...XYZ, 斜杠，冒号，小数点
        if (('0' <= strURL[i] && strURL[i] <= '9') ||  //0-9
                ('a' <= strURL[i] && strURL[i] <= 'z') ||  //abc...xyz
                ('A' <= strURL[i] && strURL[i] <= 'Z')  //ABC...XYZ
                || (strURL[i] == ':' || strURL[i] == '/' || strURL[i] == '.')
           )

        {
            szEscaped += strURL[i];
        }
        else
        {
            szEscaped += "%";
            szEscaped += char2hex(strURL[i]);//converts char 255 to string "ff"
        }
    }
    strURL = szEscaped;
}


bool commonUnits_GetIntValue(LPCSTR lpCfgContent, LPCSTR lpKey, int & iOutValue)
{
    if (NULL == lpCfgContent
            || NULL == lpKey)
    {
        return false;
    }

    int iLength = strlen(lpKey);
    if (iLength <= 0)
    {
        return false;
    }

    // "=%d"共3个字符,加上串尾符共需4个
    char * lpBuffer = new char[iLength + 4];
    sprintf(lpBuffer, "%s=", lpKey);
    LPSTR lpStartPos = (char *)strstr(lpCfgContent, lpBuffer);
    if (NULL == lpStartPos)
    {
        delete[] lpBuffer;
        return false;
    }

    sprintf(lpBuffer, "%s=%s", lpKey, "%d");
    int nRet = sscanf(lpStartPos, lpBuffer, &iOutValue);

    delete [] lpBuffer;
    return (1 == nRet);
}

bool commonUnits_GetStringValue(LPCSTR lpCfgContent, LPCSTR lpKey, LPSTR lpOut)
{
    if (NULL == lpCfgContent
            || NULL == lpKey
            || NULL == lpOut)
    {
        return false;
    }

    int iLength = strlen(lpKey);
    if (iLength <= 0)
    {
        return false;
    }


    LPSTR lpStartPos = (char *)strstr(lpCfgContent, lpKey);
    if (lpStartPos != NULL)
    {
        // "=%[^&]&"共7个字符,加上串尾符共需8个
        char * lpBuffer = new char[iLength + 8];
        // 生成格式化串,用于sscanf
        sprintf(lpBuffer, "%s=%s", lpKey, "%[^&]&");
        lpOut[0] = '\0';
        sscanf(lpStartPos, lpBuffer, lpOut);
        delete [] lpBuffer;
        return true;
    }

    return false;
}

void commonUnits_ParserDialURI(const yl::string & strURL, yl::string & strNumber,
                               yl::string & strServer)
{
    yl::string::size_type nStartIndex = 0;
    if (strURL.substr(0, 4) == "sip:")
    {
        nStartIndex = 4;
    }

    yl::string::size_type nNumberEnd = strURL.find_first_of("@", nStartIndex);
    if (nNumberEnd != yl::string::npos)
    {
        strNumber = strURL.substr(nStartIndex, nNumberEnd - nStartIndex);
        strServer = strURL.substr(nNumberEnd + 1);
    }
    else
    {
        strNumber = strURL;
    }
}

yl::string commonUnits_GetConfigFileByPhone()
{
    // M7要求从配置文件中读取
    yl::string strConfigName = configParser_GetConfigString(kszAutoProvisionCommonName);

    // 如果是空的,则从出厂中读取
    if (strConfigName.empty())
    {
        strConfigName = configParser_GetConfigString(kszAutopFacCommonFileName);
    }

    if (!strConfigName.empty())
    {
        return strConfigName;
    }

    devicelib_GetPhoneType();
    switch (g_iPhoneType)
    {
    case 22:
        strConfigName = "y000000000005.cfg";
        break;
    case 26:
#ifdef PHONE_T6X
        strConfigName = "y000000000010.cfg";
#else
        strConfigName = "y000000000004.cfg";
#endif
        break;
    case 28:
    default:
        strConfigName = "y000000000000.cfg";
        break;
    }
    return strConfigName;
}

#if IF_FEATURE_AUTOP_IME
#include "language/common/lang_common.h"
//根据配置项的值获取对应IME输入法
yl::string commonUnits_GetIMEByType(const yl::string strConfigIMEType)
{
    yl::string strIME = "";
    yl::string strIMEType = configParser_GetConfigString(strConfigIMEType.c_str());
    bool bIsPwd = false;
    // 密码界面没有Abc
    if (strConfigIMEType == kszHotelingPasswordIME
            || strConfigIMEType == kszZeroPasswordIME
            || strConfigIMEType == kszXsiPasswordIME
            || strConfigIMEType == kszAdvancedSettingPasswordIME
            || strConfigIMEType == kszUnlockPinIME
            || strConfigIMEType == kszHotDeskingPasswordIME
            || strConfigIMEType == kszLDAPPasswordIME
            || strConfigIMEType == kszAdvancedAccountPasswordIME
            || strConfigIMEType == kszWifiPasswordIME
       )
    {
        bIsPwd = true;
    }
    if (strIMEType == "1"
            || strIMEType == "123")
    {
        if (bIsPwd)
        {
            strIME = IME_123_Pwd;
        }
        else
        {
            strIME = IME_123;
        }
    }
    else if (strIMEType == "2"
             || strIMEType == "abc")
    {
        if (bIsPwd)
        {
            strIME = IME_abc_Pwd;
        }
        else
        {
            strIME = IME_abc;
        }
    }
    else if (strIMEType == "3"
             || strIMEType == "ABC")
    {
        if (bIsPwd)
        {
            strIME = IME_ABC_Pwd;
        }
        else
        {
            strIME = IME_ABC;
        }
    }
    else if (strIMEType == "4"
             || strIMEType == "Abc")
    {
        //非密码框输入法才有Abc
        if (!bIsPwd)
        {
            strIME = IME_Abc;
        }
    }
    else if (strIMEType == "5"
             || strIMEType == "2aB")
    {
        if (bIsPwd)
        {
            strIME = IME_2aB_Pwd;
        }
        else
        {
            strIME = IME_2aB;
        }
    }

    if (strIME.empty())
    {
        if (strConfigIMEType == kszHotelingUserIME)
        {
            strIME = IME_123;
        }
        else if (strConfigIMEType == kszHotelingPasswordIME)
        {
            strIME = IME_123_Pwd;
        }
        else if (strConfigIMEType == kszZeroUserIME)
        {
            strIME = IME_2aB;
        }
        else if (strConfigIMEType == kszZeroPasswordIME)
        {
            strIME = IME_abc_Pwd;
        }
        else if (strConfigIMEType == kszXsiPasswordIME)
        {
            strIME = IME_2aB_Pwd;
        }
        else if (strConfigIMEType == kszAdvancedSettingPasswordIME)
        {
            strIME = IME_abc_Pwd;
        }
        else if (strConfigIMEType == kszUnlockPinIME)
        {
            strIME = IME_123_Pwd;
        }
        else if (strConfigIMEType == kszHotDeskingPasswordIME)
        {
            strIME = IME_abc_Pwd;
        }
        else if (strConfigIMEType == kszHotDeskingUserNameIME)
        {
            strIME = IME_2aB;
        }
        else if (strConfigIMEType == kszAdvancedAccountPasswordIME)
        {
            strIME = IME_2aB;
        }
        else if (strConfigIMEType == kszAdvancedAccountNameIME)
        {
            strIME = IME_2aB;
        }
        else if (strConfigIMEType == kszWifiPasswordIME)
        {
            strIME = IME_abc_Pwd;
        }
    }

    return strIME;
}
#endif
bool commonUnits_SetIMEType(yl::string strIMEType, const yl::string strIME)
{
    if (strIMEType.empty()
            || strIME.empty())
    {
        return false;
    }
    configParser_SetConfigString(strIMEType.c_str(), strIME);
    return true;
}
#if IF_BUG_30625
yl::string filterAddr(const in_addr & addr)
{
    yl::string strResult;
    if (addr.s_addr != INADDR_NONE && addr.s_addr != INADDR_ANY)
    {
        strResult = netAddr2String(addr);
    }
    else
    {
        strResult = "";
    }
    return strResult;
}

bool commonUnits_GetWanThirdDNS(yl::string & strResult, IPAddrType addrType/* = IAT_IPv4 */)
{
    switch (addrType)
    {
    case IAT_IPv4:
        {
            sys_IP_t ip;
            char szBuff[SYS_IPv4_BUF_LEN];
            int iret = sys_get_DNS(SYS_PHY_TYPE_WAN, SYS_IP_PROTO_IPV4, SYS_DNS_INDEX_THI, &ip);
            inet_ntop(AF_INET, &ip.addr.a4.sin_addr, szBuff, SYS_IPv4_BUF_LEN);
            if (iret == 0)
            {
                strResult = filterAddr(ip.addr.a4.sin_addr);
                return TRUE;
            }
            else
            {
                COMMONUNITS_INFO("failed to get ipv4 second dns by sys_get_wan_dns fun");
            }
            break;
        }
    case IAT_IPv6:
        {
            sys_IP_t ip;
            char szBuff[SYS_IPv6_BUF_LEN] = {0};
            int iret = sys_get_DNS(SYS_PHY_TYPE_WAN, SYS_IP_PROTO_IPV6, SYS_DNS_INDEX_THI, &ip);
            inet_ntop(AF_INET6, &ip.addr.a6.sin6_addr, szBuff, SYS_IPv6_BUF_LEN);
            if (iret == 0)
            {
                strResult = szBuff;
                return TRUE;
            }
            else
            {
                COMMONUNITS_INFO("failed to get ipv6 second dns by sys_get_wan_dns fun. ret");
            }
            break;
        }
    default:
        {
            break;
        }
    }
    return FALSE;
}
#endif

bool commonUnits_System(LPCSTR lpCmdString)
{
#ifndef IF_COMMONAPI_SYSTEM
    FILE * pFile = vpopen(lpCmdString, "w");

    if (pFile != NULL)
    {
        vpclose(pFile);
    }
#else
    if (NULL == lpCmdString)
    {
        return false;
    }

//#warning V82
    COMMONUNITS_INFO("system cmd[%s]", lpCmdString);
    return commonAPI_System(lpCmdString);
#endif // IF_COMMONAPI_SYSTEM
}

#if IF_BUG_25367
void commonUnits_FormatNumber(yl::string & strText)
{
    char buff[1024] = {0};

    // 匹配10个纯数字的
    if (regExp_Replace("([0-9]{2})([0-9]{4})([0-9]{4})", strText.c_str(), "$1 $2 $3", buff, 1024) == 0)
    {
        strText = buff;
    }
    // 匹配xxxxxxxxxx(xxx)类型的
    else if (regExp_Replace("([0-9]{2})([0-9]{4})([0-9]{4})[ ]*[\\\\]?([ ]*\\(.*\\))", strText.c_str(),
                            "$1 $2 $3\\$4", buff, 1024) == 0)
    {
        strText = buff;
    }
    // 匹配斜杠前面是十个数字的
    else if (regExp_Replace("([0-9]{2})([0-9]{4})([0-9]{4})[ ]*[\\\\](.*)$", strText.c_str(),
                            "$1 $2 $3\\$4", buff, 1024) == 0)
    {
        strText = buff;
    }
}
#endif

bool commonUnits_GetBackupSoftkey(yl::string & strFileName, yl::string & strStatus)
{
    yl::string strSoftkeyTrans = CONFIG_PATH "softkey/Softkey.xml";
    if (pathFileExist(strSoftkeyTrans.c_str()))
    {
        xml_document xFile;
        xml_node nodeXmlNode;
        if (xFile.load_file(strSoftkeyTrans.c_str()))
        {
            nodeXmlNode = xFile.child(strFileName.c_str());
            if (nodeXmlNode.empty())
            {
                return false;
            }

            nodeXmlNode = nodeXmlNode.child(strStatus.c_str());
            if (nodeXmlNode.empty())
            {
                return false;
            }

            nodeXmlNode = nodeXmlNode.child("item");
            if (!nodeXmlNode.empty())
            {
                if (nodeXmlNode.attribute("FileName")
                        && nodeXmlNode.attribute("Status"))
                {
                    strFileName = nodeXmlNode.attribute("FileName").value();
                    strStatus = nodeXmlNode.attribute("Status").value();
                    COMMONUNITS_INFO("commonUnits_GetBackupSoftkey newFileName[%s] newStatus[%s]", strFileName.c_str(),
                                     strStatus.c_str());
                    return true;
                }
            }
        }
    }

    return false;
}

bool commonUnits_IsSoftkeyEnable(yl::string & strFileName, yl::string & strStatus)
{
    chASSERT(!textEMPTY(strFileName.c_str()));
    chPathNameA strKeyPath(szSoftKeyFile, strFileName.c_str());
    xml_document xFile;
    xml_node nodeXmlNode;
    if (!pathFileExist((LPCSTR)strKeyPath))
    {
        return false;
    }

    if (xFile.load_file((LPCSTR)strKeyPath))
    {
        nodeXmlNode = xFile.child(strStatus.c_str());
        if (nodeXmlNode.child("Enable").empty()
                && !commonUnits_GetBackupSoftkey(strFileName, strStatus))
        {
            return false;
        }

        yl::string strApplyStates = configParser_GetConfigString(kszSoftkeyApplyStates);
        strApplyStates = commonAPI_replaceSubStringByTag(strApplyStates, " ", "");
        if (strApplyStates.empty())
        {
            return true;
        }

        bool bAllow = true;
        if (strApplyStates.substr(0, 1) == "-")
        {
            bAllow = false;
            strApplyStates = strApplyStates.substr(1);
        }
        else if (strApplyStates.substr(0, 1) == "+")
        {
            strApplyStates = strApplyStates.substr(1);
        }

        yl::string strTempStatus = commonAPI_FormatString(",%s,", strStatus.c_str());
        strApplyStates = commonAPI_FormatString(",%s,", strApplyStates.c_str());
        bool bFind = commonAPI_StringMatch(strApplyStates, strTempStatus, false);
        return (bAllow == bFind);

    }
    return false;
}

bool commonUnits_PositiveInteger(yl::string & strInput)
{
    if (strInput.empty())
    {
        return false;
    }
    yl::string::iterator iter = strInput.begin();

    for (int i = 0; i < strInput.length(); i++)
    {
        if (!isdigit(strInput[i]))
        {
            return false;
        }
    }

    return true;
}

bool commonUnits_IsValidPagingChannel(yl::string & strChannel)
{
    if (!strChannel.empty()
            && strChannel.length() < 3
            && commonAPI_IsDigital(strChannel))
    {
        int iChannel = atoi(strChannel.c_str());

        if (iChannel >= 0
                && iChannel <= 30)
        {
            strChannel = commonAPI_FormatString("%d", iChannel);
            return true;
        }
    }

    return false;
}

void commonUnits_SetSystemMaintenance(bool bEnable)
{
    if (g_tGlobalValue.bSystemMaintenance != bEnable)
    {
        g_tGlobalValue.bSystemMaintenance = bEnable;
    }

    COMMONUNITS_INFO("set system maintenance status = [%d]", g_tGlobalValue.bSystemMaintenance);
}

yl::string commonUnits_PwAesDataEncrypt(yl::string & strSrcData, const char * lpKey)
{
    if (lpKey == NULL)
    {
        return "";
    }
    yl::string strPsd = strSrcData;

    unsigned char aes_in_data[DATA_LEN] = {0};
    unsigned char aes_out_data[DATA_LEN] = {0};
    unsigned char tmp[DATA_LEN] = {0};
    unsigned char in_16_byte[16] = {0};
    unsigned char out_16_byte[16] = {0};
    unsigned int length = 0, count = 0;

    length = strPsd.length();
    int nResultLen = length % 16 == 0 ? length + 16 : chCEIL(length, 16);
    memcpy(aes_in_data, strPsd.c_str(), length);
    memset(aes_in_data + length, nResultLen - length, nResultLen - length);
    /*AES 加密*/
    while (1)
    {
        memcpy(in_16_byte, aes_in_data + 16 * count, 16);
        pw_aes_encrypt((char *)lpKey, (char *)in_16_byte, (char *)out_16_byte);
        memcpy((aes_out_data + count * 16), out_16_byte, 16);
        if (nResultLen <= 16)
        {
            break;
        }
        else
        {
            count ++;
            nResultLen = nResultLen - 16;
        }
    }

    PW_Base64Enc(tmp, aes_out_data, 16 * (count + 1));

    strPsd = (char *)tmp;
    commonUnits_EncodeURI(strPsd);
    return strPsd;
}

yl::string commonUnits_PwAesDataDecrypt(yl::string & strSrcData, char * lpKey)
{
    unsigned char aes_in_data[DATA_LEN] = {0};
    unsigned char aes_out_data[DATA_LEN] = {0};
    char tmp[DATA_LEN] = {0};
    unsigned char in_16_byte[16] = {0};
    unsigned char out_16_byte[16] = {0};
    int length = 0, count = 0;

    length = strSrcData.length();
    unsigned char src[DATA_LEN] = {0};
    memcpy(src, strSrcData.c_str(), length);

    /*Base64算法要求 src的长度必须为4的倍数*/
    length = PW_Base64Dec(aes_in_data, src, length);/*length不一定是16的倍数*/
    if (length < 0)
    {
        return "";
    }

    length = 16 * (length / 16);
    int nLen = length;
    /*AES 解密*/
    while (1)
    {
        memcpy(in_16_byte, aes_in_data + 16 * count, 16);
        pw_aes_decrypt(lpKey, (char *)in_16_byte, (char *)out_16_byte);
        memcpy((aes_out_data + count * 16), out_16_byte, 16);
        if (length <= 16)
        {
            break;
        }
        else
        {
            count ++;
            length = length - 16;
        }
    }
    int nPaddingLen =  aes_out_data[nLen - 1];

    memcpy(tmp, aes_out_data, 16 * (count + 1));
    if (nPaddingLen >= 0 && nPaddingLen <= 16)
    {
        memset(tmp + nLen - nPaddingLen, '\0', 1);
    }

    yl::string strPwd = (char *)tmp;

    return strPwd;
}

bool commonUnits_IsSystemMaintenanceStatus()
{
    return g_tGlobalValue.bSystemMaintenance;
}

bool commonUint_IsDevSupport(LPCSTR lpDevName)
{
    /* 规则
    /* 1. 系列机型 只要包含基础机型名称即可。如 T42 就支持T42G、T43P、T43S
    /* 2. common 表示全机型支持
    /* 3. 单个机型支持需要单独包含，如T43S，就只支持T43S
    */
#define  PHONE_DEVICE_NAME_COMMON "common"

    LPCSTR lpPhoneName = devicelib_GetPhoneName();

    if (textEMPTY(lpDevName) || textEMPTY(lpPhoneName))
    {
        return false;
    }

    return strcmp(lpDevName, PHONE_DEVICE_NAME_COMMON) == 0
           || strcmp(lpPhoneName, PHONE_DEVICE_NAME_COMMON) == 0
           || strstr(lpDevName, lpPhoneName) != NULL
           || strstr(lpPhoneName, lpDevName) != NULL;
}

yl::string commUnit_ReplaceWildcard(yl::string & strIn)
{
    yl::string strOut = strIn;

    char szBuf[512] = {0};

    if (0 == sys_replace_wildcards((char *) strIn.c_str(), 0, szBuf, sizeof(szBuf) / sizeof(char)))
    {
        strOut = szBuf;
    }

    return strOut;
}

#ifdef IF_SUPPORT_VIDEO
bool commonUnits_EnableAVSwitch()
{
    return configParser_GetConfigInt(kszVideoTalkEnable) == TVO_OPTIONAL;
}
#endif //#ifdef IF_SUPPORT_VIDEO

bool commonUnit_IsMatchTestMode(const yl::string & strMac)
{
    yl::string strDectectMac = strMac.empty() ? netGetMACText('\0') : strMac;

    // Step 2. 后六位为 000000
    return strDectectMac.size() > 0
           && strDectectMac.substr(6) == "000000";

}

bool commonUnits_GetCustomLabel(yl::string & result, const yl::string & strLabelSet,
                                const yl::string & strDefLabel, int index)
{
    if (strLabelSet.empty() || index < 0)
    {
        return false;
    }

    YLVector<yl::string> veclist;
    if (ParseString(veclist, strLabelSet, "|"))
    {
        // 支持区间操作，但不支持嵌套
#if 0
        // 游标
        int iVernier = 0;
        // 暂存最后一个
        yl::string * pstrLast = NULL;

        YLVector<yl::string>::iterator it = veclist.begin();
        while (it != veclist.end())
        {
            yl::string & strLabel = *it;
            strLabel.replace("{L}", strDefLabel);

            yl::string::size_type start = strLabel.find('{', 0);
            if (start != yl::string::npos)
            {
                yl::string::size_type end = strLabel.find('}', start);
                if (end != yl::string::npos && end - start > 1)
                {
                    int iMin, iMax, iValue;
                    int iCount = sscanf(strLabel.c_str() + start + 1, "%d-%d", &iMin, &iMax);
                    int iLength = end - start + 1;
                    bool bRet = true;

                    // 单边界至无穷大
                    if (iCount == 1)
                    {
                        iValue = iMin + index - iVernier;
                    }
                    // 区间解析
                    else if (iCount > 1)
                    {
                        if (abs(iMax - iMin) >= index - iVernier)
                        {
                            // 递增
                            if (iMax >= iMin)
                            {
                                iValue = iMin + index - iVernier;
                            }
                            // 递减
                            else
                            {
                                iValue = iMin - (index - iVernier);
                            }
                        }
                        else
                        {
                            iValue = iMax;
                            bRet = false;
                        }
                    }

                    strLabel.replace(start, iLength, commonAPI_FormatString("%d", iValue));

                    if (bRet)
                    {
                        result = strLabel;
                        return true;
                    }
                    else
                    {
                        iVernier += abs(iMax - iMin) + 1;
                        pstrLast = &strLabel;
                        ++it;
                        continue;
                    }
                }
            }

            // 找到位置
            if (iVernier == index)
            {
                result = strLabel;
                return true;
            }
            else if (iVernier > index)
            {
                COMMONUNITS_ERR("Parse Error [%d][%d][%s][%s]", iVernier, index, strLabelSet.c_str(),
                                strLabel.c_str());
                return false;
            }

            pstrLast = &strLabel;
            ++iVernier;
            ++it;
        }

        // 下标超出区域则使用最后一个
        if (iVernier <= index && pstrLast != NULL)
        {
            result = *pstrLast;
            return true;
        }
#else
        // 支持嵌套，但不支持区间操作
        bool bExpanded = veclist.size() <= index + 1;

        if (bExpanded)
        {
            result = veclist[veclist.size() - 1];
        }
        else
        {
            result = veclist[index];
        }

        LPCSTR lpRexp = "{L}";
        while (result.find(lpRexp) != yl::string::npos)
        {
            result.replace(lpRexp, strDefLabel);
        }

        yl::string::size_type start = result.find('{', 0);
        while (start != yl::string::npos)
        {
            yl::string::size_type end = result.find('}', start + 1);
            if (end != yl::string::npos && end - start > 1)
            {
                int iRef = 0;
                if (sscanf(result.c_str() + start, "{%d}", &iRef) == 1)
                {
                    int iCount = bExpanded ? iRef + index + 1 - veclist.size() : iRef;
                    yl::string strCount = commonAPI_FormatString("%d", iCount);
                    result.replace(start, end - start + 1, strCount);
                }
            }

            start = result.find('{', start + 1);
        }

        return true;
#endif
    }

    return false;
}

void comminUnits_DeleteInitInfofile()
{
    remove(INIT_INFO_PATH);
}

void comminUnits_WriteInitInfo(const char * pszInitInfo)
{
    if (NULL == pszInitInfo)
    {
        return;
    }

    FILE * pKeyFile = fopen(INIT_INFO_PATH, "a+");

    if (NULL == pKeyFile)
    {
        return;
    }

    fprintf(pKeyFile, "%s\n", pszInitInfo);
    fclose(pKeyFile);
}

AMR_LICENSE_RESULT comminUnits_GetAmrLicenseResult()
{
    AMR_LICENSE_RESULT eResult = AMR_LICENSE_NOT_INSTALL;
    int nAmrResult;
    int nRet = sys_get_permission_info(mkGetHandle(),
                                       SYS_PERMISSION_AMR, SYS_PERMISSION_INFO_PERMIT, &nAmrResult);

    COMMONUNITS_INFO("comminUnits_GetAmrLicenseResult [%d][%d]", nRet, nAmrResult);

    if (nRet == -1)
    {
        eResult = AMR_LICENSE_NOT_INSTALL;
    }
    else
    {
        if (nAmrResult == 1)
        {
            eResult = AMR_LICENSE_INSTALL_SUCCESS;
        }
        else if (nAmrResult == 0)
        {
            eResult = AMR_LICENSE_INSTALL_FAIL;
        }
    }

    return eResult;
}


yl::string commonUnits_Time2String(time_t tTime)
{
    tm * tmTemp = localtime(&tTime);

    return commonAPI_FormatString("%02d/%02d %d:%02d:%02d", tmTemp->tm_mon, tmTemp->tm_mday,
                                  tmTemp->tm_hour, tmTemp->tm_min, tmTemp->tm_sec);
}

#ifdef IF_FEATURE_DIAGNOSE
yl::string commonUnits_GetUpTime()
{
    char szTime[64];
    debug_PrintUptime(szTime);
    return szTime;
}

bool commonUnits_GetMemInfo(int & nTotal, int & nFree, int & nBuff, int & nCached)
{
    return debug_GetTotalMemInfo(nTotal, nFree, nBuff, nCached);
}
#endif // IF_FEATURE_DIAGNOSE

bool commonUnits_BeMulticastIP(const yl::string & strIPPort)
{
    if (strIPPort.empty())
    {
        return false;
    }

    // 判断端口是否合格
    const char * pStrPort = strstr(strIPPort.c_str(), ":");
    if (NULL == pStrPort || !Comm_IsAllDigit(pStrPort + 1)
            || '\0' == *(pStrPort + 1) || atoi(pStrPort + 1) > 65535)
    {
        return false;
    }

    // 判断IP是否是组播IP
    int iTemp = strIPPort.size() - strlen(pStrPort);
    yl::string strIP = strIPPort.substr(0, iTemp);
    if (strIP.empty() || !Comm_IsValidIP(strIP.c_str()))
    {
        return false;
    }
    sscanf(strIP.c_str(), "%d.", &iTemp);

    return (iTemp >= 224 && iTemp <= 239);
}

bool commonUnits_AddSysLogFunc(FuncSetSysLog pFunc)
{
    if (NULL == pFunc)
    {
        return false;
    }

    printf("add log set callback[%p]\n", pFunc);
    g_listCallBackFunc.push_back(pFunc);

    return true;
}

bool commonUnits_RemoveSysLogFunc(FuncSetSysLog pFunc)
{
    bool bRemoved = false;
    for (YLList<FuncSetSysLog>::iterator itBeg = g_listCallBackFunc.begin();
            itBeg != g_listCallBackFunc.end();)
    {
        if (pFunc == *itBeg)
        {
            itBeg = g_listCallBackFunc.erase(itBeg);
            bRemoved = true;
        }
        else
        {
            ++itBeg;
        }
    }

    return bRemoved;
}

void commonUnits_InitLog(int argc, char * argv[])
{
    // run_cmd_t  run_cmd;
    // memset(&run_cmd, 0, sizeof(run_cmd_t));
    // run_cmd_parse(&run_cmd, argc, argv);
    // run_cmd_log_apply(&run_cmd);
    // if (!(run_cmd.is_manual_loglevel))
    // {
    //     // 从配置中读取log等级
    //     int iLevel = commonUnits_GetLogLevel();
    //     char strLevel[32] = {0};
    //     sprintf(strLevel, "ANY =%d", iLevel);
    //     run_cmd.loglevel_str = strLevel;//strLevel.c_str();
    //     run_cmd_log_apply(&run_cmd);
    // }

    dsklog_init(argc, argv, NULL);
    if (!dsklog_is_manual_init(argc, argv))
    {
        dsklog_setlog_level(commonUnits_GetLogLevel());
    }
}

int commonUnits_GetLogLevel(SYSTEM_LOG_TYPE eType)
{
    int iLocalLogLevel = configParser_GetConfigInt(kszLocalLogLevel);
    int iSyslogLevel = configParser_GetConfigInt(kszSysLogLevel);

    if (LOG_LOCAL_MODE == eType)
    {
        return iLocalLogLevel;
    }
    else if (LOG_SERVER_MODE)
    {
        return iSyslogLevel;
    }


    return iLocalLogLevel > iSyslogLevel ? iLocalLogLevel : iSyslogLevel;
}

DSK_PHONE_TYPE commonUnits_GetDskPhoneType()
{
    static DSK_PHONE_TYPE eType = DSK_PHONE_TYPE_NONE;

    if (DSK_PHONE_TYPE_NONE != eType)
    {
        return eType;
    }

    PHONE_TYPE_DEF ePhoneType = devicelib_GetPhoneType();

    if (DEVICELIB_IS_T23(ePhoneType)
            || DEVICELIB_IS_T27(ePhoneType)
            || DEVICELIB_IS_T40(ePhoneType)
            || DEVICELIB_IS_CP(ePhoneType)
            || DEVICELIB_IS_T42(ePhoneType)
            || ePhoneType == PT_T19
       )
    {
        eType = DSK_PHONE_TYPE_BLACK;
    }
    else if (DEVICELIB_IS_T46(ePhoneType)
             || DEVICELIB_IS_T48(ePhoneType)
             || DEVICELIB_IS_T69(ePhoneType)
             || ePhoneType == PT_T54S
             || ePhoneType == PT_T52
             || ePhoneType == PT_T49
            )
    {
        eType = DSK_PHONE_TYPE_COLOR;
    }
    else if (ePhoneType == PT_DECT
             || ePhoneType == PT_W600)
    {
        eType = DSK_PHONE_TYPE_NOSCREEN;
    }
    else
    {
        eType = DSK_PHONE_TYPE_UNKNOW;
    }

    return eType;
}

bool commonUnits_SetUserPSWD(const char* szUser, const char* szPwd)
{
    bool bRet = commonAPI_SavePassword(szUser, szPwd);
    etl_NotifyApp(FALSE, CONFIG_MSG_BCAST_CHANGED, ST_USER_INFO_CHANGED, 0);
    return bRet;
}

// 收集业务逻辑接口
LRESULT OnDumpPhoneInfo(msgObject & msg)
{
//#define DUMPINFO_LOG	
#define DUMPINFO_LOG	COMMONUNITS_INFO
	if (msg.message != DSK_MSG_QUERY_INFO_DIAG)
	{
		msg.ReplyMessage(-1);
		return 0;
	}

	COMMONUNITS_INFO("query info msg[%x] w[%d] l[%d]", msg.message, msg.wParam, msg.lParam);

	yl::string strResult;
	int nRet = 0;

	switch (msg.wParam)
	{
	case DSK_DIAG_INFO_TYPE_APP:
	{
		// 1-999
		// todo 计划通过动态库或者插件的形式实现，运行中查看内容
		etl_NotifyApp(false, MKIT_MSG_STATS, 0, 0);
	}
	break;
	case DSK_DIAG_INFO_TYPE_CONFIG_DSK:
	{
		// 101-199 配置资源相关信息
		// 话机配置路径
		Dsk_ConfigInfo info;
		memset(&info, 0, sizeof(Dsk_ConfigInfo));
		strcpy(info.szConfigData, CONFIG_DATA_PATH);
		strcpy(info.szConfigUser, USER_DATA_PATH);
		msg.ReplyMessageEx(1, sizeof(Dsk_ConfigInfo), (void *)(&info));
		return 0;
	}
	break;
	case DSK_DIAG_INFO_TYPE_CONFIG_ACCOUNT_COUNT:
	{
		nRet = acc_AccountNum(msg.lParam == DSK_ACCOUNT_PROTOCOL_H323 ? ACCOUNT_TYPE_H323 : ACCOUNT_TYPE_SIP);
	}
	break;
	case DSK_DIAG_INFO_TYPE_TALK:
	{
		// 500 - 999
		strResult.append("{");

		strResult = strResult + commonAPI_FormatString("\"sessionnum\":%d, \"focus\":%d, \"index\":%d", talklogic_GetSessionNum(),
			talklogic_GetFocusedSessionID(), talklogic_GetFocusedSessionIndex());
		strResult.append("}");

		YLVector<int> vecSessionId;
		talklogic_GetAllSessionID(vecSessionId, false);
		for (int i = 0; i < vecSessionId.size(); i++)
		{
			const CCallInfo * pCallInfo = (CCallInfo *)talklogic_GetCallInfoBySessionID(vecSessionId[i]);
			if (NULL == pCallInfo)
			{
				continue;
			}

			COMMONUNITS_INFO("index[%d] call[%d] dis[%s] num[%s] ser[%s]", i,
				pCallInfo->GetCallID(), pCallInfo->m_tRemoteInfo.sDisplayName.c_str(),
				pCallInfo->m_tRemoteInfo.sSIPName.c_str(), pCallInfo->m_tRemoteInfo.sServerName.c_str());
		}
	}
	break;
	case DSK_DIAG_INFO_TYPE_TALK_COUNT:
	{
		// todo json库要mtsw功能开启才支持
		/*
		cJSON *root = cJSON_CreateObject();
		cJSON_AddNumberToObject(root, "sessionnum", talklogic_GetSessionNum());
		// 打印JSON数据包
		char *out = cJSON_Print(root);
		strResult = out;
		// 释放内存
		cJSON_Delete(root);
		free(out);
		*/
		strResult.append("{");

		strResult = strResult + commonAPI_FormatString("\"sessionnum\":%d", talklogic_GetSessionNum());

		strResult.append("}");
	}
	break;
	case DSK_DIAG_INFO_TYPE_TALK_STATE:
	{
		// todo json库要mtsw功能开启才支持
		/*
		cJSON *root = cJSON_CreateObject();
		cJSON_AddNumberToObject(root, "sessionnum", talklogic_GetSessionNum());
		// 打印JSON数据包
		char *out = cJSON_Print(root);
		strResult = out;
		// 释放内存
		cJSON_Delete(root);
		free(out);
		*/
		nRet = (talklogic_GetCallInfoByCallId(msg.lParam) == NULL ? 0 : 1);
	}
	break;
	case DSK_DIAG_INFO_TYPE_TALK_EXIST:
	{
		nRet = talklogic_SessionExist(false);
	}
	break;
	case DSK_DIAG_INFO_TYPE_CONTACT:
	{
		// 联系人
		strResult = "Not define contact";
	}
	break;
	case DSK_DIAG_INFO_TYPE_CONTACT_GROUP:
	{
		// 联系人
		strResult = "Not define contact";
	}
	break;
	case DSK_DIAG_INFO_TYPE_CONTACT_COUNT:
	{
		// 联系人
		//strResult = "Not define contact";
		strResult.append("{");

		strResult = strResult + commonAPI_FormatString("\"local\":%d,", Dir_GetContactCount(kszAllContactId, LOCAL_DIR));
		strResult = strResult + commonAPI_FormatString("\"log\":%d", Dir_GetContactCount(kszAllContactId, LOCAL_LOG));

		strResult.append("}");
	}
	break;
	case DSK_DIAG_INFO_TYPE_CONTACT_INFO:
	{
		// 联系人
		strResult = "Not define contact";
	}
	break;
	// 	case DSK_DIAG_INFO_TYPE_MENU_SFB_PRESENCE:
	// 	{
	// 		nRet = modlync_GetPresenceState(msg.lParam);
	// 	}
	// 	break;
	// ...
	case DSK_DIAG_INFO_TYPE_MENU_USB_INFO:
	{
		Dsk_USBInfo info;
		memset(&info, 0, sizeof(Dsk_USBInfo));
#ifdef IF_USB_SUPPORT
		info.state = modRecord_IsStorageConnect() ? DSKUSBINSERT : DSKUSBNONE;
		// 待扩展
		info.type = modRecord_IsStorageConnect() ? DSKUSBDISK : DSKUSBUNKNOWN;
#endif //#ifdef IF_USB_SUPPORT
		DUMPINFO_LOG("usb insert[%d] type[%d]", info.state, info.type);

		msg.ReplyMessageEx(1, sizeof(Dsk_USBInfo), (void *)(&info));
		return 0;
	}
	break;
	case DSK_DIAG_INFO_TYPE_MENU_EXP_INFO:
	{
		Dsk_EXPInfo info;
		memset(&info, 0, sizeof(Dsk_EXPInfo));
		info.count = exp_GetEXPNum();
		info.type = exp_GetEXPType();

		info.ehs = voice_IsEHSInserted() ? DSKEHSINSERT : DSKEHSNONE;

		DUMPINFO_LOG("exp insert[%d] type[%d]", info.count, info.type);

		msg.ReplyMessageEx(1, sizeof(Dsk_EXPInfo), (void *)(&info));
		return 0;
	}
	break;
	case DSK_DIAG_INFO_TYPE_MENU_BLUETOOTH_INFO:
	{
		Dsk_BluetoothInfo info;
		memset(&info, 0, sizeof(Dsk_BluetoothInfo));

		// todo
#ifdef IF_SUPPORT_BLUETOOTH
		info.donglestate = (Bluetooth_GetDeviceState() == DS_NO_DONGLE) ? DSKBTDOGNONE : DSKBTDOGINSERT;
		info.connect = Bluetooth_IsConnected() ? DSKBTCONNECT : DSKBTDISCONNECT;
		DeviceInfo stBtInfo;
		if (Bluetooth_GetDeviceInfo(stBtInfo))
		{
			strncpy(info.szBtDongle, stBtInfo.strName.c_str(), chMIN(stBtInfo.strName.size(), DSKBTINFOSIZE - 1));
			strncpy(info.szBTDongleMac, stBtInfo.strMac.c_str(), chMIN(stBtInfo.strMac.size(), DSKBTINFOSIZE - 1));
		}

		if (Bluetooth_GetPairedInfo(stBtInfo))
		{
			strncpy(info.szBTDevice, stBtInfo.strName.c_str(), chMIN(stBtInfo.strName.size(), DSKBTINFOSIZE - 1));
			strncpy(info.szBTDeviceMac, stBtInfo.strMac.c_str(), chMIN(stBtInfo.strMac.size(), DSKBTINFOSIZE - 1));
		}

		if (info.connect == DSKBTCONNECT)
		{
			info.type = Bluetooth_IsMobileConnected() ? DSKBTTYPEMOBILE : DSKBTTYPEHEADSET;
		}
#endif //#ifdef IF_SUPPORT_BLUETOOTH

		DUMPINFO_LOG("bt dongle[%d][%s][%s] dev[%s][%s] connect[%d]", info.donglestate,
			info.szBtDongle, info.szBTDongleMac, info.szBTDevice, info.szBTDeviceMac, info.connect);

		msg.ReplyMessageEx(1, sizeof(Dsk_BluetoothInfo), (void *)(&info));
	}
	break;
	case DSK_DIAG_INFO_TYPE_MENU_WIFI_INFO:
	{
		Dsk_WifiInfo info;
		memset(&info, 0, sizeof(Dsk_WifiInfo));
		// todo
#ifdef IF_SUPPORT_WIFI
		info.donglestate = (WIFI_IsDongleVaild() ? DSKWIFIDOGNONE : DSKWIFIDOGINSERT);
		info.connect = WIFI_IsConnected() ? DSKWIFICONNECT : DSKWIFIDISCONNECT;

		yl::string strData;
		if (info.donglestate == DSKWIFIDOGINSERT)
		{
			strData = WIFI_GetLocalMac(strData);
			strncpy(info.szWifiDongleMac, strData.c_str(), chMIN(strData.size() + 1, DSKWIFIINFOSIZE - 1));
		}

		if (info.connect == DSKWIFICONNECT)
		{
			WiFiNetInfo stData;
			WIFI_GetCurrentNetConnectInfo(stData);
			strncpy(info.szWifiSSID, stData.strSSID.c_str(), chMIN(stData.strSSID.size(), DSKWIFIINFOSIZE - 1));
			strncpy(info.szWifiMac, stData.strMac.c_str(), chMIN(stData.strMac.size(), DSKWIFIINFOSIZE - 1));
		}
#endif //#ifdef IF_SUPPORT_WIFI

		DUMPINFO_LOG("wifi dongle[%d][%s] dev[%s][%s] connect[%d]", info.donglestate,
			info.szWifiDongleMac, info.szWifiSSID, info.szWifiSSID, info.connect);

		msg.ReplyMessageEx(1, sizeof(Dsk_WifiInfo), (void *)(&info));
	}
	break;
	case DSK_DIAG_INFO_TYPE_LICENSE:
	{
		// todo v83 不支持
		Dsk_LicenseInfo info;
		memset(&info, 0, sizeof(Dsk_LicenseInfo));

		AMR_LICENSE_RESULT eResult = comminUnits_GetAmrLicenseResult();
		if (eResult == AMR_LICENSE_INSTALL_SUCCESS)
		{
			info.state = DSKLICENSEINSTALL;
			info.type = DSKPERMISSIONTYPEAMR;
		}
		else
		{
			info.state = DSKLICENSENONE;
		}
		msg.ReplyMessageEx(1, sizeof(Dsk_LicenseInfo), (void *)(&info));
		return 0;
	}
	break;
	case DSK_DIAG_INFO_TYPE_MENU_SFB_EXCHANGE_STATE:
	case DSK_DIAG_INFO_TYPE_MENU_SFB_BTOE_STATE:
	case DSK_DIAG_INFO_TYPE_MENU_SFB_CAP_STATE:
	{
		// todo
		msg.ReplyMessage(0);
		nRet = 0;
	}
	break;
	default:
		strResult = "Not define";
		break;
	}

	if (strResult.size() > 1024)
	{
		COMMONUNITS_ERR("dump info data size > 1024");
		strResult.clear();
	}

	msg.ReplyMessageEx(nRet, strlen(strResult.c_str()), (void *)strResult.c_str());
	return 0;
}
