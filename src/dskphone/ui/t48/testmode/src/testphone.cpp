#include "testphone.h"
#include "testmode_common.h"
#include "teststate.h"
#include "testexp.h"
#include "keymap.h"
#include "dsskey/src/dsskey_common.h"
#include "cdlgtestmode.h"
#include "uimanager/moduimanager.h"
#include "uimanager/uimanager_common.h"
#include <QtGui/QKeyEvent>
#include <QtGui/QApplication>
#include "service_ipvp.h"
#include "testmodeinterface.h"


///////////////////////////CTestPhone///////////////////////////////////
CTestPhone::CTestPhone()
    : m_uMaxKeyNum(0)
    , m_uEXPNum(0)
    , m_ulightNum(0)
    , m_eEXPType(ET_NONE)
{
    m_pTestState = NULL;

    m_iEXPHandle = -1;
    m_iLedStatus = 0;
    m_bLightLevelUp = true;
    m_pTestEXP = NULL;
    m_StateStep = -1;
    m_isInEXPTest = false;
}

CTestPhone::~CTestPhone()
{
    if (m_pTestState != NULL)
    {
        delete m_pTestState;
        m_pTestState = NULL;
    }
    if (m_pTestEXP != NULL)
    {
        delete m_pTestEXP;
        m_pTestEXP = NULL;
    }
}

void CTestPhone::TestInitial()
{
    TESTMODE_INFO("******* CTestPhone::TestInitial begin");
    m_bLightLevelUp = true;
    InitEXPModule();//扩展台模块
    UpdateTestEXP();
    InitState();
    InitLight();//一共有10*2+3个灯的定义

    ChangeState(GetNextState());//状态的迁移，进行后面的测试

}
void CTestPhone::TestFullVoiceInit()
{
    TESTMODE_INFO("******* CTestPhone::TestFullVoiceInit begin");
    //  InitState();
    //  InitLight();
    ChangeState(new CTestStateAudio(this));
}

void CTestPhone::TestPreBurningInit()
{
    TESTMODE_INFO("******* CTestPhone::TestPreBurningInit begin");
    m_bLightLevelUp = true;
    InitEXPModule();//扩展台模块
    UpdateTestEXP();
    InitState();
    InitLight();
    ChangeState(CreateStateByType(TS_ECHO), TSI_ECHO);
}

void CTestPhone::TestSFPInit()
{
    TESTMODE_INFO("******* CTestPhone::TestSFPInit begin");

    m_bLightLevelUp = true;

    InitSFPState();
    InitLight();
    ChangeState(GetNextState());
}

void CTestPhone::ChangeState(CTestState * pTestState, int TSIndex)
{
    TESTMODE_INFO("Change State From %d  ", m_StateStep);
    if (m_pTestState != NULL)
    {
        m_pTestState->BeforeExitState();

//      if (IsEchoState())//如果是从回音测试跳转的
//      {
//          AfterEchoState();//先退出回音测试，包括关闭定时器的一些操作
//      }
        delete m_pTestState;
        m_pTestState = NULL;
    }

    m_pTestState = pTestState;

    // 状态切换之后的操作
    if (m_pTestState != NULL)
    {
        if (-1 != TSIndex)
        {
            m_StateStep = TSIndex;
        }
        else
        {
            m_StateStep++;
        }
        TESTMODE_INFO("Change To %d  ", m_StateStep);
        m_pTestState->AfterEnterState();
    }
}
void CTestPhone::AfterEchoState()//关闭了三个属于testPhone的定时器，
{
    CloseBreathLCD();

    ipvp_echo_test(mkGetHandle(), IPVP_ECHO_STOP);
    ipvp_set_mode(mkGetHandle(), IPVP_MODE_HS);
}

void CTestPhone::CloseBreathLCD()
{
    ExitLCDTest();
    AllLightOff();
}

void CTestPhone::StopTimerEvent(int iTimeID)
{
    if (-1 != iTimeID)
    {
        killTimer(iTimeID);
    }
}

int CTestPhone::SetTimerEvent(int iTimeOut)
{
    return startTimer(iTimeOut);
}

