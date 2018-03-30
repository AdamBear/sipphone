#include "testmode_common.h"
#include "testphonet4x.h"
#include "cdlgtestmode.h"
#include "teststate.h"
#include "testexp.h"
#include "devicelib/phonedevice.h"
#include "keymap.h"
#include "uimanager/moduimanager.h"
#include "uimanager/uimanager_common.h"
#include "dsskey/src/dsskey_common.h"
#include <devicelib/phonedevice.h>
#include "commonunits/commonunits_def.h"
#include "service_ipvp.h"
// LED灯每800ms变换一次
#define LIGHT_TIME_SPAN 800
// 回音测试录音与播音间隔3s
#define ECHO_TIME_SPAN  3000
// LCD变换时间
#define LCD_TIME_SPAN   100

// #define LINEKEY_NUMBER  10
extern  TestMode_Type CurTestmode;
/////////////////////////////CTestPhoneT4X/////////////////////////////////
CTestPhoneT4X * CTestPhoneT4X::s_pInst = NULL;

CTestPhoneT4X * CTestPhoneT4X::GetInstance()
{
    if (NULL == s_pInst)
    {
        s_pInst = new CTestPhoneT4X();
    }

    return s_pInst;
}

void CTestPhoneT4X::DestoryInstance()
{
    if (NULL != s_pInst)
    {
        delete s_pInst;
        s_pInst = NULL;
    }
}

CTestPhoneT4X::CTestPhoneT4X()
{
    m_uMaxKeyNum = 0;
    m_iLightTimer = -1;
    m_iLCDTimer = -1;
    m_iExpLightTimer = -1;
    m_iEchoTimer = -1;
    m_ExpLightType = (LIGHT_Type)EXP_LED_OFF;
    m_iLCDLevel = 0;
    m_bLightLevelUp = true;
    m_bCanJumpToEchoTest = true;
    // 开始循环单个LED亮灯
//  StartCircleLightTest();
}

CTestPhoneT4X::~CTestPhoneT4X()
{

    TESTMODE_INFO("CTestPhoneT4X::~CTestPhoneT4X.");
    if (-1 != m_iLightTimer)
    {
        StopTimerEvent(m_iLightTimer);
    }

    if (-1 != m_iLCDTimer)
    {
        StopTimerEvent(m_iLCDTimer);
    }

    if (-1 != m_iExpLightTimer)
    {
        StopTimerEvent(m_iExpLightTimer);
    }

    m_iLightTimer = -1;
    m_iLCDTimer = -1;
    m_iExpLightTimer = -1;

    AllLightOff();

    ipvp_echo_test(mkGetHandle(), IPVP_ECHO_STOP);
    ipvp_set_mode(mkGetHandle(), IPVP_MODE_HS);
}

