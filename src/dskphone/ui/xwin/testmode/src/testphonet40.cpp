#include "testphonet40.h"
#include "test_include.h"

/////////////////////////////CTestPhoneT40/////////////////////////////////
CTestPhoneT40::CTestPhoneT40()
{
    m_uMaxKeyNum = 32;
    m_iLineNum = 3;
    m_bHasEXP = false;
    m_bSkipEHSTest = FALSE;
}

CTestPhoneT40::~CTestPhoneT40()
{
}

void CTestPhoneT40::FlushLightIndex()
{
    m_vecLightIndex.push_back(T40_LIGHT_LK1_GREEN);
    m_vecLightIndex.push_back(T40_LIGHT_LK2_GREEN);
    m_vecLightIndex.push_back(T40_LIGHT_LK3_GREEN);

    m_vecLightIndex.push_back(T40_LIGHT_LK1_RED);
    m_vecLightIndex.push_back(T40_LIGHT_LK2_RED);
    m_vecLightIndex.push_back(T40_LIGHT_LK3_RED);

    m_vecLightIndex.push_back(T40_LIGHT_LK1_GREEN | T40_LIGHT_LK1_RED);
    m_vecLightIndex.push_back(T40_LIGHT_LK2_GREEN | T40_LIGHT_LK2_RED);
    m_vecLightIndex.push_back(T40_LIGHT_LK3_GREEN | T40_LIGHT_LK3_RED);
}

void CTestPhoneT40::InitCmpString()
{
    int iIndex = 0;
    for (int i = 0; i < 9; ++i)
    {
        m_strKey[iIndex++] = commonAPI_FormatString("%d", i + 1);
    }

    m_strKey[iIndex++] = "*";
    m_strKey[iIndex++] = "0";
    m_strKey[iIndex++] = "#";

    for (int j = 0; j < 4; ++j)
    {
        m_strKey[iIndex++] = commonAPI_FormatString("HK%d", j + 1);
    }

    for (int k = 0; k < 3; ++k)
    {
        m_strKey[iIndex++] = commonAPI_FormatString("LK%d", k + 1);
    }

    m_strKey[iIndex++] = "Up";
    m_strKey[iIndex++] = "Dn";
    m_strKey[iIndex++] = "Lf";
    m_strKey[iIndex++] = "Rt";
    m_strKey[iIndex++] = "OK";
    m_strKey[iIndex++] = "Cancel";
    m_strKey[iIndex++] = "Vol-";
    m_strKey[iIndex++] = "Vol+";

    m_strKey[iIndex++] = "Mute";
    m_strKey[iIndex++] = "Headset";
    m_strKey[iIndex++] = "Msg";
    m_strKey[iIndex++] = "RD";
    m_strKey[iIndex++] = "HandFree";
}

int CTestPhoneT40::GetNoteTextFont(int nTextType)
{
    return TEST_FONT_10;
}

