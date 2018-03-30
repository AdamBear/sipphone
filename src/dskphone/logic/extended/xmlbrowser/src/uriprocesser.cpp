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
    // �ú�����static��ʽ����ȫ�ֶ���Ĺ���˳��
    static CURIProcesser s_URIProcesser;
    return s_URIProcesser;
}

// ����URI
void CURIProcesser::ProcessURI(const yl::string & strURI)
{
    if (strURI.empty() || strURI.size() > 1024)
    {
        return;
    }

    // ���ݡ�:���������ͺ�����
    //sscanf(strURI.c_str(), "%[^:]:%s", szType, szContent);
    // ���ݴ��ڿո���ݴ���
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

    // ����http(s)���͵�URI
    if (strcmp(strType.c_str(), "http") == 0 || strcmp(strType.c_str(), "https") == 0)
    {
        yl::string strUrl = strURI;

        strUrl.trim_both(" \r\n\t"); //����URLͷβ�쳣����

        char szSuffix[1024] = { 0 };
        char szServer[128] = { 0 };
        if (sscanf(strContent.c_str(), "//%[^/]%s", szServer, szSuffix) == 2)
        {
            strUrl = szSuffix;
            // �Ƚ��б���ת��
            commonUnits_EncodeURI(strUrl);

            strUrl = commonAPI_FormatString("%s://%s%s", strType.c_str(), szServer, strUrl.c_str());
            XMLBROWSER_INFO("After EnCode URL is: %s", strUrl.c_str());
        }
        ProcessHttp(strUrl.c_str());
    }
    // �����ź������͵�URI
    else if (strcmp(strType.c_str(), "Dial") == 0)
    {
        ProcessDial(strContent.c_str());
    }
    // ����ƿ������͵�URI
    else if (strcmp(strType.c_str(), "Led") == 0)
    {
        // ��ͣ��ʾ�������������ĵ��ܵ�Ӱ�죬���������
        _XMLBrowserManager.StopBeep();
        ProcessLed(strContent.c_str());
    }
    // �����Ͱ������͵�URI
    else if (strcmp(strType.c_str(), "Key") == 0)
    {
        // ��ͣ��ʾ�������������İ�����Ӧ�ܵ�Ӱ��
        _XMLBrowserManager.StopBeep();
        ProcessKey(strContent.c_str());
    }
    // ��������Ƶ���͵�URI
    else if (strcmp(strType.c_str(), "Wav.Play") == 0)
    {
        ProcessPlayWave(strContent.c_str());
    }
    // ����ֹͣ������Ƶ���͵�URI
    else if (strcmp(strType.c_str(), "Wav.Stop") == 0)
    {
        ProcessStopWave();
    }
    // �����������͵�URI
    else if (strcmp(strType.c_str(), "Command") == 0)
    {
        ProcessCommand(strContent.c_str());
    }
}

// ����http(s)���͵�URI
void CURIProcesser::ProcessHttp(const char * szURI)
{
    if (IsURIEmpty(szURI))
    {
        return;
    }

    // ������һ��URL�Ҳ��˳����е�Element
    _XMLBrowserManager.Enter(szURI, false);
}

// �����ź������͵�URI
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

// ��ȡ��ͬ���͵ĵƺ�
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

