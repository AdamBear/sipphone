#include "test_include.h"
#include "modtestmode.h"
#include "service_system.h"
#include "innermsgdefine.h"
#include "testmodeinterface.h"
#include "commonunits/modcommonunits.h"
#include "languagehelper/modlangtranslate.h"
#ifdef IF_USB_SUPPORT
#include "record/include/modrecord.h"
#endif
#include "lamp/backlight/include/modbacklight.h"
#include "talk/linedevice/include/modlinedevice.h"
#include "xwindow-kernel/include/xmessagehandler.h"
#ifdef _CP
#include "testphonecp.h"
#include "devicelib/networkserver.h"
#endif
#ifdef _CP920
#include "globalinputhook/modinputhook.h"
#include "phonedev/cp920keypad.h"
#include "phonedev/modphonedev.h"
#endif
#ifdef IF_SUPPORT_WIFI
#include "wireless/wifi/include/modwifi.h"
#endif
#if IS_COLOR
#include "uimanager/moduimanager.h"
#include "settingui/include/pageiddefine.h"
#include "settingui/include/modsettingui.h"
#include "wireless/bluetoothNew/include/modbluetooth.h"
#include "exp/include/modexp.h"
#endif

///////////////////////////CTestState///////////////////////////////////
CTestState::CTestState(CTestPhone * pTestPhone)
{
    m_pTestPhone = pTestPhone;
}

CTestState::~CTestState()
{

}

void CTestState::ChangeState(TEST_STATE eTestState)
{
    if (NULL != m_pTestPhone)
    {
        for (int i = TT_TITLE; i < TT_MAX; ++i)
        {
            testmode_DrawText("", i);
        }

        switch (eTestState)
        {
        case TS_INIT:
            {
                m_pTestPhone->ChangeState(new CTestStateInit(m_pTestPhone));
            }
            break;
        case TS_KEY:
            {
                m_pTestPhone->ChangeState(new CTestStateKey(m_pTestPhone));
            }
            break;
#ifdef _CP920
        case TS_AUTO_TEST_KEY:
            {
                m_pTestPhone->ChangeState(new CTestStateAutoTestKey(m_pTestPhone));
            }
            break;
        case TS_BACK_LIGHT:
            {
                m_pTestPhone->ChangeState(new CTestStateBackLight(m_pTestPhone));
            }
            break;
#endif
        case TS_EXIT:
            {
                m_pTestPhone->ChangeState(new CTestStateExit(m_pTestPhone));
            }
            break;
        case TS_LCD:
            {
                m_pTestPhone->ChangeState(new CTestStateLCD(m_pTestPhone));
            }
            break;
        case TS_ECHO:
            {
#ifdef _CP
                m_pTestPhone->ChangeState(new CTestStateCPEcho(m_pTestPhone));
#else
                m_pTestPhone->ChangeState(new CTestStateEcho(m_pTestPhone));
#endif
            }
            break;
        case TS_EXP_LIGHT:
            {
                m_pTestPhone->ChangeState(new CTestStateEXPLight(m_pTestPhone));
            }
            break;
        case TS_EXP_KEY:
            {
                m_pTestPhone->ChangeState(new CTestStateEXPKey(m_pTestPhone));
            }
            break;
        case TS_EXP_LCD:
            {
                m_pTestPhone->ChangeState(new CTestStateEXPLCD(m_pTestPhone));
            }
            break;
        case TS_EXP_EXIT:
            {
                m_pTestPhone->ChangeState(new CTestStateEXPExit(m_pTestPhone));
            }
            break;
        case TS_LED:
            {
#ifdef _CP
                m_pTestPhone->ChangeState(new CTestStateCPLED(m_pTestPhone));
#else
                m_pTestPhone->ChangeState(new CTestStateLED(m_pTestPhone));
#endif
            }
            break;
#if JOYCE_TEL_BUGFREE_19348
        case TS_SN:
            {
                m_pTestPhone->ChangeState(new CTestStateSN(m_pTestPhone));
            }
            break;
#endif
#ifdef IF_USB_SUPPORT
        case TS_USB:
            {
                m_pTestPhone->ChangeState(new CTestStateUSB(m_pTestPhone));
            }
            break;
#endif
#ifdef IF_SUPPORT_WIFI
        case TS_WIFI:
            {
                m_pTestPhone->ChangeState(new CTestStateWifi(m_pTestPhone));
            }
            break;
#endif
#ifdef _CP
        case TS_FINISH:
            {
                m_pTestPhone->ChangeState(new CTestStateFinish(m_pTestPhone));
            }
            break;
        case TS_IP:
            {
                m_pTestPhone->ChangeState(new CTestStateIP(m_pTestPhone));
            }
#endif
#if IS_COLOR
        case TS_COLOR_LCD:
            {
                m_pTestPhone->ChangeState(new CTestStateColorLCD(m_pTestPhone));
            }
            break;
        case TS_BLUETOOTH:
            {
                m_pTestPhone->ChangeState(new CTestStateBlueTooth(m_pTestPhone));
            }
            break;
#endif
        default:
            break;
        }
    }
}

bool CTestState::OnDigitkeyPress(int iKey)
{
    return true;
}

///////////////////////////CTestStateInit///////////////////////////////////
CTestStateInit::CTestStateInit(CTestPhone * pTestPhone) : CTestState(pTestPhone)
{
    m_bBlack = TRUE;
}

CTestStateInit::~CTestStateInit()
{
    if (m_bBlack)
    {
        xColor colWhite = xColor(0xff, 0xff, 0xff);
        testmode_FillRect(AT_ALL, colWhite);
    }
}

bool CTestStateInit::OnFunkeyPress(int iKey, bool bIsExpKey /*= false*/)
{
    if (NULL == m_pTestPhone)
    {
        return false;
    }

    TEST_STATE eState = m_pTestPhone->MapStateOfKey(iKey, bIsExpKey);
    if (eState == TS_EXIT)
    {
        ChangeState(eState);
    }
    else if (eState == TS_RANDNUM)
    {
        if (m_bBlack)
        {
            m_bBlack = FALSE;
            DrawScreen();
        }
        else
        {
#ifdef _CP920
            if (PT_CP920 == devicelib_GetPhoneType())
            {
                ChangeState(TS_BACK_LIGHT);
                return true;
            }
#endif

            if (m_pTestPhone->GetHasLedLight())
            {
                ChangeState(TS_LED);
            }
            else if (!m_pTestPhone->GetISSFP())
            {
                if (IS_SUPPORT_USB(devicelib_GetPhoneType()))
                {
                    // 支持USB机型直接进入USB测试
                    ChangeState(TS_USB);
                }
                else
                {
                    // 查看完随机数，进入按键测试
                    ChangeState(TS_KEY);
                }
            }
        }
    }
#ifndef _CP920
    else if (eState == TS_ECHO)
    {
        // SFP测试, 没有跳转到LED则需要响应TS_ECHO
        if (m_pTestPhone->GetISSFP() && !m_bBlack)
        {
            ChangeState(eState);
        }
    }
    else if (eState != TS_EXP_EXIT && !m_pTestPhone->GetISSFP())
    {
        // 根据不同的按键跳转到不同的状态
        ChangeState(eState);
    }
#endif

    return true;
}

bool CTestStateInit::DrawScreen()
{
    if (NULL == m_pTestPhone)
    {
        return false;
    }

    // 填充屏幕为全黑
    m_pTestPhone->FillFullRect(m_bBlack);
    return true;
}

void CTestStateInit::AfterEnterState()
{
#ifndef _CP
    // 背光灯常亮
    msgPostMsgToThread(msgGetThreadByName(VP_PHONE_THREAD_NAME), DSK_MSG_LIGHT_STATUS_CHANGE, 1, 1);
#endif

    // 画测试模式初始化界面
    DrawScreen();
}

///////////////////////////CTestStateLCD///////////////////////////////////
CTestStateLCD::CTestStateLCD(CTestPhone * pTestPhone)
    : CTestState(pTestPhone)
{
    m_eCurrentStep = LS_STEP1;
}

CTestStateLCD::~CTestStateLCD()
{
    if (m_timer.IsTimerRuning())
    {
        m_timer.KillTimer();
    }
}

bool CTestStateLCD::OnFunkeyPress(int iKey, bool bIsExpKey /*= false*/)
{
    if (NULL == m_pTestPhone)
    {
        return false;
    }

    TEST_STATE eState = m_pTestPhone->MapStateOfKey(iKey, bIsExpKey);

    // 如果是半成品测试
    if (m_pTestPhone->GetISSFP())
    {
        if (TS_RANDNUM == eState)
        {
            ChangeState(TS_LED);
        }
        else if (TS_ECHO == eState || TS_EXIT == eState)
        {
            ChangeState(eState);
        }
    }
    else if (eState == TS_EXIT || eState == TS_EXP_LIGHT)
    {
        ChangeState(eState);
    }

    return true;
}

bool CTestStateLCD::DrawScreen()
{
    if (NULL == m_pTestPhone)
    {
        return false;
    }

    // 绘制屏幕内容
    m_pTestPhone->TestLCDByStep(m_eCurrentStep);
    return true;
}

void CTestStateLCD::AfterEnterState()
{
    m_timer.SetTimer(1000, TIMER_METHOD(this, CTestStateLCD::OnTimer));
    DrawScreen();
}

void CTestStateLCD::OnTimer(xThreadTimer* timer, LPVOID pExtraData)
{
    m_eCurrentStep = (LCD_STEP)(m_eCurrentStep + 1);
    if (m_eCurrentStep == LS_LAST)
    {
        m_eCurrentStep = LS_STEP1;
    }

    DrawScreen();
}

///////////////////////////CTestStateKey///////////////////////////////////
CTestStateKey::CTestStateKey(CTestPhone * pTestPhone) : CTestState(pTestPhone)
{
    m_bInputCorrect = true;
    m_iCurrentIndex = -1;
#ifdef _CP920
    m_bTestOver = false;
#endif
}

CTestStateKey::~CTestStateKey()
{

}

bool CTestStateKey::OnFunkeyPress(int iKey, bool bIsExpKey /*= false*/)
{
    if (NULL == m_pTestPhone
#ifdef _CP920
            || m_bTestOver
#endif
       )
    {
        return false;
    }

    TEST_STATE eState = m_pTestPhone->MapStateOfKey(iKey, bIsExpKey);
    if (TS_EXIT == eState || TS_EXP_LIGHT == eState)
    {
        ChangeState(eState);
    }
    else
    {
        // 判断当前的按键是否正确
        m_bInputCorrect = (GetKey(iKey, bIsExpKey) == m_pTestPhone->GetKeyString(m_iCurrentIndex));

        if (m_bInputCorrect)
        {
            ++m_iCurrentIndex;
        }

        if (m_iCurrentIndex == (int)m_pTestPhone->GetMaxKeyNum())
        {
#ifdef _CP920
            // 提示测试完成，等待退出
            testmode_DrawText("", TT_TITLE, TEST_FONT_10);
            testmode_DrawText("", TT_CONTENT_NORMAL, TEST_FONT_10);
            testmode_DrawText(_LANG(TRID_KEYBOARD_TEST_SUCCEED), TT_CONTENT_CENTER, TEST_FONT_12);
            m_bTestOver = true;
#elif IS_COLOR
            ChangeState(TS_LED);
#else
            ChangeState(TS_ECHO);
#endif
        }
        else
        {
            DrawScreen();
        }
    }

    return true;
}

