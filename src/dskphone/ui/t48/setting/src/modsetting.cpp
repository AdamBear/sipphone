#include "setting/include/modsetting.h"

#include "setting/include/common.h"
#include <ETLLib/ETLLib.hpp>
#include "interfacedefine.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "devicelib/networkserver.h"

#include "dateandtimecontroller.h"
#include "displaycontroller.h"
#include "networkcontroller.h"
#include "textmessagecontroller.h"
#include "soundcontroller.h"
#include "voicemailcontroller.h"
#include "autoanswercontroller.h"
#include "anonymouscallcontroller.h"
#include "resetandrebootcontroller.h"
#include "languagecontroller.h"
#include "contrastcontroller.h"
#include "path_define.h"
#include "devicelib/phonedevice.h"
#include <configiddefine.h>
#include "configparser/modconfigparser.h"
#include <map>
#include "commonunits/modcommonunits.h"
#include "feature/include/modfeature.h"
#include "idle/idle_data_define.h"
#include "idlescreen/include/modidlescreen.h"
#include "account/include/modaccount.h"
#include "settingui/include/modsettingui.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "keyguard/include/modkeyguard.h"
#include "autopcontrol/include/modautopcontrol.h"
#include "language/common/common_function.h"

// 最大的buffer长度.
static const int knMaxBufferLen = 260;

typedef std::map<ESettingDefaultIconType, yl::string> SettingDefaultIconMap;

static SettingDefaultIconMap lg_deftIconMap;

int Setting_GetCurDateFormatType()
{
    // 获取当前的日期格式.
    if (g_pDateAndTimeController != NULL)
    {
        DateAndTimeFormatData stData;
        if (g_pDateAndTimeController->GetDateAndTimeFormat(&stData))
        {
            return stData.m_eDateFormat;
        }
    }
    return DATE_FORMAT_TYPE_NONE;
}

int Setting_GetCurTimeFormatType()
{
    // 获取当前的时间格式.
    if (g_pDateAndTimeController != NULL)
    {
        DateAndTimeFormatData stData;
        if (g_pDateAndTimeController->GetDateAndTimeFormat(&stData))
        {
            return stData.m_eTimeFormat;
        }
    }
    return TIME_FORMAT_TYPE_NONE;
}

