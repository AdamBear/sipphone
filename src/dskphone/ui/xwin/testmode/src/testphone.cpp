#include "testphone.h"
#include "uimanager/moduimanager.h"
#include "uimanager/autovolumebar.h"
#include "modtestmode.h"
#include "testmodeinterface.h"
#include "voice/include/modvoice.h"
#include "commonunits/modcommonunits.h"
#include "extended/wifimessagedefine.h"
#include "extended/bluetoothmessagedefine.h"

#ifdef IF_USB_SUPPORT
#include "record/include/recordmessagedefine.h"

// USB插拔
static LRESULT TestModeOnUsbChange(msgObject & msg)
{
    CTestPhone * pTestPhone = testmode_GetCurrentTestPhone();
    if (NULL != pTestPhone)
    {
        if (RECORD_STORAGE_STATE_CHANGE == msg.message)
        {
            if (0 == msg.wParam)
            {
                pTestPhone->OnUSBStateChange(false);
            }
            else if (1 == msg.wParam)
            {
                pTestPhone->OnUSBStateChange(true);
            }
        }
    }
    return TRUE;
}
#endif

//模拟原有的扩展台拔插按键
static LRESULT TestModeOnExpChange(msgObject & msg)
{
    BOOL bHandle = TRUE;
    CTestPhone * pTestPhone = testmode_GetCurrentTestPhone();
    if (NULL != pTestPhone)
    {
        switch (msg.message)
        {
        case DEV_MSG_EXP_INSERT:
        case DEV_MSG_EXP_REMOVE:
            {
                int iEXPIndex = msg.wParam;
                int key = 0;
                if (DEV_MSG_EXP_INSERT == msg.message)
                {
                    key  = (iEXPIndex << 6) | EXP_INSERT;
                }
                else if (DEV_MSG_EXP_REMOVE == msg.message)
                {
                    key = (iEXPIndex << 6) | EXP_REMOVE;
                }
                else
                {
                    return FALSE;
                }
                pTestPhone->OnFunckeyPress(key, true);
            }
            break;
        case DEV_MSG_EHS_INSERT:
            {
                pTestPhone->OnEHSInsert();
            }
            break;
        case DEV_MSG_EHS_REMOVE:
            {
                pTestPhone->OnEHSRemove();
            }
            break;
#ifdef IF_SUPPORT_WIFI
        case WIFI_MSG_UPDATE_UI:
            {
                pTestPhone->OnMessage(msg);
            }
            break;
#endif
#ifdef IS_COLOR
        case BLUETOOTH_MSG_CONNECT_STATE:
            {
                pTestPhone->OnMessage(msg);
            }
            break;
#endif
        default:
            {
                bHandle = FALSE;
            }
            break;
        }
    }
    return bHandle;
}

///////////////////////////CTestPhone///////////////////////////////////
CTestPhone::CTestPhone()
#ifdef _CP
    : m_bFullFlag(true)
#endif
{
    m_pTestState = NULL;
    m_iLedStatus = 0;
    chASSERT(m_objIOMutex.OpenMutex("WatchDogLightIO"));

    m_pTestEXP = NULL;
    m_bHasEXP = false;
    m_bHasLed = true;

    m_pVolumeBar = NULL;
    m_iCurrentVolume = 8;

    m_bSkipEHSTest = true;

    m_bIsEHSInserd = (-1 != exp_GetEhsIndex());
    m_eEchoMode = EM_ECHO_MIC;
    m_iTestMethod = TST_NONE;

    etl_RegisterMsgHandle(DEV_MSG_EXP_INSERT, DEV_MSG_EXP_REMOVE, TestModeOnExpChange);
    etl_RegisterMsgHandle(DEV_MSG_EHS_INSERT, DEV_MSG_EHS_REMOVE, TestModeOnExpChange);

#ifdef IF_USB_SUPPORT
    etl_RegisterMsgHandle(RECORD_STORAGE_STATE_CHANGE, RECORD_STORAGE_STATE_CHANGE,
                          TestModeOnUsbChange);
#endif

#ifdef IF_SUPPORT_WIFI
    etl_RegisterMsgHandle(WIFI_MSG_UPDATE_UI, WIFI_MSG_UPDATE_UI, TestModeOnExpChange);
#endif

#if IS_COLOR
    etl_RegisterMsgHandle(BLUETOOTH_MSG_CONNECT_STATE, BLUETOOTH_MSG_CONNECT_STATE, TestModeOnExpChange);
#endif
}

