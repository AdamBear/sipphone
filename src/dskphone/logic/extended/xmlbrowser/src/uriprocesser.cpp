#include "logic_inc.h"
#include "../include/modxmlbrowser.h"
#include "lamp/powerled/include/modpowerledlogic.h"
#include "adapterbox/include/modadapterbox.h"
#include "dsskey/include/moddsskey.h"
#include "lamp/ledlamp/include/modledlamp.h"
#include "exp/include/modexp.h"
#include "exp/src/expdrive/deviceplugin/modexpscan.h"

#ifdef IF_FEATURE_XMLBROWSER
CURIProcesser & GetURIProcesserInstance()
{
    // 用函数内static方式控制全局对象的构造顺序
    static CURIProcesser s_URIProcesser;
    return s_URIProcesser;
}

// 处理URI
void CURIProcesser::ProcessURI(const yl::string & strURI)
{
    if (strURI.empty() || strURI.size() > 1024)
    {
        return;
    }

    // 根据“:”区分类型和内容
    //sscanf(strURI.c_str(), "%[^:]:%s", szType, szContent);
    // 兼容存在空格的容错处理
    yl::string strType, strContent;
    size_t uBeginPos = 0;
    uBeginPos = strURI.find_first_of(':');
    if (uBeginPos == yl::string::npos)
    {
        return;
    }
    strType = strURI.substr(0, uBeginPos);
    strContent = strURI.substr(uBeginPos + 1, strURI.length() - uBeginPos);
    strType = commonAPI_TrimString(strType, " ");
    strContent = commonAPI_TrimString(strContent, " ");
    XMLBROWSER_INFO("strType[%s], strContent[%s]", strType.c_str(), strContent.c_str());

    // 处理http(s)类型的URI
    if (strcmp(strType.c_str(), "http") == 0 || strcmp(strType.c_str(), "https") == 0)
    {
        yl::string strUrl = strURI;

        strUrl.trim_both(" \r\n\t"); //兼容URL头尾异常符号

        char szSuffix[1024] = { 0 };
        char szServer[128] = { 0 };
        if (sscanf(strContent.c_str(), "//%[^/]%s", szServer, szSuffix) == 2)
        {
            strUrl = szSuffix;
            // 先进行编码转换
            commonUnits_EncodeURI(strUrl);

            strUrl = commonAPI_FormatString("%s://%s%s", strType.c_str(), szServer, strUrl.c_str());
            XMLBROWSER_INFO("After EnCode URL is: %s", strUrl.c_str());
        }
        ProcessHttp(strUrl.c_str());
    }
    // 处理拨号呼出类型的URI
    else if (strcmp(strType.c_str(), "Dial") == 0)
    {
        ProcessDial(strContent.c_str());
    }
    // 处理灯控制类型的URI
    else if (strcmp(strType.c_str(), "Led") == 0)
    {
        // 先停提示音，以免声道的灯受到影响，如亮耳麦灯
        _XMLBrowserManager.StopBeep();
        ProcessLed(strContent.c_str());
    }
    // 处理发送按键类型的URI
    else if (strcmp(strType.c_str(), "Key") == 0)
    {
        // 先停提示音，以免声道的按键响应受到影响
        _XMLBrowserManager.StopBeep();
        ProcessKey(strContent.c_str());
    }
    // 处理播放音频类型的URI
    else if (strcmp(strType.c_str(), "Wav.Play") == 0)
    {
        ProcessPlayWave(strContent.c_str());
    }
    // 处理停止播放音频类型的URI
    else if (strcmp(strType.c_str(), "Wav.Stop") == 0)
    {
        ProcessStopWave();
    }
    // 处理命令类型的URI
    else if (strcmp(strType.c_str(), "Command") == 0)
    {
        ProcessCommand(strContent.c_str());
    }
}

// 处理http(s)类型的URI
void CURIProcesser::ProcessHttp(const char * szURI)
{
    if (IsURIEmpty(szURI))
    {
        return;
    }

    // 进入下一次URL且不退出现有的Element
    _XMLBrowserManager.Enter(szURI, false);
}