//yl::string Setting_Time2String(time_t tTime, const yl::string& strSep /*= " "*/)
/*{
    yl::string strDT;
    // 获取日期格式.
    tm* tmLocal = localtime(&tTime);
    if (tmLocal != NULL)
    {
        char szBuffer[knMaxBufferLen + 1] = {'\0'};

        // 获取日期.
        int nDFType = Setting_GetCurDateFormatType();
        switch (nDFType)
        {
        case DATE_FORMAT_WWW_MMM_DD:  // WWW MMM DD.
            //sprintf(szBuffer, "%d %d %02d", tmLocal->tm_wday + 1,
               // tmLocal->tm_mon + 1, tmLocal->tm_mday);

             sprintf(szBuffer, "%s %s %02d", GetWeeday(tmLocal->tm_wday),
                 GetMonth(tmLocal->tm_mon + 1), tmLocal->tm_mday);
            break;

        case DATE_FORMAT_DD_MMM_YY:  // DD-MM-YY.
            sprintf(szBuffer, "%02d-%s-%02d", tmLocal->tm_mday,
                GetMonth(tmLocal->tm_mon + 1), tmLocal->tm_year + 1900);
            break;

        case DATE_FORMAT_YYYY_MM_DD:  // // YYYY-MM-DD
            sprintf(szBuffer, "%d-%02d-%02d", tmLocal->tm_year + 1900,
                tmLocal->tm_mon + 1, tmLocal->tm_mday);
            break;

        case DATE_FORMAT_DD_MM_YYYY:  // DD/MM/YYYY.
            sprintf(szBuffer, "%02d/%02d/%d", tmLocal->tm_mday,
                tmLocal->tm_mon + 1, tmLocal->tm_year + 1900);
            break;

        case DATE_FORMAT_MM_DD_YY:  // MM/DD/YY.
            sprintf(szBuffer, "%02d/%02d/%02d", tmLocal->tm_mon + 1,
                tmLocal->tm_mday, (tmLocal->tm_year + 1900) % 100);
            break;

        case DATE_FORMAT_DD_MMM_YYYY:  // DD MMM YYYY.
            sprintf(szBuffer, "%02d %s %d", tmLocal->tm_mday,
                GetMonth(tmLocal->tm_mon + 1), tmLocal->tm_year + 1900);
            break;

        case DATE_FORMAT_WWW_DD_MMM:  // WWW DD MMM.
            //sprintf(szBuffer, "%d %02d %d", tmLocal->tm_wday + 1,
                //tmLocal->tm_mday, tmLocal->tm_mon + 1);
             sprintf(szBuffer, "%s %02d %s", GetWeeday(tmLocal->tm_wday) ,
                  tmLocal->tm_mday, GetMonth(tmLocal->tm_mon + 1));
            break;

        default:
            break;
        }

        szBuffer[knMaxBufferLen] = '\0';
        strDT.append(szBuffer);

        // 获取时间.
        memset(szBuffer, '\0', sizeof(szBuffer));
        int nTFType = Setting_GetCurTimeFormatType();
        switch (nTFType)
        {
            //在时间之后要添加AM/PM
        case TIME_FORMAT_12HOUR:
            //下午
            if (tmLocal->tm_hour > 12)
            {
                sprintf(szBuffer, "%02d:%02d:%02d PM",
                    tmLocal->tm_hour - 12, tmLocal->tm_min, tmLocal->tm_sec);
            }
            //上午
            else if (tmLocal->tm_hour < 12
                && tmLocal->tm_hour > 0)
            {
                sprintf(szBuffer, "%02d:%02d:%02d AM",
                    tmLocal->tm_hour, tmLocal->tm_min, tmLocal->tm_sec);
            }
            else if (tmLocal->tm_hour == 12)
            {
                //中午12点
                sprintf(szBuffer, "12:%02d:%02d PM",
                    tmLocal->tm_min, tmLocal->tm_sec);
            }
            else if (tmLocal->tm_hour == 0)
            {
                //半夜12点
                sprintf(szBuffer, "12:%02d:%02d AM",
                    tmLocal->tm_min, tmLocal->tm_sec);
            }
            break;

        case TIME_FORMAT_24HOUR:
            sprintf(szBuffer, "%02d:%02d:%02d", tmLocal->tm_hour,
                tmLocal->tm_min, tmLocal->tm_sec);
            break;

        default:
            break;
        }

        szBuffer[knMaxBufferLen] = '\0';
        strDT.append(strSep);
        strDT.append(szBuffer);
    }

    return strDT;
}*/

// 获取时分.
yl::string GetHourMinute(int nHour, int nMin)
{
    yl::string strAPM;  // 上下午标识, 可能为空.
    int nHourGet = nHour;

    // 根据setting的格式, 返回时分.
    char szBuffer[knMaxBufferLen + 1] = {'\0'};
    switch (Setting_GetCurTimeFormatType())
    {
    case TIME_FORMAT_12HOUR:
        if (nHour > 12)
        {
            //下午
            nHourGet = nHour - 12;
            strAPM = "PM";
        }
        else if (nHour < 12 && nHour > 0)
        {
            //上午
            nHourGet = nHour;
            strAPM = "AM";
        }
        else if (nHour == 12)
        {
            //中午12点
            nHourGet = 12;
            strAPM = "PM";
        }
        else if (nHour == 0)
        {
            //半夜12点
            nHourGet = 12;
            strAPM = "AM";
        }
        break;

    default:
        nHourGet = nHour;
        SETTING_WARN("Setting_CallLogTime2String Invalid format!");
        break;
    }

    if (strAPM.empty())
    {
        sprintf(szBuffer, "%02d:%02d", nHourGet, nMin);
    }
    else
    {
        sprintf(szBuffer, "%02d:%02d %s", nHourGet, nMin, strAPM.c_str());
    }

    szBuffer[knMaxBufferLen] = '\0';
    return szBuffer;
}

