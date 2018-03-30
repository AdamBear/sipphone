#include "testphonet42.h"
#include "test_include.h"

/////////////////////////////CTestPhoneT42/////////////////////////////////
CTestPhoneT42::CTestPhoneT42()
    : CTestPhoneT66()
{
    m_uMaxKeyNum = 35;
    m_iLineNum = 6;

    m_bHasEXP = false;
    m_bSkipEHSTest = FALSE;
}

CTestPhoneT42::~CTestPhoneT42()
{
}

void CTestPhoneT42::FlushLightIndex()
{
    m_vecLightIndex.push_back(T42_LIGHT_LK1_GREEN);
    m_vecLightIndex.push_back(T42_LIGHT_LK2_GREEN);
    m_vecLightIndex.push_back(T42_LIGHT_LK3_GREEN);
    m_vecLightIndex.push_back(T42_LIGHT_LK4_GREEN);
    m_vecLightIndex.push_back(T42_LIGHT_LK5_GREEN);
    m_vecLightIndex.push_back(T42_LIGHT_LK6_GREEN);

    m_vecLightIndex.push_back(T42_LIGHT_LK1_RED);
    m_vecLightIndex.push_back(T42_LIGHT_LK2_RED);
    m_vecLightIndex.push_back(T42_LIGHT_LK3_RED);
    m_vecLightIndex.push_back(T42_LIGHT_LK4_RED);
    m_vecLightIndex.push_back(T42_LIGHT_LK5_RED);
    m_vecLightIndex.push_back(T42_LIGHT_LK6_RED);

    m_vecLightIndex.push_back(T42_LIGHT_LK1_GREEN | T42_LIGHT_LK1_RED);
    m_vecLightIndex.push_back(T42_LIGHT_LK2_GREEN | T42_LIGHT_LK2_RED);
    m_vecLightIndex.push_back(T42_LIGHT_LK3_GREEN | T42_LIGHT_LK3_RED);
    m_vecLightIndex.push_back(T42_LIGHT_LK4_GREEN | T42_LIGHT_LK4_RED);
    m_vecLightIndex.push_back(T42_LIGHT_LK5_GREEN | T42_LIGHT_LK5_RED);
    m_vecLightIndex.push_back(T42_LIGHT_LK6_GREEN | T42_LIGHT_LK6_RED);

    m_vecLightIndex.push_back(T42_LIGHT_MUTE);
    m_vecLightIndex.push_back(T42_LIGHT_HEADSET);
    m_vecLightIndex.push_back(T42_LIGHT_SPK);
}

void CTestPhoneT42::InitCmpString()
{
    int iKeyIndex = 0;

    for (int i = 0; i < 9; ++i)
    {
        m_strKey[iKeyIndex++] = commonAPI_FormatString("%d", i + 1);
    }

    m_strKey[iKeyIndex++] = "*";
    m_strKey[iKeyIndex++] = "0";
    m_strKey[iKeyIndex++] = "#";

    for (int i = 0; i < m_iLineNum; ++i)
    {
        m_strKey[iKeyIndex++] = commonAPI_FormatString("LK%d", i + 1);
    }

    for (int i = 0; i < 4; ++i)
    {
        m_strKey[iKeyIndex++] = commonAPI_FormatString("HK%d", i + 1);
    }

    m_strKey[iKeyIndex++] = "Up";
    m_strKey[iKeyIndex++] = "Dn";
    m_strKey[iKeyIndex++] = "Lf";
    m_strKey[iKeyIndex++] = "Rt";
    m_strKey[iKeyIndex++] = "OK";
    m_strKey[iKeyIndex++] = "Cancel";
    m_strKey[iKeyIndex++] = "Vol-";
    m_strKey[iKeyIndex++] = "Vol+";
    m_strKey[iKeyIndex++] = "Mute";
    m_strKey[iKeyIndex++] = "Headset";
    m_strKey[iKeyIndex++] = "Msg";
    m_strKey[iKeyIndex++] = "RD";
    m_strKey[iKeyIndex++] = "HandFree";
}

bool CTestPhoneT42::ChangeEchoState(int iKey, ECHO_MODE & eMode)
{
    ECHO_MODE eOldMode = eMode;
    if (CTestPhone::ChangeEchoState(iKey, eMode))
    {
        if (eOldMode != EM_HEADSET
                && eMode == EM_HEADSET)
        {
            // 进入耳麦回音测试，关闭免提灯
            LightOff(T42_LIGHT_SPK);
        }
        else if (eOldMode == EM_HEADSET
                 && eMode == EM_HANDSET)
        {
            // 进入手柄回音测试，关闭耳麦
            LightOff(T42_LIGHT_HEADSET);
        }
        else if (eOldMode == EM_EHS
                 && eMode != EM_EHS)
        {
            DrawIcon(LCD_ICON_HEADS, FALSE);
        }

        return TRUE;
    }

    return FALSE;
}

void CTestPhoneT42::FillFullRect(bool bBlack)
{
    DrawAllIcon(bBlack);

    CTestPhone::FillFullRect(bBlack);
}

void CTestPhoneT42::DrawIcon(LCD_ICON_TYPE iConType, bool bShow)
{
    devicelib_SetIcon(iConType, bShow);
}

void CTestPhoneT42::DrawAllIcon(bool bShow)
{
    for (int i = 0; i < LCD_ICON_LAST; ++i)
    {
        DrawIcon((LCD_ICON_TYPE)i, bShow);
    }
}

// 新需求led测试阶段专用
void CTestPhoneT42::TestAllLightOn(int mLineColor, bool bFuncKey/* = true*/)
{
    TurnLightAllOff(true);

    switch (mLineColor)
    {
    case LS_GREEN:
        {
            for (int i = 0; i < m_iLineNum; i++)
            {
                TurnLightOn(m_vecLightIndex[m_iLineNum * mLineColor + i]);
            }

            TurnLightOn(m_vecLightIndex[T66_FUNCTION_LIGHT_BEGIN]);
            TurnLightOn(m_vecLightIndex[T66_FUNCTION_LIGHT_BEGIN + 2]);
        }
        break;
    case LS_RED:
        {
            for (int i = 0; i < m_iLineNum; i++)
            {
                TurnLightOn(m_vecLightIndex[m_iLineNum * mLineColor + i]);
            }

            TurnLightOn(m_vecLightIndex[T66_FUNCTION_LIGHT_BEGIN + 1]);
        }
        break;
    case LS_ORANGEONE:
        {
            TurnLightOn(m_vecLightIndex[m_iLineNum * mLineColor]);
            TurnLightOn(m_vecLightIndex[m_iLineNum * mLineColor + 1]);
            TurnLightOn(m_vecLightIndex[m_iLineNum * mLineColor + 4]);
            TurnLightOn(m_vecLightIndex[m_iLineNum * mLineColor + 5]);
        }
        break;
    case LS_ORANGETWO:
        {
            TurnLightOn(m_vecLightIndex[m_iLineNum * (mLineColor - 1) + 2]);
            TurnLightOn(m_vecLightIndex[m_iLineNum * (mLineColor - 1) + 3]);
            for (int i = T66_FUNCTION_LIGHT_BEGIN; i < m_vecLightIndex.size(); i++)
            {
                // 点亮所有功能键
                TurnLightOn(m_vecLightIndex[i]);
            }
        }
        break;
    default:
        break;
    }

    DoLight();
}
