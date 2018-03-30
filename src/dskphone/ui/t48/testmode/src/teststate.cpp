#include "teststate.h"
#include "testphonet4x.h"
#include "testexp.h"
#include "cdlgtestmode.h"
#include "modtestmode.h"
//#include "commonapi/commonapi.h"
#include "commonapi/systemhelperapi.h"
#include "devicelib/phonedevice.h"
#include "keymap.h"
#include "uimanager/moduimanager.h"
#include "uimanager/uimanager_common.h"
#include "lamp/ledlamp/include/modledlamp.h"
#include "commonapi/stringhelperapi.h"
#include "service_ipvp.h"
#include "mainwnd/autovolumebar.h"
#include "testmodeinterface.h"
#include "service_system.h"
#include "idle/phonestatemessagedefine.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "commonunits/commonunits_def.h"
#include "commonunits/modcommonunits.h"
#include "configparser/modconfigparser.h"
#include <configiddefine.h>
#ifdef IF_USB_SUPPORT
#include "record/include/modrecord.h"
#endif
#include <unistd.h>
extern  TestMode_Type CurTestmode;

extern "C"
{
    HPDEV_HANDLE g_handle;
};

#define RECT_FAC_AND_RPS            QRect(200, 350, 300, 200)

#define kszStatusReset              "resetflag"

void DrawRandNumString(LPCSTR lpszNum);

///////////////////////////CTestState///////////////////////////////////
CTestState::CTestState(CTestPhone * pTestPhone)
{
    m_pTestPhone = pTestPhone;
}

CTestState::~CTestState()
{

}
///////////////////////////CTestStateInit///////////////////////////////////

///////////////////////////CTestStateEasyTouch///////////////////////////////////
CTestStateEasyTouch::CTestStateEasyTouch(CTestPhone * pTestPhone) : CTestState(pTestPhone)
{

}

CTestStateEasyTouch::~CTestStateEasyTouch()
{
}

bool CTestStateEasyTouch::OnFunkeyPress(int iKey, bool bIsExpKey /*= false*/)
{
    return !testmode_getBeTouched();
}

bool CTestStateEasyTouch::DrawScreen()
{
    testmode_SetEasyTouch(true, QPoint(400, 240), 70);
    return true;
}

void CTestStateEasyTouch::AfterEnterState()
{
    TESTMODE_INFO("CTestStateEasyTouch::AfterEnterState()");
    devicelib_SetBackLightLevel(100);
    testmode_SetFrameAndTitleVisible(TS_EASY_TOUCH);
    DrawScreen();
}

void CTestStateEasyTouch::BeforeExitState()
{
    testmode_SetEasyTouch(false, QPoint(0, 0), 0);
}

///////////////////////////CTestStateLCD///////////////////////////////////
CTestStateLCD::CTestStateLCD(CTestPhone * pTestPhone) : CTestState(pTestPhone)
{
    m_eCurrentStep = LCD_WHITE;
    connect(&m_LCDTimer, SIGNAL(timeout()), this, SLOT(OnTimeOut()));
    m_LCDTimer.start(1000);
}

CTestStateLCD::~CTestStateLCD()
{
}
void CTestStateLCD::OnTimeOut()
{
    if (m_eCurrentStep == LCD_WHITE)
    {
        m_eCurrentStep = LCD_BLACK;
    }
    else if (m_eCurrentStep == LCD_BLACK)
    {
        m_eCurrentStep = LCD_WHITE;
    }
    DrawScreen();
}
bool CTestStateLCD::OnFunkeyPress(int iKey, bool bIsExpKey /*= false*/)
{
    if (iKey == PHONE_KEY_HANDFREE
            && (LCD_END - 1) !=
            m_eCurrentStep)//lcm按下了免提键而且当前不是渐变显示，只有按下了正确的键才能够推进
    {
        TESTMODE_INFO("OnFunkeyPress code = 0x%x", iKey);
        if (m_eCurrentStep == LCD_BLACK || m_eCurrentStep == LCD_WHITE)
        {
            m_LCDTimer.stop();
            m_eCurrentStep = LCD_RED;
        }
        else
        {
            m_eCurrentStep = (LCD_COLOR_STEP)(m_eCurrentStep + 1);
        }

        DrawScreen();//绘制屏幕
    }
    else if (iKey == PHONE_KEY_HANDFREE
             && m_eCurrentStep >= LCD_END -
             1)//按下免提键，且已经到了LCD的最后一个状态，顺序跳转
    {
        return false;
    }
    return true;
}

bool CTestStateLCD::DrawScreen()
{
    switch (m_eCurrentStep)
    {
    case LCD_WHITESCREEN:
        {
            testmode_FullScreen(QColor(255, 255, 255));
        }
        break;
    case LCD_WHITE:
        {
            testmode_DrawText("Degradation Testing...", true);
        }
        break;
    case LCD_BLACK:
        {
            testmode_DrawText("Degradation Testing...", true, -1, Qt::white, Qt::black);
        }
        break;
    case LCD_RED:
        {
            testmode_DrawText("", true);
            testmode_FullScreen(QColor(255, 0, 0));
        }
        break;
    case LCD_GREEN:
        {
            testmode_FullScreen(QColor(0, 255, 0));
        }
        break;
    case LCD_BLUE:
        {
            testmode_FullScreen(QColor(0, 0, 255));
        }
        break;
    case LCD_GRID:
        {
            QString strPic = PIC_TESTMODE_GRID;
            testmode_FullScreen(strPic);
        }
        break;
    case LCD_GRAY80:
        {
            QString strPic = PIC_TESTMODE_GRAY80;
            testmode_FullScreen(strPic);
        }
        break;
    case LCD_GRAY50:
        {
            QString strPic = PIC_TESTMODE_GRAY50;
            testmode_FullScreen(strPic);
        }
        break;
    case LCD_GRAY30:
        {
            QString strPic = PIC_TESTMODE_GRAY30;
            testmode_FullScreen(strPic);
        }
        break;
    case LCD_GRADIENT:
        {
            // 灰阶渐变
            testmode_FullScreen(Qt::lightGray);
        }
        break;
    default:
        {
            TESTMODE_WARN("Unkonwn LCD Color.");
        }
        break;
    }

    return true;
}

