#include "testphonecp.h"

#ifdef _CP
#include "talk/linedevice/include/modlinedevice.h"
#ifdef IF_USB_SUPPORT
#include "record/include/recordmessagedefine.h"
#endif
#ifdef _CP920
#include "keypad/keypad_api.h"
#include "phonedev/cp920keypad.h"
#include "phonedev/modphonedev.h"
#include "testmodeinterface.h"
#include "baseui/include/baseuicommon.h"
#endif

#define LED_GREEN 37
#define LED_RED 26
#define LED_OFF 0
#define CP_EXIT_KEY_TIME            2 * 1000
#define CP_LIGHT_LOOP_TIME          1200
#ifdef _CP920
#define KB_AUTO_TEST_CFG_FILE       "/phone/resource/testmode/keytest.cfg"
#define KB_CFG_SECTION_GENERAL      "general"
#define KB_CFG_GENERAL_RAWDATA_TOLE "std_rawdata_tole"
#define KB_CFG_GENERAL_DELTA_TOLE   "std_delta_tole"
#define KB_CFG_GENERAL_DELTA_BASE   "std_delta_base"
#define KB_CFG_SECTION_KEY          "KEY"
#define KB_CFG_KEY_RAWDATA          "std_rawdata"
#define KB_CFG_KEY_DELTA            "std_delta"
#define KEYMAP_CONFIG_PATH          "keymap.xml"
#endif

/////////////////////////////CTestPhoneCP/////////////////////////////////
CTestPhoneCP::CTestPhoneCP()
{
#ifdef _CP920
    m_uMaxKeyNum = 27;
#else
    m_uMaxKeyNum = 25;
#endif
    m_iLineNum = 0;
    m_bHasEXP = false;

    m_iLightTimer = CP_LIGHT_LOOP_TIME;
    m_iLedStatus = LED_OFF;
#ifdef _CP920
    m_nRoleDataTole = 0;
    m_nDeltaTole = 0;
    m_nDeltaBase = 0;
#endif
    m_PCServer.Init();
}

CTestPhoneCP::~CTestPhoneCP()
{
#ifdef _CP920
    m_mapKeyTestData.clear();
#endif
}

void CTestPhoneCP::TestFullVoiceInit()
{
#ifdef IF_SUPPORT_LINE_DEVICE
    ChangeState(new CTestStateCPAudio(this));
#endif
}

void CTestPhoneCP::TestPreburningInit()
{
#ifdef _CP
    SetEchoMode(EM_ECHO_MIC);
    ChangeState(new CTestStateCPEcho(this));
#endif
}

void CTestPhoneCP::InitLight()
{
    // 全部点亮闪灯
    ChangeAllLight(LED_STATE_FLASH);
}

void CTestPhoneCP::InitCmpString()
{
    int iIndex = 0;
    m_strKey[iIndex++] = "Dial";
#ifdef _CP920
    m_strKey[iIndex++] = "Bluetooh";
    m_strKey[iIndex++] = "OK";
#else
    m_strKey[iIndex++] = "RD";
    m_strKey[iIndex++] = "Mute";
#endif

    for (int i = 0; i < 9; ++i)
    {
        m_strKey[iIndex++] = commonAPI_FormatString("%d", i + 1);
    }

    m_strKey[iIndex++] = "*";
    m_strKey[iIndex++] = "0";
    m_strKey[iIndex++] = "#";
    m_strKey[iIndex++] = "Vol-";
#ifdef _CP920
    m_strKey[iIndex++] = "Mid Mute";
#endif
    m_strKey[iIndex++] = "Vol+";

    for (int i = 0; i < 4; ++i)
    {
        m_strKey[iIndex++] = commonAPI_FormatString("HK%d", i + 1);
    }

    m_strKey[iIndex++] = "Handup";
    m_strKey[iIndex++] = "Up";
#ifndef _CP920
    m_strKey[iIndex++] = "OK";
#endif
    m_strKey[iIndex++] = "Dn";

#ifdef _CP920
    m_strKey[iIndex++] = "Left Mute";
    m_strKey[iIndex++] = "Right Mute";
#endif
}

TEST_STATE CTestPhoneCP::MapStateOfKey(int iKey, bool bEXPKey /*= false*/)
{
    TEST_STATE eStatus = TS_NONE;

    if (bEXPKey)
    {
        return eStatus;
    }

    switch (iKey)
    {
#if JOYCE_TEL_BUGFREE_19348
    case PHONE_KEY_REDIAL:
        {
            eStatus = TS_SN;
        }
        break;
#endif
    case PHONE_KEY_STAR:
        {
            eStatus = TS_ECHO;
        }
        break;
    case PHONE_KEY_MUTE:
        {
            eStatus = TS_ECHO;
        }
        break;
    case PHONE_KEY_POUND:
        {
            eStatus = TS_ECHO;
        }
        break;
    case PHONE_KEY_OK:
        {
            eStatus = TS_RANDNUM;
        }
        break;
    default:
        break;
    }

    return eStatus;
}

