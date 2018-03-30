#include "test_include.h"
#include "testphonet66.h"
#include "testmodeinterface.h"
#include "languagehelper/modlangtranslate.h"

#define  LIGHT_LOOP_TIME            1100
#define  ECHO_LIGHT_LOOP_TIME       1000

/////////////////////////////CTestPhoneT22/////////////////////////////////
CTestPhoneT66::CTestPhoneT66()
{
    m_uMaxKeyNum = 41;
    m_iFlashIndex = 0;
    m_eLightType = LS_GREEN;

    m_iLineNum = T66_LINEKEY_NUM;
    m_iLightTimer = LIGHT_LOOP_TIME;

    m_bHasEXP = true;
    m_bSkipEHSTest = true;
}

CTestPhoneT66::~CTestPhoneT66()
{
    m_timerLight.KillTimer();
    m_timerEchoLight.KillTimer();
}

void CTestPhoneT66::InitLight()
{
#ifdef WIN32
#else
    m_objIOMutex.Lock();

    m_objIOMutex.Unlock();
    //0x01是电源灯, 0x20是背光灯，保持常亮
    devicelib_SetLedStatus(m_iLedStatus);

    // 电源灯可控之后,先点亮电源灯
    LightOn(T23_LIGHT_POWER);
#endif

    FlushLightIndex();
    StartLightTest();
}

void CTestPhoneT66::FlushLightIndex()
{
    m_vecLightIndex.push_back(T66_LIGHT_LK1_GREEN);
    m_vecLightIndex.push_back(T66_LIGHT_LK2_GREEN);
    m_vecLightIndex.push_back(T66_LIGHT_LK3_GREEN);
    m_vecLightIndex.push_back(T66_LIGHT_LK4_GREEN);
    m_vecLightIndex.push_back(T66_LIGHT_LK5_GREEN);
    m_vecLightIndex.push_back(T66_LIGHT_LK6_GREEN);
    m_vecLightIndex.push_back(T66_LIGHT_LK7_GREEN);
    m_vecLightIndex.push_back(T66_LIGHT_LK8_GREEN);

    m_vecLightIndex.push_back(T66_LIGHT_LK1_RED);
    m_vecLightIndex.push_back(T66_LIGHT_LK2_RED);
    m_vecLightIndex.push_back(T66_LIGHT_LK3_RED);
    m_vecLightIndex.push_back(T66_LIGHT_LK4_RED);
    m_vecLightIndex.push_back(T66_LIGHT_LK5_RED);
    m_vecLightIndex.push_back(T66_LIGHT_LK6_RED);
    m_vecLightIndex.push_back(T66_LIGHT_LK7_RED);
    m_vecLightIndex.push_back(T66_LIGHT_LK8_RED);

    m_vecLightIndex.push_back(T66_LIGHT_LK1_GREEN | T66_LIGHT_LK1_RED);
    m_vecLightIndex.push_back(T66_LIGHT_LK2_GREEN | T66_LIGHT_LK2_RED);
    m_vecLightIndex.push_back(T66_LIGHT_LK3_GREEN | T66_LIGHT_LK3_RED);
    m_vecLightIndex.push_back(T66_LIGHT_LK4_GREEN | T66_LIGHT_LK4_RED);
    m_vecLightIndex.push_back(T66_LIGHT_LK5_GREEN | T66_LIGHT_LK5_RED);
    m_vecLightIndex.push_back(T66_LIGHT_LK6_GREEN | T66_LIGHT_LK6_RED);
    m_vecLightIndex.push_back(T66_LIGHT_LK7_GREEN | T66_LIGHT_LK7_RED);
    m_vecLightIndex.push_back(T66_LIGHT_LK8_GREEN | T66_LIGHT_LK8_RED);

    m_vecLightIndex.push_back(T66_LIGHT_MSG);
    m_vecLightIndex.push_back(T66_LIGHT_HEADSET);
    m_vecLightIndex.push_back(T66_LIGHT_MUTE);
    m_vecLightIndex.push_back(T66_LIGHT_SPK);
}

