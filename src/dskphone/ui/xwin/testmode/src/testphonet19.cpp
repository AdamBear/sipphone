#include "testphonet19.h"
#include "test_include.h"

/////////////////////////////CTestPhoneT22/////////////////////////////////
CTestPhoneT19::CTestPhoneT19()
{
    m_uMaxKeyNum = 29;
    m_iLineNum = 0;
    m_bHasEXP = false;
    m_bHasLed = false;
}

CTestPhoneT19::~CTestPhoneT19()
{
}

void CTestPhoneT19::FlushLightIndex()
{
}

void CTestPhoneT19::InitCmpString()
{
    int iIndex = 0;
    for (int i = 0; i < 9; ++i)
    {
        m_strKey[iIndex++] = commonAPI_FormatString("%d", i + 1);
    }

    m_strKey[iIndex++] = "*";
    m_strKey[iIndex++] = "0";
    m_strKey[iIndex++] = "#";
    m_strKey[iIndex++] = "Vol-";
    m_strKey[iIndex++] = "Vol+";

    for (int j = 0; j < 4; ++j)
    {
        m_strKey[iIndex++] = commonAPI_FormatString("HK%d", j + 1);
    }

    m_strKey[iIndex++] = "Up";
    m_strKey[iIndex++] = "Dn";
    m_strKey[iIndex++] = "Lf";
    m_strKey[iIndex++] = "Rt";
    m_strKey[iIndex++] = "OK";
    m_strKey[iIndex++] = "Mute";
    m_strKey[iIndex++] = "Msg";
    m_strKey[iIndex++] = "Tran";
    m_strKey[iIndex++] = "Headset";
    m_strKey[iIndex++] = "RD";
    m_strKey[iIndex++] = "HandFree";
}

int CTestPhoneT19::GetNoteTextFont(int nTextType)
{
    return TEST_FONT_12;
}

bool CTestPhoneT19::ChangeEchoState(int iKey, ECHO_MODE & eMode)
{
    return CTestPhone::ChangeEchoState(iKey, eMode);
}