// 处理拨号呼出类型的URI
void CURIProcesser::ProcessDial(const char * szURI)
{
    if (IsURIEmpty(szURI))
    {
        return;
    }
    // ??waves??
    //caijz ????????????
    voice_StopRing();
    // ??
    talklogic_CallOut(szURI, "");
}

// 获取不同机型的灯号
int CURIProcesser::GetLampID(int iLightIndex)
{
    //int iLampID = -1;
    return ledlamp_GetLampID((LAMP_EVENTTYPE)iLightIndex);
    /*switch (devicelib_GetPhoneType())
    {
    case PT_T21:
    case PT_T23:
        {
            switch (iLightIndex)
            {
            case LAMP_SMS:iLampID = 7;break;
            case LAMP_POWSER: iLampID = 0;break;
            default:
                break;
            }
        }
        break;
    case PT_T19:
    case PT_T40:
        {
            switch (iLightIndex)
            {
            case LAMP_POWSER: iLampID = 0;break;
            default:
                break;
            }
        }
        break;
    case PT_T27:
    case PT_T27S:
        {
            switch (iLightIndex)
            {
            case LAMP_POWSER: iLampID = 0;break;
            case LAMP_SMS: iLampID = 11;break;
            case LAMP_HEADSET: iLampID = 5;break;
            default:
                break;
            }
        }
        break;
    case PT_T61:
        {
            switch (iLightIndex)
            {
            case LAMP_POWSER: iLampID = 0;break;
            case LAMP_SMS: iLampID = 1;break;
            case LAMP_HEADSET: iLampID = 2;break;
            default:
                break;
            }
        }
        break;
    case PT_T66:
        {
            switch (iLightIndex)
            {
            case LAMP_POWSER: iLampID = 0;break;
            case LAMP_SMS: iLampID = 1;break;
            case LAMP_HEADSET: iLampID = 2;break;
            case LAMP_MUTE: iLampID = 3;break;
            case LAMP_SPEAKER: iLampID = 4;break;
            default:
                break;
            }
        }
        break;
    case PT_T29:
        {
            switch (iLightIndex)
            {
            case LAMP_POWSER: iLampID = 0;break;
            case LAMP_SMS:iLampID = 12;break;
            case LAMP_HEADSET: iLampID = 6;break;
            default:
                break;
            }
        }
        break;
    case PT_T69:
    case PT_T69S:
        {
            switch (iLightIndex)
            {
            case LAMP_POWSER: iLampID = 0;break;
            case LAMP_SMS: iLampID = 10;break;
            case LAMP_HEADSET: iLampID = 6;break;
            case LAMP_MUTE: iLampID = 4;break;
            case LAMP_SPEAKER: iLampID = 5;break;
            default:
                break;
            }
        }
        break;
    case PT_T46:
    case PT_T46S:
        {
            switch (iLightIndex)
            {
            case LAMP_POWSER: iLampID = 0;break;
            case LAMP_HEADSET: iLampID = 6;break;
            case LAMP_MUTE: iLampID = 12;break;
            case LAMP_SPEAKER: iLampID = 18;break;
            default:
                break;
            }
        }
        break;
    case PT_T41:
    case PT_T42:
    case PT_T43S:
    case PT_T43P:
        {
            switch (iLightIndex)
            {
            case LAMP_POWSER    : iLampID = 0;break;
            case LAMP_MUTE      : iLampID = 4;break;
            case LAMP_HEADSET   : iLampID = 10;break;
            case LAMP_SPEAKER   : iLampID = 16;break;
            default             :break;
            }
        }
        break;
    case PT_T48:
    case PT_T48S:
        {
            switch (iLightIndex)
            {
            case LAMP_POWSER    : iLampID = 0;break;
            case LAMP_MUTE      : iLampID = 2;break;
            case LAMP_HEADSET   : iLampID = 1;break;
            case LAMP_SPEAKER   : iLampID = 3;break;
            default             :break;
            }
        }
        break;
    case PT_T49:
        {
            switch (iLightIndex)
            {
            case LAMP_POWSER    : iLampID = 0;break;
            case LAMP_MUTE      : iLampID = 4;break;
            case LAMP_HEADSET   : iLampID = 2;break;
            case LAMP_SPEAKER   : iLampID = 1;break;
            case LAMP_SMS       : iLampID = 3;break;
            default             : break;
            }
        }
        break;
    case PT_T54S:
        {
            switch (iLightIndex)
            {
                case LAMP_POWSER:
                case LAMP_MUTE:
                case LAMP_HEADSET:
                case LAMP_SPEAKER:
                default:
                    break;
            }
        }
        break;
    case PT_T52:
        break;
    default:
        break;
    }
    return iLampID;*/

}