bool CTestStateKey::OnDigitkeyPress(int iKey)
{
#ifdef _CP920
    if (m_bTestOver)
    {
        return true;
    }
#endif

    yl::string strKey = commonAPI_FormatString("%c", (char)iKey);
    m_bInputCorrect = (strKey == m_pTestPhone->GetKeyString(m_iCurrentIndex));

    if (m_bInputCorrect)
    {
        ++m_iCurrentIndex;
    }

    if (m_iCurrentIndex == (int)m_pTestPhone->GetMaxKeyNum())
    {
        ChangeState(TS_ECHO);
    }
    else
    {
        DrawScreen();
    }
    return true;
}

bool CTestStateKey::DrawScreen()
{
    if (NULL == m_pTestPhone)
    {
        return false;
    }

    // 调用基类的接口来根据不同的状态画屏幕
    m_pTestPhone->DrawKeyString(m_bInputCorrect, m_iCurrentIndex);

    if (-1 == m_iCurrentIndex)
    {
        // m_iCurrentIndex为-1表示刚进入按键测试
        ++m_iCurrentIndex;
    }

    return true;
}

void CTestStateKey::AfterEnterState()
{
#ifdef _CP920
    msgPostMsgToThread(msgGetThreadByName(VP_PHONE_THREAD_NAME), DSK_MSG_LIGHT_STATUS_CHANGE, 1, 1);
#endif

    if (NULL != m_pTestPhone)
    {
        // 拨号音
        m_pTestPhone->SetSpeakerMode(MID_VOLUME);
    }

    DrawScreen();
}

yl::string CTestStateKey::GetKey(int iKey, bool bIsExpKey)
{
    if (bIsExpKey)
    {
        if (GetEXPKeyIndex(iKey) != EXP_INSERT && GetEXPKeyIndex(iKey) != EXP_REMOVE)
        {
            return "AnyEXPKey";
        }

        return "";
    }

    switch (iKey)
    {
    case PHONE_KEY_HANDFREE:
        return (DEVICELIB_IS_CP(devicelib_GetPhoneType())) ? "Dial" : "HandFree";
        break;
    case PHONE_KEY_FN1:
    case PHONE_KEY_FN2:
    case PHONE_KEY_FN3:
    case PHONE_KEY_FN4:
        return commonAPI_FormatString("HK%d", iKey - PHONE_KEY_FN1 + 1);
        break;
    case PHONE_KEY_LINE1:
    case PHONE_KEY_LINE2:
    case PHONE_KEY_LINE3:
    case PHONE_KEY_LINE4:
    case PHONE_KEY_LINE5:
    case PHONE_KEY_LINE6:
    case PHONE_KEY_LINE7:
    case PHONE_KEY_LINE8:
    case PHONE_KEY_LINE9:
    case PHONE_KEY_LINE10:
        return commonAPI_FormatString("LK%d", iKey - PHONE_KEY_LINE1 + 1);
        break;
    case PHONE_KEY_UP:
        return "Up";
        break;
    case PHONE_KEY_DOWN:
        return "Dn";
        break;
    case PHONE_KEY_LEFT:
        return "Lf";
        break;
    case PHONE_KEY_RIGHT:
        return "Rt";
        break;
    case PHONE_KEY_OK:
        return "OK";
        break;
    case PHONE_KEY_PHONEBOOK:
        return "Contact";
        break;
    case PHONE_KEY_CANCEL:
        return "Cancel";
        break;
    case PHONE_KEY_VOLUME_INCREASE:
        return "Vol+";
        break;
    case PHONE_KEY_VOLUME_DECREASE:
        return "Vol-";
        break;
    case PHONE_KEY_MESSAGE:
        return "Msg";
        break;
    case PHONE_KEY_HEADSET_CTRL:
        return "Headset";
        break;
    case PHONE_KEY_CONFERENCE:
        return "Conf";
        break;
    case PHONE_KEY_HOLD:
        return "Hold";
        break;
    case PHONE_KEY_MUTE:
        {
#ifdef _CP920
            int nRawKey = 0;

#if IF_INPUT_HOOK_KEEP_KEY_RAW_VALUE
            nRawKey = inputHook_GetCurrentRawKey();
#endif
            if (PhoneDev_IsKeyMute2(nRawKey))
            {
                return "Left Mute";
            }

            if (PhoneDev_IsKeyMute3(nRawKey))
            {
                return "Right Mute";
            }

            return "Mid Mute";
#else
            return "Mute";
#endif
        }
        break;
    case PHONE_KEY_TRANSFER:
        return "Tran";
        break;
    case PHONE_KEY_REDIAL:
        return "RD";
        break;
    case PHONE_KEY_MENU:
        return "Menu";
        break;
#if IF_BUG_4090
    case PHONE_KEY_HISTORY:
        return "Delete";
        break;
#else
    case PHONE_KEY_HISTORY:
        return "History";
        break;
#endif
    case PHONE_KEY_DND:
        return "DND";
        break;
    case PHONE_KEY_INTERCOM:
        return "Intercom";
        break;
    case PHONE_KEY_HANDSET_ON_HOOK:
        return "Handup";
        break;
    case PHONE_KEY_BLUETOOTH:
        return "Bluetooh";
        break;
    default:
        break;
    }

    return "";
}

#ifdef _CP920
///////////////////////////CTestStateAutoTestKey///////////////////////////////////
CTestStateAutoTestKey::CTestStateAutoTestKey(CTestPhone * pTestPhone) : CTestState(pTestPhone)
{
    m_nResult = KB_TEST_RESULT_NONE;
}

CTestStateAutoTestKey::~CTestStateAutoTestKey()
{

}

bool CTestStateAutoTestKey::OnFunkeyPress(int iKey, bool bIsExpKey /*= false*/)
{
    if (NULL == m_pTestPhone)
    {
        return false;
    }

    TEST_STATE eState = m_pTestPhone->MapStateOfKey(iKey, bIsExpKey);
    if (TS_EXIT == eState || TS_EXP_LIGHT == eState)
    {
        ChangeState(eState);
    }
    else if (PHONE_KEY_MUTE == iKey)
    {
        int nRawKey = 0;

#if IF_INPUT_HOOK_KEEP_KEY_RAW_VALUE
        nRawKey = inputHook_GetCurrentRawKey();
#endif

        if (PhoneDev_IsKeyMute2(nRawKey))
        {
            AutoTestKeyBoard();
        }
        else if (PhoneDev_IsKeyMute3(nRawKey))
        {
            if (KB_TEST_RESULT_NONE == m_nResult)
            {
                m_pTestPhone->SetFullTestFlag(false);
            }
            m_pTestPhone->ShowErrorKeys(false);
            ChangeState(TS_INIT);
        }
    }

    return true;
}

void CTestStateAutoTestKey::AutoTestKeyBoard()
{
    if (NULL == m_pTestPhone)
    {
        return;
    }

    if (m_pTestPhone->KeyboardAutoTest())
    {
        m_nResult = KB_TEST_RESULT_SUCCESS;
    }
    else
    {
        m_nResult = KB_TEST_RESULT_FAIL;
    }

    DrawScreen();
}

bool CTestStateAutoTestKey::OnDigitkeyPress(int iKey)
{
    return true;
}

bool CTestStateAutoTestKey::DrawScreen()
{
    if (NULL == m_pTestPhone)
    {
        return false;
    }

    // 需求变更为：全机型都显示字眼KeyBoard Test
    testmode_DrawText(_LANG(TRID_KEYBOARD_TEST), TT_TITLE, TEST_FONT_12);

    switch (m_nResult)
    {
    case KB_TEST_RESULT_SUCCESS:
        {
            m_pTestPhone->ShowErrorKeys(false);
            testmode_DrawText(_LANG(TRID_KEYBOARD_TEST_SUCCEED), TT_CONTENT_CENTER, TEST_FONT_12);
        }
        break;
    case KB_TEST_RESULT_FAIL:
        {
            m_pTestPhone->ShowErrorKeys(true);
        }
        break;
    default:
        {
            yl::string strTip = _LANG(TRID_PLEASE_PUT_ON_ASSISTED_BORAD);

            strTip += _LANG(TRID_THEN_PRESS_THE_MUTE_KEY_ON_THE_LEFT_TO_TEST);
            strTip += _LANG(TRID_OR_PRESS_THE_MUTE_KEY_ON_THE_RIGHT_TO_NEXT_STEP);

            testmode_DrawText(strTip, TT_CONTENT_CENTER, TEST_FONT_10);
        }
        break;
    }

    return true;
}

void CTestStateAutoTestKey::AfterEnterState()
{
    // 加载配置
    if (NULL != m_pTestPhone)
    {
        m_pTestPhone->KeyboardAutoTestInit();
    }

    DrawScreen();
}


///////////////////////////CTestStateLCD///////////////////////////////////
CTestStateBackLight::CTestStateBackLight(CTestPhone* pTestPhone)
    : CTestState(pTestPhone)
    , m_bBackLightOn(true)
    , m_nInitBacklightLevel(0)
{

}

CTestStateBackLight::~CTestStateBackLight()
{
    m_timerBacklight.KillTimer();
    backlight_OpenBacklightEvent(false);
    backlight_SetBacklightLevel(m_nInitBacklightLevel);
    backlight_OpenBacklightEvent(true);
}

bool CTestStateBackLight::OnFunkeyPress(int iKey, bool bIsExpKey /*= false*/)
{
    if (NULL == m_pTestPhone)
    {
        return false;
    }

    if (PHONE_KEY_OK == iKey)
    {
        ChangeState(TS_LED);
    }
    else if (PHONE_KEY_MUTE == iKey)
    {
        int nRawKey = 0;

#if IF_INPUT_HOOK_KEEP_KEY_RAW_VALUE
        nRawKey = inputHook_GetCurrentRawKey();
#endif

        if (PhoneDev_IsKeyMute3(nRawKey))
        {
            ChangeState(TS_LED);
        }
    }

    return true;
}

bool CTestStateBackLight::DrawScreen()
{
    if (NULL == m_pTestPhone)
    {
        return false;
    }

    m_pTestPhone->DrawContentString(TRID_LCD_BACKLIGHT_TEST);
    return true;
}

void CTestStateBackLight::AfterEnterState()
{
    DrawScreen();

    m_nInitBacklightLevel = backlight_GetCurrenLightLevel();
    m_timerBacklight.SetTimer(1000, TIMER_METHOD(this, CTestStateBackLight::OnBackLightTimer));
}

void CTestStateBackLight::OnBackLightTimer(xThreadTimer* timer, LPVOID pExtraData)
{
    m_bBackLightOn = !m_bBackLightOn;
    backlight_OpenBacklightEvent(false);
    if (m_bBackLightOn)
    {
        backlight_SetBacklightLevel(m_nInitBacklightLevel);
    }
    else
    {
        backlight_SetBacklightLevel(0);
    }
    backlight_OpenBacklightEvent(m_bBackLightOn);
}
#endif

///////////////////////////CTestStateEcho///////////////////////////////////
CTestStateEcho::CTestStateEcho(CTestPhone * pTestPhone) : CTestState(pTestPhone)
{
    m_eCurrentMode = EM_ECHO_START;
    m_bNotify = false;
}

CTestStateEcho::~CTestStateEcho()
{
    StopSpeakRecordTimer();
    StopSpeakPlayTimer();
}