#ifdef _CP920
void CTestPhoneCP::KeyboardAutoTestInit()
{
    chIniFileA file(KB_AUTO_TEST_CFG_FILE);

    phraseConstTextA txtGenetal(KB_CFG_SECTION_GENERAL);
    phraseConstTextA txtRawDataTole(KB_CFG_GENERAL_DELTA_TOLE);
    phraseConstTextA txtDeltaTole(KB_CFG_GENERAL_DELTA_TOLE);
    phraseConstTextA txtDeltaBase(KB_CFG_GENERAL_DELTA_BASE);
    phraseConstTextA txtRawData(KB_CFG_KEY_RAWDATA);
    phraseConstTextA txtDelta(KB_CFG_KEY_DELTA);

    m_nRoleDataTole = file.GetIntValue(txtGenetal, txtRawDataTole, 0);
    m_nDeltaTole = file.GetIntValue(txtGenetal, txtDeltaTole, 0);
    m_nDeltaBase = file.GetIntValue(txtGenetal, txtDeltaBase, 0);

    for (int i = CP920_KP_KEY_TOUCH_BEGIN; i <= CP920_KP_KEY_TOUCH_END; ++i)
    {
        yl::string strKey = KB_CFG_SECTION_KEY + commonAPI_FormatString("%d", i);
        phraseConstTextA txtKey(strKey.c_str());
        ST_KEY_TEST_DATA stData;

        stData.nRawData = file.GetIntValue(txtKey, txtRawData, 0);
        stData.nDelta = file.GetIntValue(txtKey, txtDelta, 0);
        stData.bSuccess = false;

        m_mapKeyTestData[i] = stData;
    }
}

bool CTestPhoneCP::KeyboardAutoTest()
{
    CPhoneDev * pPhoneDev = GetPhoneDev(getRealPathByPhoneType(SYSTEM_KEYMAP_PATH, KEYMAP_CONFIG_PATH, RESPATHTYPE_KEYMAP));

    if (NULL == pPhoneDev)
    {
        return false;
    }

    int nFD = pPhoneDev->GetKeypadHandle();

    if (-1 == nFD)
    {
        return false;
    }

    struct key_sense stKeySense;
    int nFailCnt = 0;

    for (int i = CP920_KP_KEY_TOUCH_BEGIN; i <= CP920_KP_KEY_TOUCH_END; ++i)
    {
        ST_KEY_TEST_DATA & stTestData = m_mapKeyTestData[i];
        int nRawDataDeviation = stTestData.nRawData * m_nRoleDataTole / 100;
        int nDeltaDeviation = m_nDeltaBase + stTestData.nDelta * m_nDeltaTole / 100;

        stKeySense.key = i;

        keypad_helper_get_key_sense(nFD, &stKeySense);

        stKeySense.delta = (int)((signed char)stKeySense.delta);

        if ((abs(stKeySense.rawdata - stTestData.nRawData) >= nRawDataDeviation)
                || (abs(stKeySense.delta - stTestData.nDelta) >= nDeltaDeviation))
        {
            stTestData.bSuccess = false;
            ++nFailCnt;
        }
        else
        {
            stTestData.bSuccess = true;
        }
    }

    return (0 == nFailCnt);
}

void CTestPhoneCP::ShowErrorKeys(bool bShow)
{
    testmode_ShowErrorKeys(bShow);
}

bool CTestPhoneCP::IsKeyTestSucceed(int nRawKey)
{
    MAP_KEY_TEST_DATA::iterator iter = m_mapKeyTestData.find(nRawKey);

    if (iter != m_mapKeyTestData.end())
    {
        ST_KEY_TEST_DATA & stTestData = iter->second;

        return stTestData.bSuccess;
    }

    return false;
}
#endif

bool CTestPhoneCP::OnChannelKeyPress(int iKey)
{
    if (PHONE_KEY_HANDSET_ON_HOOK == iKey)
    {
        // 响按键音
        PlayLocalDTMF(PHONE_KEY_A);
        // 设置退出定时器
        m_timerExitKey.SetTimer(CP_EXIT_KEY_TIME, TIMER_METHOD(this, CTestPhoneCP::OnExitKeyTimer));
    }
    // 拔插多模线需播放按键音
    else if (PHONE_KEY_LINEDEVICE_ON_HOOK != iKey
             && PHONE_KEY_LINEDEVICE_OFF_HOOK != iKey)
    {
        // 响按键音
        PlayLocalDTMF(PHONE_KEY_A);
    }

    if (NULL != m_pTestState
            && m_pTestState->OnFunkeyPress(iKey))
    {
        return true;
    }
    return false;
}

bool CTestPhoneCP::OnKeyRelease()
{
    m_timerExitKey.KillTimer();
    return true;
}

void CTestPhoneCP::AllLightCtrl(LED_STATE eState)
{
    if (LED_STATE_GREEN == eState)
    {
        m_iLedStatus = LED_GREEN;
    }
    else if (LED_STATE_RED == eState)
    {
        m_iLedStatus = LED_RED;
    }
    else if (LED_STATE_OFF == eState)
    {
        m_iLedStatus = LED_OFF;
    }

#if LINUX_SYSTEM
    m_objIOMutex.Lock();
    devicelib_SetLedStatus(m_iLedStatus);
    devicelib_SetExtLed(m_iLedStatus);
    m_objIOMutex.Unlock();
#endif
}