CTestPhone::~CTestPhone()
{
    if (NULL != m_pTestState)
    {
        delete m_pTestState;
        m_pTestState = NULL;
    }

    if (NULL != m_pVolumeBar)
    {
        delete m_pVolumeBar;
        m_pVolumeBar = NULL;
    }

    if (NULL != m_pTestEXP)
    {
        delete m_pTestEXP;
        m_pTestEXP = NULL;
    }

    m_objIOMutex.CloseMutex();

#if IF_BUG_20900
    m_timerDelayStartMacd.KillTimer();
    // http://10.2.1.199/Bug.php?BugID=40445
    if (!commonUnit_IsMatchTestMode())
    {
        commonUnits_System("killall macd");
    }
#endif  //IF_BUG_20900

    etl_UnregisterMsgHandle(DEV_MSG_EXP_INSERT, DEV_MSG_EXP_REMOVE, TestModeOnExpChange);
    etl_UnregisterMsgHandle(DEV_MSG_EHS_INSERT, DEV_MSG_EHS_REMOVE, TestModeOnExpChange);

#ifdef IF_USB_SUPPORT
    etl_UnregisterMsgHandle(RECORD_STORAGE_STATE_CHANGE, RECORD_STORAGE_STATE_CHANGE,
                            TestModeOnUsbChange);
#endif

#ifdef IF_SUPPORT_WIFI
    etl_UnregisterMsgHandle(WIFI_MSG_UPDATE_UI, WIFI_MSG_UPDATE_UI, TestModeOnExpChange);
#endif

#if IS_COLOR
    etl_UnregisterMsgHandle(BLUETOOTH_MSG_CONNECT_STATE, BLUETOOTH_MSG_CONNECT_STATE, TestModeOnExpChange);
#endif

    // 通知外部testmode结束
    msgPostMsgToThread(msgGetThreadByName(VP_PHONE_THREAD_NAME), TESTMODE_MESSAGE_EXIT, 0, 0);
}

#ifdef _CP
void CTestPhone::CheckFlagPalyTone()
{
    Factory_State eFacState = commUnits_GetResetFactoryState();
    //0,代表恢复出厂的脚步执行完毕
    yl::string strResetFlag = commUnits_GetUserEnvString("resetflag");
    int resetflag = -1;
    if (!strResetFlag.empty())
    {
        yl::string strLeft = "resetflag=";
        strResetFlag.trim_left(strLeft);
        resetflag = atoi(strResetFlag.c_str());
    }
    //测试fail 响铃或者测试ok但是未恢复出厂响铃
    if (FACSTATE_DISABLE == eFacState || (0 != resetflag && FACSTATE_ENABLE == eFacState))
    {
        ipvp_set_mode(mkGetHandle(), IPVP_MODE_HF);
        ipvp_set_volume(mkGetHandle(), 8);
#ifndef _WIN32
#warning ipvp_2
#endif
//      ipvp_ring_play(mkGetHandle(), DEFAULT_RING_FILE, -1);
    }
}

void CTestPhone::TestResultInit()
{
#if IF_BUG_20900
    m_timerDelayStartMacd.SetTimer(300, MSG_METHOD(this, CTestPhone::OnDelayStartMacdTimer));
#endif  //IF_BUG_20900

    InitLight();
    ChangeState(new CTestStateResult(this));
}

void CTestPhone::SetFullTestFlag(bool bFull)
{
    m_bFullFlag = bFull;
}

#endif

void CTestPhone::TestInitial()
{
    TESTMODE_INFO("******* CTestPhone::TestInitial begin");
#if IF_BUG_20900
    m_timerDelayStartMacd.SetTimer(300, MSG_METHOD(this, CTestPhone::OnDelayStartMacdTimer));
#endif  //IF_BUG_20900

    InitLight();
    if (m_bHasEXP)
    {
        InitEXPModule();
        UpdateTestEXP();
    }
    InitCmpString();
#ifdef _CP
    CheckFlagPalyTone();
#endif
    ChangeState(new CTestStateRPS(this));
}

VOID CTestPhone::TestSFPInitial()
{
    InitLight();
    ChangeState(new CTestStateInit(this));
}