// 处理灯控制类型的URI
void CURIProcesser::ProcessLed(const char * szURI)
{
    if (IsURIEmpty(szURI) || strlen(szURI) > 49)
    {
        return;
    }

    yl::string strURI, strLed, strStatus;
    strURI = szURI;

    // 格式判断 [XXXX=off/on/slowflash/fastflash]
//  if (sscanf(szURI, "%[^=]=%s", szLed,  szStatus) != 2)
//  {
//      return;
//  }
    size_t uBeginPos = strURI.find_first_of('=');
    if (uBeginPos == yl::string::npos)
    {
        return;
    }
    strLed = strURI.substr(0, uBeginPos);
    strStatus = strURI.substr(uBeginPos + 1, strURI.length() - uBeginPos);
    strLed = commonAPI_TrimString(strLed, " ");
    strStatus = commonAPI_TrimString(strStatus, " ");
    XMLBROWSER_INFO("strURI[%s], strLed[%s], strStatus[%s] ", strURI.c_str(), strLed.c_str(),
                    strStatus.c_str());
    // 判断是否为扩展台
    if (strncmp(strLed.c_str(), "EXP", 3) == 0)
    {
        // 扩展台灯处理
        ProcessExpLed(strLed.c_str(), strStatus.c_str());
    }
    else
    {
        /*
         * Dsskey 灯
         */
        // 解析灯的闪烁类型（开，关，快慢闪）
        LEDSTATUS_TYPE eLightStatus = GetLightStatus(strStatus.c_str());
        int iDssKeyIndex = -1;
        LEDCOLOR_LTYE eColor = LT_GREEN;
        DSSKEY_MODULE_TYPE dsskeyModule = DMT_LINEKEY_MODULE;
        if (GetDsskeyLightInfo(strLed.c_str(), iDssKeyIndex, eColor, dsskeyModule))
        {
            XMLBROWSER_INFO("XMLBROWSER UpdateLight DsskeyIndex [%d],eColor[%d],eLightStatus[%d]",
                            iDssKeyIndex, eColor, eLightStatus);

#if IF_BUG_XMLBROWSER_EXP_LED
            LIGHT_Type eLight = GetLightType(eColor, eLightStatus);
            dsskey_SetDsskeyLightType(iDssKeyIndex, eLight);
#endif
            //int iDsskeyID = dsskey_GetDsskeyID(dsskeyModule, 0, iDssKeyIndex);
            // http://10.2.1.199/Bug.php?BugID=87303
            //dsskey_ResetDsskeyStatus(iDssKeyIndex);

            AdapterBox_XmlBrowserCtrlLight(iDssKeyIndex, eColor, eLightStatus);
            return;
        }

        /*
         * 其它类型的灯
         */
        // 获取其他类型灯的灯号
        int iLightIndex = GetLightIndex(strLed.c_str());
        if (iLightIndex < 0)
        {
            XMLBROWSER_INFO("Unknown light index.");
            return;
        }

        int iLampID = GetLampID(iLightIndex);
        XMLBROWSER_INFO("XMLBrowser Ctrl Light Index [%d] iLampID [%d]", iLightIndex, iLampID);
        // 根据状态参数来控制灯
        switch (GetLightStatus(strStatus.c_str()))
        {
        case LST_OFF:
            {
                ledlamp_PostCustomEvent(iLampID, -1, -1);
            }
            break;
        case LST_ON:
            {
                ledlamp_PostCustomEvent(iLampID, 0, 0);
            }
            break;
        case LST_SLOWFLASH:
        case LST_FASTFLASH:
            {
                int nTime = (LST_FASTFLASH == GetLightStatus(strStatus.c_str())) ? 300 : 1000;
                ledlamp_PostCustomEvent(iLampID, nTime, nTime);
            }
            break;
        default:
            XMLBROWSER_WARN("can't parse light status");
            break;
        }
    }
}