bool CTestStateEcho::OnFunkeyPress(int iKey, bool bIsExpKey /*= false*/)
{
    if (NULL == m_pTestPhone)
    {
        return false;
    }

    // 如果是半成品
    if (m_pTestPhone->GetISSFP())
    {
        TEST_STATE eState = m_pTestPhone->MapStateOfKey(iKey, bIsExpKey);
        if (eState == TS_EXIT || eState == TS_ECHO)
        {
            m_pTestPhone->ExitEchoTest();
            ChangeState(eState);
        }
        return TRUE;
    }

    if (m_bNotify)
    {
        if (PHONE_KEY_OK == iKey)
        {
            m_bNotify = false;
            m_pTestPhone->SetSkipEHSTest(true);
            ChangeState(TS_ECHO);
        }
        else if (PHONE_KEY_CANCEL == iKey)
        {
            ChangeState(TS_EXIT);
        }
        return true;
    }

    TEST_STATE eState = m_pTestPhone->MapStateOfKey(iKey, bIsExpKey);
    if (TS_EXIT == eState || TS_EXP_LIGHT == eState)
    {
        m_pTestPhone->ExitEchoTest();
        ChangeState(eState);
    }
    else
    {
        bool bUpdateWnd = false;
        ECHO_MODE eOldMode = m_eCurrentMode;

        // 如果Echo状态有变化
        if (m_pTestPhone->ChangeEchoState(iKey, m_eCurrentMode))
        {
            if (EM_EHS == eOldMode
                    && EM_SPEAKER_RECORD == m_eCurrentMode)
            {
                StartSpeakRecordTimer();
            }

            m_pTestPhone->StartEchoTest(m_eCurrentMode);
            bUpdateWnd = true;
        }

        if (EM_SPEAKER_PLAY == m_eCurrentMode)
        {
            if (PHONE_KEY_VOLUME_INCREASE == iKey)
            {
                bUpdateWnd = m_pTestPhone->ChangeSpeakerVolume(true);
            }
            else if (PHONE_KEY_VOLUME_DECREASE == iKey)
            {
                bUpdateWnd = m_pTestPhone->ChangeSpeakerVolume(false);
            }
            else
            {
                m_pTestPhone->DrawVolumeBar(false);
            }
        }
        else
        {
            // 非playmode删除音量条
            bUpdateWnd |= m_pTestPhone->ExitSpeakerVolume();
        }

        if (bUpdateWnd)
        {
            DrawScreen();
        }
    }
    return true;
}

bool CTestStateEcho::DrawScreen()
{
    if (NULL == m_pTestPhone)
    {
        return false;
    }

    if (m_bNotify)
    {
        testmode_DrawText("Skip EHS Test?(OK/Cancel)");
    }
    else
    {
        // 调用基类的接口来根据不同的状态画屏幕
        m_pTestPhone->DrawEchoString(m_eCurrentMode);
        // 画音量条
        m_pTestPhone->DrawVolumeBar();
    }

    return true;
}

void CTestStateEcho::AfterEnterState()
{
    if (NULL == m_pTestPhone)
    {
        return;
    }

    m_pTestPhone->DrawAllIcon(false);

    bool IsStartSpeak = false;
    if (m_pTestPhone->GetISSFP())
    {
        IsStartSpeak = true;
    }
    else if (m_pTestPhone->IsEHSSInserted())
    {
        m_pTestPhone->StartEchoTest(m_eCurrentMode);
        m_eCurrentMode = EM_EHS;
    }
    else if (!m_pTestPhone->IsSkipEHSTest())
    {
        m_bNotify = true;
    }
    else
    {
        IsStartSpeak = true;
    }

    if (IsStartSpeak)
    {
        // 开启录音定时器
        m_pTestPhone->StartEchoTest(m_eCurrentMode);
        StartSpeakRecordTimer();
        m_eCurrentMode = EM_SPEAKER_RECORD;
    }

    DrawScreen();
}

void CTestStateEcho::OnEHSInsert()
{
    if (m_bNotify)
    {
        m_bNotify = false;
        // 重新开始
        m_eCurrentMode = EM_ECHO_START;
        AfterEnterState();
    }
}

void CTestStateEcho::OnEHSRemove()
{
    m_bNotify = false;
}

// 开始录音计时
void CTestStateEcho::StartSpeakRecordTimer()
{
    if (m_timerSpeakRecord.IsTimerRuning())
    {
        m_timerSpeakRecord.KillTimer();
    }

    m_timerSpeakRecord.SetTimer(3 * 1000, TIMER_METHOD(this, CTestStateEcho::OnSpeakRecordTimer));
}

// 停止录音计时
void CTestStateEcho::StopSpeakRecordTimer()
{
    if (m_timerSpeakRecord.IsTimerRuning())
    {
        m_timerSpeakRecord.KillTimer();
    }
}

void CTestStateEcho::OnSpeakRecordTimer(xThreadTimer* timer, LPVOID pExtraData)
{
    if (EM_SPEAKER_RECORD != m_eCurrentMode)
    {
        return ;
    }

    StopSpeakRecordTimer();

    if (!m_pTestPhone->GetISSFP())
    {
        StartSpeakPlayTimer();
    }

    // 录音时间到,将录音播放出来
    m_eCurrentMode = EM_SPEAKER_PLAY;
    m_pTestPhone->StartEchoTest(m_eCurrentMode);
    DrawScreen();
}

// 开始播放计时
void CTestStateEcho::StartSpeakPlayTimer()
{
    if (m_timerSpeakPlay.IsTimerRuning())
    {
        m_timerSpeakPlay.KillTimer();
    }

    m_timerSpeakPlay.SetTimer(3 * 1000, TIMER_METHOD(this, CTestStateEcho::OnSpeakPlayTimer));
}

// 停止播放计时
void CTestStateEcho::StopSpeakPlayTimer()
{
    if (m_timerSpeakPlay.IsTimerRuning())
    {
        m_timerSpeakPlay.KillTimer();
    }
}

void CTestStateEcho::OnSpeakPlayTimer(xThreadTimer* timer, LPVOID pExtraData)
{
    if (EM_SPEAKER_PLAY != m_eCurrentMode)
    {
        return;
    }

    StopSpeakPlayTimer();
    StartSpeakRecordTimer();
    m_pTestPhone->ExitSpeakerVolume();
    // 录音时间到,将录音播放出来
    m_eCurrentMode = EM_SPEAKER_RECORD;
    m_pTestPhone->StartEchoTest(m_eCurrentMode);
    DrawScreen();
}

///////////////////////////CTestStateExit///////////////////////////////////
CTestStateExit::CTestStateExit(CTestPhone * pTestPhone) : CTestState(pTestPhone)
{

}

CTestStateExit::~CTestStateExit()
{

}

void CTestStateExit::AfterEnterState()
{
#if IF_BUG_20900
    // 已烧写，kill macd
    if (!commonUnit_IsMatchTestMode())
    {
        commonUnits_System("killall macd");
    }
#endif  //IF_BUG_20900

    // 灭掉所有灯
    if (NULL != m_pTestPhone)
    {
        m_pTestPhone->AllLightOff();
        m_pTestPhone->DrawAllIcon(false);

        if (NULL != m_pTestPhone->m_pTestEXP)
        {
            m_pTestPhone->m_pTestEXP->LightOffAllEXP();
        }
    }

    // 发送消息给VPM，要求退出回音测试
    ipvp_echo_test(mkGetHandle(), IPVP_ECHO_STOP);
    ipvp_set_mode(mkGetHandle(), IPVP_MODE_HS);

    // 设置背光灯熄灭
    msgPostMsgToThread(msgGetThreadByName(VP_PHONE_THREAD_NAME), DSK_MSG_LIGHT_STATUS_CHANGE, 1, 0);

    // 强制打开对应通道
    msgPostMsgToThread(msgGetThreadByName(VP_PHONE_THREAD_NAME), DSK_MSG_SELECT_CHANNEL, 1, 0);

    testmode_Exit();
}

///////////////////////////CTestStateAudio///////////////////////////////////
CTestStateAudio::CTestStateAudio(CTestPhone * pTestPhone)
    : CTestState(pTestPhone)
    , m_eCurrentMode(AD_HANDFREE_MIC)
{

}

CTestStateAudio::~CTestStateAudio()
{
    BeforeExitState();
}

bool CTestStateAudio::DrawScreen()
{
    if (NULL == m_pTestPhone)
    {
        return false;
    }

    // 调用基类的接口来根据不同的状态画屏幕
    if (AD_HANDFREE_MIC == m_eCurrentMode)
    {
        m_pTestPhone->DrawEchoString(EM_AUDIO_HANDFREE_MIC);
    }
    else if (AD_HANDFREE_SPEAKER == m_eCurrentMode)
    {
        m_pTestPhone->DrawEchoString(EM_AUDIO_HANDFREE_SPEAKER);
    }

    return true;
}

bool CTestStateAudio::OnFunkeyPress(int iKey, bool bIsExpKey /*= false*/)
{
    if (NULL == m_pTestPhone)
    {
        return false;
    }

    TEST_STATE eState = m_pTestPhone->MapStateOfKey(iKey, bIsExpKey);
    if (TS_EXIT == eState)
    {
        ChangeState(eState);
    }

    return true;
}

void CTestStateAudio::AfterEnterState()
{
    DrawScreen();

#if FEATURE_AUDIO_LOOP_TEST
    ipvp_loop_test(mkGetHandle(), 1);
#endif

    if (PT_T27 == devicelib_GetPhoneType() || PT_T27S == devicelib_GetPhoneType())
    {
        // 临时禁用背光灯
        msgPostMsgToThread(msgGetThreadByName(VP_PHONE_THREAD_NAME), BACKLIGHT_SET_ENABLE_MSG, 0, 0);
    }
}

void CTestStateAudio::BeforeExitState()
{
#if FEATURE_AUDIO_LOOP_TEST
    ipvp_loop_test(mkGetHandle(), 0);
#endif

    if (PT_T27 == devicelib_GetPhoneType() || PT_T27S == devicelib_GetPhoneType())
    {
        // 整机音频测试会禁用背光灯，此处启用背光灯
        msgPostMsgToThread(msgGetThreadByName(VP_PHONE_THREAD_NAME), BACKLIGHT_SET_ENABLE_MSG, 1, 0);
    }
}

///////////////////////////CTestStateEXPLight///////////////////////////////////
CTestStateEXPLight::CTestStateEXPLight(CTestPhone * pTestPhone) : CTestState(pTestPhone)
{
    m_eLightStep = LS_GREEN;

#ifdef _EXP_COLOR_SUPPORT
    m_iLCDLevel = 1;
    m_bLightLevelUp = true;
#endif
}

CTestStateEXPLight::~CTestStateEXPLight()
{
#ifdef _EXP_COLOR_SUPPORT
    exp_SetExpBKLight(0, 8);
#endif
}