// 获取时分.
static yl::string GetHourMinuteSecond(int nHour, int nMin, int nSec)
{
    yl::string strAPM;  // 上下午标识, 可能为空.
    int nHourGet = nHour;

    // 根据setting的格式, 返回时分.
    char szBuffer[knMaxBufferLen + 1] = {'\0'};
    switch (Setting_GetCurTimeFormatType())
    {
    case TIME_FORMAT_12HOUR:
        if (nHour > 12)
        {
            //下午
            nHourGet = nHour - 12;
            strAPM = "PM";
        }
        else if (nHour < 12 && nHour > 0)
        {
            //上午
            nHourGet = nHour;
            strAPM = "AM";
        }
        else if (nHour == 12)
        {
            //中午12点
            nHourGet = 12;
            strAPM = "PM";
        }
        else if (nHour == 0)
        {
            //半夜12点
            nHourGet = 12;
            strAPM = "AM";
        }
        break;

    default:
        nHourGet = nHour;
        SETTING_WARN("Setting_CallLogTime2String Invalid format!");
        break;
    }

    if (strAPM.empty())
    {
        sprintf(szBuffer, "%02d:%02d:%02d", nHourGet, nMin, nSec);
    }
    else
    {
        sprintf(szBuffer, "%02d:%02d:%02d %s", nHourGet, nMin, nSec, strAPM.c_str());
    }

    szBuffer[knMaxBufferLen] = '\0';
    return szBuffer;
}