// 处理发送按键类型的URI
void CURIProcesser::ProcessKey(const char * szURI)
{
    if (IsURIEmpty(szURI))
    {
        return;
    }

    SendKey(szURI);
}

// 处理播放音频类型的URI
void CURIProcesser::ProcessPlayWave(const char * szURI)
{
    if (IsURIEmpty(szURI))
    {
        return;
    }
    if (!_ElementManager.LoadWav(szURI, OPT_DOWNLOAD))
    {
        XMLBROWSER_INFO("Load wave [%s] failed", szURI);
    }
}

// 处理停止播放音频
void CURIProcesser::ProcessStopWave()
{
    _XMLBrowserManager.StopWav();
}

// 处理命令类型的URI
void CURIProcesser::ProcessCommand(const char * szURI)
{
    if (IsURIEmpty(szURI))
    {
        return;
    }

    XMLBROWSER_INFO("ProcessCommand = %s", szURI);

    if (strcmp(szURI, "Reset") == 0)
    {
        if (talklogic_SessionExist())
        {
            XMLBROWSER_WARN("Ignore reset request on talking !");
            return;
        }
        // 重置
        _XMLBrowserManager.ShowMessageBox(XB_TIP_RESET, 0);
#if IF_BUG_20555
        if (acc_LogoutBeforeReset())
#endif  //IF_BUG_20555
        {
            // 恢复出厂配置
            AdapterBox_ResetFactory((int)RESET_OPTION_ALL);
        }
    }
    else if (strcmp(szURI, "Reboot") == 0)
    {
        if (talklogic_SessionExist())
        {
            XMLBROWSER_WARN("Ignore reboot request on talking !");
            return;
        }
        // 重新启动
        //_XMLBrowserManager.ShowMessageBox(XB_TIP_REBOOT, 0);
        // commonAPI_Reboot();
        // 重启执行
        //commonUnits_Reboot(0);
        AdapterBox_Reboot(0);
    }
    else if (strcmp(szURI, "Lock") == 0)
    {
        // 加锁
        if (!keyGuard_IsEnable())
        {
            keyGuard_Enable(true);
        }
        keyGuard_Lock(true);
    }
    else if (strcmp(szURI, "Unlock") == 0)
    {
        // 解锁
        keyGuard_Enable(false);
    }
    else if (strcmp(szURI, "ClearCallersList") == 0)
    {
        // 清空所有呼叫记录
        LocalLog_ClearLogs(kszAllContactId);
    }
    else if (strcmp(szURI, "ClearRedialList") == 0)
    {
        // 清空所有呼出记录
        LocalLog_ClearLogs(kszPlacedLogId);
    }
    else if (strcmp(szURI, "ClearDirectory") == 0)
    {
        // 清空电话本
        LocalDir_ClearContact(kszAllContactId);
    }
#if IF_BUG_23718
    else if (strcmp(szURI, "UploadSystemInfo") == 0)
    {
        UploadSystemInfo();
    }
#endif
}

#if IF_BUG_23718
int CURIProcesser::UploadSystemInfo()
{
    yl::string strFilePath = szUpLoadDssKeyFile;
    yl::string strServerUrl = configParser_GetConfigString(kszSystemInfoServerURL);

    // 保存Dsskey配置到文件
    if (dsskey_SaveDsskeyInfoToFile(strFilePath))
    {
        // 上传文件
        return _XMLBrowserLoadMng.UploadFile(strFilePath, strServerUrl);
    }

    return false;
}
#endif

// 检查URI是否为空
bool CURIProcesser::IsURIEmpty(const char * szURI)
{
    return (szURI == NULL || *szURI == '\0');
}