void CTestPhoneT66::InitCmpString()
{
    int iKeyIndex = 0;
    int i = 0;
    // 数字1~9
    for (i = 0; i < 9; ++i)
    {
        m_strKey[iKeyIndex++] = commonAPI_FormatString("%d", i + 1);
    }
    m_strKey[iKeyIndex++] = "*";
    m_strKey[iKeyIndex++] = "0";
    m_strKey[iKeyIndex++] = "#";

    //LineKey
    for (i = 0; i < m_iLineNum; ++i)
    {
        m_strKey[iKeyIndex++] = commonAPI_FormatString("LK%d", i + 1);
    }

    // 4个Hotkey
    for (i = 0; i < 4; ++i)
    {
        m_strKey[iKeyIndex++] = commonAPI_FormatString("HK%d", i + 1);
    }

    //音量键
    m_strKey[iKeyIndex++] = "Vol+";
    m_strKey[iKeyIndex++] = "Vol-";
    //导航键
    m_strKey[iKeyIndex++] = "Up";
    m_strKey[iKeyIndex++] = "Dn";
    m_strKey[iKeyIndex++] = "Lf";
    m_strKey[iKeyIndex++] = "Rt";
    m_strKey[iKeyIndex++] = "OK";
    //其他功能键
    m_strKey[iKeyIndex++] = "Menu";
#if IF_BUG_27040
    m_strKey[iKeyIndex++] = "Msg";
    m_strKey[iKeyIndex++] = "DND";
#else
    m_strKey[iKeyIndex++] = "Cancel";
    m_strKey[iKeyIndex++] = "Msg";
#endif
    m_strKey[iKeyIndex++] = "Headset";
    m_strKey[iKeyIndex++] = "Tran";
    m_strKey[iKeyIndex++] = "Hold";
    m_strKey[iKeyIndex++] = "RD";
    m_strKey[iKeyIndex++] = "Mute";
    m_strKey[iKeyIndex++] = "HandFree";

    m_strKey[iKeyIndex++] = "AnyEXPKey";
}

TEST_STATE CTestPhoneT66::MapStateOfKey(int iKey, bool bEXPKey /*= false*/)
{
    if (bEXPKey)
    {
        int iEXPIndex = GetEXPIndex(iKey);
        int iEXPKey = GetEXPKeyIndex(iKey);

        TESTMODE_INFO("EXP Key Info[%d][%d]", iEXPIndex, iEXPKey);

        if (EXP_INSERT == iEXPKey)
        {
            if (1 == iEXPIndex)
            {
                return TS_EXP_LIGHT;
            }
            // 有可能扩展1先识别
            else if (0 == iEXPIndex)
            {
                if (exp_GetExpTypeByIndex(0) == exp_GetExpTypeByIndex(1))
                {
                    return TS_EXP_LIGHT;
                }
            }
        }
        else if (EXP_REMOVE == iEXPKey)
        {
            if (0 == m_uEXPNum)
            {
                return TS_EXP_EXIT;
            }
        }
    }
    else
    {
        switch (iKey)
        {
        case PHONE_KEY_HANDSET_ON_HOOK:
            return TS_EXIT;
        case PHONE_KEY_HANDFREE:
            return TS_RANDNUM;
        case PHONE_KEY_OK:
            return TS_LCD;
        case PHONE_KEY_MESSAGE:
            return TS_ECHO;
#if JOYCE_TEL_BUGFREE_19348
        case PHONE_KEY_REDIAL:
            return TS_SN;
#endif
        default:
            return TS_NONE;
        }
    }

    return TS_NONE;
}

void CTestPhoneT66::LightOn(int iIndex)
{
    if (-1 == iIndex)
    {
        return;
    }

    m_iLedStatus = m_iLedStatus | iIndex;
    devicelib_SetLedStatus(m_iLedStatus);
}