void CTestStateLCD::AfterEnterState()
{
    TESTMODE_INFO("CTestStateLCD::AfterEnterState()");
    devicelib_SetBackLightLevel(100);
    //  voice_SetVolume(2, VT_TALK_HANDFREE, false);
    ipvp_set_mode(mkGetHandle(), IPVP_MODE_HF);
    ipvp_set_volume(mkGetHandle(), 0);
    testmode_SetFrameAndTitleVisible(TS_LCD);
    DrawScreen();

    g_pTestPhone->StartLCDTest();
}

void CTestStateLCD::BeforeExitState()
{
    g_pTestPhone->CloseBreathLCD();
}

bool CTestStateLCD::isStateLastStep()
{
    return m_eCurrentStep >= LCD_END - 1;
}

///////////////////////////CTestStateRandNum///////////////////////////////////
CTestStateRandNum::CTestStateRandNum(CTestPhone * pTestPhone) : CTestState(pTestPhone)
{

}

CTestStateRandNum::~CTestStateRandNum()
{

}


void DrawRandNumString(LPCSTR lpszNum)
{
    //显示随机数
    testmode_DrawText(QString(lpszNum));
}
bool CTestStateRandNum::DrawScreen()
{
    // 获取的随机数
    char buf[128];
    memset(buf, 0, 128);
    int ret = sys_get_env("randomness", buf, 127);
    // 字符串结尾
    buf[127] = '\0';
    if (ret == 0)
    {
        TESTMODE_INFO("success sys_get_env_randomness:%s\n", buf);
    }
    else
    {
        TESTMODE_INFO("there is no randomness\n");
        memcpy(buf, "00000000000000000", 17);
    }

    yl::string strRandNum = buf;
    //直接调用DrawRandNumString（）先刷屏后显示随机数
    DrawRandNumString(buf);

    QString strFAC_RPS = "";

    int nRet = sys_get_env("certalgotithm", buf, 127);
    if (0 == nRet)
    {
        strFAC_RPS += buf;
    }
    else
    {
        strFAC_RPS += "0";
    }

    strFAC_RPS += "/";

    nRet = sys_get_env("certbit", buf, 127);
    if (0 == nRet)
    {
        strFAC_RPS += buf;
    }
    else
    {
        strFAC_RPS += "NA";
    }

    strFAC_RPS += "\n";

    yl::string strResetStatus = commUnits_GetUserEnvString(kszStatusReset);
    //获取是否恢复出厂设置
    if (strResetStatus.empty())
    {
        strFAC_RPS += "Factory Status: No Restored";
    }
    else
    {
        strFAC_RPS += "Factory Status: Restored";
    }

    strFAC_RPS += "\n";

    //获取RPS的状态
    if (1 == configParser_GetConfigInt(kszRedirectEnable))
    {
        strFAC_RPS += "RPS Status: RPS Started";
        testmode_DrawTextInRect(strFAC_RPS, RECT_FAC_AND_RPS, QColor(0, 255, 0));
    }
    else
    {
        strFAC_RPS += "RPS Status: RPS Closed";
        testmode_DrawTextInRect(strFAC_RPS, RECT_FAC_AND_RPS, QColor(255, 0, 0));
    }

    return true;

}

bool CTestStateRandNum::OnFunkeyPress(int iKey, bool bIsExpKey /*= FALSE*/)
{
    if (PHONE_KEY_HANDFREE == iKey) //按下免提就直接切换状态了
    {
        return false;
    }
    return  true;
}

void CTestStateRandNum::AfterEnterState()
{
    testmode_SetFrameAndTitleVisible(TS_RANDNUM);
    DrawScreen();
}

void CTestStateRandNum::BeforeExitState()
{
    testmode_DrawText("");
}

///////////////////////////CTestStateUSB///////////////////////////////////

CTestStateUSB::CTestStateUSB(CTestPhone * pTestPhone) : CTestState(pTestPhone)
{
    m_eUSBState = USB_NONE;
    m_AllChecked = false;
    m_isrung = false;
}

CTestStateUSB::~CTestStateUSB()
{
    TESTMODE_INFO("CTestStateUSB::~CTestStateUSB()");
#ifndef IF_USB_SUPPORT
    // 释放资源
    SignalHandler(0);
#endif
}

// 检测USB状态
void CTestStateUSB::CheckUSBState()
{
    TESTMODE_INFO("CTestStateUSB::CheckUSBState()");
#ifdef IF_USB_SUPPORT
    bool isinsert = modRecord_IsStorageConnect();
    if (USB_NONE == m_eUSBState)//如果上一次状态为nono，状态不能变为remove
    {
        m_eUSBState = isinsert ? USB_INSERT : USB_NONE;
        return ;
    }
    m_eUSBState = isinsert ? USB_INSERT : USB_REMOVE;
#else
    int ret;

    signal(SIGINT, &SignalHandler);

    ret = hpdev_create(&g_handle, &HpdevEvcb, 0);
    if (ret < 0)
    {
        TESTMODE_INFO("hpdev create err!");
        return;
    }

    ret = hpdev_run(g_handle);
    if (ret < 0)
    {
        hpdev_destroy(g_handle);
        return;
    }
#endif
}

#ifndef IF_USB_SUPPORT
void SignalHandler(int signo)
{
    TESTMODE_INFO("CTestStateUSB::SignalHandler signo = %d", signo);
    hpdev_halt(g_handle);
    hpdev_destroy(g_handle);
    g_handle = 0;
    /*exit(1);*/
}


// USB检测回调函数,在USB状态有变化时被调用
void HpdevEvcb(void * private_data, HPDEV_EVT * hpdev_evt)
{
    TESTMODE_INFO("Callback: CTestStateUSB::HpdevEvcb");
    if (NULL == g_pTestPhone)
    {
        TESTMODE_WARN("g_pTestPhone == null");
        return;
    }

    char dev_node[PATH_MAX];
    char mount_point[PATH_MAX];
    (void)private_data;

    HPDEV_DUMP_HPDEV_EVT(hpdev_evt);

    // 根据USB连接状态设置
    USB_STATE usbState = USB_NONE;
    // 对指针判空
    if (NULL == hpdev_evt)
    {
        return;
    }

    switch (hpdev_evt->dev_event)
    {
    case HPDEV_EVENT_ADD:
        usbState = USB_INSERT;
        break;
    case HPDEV_EVENT_REMOVE:
        usbState = USB_REMOVE;
        break;
    default:
        usbState = USB_NONE;
        break;
    }

    etl_NotifyApp(false, PHONESTATE_TEST_USB, usbState, 0);
}
#endif