// 对扩展台按键灯的处理
void CURIProcesser::ProcessExpLed(const char * szLed, const char * szStatus)
{
    if (IsURIEmpty(szLed) || IsURIEmpty(szStatus))
    {
        return;
    }
    else
    {
        // 扩展台序号
        int iExp = 0;
        // 按键序号
        int iKey = 0;
        // 灯颜色
        char szColor[30] = "";

        // 格式判断，如：EXP-1-9-RED(Exp和key index配置中都是从1开始)
        if (sscanf(szLed, "EXP-%d-%d-%s", &iExp, &iKey, szColor) == 3)
        {
            // 判断是否合法
            DEVICE_TYPE_DEF eType = exp_GetExpTypeByIndex(iExp - 1);
            if (strlen(szColor) == 0
                    || iExp < 1 || iExp > MAX_EXP_NUM
                    || iKey < 1 || iKey > exp_GetExpKeyNumByType(eType)
                    || GetLightColor(szColor) == (LEDCOLOR_LTYE) - 1)
            {
                XMLBROWSER_WARN("error! invalid exp led index.");
                return;
            }

            // 获取颜色
            LEDCOLOR_LTYE eColor = GetLightColor(szColor);
            // 获取状态
            LEDSTATUS_TYPE eStatus = GetLightStatus(szStatus);

            // 根据灯颜色和状态获取对应的Type
            LIGHT_Type eLight = GetLightType(eColor, eStatus);

            // 将pExpLightInfo抛给Light处理
            // 代码中的index都是从0开始，配置中从1开始
            DSSKEY_MODULE_TYPE moduleType = exp_GetEXPModuleType();

            // 获取dsskeyid
            int iDsskeyID = dsskey_GetDsskeyID(moduleType, iExp - 1, iKey - 1);

#if IF_BUG_XMLBROWSER_EXP_LED
            dsskey_SetDsskeyLightType(iDsskeyID, eLight);
#else
            dsskey_ResetDsskeyStatus(iDsskeyID);
#endif
            exp_SetExpLEDStatus(iDsskeyID, eLight);
        }
    }
}