void CTestPhoneT66::LightOff(int iIndex)
{
    if (-1 == iIndex)
    {
        return;
    }

    m_iLedStatus = m_iLedStatus & (~iIndex);
    devicelib_SetLedStatus(m_iLedStatus);
}

void CTestPhoneT66::StartLightTest()
{
    m_timerLight.SetTimer(m_iLightTimer, TIMER_METHOD(this, CTestPhoneT66::OnLightTimer));
}

void CTestPhoneT66::TestLCDByStep(LCD_STEP eStep)
{
    xColor colBack = xColor(0x00, 0x00, 0x00);
    xColor colWhite = xColor(0xff, 0xff, 0xff);

    testmode_FillRect(AT_ALL, colWhite);

    switch (eStep)
    {
    case LS_STEP1:
        {
            testmode_FillRect(AT_FIRST_OF_FOUR, colWhite);
            testmode_FillRect(AT_SECOND_OF_FOUR, colWhite);
            testmode_FillRect(AT_THIRD_OF_FOUR, colBack);
            testmode_FillRect(AT_FOURTH_OF_FOUR, colBack);
        }
        break;
    case LS_STEP2:
        {
            testmode_FillRect(AT_FIRST_OF_FOUR, colBack);
            testmode_FillRect(AT_SECOND_OF_FOUR, colWhite);
            testmode_FillRect(AT_THIRD_OF_FOUR, colWhite);
            testmode_FillRect(AT_FOURTH_OF_FOUR, colBack);
        }
        break;
    case LS_STEP3:
        {
            testmode_FillRect(AT_FIRST_OF_FOUR, colBack);
            testmode_FillRect(AT_SECOND_OF_FOUR, colBack);
            testmode_FillRect(AT_THIRD_OF_FOUR, colWhite);
            testmode_FillRect(AT_FOURTH_OF_FOUR, colWhite);
        }
        break;
    case LS_STEP4:
        {
            testmode_FillRect(AT_FIRST_OF_FOUR, colWhite);
            testmode_FillRect(AT_SECOND_OF_FOUR, colBack);
            testmode_FillRect(AT_THIRD_OF_FOUR, colBack);
            testmode_FillRect(AT_FOURTH_OF_FOUR, colWhite);
        }
        break;
    default:
        break;
    }
}

void CTestPhoneT66::DrawKeyString(bool bCorrect, int iIndex)
{
    // 全机型都显示字眼KeyBoard Test
    testmode_DrawText(_LANG(TRID_KEYBOARD_TEST), TT_TITLE, TEST_FONT_12);

    if (iIndex < 0 || iIndex >= (int)m_uMaxKeyNum)
    {
        return;
    }

    if (bCorrect)
    {
        testmode_DrawText(GetCorrectString(iIndex), TT_CONTENT_NORMAL, TEST_FONT_10);
    }
    else
    {
        yl::string strDisp = commonAPI_FormatString("Press %s", m_strKey[iIndex].c_str());
        testmode_DrawText(strDisp);
    }
}

bool CTestPhoneT66::ChangeEchoState(int iKey, ECHO_MODE & eMode)
{
    ECHO_MODE eOldMode = eMode;
    if (CTestPhone::ChangeEchoState(iKey, eMode))
    {
        if (EM_HEADSET != eOldMode
                && EM_HEADSET == eMode)
        {
            // 进入耳麦回音测试，关闭免提灯
            LightOff(T66_LIGHT_SPK);
        }
        else if (EM_HEADSET == eOldMode
                 && EM_HANDSET == eMode)
        {
            // 进入手柄回音测试，关闭耳麦
            LightOff(T66_LIGHT_HEADSET);
        }

        return true;
    }

    return false;
}