bool CTestPhoneT4X::OnTimer(UINT uTimerID)
{
    if (uTimerID == (UINT)m_iLightTimer)//此定时器两用
    {
        if (!m_bCircleLight)//lcm不是循环亮灯，这边应该是回音测试那时候的切换
        {
            //t48没有linekey，不存在循环亮灯了
            //for (int i = 0; i < LINEKEY_NUMBER; i++)
            //{
            //  //当前所亮的为红灯,切换为绿灯
            //  if (IsLightOn(m_vec_iLightIndex[i])
            //      && !IsLightOn(m_vec_iLightIndex[i+10]))
            //  {
            //      //light off
            //      m_iLightStatus &= ~m_vec_iLightIndex[i];
            //      //light on
            //      m_iLightStatus |= m_vec_iLightIndex[i+10];
            //  }
            //  //当前所亮的为绿灯,切换为橙灯，橙色灯不要单独关闭开启，而是加开红灯
            //  else if (IsLightOn(m_vec_iLightIndex[i+10])
            //      && !IsLightOn(m_vec_iLightIndex[i]))
            //  {
            //      //light on
            //      //加开红灯
            //      m_iLightStatus |= m_vec_iLightIndex[i];
            //  }
            //  //当前所亮的为橙灯或当前未亮灯,切换为红灯
            //  else if (IsLightOn(m_vec_iLightIndex[i] | m_vec_iLightIndex[i+10])
            //      || (!IsLightOn(m_vec_iLightIndex[i])
            //      && !IsLightOn(m_vec_iLightIndex[i+10])))
            //  {
            //      //light off
            //      //灭掉绿灯
            //      m_iLightStatus &= ~m_vec_iLightIndex[i+10];
            //      //开启红灯
            //      m_iLightStatus |= m_vec_iLightIndex[i];
            //  }
            //}
        }
        else if (TSI_LED !=
                 m_StateStep) //lcm这边是一进入测试之后的循环亮灯的,单独的led这边跳过
        {
            if (T48_LIGHT_HEADSET & m_iLightStatus && !(T48_LIGHT_SPK & m_iLightStatus))
            {
                m_iLightStatus &= ~T48_LIGHT_HEADSET;
                m_iLightStatus |= T48_LIGHT_MUTE;
            }
            else if (T48_LIGHT_MUTE & m_iLightStatus && !(T48_LIGHT_HEADSET & m_iLightStatus))
            {
                m_iLightStatus &= ~T48_LIGHT_MUTE;
                m_iLightStatus |= T48_LIGHT_SPK;
            }
            else if ((T48_LIGHT_SPK & m_iLightStatus && !(T48_LIGHT_MUTE & m_iLightStatus)) || \
                     (!(T48_LIGHT_MUTE & m_iLightStatus) && !(T48_LIGHT_SPK & m_iLightStatus)
                      && !(T48_LIGHT_HEADSET & m_iLightStatus)))//全都不亮，就亮第一个
            {
                m_iLightStatus &= ~T48_LIGHT_SPK;
                m_iLightStatus |= T48_LIGHT_HEADSET;
            }
        }

        //设置灯的状态
        devicelib_SetLedStatus(m_iLightStatus);//lcm将最新的状态设置上去，
        return true;
    }
    else if (uTimerID == (UINT)
             m_iLCDTimer)//lcm lcd屏幕的定时器，回音测试的时候一闪一闪用的
    {
        devicelib_SetBackLightLevel(m_iLCDLevel);
        m_iLCDLevel = m_bLightLevelUp ? ((m_iLCDLevel + 2) % 101) : ((m_iLCDLevel - 2) % 101);
        m_bLightLevelUp = (m_iLCDLevel == 100) ? false : ((m_iLCDLevel == 0) ? true : m_bLightLevelUp);
        return true;
    }

    else if (uTimerID == (UINT)
             m_iExpLightTimer)//lcm扩展台的灯需要全部亮，而且需要三种颜色之间进行切换
    {
        if (m_pTestEXP != NULL)
        {
            TESTMODE_INFO("m_ExpLightType = %d", m_ExpLightType);
            // 红绿橙交替
            //当前所亮的为红灯,切换为绿灯
            if (m_ExpLightType == (LIGHT_Type)EXP_LED_RED)
            {
                m_pTestEXP->LightEXP(m_uEXPNum, (LIGHT_Type)EXP_LED_GREEN);
                m_ExpLightType = (LIGHT_Type)EXP_LED_GREEN;
            }
            //当前所亮的为绿灯,切换为橙灯
            else if (m_ExpLightType == (LIGHT_Type)EXP_LED_GREEN)
            {
                m_pTestEXP->LightEXP(m_uEXPNum, (LIGHT_Type)EXP_LED_ORANGE);
                m_ExpLightType = (LIGHT_Type)EXP_LED_ORANGE;
            }
            //当前所亮的为橙灯或当前未亮灯,切换为红灯
            else if (m_ExpLightType == (LIGHT_Type)EXP_LED_OFF
                     || m_ExpLightType == (LIGHT_Type)EXP_LED_ORANGE)
            {
                m_pTestEXP->LightEXP(m_uEXPNum, (LIGHT_Type)EXP_LED_RED);
                m_ExpLightType = (LIGHT_Type)EXP_LED_RED;
            }
        }
    }

    if (m_pTestState != NULL)
    {
        if (m_pTestState->OnTimer(uTimerID))//echotimer
        {
            return true;
        }
    }

    return false;
}
void CTestPhoneT4X::InitState()
{
    TESTMODE_INFO("InitState");

    setCanJumpToEchoTest(true);

    m_vec_Teststate.clear();
    m_vec_Teststate.push_back(TS_RANDNUM);
    m_vec_Teststate.push_back(TS_LCD);
    m_vec_Teststate.push_back(TS_USB);
    m_vec_Teststate.push_back(TS_KEY);
    //m_vec_Teststate.push_back(TS_EXP_KEY);
    m_vec_Teststate.push_back(TS_LED);
    m_vec_Teststate.push_back(TS_ECHO);
    m_vec_Teststate.push_back(TS_EXIT);

    StartCircleLightTest();
}