bool CTestStateUSB::OnFunkeyPress(int iKey, bool bIsExpKey /*= false*/)
{
    TESTMODE_INFO("CTestStateUSB::OnFunkeyPress m_eUSBState = %d", m_eUSBState);

    if (USB_NONE != m_eUSBState
            && iKey == PHONE_KEY_HANDFREE && m_AllChecked)//向下一个状态推进
    {
        TESTMODE_INFO("Start to test key");
        return false;
    }
    return true;
}

bool CTestStateUSB::DrawScreen()
{
    // 根据状态画出显示字串
    testmode_DrawText("");
    DrawUSBString();
    return true;
}
void CTestStateUSB::AfterEnterState()
{
    TESTMODE_INFO("CTestStateUSB::AfterEnterState()");
    ipvp_set_mode(mkGetHandle(), IPVP_MODE_HF);
    ipvp_set_volume(mkGetHandle(), MID_VOLUME);
    // 检测USB的状态
    CDlgTestMode * pTestDlg = qobject_cast<CDlgTestMode *>(UIManager_GetPrivateDlg(DLG_CDlgTestMode));
    if (NULL != pTestDlg)
    {
        connect(pTestDlg, SIGNAL(CheckBoxAllChecked()), this, SLOT(AllChecked()));
    }

    CheckUSBState();
    testmode_SetFrameAndTitleVisible(TS_USB);
    DrawScreen();
}

void CTestStateUSB::SetUSBState(USB_STATE eUSBState)
{
    if (USB_INSERT == m_eUSBState && m_AllChecked && !m_isrung)
    {
        m_isrung = true;
        ipvp_ring_play(mkGetHandle(), DEFAULT_RING_FILE, -1);
        etlSleep(1500);
        ipvp_ring_stop(mkGetHandle());
    }
    if (eUSBState >= USB_NONE
            && eUSBState <= USB_REMOVE)
    {
        m_eUSBState = eUSBState;
    }
    else
    {
        m_eUSBState = USB_NONE;
    }
}

void CTestStateUSB::DrawUSBString()
{
    CDlgTestMode * pTestDlg = qobject_cast<CDlgTestMode *>(UIManager_GetPrivateDlg(DLG_CDlgTestMode));
    if (pTestDlg == NULL)
    {
        return;
    }
    switch (m_eUSBState)
    {
    case USB_INSERT:
        {
            pTestDlg->setUSBText("USB OK!", Qt::green);
            break;
        }
    case USB_REMOVE:
        {
            pTestDlg->setUSBText("USB Failed!",  Qt::red);
            break;
        }
    default:
        {
            pTestDlg->setUSBText("Input USB store!", Qt::black);
            break;
        }
    }
}
void CTestStateUSB::AllChecked()
{
    m_AllChecked = true;
    if (USB_NONE != m_eUSBState && m_AllChecked && !m_isrung)
    {
        m_isrung = true;
        ipvp_ring_play(mkGetHandle(), DEFAULT_RING_FILE, 1);
        etlSleep(1500);
        ipvp_ring_stop(mkGetHandle());
    }
}

///////////////////////////CTestStateKey///////////////////////////////////
CTestStateKey::CTestStateKey(CTestPhone * pTestPhone) : CTestState(pTestPhone)
{
    m_bInputCorrect = true;
    m_iCurrentIndex = -1;
    //  InitCmpString();
    m_uMaxKeyNum = 0;
}

CTestStateKey::~CTestStateKey()
{

}
void CTestStateKey::AddString(yl::string str)
{
    m_strKey[m_uMaxKeyNum] = str;
    m_uMaxKeyNum++;
}
void CTestStateKey::InitCmpString()
{
    m_uMaxKeyNum = 0;
    int i = 0;
    for (i = 0; i < 9; i++)
    {
        m_strKey[m_uMaxKeyNum++] = commonAPI_FormatString("%d", i + 1);
    }
    m_strKey[m_uMaxKeyNum++] = "*";
    m_strKey[m_uMaxKeyNum++] = "0";
    m_strKey[m_uMaxKeyNum++] = "#";

    //for(i = 0; i < 4; i++)
    //{
    //  m_strKey[i + 12] = commonAPI_FormatString("HK%d", i + 1);
    //}

    //for(i = 0; i < 10; i++)
    //{
    //  m_strKey[m_uMaxKeyNum++] = commonAPI_FormatString("LK%d", i + 1);
    //}

    //for(i = 0; i < 4; i++)
    //{
    //  m_strKey[m_uMaxKeyNum++] = commonAPI_FormatString("FN%d", i + 1);
    //}

    m_strKey[m_uMaxKeyNum++] = "Up";
    m_strKey[m_uMaxKeyNum++] = "Dn";
    m_strKey[m_uMaxKeyNum++] = "Lf";
    m_strKey[m_uMaxKeyNum++] = "Rt";
    m_strKey[m_uMaxKeyNum++] = "OK";
    m_strKey[m_uMaxKeyNum++] = "Cancel";
    m_strKey[m_uMaxKeyNum++] = "Vol-";
    m_strKey[m_uMaxKeyNum++] = "Vol+";

    m_strKey[m_uMaxKeyNum++] = "Headset";
    m_strKey[m_uMaxKeyNum++] = "Mute";
    m_strKey[m_uMaxKeyNum++] = "Msg";
    m_strKey[m_uMaxKeyNum++] = "Hold";
    m_strKey[m_uMaxKeyNum++] = "RD";
    m_strKey[m_uMaxKeyNum++] = "Tran";
    m_strKey[m_uMaxKeyNum++] = "HandFree";
    m_uMaxKeyNum--;//从0开始计数
}

yl::string CTestStateKey::GetCorrectString(int iIndex)//index都是以下标为开始的
{
    if (iIndex < 0 || iIndex > (int)m_uMaxKeyNum)
    {
        return "";
    }

    chString strRetVal = "";
    for (int i = 0; i <= iIndex; i++)
    {
        strRetVal = strRetVal + m_strKey[i] + " ";
    }

    return strRetVal;
}