bool CTestStateEXPLight::OnFunkeyPress(int iKey, bool bIsExpKey /*= false*/)
{
    if (NULL == m_pTestPhone)
    {
        return false;
    }

    if (!bIsExpKey)
    {
        if (PHONE_KEY_HANDFREE == iKey)
        {
            if (LS_ORANGEONE == m_eLightStep)
            {
                ChangeState(TS_EXP_KEY);
                return true;
            }

            m_eLightStep = (LIGHT_STEP)(m_eLightStep + 1);
        }

        if (NULL != m_pTestPhone->m_pTestEXP)
        {
            m_pTestPhone->m_pTestEXP->LightOnFirstEXP(m_eLightStep);
            DrawScreen();
        }
    }
    else
    {
        TEST_STATE eState = m_pTestPhone->MapStateOfKey(iKey, bIsExpKey);
        if (TS_EXP_EXIT == eState || TS_EXP_LIGHT == eState)
        {
            ChangeState(eState);
        }
    }
    return true;
}

bool CTestStateEXPLight::DrawScreen()
{
    if (NULL == m_pTestPhone)
    {
        return false;
    }

    // 调用基类的接口来根据不同的状态画屏幕
    m_pTestPhone->DrawEXPLightString(m_eLightStep);

    EXP_FILL_COLOR stuWhite;
    stuWhite.nRed = 255;
    stuWhite.nGreen = 255;
    stuWhite.nBlue = 255;
    stuWhite.bGradient = false;
    m_pTestPhone->m_pTestEXP->DrawScreenByColor(stuWhite);
    m_pTestPhone->m_pTestEXP->DrawScreenByText("USB OK");
    return true;
}

void CTestStateEXPLight::AfterEnterState()
{
    if (NULL == m_pTestPhone || NULL == m_pTestPhone->m_pTestEXP)
    {
        return;
    }

    exp_SetExpTestMode(true);

    // EXP测试中要有拨号音
    ipvp_echo_test(mkGetHandle(), IPVP_ECHO_STOP);
    ipvp_set_mode(mkGetHandle(), IPVP_MODE_HF);

    m_pTestPhone->m_pTestEXP->StartEXPTest();
    m_pTestPhone->m_pTestEXP->LightOnFirstEXP(m_eLightStep);

#ifdef _EXP_COLOR_SUPPORT
    m_timer.SetTimer(500, TIMER_METHOD(this, CTestStateEXPLight::OnTimer));
#endif

    DrawScreen();
}

#ifdef _EXP_COLOR_SUPPORT
void CTestStateEXPLight::OnTimer(xThreadTimer* timer, LPVOID pExtraData)
{
    // 设置屏幕亮度接口
    exp_SetExpBKLight(0, m_iLCDLevel);
    m_iLCDLevel = m_bLightLevelUp ? ((m_iLCDLevel + 1) % 11) : ((m_iLCDLevel - 1) % 11);
    m_bLightLevelUp = (m_iLCDLevel == 10) ? false : ((m_iLCDLevel == 1) ? true : m_bLightLevelUp);
}
#endif

///////////////////////////CTestStateEXPKey///////////////////////////////////
CTestStateEXPKey::CTestStateEXPKey(CTestPhone * pTestPhone) : CTestState(pTestPhone)
{
    m_bInputCorrect = false;
    m_iCurrentIndex = -1;
    m_bEXPOk = false;
}

CTestStateEXPKey::~CTestStateEXPKey()
{

}

bool CTestStateEXPKey::OnFunkeyPress(int iKey, bool bIsExpKey /*= false*/)
{
    if (NULL == m_pTestPhone)
    {
        return false;
    }

    // 有按键输入先将输入正确的变量置为否
    m_bInputCorrect = false;

    if (bIsExpKey)
    {
        TEST_STATE eState = m_pTestPhone->MapStateOfKey(iKey, bIsExpKey);
        if (TS_EXP_EXIT == eState || TS_EXP_LIGHT == eState)
        {
            ChangeState(eState);
            return true;
        }
        else
        {
            // 不能在函数入口判断，否则无法跳入TS_EXP_EXIT
            if (NULL == m_pTestPhone->m_pTestEXP)
            {
                return false;
            }

            int iEXPIndex = GetEXPIndex(iKey);
            int iEXPKeyIndex = GetEXPKeyIndex(iKey);

            if (0 == iEXPIndex)
            {
                if (iEXPKeyIndex == m_pTestPhone->m_pTestEXP->GetKeyIndex(m_iCurrentIndex))
                {
                    m_bInputCorrect = true;
                    m_pTestPhone->m_pTestEXP->LightOffByKeyIndex(iKey);
                    ++m_iCurrentIndex;

                    // 如果按下第一个扩展台的最后一个按键，则进入EXP39LCD测试
                    if (m_pTestPhone->m_pTestEXP->BeNeedTestLCD()
                            && m_iCurrentIndex == (int)m_pTestPhone->m_pTestEXP->GetKeyNum())
                    {
                        ChangeState(TS_EXP_LCD);
                        return true;
                    }
                }
            }
            else if (1 == iEXPIndex)
            {
                // 如果不需要LCD测试则按第二台扩展台的第一个按键退出测试模式
                if (!m_pTestPhone->m_pTestEXP->BeNeedTestLCD()
                        && 0 == iEXPKeyIndex
                        && m_iCurrentIndex == (int)m_pTestPhone->m_pTestEXP->GetKeyNum())
                {
                    m_pTestPhone->m_pTestEXP->LightOffByKeyIndex(iKey);
                    m_bEXPOk = true;

                    DrawScreen();

                    // 显示测试OK的提示1s
                    etlSleep(1000);

                    ChangeState(TS_EXP_EXIT);
                    return true;
                }
            }
        }
    }

    DrawScreen();
    return true;
}

bool CTestStateEXPKey::DrawScreen()
{
    if (NULL == m_pTestPhone)
    {
        return false;
    }

    if (m_bEXPOk)
    {
        m_pTestPhone->DrawEXPTestString("EXP is OK!");
    }
    else
    {
        if (-1 == m_iCurrentIndex)
        {
            // m_iCurrentIndex为-1表示刚进入按键测试
            ++m_iCurrentIndex;
        }
        else
        {
            // 调用基类的接口来根据不同的状态画屏幕
            m_pTestPhone->DrawEXPKeyString(m_bInputCorrect, m_iCurrentIndex);
        }
    }

    return true;
}

void CTestStateEXPKey::AfterEnterState()
{
    DrawScreen();
}

///////////////////////////CTestStateLED///////////////////////////////////
CTestStateLED::CTestStateLED(CTestPhone * pTestPhone) : CTestState(pTestPhone)
{
    m_LedStep = LS_GREEN;
}

CTestStateLED::~CTestStateLED()
{

}

bool CTestStateLED::OnFunkeyPress(int iKey, bool bIsExpKey)
{
    int iLedMaxStep = m_pTestPhone->GetLedMaxStep();
    TEST_STATE eState = m_pTestPhone->MapStateOfKey(iKey, bIsExpKey);

    // 如果是半成品
    if (m_pTestPhone->GetISSFP())
    {
        if (TS_EXIT == eState)
        {
            ChangeState(eState);
        }
        else if (m_LedStep >= iLedMaxStep && TS_ECHO == eState)
        {
            ChangeState(eState);
        }
        else if (m_pTestPhone->IsNextStateKeyPress(iKey))//免提键
        {
            if (m_LedStep < iLedMaxStep)
            {
                m_LedStep++;
                LightLedByStep();
            }
        }

        return true;
    }

    if (TS_EXIT == eState || TS_EXP_LIGHT == eState)
    {
        ChangeState(eState);
    }
    else if (m_pTestPhone->IsNextStateKeyPress(iKey))
    {
        if (m_LedStep >= iLedMaxStep)
        {
#if IS_COLOR
            if (TS_COLOR == eState)
            {
                ChangeState(TS_ECHO);
            }
            else
#endif

                if (IS_SUPPORT_USB(devicelib_GetPhoneType()))
                {
                    // 支持USB机型直接进入USB测试
                    ChangeState(TS_USB);
                }
                else
                {
                    // 进入按键测试
                    ChangeState(TS_KEY);
                }
        }
        else
        {
            ++m_LedStep;
            LightLedByStep();
        }
    }

    return true;
}

bool CTestStateLED::DrawScreen()
{
    if (NULL == m_pTestPhone)
    {
        return false;
    }

    // 调用基类的接口来根据不同的状态画屏幕
    m_pTestPhone->DrawEchoString(EM_LED);
    return true;
}

void CTestStateLED::AfterEnterState()
{
    if (NULL == m_pTestPhone)
    {
        return;
    }

    m_pTestPhone->AllLightOff();
    DrawScreen();
    LightLedByStep();
}

void CTestStateLED::LightLedByStep()
{
    if (NULL == m_pTestPhone)
    {
        return ;
    }

    if (m_LedStep >= LS_GREEN && m_LedStep <= LS_ORANGETWO)
    {
        m_pTestPhone->LightLinekeyLed((LIGHT_STEP)m_LedStep);
    }
}

///////////////////////////CTestStateEXPLCD///////////////////////////////////
CTestStateEXPLCD::CTestStateEXPLCD(CTestPhone * pTestPhone) : CTestState(pTestPhone)
{
    m_iCurrentStep = 0;
    m_bEXPOk = false;
    m_bLCDTestFinished = false;
}

CTestStateEXPLCD::~CTestStateEXPLCD()
{

}

bool CTestStateEXPLCD::OnFunkeyPress(int iKey, bool bIsExpKey /*= false*/)
{
    if (NULL == m_pTestPhone)
    {
        return false;
    }

    // 只处理EXP按键
    // 提示完成测试中不响应任何按键
    if (!bIsExpKey || m_bEXPOk)
    {
        return true;
    }

    TEST_STATE eState = m_pTestPhone->MapStateOfKey(iKey, bIsExpKey);
    if (TS_EXP_EXIT == eState || TS_EXP_LIGHT == eState)
    {
        ChangeState(eState);
        return true;
    }

    // 不能在函数入口判断，否则无法跳入TS_EXP_EXIT
    if (NULL == m_pTestPhone->m_pTestEXP)
    {
        return false;
    }

    int iEXPIndex = GetEXPIndex(iKey);
    int iEXPKeyIndex = GetEXPKeyIndex(iKey);

    if (0 == iEXPIndex)
    {
        if (iEXPKeyIndex ==
                m_pTestPhone->m_pTestEXP->GetKeyIndex((int)m_pTestPhone->m_pTestEXP->GetKeyNum() - 1))
        {
            ++m_iCurrentStep;

            int iMaxStep = m_pTestPhone->m_pTestEXP->GetEXPLCDMaxStep();
            m_iCurrentStep = m_iCurrentStep % iMaxStep;

            // 必须要把4张图片测一遍
            if (!m_bLCDTestFinished && iMaxStep - 1 == m_iCurrentStep)
            {
                m_bLCDTestFinished = true;
            }

            DrawScreen();
        }
    }
    else if (1 == iEXPIndex)
    {
        if (m_bLCDTestFinished && 0 == iEXPKeyIndex)
        {
            m_pTestPhone->m_pTestEXP->LightOffByKeyIndex(iKey);
            m_bEXPOk = true;

            DrawScreen();

            m_timer.SetTimer(1000, TIMER_METHOD(this, CTestStateEXPLCD::OnTimer));
        }
    }

    return true;
}

bool CTestStateEXPLCD::DrawScreen()
{
    if (NULL == m_pTestPhone || NULL == m_pTestPhone->m_pTestEXP)
    {
        return false;
    }

    if (m_bEXPOk)
    {
        m_pTestPhone->DrawEXPTestString("EXP is OK!");
    }
    else
    {
        yl::string strLCDTest = commonAPI_FormatString("EXP LCD Test%d", m_iCurrentStep + 1);
        m_pTestPhone->DrawEXPTestString(strLCDTest.c_str());
        m_pTestPhone->m_pTestEXP->DrawEXPLCD(m_iCurrentStep);
    }

    return true;
}