void CTestPhone::TestFullVoiceInit()
{
    ChangeState(new CTestStateAudio(this));
}

void CTestPhone::TestPreburningInit()
{
    TestInitial();
}

void CTestPhone::TestManaualKeyInit()
{
#if IF_BUG_20900
    m_timerDelayStartMacd.SetTimer(300, MSG_METHOD(this, CTestPhone::OnDelayStartMacdTimer));
#endif  //IF_BUG_20900

    InitLight();
    InitCmpString();
    ChangeState(new CTestStateKey(this));
}

#ifdef IF_SUPPORT_WIFI
void CTestPhone::TestWifiInit()
{
#if IF_BUG_20900
    m_timerDelayStartMacd.SetTimer(300, MSG_METHOD(this, CTestPhone::OnDelayStartMacdTimer));
#endif  //IF_BUG_20900

    InitLight();
    InitCmpString();
    ChangeState(new CTestStateWifi(this));
}
#endif

void CTestPhone::Show()
{
    if (NULL != m_pTestState)
    {
        m_pTestState->DrawScreen();
    }
}

bool CTestPhone::OnKeyRelease()
{
    return true;
}

bool CTestPhone::OnFunckeyPress(int iKey, bool bEXP /*= false*/)
{
    if (PHONE_KEY_HANDSET_OFF_HOOK == iKey
            || PHONE_KEY_HANDSET_ON_HOOK == iKey)
    {
        // 这些按键不播放按键音
    }
    else
    {
        PlayLocalDTMF(PHONE_KEY_A);
    }

    if (m_bHasEXP && bEXP)
    {
        if (EXPKeyPress(iKey))
        {
            return true;
        }
    }

    if (NULL != m_pTestState)
    {
        m_pTestState->OnFunkeyPress(iKey, bEXP);
    }

    return true;
}

bool CTestPhone::OnDigitkeyPress(int iKey)
{
    // 播放按键音
    if (iKey >= '0' && iKey <= '9')
    {
        PlayLocalDTMF(iKey);
    }
    else if ('*' == iKey)
    {
        PlayLocalDTMF(0x0e);
    }
    else if ('#' == iKey)
    {
        PlayLocalDTMF(0x0f);
    }

    return (NULL != m_pTestState && m_pTestState->OnDigitkeyPress(iKey));
}

#if JOYCE_TEL_BUGFREE_19348
void CTestPhone::DrawSN(LPCSTR lpszSN)
{
    if (NULL == lpszSN)
    {
        return;
    }

    yl::string strDisp = "SN:";
    strDisp.append(lpszSN);

    testmode_DrawText(strDisp, true);
}
#endif

void CTestPhone::DrawContentString(LPCTSTR lpszTest, xColor color/* = xColor(0x00, 0x00, 0x00)*/)
{
    testmode_DrawText(lpszTest);
}

void CTestPhone::FillFullRect(bool bBlack)
{
    xColor col;
    if (bBlack)
    {
        col = xColor(0x00, 0x00, 0x00);
    }
    else
    {
        col = xColor(0xff, 0xff, 0xff);
    }

    testmode_FillRect(AT_ALL, col);
}

void CTestPhone::ChangeState(CTestState * pTestState)
{
    if (NULL != m_pTestState)
    {
        delete m_pTestState;
        m_pTestState = NULL;
    }

    m_pTestState = pTestState;

    // 状态切换之后的操作
    if (NULL != m_pTestState)
    {
        m_pTestState->AfterEnterState();
    }
}

bool CTestPhone::ChangeEchoState(int iKey, ECHO_MODE & eMode)
{
    if (PHONE_KEY_HEADSET_CTRL != iKey
            && PHONE_KEY_HANDSET_OFF_HOOK != iKey
            && PHONE_KEY_MESSAGE != iKey
            && PHONE_KEY_HANDFREE != iKey)
    {
        return false;
    }

    if (PHONE_KEY_HANDFREE == iKey)
    {
        // EHS回音测试，按免提进入免提回音测试
        if (EM_EHS == eMode)
        {
            eMode = EM_SPEAKER_RECORD;
            return true;
        }
    }
    else if (PHONE_KEY_HEADSET_CTRL == iKey)
    {
        if (EM_SPEAKER_RECORD == eMode || EM_SPEAKER_PLAY == eMode)
        {
            eMode = EM_HEADSET;
            return true;
        }
    }
    else if (PHONE_KEY_HANDSET_OFF_HOOK == iKey)
    {
        if (EM_HEADSET == eMode)
        {
            eMode = EM_HANDSET;
            return true;
        }
    }

    return false;
}