LPCSTR CTestStateKey::GetKeyString(int iIndex)//获取指定下标的字符串
{
    if (iIndex < 0 || iIndex > (int)m_uMaxKeyNum)
    {
        return "";
    }

    return (LPCSTR)(m_strKey[iIndex].c_str());
}

bool CTestStateKey::OnFunkeyPress(int iKey, bool bIsExpKey /*= false*/)
{
    // 对按键的测试

    yl::string strKey = "";
    if (bIsExpKey)
    {
        strKey = "AnyEXPKey";
    }
    {
        switch (iKey)
        {
        case PHONE_KEY_HANDFREE:
            {
                strKey = "HandFree";
            }
            break;
        case PHONE_KEY_FN1:
            strKey = "FN1";
            break;
        case PHONE_KEY_FN2:
            strKey = "FN2";
            break;
        case PHONE_KEY_FN3:
            strKey = "FN3";
            break;
        case PHONE_KEY_FN4:
            strKey = "FN4";
            break;
        case PHONE_KEY_LINE1:
            strKey = "LK1";
            break;
        case PHONE_KEY_LINE2:
            strKey = "LK2";
            break;
        case PHONE_KEY_LINE3:
            strKey = "LK3";
            break;
        case PHONE_KEY_LINE4:
            strKey = "LK4";
            break;
        case PHONE_KEY_LINE5:
            strKey = "LK5";
            break;
        case PHONE_KEY_LINE6:
            strKey = "LK6";
            break;
        case PHONE_KEY_LINE7:
            strKey = "LK7";
            break;
        case PHONE_KEY_LINE8:
            strKey = "LK8";
            break;
        case PHONE_KEY_LINE9:
            strKey = "LK9";
            break;
        case PHONE_KEY_LINE10:
            strKey = "LK10";
            break;
        case PHONE_KEY_UP:
            {
                strKey = "Up";
            }
            break;
        case PHONE_KEY_DOWN:
            {
                strKey = "Dn";
            }
            break;
        case PHONE_KEY_LEFT:
            {
                strKey = "Lf";
            }
            break;
        case PHONE_KEY_RIGHT:
            {
                strKey = "Rt";
            }
            break;
        case PHONE_KEY_OK:
            {
                strKey = "OK";
            }
            break;
        case PHONE_KEY_CANCEL:
            {
                strKey = "Cancel";
            }
            break;
        case PHONE_KEY_VOLUME_INCREASE:
            {
                strKey = "Vol+";
            }
            break;
        case PHONE_KEY_VOLUME_DECREASE:
            {
                strKey = "Vol-";
            }
            break;
        case PHONE_KEY_MESSAGE:
            {
                strKey = "Msg";
            }
            break;
        case PHONE_KEY_HEADSET_CTRL:
            {
                strKey = "Headset";
            }
            break;
        case PHONE_KEY_HOLD:
            {
                strKey = "Hold";
            }
            break;
        case PHONE_KEY_MUTE:
            {
                strKey = "Mute";
            }
            break;
        case PHONE_KEY_TRANSFER:
            {
                strKey = "Tran";
            }
            break;
        case PHONE_KEY_REDIAL:
            {
                strKey = "RD";
            }
            break;
        case PHONE_KEY_CONFERENCE:
            {
                strKey = "Conf";
            }
            break;
        default:
            break;
        }
    }

    // 判断当前的按键是否正确
    m_bInputCorrect = (strKey == GetKeyString(m_iCurrentIndex));

    if (m_iCurrentIndex >= (int)GetMaxKeyNum() - 1
            && m_bInputCorrect) //按下了最后一个正确的按键
    {
        return false;//有按下了多一个按键，进入下一个状态
    }
    else
    {
        DrawScreen();
        if (m_bInputCorrect)//绘制完才加一
        {
            ++m_iCurrentIndex;
        }
    }
    return true;
}

bool CTestStateKey::OnDigitkeyPress(int iKey)
{
    yl::string strKey;
    if (m_iCurrentIndex > (int)GetMaxKeyNum())//
    {
        return true;//数字按键不会跳转状态
    }

    if (iKey >= PHONE_KEY_0
            && iKey <= PHONE_KEY_9)
    {
        strKey = commonAPI_FormatString("%d", iKey - PHONE_KEY_0);
    }
    else if (PHONE_KEY_STAR == iKey)
    {
        strKey = "*";
    }
    else if (PHONE_KEY_POUND == iKey)
    {
        strKey = "#";
    }

    m_bInputCorrect = (strKey == GetKeyString(m_iCurrentIndex));

    DrawScreen();

    if (m_bInputCorrect)//绘制完才加一
    {
        ++m_iCurrentIndex;
    }
    return true;
}

bool CTestStateKey::OnTimer(UINT uTimerID)
{
    return false;
}

bool CTestStateKey::DrawScreen()
{
    if (m_iCurrentIndex == -1)
    {
        // m_iCurrentIndex为-1表示刚进入按键测试
        ++m_iCurrentIndex;
        testmode_DrawText("Please press [1]", false, 40, Qt::red);
    }
    else
    {
        // 调用基类的接口来根据不同的状态画屏幕
        DrawKeyString(m_bInputCorrect, m_iCurrentIndex);
    }

    return true;
}
void CTestStateKey::DrawKeyString(bool bCorrect, int iIndex)
{
    if (iIndex < 0 || iIndex > (int)m_uMaxKeyNum)
    {
        return;
    }

    if (bCorrect)
    {
        testmode_DrawText(QString((LPCSTR)GetCorrectString(iIndex).c_str()), false);
    }
    else//不正确打印提示语句
    {
        chString strDisp;
        char tempBuf[100];
        sprintf(tempBuf, "Please press [%s]", (LPCSTR)m_strKey[iIndex].c_str());
        testmode_DrawText(QString(tempBuf), false, 40, Qt::red);
    }
}
void CTestStateKey::AfterEnterState()
{
    TESTMODE_INFO("CTestStateKey::AfterEnterState()");
    //voice_SwitchChannel(CHANNEL_HANDFREE);
    //voice_SetVolume(16, VT_TALK_HANDFREE, false);
    ipvp_set_mode(mkGetHandle(), IPVP_MODE_HF);
    ipvp_set_volume(mkGetHandle(), 16);
    testmode_SetFrameAndTitleVisible(TS_KEY);
    DrawScreen();
}
///////////////////////////CTestStateLED///////////////////////////////////
CTestStateLED::CTestStateLED(CTestPhone * pTestPhone) : CTestState(pTestPhone)
{
    m_LedStep = LIGHT_CROSS_STEP_1;//T48没有linekey

}