void CTestPhoneT4X::InitSFPState()
{
    TESTMODE_INFO("InitSFPState");

    setCanJumpToEchoTest(false);

    m_vec_Teststate.clear();
    m_vec_Teststate.push_back(TS_LCD);

    m_vec_Teststate.push_back(TS_EASY_TOUCH);

    m_vec_Teststate.push_back(TS_LED);
    m_vec_Teststate.push_back(TS_ECHO);
    m_vec_Teststate.push_back(TS_EXIT);

    StartCircleLightTest();
}

void CTestPhoneT4X::InitLight()
{
    TESTMODE_INFO("InitLight");
    m_vec_iLightIndex.clear();
    //m_vec_iLightIndex.push_back(T48_LIGHT_LK1_RED);
    //m_vec_iLightIndex.push_back(T48_LIGHT_LK2_RED);
    //m_vec_iLightIndex.push_back(T48_LIGHT_LK3_RED);
    //m_vec_iLightIndex.push_back(T48_LIGHT_LK4_RED);
    //m_vec_iLightIndex.push_back(T48_LIGHT_LK5_RED);
    //m_vec_iLightIndex.push_back(T48_LIGHT_LK6_RED);
    //m_vec_iLightIndex.push_back(T48_LIGHT_LK7_RED);
    //m_vec_iLightIndex.push_back(T48_LIGHT_LK8_RED);
    //m_vec_iLightIndex.push_back(T48_LIGHT_LK9_RED);
    //m_vec_iLightIndex.push_back(T48_LIGHT_LK10_RED);

    //m_vec_iLightIndex.push_back(T48_LIGHT_LK1_GREEN);
    //m_vec_iLightIndex.push_back(T48_LIGHT_LK2_GREEN);
    //m_vec_iLightIndex.push_back(T48_LIGHT_LK3_GREEN);
    //m_vec_iLightIndex.push_back(T48_LIGHT_LK4_GREEN);
    //m_vec_iLightIndex.push_back(T48_LIGHT_LK5_GREEN);
    //m_vec_iLightIndex.push_back(T48_LIGHT_LK6_GREEN);
    //m_vec_iLightIndex.push_back(T48_LIGHT_LK7_GREEN);
    //m_vec_iLightIndex.push_back(T48_LIGHT_LK8_GREEN);
    //m_vec_iLightIndex.push_back(T48_LIGHT_LK9_GREEN);
    //m_vec_iLightIndex.push_back(T48_LIGHT_LK10_GREEN);

    PHONE_TYPE_DEF Phone_type = devicelib_GetPhoneType();
    if (PT_T48 == Phone_type
            || PT_T48S == Phone_type)
    {
        m_vec_iLightIndex.push_back(T48_LIGHT_HEADSET);
        m_vec_iLightIndex.push_back(T48_LIGHT_MUTE);
        m_vec_iLightIndex.push_back(T48_LIGHT_SPK);
    }
    else if (PT_T29 == Phone_type)
    {
        m_vec_iLightIndex.push_back(T48_LIGHT_MUTE);
        m_vec_iLightIndex.push_back(T48_LIGHT_HEADSET);
    }
    else
    {
        m_vec_iLightIndex.push_back(T48_LIGHT_HEADSET);
        m_vec_iLightIndex.push_back(T48_LIGHT_MUTE);
        m_vec_iLightIndex.push_back(T48_LIGHT_SPK);
    }


}
void CTestPhoneT4X::StartLightTest()
{
    AllLightOff();//这边会将m_lightTimer设置为-1，那么后面m_bCircleLight就可以顺利被设置为false
    LightOn(T48_LIGHT_POWER);
    LightOn(T48_LIGHT_HEADSET);
    LightOn(T48_LIGHT_MUTE);

    PHONE_TYPE_DEF Phone_type = devicelib_GetPhoneType();
    if (PT_T48 == Phone_type
            || PT_T48S == Phone_type)
    {
        // T48需要亮免提灯
        LightOn(T48_LIGHT_SPK);
    }

    if (-1 == m_iLightTimer)
    {
        m_iLightTimer = SetTimerEvent(LIGHT_TIME_SPAN);
        m_bCircleLight = false;
    }

    if (-1 == m_iLCDTimer)
    {
        m_iLCDTimer = SetTimerEvent(LCD_TIME_SPAN);
    }

    if (-1 == m_iExpLightTimer)
    {
        m_iExpLightTimer = SetTimerEvent(LIGHT_TIME_SPAN);//lcm设置扩展台灯计时器
    }
}