void CTestPhone::timerEvent(QTimerEvent * event)
{
    if (NULL != event)
    {
        OnTimer(event->timerId());
    }
}
// 更新EXP
void CTestPhone::UpdateTestEXP()
{
    if (m_pTestEXP != NULL)
    {
        delete m_pTestEXP;
        m_pTestEXP = NULL;
    }

    TESTMODE_INFO(" EXP type = %d, m_uEXPNum = %d", m_eEXPType, m_uEXPNum);
    if (m_uEXPNum > 0)
    {

        m_pTestEXP = new CTestEXP(m_uEXPNum, m_eEXPType, m_ulightNum, m_uMaxKeyNum);

    }
}

/////////////////////////////For EXPModule/////////////////////////////////////
// 初始化EXP模块
void CTestPhone::InitEXPModule()
{
    TESTMODE_INFO(" CTestPhone::InitEXPModule ");
    m_uEXPNum = 0;//exp模块个数
    m_eEXPType = ET_NONE;//EXp模块的类型

    //检测EXP当前的状态(连接个数)
    m_iEXPHandle = open(EXP_DEV_NAME, O_WRONLY);//lcm打开Exp模块
    if (m_iEXPHandle < 0)//lcm打开EXp模块失败
    {
        TESTMODE_WARN("exp open fail");
        return;
    }

    int iNum = 0;
    exp_helper_get_hopcount(m_iEXPHandle, &iNum);

    if (iNum > 0)//扩展台数量大于0
    {
        int hop_type_first;
        exp_helper_get_hoptype(m_iEXPHandle, 0, &hop_type_first);//lcm用来获取第0exp的类型的

        //下面根据扩展台类型进行赋值
        if (hop_type_first == EXP_TYPE_40)
        {
            m_eEXPType = ET_EXP40;//扩展台类型
            m_uEXPColNum = 2;//列数
            m_ulightNum = 20;
            m_uMaxKeyNum = 22;//按键个数
        }
        else if (hop_type_first == EXP_TYPE_39)
        {
            m_eEXPType = ET_EXP39;
            m_uEXPColNum = 2;
            m_ulightNum = 20;
            m_uMaxKeyNum = 20;

        }
        else if (hop_type_first == EXP_TYPE_38)
        {
            m_eEXPType = ET_EXP38;
            m_uEXPColNum = 4;
            m_ulightNum = 38;
            m_uMaxKeyNum = 38;

        }
        else
        {
            m_eEXPType = ET_NONE;

        }

        int hop_type;
        for (int i = 1; i < iNum; i++)//
        {
            exp_helper_get_hoptype(m_iEXPHandle, i,
                                   &hop_type);//获取和第一个扩展台类型相同的个数
            if (hop_type != hop_type_first)
            {
                m_uEXPNum = i;
                return;
            }
        }
        m_uEXPNum = iNum;
    }

}
void CTestPhone::OnDigitkeyPress(int iKey)
{
    if (m_pTestState != NULL)
    {
        if (!m_pTestState->OnDigitkeyPress(
                    iKey))//只有一种会返回，就是StateKey的可能会通过这个函数来返回
        {
            ChangeState(GetNextState());
        }
    }
}

void CTestPhone::PlayLocalDTMF(int iKey)
{
    BYTE iDtmf = 0;
    if (iKey == PHONE_KEY_HANDSET_OFF_HOOK
            || iKey == PHONE_KEY_HANDSET_ON_HOOK)
    {
        // 这些按键不播放按键音
    }
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
void CTestPhone::StartEXPTest()
{
    // 重新初始化m_pTestEXP

    InitEXPModule();
    UpdateTestEXP();
    //切声道，这样才会有按键音
    ipvp_set_mode(mkGetHandle(), IPVP_MODE_HF);
    ipvp_set_volume(mkGetHandle(), MID_VOLUME);
    //绘制loading
    exp_helper_set_lcd_testmode(g_pTestPhone->m_iEXPHandle, 0, 5);
    exp_helper_set_lcd_testmode(g_pTestPhone->m_iEXPHandle, 1, 5);
    testmode_SetAllHide();
    ChangeState(new CTestStateEXPLight(this));
    m_isInEXPTest = true;
}
void CTestPhone::AbortEXPTest()
{
    testmode_SetFrameAndTitleVisible(TS_EXP_ABORT);
    testmode_DrawText(" ", true);
    testmode_FullScreen(QColor(255, 0, 0));
    if (NULL != m_pTestState)
    {
        delete m_pTestState;
        m_pTestState = NULL;
    }
}
