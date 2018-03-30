#include "testphonet54.h"

#if IS_COLOR
#include "test_include.h"
#include "testmodeinterface.h"
#include "commonunits/modcommonunits.h"
#include "languagehelper/modlangtranslate.h"

#define TESEMODE_RESOURCE_PATH  RESOURCE_PATH"testmode/t4x/"

CTestPhoneT54::CTestPhoneT54()
{
    m_uMaxKeyNum = 0;
    m_iLineNum = 10;
}

CTestPhoneT54::~CTestPhoneT54()
{
}

// 测试半成品初始化
void CTestPhoneT54::TestSFPInitial()
{
    InitLight();
    ChangeState(new CTestStateColorLCD(this));
}

void CTestPhoneT54::FlushLightIndex()
{
    m_vecLightIndex.push_back(T5XS_LIGHT_LK1_GREEN);
    m_vecLightIndex.push_back(T5XS_LIGHT_LK2_GREEN);
    m_vecLightIndex.push_back(T5XS_LIGHT_LK3_GREEN);
    m_vecLightIndex.push_back(T5XS_LIGHT_LK4_GREEN);
    m_vecLightIndex.push_back(T5XS_LIGHT_LK5_GREEN);
    m_vecLightIndex.push_back(T5XS_LIGHT_LK6_GREEN);
    m_vecLightIndex.push_back(T5XS_LIGHT_LK7_GREEN);
    m_vecLightIndex.push_back(T5XS_LIGHT_LK8_GREEN);
    m_vecLightIndex.push_back(T5XS_LIGHT_LK9_GREEN);
    m_vecLightIndex.push_back(T5XS_LIGHT_LK10_GREEN);

    m_vecLightIndex.push_back(T5XS_LIGHT_LK1_RED);
    m_vecLightIndex.push_back(T5XS_LIGHT_LK2_RED);
    m_vecLightIndex.push_back(T5XS_LIGHT_LK3_RED);
    m_vecLightIndex.push_back(T5XS_LIGHT_LK4_RED);
    m_vecLightIndex.push_back(T5XS_LIGHT_LK5_RED);
    m_vecLightIndex.push_back(T5XS_LIGHT_LK6_RED);
    m_vecLightIndex.push_back(T5XS_LIGHT_LK7_RED);
    m_vecLightIndex.push_back(T5XS_LIGHT_LK8_RED);
    m_vecLightIndex.push_back(T5XS_LIGHT_LK9_RED);
    m_vecLightIndex.push_back(T5XS_LIGHT_LK10_RED);

    m_vecLightIndex.push_back(T5XS_LIGHT_LK1_GREEN | T5XS_LIGHT_LK1_RED);
    m_vecLightIndex.push_back(T5XS_LIGHT_LK2_GREEN | T5XS_LIGHT_LK2_RED);
    m_vecLightIndex.push_back(T5XS_LIGHT_LK3_GREEN | T5XS_LIGHT_LK3_RED);
    m_vecLightIndex.push_back(T5XS_LIGHT_LK4_GREEN | T5XS_LIGHT_LK4_RED);
    m_vecLightIndex.push_back(T5XS_LIGHT_LK5_GREEN | T5XS_LIGHT_LK5_RED);
    m_vecLightIndex.push_back(T5XS_LIGHT_LK6_GREEN | T5XS_LIGHT_LK6_RED);
    m_vecLightIndex.push_back(T5XS_LIGHT_LK7_GREEN | T5XS_LIGHT_LK7_RED);
    m_vecLightIndex.push_back(T5XS_LIGHT_LK8_GREEN | T5XS_LIGHT_LK8_RED);
    m_vecLightIndex.push_back(T5XS_LIGHT_LK9_GREEN | T5XS_LIGHT_LK9_RED);
    m_vecLightIndex.push_back(T5XS_LIGHT_LK10_GREEN | T5XS_LIGHT_LK10_RED);

    m_vecLightIndex.push_back(T5XS_LIGHT_HEADSET);
    m_vecLightIndex.push_back(T5XS_LIGHT_MUTE);
    m_vecLightIndex.push_back(T5XS_LIGHT_SPK);
}

void CTestPhoneT54::InitCmpString()
{
    int i = 0;
    for (i = 0; i < 9; i++)
    {
        m_strKey[m_uMaxKeyNum++] = commonAPI_FormatString("%d", i + 1);
    }
    m_strKey[m_uMaxKeyNum++] = "*";
    m_strKey[m_uMaxKeyNum++] = "0";
    m_strKey[m_uMaxKeyNum++] = "#";

    for (i = 0; i < m_iLineNum; i++)
    {
        m_strKey[m_uMaxKeyNum++] = commonAPI_FormatString("LK%d", i + 1);
    }

    for (i = 0; i < 4; i++)
    {
        m_strKey[m_uMaxKeyNum++] = commonAPI_FormatString("HK%d", i + 1);
    }

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
}

bool CTestPhoneT54::ChangeEchoState(int iKey, ECHO_MODE & eMode)
{
    ECHO_MODE eOldMode = eMode;
    if (CTestPhone::ChangeEchoState(iKey, eMode))
    {
        if (EM_HEADSET != eOldMode
                && EM_HEADSET == eMode)
        {
            // 进入耳麦回音测试，关闭免提灯
            LightOff(T5XS_LIGHT_SPK);
        }
        else if (EM_HEADSET == eOldMode
                 && EM_HANDSET == eMode)
        {
            // 进入手柄回音测试，关闭耳麦
            LightOff(T5XS_LIGHT_HEADSET);
        }

        return true;
    }

    return false;
}

