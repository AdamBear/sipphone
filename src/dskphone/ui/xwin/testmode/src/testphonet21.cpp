#include "testphonet21.h"
#include "test_include.h"

/////////////////////////////CTestPhoneT22/////////////////////////////////
CTestPhoneT21::CTestPhoneT21()
{
    m_uMaxKeyNum = 31;
    m_iLineNum = 2;
    m_bHasEXP = false;
}

CTestPhoneT21::~CTestPhoneT21()
{
}

void CTestPhoneT21::FlushLightIndex()
{
    m_vecLightIndex.push_back(T23_LIGHT_LK1_GREEN);
    m_vecLightIndex.push_back(T23_LIGHT_LK3_GREEN);
    m_vecLightIndex.push_back(T23_LIGHT_LK1_RED);
    m_vecLightIndex.push_back(T23_LIGHT_LK3_RED);
    m_vecLightIndex.push_back(T23_LIGHT_LK1_GREEN | T23_LIGHT_LK1_RED);  //橙色1
    m_vecLightIndex.push_back(T23_LIGHT_LK3_GREEN | T23_LIGHT_LK3_RED);  //橙色3
    m_vecLightIndex.push_back(T23_LIGHT_MSG);
}

void CTestPhoneT21::InitCmpString()
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

    m_strKey[iIndex++] = "Up";
    m_strKey[iIndex++] = "Dn";
    m_strKey[iIndex++] = "Lf";
    m_strKey[iIndex++] = "Rt";
    m_strKey[iIndex++] = "OK";
    m_strKey[iIndex++] = "Cancel";
    m_strKey[iIndex++] = "Vol-";
    m_strKey[iIndex++] = "Vol+";

    for (int k = 0; k < 2; ++k)
    {
        m_strKey[iIndex++] = commonAPI_FormatString("LK%d", k + 1);
    }

    m_strKey[iIndex++] = "Msg";
    m_strKey[iIndex++] = "Headset";
    m_strKey[iIndex++] = "Tran";
    m_strKey[iIndex++] = "RD";
    m_strKey[iIndex++] = "HandFree";
}

int CTestPhoneT21::GetNoteTextFont(int nTextType)
{
    return TEST_FONT_10;
}

bool CTestPhoneT21::ChangeEchoState(int iKey, ECHO_MODE & eMode)
{
    return CTestPhone::ChangeEchoState(iKey, eMode);
}
