#include "testphonet27.h"
#include "test_include.h"

CTestPhoneT27::CTestPhoneT27()
{
    m_uMaxKeyNum = 41;
}

CTestPhoneT27::~CTestPhoneT27()
{

}

void CTestPhoneT27::FlushLightIndex()
{
    m_vecLightIndex.push_back(T27_LIGHT_LK1_GREEN);
    m_vecLightIndex.push_back(T27_LIGHT_LK2_GREEN);
    m_vecLightIndex.push_back(T27_LIGHT_LK3_GREEN);
    m_vecLightIndex.push_back(T27_LIGHT_LK4_GREEN);
    m_vecLightIndex.push_back(T27_LIGHT_LK5_GREEN);
    m_vecLightIndex.push_back(T27_LIGHT_LK6_GREEN);
    m_vecLightIndex.push_back(T27_LIGHT_LK7_GREEN);
    m_vecLightIndex.push_back(T27_LIGHT_LK8_GREEN);

    m_vecLightIndex.push_back(T27_LIGHT_LK1_RED);
    m_vecLightIndex.push_back(T27_LIGHT_LK2_RED);
    m_vecLightIndex.push_back(T27_LIGHT_LK3_RED);
    m_vecLightIndex.push_back(T27_LIGHT_LK4_RED);
    m_vecLightIndex.push_back(T27_LIGHT_LK5_RED);
    m_vecLightIndex.push_back(T27_LIGHT_LK6_RED);
    m_vecLightIndex.push_back(T27_LIGHT_LK7_RED);
    m_vecLightIndex.push_back(T27_LIGHT_LK8_RED);

    m_vecLightIndex.push_back(T27_LIGHT_LK1_GREEN | T27_LIGHT_LK1_RED);
    m_vecLightIndex.push_back(T27_LIGHT_LK2_GREEN | T27_LIGHT_LK2_RED);
    m_vecLightIndex.push_back(T27_LIGHT_LK3_GREEN | T27_LIGHT_LK3_RED);
    m_vecLightIndex.push_back(T27_LIGHT_LK4_GREEN | T27_LIGHT_LK4_RED);
    m_vecLightIndex.push_back(T27_LIGHT_LK5_GREEN | T27_LIGHT_LK5_RED);
    m_vecLightIndex.push_back(T27_LIGHT_LK6_GREEN | T27_LIGHT_LK6_RED);
    m_vecLightIndex.push_back(T27_LIGHT_LK7_GREEN | T27_LIGHT_LK7_RED);
    m_vecLightIndex.push_back(T27_LIGHT_LK8_GREEN | T27_LIGHT_LK8_RED);

    m_vecLightIndex.push_back(T27_LIGHT_MSG);
    m_vecLightIndex.push_back(T27_LIGHT_HEADSET);
}

void CTestPhoneT27::InitCmpString()
{
    int iKeyIndex = 0;
    int i = 0;
    // 数字1~9
    for (i = 0; i < 9; i++)
    {
        m_strKey[iKeyIndex++] = commonAPI_FormatString("%d", i + 1);
    }
    m_strKey[iKeyIndex++] = "*";
    m_strKey[iKeyIndex++] = "0";
    m_strKey[iKeyIndex++] = "#";

    //LineKey
    for (i = 0; i < m_iLineNum; i++)
    {
        m_strKey[iKeyIndex++] = commonAPI_FormatString("LK%d", i + 1);
    }

    // 4个Hotkey
    for (i = 0; i < 4; i++)
    {
        m_strKey[iKeyIndex++] = commonAPI_FormatString("HK%d", i + 1);
    }

    //导航键
    m_strKey[iKeyIndex++] = "Up";
    m_strKey[iKeyIndex++] = "Dn";
    m_strKey[iKeyIndex++] = "Lf";
    m_strKey[iKeyIndex++] = "Rt";
    m_strKey[iKeyIndex++] = "OK";
    //其他功能键
    m_strKey[iKeyIndex++] = "Cancel";
    //音量键
    m_strKey[iKeyIndex++] = "Vol-";
    m_strKey[iKeyIndex++] = "Vol+";


    m_strKey[iKeyIndex++] = "Msg";
    m_strKey[iKeyIndex++] = "Headset";
    m_strKey[iKeyIndex++] = "Conf";
    m_strKey[iKeyIndex++] = "Hold";
    m_strKey[iKeyIndex++] = "Mute";
    m_strKey[iKeyIndex++] = "Tran";
    m_strKey[iKeyIndex++] = "RD";
    m_strKey[iKeyIndex++] = "HandFree";

    m_strKey[iKeyIndex++] = "AnyEXPKey";
}

bool CTestPhoneT27::ChangeEchoState(int iKey, ECHO_MODE & eMode)
{
    ECHO_MODE eOldMode = eMode;
    if (CTestPhone::ChangeEchoState(iKey, eMode))
    {
        if (eOldMode == EM_HEADSET
                && eMode == EM_HANDSET)
        {
            // 进入手柄回音测试，关闭耳麦
            LightOff(T27_LIGHT_HEADSET);
        }

        return TRUE;
    }

    return FALSE;
}

// 新需求led测试阶段专用
void CTestPhoneT27::TestAllLightOn(int mLineColor, bool bFuncKey/* = true*/)
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
        }
        break;
    case LS_RED:
        {
            for (int i = 0; i < m_iLineNum; i++)
            {
                TurnLightOn(m_vecLightIndex[m_iLineNum * mLineColor + i]);
            }

            for (int i = T66_FUNCTION_LIGHT_BEGIN; i < m_vecLightIndex.size(); i++)
            {
                // 点亮所有功能键
                TurnLightOn(m_vecLightIndex[i]);
            }
        }
        break;
    case LS_ORANGEONE:
        {
            for (int i = 0; i < m_iLineNum / 2; i++)
            {
                TurnLightOn(m_vecLightIndex[m_iLineNum * mLineColor + i * 2]);
            }
        }
        break;
    case LS_ORANGETWO:
        {
            for (int i = 0; i < m_iLineNum / 2; i++)
            {
                TurnLightOn(m_vecLightIndex[m_iLineNum * (mLineColor - 1) + i * 2 + 1]);
            }
        }
        break;
    default:
        break;
    }

    DoLight();
}

int CTestPhoneT27::GetNoteTextFont(int nTextType)
{
    switch (nTextType)
    {
    case TT_CONTENT_SECOND_HALF:
        return TEST_FONT_10;
        break;
    case TT_CONTENT_FULL_SCREEN:
        return TEST_FONT_12;
        break;
    default:
        break;
    }

    return CTestPhoneT66::GetNoteTextFont(nTextType);
}