void CTestPhoneT66::DrawEchoString(ECHO_MODE eMode)
{
    yl::string strEchoMode = "";
    switch (eMode)
    {
    case EM_SPEAKER_RECORD:
        {
            strEchoMode = "Handfree Speaker";
        }
        break;
    case EM_SPEAKER_PLAY:
        {
            strEchoMode = "Handfree Play";
        }
        break;
    case EM_HEADSET:
        {
            strEchoMode = "HeadSet Echo Test";
        }
        break;
    case EM_HANDSET:
        {
            strEchoMode = "HandSet Echo Test";
        }
        break;
    // 整机音频测试由VPM兼容，此处切换不改变文字
    case EM_AUDIO_HANDFREE_MIC:
    case EM_AUDIO_HANDFREE_SPEAKER:
        {
            strEchoMode = "Audio Test";
        }
        break;
    case EM_EHS:
        {
            strEchoMode = "EHS Echo Test";
        }
        break;
    case EM_LED:
        {
            strEchoMode = "Test Led!";
        }
        break;
    default:
        break;
    }

    testmode_DrawText(strEchoMode, TT_CONTENT_CENTER, GetNoteTextFont(TT_CONTENT_CENTER));
}

void CTestPhoneT66::StartEchoLightTest()
{
    //点亮所有LineKey红灯
    AllLightOn(T66_LINEKEY_RED);
    if (m_pTestEXP != NULL)
    {
        m_pTestEXP->LightOnAllEXPByType(LT_RED_ON);
    }

    m_timerEchoLight.SetTimer(ECHO_LIGHT_LOOP_TIME, TIMER_METHOD(this, CTestPhoneT66::OnEchoLightTimer));
}

void CTestPhoneT66::StartEchoTest(ECHO_MODE eMode, UINT iExtData/* = 0*/)
{
    switch (eMode)
    {
    case EM_ECHO_START:
        {
            // 先灭掉所有灯
            AllLightOff();
            // 点亮红灯并按红绿橙循环亮灯
            StartEchoLightTest();

            if (IsEHSSInserted())
            {
                CTestPhone::StartEchoTest(EM_EHS);
            }
            else
            {
                ipvp_echo_test(mkGetHandle(), IPVP_ECHO_RECORD);
                //进入回音测试前,先设置vpm音量,为的是,在talk起来后设置音量,而此时,vpm未起来,会导致设置不成功,此处再设置一次
                int iDefaultVol = MID_VOLUME;
                ipvp_set_mode(mkGetHandle(), IPVP_MODE_HS);
                ipvp_set_volume(mkGetHandle(), iDefaultVol);

                ipvp_set_mode(mkGetHandle(), IPVP_MODE_HF);
                ipvp_set_volume(mkGetHandle(), iDefaultVol);
            }
        }
        break;
    default:
        CTestPhone::StartEchoTest(eMode);
        break;
    }
}

void CTestPhoneT66::AllLightOff()
{
    m_timerLight.KillTimer();

    TurnLightAllOff();

    DoLight();
}

void CTestPhoneT66::AllLightOn()
{
    m_timerLight.KillTimer();

    AllLightOn(T66_LINEKEY_GREEN);
}

void CTestPhoneT66::AllLightOn(int mLineColor, bool bFuncKey)
{
    TurnLightAllOff(bFuncKey);

    for (int i = 0; i < m_iLineNum; ++i)
    {
        TurnLightOn(m_vecLightIndex[m_iLineNum * mLineColor + i]);
    }

    if (bFuncKey)
    {
        for (int i = T66_FUNCTION_LIGHT_BEGIN; i < m_vecLightIndex.size(); ++i)
        {
            //点亮所有功能键
            TurnLightOn(m_vecLightIndex[i]);
        }
    }

    DoLight();
}

void CTestPhoneT66::TurnLightAllOff(bool bFuncKey)
{
    //电源灯常亮
    if (bFuncKey)
    {
        m_iLedStatus = T66_LIGHT_POWER;
    }
    else
    {
        for (int i = 0; i < T66_FUNCTION_LIGHT_BEGIN && i < m_vecLightIndex.size(); ++i)
        {
            TurnLightOff(m_vecLightIndex[i]);
        }
    }
}