// 发送按键
void CURIProcesser::SendKey(const yl::string & strKey)
{
    // 按键字符和按键键值的对应
    struct XBKey
    {
        const char * lpszKey;
        int  iMsg;
        int iKey;
    };

    static XBKey mapKey[] =
    {
        {"OFF_HOOK",     DEV_MSG_FUNKEY_EVENT, PHONE_KEY_HANDSET_OFF_HOOK},
        {"ON_HOOK",      DEV_MSG_FUNKEY_EVENT, PHONE_KEY_HANDSET_ON_HOOK},
        {"OK",           DEV_MSG_FUNKEY_EVENT, PHONE_KEY_OK},
        {"CANCEL",       DEV_MSG_FUNKEY_EVENT, PHONE_KEY_CANCEL},
        {"UP",           DEV_MSG_FUNKEY_EVENT, PHONE_KEY_UP},
        {"DOWN",         DEV_MSG_FUNKEY_EVENT, PHONE_KEY_DOWN},
        {"LEFT",         DEV_MSG_FUNKEY_EVENT, PHONE_KEY_LEFT},
        {"RIGHT",        DEV_MSG_FUNKEY_EVENT, PHONE_KEY_RIGHT},
        {"INCREASE",     DEV_MSG_FUNKEY_EVENT, PHONE_KEY_VOLUME_INCREASE},
        {"DECREASE",     DEV_MSG_FUNKEY_EVENT, PHONE_KEY_VOLUME_DECREASE},
        {"REDIAL",       DEV_MSG_FUNKEY_EVENT, PHONE_KEY_REDIAL},
        {"HOLD",         DEV_MSG_FUNKEY_EVENT, PHONE_KEY_HOLD},
        {"MUTE",         DEV_MSG_FUNKEY_EVENT, PHONE_KEY_MUTE},
        {"CONFERENCE",   DEV_MSG_FUNKEY_EVENT, PHONE_KEY_CONFERENCE},
        {"TRANSFER",     DEV_MSG_FUNKEY_EVENT, PHONE_KEY_TRANSFER},
        {"SMS",          DEV_MSG_FUNKEY_EVENT, PHONE_KEY_MESSAGE},
        {"HEADSET",      DEV_MSG_FUNKEY_EVENT, PHONE_KEY_HEADSET_CTRL},
        {"HANDFREE",     DEV_MSG_FUNKEY_EVENT, PHONE_KEY_HANDFREE},

        {"HOTKEY1",      DEV_MSG_FUNKEY_EVENT, PHONE_KEY_FN1},
        {"HOTKEY2",      DEV_MSG_FUNKEY_EVENT, PHONE_KEY_FN2},
        {"HOTKEY3",      DEV_MSG_FUNKEY_EVENT, PHONE_KEY_FN3},
        {"HOTKEY4",      DEV_MSG_FUNKEY_EVENT, PHONE_KEY_FN4},

        {"GROUP_LISTEN", DEV_MSG_FUNKEY_EVENT, PHONE_KEY_GROUP_LISTEN},
        {"HOLD_PUBLIC",  DEV_MSG_FUNKEY_EVENT, PHONE_KEY_HOLD_PUBLIC},
        {"HOLD_PRIVATE", DEV_MSG_FUNKEY_EVENT, PHONE_KEY_HOLD_PRIVATE},

        {"KEY_0",        DEV_MSG_DIGITKEY_EVENT, PHONE_KEY_0},
        {"KEY_1",        DEV_MSG_DIGITKEY_EVENT, PHONE_KEY_1},
        {"KEY_2",        DEV_MSG_DIGITKEY_EVENT, PHONE_KEY_2},
        {"KEY_3",        DEV_MSG_DIGITKEY_EVENT, PHONE_KEY_3},
        {"KEY_4",        DEV_MSG_DIGITKEY_EVENT, PHONE_KEY_4},
        {"KEY_5",        DEV_MSG_DIGITKEY_EVENT, PHONE_KEY_5},
        {"KEY_6",        DEV_MSG_DIGITKEY_EVENT, PHONE_KEY_6},
        {"KEY_7",        DEV_MSG_DIGITKEY_EVENT, PHONE_KEY_7},
        {"KEY_8",        DEV_MSG_DIGITKEY_EVENT, PHONE_KEY_8},
        {"KEY_9",        DEV_MSG_DIGITKEY_EVENT, PHONE_KEY_9},
        {"STAR",         DEV_MSG_DIGITKEY_EVENT, PHONE_KEY_STAR},
        {"POUND",        DEV_MSG_DIGITKEY_EVENT, PHONE_KEY_POUND},

        {"",             -1, -1} // last one
    };

    // 从数组中找对应的键值
    int i = 0;
    while (mapKey[i].iKey != -1)
    {
        if (strKey == mapKey[i].lpszKey)
        {
            // 模拟按键按下
            commonUnits_SendSimulateKey(mapKey[i].iMsg, mapKey[i].iKey);
            return;
        }

        i++;
    }

    int iKey = -1;
    if (sscanf(strKey.c_str(), "LINE%d", &iKey) == 1
            && iKey > 0)
    {
        dsskey_OnDsskeyClick(iKey - 1);
    }
    else
    {
        // 如果没有找到对应的按键且不是EXP按键则不处理
        int iExp = 0;
        // 判断是否EXP按键
        if (sscanf(strKey.c_str(), "EXP-%d-%d", &iExp, &iKey) == 2)
        {
            // 从EXP-1-1开始
            if (iExp == 0 || iExp > MAX_EXP_NUM
                    || iKey <= 0)
            {
                return;
            }
            // 获取dsskeyid
            DSSKEY_MODULE_TYPE moduleType = exp_GetEXPModuleType();
            int iDsskeyID = dsskey_GetDsskeyID(moduleType, iExp - 1, iKey - 1);
            dsskey_OnDsskeyClick(iDsskeyID);
        }
    }
}


// 获取灯状态
LEDSTATUS_TYPE CURIProcesser::GetLightStatus(const char * szStatus)
{
    if (IsURIEmpty(szStatus))
    {
        return LST_NONE;
    }

    // 判断灯状态
    if (strcmp(szStatus, "off") == 0)
    {
        return LST_OFF;
    }
    else if (strcmp(szStatus, "on") == 0)
    {
        return LST_ON;
    }
    else if (strcmp(szStatus, "slowflash") == 0)
    {
        return LST_SLOWFLASH;
    }
    else if (strcmp(szStatus, "fastflash") == 0)
    {
        return LST_FASTFLASH;
    }
    else
    {
        return LST_NONE;
    }
}