void CTestStateEXPLCD::AfterEnterState()
{
    DrawScreen();
}

void CTestStateEXPLCD::OnTimer(xThreadTimer * timer, LPVOID pExtraData)
{
    m_timer.KillTimer();
    ChangeState(TS_EXP_EXIT);
}

///////////////////////////CTestStateEXPExit///////////////////////////////////
CTestStateEXPExit::CTestStateEXPExit(CTestPhone * pTestPhone) : CTestState(pTestPhone)
{

}

CTestStateEXPExit::~CTestStateEXPExit()
{
    xColor color = xColor(0xff, 0xff, 0xff);
    testmode_FillRect(AT_ALL, color);
}

bool CTestStateEXPExit::OnFunkeyPress(int iKey, bool bIsExpKey/* = false*/)
{
    if (TS_EXP_LIGHT == m_pTestPhone->MapStateOfKey(iKey, bIsExpKey))
    {
        ChangeState(TS_EXP_LIGHT);
    }
    return true;
}

bool CTestStateEXPExit::DrawScreen()
{
    if (NULL == m_pTestPhone)
    {
        return false;
    }

    xColor color = xColor(0x00, 0x00, 0x00);
#if IS_COLOR
    color = xColor(0xff, 0x00, 0x00);
#endif

    testmode_FillRect(AT_ALL, color);
    return true;
}

void CTestStateEXPExit::AfterEnterState()
{
    if (NULL == m_pTestPhone)
    {
        return;
    }

    if (NULL != m_pTestPhone->m_pTestEXP)
    {
        m_pTestPhone->m_pTestEXP->ExitEXPTest();
    }

    DrawScreen();
}

#if JOYCE_TEL_BUGFREE_19348
///////////////////////////CTestStateSN///////////////////////////////////
CTestStateSN::CTestStateSN(CTestPhone * pTestPhone) : CTestState(pTestPhone)
{

}

CTestStateSN::~CTestStateSN()
{

}

bool CTestStateSN::OnFunkeyPress(int iKey, bool bIsExpKey /*= false*/)
{
    if (NULL == m_pTestPhone)
    {
        return false;
    }

    TEST_STATE eState = m_pTestPhone->MapStateOfKey(iKey, bIsExpKey);
    if (eState != TS_EXP_EXIT)
    {
        // 根据不同的按键跳转到不同的状态
        ChangeState(eState);
    }
    return true;
}

bool CTestStateSN::DrawScreen()
{
    if (NULL == m_pTestPhone)
    {
        return false;
    }

    // 全屏显示sn
    yl::string strSN = GetSN();
    m_pTestPhone->DrawSN(strSN.c_str());
    return true;
}

void CTestStateSN::AfterEnterState()
{
    // 画测试模式初始化界面
    DrawScreen();
}

// 获取客户特制SN内容
yl::string CTestStateSN::GetSN()
{
    char pBuffer[160] = "";
    char pBufferData[160] = "";//用于暂时存储明文的局部变量

    //etlGetProfileString("/config/ADVnumber.cfg", "sn", "sn", pBuffer, "");
    yl::string strBuffer = configParser_GetConfigString(kszCustomSN);
    strncpy(pBuffer, strBuffer.c_str(), sizeof(pBuffer) - 1);
    pBuffer[sizeof(pBuffer) - 1] = '\0';


    strcpy(pBufferData, pBuffer);
    if (strlen(pBufferData) != 0)
    {
        pw_data_decrypt((const unsigned char *)pBufferData, (unsigned char *)pBuffer);
    }

    // 如果在"/yealink/config/ADVnumber.cfg"没有读到sn,则到配置"/yealink/config/vpPhone/ADVnumber.cfg"中去读
    if (0 == strlen(pBuffer)
            || 0 == strcmp(pBuffer, "%NULL%"))
    {
        // autop那些都是延用旧的路径，还是使用vpPhone，因此gui也修改成与其统一。
        //etlGetProfileString("/config/vpPhone/ADVnumber.cfg", "sn", "sn", pBuffer, "");
        strBuffer = "";
        strBuffer = configParser_GetConfigString(kszFactorySN);
        strncpy(pBuffer, strBuffer, sizeof(pBuffer) - 1);
        pBuffer[sizeof(pBuffer) - 1] = '\0';

        strcpy(pBufferData, pBuffer);
        if (strlen(pBufferData) != 0)
        {
            pw_data_decrypt((const unsigned char *)pBufferData, (unsigned char *)pBuffer);
        }
    }

    // 如果sn长度超过4,则将sn截断保留后四位
    int iLen = strlen(pBuffer);
    if (iLen > 4 && strcmp(pBuffer, "%NULL%") != 0)
    {
        for (int i = 0; i < iLen - 4; ++i)
        {
            pBuffer[i] = '*';
        }
    }
    else if (0 == iLen)
    {
        strcpy(pBuffer, "%NULL%");
    }

    return pBuffer;
}
#endif

#ifdef _CP
///////////////////////////CTestStateCPEcho///////////////////////////////////
CTestStateCPEcho::CTestStateCPEcho(CTestPhone * pTestPhone)
    : CTestState(pTestPhone)
    , m_eCurrentMode(EM_SPEEKER)
{
    m_uPlayTime = 3 * 1000;
    m_eCurrentState = ES_RECORD;
    m_eLEDState = LED_STATE_GREEN;
    m_uMicIndex = 0;
    m_bMute = false;
}

CTestStateCPEcho::~CTestStateCPEcho()
{
    m_timerPlayTime.KillTimer();
    m_timerMicIndex.KillTimer();
    m_timerSpeakRecord.KillTimer();

    BeforeExit();
}

void CTestStateCPEcho::BeforeExit()
{
    ipvp_set_mute(mkGetHandle(), IPVP_UNMUTE);
    ipvp_select_mic(mkGetHandle(), IPVP_MODE_VCP, 1, 0);
    ipvp_set_slave(mkGetHandle(), IPVP_SLAVE_LINE, IPVP_FL_DIR_NONE, 0);
}

bool CTestStateCPEcho::OnFunkeyPress(int iKey, bool bIsExpKey /*= false*/)
{
    if (NULL == m_pTestPhone)
    {
        return false;
    }

#ifdef _CP920
    if (PHONE_KEY_MUTE == iKey && EM_ECHO_MIC == m_eCurrentMode)
    {
        int nRawKey = 0;

#if IF_INPUT_HOOK_KEEP_KEY_RAW_VALUE
        nRawKey = inputHook_GetCurrentRawKey();
#endif
        if (PhoneDev_IsKeyMute3(nRawKey))
        {
            iKey = PHONE_KEY_OK;
        }
    }

    if (PHONE_KEY_OK == iKey && m_uMicIndex >= 2 && EM_ECHO_MIC == m_eCurrentMode && TST_PREBURNING != m_pTestPhone->GetTestMethod())
    {
        m_pTestPhone->ExitSpeakerVolume();
        ChangeState(TS_IP);
        return true;
    }
#endif

    bool bUpdate = false;
#ifdef IF_SUPPORT_LINE_DEVICE
    if (PHONE_KEY_LINEDEVICE_ON_HOOK == iKey
            || PHONE_KEY_LINEDEVICE_OFF_HOOK == iKey)
    {
        bUpdate = true;
    }
    else
#endif //IF_SUPPORT_LINE_DEVICE
        // 老化测试自动切换MIC
        if (TST_PREBURNING != m_pTestPhone->GetTestMethod())
        {
            if (ChangeEchoState(iKey))
            {
                bUpdate = true;
                StartEchoByVPM();
            }
        }
#ifdef _CP920
        else
        {
            // 老化测试新增OK按键切换需求
            if (PHONE_KEY_OK == iKey)
            {
                m_uMicIndex++;
                if (m_uMicIndex > 2)
                {
                    m_uMicIndex = 0;
                }
                ChangeLightState();
                m_eCurrentState = ES_RECORD;
                StartEchoByVPM();
                DrawScreen();
            }
        }
#endif

    if (PHONE_KEY_VOLUME_INCREASE == iKey)
    {
        bUpdate |= m_pTestPhone->ChangeSpeakerVolume(true);
    }
    else if (PHONE_KEY_VOLUME_DECREASE == iKey)
    {
        bUpdate |= m_pTestPhone->ChangeSpeakerVolume(false);
    }
    else
    {
        // 非playmode删除音量条
        bUpdate |= m_pTestPhone->ExitSpeakerVolume();
    }

    if (bUpdate)
    {
        DrawScreen();
    }
    return true;
}

bool CTestStateCPEcho::OnDigitkeyPress(int iKey)
{
    if (EM_ECHO_MIC == m_eCurrentMode
            || EM_ECHO_LINEDEVICE == m_eCurrentMode)
    {
#ifndef _CP920 // CP920 老化测试新增数字按键切换需求
        // 老化测试模式，不能手动切换MIC
        if (TST_PREBURNING == m_pTestPhone->GetTestMethod())
        {
            return false;
        }
#endif

        if (iKey >=  PHONE_KEY_1
                && iKey <= PHONE_KEY_3)
        {
            UINT uOld = m_uMicIndex;
            // 按数字键切换通道
            m_uMicIndex = iKey - PHONE_KEY_1;

            if (uOld != m_uMicIndex)
            {
                ChangeLightState();
            }
            m_eCurrentState = ES_RECORD;
            StartEchoByVPM();
            DrawScreen();
        }
    }
    // 非playmode删除音量条
    if (m_pTestPhone->ExitSpeakerVolume())
    {
        DrawScreen();
    }
    return true;
}

bool CTestStateCPEcho::DrawScreen()
{
    if (NULL == m_pTestPhone)
    {
        return false;
    }
    // 先清空屏幕
    m_pTestPhone->FillFullRect(false);

    yl::string strDisp;
    if (EM_ECHO_MIC == m_eCurrentMode)
    {
        if (TST_PREBURNING == m_pTestPhone->GetTestMethod())
        {
            // 老化测试只测试机器上的MIC1、2、3，新需求也测试扩展MIC
            strDisp = commonAPI_FormatString("MIC %d", m_uMicIndex + 1);
        }
        else
        {
            if (ES_RECORD == m_eCurrentState)
            {
                strDisp = commonAPI_FormatString("MIC %d Speak", m_uMicIndex + 1);
            }
            else if (ES_PLAY == m_eCurrentState)
            {
                strDisp = "Speaker Play";
            }
        }
    }
    else if (EM_ECHO_MUTE == m_eCurrentMode)
    {
        strDisp = "Mute Test";
    }
    else if (EM_ECHO_LINEDEVICE == m_eCurrentMode)
    {
        if (!lineDevice_IsConnect(false))
        {
            strDisp = "NO PC or Phone\nPlease Connect";
        }
        else if (TST_FULLVOICE == m_pTestPhone->GetTestMethod())
        {
            // 整机测试包含MIC1、2、3及扩展MIC1、2
            strDisp = commonAPI_FormatString("Audio Test\nMIC %d", m_uMicIndex + 1);
        }
        else
        {
            strDisp = "3.5mm Audio Test";
        }
    }

    testmode_DrawText(strDisp);

    // 画音量条
    m_pTestPhone->DrawVolumeBar();
    return true;
}