/*yl::string Setting_Time2String(time_t tTime, bool bAddTime, const yl::string& strSep)
{
    static const long klSecPerDay = 24 * 60 * 60;
    // 日期分隔.
    static const char kszSepStr[] = " ";

    tm* tmLocal = localtime(&tTime);
    if (tmLocal == NULL)
    {
        SETTING_WARN("Setting_CallLogTime2String tmLocal == NULL");
        return "XXXX-XX-XX XX:XX:XX";
    }

    // 获取当前时间.
    time_t tCurrent = time(NULL);
    time_t tTodayZero = (tCurrent / klSecPerDay) * klSecPerDay;  // 今天0点的时间.
    time_t tTomorrowZero = tTodayZero + klSecPerDay;  // 明天0点的时间.
    time_t tSixDayBeforeZero = tTodayZero - klSecPerDay * 7;  // 7天前的0点时间.

    yl::string strRet;
    if (tTime >= tTodayZero
        && tTime < tTomorrowZero)
    {
        // 如果是今天, 则显示为Today.
        strRet = "Today";
        strRet.append(kszSepStr);
        strRet.append(GetHourMinute(tmLocal->tm_hour, tmLocal->tm_min));
    }
    else if (tTime >= tTodayZero - klSecPerDay
        && tTime < tTodayZero)
    {
        // 如果是昨天
        strRet = "Yesterday";
        strRet.append(kszSepStr);
        strRet.append(GetHourMinute(tmLocal->tm_hour, tmLocal->tm_min));
    }
    else if (tTime >= tSixDayBeforeZero
        && tTime < tTodayZero - klSecPerDay)
    {
        // 如果是7天前的0点到昨天23:59之间, 则直接显示星期.
        strRet = GetWeedayFull(tmLocal->tm_wday);
        strRet.append(kszSepStr);
        strRet.append(GetHourMinute(tmLocal->tm_hour, tmLocal->tm_min));
    }
    else
    {
        // 其他情况都显示年月日.
        char szBuffer[knMaxBufferLen + 1] = {'\0'};
        switch (Setting_GetCurDateFormatType())
        {
        case DATE_FORMAT_WWW_MMM_DD:  // WWW MMM DD.
            sprintf(szBuffer, "%s %s %02d", GetWeeday(tmLocal->tm_wday),
                GetMonth(tmLocal->tm_mon + 1), tmLocal->tm_mday);
            break;

        case DATE_FORMAT_DD_MMM_YY:  // DD-MM-YY.
            sprintf(szBuffer, "%02d-%s-%02d", tmLocal->tm_mday,
                GetMonth(tmLocal->tm_mon + 1), tmLocal->tm_year + 1900);
            break;

        case DATE_FORMAT_YYYY_MM_DD:  // // YYYY-MM-DD
            sprintf(szBuffer, "%d-%02d-%02d", tmLocal->tm_year + 1900,
                tmLocal->tm_mon + 1, tmLocal->tm_mday);
            break;

        case DATE_FORMAT_DD_MM_YYYY:  // DD/MM/YYYY.
            sprintf(szBuffer, "%02d/%02d/%d", tmLocal->tm_mday,
                tmLocal->tm_mon + 1, tmLocal->tm_year + 1900);
            break;

        case DATE_FORMAT_MM_DD_YY:  // MM/DD/YY.
            sprintf(szBuffer, "%02d/%02d/%02d", tmLocal->tm_mon + 1,
                tmLocal->tm_mday, (tmLocal->tm_year + 1900) % 100);
            break;

        case DATE_FORMAT_DD_MMM_YYYY:  // DD MMM YYYY.
            sprintf(szBuffer, "%02d %s %d", tmLocal->tm_mday,
                GetMonth(tmLocal->tm_mon + 1), tmLocal->tm_year + 1900);
            break;

        case DATE_FORMAT_WWW_DD_MMM:  // WWW DD MMM.
            sprintf(szBuffer, "%s %02d %s", GetWeeday(tmLocal->tm_wday) ,
                tmLocal->tm_mday, GetMonth(tmLocal->tm_mon + 1));
            break;

        default:
            break;
        }

        szBuffer[knMaxBufferLen] = '\0';
        strRet = szBuffer;
    }

    if (bAddTime)
    {
        char szBuffer[knMaxBufferLen + 1] = {'\0'};
        // 获取时间.
        memset(szBuffer, '\0', sizeof(szBuffer));
        int nTFType = Setting_GetCurTimeFormatType();
        switch (nTFType)
        {
            //在时间之后要添加AM/PM
        case TIME_FORMAT_12HOUR:
            //下午
            if (tmLocal->tm_hour > 12)
            {
                sprintf(szBuffer, "%02d:%02dPM", tmLocal->tm_hour - 12, tmLocal->tm_min);
            }
            //上午
            else if (tmLocal->tm_hour < 12
                && tmLocal->tm_hour > 0)
            {
                sprintf(szBuffer, "%02d:%02dAM", tmLocal->tm_hour, tmLocal->tm_min);
            }
            else if (tmLocal->tm_hour == 12)
            {
                //中午12点
                sprintf(szBuffer, "12:%02dPM", tmLocal->tm_min);
            }
            else if (tmLocal->tm_hour == 0)
            {
                //半夜12点
                sprintf(szBuffer, "12:%02dAM", tmLocal->tm_min);
            }
            break;
        case TIME_FORMAT_24HOUR:
            sprintf(szBuffer, "%02d:%02d", tmLocal->tm_hour,
                tmLocal->tm_min);
            break;
        default:
            break;
        }

        szBuffer[knMaxBufferLen] = '\0';
        strRet.append(strSep);
        strRet.append(szBuffer);
    }

    return strRet;
}*/

// 新需求:历史记录显示月日及时间
yl::string Setting_CallLogTime2SimpleString(time_t tTime)
{
#define klSecPerDay (24 * 60 * 60)

    tm * tmLocal = localtime(&tTime);
    if (tmLocal == NULL)
    {
        SETTING_WARN("Setting_CallLogTime2SimpleString tmLocal == NULL");
        return "XXXX-XX-XX XX:XX:XX";
    }

    // 获取当前时间.
    time_t tCurrent = time(NULL);
    time_t tTodayZero = (tCurrent / klSecPerDay) * klSecPerDay;  // 今天0点的时间.
    time_t tTomorrowZero = tTodayZero + klSecPerDay;  // 明天0点的时间.
    time_t tSixDayBeforeZero = tTodayZero - klSecPerDay * 7;  // 7天前的0点时间.

    // 新需求:历史记录显示月日及时间
    char szBuffer[knMaxBufferLen + 1] = {'\0'};
    sprintf(szBuffer, "%s %02d %s", GetMonth(tmLocal->tm_mon + 1), tmLocal->tm_mday,
            GetHourMinute(tmLocal->tm_hour, tmLocal->tm_min).c_str());
    szBuffer[knMaxBufferLen] = '\0';
    return szBuffer;
}