// ����ƿ������͵�URI
void CURIProcesser::ProcessLed(const char * szURI)
{
    if (IsURIEmpty(szURI) || strlen(szURI) > 49)
    {
        return;
    }

    yl::string strURI, strLed, strStatus;
    strURI = szURI;

    // ��ʽ�ж� [XXXX=off/on/slowflash/fastflash]
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
    // �ж��Ƿ�Ϊ��չ̨
    if (strncmp(strLed.c_str(), "EXP", 3) == 0)
    {
        // ��չ̨�ƴ���
        ProcessExpLed(strLed.c_str(), strStatus.c_str());
    }
    else
    {
        /*
         * Dsskey ��
         */
        // �����Ƶ���˸���ͣ������أ���������
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
         * �������͵ĵ�
         */
        // ��ȡ�������͵Ƶĵƺ�
        int iLightIndex = GetLightIndex(strLed.c_str());
        if (iLightIndex < 0)
        {
            XMLBROWSER_INFO("Unknown light index.");
            return;
        }

        int iLampID = GetLampID(iLightIndex);
        XMLBROWSER_INFO("XMLBrowser Ctrl Light Index [%d] iLampID [%d]", iLightIndex, iLampID);
        // ����״̬���������Ƶ�
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

// �����Ͱ������͵�URI
void CURIProcesser::ProcessKey(const char * szURI)
{
    if (IsURIEmpty(szURI))
    {
        return;
    }

    SendKey(szURI);
}

// ��������Ƶ���͵�URI
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

// ����ֹͣ������Ƶ
void CURIProcesser::ProcessStopWave()
{
    _XMLBrowserManager.StopWav();
}

// �����������͵�URI
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
        // ����
        _XMLBrowserManager.ShowMessageBox(XB_TIP_RESET, 0);
#if IF_BUG_20555
        if (acc_LogoutBeforeReset())
#endif  //IF_BUG_20555
        {
            // �ָ���������
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
        // ��������
        //_XMLBrowserManager.ShowMessageBox(XB_TIP_REBOOT, 0);
        // commonAPI_Reboot();
        // ����ִ��
        //commonUnits_Reboot(0);
        AdapterBox_Reboot(0);
    }
    else if (strcmp(szURI, "Lock") == 0)
    {
        // ����
        if (!keyGuard_IsEnable())
        {
            keyGuard_Enable(true);
        }
        keyGuard_Lock(true);
    }
    else if (strcmp(szURI, "Unlock") == 0)
    {
        // ����
        keyGuard_Enable(false);
    }
    else if (strcmp(szURI, "ClearCallersList") == 0)
    {
        // ������к��м�¼
        LocalLog_ClearLogs(kszAllContactId);
    }
    else if (strcmp(szURI, "ClearRedialList") == 0)
    {
        // ������к�����¼
        LocalLog_ClearLogs(kszPlacedLogId);
    }
    else if (strcmp(szURI, "ClearDirectory") == 0)
    {
        // ��յ绰��
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

    // ����Dsskey���õ��ļ�
    if (dsskey_SaveDsskeyInfoToFile(strFilePath))
    {
        // �ϴ��ļ�
        return _XMLBrowserLoadMng.UploadFile(strFilePath, strServerUrl);
    }

    return false;
}
#endif

// ���URI�Ƿ�Ϊ��
bool CURIProcesser::IsURIEmpty(const char * szURI)
{
    return (szURI == NULL || *szURI == '\0');
}

// ����չ̨�����ƵĴ���
void CURIProcesser::ProcessExpLed(const char * szLed, const char * szStatus)
{
    if (IsURIEmpty(szLed) || IsURIEmpty(szStatus))
    {
        return;
    }
    else
    {
        // ��չ̨���
        int iExp = 0;
        // �������
        int iKey = 0;
        // ����ɫ
        char szColor[30] = "";

        // ��ʽ�жϣ��磺EXP-1-9-RED(Exp��key index�����ж��Ǵ�1��ʼ)
        if (sscanf(szLed, "EXP-%d-%d-%s", &iExp, &iKey, szColor) == 3)
        {
            // �ж��Ƿ�Ϸ�
            DEVICE_TYPE_DEF eType = exp_GetExpTypeByIndex(iExp - 1);
            if (strlen(szColor) == 0
                    || iExp < 1 || iExp > MAX_EXP_NUM
                    || iKey < 1 || iKey > exp_GetExpKeyNumByType(eType)
                    || GetLightColor(szColor) == (LEDCOLOR_LTYE) - 1)
            {
                XMLBROWSER_WARN("error! invalid exp led index.");
                return;
            }

            // ��ȡ��ɫ
            LEDCOLOR_LTYE eColor = GetLightColor(szColor);
            // ��ȡ״̬
            LEDSTATUS_TYPE eStatus = GetLightStatus(szStatus);

            // ���ݵ���ɫ��״̬��ȡ��Ӧ��Type
            LIGHT_Type eLight = GetLightType(eColor, eStatus);

            // ��pExpLightInfo�׸�Light����
            // �����е�index���Ǵ�0��ʼ�������д�1��ʼ
            DSSKEY_MODULE_TYPE moduleType = exp_GetEXPModuleType();

            // ��ȡdsskeyid
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

// ���Ͱ���
void CURIProcesser::SendKey(const yl::string & strKey)
{
    // �����ַ��Ͱ�����ֵ�Ķ�Ӧ
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

    // ���������Ҷ�Ӧ�ļ�ֵ
    int i = 0;
    while (mapKey[i].iKey != -1)
    {
        if (strKey == mapKey[i].lpszKey)
        {
            // ģ�ⰴ������
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
        // ���û���ҵ���Ӧ�İ����Ҳ���EXP�����򲻴���
        int iExp = 0;
        // �ж��Ƿ�EXP����
        if (sscanf(strKey.c_str(), "EXP-%d-%d", &iExp, &iKey) == 2)
        {
            // ��EXP-1-1��ʼ
            if (iExp == 0 || iExp > MAX_EXP_NUM
                    || iKey <= 0)
            {
                return;
            }
            // ��ȡdsskeyid
            DSSKEY_MODULE_TYPE moduleType = exp_GetEXPModuleType();
            int iDsskeyID = dsskey_GetDsskeyID(moduleType, iExp - 1, iKey - 1);
            dsskey_OnDsskeyClick(iDsskeyID);
        }
    }
}


// ��ȡ��״̬
LEDSTATUS_TYPE CURIProcesser::GetLightStatus(const char * szStatus)
{
    if (IsURIEmpty(szStatus))
    {
        return LST_NONE;
    }

    // �жϵ�״̬
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

// ��ȡ����ɫ
LEDCOLOR_LTYE CURIProcesser::GetLightColor(const char * szColor)
{
    if (IsURIEmpty(szColor))
    {
        return (LEDCOLOR_LTYE) - 1;
    }

    // �жϵ���ɫ
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

// ��ȡ������
LIGHT_Type CURIProcesser::GetLightType(const LEDCOLOR_LTYE eColor, const LEDSTATUS_TYPE eStatus)
{
    if (eStatus == LST_OFF)
    {
        // �صƲ��ù���ɫ
        return LT_OFF;
    }
    else if (eStatus == LST_ON)
    {
        // �����ٸ�����ɫ����
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
        // �������ٸ�����ɫ����
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
        // �������ٸ�����ɫ����
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

// ��ȡ�Ƽ�ֵ
int CURIProcesser::GetLightIndex(const char * szLed)
{
    if (IsURIEmpty(szLed))
    {
        return -1;
    }

    // �����������ݺ͵Ƽ�ֵ�Ķ�Ӧ
    struct LightIndex
    {
        const char * lpszLight; // ������������
        int iLightIndex;        // �Ƽ�ֵ
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

    // ���������Ҷ�Ӧ�ļ�ֵ
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

// ��ȡ�Ƽ�ֵ
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