// 灯模式
void CTestPhoneCP::ChangeAllLight(LED_STATE eState)
{
    if (LED_STATE_NONE == eState)
    {
        return;
    }
    else if (LED_STATE_FLASH == eState)
    {
        m_timerLight.SetTimer(m_iLightTimer, TIMER_METHOD(this, CTestPhoneCP::OnLightTimer));
        // 先亮灯，否则需要等到定时器超时
        AllLightCtrl(LED_STATE_GREEN);
    }
    else
    {
        m_timerLight.KillTimer();
        AllLightCtrl(eState);
    }
}

void CTestPhoneCP::LightLinekeyLed(LIGHT_STEP eLightSetp)
{
    if (LS_GREEN == eLightSetp)
    {
        AllLightCtrl(LED_STATE_GREEN);
    }
    else if (LS_RED == eLightSetp)
    {
        AllLightCtrl(LED_STATE_RED);
    }
    else if (LS_OFF == eLightSetp)
    {
        AllLightCtrl(LED_STATE_OFF);
    }
}

void CTestPhoneCP::AllLightOff()
{
    CTestPhoneT66::AllLightOff();
    AllLightCtrl(LED_STATE_OFF);
}

// 设置免提模式
bool CTestPhoneCP::SetSpeakerMode(int iVolume)
{
    ipvp_set_mode(mkGetHandle(), IPVP_MODE_VCP);
    ipvp_set_volume(mkGetHandle(), iVolume);
    return true;
}

void CTestPhoneCP::StartEchoTest(ECHO_MODE eMode, UINT iExtData/* = 0*/)
{
    switch (eMode)
    {
    case EM_ECHO_START:
        {
            // 开启免提
            ipvp_set_mode(mkGetHandle(), IPVP_MODE_VCP);
            // 初始化时需要默认音量，设置音量
            ipvp_set_volume(mkGetHandle(), MID_VOLUME);
        }
        break;
    case EM_SPEAKER_RECORD:
        {
            UINT uChannelTmp = MIC_ALL;
            if (MIC_ALL != iExtData)
            {
                uChannelTmp = (iExtData <= 0) ? 0x1 : (1 << iExtData);
            }

            TESTMODE_INFO("Select Mic [%d] to mic test, Channel is [%d]", iExtData, uChannelTmp);
            ipvp_select_mic(mkGetHandle(), IPVP_MODE_VCP, 0, uChannelTmp);

            ipvp_echo_test(mkGetHandle(), IPVP_ECHO_STOP);
            ipvp_echo_test(mkGetHandle(), IPVP_ECHO_RECORD);
        }
        break;
    case EM_SPEAKER_PLAY:
        {
            ipvp_echo_test(mkGetHandle(), IPVP_ECHO_PLAY);
        }
        break;
    case EM_ECHO_MUTE:
        {
            ipvp_set_mute(mkGetHandle(), (iExtData > 0) ? IPVP_MUTE : IPVP_UNMUTE);
        }
        break;
    case EM_ECHO_LINEDEVICE:
        {
            ipvp_echo_test(mkGetHandle(), IPVP_ECHO_STOP);
            // 整机测试需要开启LOOP
            if (TST_FULLVOICE == GetTestMethod())
            {
                ipvp_loop_test(mkGetHandle(), 1);
            }

            // 取消静音/开启多模
            ipvp_set_mute(mkGetHandle(), IPVP_UNMUTE);
            ipvp_set_slave(mkGetHandle(), IPVP_SLAVE_LINE, IPVP_FL_DIR_LOCAL, 0);

            UINT uChannelTmp = MIC_ALL;
            if (MIC_ALL != iExtData)
            {
                uChannelTmp = (iExtData <= 0) ? 0x1 : (1 << iExtData);
            }

            TESTMODE_INFO("Select Mic [%d] to linedevice test", iExtData);
            ipvp_select_mic(mkGetHandle(), IPVP_MODE_VCP, 0, uChannelTmp);
        }
        break;
    default:
        break;
    }
}

void CTestPhoneCP::OnExitKeyTimer(xThreadTimer* timer, LPVOID pExtraData)
{
    ChangeState(new CTestStateExit(this));
    return TRUE;
}

void CTestPhoneCP::OnFlushLight()
{
    LED_STATE iNextStatus;
    if (LED_RED == m_iLedStatus)
    {
        iNextStatus = LED_STATE_GREEN;
    }
    else
    {
        iNextStatus = LED_STATE_RED;
    }
    AllLightCtrl(iNextStatus);
}

void CTestPhoneCP::SetTestModeEndFlag()
{
    m_PCServer.SendTestmodeCompleted();
}

void CTestPhoneCP::DrawStatusViewString(LPCSTR lpszTest)
{
    testmode_DrawText(lpszTest, TT_CONTENT_CENTER, TEST_FONT_10);
}

#endif