// 处理配置发生改变的消息.
// | refMsg | 是消息.
// 如果处理成功则返回TRUE, 否则返回FALSE.
static LRESULT OnMessageProcess(msgObject & refMsg)
{
    SETTING_INFO("Setting OnMessageProcess refMsgId[%d] wParam[%d] lParam[%d]",
                 refMsg.message, refMsg.wParam, refMsg.lParam);

    switch (refMsg.message)
    {
    case CONFIG_MSG_BCAST_CHANGED:
        {
            switch (refMsg.wParam)
            {
            case ST_WALLPAPER:
                {
                    return g_pDisplayController->ProcessMessage(refMsg);
                }
                break;
            default:
                {
                    return FALSE;
                }
                break;
            }
        }
        break;
    case MSG_UPDATE_TIMEZONE:
        {
            g_pDateAndTimeController->ReloadTimeZoneList();
            return netSendTimeMsg();
        }
        break;
    default:
        break;
    }

    return FALSE;
}

void Setting_RegisterMessage()
{
    // 注册消息.
    etl_RegisterMsgHandle(CONFIG_MSG_BCAST_CHANGED,
                          CONFIG_MSG_BCAST_CHANGED,
                          OnMessageProcess);
    // 注册消息.
    etl_RegisterMsgHandle(MSG_UPDATE_TIMEZONE,
                          MSG_UPDATE_TIMEZONE,
                          OnMessageProcess);
}

void Setting_UnRegisterMessage()
{
    // 取消消息注册.
    etl_UnregisterMsgHandle(CONFIG_MSG_BCAST_CHANGED,
                            CONFIG_MSG_BCAST_CHANGED,
                            OnMessageProcess);
    // 取消消息注册.
    etl_UnregisterMsgHandle(MSG_UPDATE_TIMEZONE,
                            MSG_UPDATE_TIMEZONE,
                            OnMessageProcess);
}

void Setting_Init()
{
    // 初始化Voice Mail.
    g_pVoiceMailController;
    // 初始化Text Message.
    g_pTextMessageController;
    // 初始化AutoAnswer
    g_pAutoAnswerController;
    //初始化匿名呼叫
    g_pAnonymousCallController;
    // 初始化NetworkControlelr.
    g_pNetworkController;
    // 初始化LanguageController
    g_pLanguageController;
    // Autop
    Autop_Init();
}

void Setting_Final()
{
    // 清除Text Message实例.
    NS_SETTING::CTextMessageController::ReleaseInstance();
    // 清除Voice Mail实例.
    NS_SETTING::CVoiceMailController::ReleaseInstance();
    // 销毁匿名呼叫.
    NS_SETTING::CAnonymousCallController::ReleaseInstance();
    // 清除NetworkControlelr.
    NS_SETTING::CNetworkController::ReleaseInstance();
}

yl::string Setting_GetCurrentWallpaper()
{
    // 获取当前墙纸.
    return g_pDisplayController != NULL
           ? g_pDisplayController->GetCurrentWallpaper() : "";
}

yl::string Setting_GetCurrentTheme()
{
    // 获取当前主题.
    return g_pDisplayController != NULL
           ? g_pDisplayController->GetCurrentTheme() : "";
}

yl::string Setting_GetCurrentRing()
{
    // 获取当前铃声.
    return g_pSoundController != NULL
           ? g_pSoundController->GetCurrentRingPath() : "";
}

bool Setting_GetVoiceMailList(VoiceMailInfoListData * pData)
{
    if (pData == NULL)
    {
        return false;
    }

    // 从Voice Mail中获取信息列表.
    return g_pVoiceMailController != NULL
           ? g_pVoiceMailController->GetInfoList(pData) : false;
}

void Setting_ClickVoiceMailInfo(int nIndex)
{
    if (NULL != g_pVoiceMailController)
    {
        g_pVoiceMailController->ClickInfoByIndex(nIndex);
    }
}