void CTestPhoneT4X::StartCircleLightTest()
{
    AllLightOff();

    m_iLightTimer = SetTimerEvent(LIGHT_TIME_SPAN);//lcm每800ms变换一次
    m_bCircleLight = true;
}
void CTestPhoneT4X::StartLCDTest()
{
    TESTMODE_INFO(" StartLCDTest set timer");

    if (-1 == m_iLCDTimer)
    {
        m_iLCDTimer = SetTimerEvent(100);
    }
}

// 设置回音测试定时器
VOID CTestPhoneT4X::StartEchoTimer()//lcm如果已经有，先关闭
{
    TESTMODE_INFO("StartEchoTimer set timer");
    StopTimerEvent(m_iEchoTimer);
    m_iEchoTimer = SetTimerEvent(ECHO_TIME_SPAN);//定时器3秒
    if (NULL != m_pTestState)
    {
        m_pTestState->SetTimerID(m_iEchoTimer);
    }
}

// 停止回音测试定时器
VOID CTestPhoneT4X::StopEchoTimer()
{
    StopTimerEvent(m_iEchoTimer);
    m_iEchoTimer = -1;
    if (NULL != m_pTestState)
    {
        m_pTestState->SetTimerID(m_iEchoTimer);
    }
}


void CTestPhoneT4X::AllLightOff()
{
    TESTMODE_DEBUG("CTestPhoneT4X::AllLightOff");
    if (-1 != m_iLightTimer)
    {
        StopTimerEvent(m_iLightTimer);
        m_iLightTimer = -1;
    }

    //测试模式过程，电源灯一直灭，要求是常亮
    //http://10.2.1.199/Bug.php?BugID=35422
    //打开电源灯
    m_iLightStatus |= T48_LIGHT_POWER;
    //不重置电源灯标志
    LightOff(0xfffffffe);

    if (-1 != m_iExpLightTimer)
    {
        StopTimerEvent(m_iExpLightTimer);
        m_iExpLightTimer = -1;
    }
    if (NULL != m_pTestEXP)
    {
        m_pTestEXP->LightOffAllEXP();
    }
}

void CTestPhoneT4X::LightOn(int iLight)
{
    m_iLightStatus |= iLight;
    devicelib_SetLedStatus(m_iLightStatus);
}

void CTestPhoneT4X::LightOff(int iLight)
{
    m_iLightStatus &= ~iLight;
    devicelib_SetLedStatus(m_iLightStatus);
}

bool CTestPhoneT4X::IsLightOn(int iLight)
{
    return (m_iLightStatus & iLight) == iLight;
}
void CTestPhoneT4X::LightLinekeyLed(LIGHT_STEP eLightSetp)//T48没有linekey灯
{
    //添加message和headset的亮灯
    m_iLightStatus |= T48_LIGHT_HEADSET;
    m_iLightStatus |= T48_LIGHT_MUTE;
    m_iLightStatus |= T48_LIGHT_SPK;
    m_iLightStatus |= T48_LIGHT_POWER;
    devicelib_SetLedStatus(m_iLightStatus);
}

void CTestPhoneT4X::LightLinekeyLedByCross(LIGHT_CROSS_STEP eLightSetp)
{
    switch (eLightSetp)
    {
    case LIGHT_CROSS_STEP_1:
        {
            m_iLightStatus |= T48_LIGHT_HEADSET;
            m_iLightStatus |= T48_LIGHT_SPK;
        }
        break;
    case LIGHT_CROSS_STEP_2:
        {
            m_iLightStatus &= ~T48_LIGHT_HEADSET;
            m_iLightStatus &= ~T48_LIGHT_SPK;
            //开mute
            m_iLightStatus |= T48_LIGHT_MUTE;
        }
        break;
    default:
        break;
    }

    devicelib_SetLedStatus(m_iLightStatus);
}