void CTestPhoneT66::TurnLightOn(int iIndex)
{
    if (-1 == iIndex)
    {
        return;
    }

    m_iLedStatus = m_iLedStatus | iIndex;
}

void CTestPhoneT66::TurnLightOff(int iIndex)
{
    if (-1 == iIndex)
    {
        return;
    }

    m_iLedStatus = m_iLedStatus & (~iIndex);
}

void CTestPhoneT66::DoLight()
{
    devicelib_SetLedStatus(m_iLedStatus);
}

void CTestPhoneT66::DrawRpsString(bool bRpsStatus, LPCTSTR lpszRandnum, LPCSTR lpszCert)
{
    testmode_DrawText(bRpsStatus ? TRID_RPS_OPEN : TRID_RPS_CLOSE);
    yl::string strDisplay = commonAPI_FormatString("%s(%s)", lpszRandnum, lpszCert);
    testmode_DrawText(strDisplay, TT_CONTENT_SECOND_HALF, TEST_FONT_10);
}

void CTestPhoneT66::DrawStatusViewString(LPCSTR lpszTest)
{
    testmode_DrawText(lpszTest, TT_CONTENT_CENTER, TEST_FONT_12);
}

void CTestPhoneT66::DrawEXPLightString(LIGHT_STEP eStep)
{
    yl::string strLightString = "";
    switch (eStep)
    {
    case LS_GREEN:
        {
            yl::string strHwver;
            yl::string strSwver;
            GetEXPInfo(strHwver, strSwver);

            strHwver = "Hardware: " + strHwver;
            strSwver = "Firmware: " + strSwver;
            yl::string strDisp = "\n" + strHwver + "\n\n" + strSwver;
            testmode_SetTextColor(TT_CONTENT_SECOND_HALF, xColor(0x00, 0x00, 0x00));
            testmode_DrawText(strDisp, TT_CONTENT_SECOND_HALF, GetNoteTextFont(TT_CONTENT_SECOND_HALF));

            strLightString = "Green Light Test";
        }
        break;
    case LS_RED:
        {
            testmode_DrawText("", TT_CONTENT_SECOND_HALF);
            strLightString = "Red Light Test";
        }
        break;
    case LS_ORANGEONE:
        {
            strLightString = "Orange Light Test";
        }
        break;
    default:
        break;
    }

    testmode_DrawText(strLightString, TT_CONTENT_CENTER, GetNoteTextFont(TT_CONTENT_CENTER));
}

void CTestPhoneT66::DrawEXPKeyString(bool bCorrect, int iIndex)
{
    if (NULL == m_pTestEXP)
    {
        return;
    }

    if (iIndex < 0 || iIndex > (int)m_pTestEXP->GetKeyNum())
    {
        return;
    }

    yl::string strKeyString;
    if (bCorrect)
    {
        strKeyString = m_pTestEXP->GetEXPKeyString(iIndex);
        testmode_DrawText(strKeyString, TT_CONTENT_FULL_SCREEN, GetNoteTextFont(TT_CONTENT_FULL_SCREEN));
    }
    else
    {
        strKeyString = commonAPI_FormatString("Press %d", iIndex + 1);
        testmode_DrawText(strKeyString, TT_CONTENT_CENTER, GetNoteTextFont(TT_CONTENT_CENTER));
    }
}

void CTestPhoneT66::DrawEXPTestString(LPCSTR lpszTest)
{
    yl::string strTest = lpszTest;
    testmode_DrawText(strTest, TT_CONTENT_CENTER, GetNoteTextFont(TT_CONTENT_CENTER));
}

int CTestPhoneT66::GetNoteTextFont(int nTextType)
{
    return TEST_FONT_20;
}