// 获取灯颜色
LEDCOLOR_LTYE CURIProcesser::GetLightColor(const char * szColor)
{
    if (IsURIEmpty(szColor))
    {
        return (LEDCOLOR_LTYE) - 1;
    }

    // 判断灯颜色
    if (strcmp(szColor, "RED") == 0)
    {
        return LT_RED;
    }
    else if (strcmp(szColor, "GREEN") == 0)
    {
        return LT_GREEN;
    }
    else if (strcmp(szColor, "ORANGE") == 0)
    {
        return LT_REDGREEN;
    }
    else
    {
        return (LEDCOLOR_LTYE) - 1;
    }
}

// 获取灯类型
LIGHT_Type CURIProcesser::GetLightType(const LEDCOLOR_LTYE eColor, const LEDSTATUS_TYPE eStatus)
{
    if (eStatus == LST_OFF)
    {
        // 关灯不用管颜色
        return LT_OFF;
    }
    else if (eStatus == LST_ON)
    {
        // 开灯再根据颜色返回
        switch (eColor)
        {
        case LT_RED:
            return LT_RED_ON;
        case LT_GREEN:
            return LT_GREEN_ON;
        case LT_REDGREEN:
            return LT_RED_GREEN_ON;
        default:
            break;
        }
    }
    else if (eStatus == LST_SLOWFLASH)
    {
        // 慢闪灯再根据颜色返回
        switch (eColor)
        {
        case LT_RED:
            return LT_RED_FLASH_4;
        case LT_GREEN:
            return LT_GREEN_FLASH_4;
        case LT_REDGREEN:
            return LT_RED_GREEN_FLASH_4;
        default:
            break;
        }
    }
    else if (eStatus == LST_FASTFLASH)
    {
        // 快闪灯再根据颜色返回
        switch (eColor)
        {
        case LT_RED:
            return LT_RED_FLASH_2;
        case LT_GREEN:
            return LT_GREEN_FLASH_2;
        case LT_REDGREEN:
            return LT_RED_GREEN_FLASH_2;
        default:
            break;
        }
    }

    return LT_NONE;
}

// 获取灯键值
int CURIProcesser::GetLightIndex(const char * szLed)
{
    if (IsURIEmpty(szLed))
    {
        return -1;
    }

    // 解析到的数据和灯键值的对应
    struct LightIndex
    {
        const char * lpszLight; // 解析到的数据
        int iLightIndex;        // 灯键值
    };


    static LightIndex mapIndex[] =
    {
        {"SMS",             LAMP_SMS},
        {"HEADSET",         LAMP_HEADSET},
        {"BACKLIGHT",       LAMP_BACKLIGHT},
        {"HANDFREE",        LAMP_SPEAKER},
        {"POWER",           LAMP_POWSER},
        {"MUTE",            LAMP_MUTE},
        {"",         -1}, // last one
    };

    // 从数组中找对应的键值
    int iLightIndex = -1;
    int i = 0;
    while (mapIndex[i].iLightIndex != -1)
    {
        if (strcmp(szLed, mapIndex[i].lpszLight) == 0)
        {
            iLightIndex = mapIndex[i].iLightIndex;
            break;
        }
        i++;
    }

    return iLightIndex;
}

// 获取灯键值
bool CURIProcesser::GetDsskeyLightInfo(const char * szLed, int & iKeyIndex,
                                       LEDCOLOR_LTYE & eColorType, DSSKEY_MODULE_TYPE & dsskeyModule)
{
    if (szLed == NULL)
    {
        return false;
    }

    int iLedIndex = 0;
    char szColor[64] = "";
    yl::string strLineFarmat = "";

    strLineFarmat = "LINE%d_%s";

    if (sscanf(szLed, strLineFarmat.c_str(), &iLedIndex, szColor) == 2)
    {
        iKeyIndex = iLedIndex - 1;
        eColorType = GetLightColor(szColor);
        dsskeyModule = DMT_LINEKEY_MODULE;
    }
    else
    {
        return false;
    }

    return true;
}

#endif // #ifdef IF_FEATURE_XMLBROWSER