bool CTestPhoneT4X::ProcessKeyEvent(int iKeyCode,
                                    bool bPressed)//按下和弹起都在这个函数里面处理
{
    TESTMODE_INFO("CDlgTestMode::ProcessKeyEvent iKeyCode = 0x%x, bPressed = %d", iKeyCode, bPressed);

    if (bPressed)//按下键
    {
        if (g_pTestPhone == NULL)
        {
            return false;
        }
        if (TST_FULLVOICE == CurTestmode && PHONE_KEY_HANDSET_ON_HOOK != iKeyCode)
        {
            return false;
        }
        //播放按键音
        if ((iKeyCode >= PHONE_KEY_0 && iKeyCode <= PHONE_KEY_9) \
                || PHONE_KEY_STAR == iKeyCode \
                || PHONE_KEY_POUND == iKeyCode)//0~9，*，#，都是放在OnDigitkeyPress函数里面处理
        {

            PlayLocalDTMF(iKeyCode);//播放声音
            //if (m_isInEXPTest)//EXP测试这边隔离数字键处理
            //{
            //  return true;
            //}
            this->OnDigitkeyPress(iKeyCode);
        }
        else
        {
            if (iKeyCode == PHONE_KEY_HANDSET_OFF_HOOK \
                    || iKeyCode == PHONE_KEY_HANDSET_ON_HOOK)
            {
                // 这些按键不播放按键音
            }
            else
            {
                PlayLocalDTMF(PHONE_KEY_A);
            }
            EXP_TYPE eEXPType = (EXP_TYPE)(iKeyCode >> 24);
            this->OnFunkeyPress(iKeyCode, (ET_EXP38 == eEXPType || ET_EXP39 == eEXPType
                                           || ET_EXP40 == eEXPType));
        }

        return true;
    }
    else
    {
        if (iKeyCode == PHONE_KEY_HANDSET_OFF_HOOK)//弹起只处理挂机键的弹起
        {
            this->OnFunkeyPress(iKeyCode);
            return true;
        }

        return false;
    }
}

CTestState * CTestPhoneT4X::GetNextState()
{
    if ((m_StateStep + 1) < 0  || (m_StateStep + 1) > (m_vec_Teststate.size() - 1))
    {
        TESTMODE_WARN("GetNextState(%d) is invalid.", m_StateStep + 1);
        return NULL ;
    }

    return CreateStateByType(m_vec_Teststate[m_StateStep + 1]);
}

void CTestPhoneT4X::SetUSBState(USB_STATE eUSBState)
{
    if (NULL != m_pTestState && m_StateStep == TSI_USB)
    {
        CTestStateUSB * pUSBState = static_cast<CTestStateUSB *>(m_pTestState);
        if (NULL != pUSBState)
        {
            pUSBState->SetUSBState(eUSBState);
            pUSBState->DrawUSBString();
        }
    }
}

CTestState * CTestPhoneT4X::CreateStateByType(TEST_STATE eTestState)
{
    switch (eTestState)
    {
    case TS_EASY_TOUCH:
        {
            return new CTestStateEasyTouch(this);
        }
        break;
    case TS_KEY:
        {
            return new CTestStateKey(this);
        }
        break;
    case TS_EXIT:
        {
            return new CTestStateExit(this);
        }
        break;
    case TS_LCD:
        {
            return new CTestStateLCD(this);
        }
        break;
    case TS_RANDNUM:
        {
            return new CTestStateRandNum(this);
        }
        break;
    case TS_USB:
        {
            return new CTestStateUSB(this);
        }
        break;
    case TS_ECHO:
        {
            return new CTestStateEcho(this);
        }
        break;
    case TS_EXP_KEY:
        {
            return new CTestStateEXPKey(this);
        }
        break;
    case TS_LED:
        {
            return new CTestStateLED(this);
        }
        break;
    }
    return NULL;
}

bool CTestPhoneT4X::IsEchoState()
{
    if (m_StateStep == TSI_ECHO) //当前测试的状态是回音测试
    {
        return true;
    }
    return false;
}

void CTestPhoneT4X::ExitLCDTest()
{
    StopTimerEvent(m_iLCDTimer);
    m_iLCDTimer = -1;
}