bool Setting_GetTextMsgList(AccountTextMsgListData * pData)
{
    if (pData == NULL)
    {
        return false;
    }

    // 从Voice Mail中获取信息列表.
    return g_pTextMessageController != NULL
           ? g_pTextMessageController->GetAccountMsgList(pData) : false;
}

int Setting_GetTotalUnReadMessage()
{
    return g_pTextMessageController != NULL
           ? g_pTextMessageController->GetTotalUnRead() : 0;
}

int Setting_GetAccountUnRead(int nAccountId)
{
    return g_pTextMessageController != NULL
           ? g_pTextMessageController->GetAccountUnRead(nAccountId) : 0;
}

bool Setting_OnMsgKeyPress()
{
    return g_pVoiceMailController != NULL
           ? g_pVoiceMailController->OnMsgKeyPress() : false;
    return true;
}

//设置Setting中用到的默认图标
void Setting_SetDefaultIcon(ESettingDefaultIconType eIconType, const yl::string & strIconFile)
{
    lg_deftIconMap[eIconType] = strIconFile;
}

//获取Setting中用到的默认图标
yl::string Setting_GetDefaultIcon(ESettingDefaultIconType eIconType)
{
    SettingDefaultIconMap::const_iterator iter = lg_deftIconMap.find(eIconType);

    if (iter != lg_deftIconMap.end())
    {
        return iter->second;
    }
    else
    {
        return "";
    }
}


//获取当前扩展台的对比度
int Setting_GetCurrentContrastLevel()
{
    return g_pContrastController->GetCurrentContrastLevel();
}


int Setting_GetCurrentBacklightLevel()
{
    DisplayGeneralData pData;
    g_pDisplayController->GetGeneralData(&pData);
    return pData.m_nActiveLevel;
}

yl::string Setting_GetUserDisplayName(yl::string strUsername)
{
    yl::string strTem = strUsername;

    if (strUsername == "admin")
    {
        strTem = feature_UserModeGetUserName(UT_ADMIN);
    }
    else if (strUsername == "var")
    {
        strTem = feature_UserModeGetUserName(UT_VAR);
    }
    else if (strUsername == "user")
    {
        strTem = feature_UserModeGetUserName(UT_USER);
    }

    return strTem;
}

//话机重启，基于DeviceLib的看门狗重启加上一些清理操作
void Setting_Reboot(int iTimeDelay/* = 1*/)
{
    g_pResetAndRebootController->RebootNow(chMAX(iTimeDelay, 1));
}

// 注册重启前提示的回调函数
void Setting_RegisterRebootTipCallBack(FunRebootTipCallBack fun)
{
    g_pResetAndRebootController->RegisterRebootTipCallBack(fun);
}

bool Setting_ConnectVoiceMail(int iAccountID/* = -1*/)
{
    //添加了一个KGM_MODULE_MESSAGE模块的判断，用来判断message硬按键
    if (!keyGuard_IsEnableModuld(KGM_MODULE_MESSAGE))
    {
        return false;
    }

#if IF_FEATURE_BROADSOFT_VOICEMAIL
    if (configParser_GetConfigInt(kszVoiceMailMessageKeyMode) == 1 && iAccountID == -1)
    {
        SettingUI_OpenViewVoiceMailPage();
        return true;
    }
#endif

    int iCurrentLineID = (iAccountID == -1) ? acc_GetLastVMailAccountId() : iAccountID;
    iCurrentLineID = (iCurrentLineID == -1) ? acc_GetDefaultAccount() : iCurrentLineID;
    // 优先呼出当前账号的Voicemail
    if (iCurrentLineID < 0
            || iCurrentLineID >= acc_AccountNum())
    {
        SettingUI_OpenSetVoiceMailPage(0);
        return false;
    }

    yl::string strVoiceNum = configParser_GetCfgItemStringValue(kszAccountVoiceMailNumber,
                             iCurrentLineID);
    if (strVoiceNum.empty())
    {
        SettingUI_OpenSetVoiceMailPage(iCurrentLineID);
        return true;
    }
    else//呼出则清除状态
    {
        talklogic_CallOut(strVoiceNum.c_str(), "", iCurrentLineID);
        idleScreen_ClearPopupFlag(PS_STATE_VOICEMAIL);
        return true;
    }

    return false;
}