void CTestPhone::ExitEchoTest()
{
    ipvp_echo_test(mkGetHandle(), IPVP_ECHO_STOP);//清空之前vpm录制缓存。
    ipvp_set_mode(mkGetHandle(), IPVP_MODE_HS);

    AllLightOff();
}

LPCSTR CTestPhone::GetKeyString(int iIndex)
{
    if (iIndex < 0 || iIndex >= (int)m_uMaxKeyNum)
    {
        return "";
    }

    return m_strKey[iIndex].c_str();
}

bool CTestPhone::IsLightOn(int iIndex)
{
    return (m_iLedStatus & iIndex);
}

yl::string CTestPhone::GetCorrectString(int iIndex)
{
    if (iIndex < 0 || iIndex >= (int)m_uMaxKeyNum)
    {
        return "";
    }

    yl::string strRetVal = "";
    for (int i = 0; i < iIndex; ++i)
    {
        strRetVal = strRetVal + m_strKey[i] + " ";
    }

    return strRetVal;
}

//////////////////////////////For EXPModule/////////////////////////////////////
void CTestPhone::InitEXPModule()
{
    m_uEXPNum = 0;
    m_eEXPType = DT_INVALID;

#ifndef WIN32
    m_objIOMutex.Lock();
    m_eEXPType = exp_GetExpTypeByIndex(0);
    int iNum = exp_GetExpCounts(m_eEXPType);
    m_objIOMutex.Unlock();

    if (iNum > 0 && m_eEXPType != DT_INVALID)
    {
        for (int i = 1; i < iNum; ++i)
        {
            if (GetEXPTypeFromDriver(i) != m_eEXPType)
            {
                m_uEXPNum = i;
                return ;
            }
        }

        m_uEXPNum = iNum;
    }
#endif
}

void CTestPhone::UpdateTestEXP()
{
    if (NULL != m_pTestEXP)
    {
        delete m_pTestEXP;
        m_pTestEXP = NULL;
    }

    if (m_uEXPNum > 0)
    {
        switch (m_eEXPType)
        {
        case ET_EXP38:
            {
                m_pTestEXP = new CTestEXP38(m_uEXPNum);
            }
            break;
        case ET_EXP39:
        case ET_EXP20:
            {
                m_pTestEXP = new CTestEXP39(m_uEXPNum);
            }
            break;
        case ET_EXP40:
            {
                m_pTestEXP = new CTestEXP40(m_uEXPNum);
            }
            break;
        case ET_EXP_CM69:
            {
                m_pTestEXP = new CTestCM69(m_uEXPNum);
            }
            break;
#ifdef _EXP_COLOR_SUPPORT
        case ET_EXP50:
            {
                m_pTestEXP = new CTestEXP50(m_uEXPNum);
            }
            break;
#endif
        default:
            break;
        }
    }
}

// 返回true表示已处理过，不需要再传给TestState处理
bool CTestPhone::EXPKeyPress(int iKey)
{
    int iEXPIndex = GetEXPIndex(iKey);

    if (EXP_INSERT == GetEXPKeyIndex(iKey))
    {
        if (iEXPIndex > 1 || iEXPIndex < (int)m_uEXPNum - 1)
        {
            // 只处理2台扩展台
            TESTMODE_WARN("EXP Type is invalid[%d]!!!", iEXPIndex);
            return true;
        }

        // 重新初始化m_pTestEXP
        InitEXPModule();
        UpdateTestEXP();

        if (m_uEXPNum < 2)
        {
            // 插上的台数少于2台，则不处理
            TESTMODE_WARN("EXP Type isn't enough!!!");
            return true;
        }

        if (DT_INVALID == m_eEXPType)
        {
            //第一台扩展台和第二台扩展台类型不一致
            TESTMODE_WARN("EXP Type is incongruous!!!");
            return true;
        }
    }
    else if (EXP_REMOVE == GetEXPKeyIndex(iKey))
    {
        m_uEXPNum = iEXPIndex;
        UpdateTestEXP();
    }

    return false;
}

