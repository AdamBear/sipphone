#include "testphonet23.h"
#include "test_include.h"

/////////////////////////////CTestPhoneT22/////////////////////////////////
CTestPhoneT23::CTestPhoneT23()
{
    m_uMaxKeyNum = 32;
    m_iLineNum = 3;
    m_bHasEXP = false;
}

CTestPhoneT23::~CTestPhoneT23()
{
    timerKillThreadTimer((UINT)&m_iLightTimer);
}

void CTestPhoneT23::FlushLightIndex()
{
    m_vecLightIndex.push_back(T23_LIGHT_LK1_GREEN);
    m_vecLightIndex.push_back(T23_LIGHT_LK2_GREEN);
    m_vecLightIndex.push_back(T23_LIGHT_LK3_GREEN);
    m_vecLightIndex.push_back(T23_LIGHT_LK1_RED);
    m_vecLightIndex.push_back(T23_LIGHT_LK2_RED);
    m_vecLightIndex.push_back(T23_LIGHT_LK3_RED);
    m_vecLightIndex.push_back(T23_LIGHT_LK1_GREEN | T23_LIGHT_LK1_RED); //橙色1
    m_vecLightIndex.push_back(T23_LIGHT_LK2_GREEN | T23_LIGHT_LK2_RED); //橙色2
    m_vecLightIndex.push_back(T23_LIGHT_LK3_GREEN | T23_LIGHT_LK3_RED); //橙色3
    m_vecLightIndex.push_back(T23_LIGHT_MSG);
}

void CTestPhoneT23::InitCmpString()
{
    int i = 0;
    for (i = 0; i < 9; i++)
    {
        m_strKey[i] = commonAPI_FormatString("%d", i + 1);
    }

    m_strKey[9] = "*";
    m_strKey[10] = "0";
    m_strKey[11] = "#";

    for (i = 0; i < 4; i++)
    {
        m_strKey[i + 12] = commonAPI_FormatString("HK%d", i + 1);
    }

    m_strKey[16] = "Up";
    m_strKey[17] = "Dn";
    m_strKey[18] = "Lf";
    m_strKey[19] = "Rt";
    m_strKey[20] = "OK";
    m_strKey[21] = "Cancel";
    m_strKey[22] = "Vol-";
    m_strKey[23] = "Vol+";

    for (i = 0; i < 3; i++)
    {
        m_strKey[i + 24] = commonAPI_FormatString("LK%d", i + 1);
    }

    m_strKey[27] = "Msg";
    m_strKey[28] = "Headset";
    m_strKey[29] = "Tran";
    m_strKey[30] = "RD";
    m_strKey[31] = "HandFree";
}

int CTestPhoneT23::GetNoteTextFont(int nTextType)
{
    return TEST_FONT_10;
}

bool CTestPhoneT23::ChangeEchoState(int iKey, ECHO_MODE & eMode)
{
    return CTestPhone::ChangeEchoState(iKey, eMode);
}

// 新需求led测试阶段专用
void CTestPhoneT23::TestAllLightOn(int mLineColor, bool bFuncKey/* = true*/)
{
    TurnLightAllOff(true);

    switch (mLineColor)
    {
    case LS_GREEN:
        {
            // 点亮功能键
            TurnLightOn(m_vecLightIndex[T66_FUNCTION_LIGHT_BEGIN]);
        }
    case LS_RED:
    case LS_ORANGEONE:
        {
            for (int i = 0; i < m_vecLightIndex.size(); i++)
            {
                TurnLightOn(m_vecLightIndex[m_iLineNum * mLineColor + i]);
            }
        }
        break;
    default:
        break;
    }

    DoLight();
}