CTestStateLED::~CTestStateLED()
{

}

bool CTestStateLED::OnFunkeyPress(int iKey, bool bIsExpKey)
{
    if (iKey == PHONE_KEY_HANDFREE)  //免提键
    {
        if (m_LedStep >= LIGHT_CROSS_STEP_2)
        {
            return false;//切换状态
        }
        m_LedStep++;
        LightLedByStep();

    }
    return true;
}

bool CTestStateLED::DrawScreen()
{
    testmode_DrawText("Test Led!");
    testmode_FullScreen(QColor(255, 255, 255));
    return true;
}
void CTestStateLED::AfterEnterState()
{
    m_pTestPhone->AllLightOff();
    testmode_SetFrameAndTitleVisible(TS_LED);
    DrawScreen();
    LightLedByStep();
}
void   CTestStateLED::LightLedByStep()
{
    if (m_LedStep > LIGHT_CROSS_STEP_2 || m_LedStep < LIGHT_CROSS_STEP_1)
    {
        return;
    }
    m_pTestPhone->LightLinekeyLedByCross((LIGHT_CROSS_STEP)m_LedStep);
}
bool CTestStateLED::isStateLastStep()
{
    return m_LedStep >= LIGHT_CROSS_STEP_2;
}

///////////////////////////CTestStateEcho///////////////////////////////////
CTestStateEcho::CTestStateEcho(CTestPhone * pTestPhone) : CTestState(pTestPhone)
{
    m_eCurrentMode = EM_ECHO_START;
}

CTestStateEcho::~CTestStateEcho()
{

}

void CTestStateEcho::StartEchoByVPM()
{
    StartEchoTest(m_eCurrentMode);
}

void CTestStateEcho::StartEchoTest(ECHO_MODE eMode)
{
    TESTMODE_INFO("CTestStateEcho::StartEchoTest ECHO_ MODE is %d", eMode);
    switch (eMode)
    {
    case EM_ECHO_START:
        {

            ipvp_echo_test(mkGetHandle(), IPVP_ECHO_RECORD);//lcm停止上一次的录制清空缓存
            //进入回音测试前,先设置vpm音量,为的是,在talk起来后设置音量,而此时,vpm未起来,会导致设置不成功,此处再设置一次
            int iDefaultVol = MID_VOLUME;
            ipvp_set_mode(mkGetHandle(), IPVP_MODE_HS);
            ipvp_set_volume(mkGetHandle(), iDefaultVol);//lcm设置音量

            ipvp_set_mode(mkGetHandle(), IPVP_MODE_HF);
            ipvp_set_volume(mkGetHandle(), iDefaultVol);
        }
        break;
    case EM_SPEAKER_RECORD:
        {
            ipvp_echo_test(mkGetHandle(), IPVP_ECHO_STOP);
            ipvp_echo_test(mkGetHandle(), IPVP_ECHO_RECORD);
        }
        break;
    case EM_SPEAKER_PLAY:
        {
            ipvp_echo_test(mkGetHandle(), IPVP_ECHO_PLAY);
        }
        break;
    case EM_HEADSET:
        {
            g_pTestPhone->LightOn(T48_LIGHT_HEADSET);
            g_pTestPhone->LightOn(T48_LIGHT_MUTE);
            g_pTestPhone->LightOff(T48_LIGHT_SPK);

            //// 设置测试模式下耳麦音量的初始值
            ipvp_echo_test(mkGetHandle(), IPVP_ECHO_STOP);
            ipvp_set_mode(mkGetHandle(), IPVP_MODE_ES);
            ipvp_echo_test(mkGetHandle(), IPVP_ECHO_LOOP);
            ipvp_set_volume(mkGetHandle(), MID_VOLUME);
        }
        break;
    case EM_HANDSET:
        {
            g_pTestPhone->LightOff(T48_LIGHT_SPK);
            g_pTestPhone->LightOn(T48_LIGHT_MUTE);
            g_pTestPhone->LightOff(T48_LIGHT_HEADSET);

            ipvp_set_mode(mkGetHandle(), IPVP_MODE_HS);
            ipvp_set_volume(mkGetHandle(), MID_VOLUME);
        }
        break;
    default:
        break;
    }
}

bool CTestStateEcho::OnFunkeyPress(int iKey, bool bIsExpKey /*= false*/)
{

    // 如果Echo状态有变化
    if (ChangeEchoState(iKey, m_eCurrentMode))//主要是对message headset 按键进行响应，
    {
        StartEchoByVPM();//
        DrawScreen();
    }
    return true;
}

bool CTestStateEcho::DrawScreen()
{
    // 调用基类的接口来根据不同的状态画屏幕
    DrawEchoString(m_eCurrentMode);
    if (TST_PREBURNING == CurTestmode && EM_HEADSET != m_eCurrentMode && EM_HANDSET != m_eCurrentMode)
    {
        testmode_FullScreen(Qt::lightGray);
    }
    return true;
}