void CTestStateCPEcho::OnPlayTimerEnd(xThreadTimer * timer, LPVOID pExtraData)
{
    m_timerPlayTime.KillTimer();
    if (ES_PLAY != m_eCurrentState)
    {
        return ;
    }
    m_eCurrentState = ES_RECORD;
    StartEchoByVPM();
}

void CTestStateCPEcho::OnMicIndexTimerEnd(xThreadTimer * timer, LPVOID pExtraData)
{
    // 老化测试时，切换MIC
    if (++m_uMicIndex > 2)
    {
        m_uMicIndex = 0;
    }
    ChangeLightState();
    DrawScreen();


}

// 开始播放计时
void CTestStateCPEcho::StartSpeakPlayTimer()
{
    m_timerPlayTime.SetTimer(m_uPlayTime, TIMER_METHOD(this, CTestStateCPEcho::OnPlayTimerEnd));
}

void CTestStateCPEcho::AfterEnterState()
{
    m_eCurrentMode = m_pTestPhone->GetEchoMode();
    ChangeLightState(LED_STATE_GREEN);

    if (EM_ECHO_MUTE == m_eCurrentMode)
    {
        m_uMicIndex = MIC_ALL;
    }

    // 开始回音测试，需要初始化开启免提，设置音量
    m_pTestPhone->StartEchoTest(EM_ECHO_START);

    if (TST_PREBURNING == m_pTestPhone->GetTestMethod())
    {
        // 老化测试定时切换通道，初始化时设置一次
        m_timerMicIndex.SetTimer(5 * 60 * 1000, TIMER_METHOD(this, CTestStateCPEcho::OnMicIndexTimerEnd));
    }
    StartEchoByVPM();
    DrawScreen();
}

void CTestStateCPEcho::ProcessRecordPlay()
{
    // 处理录放
    if (ES_RECORD == m_eCurrentState)
    {
        m_pTestPhone->StartEchoTest(EM_SPEAKER_RECORD, m_uMicIndex);
        // 开启录音定时器
        StartSpeakRecordTimer();
    }
    else if (ES_PLAY == m_eCurrentState)
    {
        m_pTestPhone->StartEchoTest(EM_SPEAKER_PLAY, m_uMicIndex);

        if (TST_PREBURNING == m_pTestPhone->GetTestMethod()
                || (TST_NORMAL == m_pTestPhone->GetTestMethod()
                    && EM_ECHO_MUTE == m_eCurrentMode))
        {
            // 老化测试开启播放定时器，播放结束后，切换到录音模式
            StartSpeakPlayTimer();
        }
    }
}

void CTestStateCPEcho::StartEchoByVPM()
{
    if (NULL == m_pTestPhone)
    {
        return;
    }

    // CP 将测试音频逻辑全部放到CTestStateEcho处理
    if (EM_ECHO_MIC == m_eCurrentMode)
    {
        ProcessRecordPlay();
    }
    else if (EM_ECHO_MUTE == m_eCurrentMode)
    {
        ProcessRecordPlay();

        // 录放过程保存灯状态，但只能在飞mute时设置灯状态
        if (ES_RECORD == m_eCurrentState)
        {
            m_eLEDState = LED_STATE_OFF;
        }
        else
        {
            m_eLEDState = LED_STATE_GREEN;
        }
        if (!m_bMute)
        {
            ChangeLightState(m_eLEDState);
        }
    }
    else
    {
        m_pTestPhone->StartEchoTest(m_eCurrentMode, m_uMicIndex);
    }
}

bool CTestStateCPEcho::ChangeChannel()
{
    // 按OK键时，改变通道
    switch (m_eCurrentMode)
    {
    case EM_ECHO_MIC:
        {
            m_uMicIndex += 1;
            m_eCurrentState = ES_RECORD;

            // Mic 5 则跳转到Mute模式
            if (m_uMicIndex > 2)
            {
                // 退出录音定时器
                StopSpeakRecordTimer();

                m_eCurrentMode = EM_ECHO_MUTE;
                m_uMicIndex = MIC_ALL;
                // 切换颜色
                ChangeLightState(LED_STATE_GREEN);
                m_eCurrentState = ES_RECORD;
            }
            else
            {
                // MIC 3.5mm
                ChangeLightState();
            }
        }
        break;
    case EM_ECHO_MUTE:
        {
            // 退出录音定时器
            StopSpeakRecordTimer();
            // 退出播放定时器
            m_timerPlayTime.KillTimer();

            // 退出Mute测试
            ChangeLightState(LED_STATE_GREEN);

            // 设置新状态
            m_eCurrentMode = EM_ECHO_LINEDEVICE;
            m_eCurrentState = ES_LOOP;
            m_uMicIndex = 0;
            m_bMute = false;
        }
        break;
    default:
        // 退出
        return false;
    }
    return true;
}

bool CTestStateCPEcho::ChangeEchoState(int iKey)
{
    //SCREEN_INFO("ChangeEchoState by key = %d", iKey);
    bool bChange = false;
    switch (iKey)
    {
    case PHONE_KEY_OK:
        {
            bChange = ChangeChannel();
        }
        break;
    case PHONE_KEY_MUTE:
        {
            if (EM_ECHO_MUTE == m_eCurrentMode)
            {
                m_bMute = !m_bMute;
                if (m_bMute)
                {
                    m_pTestPhone->ChangeAllLight(LED_STATE_RED);
                }
                else
                {
                    m_pTestPhone->ChangeAllLight(m_eLEDState);
                }

                m_pTestPhone->StartEchoTest(EM_ECHO_MUTE, m_bMute);
            }
        }
        break;
    case PHONE_KEY_REDIAL:
        {
            if (EM_ECHO_MIC == m_eCurrentMode)
            {
                m_eCurrentState = ES_RECORD;
                bChange = true;
            }
        }
        break;
    default:
        break;
    }
    return bChange ;
}

void CTestStateCPEcho::ChangeLightState(LED_STATE eForceState /*= LED_STATE_NONE*/)
{
    LED_STATE eState = eForceState;
    if (LED_STATE_NONE == eForceState)
    {
        m_eLEDState = m_eLEDState == LED_STATE_RED ? LED_STATE_GREEN : LED_STATE_RED;
        eState = m_eLEDState;
    }
    // 灯状态
    m_pTestPhone->ChangeAllLight(eState);
}

// 开始录音计时
void CTestStateCPEcho::StartSpeakRecordTimer()
{
    if (m_timerSpeakRecord.IsTimerRuning())
    {
        m_timerSpeakRecord.KillTimer();
    }

    m_timerSpeakRecord.SetTimer(3 * 1000, TIMER_METHOD(this, CTestStateCPEcho::OnSpeakRecordTimer));
}

// 停止录音计时
void CTestStateCPEcho::StopSpeakRecordTimer()
{
    if (m_timerSpeakRecord.IsTimerRuning())
    {
        m_timerSpeakRecord.KillTimer();
    }
}

void CTestStateCPEcho::OnSpeakRecordTimer(xThreadTimer * timer, LPVOID pExtraData)
{
    StopSpeakRecordTimer();

    // CP Echo状态与模式分开，此处判断状态
    if (ES_RECORD != m_eCurrentState)
    {
        return ;
    }

    m_eCurrentState = ES_PLAY;
    StartEchoByVPM();
    DrawScreen();
}

///////////////////////////CTestStateCPAudio///////////////////////////////////
CTestStateCPAudio::CTestStateCPAudio(CTestPhone * pTestPhone)
    : CTestState(pTestPhone)
    , m_eLEDState(LED_STATE_NONE)
    , m_uMicIndex(0)
    , m_eCurrentMode(EM_SPEEKER)
{

}

CTestStateCPAudio::~CTestStateCPAudio()
{
    BeforeExit();
}

bool CTestStateCPAudio::OnFunkeyPress(int iKey, bool bIsExpKey /*= false*/)
{
    return true;
}

bool CTestStateCPAudio::OnDigitkeyPress(int iKey)
{
    switch (iKey)
    {
    case PHONE_KEY_1:
    case PHONE_KEY_3:
    case PHONE_KEY_5:
        {
            ChangeLightState(LED_STATE_GREEN);
        }
        break;
    case PHONE_KEY_0:
    case PHONE_KEY_2:
    case PHONE_KEY_4:
    case PHONE_KEY_6:
        {
            ChangeLightState(LED_STATE_RED);
        }
        break;
    default:
        {
            return true;
        }
        break;
    }

    int nMask = 0;
    if (iKey >=  PHONE_KEY_1
            && iKey <= PHONE_KEY_3)
    {
        // 按数字键切换通道
        m_uMicIndex = iKey - PHONE_KEY_1;
        m_eCurrentMode = EM_ECHO_MIC;
    }
    else if (iKey >=  PHONE_KEY_4
             && iKey <= PHONE_KEY_6)
    {
        // 按数字键切换通道
        m_uMicIndex = iKey - PHONE_KEY_4;
        m_eCurrentMode = EM_ECHO_LINEDEVICE;
    }
    else if (PHONE_KEY_0 == iKey)
    {
        m_eCurrentMode = EM_SPEEKER;
    }

    ChangeChannel();
    DrawScreen();
    return true;
}

bool CTestStateCPAudio::DrawScreen()
{
    if (NULL == m_pTestPhone)
    {
        return false;
    }
    // 先清空屏幕
    m_pTestPhone->FillFullRect(false);

    yl::string strDisp;

    if (EM_SPEEKER == m_eCurrentMode)
    {
        // 整机测试包含SPK测试
        strDisp = commonAPI_FormatString("Audio Test\nSPK Test");
    }
    else if (EM_ECHO_MIC == m_eCurrentMode)
    {
        // 整机测试包含MIC1、2、3
        strDisp = commonAPI_FormatString("Audio Test\nMIC%d Test", m_uMicIndex + 1);
    }
    else if (EM_ECHO_LINEDEVICE == m_eCurrentMode)
    {
        // 整机测试包含MIC1、2、3的ERL测试
        strDisp = commonAPI_FormatString("Audio Test\nMIC%d - ERL Test", m_uMicIndex + 1);
    }

    testmode_DrawText(strDisp);
    return true;
}

void CTestStateCPAudio::AfterEnterState()
{
    m_eCurrentMode = EM_SPEEKER;
    ChangeLightState(LED_STATE_RED);
    if (NULL != m_pTestPhone)
    {
        // 设置最大音量
        m_pTestPhone->SetSpeakerMode(MAX_VOLUME);
    }

    ipvp_loop_test(mkGetHandle(), IPVP_LOOP_VCP);
    ChangeChannel();
    DrawScreen();
}

void CTestStateCPAudio::ChangeLightState(LED_STATE eForceState /*= LED_STATE_NONE*/)
{
    if (NULL == m_pTestPhone)
    {
        return;
    }

    if (LED_STATE_NONE != eForceState && eForceState != m_eLEDState)
    {
        m_eLEDState = eForceState;
        // 灯状态
        m_pTestPhone->ChangeAllLight(m_eLEDState);
    }
}