void  CTestPhoneT4X::OnFunkeyPress(int iKey, bool bIsExpKey /*= false*/)
{
    TESTMODE_INFO("CTestPhone::OnFunkeyPress iKey = 0x%x, bIsExpKey = %d", iKey, bIsExpKey);

    if (m_pTestState != NULL)
    {
        if (m_isInEXPTest)  //EXP测试直接截取掉
        {
            m_pTestState->OnFunkeyPress(iKey, bIsExpKey);
            return ;
        }
        //如果是挂机键，直接退出测试
        if (PHONE_KEY_HANDSET_ON_HOOK == iKey)
        {
            ChangeState(CreateStateByType(TS_EXIT), TSI_EXIT);
            // m_StateStep = TSI_EXIT;
        }
        else if (m_vec_Teststate.size() > m_StateStep
                 && TS_LCD  == m_vec_Teststate[m_StateStep]
                 &&  PHONE_KEY_MESSAGE == iKey)  //只有LCD按下message才会直接跳到回音测试
        {
            if (m_bCanJumpToEchoTest)
            {
                ChangeState(CreateStateByType(TS_ECHO), TSI_ECHO);
                //  m_StateStep = TSI_ECHO;
            }
        }
        //半成品测试需求，当进行到led测试的末尾，不能再按handfree进入到echo，只能通过message进入
        else if (m_vec_Teststate.size() > m_StateStep
                 && TS_LED == m_vec_Teststate[m_StateStep]
                 && PHONE_KEY_MESSAGE == iKey
                 && !m_bCanJumpToEchoTest)
        {
            if (m_pTestState->isStateLastStep())
            {
                ChangeState(CreateStateByType(TS_ECHO), TSI_ECHO);
            }
        }
        else if (!m_pTestState->OnFunkeyPress(iKey, bIsExpKey))//返回false代表要变状态了
        {
            //半成品测试需求，当进行到led测试的末尾，不能再按handfree进入到echo，只能通过message进入
            if (m_vec_Teststate.size() > m_StateStep
                    && TS_LED == m_vec_Teststate[m_StateStep]
                    && !m_bCanJumpToEchoTest)
            {
                //
            }
            else
            {
                CTestState * pState = GetNextState();
                if (NULL != pState)
                {
                    ChangeState(pState);
                    if (m_vec_Teststate.size() > m_StateStep
                            && TS_KEY == m_vec_Teststate[m_StateStep])
                    {
                        InitKeyTestOrder(pState);
                    }
                }
            }

        }

    }
}

void CTestPhoneT4X::InitKeyTestOrder(CTestState * pTestState)
{
    if (NULL == pTestState)
    {
        return;
    }

    CTestStateKey * pKeyState = dynamic_cast<CTestStateKey *>(pTestState);
    if (NULL == pKeyState)
    {
        return;
    }

    int i = 0;
    for (i = 0; i < 9; i++)
    {
        pKeyState->AddString(commonAPI_FormatString("%d", i + 1));
    }
    pKeyState->AddString("*");
    pKeyState->AddString("0");
    pKeyState->AddString("#");

    /*for(i = 0; i < 10; i++)
    {
        pKeyState->AddString(commonAPI_FormatString("LK%d", i + 1));
    }

    for(i = 0; i < 4; i++)
    {
        pKeyState->AddString(commonAPI_FormatString("FN%d", i + 1));
    }*/


    pKeyState->AddString("Up");
    pKeyState->AddString("Dn");
    pKeyState->AddString("Lf");
    pKeyState->AddString("Rt");
    pKeyState->AddString("OK");
    pKeyState->AddString("Cancel");
    pKeyState->AddString("Vol-");
    pKeyState->AddString("Vol+");

    PHONE_TYPE_DEF Phone_type = devicelib_GetPhoneType();
    if (PT_T48 == Phone_type
            || PT_T48S == Phone_type)
    {
        pKeyState->AddString("Headset");
        pKeyState->AddString("Mute");
        pKeyState->AddString("Msg");
        pKeyState->AddString("Hold");
        pKeyState->AddString("RD");
        pKeyState->AddString("Tran");
        pKeyState->AddString("HandFree");
    }
    else if (PT_T29 == Phone_type)
    {
        pKeyState->AddString("Msg");
        pKeyState->AddString("Headset");
        pKeyState->AddString("Conf");
        pKeyState->AddString("Hold");
        pKeyState->AddString("Mute");
        pKeyState->AddString("Tran");
        pKeyState->AddString("RD");
        pKeyState->AddString("HandFree");
    }
    else
    {
        pKeyState->AddString("Headset");
        pKeyState->AddString("Mute");
        pKeyState->AddString("Msg");
        pKeyState->AddString("Hold");
        pKeyState->AddString("RD");
        pKeyState->AddString("Tran");
        pKeyState->AddString("HandFree");
    }
    pKeyState->AddString("AnyEXPKey");
}

void CTestPhoneT4X::InitEXPTest(LIGHT_STEP eLightSetp)
{
    if (-1 != m_iExpLightTimer)//停止扩展台的定时器，
    {
        StopTimerEvent(m_iExpLightTimer);
        m_iExpLightTimer = -1;
    }

    if (NULL != m_pTestEXP)
    {
        m_pTestEXP->LightOnFirstEXP(eLightSetp);//点亮第一个扩展台
    }
}