// 画中间文字
void CTestPhoneT54::DrawContentString(LPCTSTR lpszTest, xColor color/* = xColor(0x00, 0x00, 0x00)*/)
{
    testmode_SetTextColor(TT_CONTENT_CENTER, color);
    testmode_DrawText(lpszTest, TT_CONTENT_CENTER, GetNoteTextFont(TT_CONTENT_CENTER));
}

void CTestPhoneT54::DrawRpsString(bool bRpsStatus, LPCTSTR lpszRandnum, LPCSTR lpszCert)
{
    yl::string strRpsStatus = "RPS Status: RPS ";
    strRpsStatus += bRpsStatus ? "Started" : "Closed";

    // 恢复出厂的状态
    yl::string strFactoryStatus = commUnits_GetUserEnvString(kszStatusReset);
    strFactoryStatus = strFactoryStatus.empty() ? "No Restored" : "Restored";
    strFactoryStatus = "Factory Status: " + strFactoryStatus;

    testmode_DrawText(lpszRandnum, TT_CONTENT_CENTER, GetNoteTextFont(TT_CONTENT_CENTER));

    yl::string strDisplay = lpszCert;
    strDisplay += "\n" + strFactoryStatus;
    strDisplay += "\n" + strRpsStatus;

    xColor colGreen = xColor(0x0, 0xff, 0x0);
    testmode_SetTextColor(TT_CONTENT_SECOND_HALF, colGreen);
    testmode_DrawText(strDisplay, TT_CONTENT_SECOND_HALF, GetNoteTextFont(TT_CONTENT_SECOND_HALF));
}

void CTestPhoneT54::DrawStatusViewString(LPCSTR lpszTest)
{
    testmode_DrawText(lpszTest, TT_CONTENT_CENTER, TEST_FONT_20);
}

void CTestPhoneT54::FillPicture(const yl::string& strPic)
{
    yl::string strFullPath = TESEMODE_RESOURCE_PATH + strPic;
    testmode_FillRect(strFullPath);
}

TEST_STATE CTestPhoneT54::MapStateOfKey(int iKey, bool bEXPKey /*= false*/)
{
    if (!bEXPKey && PHONE_KEY_HANDFREE == iKey)
    {
        return TS_COLOR;
    }

    return CTestPhoneT66::MapStateOfKey(iKey, bEXPKey);
}

void CTestPhoneT54::DrawKeyString(bool bCorrect, int iIndex)
{
    if (iIndex < 0 || iIndex >= (int)m_uMaxKeyNum)
    {
        // 全机型都显示字眼KeyBoard Test
        testmode_SetTextColor(TT_CONTENT_CENTER, xColor(0x00, 0x00, 0x00));
        testmode_DrawText(_LANG("Start to Test Key"), TT_CONTENT_CENTER, GetNoteTextFont(TT_CONTENT_CENTER));
    }
    else if (bCorrect)
    {
        testmode_DrawText(GetCorrectString(iIndex), TT_CONTENT_FULL_SCREEN, GetNoteTextFont(TT_CONTENT_FULL_SCREEN));
    }
    else
    {
        yl::string strDisp = commonAPI_FormatString("Press %s", m_strKey[iIndex].c_str());
        testmode_DrawText(strDisp, TT_CONTENT_CENTER, GetNoteTextFont(TT_CONTENT_CENTER));
    }
}

void CTestPhoneT54::TestAllLightOn(int mLineColor, bool bFuncKey/* = true*/)
{
    TurnLightAllOff(true);

    switch (mLineColor)
    {
    case LS_GREEN:
        {
            for (int i = 0; i < m_iLineNum; ++i)
            {
                TurnLightOn(m_vecLightIndex[i]);
            }

            TurnLightOn(m_vecLightIndex[T66_FUNCTION_LIGHT_BEGIN + 1]);
            TurnLightOn(m_vecLightIndex[T66_FUNCTION_LIGHT_BEGIN + 2]);
        };
        break;
    case LS_RED:
        {
            for (int i = 0; i < m_iLineNum; ++i)
            {
                TurnLightOn(m_vecLightIndex[m_iLineNum + i]);
            }

            TurnLightOn(m_vecLightIndex[T66_FUNCTION_LIGHT_BEGIN]);
        }
        break;
    case LS_ORANGEONE:
        {
            for (int i = 0; i < m_iLineNum / 2; ++i)
            {
                if (i % 2 != 0)
                {
                    continue;
                }

                // 对角亮
                TurnLightOn(m_vecLightIndex[m_iLineNum * 2 + i]);
                TurnLightOn(m_vecLightIndex[m_iLineNum * 2 + i + m_iLineNum / 2]);
            }
        }
        break;
    case LS_ORANGETWO:
        {
            for (int i = 0; i < m_iLineNum / 2; ++i)
            {
                if (i % 2 != 1)
                {
                    continue;
                }

                // 对角亮
                TurnLightOn(m_vecLightIndex[m_iLineNum * 2 + i]);
                TurnLightOn(m_vecLightIndex[m_iLineNum * 2 + i + m_iLineNum / 2]);
            }

            for (int i = T66_FUNCTION_LIGHT_BEGIN; i < m_vecLightIndex.size(); ++i)
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

int CTestPhoneT54::GetNoteTextFont(int nTextType)
{
    switch (nTextType)
    {
    case TT_CONTENT_CENTER:
        return TEST_FONT_50;
        break;
    case TT_CONTENT_SECOND_HALF:
        return TEST_FONT_20;
        break;
    case TT_CONTENT_FULL_SCREEN:
        return TEST_FONT_30;
        break;
    default:
        break;
    }

    return CTestPhoneT66::GetNoteTextFont(nTextType);
}

#endif //IS_COLOR