bool CTestStateCPAudio::ChangeChannel()
{
    int nMask = 0;
    if (EM_SPEEKER == m_eCurrentMode)
    {
        ipvp_select_mic(mkGetHandle(), IPVP_MODE_VCP, 0, nMask);
    }
    else if (EM_ECHO_MIC == m_eCurrentMode || EM_ECHO_LINEDEVICE == m_eCurrentMode)
    {
        nMask = 1 << m_uMicIndex;
        ipvp_select_mic(mkGetHandle(), IPVP_MODE_VCP, 0, nMask);
    }
    return true;
}

void CTestStateCPAudio::BeforeExit()
{
    if (NULL != m_pTestPhone)
    {
        // 设置最大音量
        m_pTestPhone->SetSpeakerMode(MID_VOLUME);
    }

    ipvp_set_mode(mkGetHandle(), IPVP_MODE_VCP);
}

///////////////////////////CTestStateFinish///////////////////////////////////
CTestStateFinish::CTestStateFinish(CTestPhone * pTestPhone)
    : CTestState(pTestPhone)
{

}

CTestStateFinish::~CTestStateFinish()
{

}

bool CTestStateFinish::OnFunkeyPress(int iKey, bool bIsExpKey /*= false*/)
{
    return true;
}

bool CTestStateFinish::DrawScreen()
{
    if (NULL == m_pTestPhone)
    {
        return false;
    }
    testmode_DrawText(TRID_TEST_LAST_STEP, TT_CONTENT_CENTER, TEST_FONT_12);
    return true;
}

void CTestStateFinish::AfterEnterState()
{
    if (NULL == m_pTestPhone)
    {
        return;
    }

    if (m_pTestPhone->GetFullTestFlag())
    {
        m_pTestPhone->SetTestModeEndFlag();
    }

    DrawScreen();
}

bool CTestStateIP::OnFunkeyPress(int iKey, bool bIsExpKey /*= false*/)
{
    if (PHONE_KEY_OK == iKey)
    {
        ChangeState(TS_FINISH);
    }
    return true;
}

CTestStateIP::CTestStateIP(CTestPhone * pTestPhone)
    : CTestState(pTestPhone)
{

}

CTestStateIP::~CTestStateIP()
{

}

bool CTestStateIP::DrawScreen()
{
    if (NULL == m_pTestPhone)
    {
        return false;
    }
    testmode_DrawText(strIp, TT_CONTENT_CENTER, TEST_FONT_12);
    return true;
}

void CTestStateIP::AfterEnterState()
{
    strIp.clear();
    netGetLocalIPv4(strIp);
    DrawScreen();
}

///////////////////////////CTestStateResult///////////////////////////////////
CTestStateResult::CTestStateResult(CTestPhone * pTestPhone)
    : CTestState(pTestPhone)
{

}

CTestStateResult::~CTestStateResult()
{

}

bool CTestStateResult::OnFunkeyPress(int iKey, bool bIsExpKey /*= false*/)
{
    return true;
}

bool CTestStateResult::DrawScreen()
{
    if (NULL == m_pTestPhone)
    {
        return false;
    }
    testmode_DrawText(m_strTestResult, TT_CONTENT_CENTER, TEST_FONT_12);
    return true;
}

void CTestStateResult::AfterEnterState()
{
    m_strTestResult.clear();
    Factory_State eFacState = commUnits_GetResetFactoryState();
    switch (eFacState)
    {
    case FACSTATE_NONE:
        {
            m_strTestResult = _LANG(TRID_TESTMODE_UNTESTED);
        }
        break;
    case FACSTATE_DISABLE:
        {
            m_strTestResult = _LANG(TRID_TESTMODE_TEST_FAIL);
        }
        break;
    case FACSTATE_ENABLE:
        {
            m_strTestResult = _LANG(TRID_TESTMODE_TEST_OK);
        }
        break;
    default:
        break;
    }
    DrawScreen();
}

///////////////////////////CTestStateCPLED///////////////////////////////////
CTestStateCPLED::CTestStateCPLED(CTestPhone * pTestPhone)
    : CTestStateLED(pTestPhone)
{

}

CTestStateCPLED::~CTestStateCPLED()
{
    if (NULL != m_pTestPhone)
    {
        m_pTestPhone->ChangeAllLight(LED_STATE_FLASH);
    }
}

bool CTestStateCPLED::OnFunkeyPress(int iKey, bool bIsExpKey/* = false*/)
{
    if (NULL == m_pTestPhone)
    {
        return true;
    }

    if (m_pTestPhone->IsNextStateKeyPress(iKey))
    {
        ChangeState(TS_ECHO);
    }
#ifdef _CP920
    else if (PHONE_KEY_VOLUME_DECREASE == iKey || PHONE_KEY_VOLUME_INCREASE == iKey)
    {
        m_pTestPhone->ChangeAllLight(LED_STATE_RED);
    }
    else if (PHONE_KEY_MUTE == iKey)
    {
        int nRowKey = 0;

#if IF_INPUT_HOOK_KEEP_KEY_RAW_VALUE
        nRowKey = inputHook_GetCurrentRawKey();
#endif

        if (PhoneDev_IsKeyMute3(nRowKey))
        {
            ChangeState(TS_USB);
        }
        else
        {
            m_pTestPhone->ChangeAllLight(LED_STATE_GREEN);
        }
    }
#endif
    return true;
}

#ifdef _CP920
bool CTestStateCPLED::DrawScreen()
{
    if (NULL == m_pTestPhone)
    {
        return false;
    }

    yl::string strTitle = _LANG(TRID_LED_TEST);
    testmode_DrawText(strTitle, TT_TITLE, TEST_FONT_12);
    yl::string strNote = _LANG(TRID_PLEASE_INPUT_VOLUME_KEY_AND_LEFT_MUTE_KEY_TO_CHANGE);
    strNote += _LANG(TRID_OR_PRESS_THE_MUTE_KEY_ON_THE_RIGHT_TO_NEXT_STEP);
    testmode_DrawText(strNote, TT_CONTENT_NORMAL, TEST_FONT_10);
    return true;
}
#endif

#endif

#ifdef IF_USB_SUPPORT
///////////////////////////CTestStateUSB///////////////////////////////////
CTestStateUSB::CTestStateUSB(CTestPhone * pTestPhone)
    : CTestState(pTestPhone)
    , m_bUSBCheckSuccess(false)
{
}

CTestStateUSB::~CTestStateUSB()
{
#ifdef _CP920
    m_timerAutoTestNext.KillTimer();
#endif
}

bool CTestStateUSB::OnFunkeyPress(int iKey, bool bIsExpKey /*= false*/)
{
    if (NULL == m_pTestPhone)
    {
        return false;
    }

    if (m_pTestPhone->IsNextStateKeyPress(iKey) && IsUSBCheckSuccess())
    {
#ifdef _CP920
        ChangeState(TS_ECHO);
#else
        ChangeState(TS_KEY);
#endif
        return true;
    }

#ifndef _CP
    TEST_STATE eState = m_pTestPhone->MapStateOfKey(iKey, bIsExpKey);
    if (TS_EXIT == eState)
    {
        ChangeState(eState);
    }
#endif

    return true;
}

bool CTestStateUSB::DrawScreen()
{
    if (NULL == m_pTestPhone)
    {
        return false;
    }

    xColor color(0x00, 0x00, 0x00);
    if (0 == m_strDisp.compare("USB OK"))
    {
        color = xColor(0x00, 0xff, 0x00);
    }
    else if (0 == m_strDisp.compare("USB failed"))
    {
        color = xColor(0xff, 0x00, 0x00);
    }

    m_pTestPhone->DrawContentString(_LANG(m_strDisp).c_str(), color);
    return true;
}

void CTestStateUSB::SetUSBCheckStatus(bool bStatus)
{
    if (m_bUSBCheckSuccess != bStatus)
    {
        m_bUSBCheckSuccess = bStatus;
#ifdef _CP920
        m_timerAutoTestNext.KillTimer();
        if (m_bUSBCheckSuccess)
        {
            m_timerAutoTestNext.SingleShot(1000, TIMER_METHOD(this, CTestStateUSB::OnCheckSuccessed));
        }
#endif
    }
}

void CTestStateUSB::AfterEnterState()
{
    // 一进入USB测试界面就检测USB状态
    SetUSBCheckStatus(modRecord_IsStorageConnect());
    if (IsUSBCheckSuccess())
    {
        m_strDisp = "USB OK";
    }
    else
    {
        m_strDisp = "Input USB store!";
    }

    DrawScreen();
}

#ifdef _CP920
void CTestStateUSB::OnCheckSuccessed(xThreadTimer * timer, LPVOID pExtraData)
{
    ChangeState(TS_ECHO);
}
#endif

void CTestStateUSB::OnUSBStateChange(bool bIn)
{
    // 检测成功过即可
    if (!IsUSBCheckSuccess())
    {
        SetUSBCheckStatus(bIn);
    }

    if (bIn)
    {
        m_strDisp = "USB OK";
    }
    else
    {
        m_strDisp = "USB failed";
    }
    DrawScreen();
}
#endif  //IF_USB_RECORD

#ifdef IF_SUPPORT_WIFI
///////////////////////////CTestStateWifi///////////////////////////////////
CTestStateWifi::CTestStateWifi(CTestPhone * pTestPhone)
    : CTestState(pTestPhone)
    , m_bOriginWifiEnable(false)
{

}

CTestStateWifi::~CTestStateWifi()
{
    if (WIFI_IsScan())
    {
        WIFI_StopScan();
    }
    ResumeWifiStatus();
    m_timerAutoTestNext.KillTimer();
}

// 进入该状态之后的操作
void CTestStateWifi::AfterEnterState()
{
    m_bOriginWifiEnable = WIFI_IsEnable();

    m_strDisp = _LANG("Wifi test");
    if (!m_bOriginWifiEnable)
    {
        WIFI_SetEnable(true);
    }
    else
    {
        WIFI_StartScan();

        m_strDisp = _LANG("Wifi scan");
    }

    // 画Wifi界面
    DrawScreen();
}

bool CTestStateWifi::OnFunkeyPress(int iKey, bool bIsExpKey /*= false*/)
{
    return true;
}

bool CTestStateWifi::DrawScreen()
{
    testmode_DrawText(m_strDisp);
    return true;
}

bool CTestStateWifi::OnMessage(msgObject & msg)
{
    if (WIFI_MSG_UPDATE_UI != msg.message)
    {
        return false;
    }

    if (OP_WIFI_OPEN_RESULT == msg.wParam
            && WIRELESS_OP_OPEN_RESULT_SUCCESS == msg.lParam)
    {
        WIFI_StartScan();

        m_strDisp = _LANG("Wifi scan");
        DrawScreen();
    }
    else if (OP_SCAN_WIFI_RESULT == msg.wParam)
    {
        bool bWifiTestOK = false;
        if (WIRELESS_OP_SCAN_RESULT_SUCCESS == msg.lParam)
        {
            UI_WIFI_NETINFO_LIST listNetInfo;
            bool bSucceed = WIFI_GetScanNetworkList(listNetInfo);
            TESTMODE_INFO("GetScanNetworkList : bSucceed [%d], listNetInfo size is [%d].", bSucceed,
                          listNetInfo.size());
            if (bSucceed && listNetInfo.size() > 0)
            {
                WiFiNetInfoItem it = listNetInfo.begin();
                for (; it != listNetInfo.end(); ++it)
                {
                    const WiFiNetInfo & stNetInfo = (*it);
                    TESTMODE_INFO("wifi signal:%d.", stNetInfo.iBaseSignal);
                    if (stNetInfo.iBaseSignal > WIFI_HIGH_SIGNAL)
                    {
                        bWifiTestOK = true;
                        break;
                    }
                }
            }
        }

        if (bWifiTestOK)
        {
            m_strDisp = _LANG("Wifi signal ok");
        }
        else
        {
            m_strDisp = _LANG("Wifi singnal fail");
        }
        DrawScreen();

        if (bWifiTestOK)
        {

            m_timerAutoTestNext.SingleShot(1000, TIMER_METHOD(this, CTestStateWifi::OnWifiSignalOkSuccessed));
        }
        else
        {
            // 检测完成，恢复原始状态
            ResumeWifiStatus();
        }
    }
    return true;
}