void CTestPhoneT66::LightLinekeyLed(LIGHT_STEP eLightSetp)
{
    // LineKey
    TestAllLightOn(eLightSetp, true);
}

void CTestPhoneT66::OnLightTimer(xThreadTimer* timer, LPVOID pExtraData)
{
    OnFlushLight();
}

void CTestPhoneT66::OnEchoLightTimer(xThreadTimer* timer, LPVOID pExtraData)
{
    switch (m_eLightType)
    {
    case LS_GREEN:
        {
            AllLightOn(T66_LINEKEY_GREEN, false);

            if (m_pTestEXP != NULL)
            {
                m_pTestEXP->LightOnAllEXPByType(LT_GREEN_ON);
            }
            m_eLightType = LS_ORANGEONE;
        }
        break;
    case LS_RED:
        {
            AllLightOn(T66_LINEKEY_RED, false);

            if (m_pTestEXP != NULL)
            {
                m_pTestEXP->LightOnAllEXPByType(LT_RED_ON);
            }
            m_eLightType = LS_GREEN;
        }
        break;
    case LS_ORANGEONE:
        {
            AllLightOn(T66_LINEKEY_ORANGE, false);

            if (m_pTestEXP != NULL)
            {
                m_pTestEXP->LightOnAllEXPByType(LT_RED_GREEN_ON);
            }
            m_eLightType = LS_RED;
        }
        break;
    default:
        break;
    }
}

void CTestPhoneT66::OnFlushLight()
{
    //超范围置0
    if (m_iFlashIndex >= m_vecLightIndex.size())
    {
        m_iFlashIndex = 0;
    }
    //跳过Line的红灯和橙灯
    else if (m_iFlashIndex == m_iLineNum)
    {
        m_iFlashIndex = T66_FUNCTION_LIGHT_BEGIN;
    }


    //先灭掉所有灯
    TurnLightAllOff();

    //点亮某个灯
    LightOn(m_vecLightIndex[m_iFlashIndex]);

    ++m_iFlashIndex;
}


void CTestPhoneT66::TestAllLightOn(int mLineColor, bool bFuncKey)
{
    TurnLightAllOff(true);

    if (mLineColor >= LS_GREEN && mLineColor <= LS_ORANGEONE)
    {
        for (int i = 0; i < m_iLineNum; i++)
        {
            TurnLightOn(m_vecLightIndex[m_iLineNum * mLineColor + i]);
        }
    }

    DoLight();
}

// 获取exp的信息
void CTestPhoneT66::GetEXPInfo(yl::string& strHardware, yl::string& strFirmware)
{
    char hwver[128] = { 0 };
    char swver[128] = { 0 };
    int lcdopen = exp_GetExpHandle();

    if (DT_EXP_50 == m_eEXPType)
    {
        struct expusb_hwver usb_hwver;
        struct expusb_swver usb_swver;
        memset(&usb_hwver, 0, sizeof(usb_hwver));
        memset(&usb_swver, 0, sizeof(usb_swver));
        usb_hwver.port = 0;
        usb_swver.port = 0;
        expusb_helper_hwver(lcdopen, &usb_hwver);
        expusb_helper_appver(lcdopen, &usb_swver);

        sprintf(hwver, "%d.%d.%d.%d.%d.%d.%d.%d",
                usb_hwver.version[7], usb_hwver.version[6],
                usb_hwver.version[5], usb_hwver.version[4],
                usb_hwver.version[3], usb_hwver.version[2],
                usb_hwver.version[1], usb_hwver.version[0]);

        sprintf(swver, "%d.%d.%d.%d",
                usb_swver.version[3], usb_swver.version[2],
                usb_swver.version[1], usb_swver.version[0]);
    }
    else
    {
        exp_helper_get_hwversion(lcdopen, 0, 127, hwver);
        exp_helper_get_swversion(lcdopen, 0, 127, swver);
    }

    strHardware = hwver;
    strFirmware = swver;
}