void CTestStateEcho::DrawEchoString(ECHO_MODE eMode)
{
    chString strEchoMode = "";
    switch (eMode)
    {
    case EM_ECHO_START:
        {
            strEchoMode = "";
        }
        break;
    case EM_SPEAKER_RECORD:
        {
            strEchoMode = "Handfree Speaker";
        }
        break;
    case EM_SPEAKER_PLAY:
        {
            strEchoMode = "Handsfree Play";
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
    default:
        break;
    }

    testmode_DrawText(QString((LPCSTR)strEchoMode.c_str()));
}

bool CTestStateEcho::ChangeEchoState(int iKey, ECHO_MODE & eMode)
{
    TESTMODE_INFO("CTestPhone::ChangeEchoState iKey[0x%x] eMode[%d]", iKey, eMode);
    if (iKey != PHONE_KEY_HEADSET_CTRL && iKey != PHONE_KEY_HANDSET_OFF_HOOK
            && iKey != PHONE_KEY_VOLUME_INCREASE && iKey != PHONE_KEY_VOLUME_DECREASE
            && iKey != PHONE_KEY_MESSAGE)
    {
        //可以跳转模式的有headset 耳机，handset手柄 ，message键，对音量加，音量减并不改变模式
        return FALSE;
    }

    CDlgTestMode * pTestDlg = qobject_cast<CDlgTestMode *>(UIManager_GetPrivateDlg(DLG_CDlgTestMode));
    if (pTestDlg == NULL)
    {
        return FALSE;
    }
    // 按HEADSET键进入耳机测试模式
    if (iKey == PHONE_KEY_HEADSET_CTRL)
    {
        //清一下屏，避免老化模式带来的影响
        if (TST_PREBURNING ==        CurTestmode)
        {
            testmode_FullScreen(Qt::white);
        }
        if (eMode == EM_SPEAKER_RECORD || eMode == EM_SPEAKER_PLAY)//if (eMode == EM_SPEEKER)
        {
            g_pTestPhone->StopEchoTimer();
            eMode = EM_HEADSET;
            // 将音量控制条隐藏起来
            pTestDlg->GetVolumeBar()->hide();
            return TRUE;
        }
    }
    // 按摘机键进入手柄测试模式
    else if (iKey == PHONE_KEY_HANDSET_OFF_HOOK)
    {
        //g_pTestPhone->StopEchoTimer();
        if (eMode == EM_HEADSET)//lcm上一次的测试是耳机测试才能转
        {
            eMode = EM_HANDSET;
            // 隐藏声音控制条
            pTestDlg->GetVolumeBar()->hide();
            return TRUE;
        }
    }
    // 按Message键再次进入回音测试
    else if (iKey == PHONE_KEY_MESSAGE)
    {
        if (!g_pTestPhone->getCanJumpToEchoTest())
        {
            g_pTestPhone->StopEchoTimer();
            g_pTestPhone->StartEchoTimer();
            g_pTestPhone->resetLCDLevel();

            eMode = EM_SPEAKER_RECORD;
            // 隐藏声音控制条
            pTestDlg->GetVolumeBar()->hide();
            return TRUE;
        }
    }
    // 音量变化
    else if (iKey == PHONE_KEY_VOLUME_INCREASE
             || iKey == PHONE_KEY_VOLUME_DECREASE)
    {
        if (eMode != EM_SPEAKER_PLAY)
        {
            //其他界面不响应
            return false;
        }

        if (!pTestDlg->GetVolumeBar()->isVisible())
        {
            pTestDlg->GetVolumeBar()->setVisible(true);
        }
        QKeyEvent tKeyEvt(QEvent::KeyPress, iKey, 0);
        pTestDlg->GetVolumeBar()->eventFilter(pTestDlg->GetVolumeBar(), &tKeyEvt);

        return false;
    }

    return FALSE;
}

void CTestStateEcho::AfterEnterState()
{
    TESTMODE_INFO("CTestStateEcho::AfterEnterState ");
    StartEchoByVPM();//开始测试，这个函数里面有一直跑到，最顶层去，有渐变绘图的，调用函数进行录制相关操作的
    m_eCurrentMode = EM_SPEAKER_RECORD;//当前是录音状态
    testmode_SetFrameAndTitleVisible(TS_ECHO);
    DrawScreen();
    g_pTestPhone->StartEchoTimer();//自己用的自己开启
    StartEchoByVPM();//录制

    g_pTestPhone->StartLCDTest();
    g_pTestPhone->StartLightTest();
}

void CTestStateEcho::BeforeExitState()
{
    g_pTestPhone->AfterEchoState();
}

ECHO_MODE CTestStateEcho::GetEchoType()
{
    return m_eCurrentMode;
}

//根据状态，切换播放或录音
bool CTestStateEcho::OnTimer(UINT uTimerID)//主要是给循环
{
    if (uTimerID != m_iTimerID)
    {
        return false;
    }

    if (m_eCurrentMode == EM_SPEAKER_RECORD)
    {
        m_eCurrentMode = EM_SPEAKER_PLAY;
        StartEchoByVPM();
        DrawScreen();
        return true;
    }
    else if (m_eCurrentMode == EM_SPEAKER_PLAY)
    {
        m_eCurrentMode = EM_SPEAKER_RECORD;
        CDlgTestMode * pTestDlg = qobject_cast<CDlgTestMode *>(UIManager_GetPrivateDlg(DLG_CDlgTestMode));
        if (pTestDlg != NULL)
        {
            if (pTestDlg->GetVolumeBar()->isVisible())
            {
                // 隐藏声音控制条
                pTestDlg->GetVolumeBar()->hide();
            }
        }

        StartEchoByVPM();
        DrawScreen();
        return true;
    }
    else
    {
        return false;
    }
}

///////////////////////////CTestStateExit///////////////////////////////////
CTestStateExit::CTestStateExit(CTestPhone * pTestPhone) : CTestState(pTestPhone)
{

}

CTestStateExit::~CTestStateExit()
{

}

bool CTestStateExit::OnFunkeyPress(int iKey, bool bIsExpKey)
{
    return true;
}

void CTestStateExit::AfterEnterState()
{
    TESTMODE_INFO("CTestStateExit::AfterEnterState");
    // 灭掉所有灯
    if (m_pTestPhone != NULL)
    {
        m_pTestPhone->AllLightOff();

        if (m_pTestPhone->m_pTestEXP != NULL)
        {
            m_pTestPhone->m_pTestEXP->LightOffAllEXP();
        }
    }

    // 设置背光灯为最亮
    devicelib_SetBackLightLevel(100);

    ipvp_loop_test(mkGetHandle(), 0);
    // 发送消息给VPM，要求退出回音测试
    ipvp_echo_test(mkGetHandle(), IPVP_ECHO_STOP);
    ipvp_set_mode(mkGetHandle(), IPVP_MODE_HS);

    testmode_Exit();
}

///////////////////////////CTestStateEXPLight///////////////////////////////////

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

bool CTestStateEXPKey::OnFunkeyPress(int iKey,
                                     bool bIsExpKey /*= false*/)//只是检测EXP是否连接成功
{

    if (bIsExpKey)//如果是EXP按键，说明连接是成功的
    {
        return false;//进入下一个状态
    }
    return true;
}

bool CTestStateEXPKey::DrawScreen()
{
    testmode_DrawText("Press AnyEXPKey", false, 40, Qt::red);
    return true;
}

void CTestStateEXPKey::AfterEnterState()
{
    TESTMODE_INFO("CTestStateEXPKey::AfterEnterState");
    DrawScreen();

}

///////////////////////////CTestStateEXPLCD///////////////////////////////////

///////////////////////////CTestStateAudio///////////////////////////////////

bool CTestStateAudio::DrawScreen()
{
    //直接调用DrawRandNumString（）先刷屏后显示提示，具体有接口的时候在写过
    testmode_DrawText("Audio Test");
    return true;
}

CTestStateAudio::CTestStateAudio(CTestPhone * pTestPhone) : CTestState(pTestPhone)
{

}

CTestStateAudio::~CTestStateAudio()
{

}

void CTestStateAudio::AfterEnterState()
{
    TESTMODE_INFO("CTestStateAudio::AfterEnterState");
    DrawScreen();
    ipvp_loop_test(mkGetHandle(), 1);
}

bool  CTestStateAudio::OnFunkeyPress(int iKey, bool bIsExpKey)
{
    return true;
}


///////////////////////////CTestStateEXPLight///////////////////////////////////
CTestStateEXPLight::CTestStateEXPLight(CTestPhone * pTestPhone) : CTestState(pTestPhone)
{
    m_eLightStep = LS_GREEN;
}
CTestStateEXPLight::~CTestStateEXPLight()
{

}
bool CTestStateEXPLight::DrawScreen()
{
    if (m_pTestPhone == NULL)
    {
        return false;
    }

    // 调用基类的接口来根据不同的状态画屏幕
    DrawEXPLightString(m_eLightStep);
    DrawEXPVersionString();

    return true;
}
void CTestStateEXPLight::DrawEXPLightString(LIGHT_STEP eStep)
{
    chString strLightString = "";
    switch (eStep)
    {
    case LS_GREEN:
        {
            strLightString = "Green Light Test";
        }
        break;
    case LS_RED:
        {
            strLightString = "Red Light Test";
        }
        break;
    case LS_ORANGE:
        {
            strLightString = "Orange Light Test";
        }
        break;
    default:
        break;
    }
    testmode_FullScreen(Qt::white);
    testmode_DrawText(strLightString.c_str(), true);

}
void CTestStateEXPLight::AfterEnterState()
{
    TESTMODE_INFO("CTestStateEXPLight::AfterEnterState()");
    if (m_pTestPhone == NULL || m_pTestPhone->m_pTestEXP == NULL)
    {
        return;
    }

    m_pTestPhone->InitEXPTest(m_eLightStep);
    testmode_SetFrameAndTitleVisible(TS_EXP_LIGHT);
    DrawScreen();
}
bool CTestStateEXPLight::OnFunkeyPress(int iKey, bool bIsExpKey /*= false*/)
{
    if (m_pTestPhone == NULL || m_pTestPhone->m_pTestEXP == NULL)
    {
        return true;
    }
    if (!bIsExpKey)
    {
        if (iKey == PHONE_KEY_HANDFREE)
        {
            m_eLightStep = (LIGHT_STEP)(m_eLightStep + 1);
        }

        if (m_eLightStep > LS_ORANGE)
        {
            m_pTestPhone->ChangeState(new CTestRealStateEXPKey(m_pTestPhone));  // 跳转到EXP按键测试
            return true;
        }

        if (m_pTestPhone->m_pTestEXP != NULL)
        {
            m_pTestPhone->m_pTestEXP->LightOnFirstEXP(m_eLightStep);//点亮第一个扩展台
            DrawScreen();
        }
    }

    return true;
}


void CTestStateEXPLight::DrawEXPVersionString()
{
    QString strEXPInfo;

    int lcdopen = exp_helper_open();
    if (lcdopen > 0)
    {
        char hwver[128] = { 0 };
        char swver[128] = { 0 };
        exp_helper_get_hwversion(lcdopen, 0, 127, hwver);
        exp_helper_get_swversion(lcdopen, 0, 127, swver);

        strEXPInfo = "Hardware: ";
        strEXPInfo += hwver;
        strEXPInfo += "\n";

        strEXPInfo += "Firmware: ";
        strEXPInfo += swver;

        exp_helper_close(lcdopen);
    }

    testmode_DrawTextInRect(strEXPInfo, RECT_FAC_AND_RPS);
}

///////////////////////////CTestStateEXPKey///////////////////////////////////
CTestRealStateEXPKey::CTestRealStateEXPKey(CTestPhone * pTestPhone) : CTestState(pTestPhone)
{
    m_bInputCorrect = false;
    m_iCurrentIndex = -1;
    m_bEXPOk = false;
}

CTestRealStateEXPKey::~CTestRealStateEXPKey()
{

}
bool CTestRealStateEXPKey::OnDigitkeyPress(int iKey)
{
    DrawEXPKeyString(false, m_iCurrentIndex);
    return true;
}
bool CTestRealStateEXPKey::OnFunkeyPress(int iKey, bool bIsExpKey /*= false*/)
{
    TESTMODE_INFO("CTestStateEXPKey::OnFunkeyPress iKey = 0x%x, bIsExpKey = %d", iKey, bIsExpKey);
    if (m_pTestPhone == NULL || (m_pTestPhone->m_pTestEXP == NULL)) //
    {
        return true;
    }
    // 有按键输入先将输入正确的变量置为否
    m_bInputCorrect = false;

    if (bIsExpKey)
    {
        int iEXPIndex = GetEXPIndex(iKey);
        int iEXPKeyIndex = GetEXPKeyIndex(iKey);
        TESTMODE_INFO("iEXPIndex = %d, iEXPKeyIndex = %d", iEXPIndex, iEXPKeyIndex);
        if (0 == iEXPIndex) //第一个扩展台按下键
        {
            if (iEXPKeyIndex == m_pTestPhone->m_pTestEXP->GetKeyIndex(
                        m_iCurrentIndex))//得到按键的真正index，因为顺序不一样,当前和按下的比较
            {
                m_bInputCorrect = true;//按键正确
                m_pTestPhone->m_pTestEXP->LightOffByKeyIndex(iKey);//关灯
                ++m_iCurrentIndex;

                // 如果按下第一个扩展台的最后一个按键，则进入EXP40LCD测试

                if (m_iCurrentIndex == (int)
                        m_pTestPhone->m_pTestEXP->GetEXPkeyNum())//完成了最后一个按键测试
                {
                    if (m_pTestPhone->m_pTestEXP->BeNeedTestEXPLCD()) //需要进行EXPLCD测试
                    {
                        m_pTestPhone->ChangeState(new CTestStateEXPLCD(m_pTestPhone));
                        return true;
                    }
                    else//为了EXP38
                    {

                        return true;
                    }
                }
            }
        }

        else if (1 == iEXPIndex) //第二个扩展台的按键，退出的时候使用
        {
            // 如果不需要LCD测试则按第二台扩展台的第一个按键 退出测试模式
            if (!m_pTestPhone->m_pTestEXP->BeNeedTestEXPLCD() //不需要LCD测试
                    && iEXPKeyIndex == 0x0000 //第一个按键
                    && m_iCurrentIndex == (int)m_pTestPhone->m_pTestEXP->GetEXPkeyNum())//按键测试已经走完了
            {
                m_pTestPhone->m_pTestEXP->LightOffByKeyIndex(iKey);//关灯
                m_pTestPhone->ChangeState(new CTestStateEXPExit(m_pTestPhone));
                return true;
            }
        }

    }

    DrawScreen();
    return true;
}

bool CTestRealStateEXPKey::DrawScreen()
{
    if (m_pTestPhone == NULL)//
    {
        return true;
    }

    if (m_iCurrentIndex == -1)
    {
        testmode_DrawText("Please Press EXP [1]", false, 40, Qt::red);
        ++m_iCurrentIndex;
    }
    else
    {
        // 调用基类的接口来根据不同的状态画屏幕
        DrawEXPKeyString(m_bInputCorrect, m_iCurrentIndex);
    }


    return true;
}

void CTestRealStateEXPKey::AfterEnterState()
{

    testmode_SetFrameAndTitleVisible(TS_EXP_REAL_KEY);
    DrawScreen();
}
void CTestRealStateEXPKey::DrawEXPKeyString(bool bCorrect, int iIndex)
{
    if (m_pTestPhone->m_pTestEXP == NULL)
    {
        return;
    }

    if (iIndex < 0 || iIndex > m_pTestPhone->m_pTestEXP->GetEXPkeyNum())
    {
        return;
    }
    if (bCorrect)
    {
        testmode_DrawText(QString((LPCSTR)m_pTestPhone->m_pTestEXP->GetEXPKeyString(iIndex).c_str()),
                          false);
    }
    else
    {
        chString strDisp;// = commonAPI_FormatString("Press %d", iIndex+1);
        char tempBuf[100];
        sprintf(tempBuf, "Please Press EXP [%d]", iIndex + 1);
        testmode_DrawText(QString(tempBuf), false, 40, Qt::red);
    }
}


/////////////////////////////CTestStateEXPLCD///////////////////////////////////
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
    if (m_pTestPhone == NULL || (m_pTestPhone->m_pTestEXP == NULL)) //
    {
        return true;
    }

    if (bIsExpKey)
    {
        int iEXPIndex = GetEXPIndex(iKey);
        int iEXPKeyIndex = GetEXPKeyIndex(iKey);
        if (iEXPIndex == 0)
        {
            if (iEXPKeyIndex == m_pTestPhone->m_pTestEXP->GetKeyIndex((int)
                    m_pTestPhone->m_pTestEXP->GetEXPkeyNum() - 1))
            {
                ++m_iCurrentStep;
                m_iCurrentStep = m_iCurrentStep % 4; //循环用

                // 必须要把4张图片测一遍
                if (!m_bLCDTestFinished && m_iCurrentStep == 3)
                {
                    m_bLCDTestFinished = true;
                }

                DrawScreen();
            }
        }
        else if (iEXPIndex == 1)//第二个扩展台的第一个按键退出测试模式
        {
            if (m_bLCDTestFinished && iEXPKeyIndex == 0)
            {
                m_pTestPhone->m_pTestEXP->LightOffByKeyIndex(iKey);
                m_pTestPhone->ChangeState(new CTestStateEXPExit(m_pTestPhone));
            }
        }
    }

    return true;
}