void CTestStateWifi::ResumeWifiStatus()
{
    if (!m_bOriginWifiEnable && WIFI_IsEnable())
    {
        WIFI_SetEnable(false);
    }
}

void CTestStateWifi::OnWifiSignalOkSuccessed(xThreadTimer * timer, LPVOID pExtraData)
{
    ResumeWifiStatus();
}

#endif // IF_SUPPORT_WIFI

///////////////////////////CTestStateRPS///////////////////////////////////
CTestStateRPS::CTestStateRPS(CTestPhone * pTestPhone) : CTestState(pTestPhone)
{

}

CTestStateRPS::~CTestStateRPS()
{

}

bool CTestStateRPS::OnFunkeyPress(int iKey, bool bIsExpKey/* = false*/)
{
    if (NULL == m_pTestPhone)
    {
        return false;
    }

    TEST_STATE eState = m_pTestPhone->MapStateOfKey(iKey, bIsExpKey);
    if (TS_EXIT == eState)
    {
        ChangeState(eState);
    }
#if IS_COLOR
    else if (TS_COLOR == eState)
    {
        ChangeState(TS_COLOR_LCD);
    }
#endif
    else if (TS_RANDNUM == eState)
    {
#ifdef _CP920
        ChangeState(TS_AUTO_TEST_KEY);
#else
        ChangeState(TS_INIT);
#endif
    }

    return true;
}

bool CTestStateRPS::DrawScreen()
{
    if (NULL == m_pTestPhone)
    {
        return false;
    }

    // 随机数
    char buf[128];
    memset(buf, 0, 128);
    int ret = sys_get_env("randomness", buf, 127);

    yl::string strRandNum = buf;
    if (ret != 0 || strlen(buf) < 17) //容错
    {
        strRandNum = "00000000000000000";
        TESTMODE_INFO("there is no randomness\n");
    }

    // 证书状态
    yl::string strCert;
    ret = sys_get_env("certalgotithm", buf, 127);
    strCert = (ret == 0) ? buf : "0";
    strCert += "/";

    ret = sys_get_env("certbit", buf, 127);
    strCert += (ret == 0) ? buf : "NA";

    // RPS状态
    bool bRpsStatus = (1 == configParser_GetConfigInt(kszRedirectEnable));

    // 填充背景为白色
    xColor colWhite = xColor(0xff, 0xff, 0xff);
    testmode_FillRect(AT_ALL, colWhite);

    // 画文字
    m_pTestPhone->DrawRpsString(bRpsStatus, strRandNum.c_str(), strCert.c_str());

    return true;
}

// 进入该状态之后的操作
void CTestStateRPS::AfterEnterState()
{
    // 背光灯常亮
    msgPostMsgToThread(msgGetThreadByName(VP_PHONE_THREAD_NAME), DSK_MSG_LIGHT_STATUS_CHANGE, 1, 1);

    // 画RPS界面
    DrawScreen();
}

///////////////////////////CTestStateView///////////////////////////////////
CTestStateView::CTestStateView(CTestPhone * pTestPhone) : CTestState(pTestPhone)
{

}

CTestStateView::~CTestStateView()
{

}

bool CTestStateView::OnFunkeyPress(int iKey, bool bIsExpKey/* = false*/)
{
    if (NULL == m_pTestPhone)
    {
        return false;
    }

    if (TS_EXIT == m_pTestPhone->MapStateOfKey(iKey, bIsExpKey))
    {
        ChangeState(TS_EXIT);
    }

    return true;
}

bool CTestStateView::DrawScreen()
{
    if (NULL == m_pTestPhone)
    {
        return false;
    }

    // 获取RPS的内容
    yl::string strRpsStatus = "RPS Closed";
    if (1 == configParser_GetConfigInt(kszRedirectEnable))
    {
        strRpsStatus = "RPS Started";
    }

    strRpsStatus = "RPS Status: " + strRpsStatus;

    // 获取恢复出厂的状态
    yl::string strFactoryStatus = commUnits_GetUserEnvString(kszStatusReset);
    strFactoryStatus = strFactoryStatus.empty() ? "No Restored" : "Restored";
    strFactoryStatus = "Factory Status: " + strFactoryStatus;

    yl::string strDisplay = strRpsStatus + "\n \n" + strFactoryStatus;
    m_pTestPhone->DrawStatusViewString(strDisplay.c_str());

    return true;
}

// 进入该状态之后的操作
void CTestStateView::AfterEnterState()
{
    // 背光灯常亮
    msgPostMsgToThread(msgGetThreadByName(VP_PHONE_THREAD_NAME), DSK_MSG_LIGHT_STATUS_CHANGE, 1, 1);

    DrawScreen();
}

#if IS_COLOR
///////////////////////////CTestStateColorLCD///////////////////////////////////
CTestStateColorLCD::CTestStateColorLCD(CTestPhone * pTestPhone): CTestState(pTestPhone)
{
    m_eCurrentStep = LCD_RED;
    m_bLightLevelUp = true;
    m_iLCDLevel = 0;
}

CTestStateColorLCD::~CTestStateColorLCD()
{
    m_xTimer.KillTimer();

    if (NULL != m_pTestPhone)
    {
        m_pTestPhone->AllLightOff();
    }

    testmode_FillRect(AT_ALL, xColor(0xff, 0xff, 0xff));
}

bool CTestStateColorLCD::OnFunkeyPress(int iKey, bool bIsExpKey /*= false*/)
{
    if (NULL == m_pTestPhone)
    {
        return false;
    }

    TEST_STATE eState = m_pTestPhone->MapStateOfKey(iKey, bIsExpKey);
    if (TS_EXIT == eState)
    {
        ChangeState(eState);
    }
    else if ((TS_ECHO == eState || TS_EXP_LIGHT == eState)
             && !m_pTestPhone->GetISSFP())
    {
        ChangeState(eState);
    }
    else if (m_pTestPhone->IsNextStateKeyPress(iKey))
    {
        if (m_eCurrentStep < (LCD_END - 1))
        {
            m_eCurrentStep = (LCD_COLOR_STEP)(m_eCurrentStep + 1);
            DrawScreen();
        }
        else if (m_pTestPhone->GetISSFP())
        {
            ChangeState(TS_LED);
        }
        else if (DEVICELIB_IS_T46(devicelib_GetPhoneType()))
        {
            ChangeState(TS_USB);
        }
        else
        {
            ChangeState(TS_BLUETOOTH);
        }
    }

    return true;
}

bool CTestStateColorLCD::DrawScreen()
{
    yl::string strPicture;

    switch (m_eCurrentStep)
    {
    case LCD_RED:
        {
            testmode_FillRect(AT_ALL, xColor(0xff, 0x0, 0x0));
        }
        break;
    case LCD_GREEN:
        {
            testmode_FillRect(AT_ALL, xColor(0x0, 0xff, 0x0));
        }
        break;
    case LCD_BLUE:
        {
            testmode_FillRect(AT_ALL, xColor(0x0, 0x0, 0xff));
        }
        break;
    case LCD_GRID:
        {
            strPicture = PIC_TESTMODE_GRID;
        }
        break;
    case LCD_GRAY80:
        {
            strPicture = PIC_TESTMODE_GRAY80;
        }
        break;
    case LCD_GRAY50:
        {
            strPicture = PIC_TESTMODE_GRAY50;
        }
        break;
    case LCD_GRAY30:
        {
            strPicture = PIC_TESTMODE_GRAY30;
        }
        break;
    case LCD_FLICK:
        {
            strPicture = PIC_TESTMODE_FLICK;
        }
        break;
    case LCD_GRADIENT:
        {
            strPicture = PIC_TESTMODE_GRADIENT;
        }
        break;
    case LCD_WHITESCREEN:
        {
            testmode_FillRect(AT_ALL, xColor(0xff, 0xff, 0xff));
        }
        break;
    default:
        break;
    }

    if (!strPicture.empty())
    {
        m_pTestPhone->FillPicture(strPicture);
    }

    return true;
}

void CTestStateColorLCD::AfterEnterState()
{
    TESTMODE_INFO("CTestStateLCD::AfterEnterState()");
    devicelib_SetBackLightLevel(100);

    DrawScreen();

    TESTMODE_INFO(" StartLCDTest set timer");

    m_xTimer.SetTimer(100, TIMER_METHOD(this, CTestStateColorLCD::OnTimer));
}

void CTestStateColorLCD::OnTimer(xThreadTimer * timer, LPVOID pExtraData)
{
    devicelib_SetBackLightLevel(m_iLCDLevel);
    m_iLCDLevel = m_bLightLevelUp ? ((m_iLCDLevel + 2) % 101) : ((m_iLCDLevel - 2) % 101);
    m_bLightLevelUp = (m_iLCDLevel == 100) ? false : ((m_iLCDLevel == 0) ? true : m_bLightLevelUp);
}

///////////////////////////CTestStateBlueTooth/////////////////////////////
CTestStateBlueTooth::CTestStateBlueTooth(CTestPhone * pTestPhone) : CTestState(pTestPhone)
{

}

CTestStateBlueTooth::~CTestStateBlueTooth()
{

}

bool CTestStateBlueTooth::DrawScreen()
{
    if (NULL == m_pTestPhone)
    {
        return false;
    }

    m_pTestPhone->DrawContentString("Blue Tooth");
    return true;
}

void CTestStateBlueTooth::AfterEnterState()
{
    DrawScreen();
    SettingUI_EnterPage(kszPageIDBluetoothMenu);
}

bool CTestStateBlueTooth::OnMessage(msgObject & msg)
{
    if (BLUETOOTH_MSG_CONNECT_STATE != msg.message)
    {
        return false;
    }

    if (msg.wParam && OP_DISCONNECT_RESULT == msg.lParam)
    {
        // 切换声道到蓝牙耳机
        ipvp_set_mode(mkGetHandle(), IPVP_MODE_BTES);
        ipvp_set_volume(mkGetHandle(), 10);

        // 播放音乐
        ipvp_ring_play(mkGetHandle(), DEFAULT_RING_FILE, -1);

        Bluetooth_HSStartPlay();
    }

    return true;
}

bool CTestStateBlueTooth::OnFunkeyPress(int iKey, bool bIsExpKey /* = false */)
{
    if (TS_EXIT == m_pTestPhone->MapStateOfKey(iKey, bIsExpKey))
    {
        UIManager_ReturnToIdle();
        ChangeState(TS_EXIT);
    }
    else if (m_pTestPhone->IsNextStateKeyPress(iKey))
    {
        UIManager_ReturnToIdle();
        ChangeState(TS_USB);

        ipvp_ring_stop(mkGetHandle());
        Bluetooth_DeleteDevice();
    }

    return  true;
}
#endif //IS_COLOR