DEVICE_TYPE_DEF CTestPhone::GetEXPTypeFromDriver(int iEXPIndex)
{
    return exp_GetExpTypeByIndex(0);
}

//　播按键音
void CTestPhone::PlayLocalDTMF(int iKey)
{
    BYTE iDtmf = 0;
    switch (iKey)
    {
    case PHONE_KEY_0:
        iDtmf = IPVP_DTMF_0;
        break;
    case PHONE_KEY_1:
        iDtmf = IPVP_DTMF_1;
        break;
    case PHONE_KEY_2:
        iDtmf = IPVP_DTMF_2;
        break;
    case PHONE_KEY_3:
        iDtmf = IPVP_DTMF_3;
        break;
    case PHONE_KEY_4:
        iDtmf = IPVP_DTMF_4;
        break;
    case PHONE_KEY_5:
        iDtmf = IPVP_DTMF_5;
        break;
    case PHONE_KEY_6:
        iDtmf = IPVP_DTMF_6;
        break;
    case PHONE_KEY_7:
        iDtmf = IPVP_DTMF_7;
        break;
    case PHONE_KEY_8:
        iDtmf = IPVP_DTMF_8;
        break;
    case PHONE_KEY_9:
        iDtmf = IPVP_DTMF_9;
        break;
    case PHONE_KEY_A:
        iDtmf = IPVP_DTMF_A;
        break;
    case PHONE_KEY_B:
        iDtmf = IPVP_DTMF_B;
        break;
    case PHONE_KEY_C:
        iDtmf = IPVP_DTMF_C;
        break;
    case PHONE_KEY_D:
        iDtmf = IPVP_DTMF_D;
        break;
    case PHONE_KEY_STAR:
        iDtmf = IPVP_DTMF_STAR;
        break;
    case PHONE_KEY_POUND:
        iDtmf = IPVP_DTMF_POUND;
        break;
    default:
        iDtmf = IPVP_DTMF_A;
        break;
    }

    ipvp_dtmf_play(mkGetHandle(), iDtmf, 0);;
}

void CTestPhone::StartEchoTest(ECHO_MODE eMode, UINT iExtData/* = 0*/)
{
    switch (eMode)
    {
    case EM_SPEAKER_RECORD:
        {
            ipvp_echo_test(mkGetHandle(), IPVP_ECHO_STOP);//清空之前vpm录制缓存。
            ipvp_set_mode(mkGetHandle(), IPVP_MODE_HF);
            ipvp_echo_test(mkGetHandle(), IPVP_ECHO_RECORD);
            ipvp_set_volume(mkGetHandle(), MID_VOLUME);
        }
        break;
    case EM_SPEAKER_PLAY:
        {
            ipvp_echo_test(mkGetHandle(), IPVP_ECHO_PLAY);
            ipvp_set_volume(mkGetHandle(), m_iCurrentVolume);
        }
        break;
    case EM_EHS: // EHS回音测试使用的声道是耳麦声道
    case EM_HEADSET:
        {
            ipvp_echo_test(mkGetHandle(), IPVP_ECHO_STOP);//清空之前vpm录制缓存。
            ipvp_set_mode(mkGetHandle(), IPVP_MODE_ES);

            ipvp_echo_test(mkGetHandle(), IPVP_ECHO_LOOP);

            // 设置测试模式下耳麦音量的初始值
            ipvp_set_volume(mkGetHandle(), MID_VOLUME);
        }
        break;
    case EM_HANDSET:
        {
            //进入回音测试前,先设置vpm音量,为的是,在talk起来后设置音量,而此时,vpm未起来,会导致设置不成功,此处再设置一次
            ipvp_set_mode(mkGetHandle(), IPVP_MODE_HS);
            ipvp_set_volume(mkGetHandle(), MID_VOLUME);
        }
        break;
    default:
        break;
    }
}

bool CTestPhone::SetSpeakerMode(int iVolume)
{
    ipvp_set_mode(mkGetHandle(), IPVP_MODE_HF);
    ipvp_set_volume(mkGetHandle(), iVolume);
    return true;
}