bool CTestStateEXPLCD::DrawScreen()
{
    if (m_pTestPhone == NULL || m_pTestPhone->m_pTestEXP == NULL)
    {
        return false;
    }

    testmode_DrawText("");
    if (m_bEXPOk)
    {
        testmode_DrawText("EXP is OK!");
    }
    else
    {
        chString strLCDTest;// = commonAPI_FormatString("EXP LCD Test%d", m_iCurrentStep+1);
        char tempBuf[100];
        sprintf(tempBuf, "EXP LCD Test%d", m_iCurrentStep + 1);
        testmode_DrawText(tempBuf);
        m_pTestPhone->m_pTestEXP->DrawEXPLCD(m_iCurrentStep);
    }
    return true;
}

void CTestStateEXPLCD::AfterEnterState()
{
    testmode_SetFrameAndTitleVisible(TS_EXP_LCD);
    DrawScreen();
}

/////////////////////////////CTestStateEXPExit///////////////////////////////////
CTestStateEXPExit::CTestStateEXPExit(CTestPhone * pTestPhone) : CTestState(pTestPhone)
{

}

CTestStateEXPExit::~CTestStateEXPExit()
{

}

bool CTestStateEXPExit::OnFunkeyPress(int iKey, bool bIsExpKey/* = false*/)
{
    return true;
}

bool CTestStateEXPExit::DrawScreen()
{
    if (m_pTestPhone == NULL)
    {
        return false;
    }
    testmode_SetFrameAndTitleVisible(TS_EXP_EXIT);
    testmode_DrawText("");
    QApplication::processEvents();
    if (m_pTestPhone->m_pTestEXP->BeNeedTestEXPLCD()) //需要EXPLCD测试的
    {
        //绘制loading 图片
        exp_helper_set_lcd_testmode(g_pTestPhone->m_iEXPHandle, 0, 5);
        exp_helper_set_lcd_testmode(g_pTestPhone->m_iEXPHandle, 1, 5);
    }
    usleep(1000 * 1000 * 2); //延时2秒
    testmode_SetAllHide();
    testmode_FullScreen(QColor(255, 0, 0));
    return true;
}

void CTestStateEXPExit::AfterEnterState()
{
    TESTMODE_INFO("CTestStateEXPExit::AfterEnterState()");
    if (m_pTestPhone == NULL)
    {
        return;
    }

    DrawScreen();

}