// 回音测试音量调节
bool CTestPhone::ChangeSpeakerVolume(bool bIncrease)
{
    bool bHandled = false;
    if (NULL == m_pVolumeBar)
    {
        bHandled = true;

        m_pVolumeBar = new CAutoVolumeBar;
        if (NULL != m_pVolumeBar)
        {
            m_pVolumeBar->SetVolumeType(VT_TALK_HANDFREE);
            m_pVolumeBar->SetAutoHideEnable(false);
            m_pVolumeBar->UpdateVolume(m_iCurrentVolume);
        }
    }
    else
    {
        m_pVolumeBar->ChangeVolume(bIncrease);
        bHandled = true;
        m_iCurrentVolume = m_pVolumeBar->GetVolumeValue();
        ipvp_set_volume(mkGetHandle(), MID_VOLUME - 8 + m_iCurrentVolume);
    }

    return bHandled;
}

// 绘制音量条
void CTestPhone::DrawVolumeBar(bool bShow /*= true*/)
{
    if (NULL == m_pVolumeBar)
    {
        return;
    }

    if (bShow)
    {
        m_pVolumeBar->show();
    }
    else
    {
        m_pVolumeBar->hide();
    }
}

// 退出
bool CTestPhone::ExitSpeakerVolume()
{
    if (NULL != m_pVolumeBar)
    {
        delete m_pVolumeBar;
        m_pVolumeBar = NULL;
        return true;
    }
    return false;
}

// 通道键处理
bool CTestPhone::OnChannelKeyPress(int iKey)
{
    if (PHONE_KEY_HANDSET_OFF_HOOK != iKey
            && PHONE_KEY_HANDSET_ON_HOOK != iKey)
    {
        PlayLocalDTMF(PHONE_KEY_A);
    }

    return (NULL != m_pTestState && m_pTestState->OnFunkeyPress(iKey));
}

#ifdef IF_USB_SUPPORT
void CTestPhone::OnUSBStateChange(bool bIn)
{
    CTestStateUSB * pUsbState = dynamic_cast<CTestStateUSB *>(m_pTestState);
    if (NULL != pUsbState)
    {
        pUsbState->OnUSBStateChange(bIn);
    }
}
#endif //IF_USB_RECORD

void CTestPhone::OnEHSInsert()
{
    m_bIsEHSInserd = true;
    if (NULL != m_pTestState)
    {
        m_pTestState->OnEHSInsert();
    }
}

void CTestPhone::OnEHSRemove()
{
    m_bIsEHSInserd = false;
    if (NULL != m_pTestState)
    {
        m_pTestState->OnEHSRemove();
    }
}

bool CTestPhone::onKeyPressed(int key, bool bExp)
{
    // 播放按键音
    if ((key >= PHONE_KEY_0 && key <= PHONE_KEY_9) \
            || PHONE_KEY_STAR == key \
            || PHONE_KEY_POUND == key)//0~9，*，#，都是放在OnDigitkeyPress函数里面处理
    {
        OnDigitkeyPress(key);
    }
    else
    {
        if (voice_IsChannelKey(key))
        {
            OnChannelKeyPress(key);
        }
        else
        {
            OnFunckeyPress(key, bExp);
        }
    }

    return true;
}

void CTestPhone::OnMessage(msgObject & msg)
{
    if (NULL != m_pTestState)
    {
        m_pTestState->OnMessage(msg);
    }
}

#if IF_BUG_20900
BOOL CTestPhone::OnDelayStartMacdTimer(msgObject & msg)
{
    // kill macd进程重新启动；
    commonUnits_System("nice -n 19 /boot/bin/macd &");
    return true;
}
#endif

void CTestPhone::SetTestMethod(int iTestMethod)
{
    m_iTestMethod = iTestMethod;

    if (TST_PREBURNING == iTestMethod)
    {
        TestPreburningInit();
    }
    else if (TST_FULLVOICE == iTestMethod)
    {
        TestFullVoiceInit();
    }
    else if (TST_MANAUAL_KEY == iTestMethod)
    {
        TestManaualKeyInit();
    }
#ifdef IF_SUPPORT_WIFI
    else if (TST_WIFI == iTestMethod)
    {
        TestWifiInit();
    }
#endif
#ifdef _CP
    else if (TST_CHECKFLAG == iTestMethod)
    {
        TestResultInit();
    }
#endif
    else if (TST_SFP == iTestMethod)
    {
        TestSFPInitial();
    }
    else if (TST_VIEWSTATUS == iTestMethod)
    {
        ChangeState(new CTestStateView(this));
    }
    else
    {
        TestInitial();
    }
}
